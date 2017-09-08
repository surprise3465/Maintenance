#ifndef _DEVUPDATE_H_
#define _DEVUPDATE_H_


#include "cdef.h"
#include "DevMsgParser.h"

#define START_SEC   4
#define LAST_SEC    7

#define LAST_PAC    1
#define PACKET      1024
#define RCVRP       32
#define ACSUP    256

#define CHKOK       'Y'
#define CHKERR      'N'

#define MODE_BOOT   1
#define MODE_NORMAL 2
#define MODE_NOFPGA 3

#define TIMEOUT     15
#define TIMEOUTUS     30000

#define FILENAME    256

#define LEFT_LCD    0
#define RIGHT_LCD   1

#define FPGA_ACK    1
#define COMM_ACK    2

class CDevUpdate
{
public:
	CDevUpdate();
	~CDevUpdate();

	BOOL DevUpdateInit(WORD gen_prot,WORD pro_prot,WORD proID);
	BOOL DevUpdateInitNew(WORD gen_prot,WORD pro_prot,WORD pro_protsw,WORD proID);

	void DevUpdateClose();

	void SetAddrFromIP(unsigned char *ip);

	BOOL EraseFPGA(WORD dev,unsigned char *ip,BYTE start,BYTE end);
	BOOL EraseRCVR(WORD dev,unsigned char *ip,BYTE start,BYTE end);

	BOOL WriteFlash(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);
	BOOL WriteReg(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);

	BOOL WriteFPGAFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);//  Ҫд������dataһ�𴫹������ּ���д��address��
	BOOL WriteFPGAReg(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);//  Ҫд������dataһ�𴫹������ּ���д��address��

	BOOL WriteRCVRFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);//  Ҫд������dataһ�𴫹������ּ���д��address��
	BOOL WriteRCVRReg(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);//  Ҫд������dataһ�𴫹������ּ���д��address��

	BOOL WriteUimage(WORD dev,unsigned char *ip,unsigned char *data,unsigned int cnt);//  Ҫд������dataһ�𴫹������ּ���д��address��
	
	BOOL CheckLCDNUM();//��������32��
	BOOL SetLCDNum(WORD dev,BYTE left,BYTE right,unsigned char *ip);
	BOOL GetLCDNumFromFlash(WORD dev,unsigned char *ip);
	BYTE GetLeftLCDNum();
	BYTE GetRightLCDNum();

	BYTE GetEraseProg();
	BYTE GetWriteProg();
	BYTE GetReadProg();
	void ResetProg();

	BOOL ReadFlash(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	BOOL ReadTDVRFlash(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	BOOL ReadRCVR(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	//�Ĵ���һ��ֻ�ܶ�һ���ֽڣ���
	int ReadReg(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);

	BOOL ReadFPGAFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	BOOL ReadTDVRFPGAFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	int ReadFPGAReg(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);

	BOOL ReadRCVRFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	BOOL ReadRCVRReg(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);

	BOOL ReadUimage(WORD dev,unsigned char *ip,unsigned int cnt,unsigned char *dest);//addr����ʼ��ַ����ʼ����ţ�cnt�Ƕ����������浽dest

	BOOL CheckResult(WORD dev,unsigned char *ip,BYTE res);

	BOOL ReStart(WORD dev,unsigned char *ip);

	BOOL HeartBeat(WORD dev,unsigned char *ip);

	BOOL KillSSAApp(WORD dev,unsigned char *ip);

	//��ģʽ���ο���DECODER����ģʽIIC�����ĵ���
	int GetFPGAMode(WORD dev,unsigned char *ip);//1-boot;2-normal;0-error
    int GetTDVRFPGAMode(WORD dev,unsigned char *ip);//1-boot;2-normal;0-error
    int GetFPGABig(WORD dev,unsigned char *ip,DWORD addr);//1-boot;2-normal;0-error
	//����������LCD����
	BOOL LCDStatusFromIIC(WORD dev,unsigned char *ip,int pos);
	BOOL LCDSVerFromIIC(WORD dev,unsigned char *ip,int pos);
	void GetLCDStatus(int pos,unsigned char *dest);//pos-left=0;right=1
	void GetLCDVersion(int pos,unsigned char *dest);


	//��ģʽ���ο���DECODERά������Э�顷
	BOOL SetFPGAToBoot(WORD dev,unsigned char *ip);
    BOOL SetTDVRFPGAToBoot(WORD dev,unsigned char *ip);
	BOOL SetFPGAToNormal(WORD dev,unsigned char *ip);
	BOOL SetTDVRFPGAToNormal(WORD dev,unsigned char *ip);
	BOOL SetRCVRToBoot(WORD dev,unsigned char *ip);
	BOOL SetRCVRToNormal(WORD dev,unsigned char *ip);

	//ɾ�����ϴ��������ļ�
	BOOL Delete_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,BOOL &success);
	BOOL Upload_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,WORD fileauthority,unsigned char *data,unsigned int cnt,DWORD addr);
	BOOL Read_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,unsigned int cnt,unsigned char *dest);

	BOOL Download_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,string dest,unsigned int& cnt);
    BOOL Download_File_Big(WORD dev,unsigned char *ip,unsigned char*filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt);
	BOOL Download_File_BigNew(WORD dev,unsigned char *ip,unsigned char*filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt);
	BOOL Download_UBLFile(WORD dev,unsigned char *ip,unsigned char*filename,unsigned int filenamelen,string& info,unsigned int& cnt);

	BYTE GetUploadProg();
	BYTE GetDownloadProg();
	BYTE GetReadFileProg();
	unsigned int GetIsuImageWrite();
	void ResetFileProg();

	//acsu update
	BOOL AcsuCommPack(unsigned char comm,unsigned int addr,unsigned short cnt,unsigned char *data,ACSU_CTRL *dest);
	int  AcsuRespParse(ACSU_RESP data);//0=err;1=fpga;2=comment
	BOOL CheckASW(SOCKET sock=0);
	int  GetAcsuMode(WORD dev,unsigned char *ip);//err=0;normal=2;boot=1
	BOOL SetAcsuToBoot(WORD dev,unsigned char *ip);

	BOOL SetAcsuToNormal(WORD dev,unsigned char *ip);
	BOOL EraseAcsuFlash(WORD dev,unsigned char *ip,DWORD start_sec,DWORD end_sec);
	BOOL ReadAcsuFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest);
	BOOL WriteAcsuFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt);

	//���¼���
	void SetVersionInfo(int info);
		
private:

	SOCKET sockClient;
	SOCKADDR_IN addrSrv;

	CDevMsgParser SendExtractor;
	CDevMsgParser RecvExtractor;

	unsigned int m_EraseProg;
	unsigned int m_WriteProg;
	unsigned int m_ReadProg;

	int m_IsuImageWrite;

	unsigned int m_UploadProg;
	unsigned int m_DownloadProg;
	unsigned int m_ReadFileProg;

	BYTE m_LeftLCD;
	BYTE m_RightLCD;

	unsigned char m_LeftLCDStatus[32];
	unsigned char m_RightLCDStatus[32];

	unsigned char m_LeftLCDVersion[32];
	unsigned char m_RightLCDVersion[32];

	int m_IsOldVersion;//0:daemon<2.10,1:daemon>=2.10

	
};

#endif