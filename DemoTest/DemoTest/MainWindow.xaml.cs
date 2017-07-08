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
using System.Windows.Threading;

namespace DemoTest
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private String ipaddr = "";
        private DispatcherTimer timer1 = new DispatcherTimer();
        public MainWindow()
        {
            InitializeComponent();
            SetIpWin window = new SetIpWin();
            if(window.ShowDialog() == true)
            {
                ipaddr = window.ipaddr;
                ;
            }
            timer1.Interval = TimeSpan.FromSeconds(1);
            timer1.Tick += timer1_Tick;
            timer1.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //（你的定时处理）
            datatime.Text = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
            int utchour = DateTime.UtcNow.Hour;
            int currhour = DateTime.Now.Hour;


        }

        private void image1_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            tabControl.SelectedIndex = 0;
            
        }

        private void image2_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            tabControl.SelectedIndex = 1;
        }
    }
}
