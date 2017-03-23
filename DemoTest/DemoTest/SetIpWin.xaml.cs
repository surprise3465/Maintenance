using System;
using System.Windows;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Collections.Generic;
namespace DemoTest
{
    /// <summary>
    /// SetIpWin.xaml 的交互逻辑
    /// </summary>
    public partial class SetIpWin : Window
    {
        public String ipaddr = "";
        int AdapterNum = 0;

        public struct m_Adapter
        {
            public int SelectNum{ get; set; }
            public String IPAddr{ get; set; }
            public String MACAddr{ get; set; }
            public String AdapterName{ get; set; }
        }

        List<m_Adapter> service = new List<m_Adapter>();

        public SetIpWin()
        {
            InitializeComponent();
            NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface adapter in adapters)
            {
                if (adapter.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
                {
                    m_Adapter m_temp = new m_Adapter();
                    AdapterNum++;
                    m_temp.SelectNum = AdapterNum;

                    UnicastIPAddressInformationCollection c = adapter.GetIPProperties().UnicastAddresses;
                    foreach (UnicastIPAddressInformation ipaddr in c)
                    {
                        if (ipaddr.Address.AddressFamily == AddressFamily.InterNetwork)
                        {
                            m_temp.IPAddr = ipaddr.Address.ToString();
                        }
                    }
                    m_temp.MACAddr = adapter.GetPhysicalAddress().ToString();
                    m_temp.AdapterName= adapter.Name + ":" + adapter.Description;
                    service.Add(m_temp);
                }
            }
            dataGridView.ItemsSource = service;
            AdapterNum = 0;
        }

        private void dataGridView_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            var a = this.dataGridView.SelectedItem;
            var b = (m_Adapter)a;
            ipaddr = b.IPAddr;
            DialogResult = true;
            Close();
        }
    }
}
