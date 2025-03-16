using SampleClient.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace SampleClient.ViewModel
{
    internal class RoomListViewModel
    {
        public ICommand CreateRoomCommand => new DelegateCommand(openCreateRoomWindow);

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

        public void ClearRooms()
        {
            Rooms.Clear();
        }

        public void AddRoom(Room room)
        {
            Rooms.Add(room);
        }
    }
}
