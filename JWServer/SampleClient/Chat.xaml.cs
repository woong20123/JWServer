using System;
using System.Collections.Generic;
using System.Linq;
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

namespace SampleClient
{
    /// <summary>
    /// Chat.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Chat : Page
    {
        LoginInfo LoginInfo { get; set; }
        public Chat(LoginInfo loginInfo)
        {
            InitializeComponent();
            this.LoginInfo = loginInfo;
        }

        private void inputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (Key.Enter == e.Key)
            {
                sendText();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            sendText();
        }
        void sendText()
        {
            string? inputString = inputTextBox.Text;

            if (String.IsNullOrEmpty(inputString))
                return;

            inputTextBox.Text = String.Empty;
            viewTextBox.Text += String.Format($"{LoginInfo.UserName} : {inputString} {Environment.NewLine}");

            Network.Network.Instance.AsyncSendChatReq(LoginInfo.UserName, inputString);
        }
    }
}
