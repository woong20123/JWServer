using Google.Protobuf;
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


        private void handleGameLoginOk(Session session, byte[] packetData, int packetBodySize)
        {
            var loginOk = ToPacket<GameLoginOk>(packetData, 0, packetBodySize);

            var callBackAction = () =>
            {

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
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameCreateRoomFail(Session session, byte[] packetData, int packetBodySize)
        {
            var createRoomFail = ToPacket<GameCreateRoomFail>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }

        private void handleGameRoomListOk(Session session, byte[] packetData, int packetBodySize)
        {
            var roomListOk = ToPacket<GameRoomListOk>(packetData, 0, packetBodySize);
            var callBackAction = () =>
            {
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
                var mainWindow = GetMainWindow();
                var chatPage = mainWindow?.GetChatPage();
                chatPage?.ViewText(chatOk.Name, chatOk.Msg);
            };

            // UI 로직이여서 메인 스레드에서 처리
            _mainDispatcher?.BeginInvoke(DispatcherPriority.Background, callBackAction);
        }
    }
}
