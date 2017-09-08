#ifndef _CDEF_H_
#define _CDEF_H_

#include <Winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
using namespace std;
#pragma comment(lib,"ws2_32.lib")


#define TCP_PORT             20050
#define UDP_ADDR             "234.4.5.6"
#define UDP_SENDSRC          30050
#define UDP_SENDDES          30040
#define UDP_RECVSRC          30055
#define UDP_RECVDES          30045
#define UDP_RECVDES_UPDATE   30060


//message type
#define MSG_REQUEST_INFO         0x100
#define MSG_ANSWER_INFO          0x101
#define MSG_RESTART              0x102
#define MSG_RESTART_ACK          0x103
#define MSG_RESTART_NACK         0x104
#define MSG_HEARTBEAT            0x105

#define MSG_UPDATE_OK            0x10B
#define MSG_PROCESS_ERR          0x10C

#define MSG_USER_DEF_MSG           0x10D
#define MSG_USER_DEF_MSG_ACK       0x10E

#define MSG_FPGA_ERASE           0x200
#define MSG_RCVR_ERASE           0x211
#define MSG_ERASE_PROGRESS       0x201

#define MSG_WRITE_FPGA_FLASH     0x202
#define MSG_WRITE_FPGA_REG       0x20B
#define MSG_WRITE_RCVR_FLASH     0x20C
#define MSG_WRITE_RCVR_REG       0x20D
#define MSG_WRITE_UIMAGE         0x203
#define MSG_REQUEST_DATA         0x204
#define MSG_DOWNLOAD_DATA        0x205

#define MSG_READ_FPGA_FLASH      0x206
#define MSG_READ_FPGA_REG        0x20E
#define MSG_READ_RCVR_FLASH      0x20F
#define MSG_READ_RCVR_REG        0x210
#define MSG_READ_UIMAGE          0x207
#define MSG_UPLOAD_DATA          0x208

#define MSG_CHECK_RESULT         0x209
#define MSG_CHECK_ACK            0x20A

#define MSG_SET_IP               0x217
#define MSG_SET_IP_ACK           0x218

#define MSG_LCD_NUM              0x212

#define MSG_KILL_SSA_APP         0x213
#define MSG_KILL_SSA_APP_ACK     0x214

#define MSG_ACTRL_FPGA           0x215
#define MSG_ACTRL_FPGA_ACK       0x216

#define MSG_DELFILE              0x106
#define MSG_DELFILE_ACK          0x107
#define MSG_DELFILE_NACK         0x108
#define MSG_UPLOADFILE           0x109
#define MSG_DOWNLOADFILE         0x10A

//project ID
#define PRO_BXEMU            0x08

//device ID
#define DEV_DVS              0x01
#define DEV_ENC              0x02
#define DEV_SW               0x03
#define DEV_ACTRL            0x04
#define DEV_IOM              0x05
#define DEV_DACU             0x06
#define DEV_HDLC             0x07
#define DEV_TDVR             0x08
#define DEV_MP               0x09
#define DEV_ALLDEV           0xFFFF


#define COMM_LEN       8
#define INTER_LEN      12
#define DEVINFO_LEN    36
#define IP_LEN      4

#define ACSU_CTRL_LEN  284
#define ACSU_RESP_LEN  34
#define ACSU_SYN_LEN   24

#define SYN_BYTE  "\x55\xaa\x00\x01\x02\x04\x08\x10\x20\x40\x80\xff\xaa\x55\xff\xfe\xfd\xfb\xf7\xef\xdf\xbf\x7f\x00"

#define UPDATE  0xF0
#define WREN    0x06
#define SE      0xD8
#define RDSR    0x05
#define PP      0x02
#define REBOOT  0xF5
#define READ    0x03

struct  COMM_HEAD
{
	WORD wLenth;
	WORD wProtocolType;
	BYTE dyCheckSum;
	BYTE byRes[3];
};

struct INTER_HEAD 
{
	WORD wProjectNo;
	WORD wProjectProtocol;
	WORD wDeviceNo;
	BYTE byRes[2];
	DWORD dwMsgType;

};

struct ERASE_COMM 
{
	BYTE byFirstSection;
	BYTE byLastSection;
	BYTE byRes[2];
};

struct ERASE_PROG 
{
	BYTE byProgress;
	BYTE byRes[3];
};

struct WRITE_DATA 
{
	DWORD dwPacketNo;
	DWORD dwStartAddr;
	WORD wDataLength;
	BYTE byRes[2];
	BYTE byLastPacket;
	BYTE byRes2[3];
	BYTE byFileData[1024];
};

struct LCD_NUM 
{
	BYTE byLeft;
	BYTE byRight;
	BYTE byRes[2];
};

struct READ_DATA
{
	DWORD dwStartAddr;
	WORD wDataLength;
	BYTE byRes[2];
};

struct UPLOAD_DATA 
{
	DWORD dwStartAddr;
	WORD wDataLength;
	BYTE byRes[2];
	BYTE byFileData[1024];
};

struct CHECK_RESULT 
{
	BYTE byCheckResult;
	BYTE byRes[3];
};

struct CHECK_CONFIRM 
{
	BYTE byCheckConfirm;
	BYTE byRes[3];
};

