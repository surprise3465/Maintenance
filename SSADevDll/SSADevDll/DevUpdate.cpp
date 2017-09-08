#include "stdafx.h"
#include <string>
#include <sstream>
#include <fstream>
using namespace std;
#include <iostream> 
#include "DevUpdate.h"
int ssa_error_code=ERROR_NO;
CRITICAL_SECTION cs;
CDevUpdate::CDevUpdate()
{
	m_LeftLCD=0;
	m_RightLCD=0;
	m_IsOldVersion=0;
	m_EraseProg = 0;
	m_WriteProg = 0;
	m_ReadProg = 0;
	m_IsuImageWrite = 0;
	m_UploadProg = 0;
	m_DownloadProg = 0;
	m_ReadFileProg = 0;
	memset(m_RightLCDStatus,0,sizeof(m_RightLCDStatus));
	memset(m_LeftLCDStatus,0,sizeof(m_LeftLCDStatus));
	memset(m_LeftLCDVersion,0,sizeof(m_LeftLCDVersion));
	memset(m_RightLCDVersion,0,sizeof(m_RightLCDVersion));
	::InitializeCriticalSection(&cs);
}

CDevUpdate::~CDevUpdate()
{
	::DeleteCriticalSection(&cs);
}

BOOL CDevUpdate::DevUpdateInit(WORD gen_prot,WORD pro_prot,WORD proID)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		ssa_error_code=ERROR_WSASTARTUP_FAILED;
		return FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return FALSE; 
	}

	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");//INADDR_ANY
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(TCP_PORT);

	SendExtractor.SetProt(gen_prot,pro_prot,proID);
	RecvExtractor.SetProt(gen_prot,pro_prot,proID);

	ssa_error_code=ERROR_NO;
	return TRUE;

}

BOOL CDevUpdate::DevUpdateInitNew(WORD gen_prot,WORD pro_prot,WORD pro_protsw,WORD proID)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		ssa_error_code=ERROR_WSASTARTUP_FAILED;
		return FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return FALSE; 
	}

	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");//INADDR_ANY
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(TCP_PORT);

	SendExtractor.SetProtNew(gen_prot,pro_prot,proID,pro_protsw);
	RecvExtractor.SetProtNew(gen_prot,pro_prot,proID,pro_protsw);

	ssa_error_code=ERROR_NO;
	return TRUE;

}

void CDevUpdate::DevUpdateClose()
{
	
}

void CDevUpdate::SetAddrFromIP(unsigned char *ip)
{
	DWORD adr = ip[3]+ip[2]*0x100+ip[1]*0x10000+ip[0]*0x1000000;
	DWORD ad = ((adr&0xFF)<<24) + (((adr>>8)&0xFF)<<16) + 
		(((adr>>16)&0xFF)<<8) + (adr>>24);
	addrSrv.sin_addr.s_addr = ad;
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(TCP_PORT);
	return;

}


BOOL CDevUpdate::EraseFPGA(WORD dev,unsigned char *ip,BYTE start,BYTE end)
{
	fd_set ReadSet;
	timeval TimeOut;


	ERASE_COMM EraseComm;
	ERASE_PROG EraseProg;
	char RecvBuf[COMM_LEN+INTER_LEN+sizeof(EraseProg)];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_FPGA_ERASE);

	EraseComm.byFirstSection=start;
	EraseComm.byLastSection=end;
	memset(EraseComm.byRes,0,2);

	SendExtractor.OrgPacket(&EraseComm,sizeof(EraseComm));

	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	while (1)
	{
		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_ERASE_PROGRESS))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}
			memcpy((char*)&EraseProg,(char*)RecvExtractor.GetData().c_str(),sizeof(EraseProg));
			m_EraseProg=EraseProg.byProgress;
			if (m_EraseProg==100)
			{
				break;
			}
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;

		}
		

	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::EraseRCVR(WORD dev,unsigned char *ip,BYTE start,BYTE end)
{
	fd_set ReadSet;
	timeval TimeOut;


	ERASE_COMM EraseComm;
	ERASE_PROG EraseProg;
	char RecvBuf[COMM_LEN+INTER_LEN+sizeof(EraseProg)];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_RCVR_ERASE);

	EraseComm.byFirstSection=start;
	EraseComm.byLastSection=end;
	memset(EraseComm.byRes,0,2);

	SendExtractor.OrgPacket(&EraseComm,sizeof(EraseComm));

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	while (1)
	{
		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_ERASE_PROGRESS))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}
			memcpy((char*)&EraseProg,(char*)RecvExtractor.GetData().c_str(),sizeof(EraseProg));
			m_EraseProg=EraseProg.byProgress;
			if (m_EraseProg==100)
			{
				break;
			}
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;

		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}


BYTE CDevUpdate::GetEraseProg()
{
	return m_EraseProg;
}

BOOL CDevUpdate::WriteFPGAFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{
	BOOL result;
	result=WriteFlash(dev,MSG_WRITE_FPGA_FLASH,ip,addr,data,cnt);
	return result;
}


BOOL CDevUpdate::WriteFPGAReg(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{
	BOOL result;
	result=WriteReg(dev,MSG_WRITE_FPGA_REG,ip,addr,data,cnt);
	return result;

}

BOOL CDevUpdate::WriteRCVRFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{
	BOOL result;
	result=WriteFlash(dev,MSG_WRITE_RCVR_FLASH,ip,addr,data,cnt);
	return result;

}

BOOL CDevUpdate::WriteRCVRReg(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{
	BOOL result;
	result=WriteReg(dev,MSG_WRITE_RCVR_REG,ip,addr,data,cnt);
	return result;

}

BOOL CDevUpdate::WriteUimage(WORD dev,unsigned char *ip,unsigned char *data,unsigned int cnt)
{

	fd_set ReadSet;
	timeval TimeOut;

	WRITE_DATA WriteData;
	char RecvBuf[100];
	int result;
	int lastp=0;
	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_WRITE_UIMAGE);

	SendExtractor.OrgPacket(NULL,0);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			lastp=LAST_PAC;
			curLen=last;
		}
		else if ((last==0) && (curpa==packet-1))
		{
			lastp=LAST_PAC;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_REQUEST_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			SendExtractor.SetDevMsg(dev,MSG_DOWNLOAD_DATA);

			WriteData.dwPacketNo=curpa;
			WriteData.dwStartAddr=0x0000;
			WriteData.byLastPacket=lastp;
			WriteData.wDataLength=curLen;
			memcpy((char*)WriteData.byFileData,&data[curpa*PACKET],curLen);
			memset((char*)WriteData.byRes,0,2);
			memset((char*)WriteData.byRes2,0,3);

			SendExtractor.OrgPacket(&WriteData,sizeof(WriteData)-PACKET+curLen);
			result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
			if (result==SOCKET_ERROR)
			{
				ssa_error_code=ERROR_DATA_SEND_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			if (m_IsOldVersion==1)
			{
				if (curpa==packet-1)
				{
					m_IsuImageWrite=1;

					FD_ZERO(&ReadSet);
					FD_SET(sockClient,&ReadSet);
					TimeOut.tv_sec = TIMEOUT;
					TimeOut.tv_usec = 0;
					result = select(0, &ReadSet, NULL,NULL, &TimeOut);
					if (result>0)
					{
						//等待应答
						m_IsuImageWrite=0;
						result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
						if ((result==0) || (result==SOCKET_ERROR))
						{
							ssa_error_code=ERROR_DATA_RECV_FAILED;
							closesocket(sockClient);
							return FALSE;
						}
						if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPDATE_OK))
						{
							ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
							closesocket(sockClient);
							return FALSE;
						}
					}
					else
					{
						ssa_error_code=ERROR_RECV_NO_DATA;
						m_IsuImageWrite=0;
						closesocket(sockClient);
						return FALSE;
					}
				}

			}

			
			curpa++;
			m_WriteProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::WriteFlash(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{
	fd_set ReadSet;
	timeval TimeOut;
	WRITE_DATA WriteData;
	char RecvBuf[100];
	int result;
	int lastp=0;
	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		last=PACKET;
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,msg);

	SendExtractor.OrgPacket(NULL,0);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			lastp=LAST_PAC;
			curLen=last;
		}
		else if ((last==0) && (curpa==packet-1))
		{
			lastp=LAST_PAC;
		}
		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_REQUEST_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			SendExtractor.SetDevMsg(dev,MSG_DOWNLOAD_DATA);

			WriteData.dwPacketNo=curpa;
			WriteData.dwStartAddr=addr+curpa*PACKET;
			WriteData.byLastPacket=lastp;
			WriteData.wDataLength=curLen;
			memset((char*)WriteData.byFileData,0xFF,PACKET);
			memcpy((char*)WriteData.byFileData,&data[curpa*PACKET],curLen);
			memset((char*)WriteData.byRes,0,2);
			memset((char*)WriteData.byRes2,0,3);

			SendExtractor.OrgPacket(&WriteData,sizeof(WriteData));
			result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
			if (result==SOCKET_ERROR)
			{
				ssa_error_code=ERROR_DATA_SEND_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			if (m_IsOldVersion==1)
			{
				if (curpa==packet-1)
				{
					FD_ZERO(&ReadSet);
					FD_SET(sockClient,&ReadSet);
					TimeOut.tv_sec = TIMEOUT;
					TimeOut.tv_usec = 0;
					result = select(0, &ReadSet, NULL,NULL, &TimeOut);
					if (result>0)
					{
						result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
						if ((result==0) || (result==SOCKET_ERROR))
						{
							ssa_error_code=ERROR_DATA_RECV_FAILED;
							closesocket(sockClient);
							return FALSE;
						}
						if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPDATE_OK))
						{
							ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
							closesocket(sockClient);
							return FALSE;
						}

					}
					else
					{
						ssa_error_code=ERROR_RECV_NO_DATA;
						closesocket(sockClient);
						return FALSE;
					}
				}
			}

			

			curpa++;
			m_WriteProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}


