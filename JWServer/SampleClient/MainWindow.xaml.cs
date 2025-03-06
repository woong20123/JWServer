using SampleClient.Network;
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
        List<TextBox> viewTextBoxsList = new List<TextBox>();

        public MainWindow()
        {
            InitializeComponent();
            Chat chat = new Chat();

            rightFrame.Navigate(chat);
            rightFrame.RegisterName("chatPage", chat);

            RoomList roomList = new RoomList();
            mainFrame.Navigate(roomList);
            mainFrame.RegisterName("roomList", roomList);
        }

        public Chat? GetChatPage()
        {
            return rightFrame.FindName("chatPage") as Chat;
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            Network.Network.Instance.Close();
        }
    }
}