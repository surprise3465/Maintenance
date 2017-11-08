using System;
using System.Windows;
using System.Windows.Threading;
using System.Xml;
using Microsoft.Win32;
using TheExcelEdit;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace MaintenanceToolSet
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        ExcelEdit systemtable = new ExcelEdit();

        private string path = AppDomain.CurrentDomain.BaseDirectory;
        
        public ViewModeProject viewModeProject = new ViewModeProject();

        public class ViewModeProject: INotifyPropertyChanged
        {
            private ObservableCollection<ProjectInfo> ProjectInfoList = new ObservableCollection<ProjectInfo>();
            public ProjectInfo CurrentProjectInfo = new ProjectInfo("All", 0x00, 0x11b);

            public ProjectInfo CurrentProjectBinding
            {
                get { return CurrentProjectInfo; }
                set
                {
                    if (CurrentProjectInfo != value)
                    {
                        CurrentProjectInfo = value;
                        OnPropertyChanged("CurrentProjectBinding");
                    }
                }
            }

            public ObservableCollection<ProjectInfo> ProjectBindList
            {
                get{return ProjectInfoList;}
                set
                {
                    if (ProjectInfoList != value)
                    {
                        ProjectInfoList = value;
                        OnPropertyChanged("ProjectBindList");
                    }
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;
            private void OnPropertyChanged(string propertyName)
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        
        private string ipaddr_string = "";
        private DispatcherTimer timer1 = new DispatcherTimer();

        public class ProjectInfo 
        {           
            public string ProjectName { get; set; }
            public int ProjectNum { get; set; }
            public int Projectpro { get; set; }

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

            ProjectSelCom.DataContext = viewModeProject;

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
            InitSocket();
            SetStatusBarclock();
        }

        private void InitSocket()
        {
            try
            {
               
            }
            catch (SocketException ex)
            {
                if (ex.SocketErrorCode == SocketError.AddressAlreadyInUse)
                {
                    MessageBox.Show("The Port Has Been In Use. Please Close Other Applications And Restart This Application!");
                }
                else
                {
                    MessageBox.Show("Socket Initialize Failed! The Application Can't Work Normally!\r\nPlease Close Other Applications And Restart This Application! ");
                }
            }
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
                viewModeProject.ProjectBindList.Clear();
                XmlNodeList list = doc.SelectNodes("/ProjectInf/Item");
                foreach (XmlNode n in list)
                {
                    viewModeProject.ProjectBindList.Add(new MainWindow.ProjectInfo(n.Attributes["name"].Value, Convert.ToInt32(n.Attributes["projectnum"].Value,16), Convert.ToInt32(n.Attributes["projectpro"].Value, 16)));
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
