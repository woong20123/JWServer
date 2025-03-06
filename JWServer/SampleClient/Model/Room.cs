using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient.Model
{
    internal class Room
    {
        public string? Name { get; set; }
        public int Capacity { get; set; }
        public bool IsAvailable { get; set; }
    }
}
