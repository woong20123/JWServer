using SampleClient.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace SampleClient.ViewModel
{
    internal class CreateRoomWindowViewModel : BindableBase
    {
        private CreateRoomInfo? _roomInfo = null;
        public ICommand CreateRoomCommand => new DelegateCommand(createRoom);
        public ICommand CancelCommand => new DelegateCommand(exit);

        public event EventHandler? OnRequestWindowClose;

        public CreateRoomWindowViewModel()
        {
            _roomInfo = new CreateRoomInfo();
        }

        public CreateRoomInfo? RoomInfo
        {
            get => _roomInfo;
            set => SetProperty(ref _roomInfo, value);
        }
        private void createRoom()
        {
            Network.Network.Instance.GetPacketSender()?.SendCreateRoom(RoomInfo!.Name);
        }

        void exit()
        {
            RoomInfo = null;
            OnRequestWindowClose!(this, EventArgs.Empty);
        }
    }
}
