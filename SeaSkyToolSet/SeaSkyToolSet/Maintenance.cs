using System.Windows;
using System.Collections.Generic;
using Microsoft.Win32;
using System.Windows;

namespace MaintenanceToolSet
{
    public partial class MainWindow : Window
    {
        int rowStart, rowEnd, colStart, colEnd;

        private void buttonCompare_Click(object sender, RoutedEventArgs e)
        {

        }

        private void buttonSysver_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "xlsx|*.xlsx|xls|*.xls|All|*.*";
            var result = dlg.ShowDialog();
            if (result == true)
            {
                textBoxSysver.Text = dlg.FileName;
                systemtable.Open(dlg.FileName);
            }
        }

        private void MaintGetAll_Click(object sender, RoutedEventArgs e)
        {
            //List<NetworkInformation> alist = devmanager.GetNetInformation();
            //deviceAllList = ExtractionNetInfo(alist);
            viewMode.DeviceInforBindList.Clear();
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93","DACU","172.16.0.112","A4-4A-35-03-02-01","255.255.255.240",
                "V100D01","V2.20","V2.36","V1.00","V1.2345","V3.00","V1.00r02","048190011802090020003","Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "TLCD", "172.16.0.113", "A4-4A-35-03-02-02", "255.255.255.240",
                "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020004", "Normal"));    
        }

        public List<DeviceInformation> ExtractionNetInfo(List<NetworkInformation> Srclist)
        {
            List<DeviceInformation> Destlist =new List<DeviceInformation>();
            return Destlist;
        }
    }
}