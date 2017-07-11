using System;
using System.Windows;
using System.Windows.Threading;
using wrapper;
using TheExcelEdit;

namespace DemoTest
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        ManagedClass ssadevdll = new ManagedClass();
        ExcelEdit systemtable = new ExcelEdit();
        private String ipaddr = "";
        private DispatcherTimer timer1 = new DispatcherTimer();
        public MainWindow()
        {
            InitializeComponent();
            SetIpWin window = new SetIpWin();
            if(window.ShowDialog() == true)
            {
                ipaddr = window.ipaddr;
                ssadevdll.SSADevDllClose();
            }
            set_clock();
            ReadProjectXML();
        }

        private void ReadProjectXML()
        {

        }

        private void set_clock()
        {
            timer1.Interval = TimeSpan.FromSeconds(1);
            timer1.Tick += timer1_Tick;
            timer1.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            int utchour = DateTime.UtcNow.Hour;
            int currhour = DateTime.Now.Hour;
            int time_zone = currhour - utchour;
            if (time_zone <= -12)
            {
                time_zone += 24;
            }
            else if (time_zone > 12)
            {
                time_zone -= 24;
            }

            if (time_zone >= 0)
                datatime.Text = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")+" UTC+"+time_zone.ToString()+":00";
            else
                datatime.Text = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + " UTC" + time_zone.ToString() + ":00";
        }

    }
}
