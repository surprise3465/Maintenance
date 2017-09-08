// SSADevDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SSADevDll.h"
#include "DevInformation.h"
#include "DevUpdate.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

CDevInformation m_DevInfo;
CDevUpdate m_DevUpdate;

int SSADevDll::SSADevDllInit(unsigned short gen_prot,unsigned short pro_prot,unsigned short proID,int type,char* adapterinfo /*=NULL*/)
{
	PSINFO_ADAPTER adapterinfotmp=(PSINFO_ADAPTER) adapterinfo;
	int result;
	result=m_DevUpdate.DevUpdateInit(gen_prot,pro_prot,proID);
	if (!result)
	{
		return result;
	}
	result=m_DevInfo.DevInfoInit(gen_prot,pro_prot,proID,type,adapterinfotmp);
	if (!result)
	{
		return result;
	}
	return result;

}

int SSADevDll:: SSADevDllInitNew(unsigned short gen_prot,unsigned short pro_prot,unsigned short pro_protsw,unsigned short proID,int type,char* adapterinfo /*=NULL*/)
{
	PSINFO_ADAPTER adapterinfotmp=(PSINFO_ADAPTER) adapterinfo;
	int result;
	//result=m_DevUpdate.DevUpdateInit(gen_prot,pro_prot,proID);
    result=m_DevUpdate.DevUpdateInitNew(gen_prot,pro_prot,pro_protsw,proID);
	if (!result)
	{
		return result;
	}
	result=m_DevInfo.DevInfoInitNew(gen_prot,pro_prot,pro_protsw,proID,type,adapterinfotmp);
	if (!result)
	{
		return result;
	}
	return result;

}

void SSADevDll::SSADevDllClose()
{
	m_DevUpdate.DevUpdateClose();
	m_DevInfo.DevInfoClose();
}

//device information
int SSADevDll::GetDevInfo(unsigned int dev,string& info)
{
	int result;	
	if (dev!=DEV_ALLDEV)
	{
		if (dev==DEV_SW)
		{			
			result=m_DevInfo.GetSWInformation(dev,info);
		}
		else
		{
			result=m_DevInfo.GetDevInformation(dev,info);
		}
	}
	else
	{
		result=m_DevInfo.GetDevInformation(dev,info);
		result=m_DevInfo.GetSWInformation(DEV_SW,info);
	}
	return result;
}

int SSADevDll:: SetDevIP(unsigned int dev,unsigned char *info,unsigned int cnt)
{
	int result;
	result=m_DevInfo.SetDevIP(dev,info,cnt);
	return result;
}

int SSADevDll:: GetDevInfo_Ex(unsigned int dev,DEV_INFOR *devinfo)
{
	m_DevInfo.m_devinfo=(DEV*)devinfo; 
	m_DevInfo.m_devcnt=0;
	m_DevInfo.ClearSocketRecv();

	int result;	
	int t=0;
	if (dev!=DEV_ALLDEV)
	{
		if (dev==DEV_SW)
		{			
			result=m_DevInfo.GetSWInformationNew(dev,t);
		}
		else
		{
			result=m_DevInfo.GetDevInformationNew(dev,t);
		}
	}
	else
	{
		result=m_DevInfo.GetDevInformationNew(dev,t);
		result=m_DevInfo.GetSWInformationNew(DEV_SW,result);
	}
	//return result;

    m_DevInfo.m_devinfo=NULL;

    return m_DevInfo.m_devcnt;

}


//devide update
int SSADevDll:: EraseFPGA(unsigned int dev,unsigned char *ip,unsigned char start,unsigned char end)
{
	int result;
	result=m_DevUpdate.EraseFPGA(dev,ip,start,end);
	
	return result;
}

int SSADevDll:: EraseRCVR(unsigned int dev,unsigned char *ip,unsigned char start,unsigned char end)
{
	int result;
	result=m_DevUpdate.EraseRCVR(dev,ip,start,end);
	return result;
}



unsigned char SSADevDll:: GetEraseProg()
{
	unsigned char prog;
	prog=m_DevUpdate.GetEraseProg();
	return prog;
}

int SSADevDll:: WriteFPGAFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt)
{
	int result;
	result=m_DevUpdate.WriteFPGAFlash(dev,ip,addr,data,cnt);
	return result;
}

int SSADevDll:: WriteFPGAReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt)
{
	int result;
	result=m_DevUpdate.WriteFPGAReg(dev,ip,addr,data,cnt);
	return result;
}

int SSADevDll:: WriteRCVRFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt)
{
	int result;
	result=m_DevUpdate.WriteRCVRFlash(dev,ip,addr,data,cnt);
	return result;
}

