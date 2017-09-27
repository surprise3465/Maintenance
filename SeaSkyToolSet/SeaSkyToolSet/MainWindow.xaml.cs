using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;
using System.Xml;
using Microsoft.Win32;
using TheExcelEdit;
using wrapper;

namespace MaintenanceToolSet
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        ManagedClass ssadevdll = new ManagedClass();
        ExcelEdit systemtable = new ExcelEdit();
        private String path = AppDomain.CurrentDomain.BaseDirectory;
        private List<ProjectInfo> ProjectInfoList = new List<ProjectInfo>();
        private string ipaddr_string = "";
        private DispatcherTimer timer1 = new DispatcherTimer();

        public class ProjectInfo
        {
            public string ProjectName { get; private set;}
            public int ProjectNum { get; private set; }
            public int Projectpro { get; private set; }

            public ProjectInfo(string projectName, int projectNum, int projectpro)
            {
                ProjectName = projectName;
                ProjectNum = projectNum;
                Projectpro = projectpro;
            }
        }

        public MainWindow()
        {
            InitializeComponent();

            SetIpWin window = new SetIpWin();
            if (window.ShowDialog() == true)
            {
                ipaddr_string = window.ipaddr;
            }
            else
            {
                this.Close();
            }
            
            ReadProjectXml();
            SetStatusBarclock();
        }

        private void ReadProjectXml()
        {
            //InitSocket(ipaddr_string);
        }

        private void InitSocket()
        {
            //ssadevdll.SSADevDllInit(ipaddr_string);
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
            int timeZone = currhour - utchour;
            if (timeZone <= -12)
            {
                timeZone += 24;
            }
            else if (timeZone > 12)
            {
                timeZone -= 24;
            }

            if (timeZone >= 0)
                datatime.Text = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")+" UTC+"+timeZone+":00";
            else
                datatime.Text = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + " UTC" + timeZone + ":00";
        }

        private void browsebutton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.DefaultExt = ".xml";
            dlg.Filter = "XML documents (.xml)|*.xml";
            if (true == dlg.ShowDialog())
            {
                textBoxProject.Text = dlg.FileName;
                LoadXmlFile(textBoxProject.Text);
            }           
        }

        private void LoadXmlFile(string fileName)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(fileName);
                ProjectInfoList.Clear();
                XmlNodeList list = doc.SelectNodes("/ProjectInf/Item");
                foreach (XmlNode n in list)
                {
                    ProjectInfoList.Add(new ProjectInfo(n.Attributes["name"].Value, Convert.ToInt32(n.Attributes["projectnum"].Value,16), Convert.ToInt32(n.Attributes["projectpro"].Value, 16)));
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return;
            }
        }

    }
}
