using Jw;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Security.RightsManagement;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace SampleClient
{
    class Network
    {
        public static readonly object NetworkLock = new object();
        private static Network? instance = null;
        public static Network Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (NetworkLock)
                    {
                        if (instance == null)
                        {
                            instance = new Network();
                            instance.Initialize();
                        }
                    }
                }
                return instance;
            }
        }

        private Session session;

        public void Initialize()
        {
            session = new Session();
            session.Initialize();
        }

        async public Task SessionConnect(string ip, int port, Action onConnectAction)
        {
            if (session.IsConnected())
                return;

            try
            {
                session.OnConnected += (s, e) => onConnectAction();
                await session.AsyncConnect(ip, port);
                session.OnConnected -= (s, e) => onConnectAction();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        async public Task AsyncSend(byte[] sendBytes)
        {
            if (!session.IsConnected())
                return;

            try
            {
                await session.AsyncSend(sendBytes);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

        }

        public void Send(byte[] sendBytes)
        {
            if (!session.IsConnected())
                return;

            session.Send(sendBytes);

        }

        async public void AsyncRead()
        {
            if (!session.IsConnected())
                return;

            await session.AsyncRecv();
        }

        private void HandleMessage(byte[] readBytes)
        {

        }

        public static ushort GetDefaultPacketSize()
        {
            return sizeof(ushort) + sizeof(uint);
        }

        public void SendPingReq()
        {
            ushort size = GetDefaultPacketSize();
            uint cmd = (uint)GamePacketCmd.PingReq;
            byte[] sendBuffer = new byte[size];

            int bufferIndex = 0;
            Array.Copy(BitConverter.GetBytes(size), 0, sendBuffer, bufferIndex, sizeof(ushort)); bufferIndex += sizeof(ushort);
            Array.Copy(BitConverter.GetBytes(cmd), 0, sendBuffer, bufferIndex, sizeof(uint)); bufferIndex += sizeof(uint);

            Network.Instance.Send(sendBuffer);
        }

        async public Task AsyncSendPingReq()
        {
            ushort size = GetDefaultPacketSize();
            uint cmd = (uint)GamePacketCmd.PingReq;
            byte[] sendBuffer = new byte[size];

            int bufferIndex = 0;
            Array.Copy(BitConverter.GetBytes(size), 0, sendBuffer, bufferIndex, sizeof(ushort)); bufferIndex += sizeof(ushort);
            Array.Copy(BitConverter.GetBytes(cmd), 0, sendBuffer, bufferIndex, sizeof(uint)); bufferIndex += sizeof(uint);

            await Network.Instance.AsyncSend(sendBuffer);
        }

    }
}
