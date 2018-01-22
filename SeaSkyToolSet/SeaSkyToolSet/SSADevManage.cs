using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace MaintenanceToolSet
{
    class SSADevManage
    {
        const ushort TCP_PORT = 20050;
        const string UDP_ADDR = "234.4.5.6";
        const ushort UDP_SENDSRC = 30050;
        const ushort UDP_SENDDES = 30040;
        const ushort UDP_RECVSRC = 30055;
        const ushort UDP_RECVDES = 30045;
        const ushort UDP_RECVDES_UPDATE = 30060;

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct DEV_INFOR
        {
            ushort byProjectno;
            ushort byprotol;
            ushort bydevno;
            ushort byRes3;
            uint message;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            string byIp;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
            string byMAC;
            ushort byRes;
            ushort byMask;
            ushort byFpgaB;
            ushort byFpgaA;
            ushort byUboot;
            ushort byUimage;
            ushort byFileSys;
            ushort byApp;
            ushort byDaemon;
            ushort byHardVer;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            string byRes2;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            string proname;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            string devicename;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            string APP;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            string Sys_MP;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 31)]
            string sn;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            string otherinf;
        }

        public struct DEV_INFORMATION
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
            string byMAC;
            ushort byRes;
            uint byMask;
            ushort byFpgaB;
            ushort byFpgaA;
            ushort byUboot;
            ushort byUimage;
            ushort byFileSys;
            ushort byApp;
            ushort byDaemon;
            ushort byHardVer;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            string byRes2;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 200)]
            string byinfo;
        }

        public static T BytesToStruct<T>(byte[] bytes, int startIndex, int length)
        {
            if (bytes == null) return default(T);
            if (bytes.Length <= 0) return default(T);
            IntPtr buffer = Marshal.AllocHGlobal(length);
            try//struct_bytes转换
            {
                Marshal.Copy(bytes, startIndex, buffer, length);
                return (T)Marshal.PtrToStructure(buffer, typeof(T));
            }
            catch (Exception ex)
            {
                throw new Exception("Error in BytesToStruct ! " + ex.Message);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        public static byte[] StructToBytes(object structObj, int size)
        {
            byte[] bytes = new byte[size];
            IntPtr structPtr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(structObj, structPtr, false);
            Marshal.Copy(structPtr, bytes, 0, size);
            Marshal.FreeHGlobal(structPtr);
            return bytes;
        }

        public static object ByteToStruct(byte[] bytes, Type type)
        {
            int size = Marshal.SizeOf(type);
            if (size > bytes.Length)
            {
                return null;
            }
            IntPtr structPtr = Marshal.AllocHGlobal(size);
            Marshal.Copy(bytes, 0, structPtr, size);
            object obj = Marshal.PtrToStructure(structPtr, type);
            Marshal.FreeHGlobal(structPtr);
            return obj;
        }

        public enum MessageInfo
        {
            MSG_REQUEST_INFO = 0x100,
            MSG_ANSWER_INFO,
            MSG_RESTART,
            MSG_RESTART_ACK,
            MSG_RESTART_NACK,
            MSG_HEARTBEAT,
            MSG_DELFILE,
            MSG_DELFILE_ACK,
            MSG_DELFILE_NACK,
            MSG_UPLOADFILE,
            MSG_DOWNLOADFILE,
            MSG_UPDATE_OK = 0x10b,
            MSG_PROCESS_ERR,
            MSG_USER_DEF_MSG,
            USER_DEF_MSG_ACK,
            MSG_FPGA_ERASE = 0x200,
            MSG_ERASE_PROGRESS,
            MSG_WRITE_FPGA_FLASH,
            MSG_WRITE_UIMAGE,
            MSG_REQUEST_DATA,
            MSG_DOWNLOAD_DATA,
            MSG_READ_FPGA_FLASH,
            MSG_READ_UIMAGE,
            MSG_UPLOAD_DATA,
            MSG_CHECK_RESULT,
            MSG_CHECK_ACK,
            MSG_WRITE_FPGA_REG,
            MSG_WRITE_RCVR_FLASH,
            MSG_WRITE_RCVR_REG,
            MSG_READ_FPGA_REG,
            MSG_READ_RCVR_FLASH,
            MSG_READ_RCVR_REG,
            MSG_RCVR_ERASE,//0x211
            MSG_LCD_NUM,
            MSG_KILL_SSA_APP,
            MSG_KILL_SSA_APP_ACK,
            MSG_ACTRL_FPGA,
            MSG_ACTRL_FPGA_ACK,
            MSG_SET_IP,
            MSG_SET_IP_ACK
        }

        //device ID
        const ushort DEV_DVS = 0x01;
        const ushort DEV_ENC = 0x02;
        const ushort DEV_SW = 0x03;
        const ushort DEV_ACTRL = 0x04;
        const ushort DEV_IOM = 0x05;
        const ushort DEV_DACU = 0x06;
        const ushort DEV_HDLC = 0x07;
        const ushort DEV_TDVR = 0x08;
        const ushort DEV_MP = 0x09;
        const ushort DEV_ALLDEV = 0xFFFF;

        const ushort COMM_LEN = 8;
        const ushort INTER_LEN = 12;
        const ushort DEVINFO_LEN = 36;
        const ushort IP_LEN = 4;

        const ushort ACSU_CTRL_LEN = 284;
        const ushort ACSU_RESP_LEN = 34;
        const ushort ACSU_SYN_LEN = 24;

        const string SYN_BYTE = "\x55\xaa\x00\x01\x02\x04\x08\x10\x20\x40\x80\xff\xaa\x55\xff\xfe\xfd\xfb\xf7\xef\xdf\xbf\x7f\x00";

        const byte UPDATE = 0xF0;
        const byte WREN = 0x06;
        const byte SE = 0xD8;
        const byte RDSR = 0x05;
        const byte PP = 0x02;
        const byte REBOOT = 0xF5;
        const byte READ = 0x03;

        public struct COMM_HEAD
        {
            public ushort wLenth;
            public ushort wProtocolType;
            public byte dyCheckSum;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
            public string byRes;
        }

        public struct INTER_HEAD
        {
            public ushort wProjectNo;
            public ushort wProjectProtocol;
            public ushort wDeviceNo;
            public ushort byRes;
            public uint dwMsgType;
        }

        public enum Error_Code
        {
            ERROR_NO = 0,
            ERROR_WSASTARTUP_FAILED,
            ERROR_SOCKET_CREATE_FAILED,
            ERROR_SOCKET_BIND_FAILED,
            ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG,
            ERROR_RECV_NO_DATA,
            ERROR_RECV_UMSG_HEADERTYPE_WRONG,
            ERROR_DATA_SEND_FAILED,
            ERROR_DATA_RECV_FAILED,
            ERROR_LCDNUM_WRONG,
            ERROR_RECV_CHECKRESULT_WRONG,
            ERROR_SET_FPGA_TO_BOOT_FAILED,
            ERROR_SET_FPGA_TO_NORMAL_FAILED,
            ERROR_SET_RECEIVER_TO_BOOT_FAILED,
            ERROR_SET_RECEIVER_TO_NORMAL_FAILED,
            ERROR_ACSU_RESPPARSE_WRONG
        }

        public static Error_Code ssa_error_code = Error_Code.ERROR_NO;
        public MessageInfo m_messageInfo;
        public static string MIpAddr;

        public class DevInformation
        {
            int m_devcnt;
            Socket sockDTLMaint;//用于读版本工具
            Socket sockSW;
            CDevMsgParser SendExtractor = new CDevMsgParser();
            CDevMsgParser RecvExtractor = new CDevMsgParser();

            public int GetDevInformationNew(ushort dev, int t)
            {
                EndPoint DevMultiPollAddr = new IPEndPoint(IPAddress.Parse(UDP_ADDR), UDP_SENDDES);
                EndPoint fromaddr = new IPEndPoint(IPAddress.Any, 0);
                byte[] RecvBuf = new byte[2000];
                int nread;
                string info;
                DEV_INFORMATION DevInfo;
                SendExtractor.SetDevMsg(dev, (uint)MessageInfo.MSG_REQUEST_INFO);
                SendExtractor.OrgPacket(null, 0);
                try
                {
                    sockDTLMaint.SendTo(SendExtractor.GetPacket(), 0, SendExtractor.GetPacketSize(),
                        SocketFlags.None, DevMultiPollAddr);
                }
                catch (SocketException)
                {
                    ssa_error_code = Error_Code.ERROR_DATA_SEND_FAILED;
                    return 0;
                }

                while (true)
                {
                    sockDTLMaint.ReceiveTimeout = 2500;
                    try
                    {
                        nread = sockDTLMaint.ReceiveFrom(RecvBuf, 2000, SocketFlags.None, ref fromaddr);
                        if (nread > 0)
                        {
                            if (!RecvExtractor.Parser(RecvBuf, nread, (uint)MessageInfo.MSG_ANSWER_INFO))
                            {
                                ssa_error_code = Error_Code.ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
                                return 0;
                            }
                            byte[] ip = ((IPEndPoint) fromaddr).Address.GetAddressBytes();
                        }

                        //                           memcpy((char*)&DevInfo,(char*)RecvExtractor.GetData().c_str(),sizeof(DevInfo));
                        //			            unsigned char head[20];
                        //                           RecvExtractor.GetInterHead(head);
                        //			            info.append((char*)head,INTER_LEN);
                        //			            info.append((char*)ip,IP_LEN);


                        //                           memcpy((char*)&m_devinfo[t],(char*)info.c_str(),16);			

                        //                           RecvExtractor.StoreDevInfo(&DevInfo,&m_devinfo[t]);

                        //			            t++;

                        //		            } 
                    }
                    catch (SocketException excp)
                    {
                        break;
                    }
                }
                //            while (1)
                //            {	
                //                   FD_ZERO(&ReadSet);//每次循环都要清空，否则不能检测描述符变化
                //	            if (m_Type==0)
                //	            {
                //                       FD_SET(sockDTLMaint,&ReadSet);
                //	            }

                //	            TimeOut.tv_sec = 0;
                //	            TimeOut.tv_usec = 2500*1000;
                //	            result = select(0, &ReadSet, NULL, NULL, &TimeOut);
                //	            if(result > 0) 
                //	            {
                //		            info.clear();

                //		            if (m_Type==0)
                //		            {
                //			            nread = recvfrom(sockDTLMaint, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
                //		            }

                //		            
                //	            }
                //	            else
                //	            {
                //		            m_devcnt=t;
                //		            break;
                //	            }
                //            }

                ssa_error_code = Error_Code.ERROR_NO;
                return m_devcnt;
            }

            public bool InitSocket(ushort gen_prot, ushort pro_prot, ushort pro_protsw, ushort proID)
            {
                int nRecvBuf = 256 * 1024;//设置为32K

                try
                {
                    sockDTLMaint = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                }
                catch (SocketException)
                {
                    ssa_error_code = Error_Code.ERROR_SOCKET_CREATE_FAILED;
                    return false;
                }

                try
                {
                    sockDTLMaint.Bind(new IPEndPoint(IPAddress.Parse(MIpAddr), UDP_SENDSRC));
                }
                catch (SocketException)
                {
                    ssa_error_code = Error_Code.ERROR_SOCKET_BIND_FAILED;
                    return false;
                }

                sockDTLMaint.ReceiveBufferSize = nRecvBuf;

                try
                {
                    sockSW = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                }
                catch (SocketException)
                {
                    ssa_error_code = Error_Code.ERROR_SOCKET_CREATE_FAILED;
                    return false;
                }

                try
                {
                    sockSW.Bind(new IPEndPoint(IPAddress.Parse(MIpAddr), UDP_RECVDES));
                }
                catch (SocketException)
                {
                    ssa_error_code = Error_Code.ERROR_SOCKET_BIND_FAILED;
                    return false;
                }
                sockSW.ReceiveBufferSize = nRecvBuf;

                SendExtractor.SetProtNew(gen_prot, pro_prot, proID, pro_protsw);
                RecvExtractor.SetProtNew(gen_prot, pro_prot, proID, pro_protsw);

                ssa_error_code = Error_Code.ERROR_NO;
                return true;
            }
        }

        public class DevUpdate
        {
            Socket sockDTLUpdate;//用于更新工具
            IPEndPoint addrSrv;

            uint m_EraseProg;
            uint m_WriteProg;
            uint m_ReadProg;

            int m_IsuImageWrite;

            uint m_UploadProg;
            uint m_DownloadProg;
            uint m_ReadFileProg;

            byte m_LeftLCD;
            byte m_RightLCD;

            byte[] m_LeftLCDStatus = new byte[32];
            byte[] m_RightLCDStatus = new byte[32];
            byte[] m_LeftLCDVersion = new byte[32];
            byte[] m_RightLCDVersion = new byte[32];

            int m_IsOldVersion;//0:daemon<2.10,1:daemon>=2.10

            CDevMsgParser SendExtractor = new CDevMsgParser();
            CDevMsgParser RecvExtractor = new CDevMsgParser();

            public bool InitSocket(ushort gen_prot, ushort pro_prot, ushort pro_protsw, ushort proID)
            {
                addrSrv = new IPEndPoint(IPAddress.Parse("127.0.0.1"), TCP_PORT);

                SendExtractor.SetProtNew(gen_prot, pro_prot, proID, pro_protsw);
                RecvExtractor.SetProtNew(gen_prot, pro_prot, proID, pro_protsw);

                ssa_error_code = Error_Code.ERROR_NO;
                return true;
            }
        }

        public class CDevMsgParser
        {
            public CDevMsgParser()
            {
                m_DataCnt = 0;
            }

            public void InterHeader()
            {
                m_InterHeader.wProjectNo = m_ProjectNo;
                m_InterHeader.wProjectProtocol = m_ProjectProtocol;
                m_InterHeader.wDeviceNo = m_DeviceNo;
                m_InterHeader.byRes = 0;
                m_InterHeader.dwMsgType = m_MsgType;
            }
            public void CommHeader()
            {
                m_CommHeader.wLenth = (ushort)GetPacketSize();
                m_CommHeader.wProtocolType = m_ProtocolType;
                m_CommHeader.dyCheckSum = CheckSum();
                m_CommHeader.byRes = "SSA";
            }

            public void OrgPacket(string src, int cnt)
            {
                SetDataBuf(src, cnt);
                SetDataCnt(cnt);
                InterHeader();
                CommHeader();

                m_PacketMsg.Clear();
                m_PacketMsg.AddRange(StructToBytes(m_CommHeader, COMM_LEN));
                m_PacketMsg.AddRange(StructToBytes(m_InterHeader, INTER_LEN));
                if (m_DataBuffer != null) m_PacketMsg.AddRange(m_DataBuffer);
            }

            public void SetDataBuf(string src, int cnt)
            {
                if (src == null)
                {
                    m_DataBuffer.Clear();
                    return;
                }

                for (int i = 0; i < cnt; i++)
                {
                    m_DataBuffer.Add(Convert.ToByte(src[i]));
                }
            }

            public void GetInterHead(ref byte[] dest)
            {
                if (dest == null) throw new ArgumentNullException(nameof(dest));
                dest = StructToBytes(m_InterHeader, INTER_LEN);
            }

            public void SetProtNew(ushort gen_prot, ushort pro_prot, ushort proID, ushort pro_protsw)
            {
                m_ProtocolType = gen_prot;
                m_ProjectProtocol = pro_prot;
                m_ProjectNo = proID;
                m_ProjectProtocolSw = pro_protsw;
            }

            public byte CheckSum()
            {
                byte[] header = StructToBytes(m_InterHeader, INTER_LEN);
                var checksum = header[0];
                for (uint i = 1; i < header.Length; i++)
                {
                    checksum ^= header[i];
                }

                for (int i = 0; i < m_DataBuffer.Count; i++)
                {
                    checksum ^= m_DataBuffer[i];
                }

                return checksum;
            }

            public bool Parser(byte[] buf, int cnt, uint msg)
            {
                m_CommHeader = BytesToStruct<COMM_HEAD>(buf, 0, COMM_LEN);
                m_InterHeader = BytesToStruct<INTER_HEAD>(buf, COMM_LEN, INTER_LEN);
                m_DataBuffer.Clear();
                byte[] tmp = new byte[cnt - (COMM_LEN + INTER_LEN)];
                Array.Copy(tmp, 0, buf, COMM_LEN + INTER_LEN, cnt - (COMM_LEN + INTER_LEN));
                m_DataBuffer.AddRange(tmp);
                byte check = CheckSum();

                if (check != m_CommHeader.dyCheckSum)
                {
                    return false;
                }
                if (msg != GetMsgType())
                {
                    return false;
                }

                return true;
            }

            public int GetPacketSize()
            {
                return m_DataCnt + COMM_LEN + INTER_LEN;
            }

            public byte[] GetPacket()
            {
                return m_PacketMsg.ToArray();
            }

            public void SetDevMsg(ushort dev, uint msg)
            {
                m_DeviceNo = dev;
                m_MsgType = msg;
            }

            public uint GetMsgType()
            {
                return m_InterHeader.dwMsgType;
            }

            public void SetDataCnt(int cnt)
            {
                m_DataCnt = cnt;
            }

            private COMM_HEAD m_CommHeader;
            private INTER_HEAD m_InterHeader;

            private ushort m_ProtocolType;
            private ushort m_ProjectProtocol;
            private ushort m_ProjectProtocolSw;
            private ushort m_ProjectNo;
            private ushort m_DeviceNo;
            private uint m_MsgType;

            private List<byte> m_DataBuffer = new List<byte>();
            private int m_DataCnt;
            private List<byte> m_PacketMsg = new List<byte>();
        }

        public class DevUmsgParser
        {
        }

        CDevMsgParser SendExtractor = new CDevMsgParser();
        CDevMsgParser RecvExtractor = new CDevMsgParser();
        DevUmsgParser SendUmsgcommand = new DevUmsgParser();
        DevUmsgParser RecvUmsgcommand = new DevUmsgParser();
        DevInformation m_DevInformation = new DevInformation();
        DevUpdate m_Update = new DevUpdate();

        public int GetDevInformation(ushort dev, int t)
        {
            int result = 0;
            return result;
        }

        public int GetSWInformationNew(ushort dev, int t)
        {
            return 0;
        }

        public bool SSADevDllInit(ushort gen_prot, ushort pro_prot, ushort pro_protsw, ushort proID, string adapterinfo)
        {
            MIpAddr = adapterinfo;
            bool result = m_Update.InitSocket(gen_prot, pro_prot, pro_protsw, proID);
            if (!result)
            {
                return false;
            }
            result = m_DevInformation.InitSocket(gen_prot, pro_prot, pro_protsw, proID);
            if (!result)
            {
                return false;
            }
            return true;
        }
    }
}
