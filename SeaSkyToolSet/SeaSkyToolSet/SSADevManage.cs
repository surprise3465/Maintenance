using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

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

        enum MessageInfo
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
            MSG_SET_IP_ACK,
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
        const byte WREN= 0x06;
        const byte SE= 0xD8;
        const byte RDSR= 0x05;
        const byte PP= 0x02;
        const byte REBOOT= 0xF5;
        const byte READ= 0x03;

        public static byte[] StructToBytes(object structObj, int size)
        {
            byte[] bytes = new byte[size];
            IntPtr structPtr = Marshal.AllocHGlobal(size);
            //将结构体拷到分配好的内存空间
            Marshal.StructureToPtr(structObj, structPtr, false);
            //从内存空间拷贝到byte 数组
            Marshal.Copy(structPtr, bytes, 0, size);
            //释放内存空间
            Marshal.FreeHGlobal(structPtr);
            return bytes;

        }

        //将Byte转换为结构体类型
        public static object ByteToStruct(byte[] bytes, Type type)
        {
            int size = Marshal.SizeOf(type);
            if (size > bytes.Length)
            {
                return null;
            }
            //分配结构体内存空间
            IntPtr structPtr = Marshal.AllocHGlobal(size);
            //将byte数组拷贝到分配好的内存空间
            Marshal.Copy(bytes, 0, structPtr, size);
            //将内存空间转换为目标结构体
            object obj = Marshal.PtrToStructure(structPtr, type);
            //释放内存空间
            Marshal.FreeHGlobal(structPtr);
            return obj;
        }

        [StructLayoutAttribute(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
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

        struct DEV_INFORMATION
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

        };

        public struct COMM_HEAD
        {
            ushort wLenth;
            ushort wProtocolType;
            byte dyCheckSum;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
            string byRes;
        };

        struct INTER_HEAD
        {
            ushort wProjectNo;
            ushort wProjectProtocol;
            ushort wDeviceNo;
            ushort byRes;
            uint dwMsgType;
        };

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
            ERROR_ACSU_RESPPARSE_WRONG,
        }

        public Error_Code ssa_error_code = Error_Code.ERROR_NO;

        Socket sockDTLMaint;//用于读版本工具
        Socket sockSW;
        Socket sockDTLUpdate;//用于更新工具
        EndPoint addrSrv;

        int m_devcnt;

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

        public class CDevMsgParser
        {
            public CDevMsgParser()
            {
                m_DataCnt = 0;
            }

            public byte CheckSum()
            {
                byte checksum;
                byte[] header = StructToBytes(m_InterHeader, INTER_LEN);
                checksum = header[0];
                for (uint i = 1; i < header.Length; i++)
                {
                    checksum ^= header[i];
                }
                byte[] buffer = Encoding.ASCII.GetBytes(m_DataBuffer);
                for (uint i = 0; i < buffer.Length; i++)
                {
                    checksum ^= buffer[i];
                }

                return checksum;
            }

            private COMM_HEAD m_CommHeader;
            private INTER_HEAD m_InterHeader;

            private ushort m_ProtocolType;
            private ushort m_ProjectProtocol;
            private ushort m_ProjectProtocolSw;

            private ushort m_ProjectNo;
            private ushort m_DeviceNo;
            private uint m_MsgType;

            private string m_DataBuffer;
            private uint m_DataCnt;
            private string m_PacketMsg;
        }

        public class DevUmsgParser
        {
            public DevUmsgParser()
            {

            }
        }

        CDevMsgParser SendExtractor = new CDevMsgParser();
        CDevMsgParser RecvExtractor = new CDevMsgParser(); 
        DevUmsgParser SendUmsgcommand = new DevUmsgParser();
        DevUmsgParser RecvUmsgcommand = new DevUmsgParser();

        public bool InitSocket(string str)
        {
            return true;
        }

        public int GetDevInformation(ushort dev, int t)
        {
            return 0;
        }

        public int GetSWInformationNew(ushort dev, int t)
        {
            return 0;
        }
    }
}