struct DEV_INFORMATION
{
	BYTE byMAC[6];
	BYTE byRes[2];
	DWORD byMask;
	WORD byFpgaB;
	WORD byFpgaA;
	WORD byUboot;
	WORD byUimage;
	WORD byFileSys;
	WORD byApp;
	WORD byDaemon;
	WORD byHardVer;
	BYTE byRes2[8];
	//string byaddition;
    BYTE byinfo[200];

};

typedef struct _dev
{
	WORD byProjectno;
	WORD byprotol;
	WORD bydevno;
	BYTE byRes3[2];
    DWORD message;
    BYTE byIp[4];
	BYTE byMAC[6];
	BYTE byRes[2];
	DWORD byMask;
	WORD byFpgaB;
	WORD byFpgaA;
	WORD byUboot;
	WORD byUimage;
	WORD byFileSys;
	WORD byApp;
	WORD byDaemon;
	WORD byHardVer;
	BYTE byRes2[8];
	BYTE proname[16];
	BYTE devicename[16];
	BYTE APP[32];
	BYTE Sys_MP[32];
	BYTE sn[21];
	BYTE otherinf[32];

}DEV,*DEV_MA;
//
struct UPLOAD_FILE
{
	BYTE byFileName[256];
	WORD byFileAuthority;
	BYTE byRes[2];
};

struct DOWNLOAD_FILE
{
	DWORD dwStartAddr;
	WORD wDataLength;
	BYTE byRes[2];
	BYTE byfilename[256];
};

struct UPLOAD_FILE_DATA 
{
	DWORD dwStartAddr;
	WORD wDataLength;
	BYTE byLastPackage;
	BYTE byUpdateProg;
	BYTE byFileData[1024];
};

struct ACSU_CTRL 
{
	unsigned char bySynData[24];
	unsigned char byComm;
	unsigned char byAddr[3];
	unsigned char byData[256];
};

struct ACSU_CTRL_COMM 
{
	unsigned int dwPacketNo;
	unsigned char byRes[4];
	unsigned short wDataLength;
	unsigned char byRes2[2];
	unsigned char byLastPackage;
	unsigned char byRes3[3];
	ACSU_CTRL byData;
};

struct ACSU_RESP 
{
	unsigned char byStart;
	unsigned char byComm;
	unsigned char byData[32];
};
struct ACSU_RESP_COMM 
{
	unsigned int dwPacketNo;
	unsigned short wDataLength;
	unsigned char byRes[2];
	ACSU_RESP byData;

};

struct UMSG_HEADER
{
	unsigned int dwMac_id;
	unsigned char byMark[4];
	unsigned short wLength;
	unsigned short wHeadertype;
	unsigned char byRes1[4];
	unsigned char byComstring[16];
	unsigned char byRes2[32];
};

struct UMSG_MSGDATA
{
	unsigned char byData[960];
};

typedef   struct                     
{ 
	char   szIPAddrStr[16];                         
    int    sign;
}NOTE_IP;

typedef   struct                     
{ 
	NOTE_IP note_ip[5];
}PSINFO_IP; 

typedef   struct   _SADAPTERINFO                     
{ 
	char   szDeviceName[128];                       //   名字 
	char   szHWAddrStr[18];                           //   MAC 
	DWORD   dwIndex;                                       //   编号 
	int    ipnum;
    PSINFO_IP  info_ip;
}SINFO_ADAPTER,   *PSINFO_ADAPTER; 

extern int ssa_error_code;
//ssa_error_code 错误宏
#define ERROR_NO   0
#define ERROR_WSASTARTUP_FAILED  1//调用WSASTARTUP函数失败
#define ERROR_SOCKET_CREATE_FAILED 2//调用Create函数创建socket失败
#define ERROR_SOCKET_BIND_FAILED   3//调用bind函数绑定socket失败
#define ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG  4//接收到数据，但是校验或者消息类型错误
#define ERROR_RECV_NO_DATA                     5//没有接收到任何数据
#define ERROR_RECV_UMSG_HEADERTYPE_WRONG       6//对于接收到的自定义消息，消息类型错误
#define ERROR_DATA_SEND_FAILED                 7//调用send函数失败
#define ERROR_DATA_RECV_FAILED                 8//调用recv函数失败
#define ERROR_LCDNUM_WRONG                     9//调用CheckLCDNUM函数检查LCD数量，LCD数量错误，不符合要求
#define ERROR_RECV_CHECKRESULT_WRONG           10//调用CheckResult函数时，收到的CheckResult与给定的不一样
#define ERROR_SET_FPGA_TO_BOOT_FAILED          11//将FPGA切换到BootLoader模式失败
#define ERROR_SET_FPGA_TO_NORMAL_FAILED        12//将FPGA切换到Normal模式失败
#define ERROR_SET_RECEIVER_TO_BOOT_FAILED      13//将接收卡切换到Bootloader模式失败
#define ERROR_SET_RECEIVER_TO_NORMAL_FAILED    14//将接收卡切换到Normal模式失败
#define ERROR_ACSU_RESPPARSE_WRONG             15//调用AcsuRespParse函数，获得的返回值与比较值不同
#endif