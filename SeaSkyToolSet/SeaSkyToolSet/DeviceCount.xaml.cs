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

namespace MaintenanceToolSet
{
    /// <summary>
    /// DeviceCount.xaml 的交互逻辑
    /// </summary>
    public partial class DeviceCount : Window
    {
        public Dictionary<string, int>DevNumberDiction{ get; set; }

        public class DevCntUint
        {
            public string DevName { get; set; }
            public int DevNumber { get; set; }

            public DevCntUint(string devName, int devNumber)
            {
                DevName = devName;
                DevNumber = devNumber;
            }
        }

        List<DevCntUint> DevCntlist = new List<DevCntUint>();

        public DeviceCount()
        {
            InitializeComponent();  
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            foreach (var item in DevNumberDiction)
            {
                DevCntlist.Add(new DevCntUint(item.Key, item.Value));
            }
            dataGrid.ItemsSource = DevCntlist;
        }
    }
}
