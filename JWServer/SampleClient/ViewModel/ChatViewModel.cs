using SampleClient.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace SampleClient.ViewModel
{
    public class ChatViewModel : BindableBase
    {
        public ICommand SendCommand => new DelegateCommand(send);

        public ObservableCollection<MemberInfo> MemberList
        {
            get; set;
        } = new ObservableCollection<MemberInfo> { };

        private long _roomId;
        private string _inputString = string.Empty;
        private string _viewText = string.Empty;
        public string InputString
        {
            get => _inputString;
            set => SetProperty(ref _inputString, value);
        }

        public string ViewText
        {
            get => _viewText;
            set
            {
                SetProperty(ref _viewText, value);
            }
        }

        public long RoomId
        {
            get => _roomId;
            set => SetProperty(ref _roomId, value);
        }

        public ChatViewModel()
        {
        }

        private void send()
        {
            if (string.IsNullOrEmpty(InputString))
                return;

            var LoginInfo = Network.Network.Instance.LoginInfo;
            var inputMsg = InputString;

            InputString = string.Empty;

            Network.Network.Instance.GetPacketSender()?.SendRoomChatReq(_roomId, LoginInfo.UserName, inputMsg);
        }

        public void SetViewText(string name, string str)
        {
            ViewText += string.Format($"{name} : {str} {Environment.NewLine}");
        }

        public void ClearMemberName()
        {
            MemberList.Clear();
        }

        public void AddMemberName(MemberInfo memberInfo)
        {
            MemberList.Add(memberInfo);
        }

        public void RemoveMemberName(MemberInfo memberInfo)
        {
            var removeidx = MemberList.Select((value, idx) => new { value, idx }).Where(x => x.value.Id == memberInfo.Id).Select(x => x.idx).FirstOrDefault(-1);
            if (0 <= removeidx)
                MemberList.RemoveAt(removeidx);
        }

        public void OnClose()
        {
            Network.Network.Instance.GetPacketSender()?.SendLeaveRoom(_roomId);
        }
    }
}
