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

            RoomList roomList = new RoomList();
            mainFrame.Navigate(roomList);
            mainFrame.RegisterName("roomList", roomList);

            this.Closed += Window_Closed;
        }

        public Chat? GetChat()
        {
            return rightFrame.FindName("chatPage") as Chat;
        }

        public RoomList? RoomList
        {
            get
            {
                return mainFrame.FindName("roomList") as RoomList;
            }
        }

        private void Window_Closed(object? sender, EventArgs e)
        {
            Network.Network.Instance.Close();
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            this.processKill();
            this.Close();
        }
        private void processKill()
        {
            Environment.Exit(0);
            System.Diagnostics.Process.GetCurrentProcess().Kill();
        }

    }
}