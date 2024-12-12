using System.Text;
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
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        LoginInfo LoginInfo { get; set; }
        List<TextBox> viewTextBoxsList = new List<TextBox>();

        public MainWindow(LoginInfo loginInfo)
        {
            InitializeComponent();
            this.LoginInfo = loginInfo;

            Chat chat = new Chat(loginInfo);
            rightFrame.Navigate(chat);
        }
    }
}