BOOL CDevUpdate::WriteReg(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{

	fd_set ReadSet;
	timeval TimeOut;
	WRITE_DATA WriteData;
	char RecvBuf[100];
	int result;
	int lastp=0;
	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,msg);

	SendExtractor.OrgPacket(NULL,0);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			lastp=LAST_PAC;
			curLen=last;
		}
		else if ((last==0) && (curpa==packet-1))
		{
			lastp=LAST_PAC;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_REQUEST_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			SendExtractor.SetDevMsg(dev,MSG_DOWNLOAD_DATA);

			WriteData.dwPacketNo=curpa;
			WriteData.dwStartAddr=addr+curpa*PACKET;
			WriteData.byLastPacket=lastp;
			WriteData.wDataLength=curLen;
			memcpy((char*)WriteData.byFileData,&data[curpa*PACKET],curLen);
			memset((char*)WriteData.byRes,0,2);
			memset((char*)WriteData.byRes2,0,3);

			SendExtractor.OrgPacket(&WriteData,sizeof(WriteData)-PACKET+curLen);
			result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
			if (result==SOCKET_ERROR)
			{
				ssa_error_code=ERROR_DATA_SEND_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			if (m_IsOldVersion==1)
			{
				if (curpa==packet-1)
				{
					FD_ZERO(&ReadSet);
					FD_SET(sockClient,&ReadSet);
					TimeOut.tv_sec = TIMEOUT;
					TimeOut.tv_usec = 0;
					result = select(0, &ReadSet, NULL,NULL, &TimeOut);
					if (result>0)
					{
						result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
						if ((result==0) || (result==SOCKET_ERROR))
						{
							ssa_error_code=ERROR_DATA_RECV_FAILED;
							closesocket(sockClient);
							return FALSE;
						}
						if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPDATE_OK))
						{
							ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
							closesocket(sockClient);
							return FALSE;
						}
					}
					else
					{
						ssa_error_code=ERROR_RECV_NO_DATA;
						closesocket(sockClient);
						return FALSE;
					}
				}
			}
			curpa++;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;

		}		
		//m_WriteProg=curpa*100/packet;

	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::CheckLCDNUM()
{
	if (m_LeftLCD<0 || m_LeftLCD>32)
	{
		m_LeftLCD=0;
		m_RightLCD=0;
		ssa_error_code=ERROR_LCDNUM_WRONG;
		return FALSE;
	}
	if (m_RightLCD<0 || m_RightLCD>32)
	{
		m_LeftLCD=0;
		m_RightLCD=0;
		ssa_error_code=ERROR_LCDNUM_WRONG;
		return FALSE;
	}
	if (m_LeftLCD==0 && m_RightLCD==0)
	{
		m_LeftLCD=0;
		m_RightLCD=0;
		ssa_error_code=ERROR_LCDNUM_WRONG;
		return FALSE;
	}

	ssa_error_code=ERROR_NO;
	return TRUE;
}

BOOL CDevUpdate::SetLCDNum(WORD dev,BYTE left,BYTE right,unsigned char *ip)
{
	LCD_NUM LCDNum;
	int result;

	m_LeftLCD=left;
	m_RightLCD=right;
	if (!CheckLCDNUM())
	{		
		return FALSE;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_LCD_NUM);

	LCDNum.byLeft=m_LeftLCD;
	LCDNum.byRight=m_RightLCD;
	memset((char*)&LCDNum.byRes,0,2);

	SendExtractor.OrgPacket(&LCDNum,sizeof(LCDNum));

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));	

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BYTE CDevUpdate::GetLeftLCDNum()
{
	return m_LeftLCD;
}

BYTE CDevUpdate::GetRightLCDNum()
{
	return m_RightLCD;
}

BOOL CDevUpdate::GetLCDNumFromFlash(WORD dev,unsigned char *ip)
{
	BOOL result;
	unsigned char Cmd[8];
	memset(Cmd,0,8);
	//Cmd[0]=0x03;
	Cmd[0]=0x03;
	Cmd[1]=0x00;
	Cmd[2]=0xff;
	Cmd[3]=0x03;
	
	
	for(int i=0;i<4;i++)
	{
		result=WriteRCVRReg(dev,ip,0x00+i,&Cmd[i],1);
		
		if (result==FALSE)
		{
			return FALSE;
		}

	}
	
	Sleep(1500);
	
	result=ReadRCVRReg(dev,ip,0x3ff,1,&m_LeftLCD);
	if (result==FALSE)
	{
		return FALSE;
	}
	result=ReadRCVRReg(dev,ip,0x5ff,1,&m_RightLCD);
	if (result==FALSE)
	{
		return FALSE;
	}
/*	unsigned char dest[1024];*/
// 	memset(dest,0,1024);
// 	result=ReadRCVRReg(dev,ip,0x400,1,&dest[0]);
// 	result=ReadRCVRReg(dev,ip,0x401,1,&dest[1]);
// 	result=ReadRCVRReg(dev,ip,0x402,1,&dest[2]);	
	
	return TRUE;
}



BYTE CDevUpdate::GetWriteProg()
{
	return m_WriteProg;
}

BOOL CDevUpdate::ReadFPGAFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	BOOL result;
	result=ReadFlash(dev,MSG_READ_FPGA_FLASH,ip,addr,cnt,dest);
	return result;

}

BOOL CDevUpdate::ReadTDVRFPGAFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	BOOL result;
	result=ReadTDVRFlash(dev,MSG_READ_FPGA_FLASH,ip,addr,cnt,dest);
	return result;

}


int CDevUpdate::ReadFPGAReg(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	int result;
	result=ReadReg(dev,MSG_READ_FPGA_REG,ip,addr,cnt,dest);
	return result;

}


BOOL CDevUpdate::ReadRCVRFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	BOOL result;
	result=ReadRCVR(dev,MSG_READ_RCVR_FLASH,ip,addr,cnt,dest);
	return result;
}


BOOL CDevUpdate::ReadRCVRReg(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	BOOL result;
	result=ReadReg(dev,MSG_READ_RCVR_REG,ip,addr,cnt,dest);
	return result;

}


BOOL CDevUpdate::ReadUimage(WORD dev,unsigned char *ip,unsigned int cnt,unsigned char *dest)
{

	fd_set ReadSet;
	timeval TimeOut;
	READ_DATA ReadData;
	UPLOAD_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_READ_UIMAGE);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			curLen=last;
		}

		ReadData.dwStartAddr=curpa;
		ReadData.wDataLength=curLen;
		memset((char*)ReadData.byRes,0,2);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT*3;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));

			memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,curLen);

			curpa++;

			m_ReadProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::ReadFlash(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	fd_set ReadSet;
	timeval TimeOut;
	READ_DATA ReadData;
	UPLOAD_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		last=PACKET;
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,msg);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (curpa<packet)
	{
		ReadData.dwStartAddr=addr+curpa*PACKET;
		ReadData.wDataLength=curLen;
		memset((char*)ReadData.byRes,0,2);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT*2;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));

			memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,curLen);

			curpa++;

			m_ReadProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::ReadTDVRFlash(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	fd_set ReadSet;
	timeval TimeOut;
	READ_DATA ReadData;
	UPLOAD_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int packet=1;//一共几包
	unsigned int curpa=0;//当前包号

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,msg);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (curpa<packet)
	{
		ReadData.dwStartAddr=addr+curpa*PACKET;
		ReadData.wDataLength=cnt;
		memset((char*)ReadData.byRes,0,2);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT*2;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));

			//memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,curLen);
            memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,cnt);
			curpa++;

			m_ReadProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}



