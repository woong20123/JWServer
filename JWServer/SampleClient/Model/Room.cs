using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient.Model
{
    public class Room
    {
        public string? Name { get; set; }
        public long Id { get; set; }
        public long HostId { get; set; }
        public string? HostName { get; set; }
    }
}
