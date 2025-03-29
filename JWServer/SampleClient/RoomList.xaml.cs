using SampleClient.Model;
using SampleClient.ViewModel;
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
using System.Windows.Threading;

namespace SampleClient
{
    /// <summary>
    /// RoomList.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class RoomList : Page
    {
        RoomListViewModel rlvm;
        public RoomList()
        {
            InitializeComponent();
            rlvm = new RoomListViewModel();
            this.DataContext = rlvm;

            Thread.Sleep(100);
            System.Windows.Threading.Dispatcher.CurrentDispatcher.BeginInvoke(DispatcherPriority.Background, () => Network.Network.Instance.GetPacketSender()?.SendRoomList());


        }

        public RoomListViewModel getRoomListViewModel()
        {
            return rlvm;
        }

        public void UpdateRoomList(List<Room> rooms)
        {
            rlvm.ClearRooms();
            foreach (var room in rooms)
            {
                rlvm.AddRoom(room);
            }
        }
    }
}