BOOL CDevUpdate::ReadRCVR(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	fd_set ReadSet;
	timeval TimeOut;
	READ_DATA ReadData;
	UPLOAD_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=RCVRP;//当前包大小

	packet=cnt/RCVRP;
	last=cnt-packet*RCVRP;
	if (last!=0)
	{
		last=RCVRP;
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,msg);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (curpa<packet)
	{
		//if ((last!=0) &&(curpa==packet-1))//last packet
		//{
		//	curLen=last;
		//}

		ReadData.dwStartAddr=addr+curpa*RCVRP;
		ReadData.wDataLength=curLen;
		memset((char*)ReadData.byRes,0,2);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT*2;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));

			memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,PACKET);

			curpa++;

			m_ReadProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}






int CDevUpdate::ReadReg(WORD dev,DWORD msg,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{

	fd_set ReadSet;
	timeval TimeOut;
	READ_DATA ReadData;
	UPLOAD_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return 0;
	}

	SendExtractor.SetDevMsg(dev,msg);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			curLen=last;
		}

		ReadData.dwStartAddr=addr+curpa*PACKET;
		ReadData.wDataLength=curLen;
		memset((char*)ReadData.byRes,0,2);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return 0;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT*2;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return 0;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return 0;
			}
			if(RecvExtractor.Parser2(RecvBuf,result,MSG_PROCESS_ERR)==2)
			{
				closesocket(sockClient);
				return 1;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));

			memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,curLen);

			curpa++;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return 0;
		}
		//m_ReadProg=curpa*100/packet;

	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return 2;
}


BYTE CDevUpdate::GetReadProg()
{
	return m_ReadProg;
}

void CDevUpdate::ResetProg()
{
	m_EraseProg=0;
	m_WriteProg=0;
	m_ReadProg=0;

	m_IsuImageWrite=0;
}



