using System;
using System.Windows;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Microsoft.Win32;
using System.Linq;
using System.Text.RegularExpressions;
using TheExcelEdit;
using System.Windows.Controls;

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
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93","DACU(01)","172.16.0.102","A4-4A-35-03-01-01","255.255.255.240",
                "V100D01","V2.20","V2.36","V1.00","V1.2345","V3.00","V1.00r02","048190011802090020033","Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "DACU(02)", "172.16.0.102", "A4-4A-35-03-01-02", "255.255.255.240",
                "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020023", "Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "Switch(00)", "172.16.0.95", "A4-4A-35-04-01-02", "255.255.255.240",
              "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020013", "Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "Switch(01)", "172.16.0.25", "A4-4A-35-04-01-02", "255.255.255.240",
              "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020000", "Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "Switch(02)", "172.16.0.35", "A4-4A-35-04-01-02", "255.255.255.240",
              "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020001", "Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "Switch(03)", "172.16.0.45", "A4-4A-35-04-01-02", "255.255.255.240",
              "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020002", "Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "TLCD(01)", "172.16.0.111", "A4-4A-35-03-02-02", "255.255.255.240",
                "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020003", "Normal"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "TLCD(02)", "172.16.0.112", "A4-4A-35-03-02-03", "255.255.255.240",
                "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r03", "048190011802090020004", "BootLoader"));
            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "TLCD(03)", "172.16.0.113", "A4-4A-35-03-02-04", "255.255.255.240",
                "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.01r02", "048190011802090020004", "BootLoader"));

            viewMode.DevNumDictionBindList.Clear();
            viewMode.DevNumDictionBindList.Add("ALL", 0);
            foreach (DeviceInformation devinfo in viewMode.DeviceInforBindList)
            {
                viewMode.DevNumDictionBindList["ALL"]++;
                if (viewMode.DevNumDictionBindList.Keys.Contains(devinfo.deviceShortName))
                {
                    viewMode.DevNumDictionBindList[devinfo.deviceShortName]++;
                }
                else
                {
                    viewMode.DevNumDictionBindList.Add(devinfo.deviceShortName, 1);
                }
            }
        }

        public List<DeviceInformation> ExtractionNetInfo(List<NetworkInformation> Srclist)
        {
            List<DeviceInformation> Destlist =new List<DeviceInformation>();
            return Destlist;
        }

        private void DevCount_Click(object sender, RoutedEventArgs e)
        {
            DeviceCount DevCntWindow = new DeviceCount();
            DevCntWindow.DevNumberDiction = viewMode.DevNumDictionBindList;
            DevCntWindow.ShowDialog();
        }

        private void DeviceSelCom_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            ObservableCollection<DeviceInformation> DevTempList = new ObservableCollection<DeviceInformation>();
            string result = Regex.Match(viewMode.CurrentDeviceBinding, @"[\w]+").Value;
            if (result == "ALL")
            {
                DevTempList = viewMode.DeviceInforBindList;
            }
            else
            {
                foreach (DeviceInformation dev in viewMode.DeviceInforBindList)
                {
                    if (result == dev.deviceShortName)
                    {
                        DevTempList.Add(dev);
                    }
                }
            }

            MaintGrid.ItemsSource = DevTempList;
        }

        private void ExportFile_Click(object sender, RoutedEventArgs e)
        {
            ExcelEdit exportExcel = new ExcelEdit();
            exportExcel.Create();
            string name = exportExcel.GetSheetName(1);
            rowStart = 1;
            colStart = 1;
            rowEnd = MaintGrid.Items.Count + 1;
            colEnd = MaintGrid.Columns.Count;
            exportExcel.SetNumberFormat(name,rowStart,colStart,rowEnd,colEnd, "@");
            for (int i = 0; i < MaintGrid.Columns.Count; i++)
            {
                exportExcel.SetCellValue(name, 1, i+1, MaintGrid.Columns[i].Header);
                for (int j = 0; j < MaintGrid.Items.Count; j++)
                {
                    exportExcel.SetCellValue(name, j+2, i+1, ((TextBlock) MaintGrid.Columns[i].GetCellContent(MaintGrid.Items[j]))?.Text);
                }
            }
            
            SaveFileDialog saveDlg = new SaveFileDialog();
            saveDlg.Filter = "xlsx|*.xlsx|xls|*.xls|All|*.*";
            saveDlg.FileName = DateTime.Now.ToString();
            var result = saveDlg.ShowDialog();
            if (result == true)
            {
                MessageBox.Show(exportExcel.SaveAs(saveDlg.FileName) ? "保存成功" : "保存失败");
            }

            exportExcel.Close();
        }
    }
}