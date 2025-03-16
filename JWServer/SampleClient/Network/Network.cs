using Google.Protobuf;
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
using System.Windows.Media;
using System.Windows.Threading;

namespace SampleClient.Network
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
                        }
                    }
                }
                return instance;
            }
        }

        private Dispatcher? _mainDispacher = null;

        public EventQueue EventQuene
        {
            get
            {
                return _eventQuene;
            }
        }

        private Session _session;
        private EventQueue _eventQuene;
        private Thread _thread;
        private CancellationTokenSource _cts;
        private PacketHandler _packetHandler;
        private PacketSender _packetSender;
        public LoginInfo LoginInfo { get; set; }

        public Network()
        {
            _session = new Session();
            _eventQuene = new EventQueue("NetworkEvent");
            _cts = new CancellationTokenSource();
            _thread = new Thread(() => run(_cts.Token));
            _packetHandler = new PacketHandler();
            _packetSender = new PacketSender();
        }

        public void Initialize()
        {
            _session.Initialize();
            _thread.Start();
            _packetHandler.Initialize();
            _packetSender.Initialize();

        }

        public void Close()
        {
            _session.Dispose();
            _eventQuene.Stop();
            _cts.Cancel();
            _thread.Join();
        }

        public void SetDispatcher(Dispatcher mainDispacher)
        {
            _mainDispacher = mainDispacher;
            _packetHandler.SetDispatcher(mainDispacher);
        }

        private void run(CancellationToken token)
        {
            _eventQuene.Process(token);
        }

        public void SessionConnect(string ip, int port, Action? onConnectAction)
        {
            if (_session.IsConnected())
                return;

            try
            {
                _eventQuene.Add(AsyncEventFactory.Create(async (sessionConnectArg, onConnectAction, dispatcher) =>
                {
                    await _session.AsyncConnect(sessionConnectArg);
                    await dispatcher.BeginInvoke(DispatcherPriority.Background, () => { onConnectAction?.Invoke(); });
                    _session.OnRecved += HandleMessage;
                    await _session.AsyncRecv();
                }, "asyncConnect", new SessionConnectArg(ip, port), onConnectAction, Dispatcher.CurrentDispatcher));

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public void AsyncSend(byte[] sendBytes, Action? onSendAction = null)
        {
            if (!_session.IsConnected())
                return;

            try
            {
                _eventQuene.Add(AsyncEventFactory.Create(async (sessionSendArg, onSendAction, dispatcher) =>
                {
                    await _session.AsyncSend(sendBytes);
                    if (null != dispatcher)
                        await dispatcher.BeginInvoke(DispatcherPriority.Background, () => { onSendAction?.Invoke(); });
                }, "asyncSend", new SessionSendArg(sendBytes), onSendAction, _mainDispacher));

            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

        }

        public static ushort GetDefaultPacketSize()
        {
            return sizeof(ushort) + sizeof(uint);
        }

        async public void AsyncRead()
        {
            if (!_session.IsConnected())
                return;

            await _session.AsyncRecv();
        }

        private void HandleMessage(object? sender, SessionRecvEventArg e)
        {
            var dataBytes = e.Data;
            var dataSize = e.DataSize;
            while (GetDefaultPacketSize() < dataSize)
            {
                int index = 0;
                ushort packetTotalSize = BitConverter.ToUInt16(dataBytes, index); index += sizeof(UInt16);

                if (packetTotalSize < 0)
                    break;

                int cmd = BitConverter.ToInt32(dataBytes, index); index += sizeof(Int32);
                var packetBodySize = packetTotalSize - index;

                _packetHandler.HandlePacket(cmd, _session, dataBytes.Skip(index).ToArray(), packetBodySize);

                e.UseSize += packetTotalSize;
                dataSize -= packetTotalSize;
                dataBytes = e.Data.Skip(e.UseSize).ToArray();
            }
        }

        public PacketSender? GetPacketSender()
        {
            return _packetSender;
        }


    }
}
