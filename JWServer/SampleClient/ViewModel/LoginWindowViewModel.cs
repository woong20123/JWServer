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
using System.Windows.Threading;
using Serilog;

namespace SampleClient.ViewModel
{
    class LoginWindowViewModel : BindableBase
    {
        public ICommand LoginCommand => new DelegateCommand(Login);
        public ICommand ExitCommand => new DelegateCommand(Exit);
        public event EventHandler? OnRequestWindowClose;

        private string _name = "woong20123";
        private string _addressInfo = "127.0.0.1:13211";

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }
        public string AddressInfo
        {
            get => _addressInfo;
            set => SetProperty(ref _addressInfo, value);
        }

        private void Login()
        {
            string ip = string.Empty;
            int port = 0;

            if (string.IsNullOrEmpty(AddressInfo) || !AddressInfo.Contains(':'))
            {
                MessageBox.Show($"주소 정보 {AddressInfo} 가 이상합니다.", "Information");
                return;
            }

            var addressInfos = AddressInfo.Split(':');
            ip = addressInfos[0];
            port = int.Parse(addressInfos[1]);

            Network.Network.Instance.SessionConnect(ip, port,
                () =>
                {
                    Dispatcher.CurrentDispatcher.Invoke(new Action(() =>
                    {
                        Log.Information("Success Session Connected");

                        MainWindow mainWindow = new MainWindow();
                        mainWindow.Show();
                        Network.Network.Instance.LoginInfo = new LoginInfo(_name, ip, port);

                        OnRequestWindowClose!(this, new EventArgs());
                        Network.Network.Instance.AsyncSendLoginReq(_name);
                    }));
                });
        }

        private void Exit()
        {
            Environment.Exit(0);
            System.Diagnostics.Process.GetCurrentProcess().Kill();
            OnRequestWindowClose!(this, new EventArgs());
        }
    }
}