BOOL CDevUpdate::CheckResult(WORD dev,unsigned char *ip,BYTE res)
{

	fd_set ReadSet;
	timeval TimeOut;
	CHECK_RESULT CheckResult;
	CHECK_CONFIRM CheckConfirm;

	char RecvBuf[100];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_CHECK_RESULT);
	CheckResult.byCheckResult=res;
	memset((char*)CheckResult.byRes,0,3);

	SendExtractor.OrgPacket((char*)&CheckResult,sizeof(CheckResult));

	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	FD_ZERO(&ReadSet);
	FD_SET(sockClient,&ReadSet);
	TimeOut.tv_sec = TIMEOUT;
	TimeOut.tv_usec = 0;
	result = select(0, &ReadSet, NULL,NULL, &TimeOut);
	if (result>0)
	{
		result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
		if ((result==0) || (result==SOCKET_ERROR))
		{
			ssa_error_code=ERROR_DATA_RECV_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!RecvExtractor.Parser(RecvBuf,result,MSG_CHECK_ACK))
		{
			ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
			closesocket(sockClient);
			return FALSE;
		}

		memcpy((char*)&CheckConfirm,(char*)RecvExtractor.GetData().c_str(),sizeof(CheckConfirm));

		if (CheckConfirm.byCheckConfirm!=res)
		{
			ssa_error_code=ERROR_RECV_CHECKRESULT_WRONG;
			closesocket(sockClient);
			return FALSE;
		}
	}
	else
	{
		ssa_error_code=ERROR_RECV_NO_DATA;
		closesocket(sockClient);
		return FALSE;
	}	

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BOOL CDevUpdate::ReStart(WORD dev,unsigned char *ip)
{
	fd_set ReadSet;
	timeval TimeOut;

	char RecvBuf[100];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_RESTART);

	SendExtractor.OrgPacket(NULL,0);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	FD_ZERO(&ReadSet);
	FD_SET(sockClient,&ReadSet);
	TimeOut.tv_sec = TIMEOUT;
	TimeOut.tv_usec = 0;
	result = select(0, &ReadSet, NULL,NULL, &TimeOut);
	if (result>0)
	{
		result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
		if ((result==0) || (result==SOCKET_ERROR))
		{
			ssa_error_code=ERROR_DATA_RECV_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!RecvExtractor.Parser(RecvBuf,result,MSG_RESTART_ACK))
		{
			ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
			closesocket(sockClient);
			return FALSE;
		}
	}
	else
	{
		ssa_error_code=ERROR_RECV_NO_DATA;
		closesocket(sockClient);
		return FALSE;
	}	

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BOOL CDevUpdate::HeartBeat(WORD dev,unsigned char *ip)
{
//	char RecvBuf[100];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_HEARTBEAT);

	SendExtractor.OrgPacket(NULL,0);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BOOL CDevUpdate::KillSSAApp(WORD dev,unsigned char *ip)
{
	fd_set ReadSet;
	timeval TimeOut;

	char RecvBuf[100];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_KILL_SSA_APP);

	SendExtractor.OrgPacket(NULL,0);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	FD_ZERO(&ReadSet);
	FD_SET(sockClient,&ReadSet);
	TimeOut.tv_sec = TIMEOUT;
	TimeOut.tv_usec = 0;
	result = select(0, &ReadSet, NULL,NULL, &TimeOut);
	if (result>0)
	{
		result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
		if ((result==0) || (result==SOCKET_ERROR))
		{
			ssa_error_code=ERROR_DATA_RECV_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!RecvExtractor.Parser(RecvBuf,result,MSG_KILL_SSA_APP_ACK))
		{
			ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
			closesocket(sockClient);
			return FALSE;
		}
	}
	else
	{
		ssa_error_code=ERROR_RECV_NO_DATA;
		closesocket(sockClient);
		return FALSE;
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}


int CDevUpdate::GetFPGAMode(WORD dev,unsigned char *ip)
{
	unsigned char fpgamode[50];
	memset(fpgamode,0xff,sizeof(fpgamode));
	int c=ReadFPGAReg(dev,ip,0x1FE,1,fpgamode);
	if(c==1)
	{
		return MODE_NOFPGA;
	}
	else if(c==2)
	{
		if (fpgamode[0]==0x55)
		{
			return MODE_NORMAL;
		}
		else if (fpgamode[0]==0x00)
		{
			return MODE_BOOT;
		}
		else 
			return 0;
	}
	else
       return 0;

}
int CDevUpdate::GetTDVRFPGAMode(WORD dev,unsigned char *ip)
{
	unsigned char fpgamode[50];
	memset(fpgamode,0xff,sizeof(fpgamode));
	int c=ReadFPGAReg(dev,ip,0x3FE,1,fpgamode);
	//ReadFPGAReg(dev,ip,0x1FE,1,fpgamode);

	/*if (fpgamode[0]==0x55)
	{
		return MODE_NORMAL;
	}
	else if (fpgamode[0]==0x00)
	{
		return MODE_BOOT;
	}
	else 
		return 0;*/
	if(c==1)
	{
		return MODE_NOFPGA;
	}
	else if(c==2)
	{
		if (fpgamode[0]==0x55)
		{
			return MODE_NORMAL;
		}
		else if (fpgamode[0]==0x00)
		{
			return MODE_BOOT;
		}
		else 
			return 0;
	}
	else
       return 0;

}
int CDevUpdate::GetFPGABig(WORD dev,unsigned char *ip,DWORD addr)
{
	unsigned char fpgamode[50];
	memset(fpgamode,0xff,sizeof(fpgamode));
	//ReadFPGAReg(dev,ip,0x1FD,1,fpgamode);//原先读指数寄存器为0x1FD
	ReadFPGAReg(dev,ip,addr,1,fpgamode);

	return fpgamode[0];


}
BOOL CDevUpdate::LCDStatusFromIIC(WORD dev,unsigned char *ip,int pos)
{
	BOOL result;
	result=CheckLCDNUM();
	if (!result)
	{
		return FALSE;
	}
	switch(pos)
	{
	case 0:
		for (int i=0;i<32;i++)
		{
			result=ReadFPGAReg(dev,ip,0x00+i,1,&m_LeftLCDStatus[i]);
			if (!result)
			{
				return FALSE;
			}
		}

		break;
	case 1:
		for (int i=0;i<32;i++)
		{
			result=ReadFPGAReg(dev,ip,0x20+i,1,&m_RightLCDStatus[i]);
			if (!result)
			{
				return FALSE;
			}

		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CDevUpdate::LCDSVerFromIIC(WORD dev,unsigned char *ip,int pos)
{
	BOOL result;
// 	result=CheckLCDNUM();
// 	if (!result)
// 	{
// 		return FALSE;
// 	}
	switch(pos)
	{
	case 0:
		for (int i=0;i<32;i++)
		{
			result=ReadFPGAReg(dev,ip,0x40+i,1,&m_LeftLCDVersion[i]);
			if (!result)
			{
				return FALSE;
			}
		}
		break;
	case 1:
		for (int i=0;i<32;i++)
		{
			result=ReadFPGAReg(dev,ip,0x60+i,1,&m_RightLCDVersion[i]);
			if (!result)
			{
				return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void CDevUpdate::GetLCDStatus(int pos,unsigned char *dest)
{
	if (pos==LEFT_LCD)
	{
		memcpy(dest,m_LeftLCDStatus,32);
	}
	else if (pos==RIGHT_LCD)
	{
		memcpy(dest,m_RightLCDStatus,32);
	}
}

void CDevUpdate::GetLCDVersion(int pos,unsigned char *dest)
{
	if (pos==LEFT_LCD)
	{
		memcpy(dest,m_LeftLCDVersion,32);
	}
	else if (pos==RIGHT_LCD)
	{
		memcpy(dest,m_RightLCDVersion,32);
	}
}


BOOL CDevUpdate::SetFPGAToBoot(WORD dev,unsigned char *ip)
{
	unsigned char data[2];
	memset(data,0,2);
	data[0]=0xF5;//状态转换指令
	WriteFPGAReg(dev,ip,0x00,data,1);
	int mode;

	Sleep(100);
	unsigned int cnt=0;
	while(cnt<10)
	{
		mode=GetFPGAMode(dev,ip);
	    if (mode==MODE_BOOT)
			break;
		Sleep(200);
		cnt++;	

	}
	if (cnt==10)
	{
		ssa_error_code=ERROR_SET_FPGA_TO_BOOT_FAILED;
		return FALSE;
	}
	
	//mode=GetFPGAMode(dev,ip);
	if (mode==MODE_BOOT)
	{
		data[0]=0xF0;//更新指令，使FPGA停留在维护模式

		for (int i=0;i<10;i++)
		{
			WriteFPGAReg(dev,ip,0x00,data,1);
			Sleep(500);
		}	
		
	}


	ssa_error_code=ERROR_NO;
	return TRUE;
}

BOOL CDevUpdate::SetTDVRFPGAToBoot(WORD dev,unsigned char *ip)
{
	unsigned char data[2];
	memset(data,0,2);
	data[0]=0xF5;//状态转换指令
	WriteFPGAReg(dev,ip,0x01,data,1);
	int mode;

	Sleep(100);

	unsigned char hand[2];
	memset(hand,0,2);
	hand[0]=0x55;

	int t=0;
	int result;
	while(t<30)
	{
		result=WriteFPGAReg(dev,ip,0x1FB,hand,1);
		if(result==TRUE)
		{
			result=ReadFPGAReg(dev,ip,0x1FB,1,hand);
			if(hand[0]==0xaa)
			{
				break;
			}
			else
			{
				hand[0]=0x55;
				t++;
			}
		}
		
		Sleep(100);
	}
	if(hand[0]!=0xaa)
	{

		return FALSE;
	}

	unsigned int cnt=0;
	while(cnt<3)
	{
		mode=GetFPGAMode(dev,ip);
		if (mode==MODE_BOOT)
			break;
		Sleep(200);
		cnt++;	

	}
	if (cnt==3)
	{
		ssa_error_code=ERROR_SET_FPGA_TO_BOOT_FAILED;
		return FALSE;
	}

	//mode=GetFPGAMode(dev,ip);
	if (mode==MODE_BOOT)
	{
		data[0]=0xF0;//更新指令，使FPGA停留在维护模式

		for (int i=0;i<3;i++)
		{
			WriteFPGAReg(dev,ip,0x01,data,1);
			Sleep(500);
		}	

	}


	ssa_error_code=ERROR_NO;
	return TRUE;
}


BOOL CDevUpdate::SetFPGAToNormal(WORD dev,unsigned char *ip)
{
	unsigned char data[2];
	memset(data,0,2);
	data[0]=0xF5;

	BOOL result;
	result=WriteFPGAReg(dev,ip,0x00,data,1);
	Sleep(1000);
	
	int cnt=30;
	while(cnt)
	{
		int mode=GetFPGAMode(dev,ip);
		if (mode==MODE_NORMAL)
		{
			return TRUE;
		}
		else
		{
			cnt--;
			Sleep(100);
		}

	}
	if (cnt==0)
	{
		ssa_error_code=ERROR_SET_FPGA_TO_NORMAL_FAILED;
		return FALSE;
	}
	else
	{
		ssa_error_code=ERROR_NO;
		return TRUE;
	}

}

BOOL CDevUpdate::SetTDVRFPGAToNormal(WORD dev,unsigned char *ip)
{
	unsigned char data[2];
	memset(data,0,2);
	data[0]=0xF5;

	BOOL result;
	result=WriteFPGAReg(dev,ip,0x01,data,1);
	Sleep(1000);

	unsigned char hand[2];
	memset(hand,0,2);
	hand[0]=0x55;

	int t=0;
	while(t<30)
	{
		result=WriteFPGAReg(dev,ip,0x1FB,hand,1);
		if(result==TRUE)
		{
			result=ReadFPGAReg(dev,ip,0x1FB,1,hand);
			if(hand[0]==0xaa)
			{
				break;
			}
			else
			{
				hand[0]=0x55;
				t++;
			}
		}

		Sleep(100);
	}
	if(hand[0]!=0xaa)
	{

		return FALSE;
	}
	
	int cnt=30;
	while(cnt)
	{
		int mode=GetFPGAMode(dev,ip);
		if (mode==MODE_NORMAL)
		{
			return TRUE;
		}
		else
		{
			cnt--;
			Sleep(100);
		}

	}
	if (cnt==0)
	{
		ssa_error_code=ERROR_SET_FPGA_TO_NORMAL_FAILED;
		return FALSE;
	}
	else
	{
		ssa_error_code=ERROR_NO;
		return TRUE;
	}

}

BOOL CDevUpdate::SetRCVRToBoot(WORD dev,unsigned char *ip)
{
	unsigned char data[2];
	memset(data,0,2);
	data[0]=0xF5;
	WriteRCVRReg(dev,ip,0x00,data,1);
	WriteFPGAReg(dev,ip,0x00,data,1);

	Sleep(500);
	int mode=0;
	unsigned int cnt=0;
	while(cnt<10)
	{
		mode=GetFPGAMode(dev,ip);
		if (mode==MODE_BOOT)
			break;
		Sleep(200);
		cnt++;	

	}
	if (cnt==10)
	{
		ssa_error_code=ERROR_SET_RECEIVER_TO_BOOT_FAILED;
		return FALSE;
	}

	//mode=GetFPGAMode(dev,ip);
	if (mode==MODE_BOOT)
	{
		data[0]=0xF0;
		data[1]=0x01;
		for (int i=0;i<10;i++)
		{
			WriteRCVRReg(dev,ip,0x00,data,2);
			WriteFPGAReg(dev,ip,0x00,data,1);		
			Sleep(500);
		}		
	}
	
	ssa_error_code=ERROR_NO;
	return TRUE;
}

BOOL CDevUpdate::SetRCVRToNormal(WORD dev,unsigned char *ip)
{
	unsigned char data[2];
	memset(data,0,2);
	data[0]=0xF5;

	BOOL result;

	result=WriteRCVRReg(dev,ip,0x00,data,1);
	result=WriteFPGAReg(dev,ip,0x00,data,1);
	Sleep(1000);
	int cnt=30;
	while(cnt)
	{
		int mode=GetFPGAMode(dev,ip);
		if (mode==MODE_NORMAL)
		{
			return TRUE;
		}
		else
		{
			cnt--;
			Sleep(100);
		}

	}
	if (cnt==0)
	{
		ssa_error_code=ERROR_SET_RECEIVER_TO_NORMAL_FAILED;
		return FALSE;
	}
	else
	{
		ssa_error_code=ERROR_NO;
		return TRUE;
	}
	
}



BOOL CDevUpdate::Delete_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,BOOL &success)
{
	fd_set ReadSet;
	timeval TimeOut;
	
	char RecvBuf[2000];
	int result;	
	unsigned char Filename[256];
	memset(Filename,0,sizeof(Filename));

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_DELFILE);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	memcpy((char*)Filename,(char*)filename,filenamelen);

	SendExtractor.OrgPacket(Filename,sizeof(Filename));
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	FD_ZERO(&ReadSet);
	FD_SET(sockClient,&ReadSet);
	TimeOut.tv_sec = TIMEOUT;
	TimeOut.tv_usec = 0;
	result = select(0, &ReadSet, NULL,NULL, &TimeOut);
	if (result>0)
	{
		result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
		if ((result==0) || (result==SOCKET_ERROR))
		{
			ssa_error_code=ERROR_DATA_RECV_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(RecvExtractor.Parser(RecvBuf,result,MSG_DELFILE_ACK))
		{
			success=TRUE;			
		}
		else if (RecvExtractor.Parser(RecvBuf,result,MSG_DELFILE_NACK))
		{
			success=FALSE;
		}
		else
		{
			ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
			closesocket(sockClient);
			return FALSE;

		}
		
	}
	else
	{
		ssa_error_code=ERROR_RECV_NO_DATA;
		closesocket(sockClient);
		return FALSE;

	}
	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BOOL CDevUpdate::Upload_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,WORD fileauthority,unsigned char *data,unsigned int cnt,DWORD addr)
{
	fd_set ReadSet;
	timeval TimeOut;
	WRITE_DATA WriteData;
	char RecvBuf[100];
	int result;
	int lastp=0;
	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
////	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_UPLOADFILE);
	unsigned char fileinfo[260];
	memset(fileinfo,0,sizeof(fileinfo));
	memcpy((char*)fileinfo,(char*)filename,filenamelen);
	memcpy((char*)&fileinfo[256],&fileauthority,sizeof(fileauthority));

	SendExtractor.OrgPacket(fileinfo,sizeof(fileinfo));

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			lastp=LAST_PAC;
			curLen=last;
		}
		else if ((last==0) && (curpa==packet-1))
		{
			lastp=LAST_PAC;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_REQUEST_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			SendExtractor.SetDevMsg(dev,MSG_DOWNLOAD_DATA);

			WriteData.dwPacketNo=curpa;
			WriteData.dwStartAddr=addr+curpa*PACKET;
			WriteData.byLastPacket=lastp;
			WriteData.wDataLength=curLen;
			memcpy((char*)WriteData.byFileData,&data[curpa*PACKET],curLen);
			memset((char*)WriteData.byRes,0,2);
			memset((char*)WriteData.byRes2,0,3);

			SendExtractor.OrgPacket(&WriteData,sizeof(WriteData)-PACKET+curLen);
			result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
			if (result==SOCKET_ERROR)
			{
				ssa_error_code=ERROR_DATA_SEND_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			if (m_IsOldVersion==1)
			{
				if (curpa==packet-1)
				{
					FD_ZERO(&ReadSet);
					FD_SET(sockClient,&ReadSet);
					TimeOut.tv_sec = TIMEOUT;
					TimeOut.tv_usec = 0;
					result = select(0, &ReadSet, NULL,NULL, &TimeOut);
					if (result>0)
					{
						//等待应答						
						result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
						if ((result==0) || (result==SOCKET_ERROR))
						{
							ssa_error_code=ERROR_DATA_RECV_FAILED;
							closesocket(sockClient);
							return FALSE;
						}
						if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPDATE_OK))
						{
							ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
							closesocket(sockClient);
							return FALSE;
						}
					}
					else
					{	
						ssa_error_code=ERROR_RECV_NO_DATA;
						closesocket(sockClient);
						return FALSE;
					}
				}

			}


			curpa++;
			m_UploadProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;

		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BOOL CDevUpdate::Read_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,unsigned int cnt,unsigned char *dest)
{
	fd_set ReadSet;
	timeval TimeOut;
	DOWNLOAD_FILE ReadData;
	UPLOAD_FILE_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
//	DWORD curaddr;//当前地址
	WORD curLen=PACKET;//当前包大小

	packet=cnt/PACKET;
	last=cnt-packet*PACKET;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_DOWNLOADFILE);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			curLen=last;
		}

		ReadData.dwStartAddr=curpa;
		ReadData.wDataLength=curLen;
		memset((char*)ReadData.byRes,0,2);
		memset((char*)ReadData.byfilename,0,sizeof(ReadData.byfilename));
		memcpy((char*)&ReadData.byfilename,(char*)filename,filenamelen);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));
			if (UploadData.dwStartAddr!=ReadData.dwStartAddr)
			{
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&dest[curpa*PACKET],(char*)&UploadData.byFileData,curLen);

			curpa++;
			m_ReadFileProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}



		//m_ReadProg=curpa*100/packet;

	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}


BOOL CDevUpdate::Download_File_BigNew(WORD dev,unsigned char *ip,unsigned char*filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt)
 {
	 fd_set ReadSet;
	 timeval TimeOut;
	 DOWNLOAD_FILE ReadData;
	 UPLOAD_FILE_DATA UploadData;
	 char RecvBuf[2000];
	 int result;

	 unsigned int curpa=0;//当前包号
	 WORD curLen=PACKET;//当前包大小
	 BOOL DownOver=FALSE;
	 CDevMsgParser RecvExtractor;

	 cnt=0;

	 SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);
	 if (sockClient==SOCKET_ERROR)
	 {
		 closesocket(sockClient);
		 ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		 return FALSE;
	 }

	 struct sockaddr_in serv_addr;

	 // 以服务器地址填充结构serv_addr
	 char ipaddr[16];
	 unsigned char iptmp[5];
	 memset(iptmp,0,sizeof(iptmp));
	 memcpy(iptmp,ip,sizeof(iptmp));

	 sprintf(ipaddr,"%d.%d.%d.%d",iptmp[0],iptmp[1],iptmp[2],iptmp[3]);

	 serv_addr.sin_family = AF_INET;
	 serv_addr.sin_addr.s_addr = inet_addr(ipaddr);
	 serv_addr.sin_port = htons(20050);

	 int error = -1;
	 int len = sizeof(int);
	 timeval tm;//连接超时时间
	 //fd_set set;
	 unsigned long ul = 1;
	 ioctlsocket(sockClient, FIONBIO, &ul); //设置为非阻塞模式
	 bool ret = false;
	 if( connect(sockClient, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	 {
		 tm.tv_sec  = 3;
		 tm.tv_usec = 0;
		 FD_ZERO(&ReadSet);
		 FD_SET(sockClient,&ReadSet);
		 if( select(0, NULL, &ReadSet, NULL, &tm) > 0)
		 {
			 getsockopt(sockClient, SOL_SOCKET, SO_ERROR, (char *)&error, /*(socklen_t *)*/&len);
			 if(error == 0) 
				 ret = true;
			 else 
			 {

				 ret = false;
			 }
		 } 
		 else 
		 {
			 ret = false;
		 }
	 }
	 else 
		 ret = true;
	 ul = 0;
	 ioctlsocket(sockClient, FIONBIO, &ul); //设置为阻塞模式
	 if(!ret) 
	 {
		 closesocket( sockClient );
		 return FALSE;
	 }

	 while (DownOver==FALSE)
	 {
		 ::EnterCriticalSection(&cs);
		 
		 SendExtractor.SetDevMsg(dev,MSG_DOWNLOADFILE);

		 ReadData.dwStartAddr=curpa;
		 ReadData.wDataLength=curLen;
		 memset((char*)ReadData.byRes,0,2);
		 memset((char*)ReadData.byfilename,0,sizeof(ReadData.byfilename));
		 memcpy((char*)&ReadData.byfilename,(char*)filename,filenamelen);
		
		 SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		 
		 result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		 int aa=WSAGetLastError();
		 if (result==SOCKET_ERROR)
		 {
			 ssa_error_code=ERROR_DATA_SEND_FAILED;
			 closesocket(sockClient);
			 return FALSE;
		 }
		::LeaveCriticalSection(&cs);


		 FD_ZERO(&ReadSet);
		 FD_SET(sockClient,&ReadSet);
		 TimeOut.tv_sec =0 ;

		 TimeOut.tv_usec =TIMEOUTUS;

		 result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		 if (result>0)
		 {
			 result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			 if ((result==0) || (result==SOCKET_ERROR))
			 {
				 ssa_error_code=ERROR_DATA_RECV_FAILED;
				 closesocket(sockClient);
				 return FALSE;
			 }
			 if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			 {
				 ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				 closesocket(sockClient);
				 return FALSE;
			 }

			 memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));
			 if (UploadData.dwStartAddr!=ReadData.dwStartAddr)
			 {
				 closesocket(sockClient);
				 return FALSE;
			 }

			 dest->write((char*)UploadData.byFileData,UploadData.wDataLength);			 

			 cnt+=UploadData.wDataLength;
			 if (UploadData.byLastPackage==1)
			 {
				 DownOver=TRUE;
			 }

			 curpa++;
			 m_DownloadProg=UploadData.byUpdateProg;
		 }
		 else
		 {
			

			 ssa_error_code=ERROR_RECV_NO_DATA;
			 closesocket(sockClient);
			 return FALSE;
		 }

	 }

	 ssa_error_code=ERROR_NO;
	 closesocket(sockClient);
	 return TRUE;

 }

 BOOL CDevUpdate::Download_File_Big(WORD dev,unsigned char *ip,unsigned char*filename,unsigned int filenamelen,ofstream* dest,unsigned int& cnt)
 {
	 fd_set ReadSet;
	 timeval TimeOut;
	 DOWNLOAD_FILE ReadData;
	 UPLOAD_FILE_DATA UploadData;
	 char RecvBuf[2000];
	 int result;

	 unsigned int curpa=0;//当前包号
	 WORD curLen=PACKET;//当前包大小
	 BOOL DownOver=FALSE;

	 cnt=0;

	 sockClient=socket(AF_INET,SOCK_STREAM,0);
	 if (sockClient==SOCKET_ERROR)
	 {
		 ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		 return FALSE;
	 }

	 SendExtractor.SetDevMsg(dev,MSG_DOWNLOADFILE);

	 SetAddrFromIP(ip);

	 connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	 while (DownOver==FALSE)
	 {
		 ReadData.dwStartAddr=curpa;
		 ReadData.wDataLength=curLen;
		 memset((char*)ReadData.byRes,0,2);
		 memset((char*)ReadData.byfilename,0,sizeof(ReadData.byfilename));
		 memcpy((char*)&ReadData.byfilename,(char*)filename,filenamelen);

		 SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		 result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		 if (result==SOCKET_ERROR)
		 {
			 ssa_error_code=ERROR_DATA_SEND_FAILED;
			 closesocket(sockClient);
			 return FALSE;
		 }

		 FD_ZERO(&ReadSet);
		 FD_SET(sockClient,&ReadSet);
		 TimeOut.tv_sec = 0;

		 TimeOut.tv_usec =TIMEOUTUS;
		 result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		 if (result>0)
		 {
			 result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			 if ((result==0) || (result==SOCKET_ERROR))
			 {
				 ssa_error_code=ERROR_DATA_RECV_FAILED;
				 closesocket(sockClient);
				 return FALSE;
			 }
			 if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			 {
				 ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				 closesocket(sockClient);
				 return FALSE;
			 }

			 memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));
			 if (UploadData.dwStartAddr!=ReadData.dwStartAddr)
			 {
				 closesocket(sockClient);
				 return FALSE;
			 }

			 dest->write((char*)UploadData.byFileData,UploadData.wDataLength);			 

			 cnt+=UploadData.wDataLength;
			 if (UploadData.byLastPackage==1)
			 {
				 DownOver=TRUE;
			 }

			 curpa++;
			 m_DownloadProg=UploadData.byUpdateProg;
		 }
		 else
		 {
			 ssa_error_code=ERROR_RECV_NO_DATA;
			 closesocket(sockClient);
			 return FALSE;
		 }

	 }

	 ssa_error_code=ERROR_NO;
	 closesocket(sockClient);
	 return TRUE;

 }
 BOOL CDevUpdate::Download_UBLFile(WORD dev,unsigned char *ip,unsigned char*filename,unsigned int filenamelen,string& info,unsigned int& cnt)
 {
	 fd_set ReadSet;
	 timeval TimeOut;
	 DOWNLOAD_FILE ReadData;
	 UPLOAD_FILE_DATA UploadData;
	 char RecvBuf[2000];
	 int result;

	 unsigned int curpa=0;//当前包号
	 WORD curLen=PACKET;//当前包大小
	 BOOL DownOver=FALSE;

	 cnt=0;

	 sockClient=socket(AF_INET,SOCK_STREAM,0);
	 if (sockClient==SOCKET_ERROR)
	 {
		 ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		 return FALSE;
	 }

	 SendExtractor.SetDevMsg(dev,MSG_DOWNLOADFILE);

	 SetAddrFromIP(ip);

	 int t;
	 t=connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	 if(t==-1)
	 {
		 GetLastError();
	 }

	 while (DownOver==FALSE)
	 {
		 ReadData.dwStartAddr=curpa;
		 ReadData.wDataLength=curLen;
		 memset((char*)ReadData.byRes,0,2);
		 memset((char*)ReadData.byfilename,0,sizeof(ReadData.byfilename));
		 memcpy((char*)&ReadData.byfilename,(char*)filename,filenamelen);

		 SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		 result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		 if (result==SOCKET_ERROR)
		 {
			 ssa_error_code=ERROR_DATA_SEND_FAILED;
			 closesocket(sockClient);
			 return FALSE;
		 }

		 FD_ZERO(&ReadSet);
		 FD_SET(sockClient,&ReadSet);
		 TimeOut.tv_sec = TIMEOUT;
		 TimeOut.tv_usec = 0;
		 result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		 if (result>0)
		 {
			 result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			 if ((result==0) || (result==SOCKET_ERROR))
			 {
				 ssa_error_code=ERROR_DATA_RECV_FAILED;
				 closesocket(sockClient);
				 return FALSE;
			 }
			 if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			 {
				 ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				 closesocket(sockClient);
				 return FALSE;
			 }

			 memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));
			 if (UploadData.dwStartAddr!=ReadData.dwStartAddr)
			 {
				 closesocket(sockClient);
				 return FALSE;
			 }

			 info.append((char*)UploadData.byFileData,UploadData.wDataLength);

			 cnt+=UploadData.wDataLength;
			 if (UploadData.byLastPackage==1)
			 {
				 DownOver=TRUE;
			 }

			 curpa++;
			 m_DownloadProg=UploadData.byUpdateProg;
		 }
		 else
		 {
			 ssa_error_code=ERROR_RECV_NO_DATA;
			 closesocket(sockClient);
			 return FALSE;
		 }

	 }

	 ssa_error_code=ERROR_NO;
	 closesocket(sockClient);
	 return TRUE;

 }

