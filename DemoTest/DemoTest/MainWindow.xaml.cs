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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace DemoTest
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private String ipaddr = "";
        public MainWindow()
        {
            InitializeComponent();
            SetIpWin window1 = new SetIpWin();
            if(window1.ShowDialog() == true)
            {
                ipaddr = window1.ipaddr;
                return;
            }
        }
    }
}
