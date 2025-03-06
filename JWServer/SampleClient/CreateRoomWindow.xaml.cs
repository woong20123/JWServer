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
using System.Windows.Shapes;

namespace SampleClient
{
    /// <summary>
    /// CreateRoomWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class CreateRoomWindow : Window
    {
        public CreateRoomWindow()
        {
            InitializeComponent();

            var vm = new CreateRoomWindowViewModel();
            this.DataContext = vm;

            vm.OnRequestWindowClose += (s, e) =>
            {
                this.Close();
            };

        }
        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