BOOL CDevUpdate::Download_File(WORD dev,unsigned char *ip,unsigned char* filename,unsigned int filenamelen,string dest,unsigned int& cnt)
{
	fd_set ReadSet;
	timeval TimeOut;
	DOWNLOAD_FILE ReadData;
	UPLOAD_FILE_DATA UploadData;
	char RecvBuf[2000];
	int result;

	unsigned int curpa=0;//当前包号
	WORD curLen=PACKET;//当前包大小
	BOOL DownOver=FALSE;

	cnt=0;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_DOWNLOADFILE);

	SetAddrFromIP(ip);

	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	while (DownOver==FALSE)
	{
		ReadData.dwStartAddr=curpa;
		ReadData.wDataLength=curLen;
		memset((char*)ReadData.byRes,0,2);
		memset((char*)ReadData.byfilename,0,sizeof(ReadData.byfilename));
		memcpy((char*)&ReadData.byfilename,(char*)filename,filenamelen);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = TIMEOUT;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_UPLOAD_DATA))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));
			if (UploadData.dwStartAddr!=ReadData.dwStartAddr)
			{
				closesocket(sockClient);
				return FALSE;
			}

			dest.append((char*)&UploadData.byFileData,UploadData.wDataLength);

			cnt+=UploadData.wDataLength;
			if (UploadData.byLastPackage==1)
			{
				DownOver=TRUE;
			}

			curpa++;
			m_DownloadProg=UploadData.byUpdateProg;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BYTE CDevUpdate::GetUploadProg()
{
	return m_UploadProg;

}

