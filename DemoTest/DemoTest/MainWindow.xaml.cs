using System;
using System.Windows;
using System.Windows.Threading;
using wrapper;
using TheExcelEdit;
using System.Collections.Generic;

namespace DemoTest
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        ManagedClass ssadevdll = new ManagedClass();
        ExcelEdit systemtable = new ExcelEdit();

        private string ipaddr_string = "";
        private sbyte[] ipaddr_inbyte = new sbyte[256];
        private DispatcherTimer timer1 = new DispatcherTimer();

        public MainWindow()
        {
            InitializeComponent();

            SetIpWin window = new SetIpWin();
            if (window.ShowDialog() == true)
            {
                ipaddr_string = window.ipaddr;
                int i = 0;
                foreach (var s in ipaddr_string)
                {
                    ipaddr_inbyte[156 + i] = (Convert.ToSByte(s));
                    i++;
                }
                ipaddr_inbyte[172] = 1;
            }
            else
            {
                this.Close();
            }
            
            ReadProjectXML();
            SetStatusBarclock();
        }

        private void ReadProjectXML()
        {

            InitSocket();
        }

        private void InitSocket()
        {
            //ssadevdll.SSADevDllInit();
        }
        private void SetStatusBarclock()
        {
            timer1.Interval = TimeSpan.FromSeconds(1);
            timer1.Tick += StatusTimerTick;
            timer1.Start();
        }

        private void StatusTimerTick(object sender, EventArgs e)
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
