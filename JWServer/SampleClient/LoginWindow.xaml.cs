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
using System.Windows.Shapes;

namespace SampleClient
{
    /// <summary>
    /// StartWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class LoginWindow : Window
    {
        TcpClient? client = null;
        public LoginWindow()
        {
            InitializeComponent();
        }

        async private void SessionConnect(string ip, int port)
        {
            client = new TcpClient();
            await client.ConnectAsync(ip, port);
            MessageBox.Show("서버 연결에 성공하였습니다.", "Information");
        }
        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string addressInfoText = addressInputText.Text;
                string ip = "";
                int port = 0;
                if (addressInfoText.Contains(':'))
                {
                    var addressInfos = addressInfoText.Split(':');
                    ip = addressInfos[0];
                    port = int.Parse(addressInfos[1]);
                }
                SessionConnect(ip, port);

            }
            catch
            {
                MessageBox.Show("서버 연결에 실패하였습니다.", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                client = null;
            }
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
            System.Diagnostics.Process.GetCurrentProcess().Kill();
            this.Close();
        }
    }
}