BYTE CDevUpdate::GetDownloadProg()
{
	return m_DownloadProg;
}


BYTE CDevUpdate::GetReadFileProg()
{
	return m_ReadFileProg;
}

unsigned int CDevUpdate::GetIsuImageWrite()
{
	return m_IsuImageWrite;
}

void CDevUpdate::ResetFileProg()
{
	m_DownloadProg=0;
	m_UploadProg=0;
	m_ReadFileProg=0;
}

BOOL CDevUpdate::AcsuCommPack(unsigned char comm,unsigned int addr,unsigned short cnt,unsigned char *data,ACSU_CTRL *dest)
{
	if (cnt>256 || cnt<0)
	{
		return FALSE;
	}
	if (data==NULL && cnt!=0)
	{
		return FALSE;
	}
	if (data!=NULL && cnt==0)
	{
		return FALSE;
	}
	memcpy(dest->bySynData,SYN_BYTE,ACSU_SYN_LEN);
	dest->byComm=comm;
	if (addr==0)
	{
		memset(dest->byAddr,0,3);
	}
	else
		memcpy(dest->byAddr,&addr,3);

	if (data==NULL && cnt==0)
	{
		memset(dest->byData,0,256);
	}
	else
	{
		memset(dest->byData,0,256);
		memcpy(dest->byData,data,cnt);
	}
	return TRUE;

}

