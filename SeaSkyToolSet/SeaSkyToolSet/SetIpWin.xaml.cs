using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Windows;

namespace MaintenanceToolSet
{
    /// <summary>
    /// SetIpWin.xaml 的交互逻辑
    /// </summary>
    public partial class SetIpWin : Window
    {
        public string ipaddr = "";
        int AdapterNum = 0;

        public struct m_Adapter
        {
            public int SelectNum{ get; set; }
            public string IPAddr { get; set; }
            public string MACAddr { get; set; }
            public string AdapterName { get; set; }
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

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var a = dataGridView.SelectedItem;
            var b = (m_Adapter)a;
            ipaddr = b.IPAddr;
            DialogResult = true;
            Close();
        }
    }
}
