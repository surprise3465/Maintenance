using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Controls.Primitives;
using System.Windows.Threading;
using System.Xml;
using Microsoft.Win32;
using TheExcelEdit;
using Vlc.DotNet.Wpf;

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

        private DispatcherTimer clocktimer = new DispatcherTimer();
        private DispatcherTimer videotimer = new DispatcherTimer();

        private readonly DirectoryInfo vlcLibDirectory;
        private VlcControl vlcControl;

        public ObservableCollection<DeviceInformation> DevTempList = new ObservableCollection<DeviceInformation>();

        public MainWindow()
        {
            InitializeComponent();
            var currentAssembly = Assembly.GetEntryAssembly();
            var currentDirectory = new FileInfo(currentAssembly.Location).DirectoryName;
            // Default installation path of VideoLAN.LibVLC.Windows
            vlcLibDirectory = new DirectoryInfo(Path.Combine(currentDirectory, "libvlc", IntPtr.Size == 4 ? "win-x86" : "win-x64"));

            DataContext = viewMode;
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
<<<<<<< HEAD
            FpgaUpdate1.localIPAddr = ipaddr_string;
=======
>>>>>>> 75de24790ce485905e4b84a1a0899a508457c393
            //devmanager.SSADevDllInit(0x101, viewMode.CurrentProjectBinding.Projectpro,
            //    viewMode.CurrentProjectBinding.ProjectNum, 0x101,ipaddr_string);
            SetStatusBarclock();
        }

        private void SetStatusBarclock()
        {
            clocktimer.Interval = TimeSpan.FromSeconds(1);
            clocktimer.Tick += StatusTimerTick;
            clocktimer.Start();
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

        private void palyfileBtn_Click(object sender, RoutedEventArgs e)
        {
            vlcControl?.Dispose();
            vlcControl = new VlcControl();
            ControlContainer.Content = vlcControl;
            vlcControl.SourceProvider.CreatePlayer(vlcLibDirectory);
            //vlcControl.SourceProvider.MediaPlayer.Play(new Uri("http://download.blender.org/peach/bigbuckbunny_movies/big_buck_bunny_480p_stereo.avi"));
            FileInfo fileInfo = new FileInfo(@"E:\2.avi");
            vlcControl.SourceProvider.MediaPlayer.Play(fileInfo);
            Videostatusclock();           
        }

        private void stopPlayBtn_Click(object sender, RoutedEventArgs e)
        {
            vlcControl?.Dispose();
            vlcControl = null;
            videotimer.Stop();
        }

        private void pauseBtn_Click(object sender, RoutedEventArgs e)
        {
            vlcControl.SourceProvider.MediaPlayer.Pause();
        }

        private void backBtn_Click(object sender, RoutedEventArgs e)
        {
            vlcControl.SourceProvider.MediaPlayer.Rate *= (float)0.5;
        }

        private void forwardBtn_Click(object sender, RoutedEventArgs e)
        {
            if (vlcControl == null)
            {
                return;
            }

            vlcControl.SourceProvider.MediaPlayer.Rate *= 2;
        }

        private void addURLBtn_Click(object sender, RoutedEventArgs e)
        {
            string mrl = fileNametextBox.Text;
        }

        private void openfileBtn_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Videostatusclock()
        {
            videotimer.Interval = TimeSpan.FromSeconds(1);
            videotimer.Tick += VideoTimerTick;
            videotimer.Start();
        }

        public static string formatLongToTimeStr(long l)
        {
            int hour = 0;
            int minute = 0;
            int second = 0;

            second = (int)l/1000;

            if (second>60)
            {
                minute = second / 60;
                second = second % 60;
            }
            if (minute > 60)
            {
                hour = minute / 60;
                minute = minute % 60;
            }
            return $"{hour:00}:{minute:00}:{second:00}";
        }

        private void VideoTimerTick(object sender, EventArgs e)
        {
            if (vlcControl == null)
            {
                return;
            }
           
            if (vlcControl.SourceProvider.MediaPlayer.Length != 0)
            {
                currentTime.Content = formatLongToTimeStr(vlcControl.SourceProvider.MediaPlayer.Time);
                totalLength.Content = formatLongToTimeStr(vlcControl.SourceProvider.MediaPlayer.Length);
                sliderVideo.Maximum = vlcControl.SourceProvider.MediaPlayer.Length;
                sliderVideo.Value = vlcControl.SourceProvider.MediaPlayer.Time;
                sliderVolume.Value = vlcControl.SourceProvider.MediaPlayer.Audio.Volume;
            }
            
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            vlcControl?.Dispose();
            vlcControl = null;
            clocktimer.Stop();
            videotimer.Stop();
        }

        private void sliderVideo_Drag(object sender, DragCompletedEventArgs dragCompletedEventArgs)
        {
            if (vlcControl == null)
            {
                return;
            }

            vlcControl.SourceProvider.MediaPlayer.Time = (long)sliderVideo.Value;
        }

        private void sliderVolume_Drag(object sender, DragCompletedEventArgs dragCompletedEventArgs)
        {
            if (vlcControl == null)
            {
                return;
            }

            vlcControl.SourceProvider.MediaPlayer.Audio.Volume =(int)sliderVolume.Value;
        }

        private void sliderVideo_MouseLefButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (vlcControl == null)
            {
                return;
            }

            vlcControl.SourceProvider.MediaPlayer.Time = (long)sliderVideo.Value;
        }

        private void sliderVolume_MouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (vlcControl == null)
            {
                return;
            }

            vlcControl.SourceProvider.MediaPlayer.Audio.Volume = (int)sliderVolume.Value;
        }
    }
}
