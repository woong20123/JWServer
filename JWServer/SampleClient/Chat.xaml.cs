using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
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
using SampleClient.Model;
using SampleClient.ViewModel;

namespace SampleClient
{
    /// <summary>
    /// Chat.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Chat : Window
    {
        ChatViewModel cvm;
        public Chat()
        {
            InitializeComponent();

            cvm = new ChatViewModel();
            this.DataContext = cvm;

            viewTextBox.TextChanged += (s, e) =>
            {
                viewTextBox.ScrollToEnd();
            };
        }

        public void SetRoomId(long roomId)
        {
            cvm.RoomId = roomId;
        }

        private void inputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (Key.Enter == e.Key)
            {
                cvm.SendCommand.Execute(null);
            }
        }

        public void ViewText(string name, string str)
        {
            cvm.SetViewText(name, str);
        }

        public void UpdateCreateRoomInfo(Room roomInfo)
        {
            cvm.ClearMemberName();
            var hostName = roomInfo.HostName is null ? "empty" : roomInfo.HostName;
            cvm.AddMemberName(new Model.MemberInfo { Name = hostName, Id = roomInfo.HostId });
        }
    }
}