int SSADevDll:: WriteRCVRReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt)
{
	int result;
	result=m_DevUpdate.WriteRCVRReg(dev,ip,addr,data,cnt);
	return result;
}

int SSADevDll:: WriteUimage(unsigned short dev,unsigned char *ip,unsigned char *data,unsigned int cnt)
{
	int result;
	result=m_DevUpdate.WriteUimage(dev,ip,data,cnt);
	return result;
}

int SSADevDll:: SetLCDNum(unsigned short dev,unsigned char left,unsigned char right,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetLCDNum(dev,left,right,ip);
	return result;
}

int SSADevDll:: GetLCDNumFromFlash(WORD dev,unsigned char *ip,unsigned char& left,unsigned char& right)
{
	int result;
	result=m_DevUpdate.GetLCDNumFromFlash(dev,ip);
	left=m_DevUpdate.GetLeftLCDNum();
	right=m_DevUpdate.GetRightLCDNum();
	return result;
}

unsigned char SSADevDll:: GetWriteProg()
{
	unsigned char prog;
	prog=m_DevUpdate.GetWriteProg();
	return prog;
}

int SSADevDll::ReadFPGAFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadFPGAFlash(dev,ip,addr,cnt,dest);
	return result;
}

int SSADevDll:: ReadTDVRFPGAFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadTDVRFPGAFlash(dev,ip,addr,cnt,dest);
	return result;
}

int SSADevDll:: ReadFPGAReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadFPGAReg(dev,ip,addr,cnt,dest);
	return result;
}

int SSADevDll::ReadRCVRFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadRCVRFlash(dev,ip,addr,cnt,dest);
	return result;
}

int SSADevDll:: ReadRCVRReg(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadRCVRReg(dev,ip,addr,cnt,dest);
	return result;
}

int SSADevDll:: ReadUimage(unsigned short dev,unsigned char *ip,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadUimage(dev,ip,cnt,dest);
	return result;
}

unsigned char SSADevDll:: GetReadProg()
{
	unsigned char prog;
	prog=m_DevUpdate.GetReadProg();
	return prog;
}

void SSADevDll:: ResetProg()
{
	m_DevUpdate.ResetProg();
}

int SSADevDll:: CheckResult(unsigned short dev,unsigned char *ip,unsigned char res)
{
	int result;
	result=m_DevUpdate.CheckResult(dev,ip,res);
	return result;
}

int SSADevDll:: SetFPGAToBoot(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetFPGAToBoot(dev,ip);
	return result;
}

int SSADevDll:: SetTDVRFPGAToBoot(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetTDVRFPGAToBoot(dev,ip);
	return result;
}

int SSADevDll:: SetFPGAToNormal(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetFPGAToNormal(dev,ip);
	return result;
}

int SSADevDll:: SetTDVRFPGAToNormal(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetTDVRFPGAToNormal(dev,ip);
	return result;
}

int SSADevDll:: GetFPGAMode(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.GetFPGAMode(dev,ip);
	return result;
}

int SSADevDll:: GetTDVRFPGAMode(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.GetTDVRFPGAMode(dev,ip);
	return result;
}

int SSADevDll:: GetFPGABig(unsigned short dev,unsigned char *ip,DWORD addr)
{
	int result;
	result=m_DevUpdate.GetFPGABig(dev,ip,addr);
	return result;
}

int SSADevDll:: SetRCVRToBoot(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetRCVRToBoot(dev,ip);
	return result;
}

int SSADevDll:: SetRCVRToNormal(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetRCVRToNormal(dev,ip);
	return result;
}

int SSADevDll:: GetLCDStatus(unsigned short dev,unsigned char *ip,int pos,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.LCDStatusFromIIC(dev,ip,pos);
	if (!result)
	{
		return FALSE;
	}
	m_DevUpdate.GetLCDStatus(pos,dest);
	return TRUE;
}

int SSADevDll:: GetLCDVersion(unsigned short dev,unsigned char *ip,int pos,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.LCDSVerFromIIC(dev,ip,pos);
	if (!result)
	{
		return FALSE;
	}
	m_DevUpdate.GetLCDVersion(pos,dest);
	return TRUE;
}

int SSADevDll:: ReStart(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.ReStart(dev,ip);
	return result;
}

int SSADevDll:: HeartBeat(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.HeartBeat(dev,ip);
	return result;
}

int SSADevDll:: KillApp(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.KillSSAApp(dev,ip);
	return result;
}

