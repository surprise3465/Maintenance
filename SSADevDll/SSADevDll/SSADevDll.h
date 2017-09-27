// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SSADEVDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SSADEVDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SSADEVDLL_EXPORTS
#define SSADEVDLL_API __declspec(dllexport)
#else
#define SSADEVDLL_API __declspec(dllimport)
#endif

#include <string>
using namespace std;
//data type

typedef struct _dev_informa
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

}DEV_INFOR,*DEV_INFORMA;

class SSADevDll
{
public:		
	int SSADevDllInit(unsigned short gen_prot,unsigned short pro_prot,unsigned short proID,int type,char* adapterinfo=NULL);//type=0:初始化sockDTLMaint，用于读版本工具；type=1:初始化sockDTLUpdate，用于更新工具；
	static int SSADevDllInitNew(unsigned short gen_prot,unsigned short pro_prot,unsigned short pro_protsw,unsigned short proID,int type,char* adapterinfo=NULL);
	void SSADevDllClose();


	int EraseFPGA(unsigned int dev,unsigned char *ip,unsigned char start,unsigned char end);
	int EraseRCVR(unsigned int dev,unsigned char *ip,unsigned char start,unsigned char end);

	unsigned char GetEraseProg();

	int WriteFPGAFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt);
	int WriteFPGAReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt);
	int WriteRCVRFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt);
	int WriteRCVRReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt);
	int WriteUimage(unsigned short dev,unsigned char *ip,unsigned char *data,unsigned int cnt);
	int SetLCDNum(unsigned short dev,unsigned char left,unsigned char right,unsigned char *ip);
	int GetLCDNumFromFlash(WORD dev,unsigned char *ip,unsigned char& left,unsigned char& right);
	unsigned char GetWriteProg();

	int ReadFPGAFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest);
	int ReadTDVRFPGAFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest);
	int ReadFPGAReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest);
	int ReadRCVRFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest);
	int ReadRCVRReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest);
	int ReadUimage(unsigned short dev,unsigned char *ip,unsigned int cnt,unsigned char *dest);
	unsigned char GetReadProg();

	void ResetProg();

	int CheckResult(unsigned short dev,unsigned char *ip,unsigned char res);


	int SetFPGAToBoot(unsigned short dev,unsigned char *ip);
	int SetTDVRFPGAToBoot(unsigned short dev,unsigned char *ip);
	int SetFPGAToNormal(unsigned short dev,unsigned char *ip);
	int SetTDVRFPGAToNormal(unsigned short dev,unsigned char *ip);
	int GetFPGAMode(unsigned short dev,unsigned char *ip);
	int GetTDVRFPGAMode(unsigned short dev,unsigned char *ip);
	int GetFPGABig(unsigned short dev,unsigned char *ip,DWORD addr);

	int SetRCVRToBoot(unsigned short dev,unsigned char *ip);
	int SetRCVRToNormal(unsigned short dev,unsigned char *ip);

	int GetLCDStatus(unsigned short dev,unsigned char *ip,int pos,unsigned char *dest);
	int GetLCDVersion(unsigned short dev,unsigned char *ip,int pos,unsigned char *dest);

	int ReStart(unsigned short dev,unsigned char *ip);
	int HeartBeat(unsigned short dev,unsigned char *ip);

	int KillApp(unsigned short dev,unsigned char *ip);

	int Delete_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,int &success);
	int Upload_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,unsigned short fileauthority,unsigned char *data,unsigned int cnt,unsigned int addr=0x0000);
	int Read_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,unsigned int cnt,unsigned char *dest); 

	int Download_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,string& dest,unsigned int& cnt); 
	int Download_File_Big(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt);
	int Download_File_BigNew(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt);
	int Download_UBLFile(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,string &info,unsigned int& cnt);


	unsigned char GetUploadProg();
	unsigned char GetDownloadProg();
	unsigned char GetReadFileProg();
	unsigned int  GetIsuImageWrite();
	void ResetFileProg();

	int GetAcsuMode(unsigned short dev,unsigned char *ip);//对于大同项目读模式也使用
	int SetAcsuToBoot(unsigned short dev,unsigned char *ip);
	int SetAcsuToNormal(unsigned short dev,unsigned char *ip);//对于大同项目切到正常模式也使用
	int ReadAcsuFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest);
	int EraseAcsuFlash(unsigned short dev,unsigned char *ip,unsigned int start_sec,unsigned int end_sec);
	int WriteAcsuFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt);

	int GetDevInfo(unsigned int dev,string& info);
	int SetDevIP(unsigned int dev,unsigned char *info,unsigned int cnt);//目前只有无线路由支持配IP功能；
	int GetDevInfo_Ex(unsigned int dev,DEV_INFOR *devinfo);
	void SetVersionInfo(int info);


	int CustomCommandControl(unsigned int dev,unsigned int macid,unsigned short headertype,string& msgdata,string& comstring,string& recvstring,int timeout=5,unsigned int devip=((234<<24)+(4<<16)+(5<<8)+6));//用于自定义消息,注意：string如果不用引用，会在dll中重新分配内存，则返回时会触发断点 timeout为超时秒数

	int GetAdapterInfoList(char* list);//获得本机网卡列表信息
	int GetErrorCode();

	int NewUpdateMessage(string &info);
	void Devrestart368();
};

