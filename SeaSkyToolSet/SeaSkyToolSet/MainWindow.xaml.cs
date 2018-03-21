using System;
using System.Windows;
using System.Windows.Threading;
using System.Xml;
using Microsoft.Win32;
using TheExcelEdit;

namespace MaintenanceToolSet
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private string path = AppDomain.CurrentDomain.BaseDirectory;
        ExcelEdit systemtable = new ExcelEdit();
        SSADevManager devmanager = new SSADevManager();
        
        public ViewModeProject viewMode =new ViewModeProject();
        private string ipaddr_string = "";
        private DispatcherTimer timer1 = new DispatcherTimer();

        public MainWindow()
        {
            InitializeComponent();
            this.DataContext = viewMode;

            MaintGrid.ItemsSource = viewMode.DeviceInforBindList;

            SetIpWin window = new SetIpWin();
            if (window.ShowDialog() == true)
            {
                ipaddr_string = window.ipaddr;
            }
            else
            {
                Close();
            }
            textBoxProject.Text = path + "Project_PIS_Config.xml";
            LoadProjectXmlFile(textBoxProject.Text);
            devmanager.SSADevDllInit(0x101, viewMode.CurrentProjectBinding.Projectpro,
                viewMode.CurrentProjectBinding.ProjectNum, 0x101,ipaddr_string);
            SetStatusBarclock();
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
                LoadProjectXmlFile(textBoxProject.Text);
            }           
        }

        private void LoadProjectXmlFile(string fileName)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(fileName);
                viewMode.ProjectBindList.Clear();
                XmlNodeList list = doc.SelectNodes("/ProjectInf/Item");
                foreach (XmlNode n in list)
                {
                    viewMode.ProjectBindList.Add(new ProjectInfo(n.Attributes["name"].Value, Convert.ToUInt16(n.Attributes["projectnum"].Value,16), Convert.ToUInt16(n.Attributes["projectpro"].Value, 16)));
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void MenuItem_AboutClick(object sender, RoutedEventArgs e)
        {
            About about = new About();
            about.ShowDialog();
        }
    }
}
