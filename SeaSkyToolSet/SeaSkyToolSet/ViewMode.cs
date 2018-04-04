using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Text.RegularExpressions;
using System.Windows;

namespace MaintenanceToolSet
{
    public partial class MainWindow : Window
    {
        public class ProjectInfo
        {
            public string ProjectName { get; set; }
            public ushort ProjectNum { get; set; }
            public ushort Projectpro { get; set; }

            public ProjectInfo(string projectName, ushort projectNum, ushort projectpro)
            {
                ProjectName = projectName;
                ProjectNum = projectNum;
                Projectpro = projectpro;
            }
        }

        public class DeviceInformation
        {
            public string deviceProject { get; set; }
            public string deviceName { get; set; }
            public string deviceIp { get; set; }
            public string deviceMAC { get; set; }
            public string deviceMask { get; set; }
            public string deviceFpgaA { get; set; }
            public string deviceUboot { get; set; }
            public string deviceUimage { get; set; }
            public string deviceFileSys { get; set; }
            public string deviceApp { get; set; }
            public string deviceHardVer { get; set; }
            public string deviceSys_MP { get; set; }
            public string deviceSn { get; set; }
            public string deviceOtherInf { get; set; }
            public string deviceShortName { get; set; }

            public DeviceInformation(string devProject, string devName, string devIp, string devMac, string devMask, string devFpgaA,
                string devUboot, string devUimage, string devFileSys, string devApp, string devHardVer, string devSys_MP, string devSn, string devOtherInf)
            {
                deviceProject = devProject;
                deviceName = devName;
                deviceIp = devIp;
                deviceMAC = devMac;
                deviceMask = devMask;
                deviceFpgaA = devFpgaA;
                deviceUboot = devUboot;
                deviceUimage = devUimage;
                deviceFileSys = devFileSys;
                deviceApp = devApp;
                deviceHardVer = devHardVer;
                deviceSys_MP = devSys_MP;
                deviceSn = devSn;
                deviceOtherInf = devOtherInf;
                deviceShortName = Regex.Replace(deviceName, @"\([^\(]*\)", "");
            }
        }

        public class ViewModeProject : INotifyPropertyChanged
        {
            private ObservableCollection<ProjectInfo> ProjectInfoList = new ObservableCollection<ProjectInfo>();
            private ProjectInfo CurrentProjectInfo = new ProjectInfo("All", 0x00, 0x11b);
            private ObservableCollection<DeviceInformation> DevInforList = new ObservableCollection<DeviceInformation>();
            private Dictionary<string, int> DevNumDiction = new Dictionary<string, int>();
            private string CurrentDiction;

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
                get { return ProjectInfoList; }
                set
                {
                    if (ProjectInfoList != value)
                    {
                        ProjectInfoList = value;
                        OnPropertyChanged("ProjectBindList");
                    }
                }
            }

            public ObservableCollection<DeviceInformation> DeviceInforBindList
            {
                get { return DevInforList; }
                set
                {
                    if (DevInforList != value)
                    {
                        DevInforList = value;
                        OnPropertyChanged("DeviceInforBindList");
                    }
                }
            }

            public Dictionary<string, int> DevNumDictionBindList
            {
                get { return DevNumDiction; }
                set
                {
                    if (DevNumDiction != value)
                    {
                        DevNumDiction = value;
                        OnPropertyChanged("DevNumDictionBindList");
                    }
                }
            }

            public string CurrentDeviceBinding
            {
                get { return CurrentDiction; }
                set
                {
                    if (CurrentDiction != value)
                    {
                        CurrentDiction = value;
                        OnPropertyChanged("CurrentDeviceBinding");
                    }
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;
            public void OnPropertyChanged(string propertyName)
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
