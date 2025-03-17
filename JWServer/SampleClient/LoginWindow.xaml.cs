using SampleClient.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Serilog;

namespace SampleClient
{
    public struct LoginInfo
    {
        public LoginInfo(string username, string ip, int port)
        {
            this.UserName = username;
            this.IP = ip;
            this.Port = port;
        }
        public string UserName { get; }
        public string IP { get; }
        public int Port { get; }
    }
    /// <summary>
    /// StartWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class LoginWindow : Window
    {
        public LoginWindow()
        {
            InitializeComponent();

            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.File("logs/SampleClient_.txt", rollingInterval: RollingInterval.Day)
                .CreateLogger();

            Network.Network.Instance.Initialize();
            Network.Network.Instance.SetDispatcher(System.Windows.Threading.Dispatcher.CurrentDispatcher);

            var vm = new LoginWindowViewModel();
            this.DataContext = vm;
            this.Closed += Window_Closed;

            // 로그인 창만 닫히도록 등록
            vm.OnRequestWindowClose += (s, e) =>
            {
                this.Close();
            };
        }

        private void Window_Closed(object? sender, EventArgs e)
        {

        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            this.processKill();
            this.Close();
            Network.Network.Instance.Close();
        }

        private void processKill()
        {
            Environment.Exit(0);
            System.Diagnostics.Process.GetCurrentProcess().Kill();
        }
    }
}