int SSADevDll:: Delete_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,int &success)
{
	int result;
	result=m_DevUpdate.Delete_File(dev,ip,filename,filenamelen,success);
	return result;
}

int SSADevDll:: Upload_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,unsigned short fileauthority,unsigned char *data,unsigned int cnt,unsigned int addr)
{
	int result;	
	result=m_DevUpdate.Upload_File(dev,ip,filename,filenamelen,fileauthority,data,cnt,addr);
	return result;
}

int SSADevDll:: Read_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.Read_File(dev,ip,filename,filenamelen,cnt,dest);
	return result;
}

int SSADevDll:: Download_File(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamecnt,string& dest,unsigned int& cnt)
{
	int result;
	result=m_DevUpdate.Download_File(dev,ip,filename,filenamecnt,dest,cnt);
	return result;
}

int SSADevDll:: Download_File_Big(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt)
{
	int result;
	result=m_DevUpdate.Download_File_Big(dev,ip,filename,filenamelen,dest,cnt);
	return result;

}

int SSADevDll:: Download_File_BigNew(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt)
{
	int result;
	result=m_DevUpdate.Download_File_BigNew(dev,ip,filename,filenamelen,dest,cnt);
	return result;

}

int SSADevDll:: Download_UBLFile(unsigned short dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,string &info,unsigned int& cnt)
{
	int result;
	result=m_DevUpdate.Download_UBLFile(dev,ip,filename,filenamelen,info,cnt);
	return result;

}

unsigned char SSADevDll:: GetUploadProg()
{
	unsigned char prog;
	prog=m_DevUpdate.GetUploadProg();
	return prog;

}

unsigned char SSADevDll:: GetDownloadProg()
{
	unsigned char prog;
	prog=m_DevUpdate.GetDownloadProg();
	return prog;

}

unsigned char SSADevDll:: GetReadFileProg()
{
	unsigned char prog;
	prog=m_DevUpdate.GetReadFileProg();
	return prog;

}

unsigned int  SSADevDll:: GetIsuImageWrite()
{
	unsigned int iswrite;
	iswrite=m_DevUpdate.GetIsuImageWrite();
	return iswrite;
}

void SSADevDll::ResetFileProg()
{
	m_DevUpdate.ResetFileProg();
}

int SSADevDll:: GetAcsuMode(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.GetAcsuMode(dev,ip);
	return result;
}

int SSADevDll:: SetAcsuToBoot(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetAcsuToBoot(dev,ip);
	return result;
}

int SSADevDll:: SetAcsuToNormal(unsigned short dev,unsigned char *ip)
{
	int result;
	result=m_DevUpdate.SetAcsuToNormal(dev,ip);
	return result;
}

int SSADevDll:: ReadAcsuFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=m_DevUpdate.ReadAcsuFlash(dev,ip,addr,cnt,dest);
	return result;
}

int SSADevDll:: EraseAcsuFlash(unsigned short dev,unsigned char *ip,unsigned int start_sec,unsigned int end_sec)
{
	int result;
	result=m_DevUpdate.EraseAcsuFlash(dev,ip,start_sec,end_sec);
	return result;

}

int SSADevDll:: WriteAcsuFlash(unsigned short dev,unsigned char *ip,unsigned int addr,unsigned char *data,unsigned int cnt)
{
	int result;
	result=m_DevUpdate.WriteAcsuFlash(dev,ip,addr,data,cnt);
	return result;
}

void SSADevDll:: SetVersionInfo(int info)
{
	m_DevUpdate.SetVersionInfo(info);
}

int SSADevDll:: CustomCommandControl(unsigned int dev,unsigned int macid,unsigned short headertype,string& msgdata,string& comstring,string& recvstring,int timeout,unsigned int devip)
{
	m_DevInfo.ClearSocketRecv();
	int result;
	result=m_DevInfo.CustomCommandControl(dev,macid,headertype,msgdata,comstring,recvstring,timeout,devip);
	return result;
		 
}

int SSADevDll:: GetAdapterInfoList(char* list)
{
	SINFO_ADAPTER* listtmp=(SINFO_ADAPTER*)list;
	int result;
	result=m_DevInfo.GetAdapterInfoList(listtmp);
	return result;
}

int SSADevDll:: GetErrorCode()
{
	return ssa_error_code;
}

int SSADevDll:: NewUpdateMessage(string &info)
{
	int result;
	result=m_DevInfo.NewUpdateMessage(info);
	return result;
}

void SSADevDll:: Devrestart368()
{
	m_DevInfo.Dev368Close();
	m_DevInfo.Dev368Init();
}







