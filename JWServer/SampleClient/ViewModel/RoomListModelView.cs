using SampleClient.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace SampleClient.ViewModel
{
    public class RoomListViewModel
    {
        public ICommand CreateRoomCommand => new DelegateCommand(openCreateRoomWindow);
        public ICommand EnterRoomCommand => new RelayCommand<Room>(onEnterRoom);

        public ICommand RefreshRoomListCommand => new DelegateCommand(onRefreshRoomList);

        public ObservableCollection<Room> Rooms { get; set; }

        public RoomListViewModel()
        {
            Rooms = new ObservableCollection<Room>
            {
            };
        }

        private void openCreateRoomWindow()
        {
            CreateRoomWindow createWindow = new CreateRoomWindow();
            createWindow.Show();
        }

        private void onEnterRoom(Room? enterRoom)
        {
            long roomId = enterRoom?.Id ?? 0;
            Network.Network.Instance.GetPacketSender()?.SendEnterRoom(roomId);
        }

        private void onRefreshRoomList()
        {
            Network.Network.Instance.GetPacketSender()?.SendRoomList();
        }

        public void ClearRooms()
        {
            Rooms.Clear();
        }

        public void AddRoom(Room room)
        {
            if (0 == Rooms.Where(x => x.Id == room.Id).Count())
            {
                Rooms.Add(room);
            }
        }

        public void DeleteRoom(long roomId)
        {
            var room = Rooms.FirstOrDefault(x => x.Id == roomId);
            if (room != null)
            {
                Rooms.Remove(room);
            }
        }

    }
}