int CDevUpdate::AcsuRespParse(ACSU_RESP data)
{
	if (data.byStart==0xf0 && data.byComm==0xc3)
	{
		return 0;
	}
	else if (data.byStart==0xf0 && data.byComm==0x60)
	{
		return FPGA_ACK;
	}
	else if (data.byStart==0xf0 && data.byComm==0x70)
	{
		return COMM_ACK;
	}
	else
		return 0;

}

BOOL CDevUpdate::CheckASW(SOCKET sock)
{
	fd_set ReadSet;
	timeval TimeOut;
	int result;
	char RecvBuf[100];

	ACSU_RESP_COMM COMMASW;

	FD_ZERO(&ReadSet);
	FD_SET(sock,&ReadSet);
	TimeOut.tv_sec = 7;
	TimeOut.tv_usec = 0;
	result = select(0, &ReadSet, NULL,NULL, &TimeOut);
	if (result>0)
	{
		result=recv(sock,RecvBuf,sizeof(RecvBuf),0);
		if ((result==0) || (result==SOCKET_ERROR))
		{
			ssa_error_code=ERROR_DATA_RECV_FAILED;
			return 0;
		}
		if(!RecvExtractor.Parser(RecvBuf,result,MSG_ACTRL_FPGA_ACK))
		{
			ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
			return 0;
		}

		memcpy((char*)&COMMASW,(char*)RecvExtractor.GetData().c_str(),sizeof(COMMASW));

		if (AcsuRespParse(COMMASW.byData)==COMM_ACK)
		{
			ssa_error_code=ERROR_NO;
			return TRUE;		
		}
		else
		{
			ssa_error_code=ERROR_ACSU_RESPPARSE_WRONG;
			return FALSE;
		}
	}
	else
	{
		ssa_error_code=ERROR_RECV_NO_DATA;
		return FALSE;
	}	
}

int CDevUpdate::GetAcsuMode(WORD dev,unsigned char *ip)
{
	fd_set ReadSet;
	timeval TimeOut;

	ACSU_CTRL_COMM CheckMode;
	ACSU_RESP_COMM CheckResult;

	char RecvBuf[100];
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return 0;
	}

	SendExtractor.SetDevMsg(dev,MSG_ACTRL_FPGA);
	memset(&CheckMode,0,sizeof(ACSU_CTRL_COMM));
	CheckMode.byLastPackage=1;
	CheckMode.wDataLength=ACSU_CTRL_LEN;
	AcsuCommPack(RDSR,0,0,NULL,&CheckMode.byData);

	SendExtractor.OrgPacket((char*)&CheckMode,sizeof(CheckMode));

	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	int re=0;
	while (re<5)
	{
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return 0;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = 5;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return 0;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_ACTRL_FPGA_ACK))
			{
				ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
				closesocket(sockClient);
				return 0;
			}

			memcpy((char*)&CheckResult,(char*)RecvExtractor.GetData().c_str(),sizeof(CheckResult));

			if (AcsuRespParse(CheckResult.byData)==FPGA_ACK)
			{
				ssa_error_code=ERROR_NO;
				closesocket(sockClient);
				return 1;		
			}
			else
			{
				re++;
			}
		}
		else
		{
			re++;
		}	

	}
	if (re==5)
	{
		ssa_error_code=ERROR_NO;
		closesocket(sockClient);
		return 2;
	}

}

BOOL CDevUpdate::SetAcsuToBoot(WORD dev,unsigned char *ip)
{
	ACSU_CTRL_COMM SetToBoot;

	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_ACTRL_FPGA);
	memset(&SetToBoot,0,sizeof(ACSU_CTRL_COMM));
	SetToBoot.byLastPackage=1;
	SetToBoot.wDataLength=ACSU_CTRL_LEN;
	AcsuCommPack(REBOOT,0,0,NULL,&SetToBoot.byData);
	SendExtractor.OrgPacket((char*)&SetToBoot,sizeof(SetToBoot));
	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}	
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	//Sleep(1000);
	
	AcsuCommPack(UPDATE,0,0,NULL,&SetToBoot.byData);
	SendExtractor.OrgPacket((char*)&SetToBoot,sizeof(SetToBoot));
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	Sleep(1000);
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	Sleep(1000);
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}
	Sleep(1000);
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}



