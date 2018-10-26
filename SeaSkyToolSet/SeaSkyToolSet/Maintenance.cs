using System;
using System.Windows;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using Microsoft.Win32;
using System.Linq;
using System.Text.RegularExpressions;
using TheExcelEdit;
using System.Windows.Controls;
using System.Xml;
using System.Xml.Serialization;

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

            viewMode.DeviceInforBindList.Add(new DeviceInformation("HK93", "DACU(01)", "172.16.0.102", "A4-4A-35-03-01-01", "255.255.255.240",
                "V100D01", "V2.20", "V2.36", "V1.00", "V1.2345", "V3.00", "V1.00r02", "048190011802090020033", "Normal"));
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
            MaintGrid.ItemsSource = viewMode.DeviceInforBindList;
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
            string result = Regex.Match(viewMode.CurrentDeviceBinding, @"[\w]+").Value;
            if (result == "ALL")
            {
                DevTempList.Clear();
                foreach (DeviceInformation dev in viewMode.DeviceInforBindList)
                {
                        DevTempList.Add(dev);
                }
            }
            else
            {
                DevTempList.Clear();
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
            DateTime tNow = DateTime.Now;
            saveDlg.FileName = tNow.Year + tNow.Month.ToString() + tNow.Day + tNow.Hour + tNow.Minute + tNow.Second;
            var result = saveDlg.ShowDialog();
            if (result == true)
            {
                MessageBox.Show(exportExcel.SaveAs(saveDlg.FileName) ? "Save Success!" : "Save Fail!");
            }

            exportExcel.Close();
        }

        private void DeserializeXMLDoc(object sender, RoutedEventArgs routedEventArgs)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.DefaultExt = ".txt";
            dlg.Filter = "Text documents (.txt)|*.txt";
            XmlSerializer xmlserializer = new XmlSerializer(typeof(ObservableCollection<DeviceInformation>));
            if (true == dlg.ShowDialog())
            {
                FileStream readstream = new FileStream(dlg.FileName, FileMode.Open, FileAccess.Read,FileShare.Read);
                DevTempList = (ObservableCollection < DeviceInformation >)xmlserializer.Deserialize(readstream);
                MaintGrid.ItemsSource = DevTempList;
                readstream.Close();
            }
        }

        private void SerializeIntoXML(object sender, RoutedEventArgs routedEventArgs)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.DefaultExt = ".txt";
            dlg.Filter = "Text documents (.txt)|*.txt";
            if (true == dlg.ShowDialog())
            {
                XmlSerializer xmlserializer = new XmlSerializer(typeof(ObservableCollection<DeviceInformation>));
                Stream stream = new FileStream(dlg.FileName, FileMode.Create, FileAccess.Write, FileShare.ReadWrite);
                xmlserializer.Serialize(stream, DevTempList);
                stream.Close();
            }
        }

        private void MaintClearBtn_Click(object sender, RoutedEventArgs e)
        {
            viewMode.DeviceInforBindList.Clear();
            MaintGrid.ItemsSource = viewMode.DeviceInforBindList;
        }
    }
}