namespace wrapper
{
	public ref class ManagedClass 
	{
	public:
		// Allocate the native object on the C++ Heap via a constructor
		ManagedClass() : m_Impl( new SSADevDll ) {}

		// Deallocate the native object on a destructor
		~ManagedClass() {
			delete m_Impl;
		}

	protected:
		// Deallocate the native object on the finalizer just in case no destructor is called
		!ManagedClass() {
			delete m_Impl;
		}

	public:
		int SSADevDllInit(unsigned short gen_prot,unsigned short pro_prot,unsigned short pro_protsw,unsigned short proID,int type,char* adapterinfo)
		{
			return m_Impl->SSADevDllInitNew(gen_prot,pro_prot,pro_protsw,proID,type,adapterinfo);
		}

		void SSADevDllClose()
		{
			m_Impl->SSADevDllClose();
			return;
		}

		int GetDevInfo_Ex(unsigned int dev, DEV_INFOR* info)
		{
			return m_Impl->GetDevInfo_Ex(dev, info);
		}

		int CustomCommandControl(unsigned int dev, unsigned int macid, unsigned short headertype, string& msgdata, string& comstring, string& recvstring, int timeout, unsigned int devip)
		{
			return m_Impl->CustomCommandControl(dev, macid, headertype, msgdata, comstring, recvstring, timeout, devip);
		}

		int GetErrorCode()
		{
			return m_Impl->GetErrorCode();
		}

		int ReStart(unsigned short dev, unsigned char *ip)
		{
			return m_Impl->ReStart(dev, ip);
		}

	private:
		SSADevDll* m_Impl;
	};
}

//ssa_error_code 错误宏
#define ERROR_NO   0
#define ERROR_WSASTARTUP_FAILED					1//调用WSASTARTUP函数失败
#define ERROR_SOCKET_CREATE_FAILED				2//调用Create函数创建socket失败
#define ERROR_SOCKET_BIND_FAILED				3//调用bind函数绑定socket失败
#define ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG	4//接收到数据，但是校验或者消息类型错误
#define ERROR_RECV_NO_DATA						5//没有接收到任何数据
#define ERROR_RECV_UMSG_HEADERTYPE_WRONG		6//对于接收到的自定义消息，消息类型错误
#define ERROR_DATA_SEND_FAILED					7//调用send函数失败
#define ERROR_DATA_RECV_FAILED					8//调用recv函数失败
#define ERROR_LCDNUM_WRONG						9//调用CheckLCDNUM函数检查LCD数量，LCD数量错误，不符合要求
#define ERROR_RECV_CHECKRESULT_WRONG			10//调用CheckResult函数时，收到的CheckResult与给定的不一样
#define ERROR_SET_FPGA_TO_BOOT_FAILED			11//将FPGA切换到BootLoader模式失败
#define ERROR_SET_FPGA_TO_NORMAL_FAILED			12//将FPGA切换到Normal模式失败
#define ERROR_SET_RECEIVER_TO_BOOT_FAILED		13//将接收卡切换到Bootloader模式失败
#define ERROR_SET_RECEIVER_TO_NORMAL_FAILED		14//将接收卡切换到Normal模式失败
#define ERROR_ACSU_RESPPARSE_WRONG				15//调用AcsuRespParse函数，获得的返回值与比较值不同
