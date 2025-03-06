using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient.Model
{
    class CreateRoomInfo : BindableBase
    {
        private string _name = "empty";
        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

    }

}
