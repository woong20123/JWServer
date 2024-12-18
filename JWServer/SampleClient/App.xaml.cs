using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Windows;

namespace SampleClient
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : PrismApplication
    {
        // CreateShell 구현
        protected override Window CreateShell()
        {
            return Container.Resolve<LoginWindow>();
        }
        // RegisterTypes구현
        protected override void RegisterTypes(IContainerRegistry containerRegistry)
        {
        }
    }

}
