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
using SampleClient.ViewModel;

namespace SampleClient
{
    /// <summary>
    /// Chat.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Chat : Page
    {
        ChatViewModel vm;
        public Chat()
        {
            InitializeComponent();

            vm = new ChatViewModel();
            this.DataContext = vm;

            viewTextBox.TextChanged += (s, e) =>
            {
                viewTextBox.ScrollToEnd();
            };
        }

        private void inputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (Key.Enter == e.Key)
            {
                vm.SendCommand.Execute(null);
            }
        }

        public void ViewText(string name, string str)
        {
            vm.SetViewText(name, str);
        }
    }
}
