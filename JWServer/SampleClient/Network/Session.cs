using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace SampleClient.Network
{

    class SessionRecvEventArg
    {
        public SessionRecvEventArg(byte[] data, int dataSize)
        {
            Data = data;
            DataSize = dataSize;
            UseSize = 0;
        }
        public readonly byte[] Data;
        public readonly int DataSize;
        public int UseSize { get; set; }
    }

    class SessionConnectArg
    {
        public SessionConnectArg(string ip, int port)
        {
            IP = ip;
            Port = port;
        }
        public readonly string IP;
        public readonly int Port;
    }

    class SessionSendArg
    {
        public SessionSendArg(byte[] data)
        {
            Data = data;
        }
        public readonly byte[] Data;
    }

    class Session : IDisposable
    {
        public event EventHandler<EventArgs>? OnInitialize;
        public event EventHandler<EventArgs>? OnConnected;
        public event EventHandler<SessionRecvEventArg>? OnRecved;
        public event EventHandler<EventArgs>? OnSent;

        public readonly object NetworkStreamLock = new object();

        private TcpClient client = new TcpClient();
        private NetworkStream? ns = null;
        private Thread? workerThread = null;
        private ConcurrentQueue<EventArgs> _eventArgQueue = new ConcurrentQueue<EventArgs>();
        private byte[] _recvBuffer = new byte[8192];

        private bool _disposed = false;

        public NetworkStream NetworkStream
        {
            get
            {
                if (null == ns)
                {
                    if (!client.Connected)
                        return ns!;

                    lock (NetworkStreamLock)
                    {
                        if (null == ns)
                        {
                            ns = client.GetStream();
                        }
                    }
                }

                return ns;
            }
        }

        ~Session() => Dispose(false);


        public void Initialize()
        {
            OnInitialize?.Invoke(this, new EventArgs());
            workerThread = new Thread(worker);
            workerThread.Start();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public void worker()
        {

        }

        protected virtual void Dispose(bool disposing)
        {
            if (_disposed)
            {
                return;
            }

            if (disposing)
            {
                ns!.Close();
                client.Close();
                // TODO: dispose managed state (managed objects).
            }

            // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
            // TODO: set large fields to null.

            _disposed = true;
        }

        public bool IsConnected()
        {
            if (client == null) return false;
            if (!client.Connected) return false;
            return true;
        }

        async public Task AsyncConnect(SessionConnectArg arg)
        {
            await client.ConnectAsync(arg.IP, arg.Port).ConfigureAwait(false);

            Dispatcher.CurrentDispatcher.Invoke(DispatcherPriority.Background, new Action(() =>
            {
                OnConnected?.Invoke(this, new EventArgs());
            }));
        }

        async public Task AsyncSend(byte[] sendBuffer)
        {
            await NetworkStream.WriteAsync(sendBuffer, 0, sendBuffer.Length).ConfigureAwait(false);

            if (OnSent != null)
                OnSent?.Invoke(this, new EventArgs());
        }


        async public Task AsyncRecv()
        {
            NetworkStream ns = NetworkStream;
            int recvSize = 0;

            try
            {
                while (client.Connected)
                {
                    int remainRecvBufferSize = _recvBuffer.Length - recvSize;
                    int bytesRead = await ns.ReadAsync(_recvBuffer, recvSize, remainRecvBufferSize).ConfigureAwait(false);
                    
                    recvSize += bytesRead;
                    if (!ns.DataAvailable)
                    {
                        if (OnRecved != null)
                        {
                            SessionRecvEventArg arg = new SessionRecvEventArg(_recvBuffer, recvSize);
                            OnRecved?.Invoke(this, arg);

                            if (recvSize < arg.UseSize)
                                new Exception($"recvSize is low then zero {recvSize}");

                            // 사용한 버퍼 초기화 
                            Array.Clear(_recvBuffer, 0, arg.UseSize);
                            // 남아 있는 버퍼 복사
                            int remainRecvSize = recvSize - arg.UseSize;
                            if (remainRecvSize > 0)
                                Array.Copy(_recvBuffer.Skip(arg.UseSize).ToArray(), _recvBuffer, remainRecvSize);
                            recvSize -= arg.UseSize;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }
    }

}
