using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace SampleClient.ViewModel
{
    public class ChatViewModel : BindableBase
    {
        public ICommand SendCommand => new DelegateCommand(send);

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

        private void send()
        {
            if (string.IsNullOrEmpty(InputString))
                return;

            var LoginInfo = Network.Network.Instance.LoginInfo;
            var inputMsg = InputString;

            InputString = string.Empty;

            Network.Network.Instance.AsyncSendChatReq(LoginInfo.UserName, inputMsg);
        }

        public void SetViewText(string name, string str)
        {
            ViewText += string.Format($"{name} : {str} {Environment.NewLine}");
        }
    }
}
