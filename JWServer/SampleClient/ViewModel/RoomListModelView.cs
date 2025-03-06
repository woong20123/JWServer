using SampleClient.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient.ViewModel
{
    internal class RoomListViewModel
    {
        public ObservableCollection<Room> Rooms { get; set; }

        public RoomListViewModel()
        {
            Rooms = new ObservableCollection<Room>
            {
                new Room { Name = "Conference Room A", Capacity = 10, IsAvailable = true },
                new Room { Name = "Meeting Room B", Capacity = 5, IsAvailable = false },
                new Room { Name = "Seminar Hall", Capacity = 50, IsAvailable = true }
            };
        }
    }
}