BOOL CDevUpdate::SetAcsuToNormal(WORD dev,unsigned char *ip)
{
	ACSU_CTRL_COMM SetToNormal;
	int result;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_ACTRL_FPGA);
	memset(&SetToNormal,0,sizeof(ACSU_CTRL_COMM));
	SetToNormal.byLastPackage=1;
	SetToNormal.wDataLength=ACSU_CTRL_LEN;
	AcsuCommPack(REBOOT,0,0,NULL,&SetToNormal.byData);
	SendExtractor.OrgPacket((char*)&SetToNormal,sizeof(SetToNormal));
	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::ReadAcsuFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned int cnt,unsigned char *dest)
{
	fd_set ReadSet;
	timeval TimeOut;

	ACSU_CTRL_COMM ReadData;
	ACSU_RESP_COMM UploadData;
	char RecvBuf[100];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数

	packet=cnt/RCVRP;
	last=cnt-packet*RCVRP;
	if (last!=0)
	{
		packet++;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_ACTRL_FPGA);
	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	memset(&ReadData,0,sizeof(ACSU_CTRL_COMM));
	ReadData.wDataLength=ACSU_CTRL_LEN;

	while (curpa<packet)
	{
		if ((curpa+1)==packet)
		{
			ReadData.byLastPackage=1;
		}
		ReadData.dwPacketNo=curpa;
		AcsuCommPack(READ,addr+curpa*0x20,0,NULL,&ReadData.byData);

		SendExtractor.OrgPacket((char*)&ReadData,sizeof(ReadData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}

		FD_ZERO(&ReadSet);
		FD_SET(sockClient,&ReadSet);
		TimeOut.tv_sec = 5;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if (result>0)
		{
			result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
			if ((result==0) || (result==SOCKET_ERROR))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}
			if(!RecvExtractor.Parser(RecvBuf,result,MSG_ACTRL_FPGA_ACK))
			{
				ssa_error_code=ERROR_DATA_RECV_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			memcpy((char*)&UploadData,(char*)RecvExtractor.GetData().c_str(),sizeof(UploadData));
			if (AcsuRespParse(UploadData.byData)!=FPGA_ACK)
			{
				ssa_error_code=ERROR_ACSU_RESPPARSE_WRONG;
				closesocket(sockClient);
				return FALSE;
			}

			if (UploadData.dwPacketNo!=ReadData.dwPacketNo)
			{
				closesocket(sockClient);
				return FALSE;
			}
			if ((curpa+1)==packet && last!=0)
			{
				memcpy((char*)&dest[curpa*RCVRP],(char*)&UploadData.byData.byData,last);
			}
			else
				memcpy((char*)&dest[curpa*RCVRP],(char*)&UploadData.byData.byData,RCVRP);

			curpa++;

			m_ReadProg=curpa*100/packet;
		}
		else
		{
			ssa_error_code=ERROR_RECV_NO_DATA;
			closesocket(sockClient);
			return FALSE;
		}
	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

BOOL CDevUpdate::EraseAcsuFlash(WORD dev,unsigned char *ip,DWORD start_sec,DWORD end_sec)
{
	fd_set ReadSet;
	timeval TimeOut;

	ACSU_CTRL_COMM EraseSection;
	ACSU_RESP_COMM EraseStatus;

	char RecvBuf[100];
	int result;


	BOOL timeout=FALSE;
	BOOL end_erase=FALSE;

	int MP;
	char dir;
	if (end_sec>=start_sec)
	{
		MP=(end_sec-start_sec)/0x10000+1;
		dir=1;

	}
	else if (start_sec>=end_sec)
	{
		MP=(start_sec-end_sec)/0x10000+1;
		dir=-1;
	}
	int cursec=0;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}
	int bufsize=320;
	setsockopt(sockClient,SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));

	SendExtractor.SetDevMsg(dev,MSG_ACTRL_FPGA);
	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	memset(&EraseSection,0,sizeof(ACSU_CTRL_COMM));
	EraseSection.byLastPackage=1;
	EraseSection.wDataLength=ACSU_CTRL_LEN;
	AcsuCommPack(UPDATE,0,0,NULL,&EraseSection.byData);
	SendExtractor.OrgPacket((char*)&EraseSection,sizeof(EraseSection));
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	while (cursec<MP)
	{
		AcsuCommPack(WREN,0,0,NULL,&EraseSection.byData);
		SendExtractor.OrgPacket((char*)&EraseSection,sizeof(EraseSection));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!CheckASW(sockClient))
		{
			closesocket(sockClient);
			return FALSE;
		}

		AcsuCommPack(SE,start_sec+dir*cursec*0x10000,0,NULL,&EraseSection.byData);
		SendExtractor.OrgPacket((char*)&EraseSection,sizeof(EraseSection));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!CheckASW(sockClient))
		{
			closesocket(sockClient);
			return FALSE;
		}

		//erase complete or not
		while(1)
		{
			AcsuCommPack(RDSR,0,0,NULL,&EraseSection.byData);
			SendExtractor.OrgPacket((char*)&EraseSection,sizeof(EraseSection));
			result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
			if (result==SOCKET_ERROR)
			{
				ssa_error_code=ERROR_DATA_SEND_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			FD_ZERO(&ReadSet);
			FD_SET(sockClient,&ReadSet);
			TimeOut.tv_sec = 3;
			TimeOut.tv_usec = 0;
			result = select(0, &ReadSet, NULL,NULL, &TimeOut);
			if (result>0)
			{
				result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
				if ((result==0) || (result==SOCKET_ERROR))
				{
					ssa_error_code=ERROR_DATA_RECV_FAILED;
					closesocket(sockClient);
					return FALSE;
				}
				if(!RecvExtractor.Parser(RecvBuf,result,MSG_ACTRL_FPGA_ACK))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					closesocket(sockClient);
					return FALSE;
				}

				memcpy((char*)&EraseStatus,(char*)RecvExtractor.GetData().c_str(),sizeof(EraseStatus));
				if (AcsuRespParse(EraseStatus.byData)==FPGA_ACK)
				{
					if((EraseStatus.byData.byData[0] &0x1)==0)
					{			
						end_erase=TRUE;
						break;
					}
					else
					{
						end_erase=FALSE;
					}			
				}
				else
				{
					end_erase=FALSE;
				}
				
			}
			else
			{
				ssa_error_code=ERROR_RECV_NO_DATA;
				closesocket(sockClient);
				return FALSE;
			}
			Sleep(5);

		}

		cursec++;
		m_EraseProg=cursec*100/MP;

	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;

}

BOOL CDevUpdate::WriteAcsuFlash(WORD dev,unsigned char *ip,DWORD addr,unsigned char *data,unsigned int cnt)
{
	fd_set ReadSet;
	timeval TimeOut;

	ACSU_CTRL_COMM WriteData;
	ACSU_RESP_COMM WriteStatus;

	char RecvBuf[100];
	int result;

	unsigned int packet;//一共几包
	unsigned int curpa=0;//当前包号
	unsigned int last;//最后一包字节数
	WORD curLen=ACSUP;//当前包大小

	packet=cnt/ACSUP;
	last=cnt-packet*ACSUP;
	if (last!=0)
	{
		//last=ACSUP;
		packet++;
	}

	BOOL timeout=FALSE;
	BOOL end_erase=FALSE;

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (sockClient==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		return FALSE;
	}

	SendExtractor.SetDevMsg(dev,MSG_ACTRL_FPGA);
	SetAddrFromIP(ip);
	connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	memset(&WriteData,0,sizeof(ACSU_CTRL_COMM));
	WriteData.byLastPackage=1;
	WriteData.wDataLength=ACSU_CTRL_LEN;
	AcsuCommPack(UPDATE,0,0,NULL,&WriteData.byData);
	SendExtractor.OrgPacket((char*)&WriteData,sizeof(WriteData));
	result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		closesocket(sockClient);
		return FALSE;
	}
	if(!CheckASW(sockClient))
	{
		closesocket(sockClient);
		return FALSE;
	}

	while (curpa<packet)
	{
		if ((last!=0) &&(curpa==packet-1))//last packet
		{
			curLen=last;
		}

		AcsuCommPack(WREN,0,0,NULL,&WriteData.byData);
		SendExtractor.OrgPacket((char*)&WriteData,sizeof(WriteData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!CheckASW(sockClient))
		{
			closesocket(sockClient);
			return FALSE;
		}

		AcsuCommPack(PP,addr+curpa*ACSUP,curLen,&data[curpa*ACSUP],&WriteData.byData);
		SendExtractor.OrgPacket((char*)&WriteData,sizeof(WriteData));
		result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			closesocket(sockClient);
			return FALSE;
		}
		if(!CheckASW(sockClient))
		{
			closesocket(sockClient);
			return FALSE;
		}

		//write complete or not
		while(1)
		{
			AcsuCommPack(RDSR,0,0,NULL,&WriteData.byData);
			SendExtractor.OrgPacket((char*)&WriteData,sizeof(WriteData));
			result=send(sockClient,(char*)SendExtractor.GetPacket().c_str(),SendExtractor.GetPacketSize(),0);
			if (result==SOCKET_ERROR)
			{
				ssa_error_code=ERROR_DATA_SEND_FAILED;
				closesocket(sockClient);
				return FALSE;
			}

			FD_ZERO(&ReadSet);
			FD_SET(sockClient,&ReadSet);
			TimeOut.tv_sec =3;
			TimeOut.tv_usec = 0;
			result = select(0, &ReadSet, NULL,NULL, &TimeOut);
			if (result>0)
			{
				result=recv(sockClient,RecvBuf,sizeof(RecvBuf),0);
				if ((result==0) || (result==SOCKET_ERROR))
				{
					ssa_error_code=ERROR_DATA_RECV_FAILED;
					closesocket(sockClient);
					return FALSE;
				}
				if(!RecvExtractor.Parser(RecvBuf,result,MSG_ACTRL_FPGA_ACK))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					closesocket(sockClient);
					return FALSE;
				}

				memcpy((char*)&WriteStatus,(char*)RecvExtractor.GetData().c_str(),sizeof(WriteStatus));
				if (AcsuRespParse(WriteStatus.byData)!=FPGA_ACK)
				{
					ssa_error_code=ERROR_ACSU_RESPPARSE_WRONG;
					closesocket(sockClient);
					return FALSE;
				}
				if((WriteStatus.byData.byData[0] &0x1)==0)
				{			
					end_erase=TRUE;
					break;
				}
				else
				{
					end_erase=FALSE;
				}			
			}
			else
			{
				ssa_error_code=ERROR_RECV_NO_DATA;
				closesocket(sockClient);
				return FALSE;
			}
		}

		Sleep(5);
		curpa++;
		m_WriteProg=curpa*100/packet;


	}

	ssa_error_code=ERROR_NO;
	closesocket(sockClient);
	return TRUE;
}

void CDevUpdate::SetVersionInfo(int info)
{
	m_IsOldVersion=info;
}























