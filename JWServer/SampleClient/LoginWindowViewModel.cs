using Jw;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace SampleClient
{
    class LoginWindowViewModel : BindableBase
    {
        public ICommand LoginCommand => new DelegateCommand(Login);
        public ICommand ExitCommand => new DelegateCommand(Exit);
        public event EventHandler? OnRequestWindowClose;

        private string name = "woong20123";
        private string addressInfo = "127.0.0.1:13211";

        public string Name
        {
            get => name;
            set => SetProperty(ref name, value);
        }
        public string AddressInfo
        {
            get => addressInfo;
            set => SetProperty(ref addressInfo, value);
        }

        async private void Login()
        {
            string ip = String.Empty;
            int port = 0;

            if (String.IsNullOrEmpty(AddressInfo) || !AddressInfo.Contains(':'))
            {
                MessageBox.Show($"주소 정보 {AddressInfo} 가 이상합니다.", "Information");
                return;
            }

            var addressInfos = AddressInfo.Split(':');
            ip = addressInfos[0];
            port = int.Parse(addressInfos[1]);

            await Network.Instance.SessionConnect(ip, port,
                () =>
                {
                    MainWindow mainWindow = new MainWindow(new LoginInfo("woong", ip, port));
                    mainWindow.Show();

                    this.OnRequestWindowClose!(this, new EventArgs());
                    Task.Run(() =>
                    {
                        MessageBox.Show("서버 연결에 성공하였습니다.", "Information");
                    });
                });
        }

        private void Exit()
        {
            Environment.Exit(0);
            System.Diagnostics.Process.GetCurrentProcess().Kill();
            this.OnRequestWindowClose!(this, new EventArgs());
        }
    }
}
