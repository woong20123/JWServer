using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient
{

    struct SessionRecvEventArg
    {
        public byte[] Data;
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
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
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

        async public Task AsyncConnect(string ip, int port)
        {
            await client.ConnectAsync(ip, port);
            OnConnected?.Invoke(this, new EventArgs());
        }

        async public Task AsyncSend(byte[] sendBuffer)
        {
            await NetworkStream.WriteAsync(sendBuffer, 0, sendBuffer.Length);

            if (OnSent != null)
                OnSent(this, new EventArgs());
        }

        public void Send(byte[] sendBuffer)
        {
            NetworkStream.Write(sendBuffer, 0, sendBuffer.Length);

            if (OnSent != null)
                OnSent(this, new EventArgs());
        }


        async public Task AsyncRecv()
        {
            NetworkStream ns = NetworkStream;
            using MemoryStream ms = new MemoryStream();
            byte[] buffer = new byte[4096];

            while (client.Connected)
            {
                int bytesRead = await ns.ReadAsync(buffer, 0, buffer.Length);
                ms.Write(buffer, 0, bytesRead);
                if (!ns.DataAvailable)
                {
                    ms.Seek(0, SeekOrigin.Begin);
                    if (OnRecved != null)
                    {
                        SessionRecvEventArg arg = new SessionRecvEventArg();
                        arg.Data = ms.ToArray();
                        OnRecved?.Invoke(this, arg);
                    }
                }
            }
        }
    }

}
