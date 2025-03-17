using Google.Protobuf;
using Jw;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient.Network
{
    class PacketSender
    {
        public void Initialize()
        {
        }


        public byte[] makeSendBuffer(uint cmd, int reqSize, byte[] reqBytes)
        {
            ushort defaultSize = Network.GetDefaultPacketSize();
            ushort size = (ushort)(defaultSize + reqSize);
            byte[] sendBuffer = new byte[size];
            int bufferIndex = 0;
            Array.Copy(BitConverter.GetBytes(size), 0, sendBuffer, bufferIndex, sizeof(ushort)); bufferIndex += sizeof(ushort);
            Array.Copy(BitConverter.GetBytes(cmd), 0, sendBuffer, bufferIndex, sizeof(uint)); bufferIndex += sizeof(uint);
            if (0 < reqSize)
                Array.Copy(reqBytes, 0, sendBuffer, bufferIndex, reqSize); bufferIndex += reqSize;

            return sendBuffer;
        }

        public byte[] PacketToSendBuffer<T>(T packet, uint cmd) where T : IMessage<T>
        {
            byte[] reqBytes = packet.ToByteArray();
            int reqBytesLength = reqBytes.Length;
            return makeSendBuffer(cmd, reqBytesLength, reqBytes);

        }

        public void SendPingReq()
        {
            byte[] sendBuffer = makeSendBuffer((uint)GamePacketCmd.PingReq, 0, []);
            Network.Instance.AsyncSend(sendBuffer);
        }

        public void SendLoginReq(string name)
        {
            GameLoginReq loginReq = new GameLoginReq
            {
                Name = name,
                PacketVersion = (int)GamePacketInfo.Version,
                AuthKey = (int)GamePacketInfo.GamePacketAuthKey,
            };

            byte[] sendBuffer = PacketToSendBuffer(loginReq, (uint)GamePacketCmd.LoginReq);
            Network.Instance.AsyncSend(sendBuffer);
        }

        public void SendCreateRoom(string roomName)
        {
            GameCreateRoomReq createRoomReq = new GameCreateRoomReq
            {
                Name = roomName
            };

            byte[] sendBuffer = PacketToSendBuffer(createRoomReq, (uint)GamePacketCmd.CreateRoomReq);
            Network.Instance.AsyncSend(sendBuffer);
        }

        public void SendRoomList()
        {
            GameRoomListReq roomListReq = new GameRoomListReq
            {
                StartSeq = 0,
                EndSeq = 10
            };
            byte[] sendBuffer = PacketToSendBuffer(roomListReq, (uint)GamePacketCmd.RoomListReq);
            Network.Instance.AsyncSend(sendBuffer);
        }

        public void SendChatReq(string name, string msg)
        {
            GameChatReq chatReq = new GameChatReq
            {
                Name = name,
                Msg = msg
            };

            byte[] sendBuffer = PacketToSendBuffer(chatReq, (uint)GamePacketCmd.ChatReq);
            Network.Instance.AsyncSend(sendBuffer);
        }

        public void SendEnterRoom(long roomId)
        {
            GameRoomEnterReq enterRoomReq = new GameRoomEnterReq
            {
                RoomId = roomId,
            };
            byte[] sendBuffer = PacketToSendBuffer(enterRoomReq, (uint)GamePacketCmd.RoomEnterReq);
            Network.Instance.AsyncSend(sendBuffer);
        }
    }
}
