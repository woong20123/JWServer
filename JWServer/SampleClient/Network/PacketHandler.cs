﻿using Google.Protobuf;
using Jw;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.RightsManagement;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Windows.Threading;
using System.Xml.Linq;

namespace SampleClient.Network
{
    internal class PacketHandler
    {
        public Dictionary<int, Action<Session, byte[], int>> packetHandler;
        private Dispatcher? _mainDispatcher;

        public static T ToPacket<T>(byte[] packetData, int index, int packetBodySize) where T : IMessage<T>, new()
        {
            T packet = new T();
            packet.MergeFrom(packetData, index, packetBodySize);
            return packet;
        }

        public PacketHandler()
        {
            packetHandler = new Dictionary<int, Action<Session, byte[], int>>();
            _mainDispatcher = null;
        }

        public void Initialize()
        {
            packetHandler.Add((int)GamePacketCmd.LoginOk, handleGameLoginOk);
            packetHandler.Add((int)GamePacketCmd.LoginFail, handleGameLoginFail);
            packetHandler.Add((int)GamePacketCmd.CreateRoomOk, handleGameCreateRoomOk);
            packetHandler.Add((int)GamePacketCmd.CreateRoomFail, handleGameCreateRoomFail);
            packetHandler.Add((int)GamePacketCmd.RoomListOk, handleGameRoomListOk);
            packetHandler.Add((int)GamePacketCmd.RoomListFail, handleGameRoomListFail);
            packetHandler.Add((int)GamePacketCmd.ChatOk, handleGameChatOk);
            packetHandler.Add((int)GamePacketCmd.RoomChatOk, handleGameRoomChatOk);
            packetHandler.Add((int)GamePacketCmd.RoomEnterOk, handleGameRoomEnterOk);
            packetHandler.Add((int)GamePacketCmd.RoomEnterNotify, handleGameRoomEnterNotify);
            packetHandler.Add((int)GamePacketCmd.RoomEnterFail, handleGameRoomEnterFail);
            packetHandler.Add((int)GamePacketCmd.RoomLeaveOk, handleGameRoomLeaveOk);
            packetHandler.Add((int)GamePacketCmd.RoomLeaveFail, handleGameRoomLeaveFail);
            packetHandler.Add((int)GamePacketCmd.RoomLeaveNotify, handleGameRoomLeaveNotify);
            packetHandler.Add((int)GamePacketCmd.CreateRoomNotify, handleCreateRoomNotify);
            packetHandler.Add((int)GamePacketCmd.DestroyRoomNotify, handleDestroyRoomNotify);
        }

        public void SetDispatcher(Dispatcher mainDispacher)
        {
            _mainDispatcher = mainDispacher;
        }

        public void HandlePacket(int cmd, Session session, byte[] packetData, int packetBodySize)
        {
            if (packetHandler.TryGetValue(cmd, out var handler))
            {
                handler?.Invoke(session, packetData, packetBodySize);
            }

        }

        private MainWindow? GetMainWindow()
        {
            return Application.Current.Windows.OfType<Window>().Where(window => window is MainWindow).FirstOrDefault() as MainWindow;
        }

        private CreateRoomWindow? GetCreateRoomWindow()
        {
            return Application.Current.Windows.OfType<Window>().Where(window => window is CreateRoomWindow).FirstOrDefault() as CreateRoomWindow;
        }

        private Chat? GetChatWindow(long roomId)
        {
            return Application.Current.Windows.OfType<Window>().Where((Window window) =>
            {
                if (window is Chat chat)
                {
                    return chat.GetRoomId() == roomId;
                }
                return false;
            }).FirstOrDefault() as Chat;
        }

        private void handleGameLoginOk(Session session, byte[] packetData, int packetBodySize)
        {
            var loginOk = ToPacket<GameLoginOk>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                Network.IsLogin = true;
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);

            Log.Information($"[PacketHandler] handleGameLoginOk name:{loginOk.Name}, userID:{loginOk.UserId}");
        }

        private void handleGameLoginFail(Session session, byte[] packetData, int packetBodySize)
        {
            var loginFail = ToPacket<GameLoginFail>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                MessageBox.Show($"Login Fail. ErrorCode:{loginFail.ErrCode}");
                var mainWindow = GetMainWindow();
                Environment.Exit(0);
                System.Diagnostics.Process.GetCurrentProcess().Kill();
                mainWindow?.Close();
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);

            Log.Information($"[PacketHandler] handleGameLoginFail errorCode:{loginFail.ErrCode}");
        }

        private void handleGameCreateRoomOk(Session session, byte[] packetData, int packetBodySize)
        {
            var createRoomOk = ToPacket<GameCreateRoomOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
                // CreateRoomWindow 닫기
                var createRoomWindow = GetCreateRoomWindow();
                createRoomWindow?.Close();

                // Room 관련 윈도우 띄우기
                Network.Instance.GetPacketSender()?.SendRoomList();

                var Room = new Model.Room { Name = createRoomOk.RoomInfo.Name, Id = createRoomOk.RoomInfo.RoomId, HostId = createRoomOk.RoomInfo.HostUserId, HostName = createRoomOk.RoomInfo.HostUserName };

                Chat chatWindow = new Chat();
                var cvm = chatWindow.GetChatViewModel();
                cvm.RoomId = Room.Id;
                cvm.ClearMemberName();
                cvm.AddMemberName(new Model.MemberInfo { Id = createRoomOk.RoomInfo.HostUserId, Name = createRoomOk.RoomInfo.HostUserName });
                chatWindow.Show();

            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameCreateRoomFail(Session session, byte[] packetData, int packetBodySize)
        {
            var createRoomFail = ToPacket<GameCreateRoomFail>(packetData, 0, packetBodySize);

            string errMsg = "Unknown Error";
            switch (createRoomFail.ErrCode)
            {
                case ErrorCode.CreateRoomFail:
                    errMsg = "입장하려는 방을 찾을 수 없습니다.";
                    break;
            }

            var callBackAction = () =>
            {
                MessageBox.Show($"방 생성 실패. Error : {errMsg}");
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomListOk(Session session, byte[] packetData, int packetBodySize)
        {
            var roomListOk = ToPacket<GameRoomListOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
                var mainWindow = GetMainWindow();
                mainWindow?.RoomList?.UpdateRoomList(roomListOk.RoomInfo.Select(r => new Model.Room { Name = r.Name, Id = r.RoomId, HostId = r.HostUserId, HostName = r.HostUserName }).ToList());
            };

            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomListFail(Session session, byte[] packetData, int packetBodySize)
        {
            var roomListFail = ToPacket<GameRoomListOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
            };
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameChatOk(Session session, byte[] packetData, int packetBodySize)
        {
            var chatOk = ToPacket<GameChatOk>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                //var mainWindow = GetMainWindow();
                //var chat = mainWindow?.GetChat();

                //chat?.ViewText(chatOk.Name, chatOk.Msg);
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomChatOk(Session session, byte[] packetData, int packetBodySize)
        {
            var roomChatOk = ToPacket<GameRoomChatOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
                var chatWindow = GetChatWindow(roomChatOk.RoomId);
                chatWindow?.GetChatViewModel().SetViewText(roomChatOk.Name, roomChatOk.Msg);
            };
            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomEnterOk(Session session, byte[] packetData, int packetBodySize)
        {
            var roomEnterOk = ToPacket<GameRoomEnterOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
                // CreateRoomWindow 닫기
                var createRoomWindow = GetCreateRoomWindow();
                createRoomWindow?.Close();

                // Room 관련 윈도우 띄우기
                Network.Instance.GetPacketSender()?.SendRoomList();

                var Room = new Model.Room { Name = roomEnterOk.RoomInfo.Name, Id = roomEnterOk.RoomInfo.RoomId, HostId = roomEnterOk.RoomInfo.HostUserId, HostName = roomEnterOk.RoomInfo.HostUserName };

                Chat chatWindow = new Chat();

                var cvm = chatWindow.GetChatViewModel();
                cvm.RoomId = Room.Id;
                cvm.ClearMemberName();
                roomEnterOk.MemberUserInfos.ToList().ForEach(m => cvm.AddMemberName(new Model.MemberInfo { Name = m.UserName, Id = m.UserId }));

                chatWindow.Show();

            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomEnterNotify(Session session, byte[] packetData, int packetBodySize)
        {
            var roomEnterNotify = ToPacket<GameRoomEnterNotify>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                // CreateRoomWindow 닫기
                var chatWindow = GetChatWindow(roomEnterNotify.RoomId);
                var userInfo = roomEnterNotify?.EnterUserInfo;
                if (chatWindow != null && userInfo != null)
                {
                    chatWindow.GetChatViewModel().SetViewText("System", string.Format($"{userInfo.UserName}님이 입장하셨습니다."));
                    chatWindow.GetChatViewModel().AddMemberName(new Model.MemberInfo { Name = userInfo.UserName, Id = userInfo.UserId });
                }
            };
            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomEnterFail(Session session, byte[] packetData, int packetBodySize)
        {
            var createRoomFail = ToPacket<GameCreateRoomFail>(packetData, 0, packetBodySize);

            string errMsg = "Unknown Error";
            switch (createRoomFail.ErrCode)
            {
                case ErrorCode.RoomNotFindRoom:
                    errMsg = "입장하려는 방을 찾을 수 없습니다.";
                    break;
                case ErrorCode.RoomExistUser:
                    errMsg = "이미 입장한 방입니다.";
                    break;
            }

            var callBackAction = () =>
            {
                MessageBox.Show($"방 입장 실패. Error : {errMsg}");
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomLeaveOk(Session session, byte[] packetData, int packetBodySize)
        {
            var roomLeaveOk = ToPacket<GameRoomLeaveOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
                GetChatWindow(roomLeaveOk.RoomId)?.Close();
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomLeaveNotify(Session session, byte[] packetData, int packetBodySize)
        {
            var roomLeaveNotify = ToPacket<GameRoomLeaveNotify>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                var ChatWindow = GetChatWindow(roomLeaveNotify.RoomId);
                var leaveUserInfo = roomLeaveNotify.LeaveUserInfo;
                ChatWindow?.GetChatViewModel().SetViewText("System",string.Format($"{leaveUserInfo.UserName}님이 퇴장 하셨습니다."));
                ChatWindow?.GetChatViewModel().RemoveMemberName(new Model.MemberInfo { Name = leaveUserInfo.UserName, Id = leaveUserInfo.UserId });
            };
            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomLeaveFail(Session session, byte[] packetData, int packetBodySize)
        {
            var createRoomFail = ToPacket<GameCreateRoomFail>(packetData, 0, packetBodySize);

            string errMsg = "Unknown Error";
            switch (createRoomFail.ErrCode)
            {
                case ErrorCode.RoomNotFindRoom:
                    errMsg = "방을 찾을 수 없습니다.";
                    break;
                case ErrorCode.RoomNotExistUser:
                    errMsg = "방에서 유저 정보를 찾을 수 없습니다.";
                    break;
            }

            var callBackAction = () =>
            {
                MessageBox.Show($"방 입장 실패. Error : {errMsg}");
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleCreateRoomNotify(Session session, byte[] packetData, int packetBodySize)
        {
            var destoroyRoomNotify = ToPacket<GameCreateRoomNotify>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                var r = destoroyRoomNotify.RoomInfo;
                var mainWindow = GetMainWindow();
                mainWindow?.RoomList?.getRoomListViewModel().AddRoom(new Model.Room { Name = r.Name, Id = r.RoomId, HostId = r.HostUserId, HostName = r.HostUserName });

            };
            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleDestroyRoomNotify(Session session, byte[] packetData, int packetBodySize)
        {
            var destoroyRoomNotify = ToPacket<GameDestroyRoomNotify>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {
                var mainWindow = GetMainWindow();
                mainWindow?.RoomList?.getRoomListViewModel().DeleteRoom(destoroyRoomNotify.RoomId);

            };
            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }




    }
}
