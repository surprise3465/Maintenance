#include "stdafx.h"
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include "DevInformation.h"

CDevInformation::CDevInformation()
{
	m_Type=0;
	m_devcnt = 0;
	m_devinfo=NULL;
}

CDevInformation::~CDevInformation()
{
	if (m_devinfo!=NULL)
	{
		free(m_devinfo);
		m_devinfo=NULL;
	}
}

BOOL CDevInformation::DevInfoInit(WORD gen_prot,WORD pro_prot,WORD proID,int type,PSINFO_ADAPTER adaterinfo)
{
	m_Type=type;
	WORD wVersionRequested; 
	WSADATA wsaData;
	int result;

	wVersionRequested = MAKEWORD( 2, 2 );
	result = WSAStartup( wVersionRequested, &wsaData ); 
	if ( result != 0 ) 
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

	BOOL flag = TRUE;
	struct sockaddr_in localaddr;

	if (m_Type==0)
	{
		sockDTLMaint = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockDTLMaint == SOCKET_ERROR) 
		{
			ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
			WSACleanup(); 
			return FALSE;
		}	

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(UDP_SENDSRC);
		if (adaterinfo==NULL)
		{
			localaddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			localaddr.sin_addr.s_addr = inet_addr(adaterinfo->info_ip.note_ip[0].szIPAddrStr);
		}
		

		result = bind(sockDTLMaint, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if(result == SOCKET_ERROR ) 
		{	
			ssa_error_code=ERROR_SOCKET_BIND_FAILED;
			closesocket(sockDTLMaint);
			WSACleanup(); 
			return FALSE;

		}

		sockSW = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockSW == SOCKET_ERROR) 
		{
			ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
			closesocket(sockDTLMaint);
			WSACleanup(); 
			return FALSE;
		}

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(UDP_RECVDES);
		if(adaterinfo==NULL)
		{
			localaddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			localaddr.sin_addr.s_addr = inet_addr(adaterinfo->info_ip.note_ip[0].szIPAddrStr);
		}
		

		result = bind(sockSW, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if(result == SOCKET_ERROR ) 
		{
			ssa_error_code=ERROR_SOCKET_BIND_FAILED;
			closesocket(sockDTLMaint);
			closesocket(sockSW);
			WSACleanup(); 
			return FALSE;

		}

	}
	else if (m_Type==1)
	{
		sockDTLUpdate = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockDTLUpdate == SOCKET_ERROR) 
		{ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
			WSACleanup(); 
			return FALSE;
		}

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(UDP_RECVDES_UPDATE);
		if (adaterinfo==NULL)
		{
			localaddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			localaddr.sin_addr.s_addr = inet_addr(adaterinfo->info_ip.note_ip[0].szIPAddrStr);
		}
		
		result = bind(sockDTLUpdate, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if(result == SOCKET_ERROR ) 
		{
			ssa_error_code=ERROR_SOCKET_BIND_FAILED;
			closesocket(sockDTLUpdate);
			WSACleanup(); 
			return FALSE;
		}
	}

	SendExtractor.SetProt(gen_prot,pro_prot,proID);
	RecvExtractor.SetProt(gen_prot,pro_prot,proID);

	ssa_error_code=ERROR_NO;
	return TRUE;

}

BOOL CDevInformation::DevInfoInitNew(WORD gen_prot,WORD pro_prot,WORD pro_protsw,WORD proID,int type,char* adaterinfo)
{
	m_Type=type;
	WORD wVersionRequested; 
	WSADATA wsaData;
	int result;
	int i=0;

	wVersionRequested = MAKEWORD( 2, 2 );
	result = WSAStartup( wVersionRequested, &wsaData ); 
	if ( result != 0 ) 
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

	BOOL flag = TRUE;
	int nRecvBuf=256*1024;//设置为32K
	struct sockaddr_in localaddr;

	if (m_Type==0)
	{
		sockDTLMaint = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockDTLMaint == SOCKET_ERROR) 
		{
			ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
			WSACleanup(); 
			return FALSE;
		}	

		result= setsockopt(sockDTLMaint, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
		if (result == SOCKET_ERROR)
		{
			closesocket(sockDTLMaint);
			return FALSE;
		}

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(UDP_SENDSRC);
		if (adaterinfo== nullptr)
		{
			localaddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
             localaddr.sin_addr.s_addr = inet_addr(adaterinfo);
		}

		

		result = bind(sockDTLMaint, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if(result == SOCKET_ERROR ) 
		{	
			ssa_error_code=ERROR_SOCKET_BIND_FAILED;
			closesocket(sockDTLMaint);
			WSACleanup(); 
			return FALSE;
		}

		sockSW = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockSW == SOCKET_ERROR) 
		{
			ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
			closesocket(sockDTLMaint);
			WSACleanup(); 
			return FALSE;
		}

		result= setsockopt(sockSW, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
		if (result == SOCKET_ERROR)
		{
			closesocket(sockSW);
			return FALSE;
		}

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(UDP_RECVDES);
		if(adaterinfo== nullptr)
		{
			localaddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			localaddr.sin_addr.s_addr = inet_addr(adaterinfo);
		}
		
		result = bind(sockSW, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if(result == SOCKET_ERROR ) 
		{
			ssa_error_code=ERROR_SOCKET_BIND_FAILED;
			closesocket(sockDTLMaint);
			closesocket(sockSW);
			WSACleanup(); 
			return FALSE;
		}	
	}
	else if (m_Type==1)
	{
		sockDTLUpdate = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockDTLUpdate == SOCKET_ERROR) 
		{ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
			WSACleanup(); 
			return FALSE;
		}

		result= setsockopt(sockDTLUpdate, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));

		if (result == SOCKET_ERROR)
		{
			closesocket(sockDTLUpdate);
			return FALSE;
		}

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(UDP_RECVDES_UPDATE);
		if (adaterinfo==NULL)
		{
			localaddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			localaddr.sin_addr.s_addr = inet_addr(adaterinfo);
		}
		
		result = bind(sockDTLUpdate, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if(result == SOCKET_ERROR ) 
		{
			ssa_error_code=ERROR_SOCKET_BIND_FAILED;
			closesocket(sockDTLUpdate);
			WSACleanup(); 
			return FALSE;
		}
	}

	SendExtractor.SetProtNew(gen_prot,pro_prot,proID,pro_protsw);
	RecvExtractor.SetProtNew(gen_prot,pro_prot,proID,pro_protsw);

	ssa_error_code=ERROR_NO;
	return TRUE;

}

BOOL CDevInformation::Dev368Init()
{
	socksecondip = socket(AF_INET, SOCK_DGRAM, 0);
	if(socksecondip == SOCKET_ERROR) 
	{
		ssa_error_code=ERROR_SOCKET_CREATE_FAILED;
		WSACleanup(); 
		return FALSE;
	}

	struct sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(6666);
    localaddr.sin_addr.s_addr =inet_addr("193.166.1.10");

	int result;
	result = bind(socksecondip, (struct sockaddr*)&localaddr, sizeof(localaddr));
	if(result == SOCKET_ERROR ) 
	{
		ssa_error_code=ERROR_SOCKET_BIND_FAILED;
		closesocket(socksecondip);
		WSACleanup(); 
		return FALSE;
	}

    return TRUE;;
}

void CDevInformation::DevInfoClose()
{
	if (m_Type==0)
	{
		closesocket(sockDTLMaint);
		closesocket(sockSW);
        closesocket(socksecondip);
	}
	else if (m_Type==1)
	{
		closesocket(sockDTLUpdate);
	}
		
	WSACleanup();
}

void CDevInformation::Dev368Close()
{
    closesocket(socksecondip);
}

BOOL CDevInformation::GetDevInformation(WORD dev,string& info)
{
	struct sockaddr_in DevMultiPollAddr;
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;

	int addrLen;
	addrLen = sizeof(DevMultiPollAddr);
	fd_set ReadSet;
	timeval TimeOut;

	DevMultiPollAddr.sin_family = AF_INET;
	DevMultiPollAddr.sin_port = htons(UDP_SENDDES);
	DevMultiPollAddr.sin_addr.s_addr = inet_addr(UDP_ADDR);


	DEV_INFORMATION DevInfo;
	SendExtractor.SetDevMsg(dev,MSG_REQUEST_INFO);
	SendExtractor.OrgPacket(NULL,0);

	if (m_Type==0)
	{
		result = sendto(sockDTLMaint, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
			(struct sockaddr*)&DevMultiPollAddr,addrLen);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			return FALSE;
		}
	}
	else if (m_Type==1)
	{
		result = sendto(sockDTLUpdate, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
			(struct sockaddr*)&DevMultiPollAddr,addrLen);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			return FALSE;
		}
	}

	while (1)
	{	
		FD_ZERO(&ReadSet);//每次循环都要清空，否则不能检测描述符变化
		if (m_Type==0)
		{
			FD_SET(sockDTLMaint,&ReadSet);
		}
		else if (m_Type==1)
		{
			FD_SET(sockDTLUpdate,&ReadSet);
		}

		TimeOut.tv_sec = 0;
		TimeOut.tv_usec = 1500*1000;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			if (m_Type==0)
			{
				nread = recvfrom(sockDTLMaint, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			}
			else if (m_Type==1)
			{
				nread = recvfrom(sockDTLUpdate, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			}

			if(nread>0)
			{
				if(!RecvExtractor.Parser(RecvBuf,nread,MSG_ANSWER_INFO))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					return FALSE;
				}
				unsigned char ip[4];
				ip[0]=fromaddr.sin_addr.S_un.S_un_b.s_b1;
				ip[1]=fromaddr.sin_addr.S_un.S_un_b.s_b2;
				ip[2]=fromaddr.sin_addr.S_un.S_un_b.s_b3;
				ip[3]=fromaddr.sin_addr.S_un.S_un_b.s_b4;
				memcpy((char*)&DevInfo,(char*)RecvExtractor.GetData().c_str(),sizeof(DevInfo));
				unsigned char head[20];
				RecvExtractor.GetInterHead(head);
				info.append((char*)head,INTER_LEN);
				info.append((char*)&DevInfo,DEVINFO_LEN);
                info.append((char*)ip,IP_LEN);

			} 
		}
		else
		{
			break;
		}
	}

	ssa_error_code=ERROR_NO;
	return TRUE;

}

int CDevInformation::GetDevInformationNew(WORD dev,int t)
{
	struct sockaddr_in DevMultiPollAddr;
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;
	string info;

	int addrLen;
	addrLen = sizeof(DevMultiPollAddr);
	fd_set ReadSet;
	timeval TimeOut;

	DevMultiPollAddr.sin_family = AF_INET;
	DevMultiPollAddr.sin_port = htons(UDP_SENDDES);
	DevMultiPollAddr.sin_addr.s_addr = inet_addr(UDP_ADDR);


	DEV_INFORMATION DevInfo;
	SendExtractor.SetDevMsg(dev,MSG_REQUEST_INFO);
	SendExtractor.OrgPacket(NULL,0);

	if (m_Type==0)
	{
		result = sendto(sockDTLMaint, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
			(struct sockaddr*)&DevMultiPollAddr,addrLen);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			return FALSE;
		}
	}
	else if (m_Type==1)
	{
		result = sendto(sockDTLUpdate, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
			(struct sockaddr*)&DevMultiPollAddr,addrLen);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			return FALSE;
		}
	}

	while (1)
	{	

		FD_ZERO(&ReadSet);//每次循环都要清空，否则不能检测描述符变化
		if (m_Type==0)
		{
			FD_SET(sockDTLMaint,&ReadSet);
		}
		else if (m_Type==1)
		{
			FD_SET(sockDTLUpdate,&ReadSet);
		}
		
		TimeOut.tv_sec = 0;
		TimeOut.tv_usec = 2500*1000;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			info.clear();

			if (m_Type==0)
			{
				nread = recvfrom(sockDTLMaint, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			}
			else if (m_Type==1)
			{
				nread = recvfrom(sockDTLUpdate, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			}
			
			if(nread>0)
			{
				if(!RecvExtractor.Parser(RecvBuf,nread,MSG_ANSWER_INFO))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					return FALSE;
				}
				unsigned char ip[4];
				ip[0]=fromaddr.sin_addr.S_un.S_un_b.s_b1;
				ip[1]=fromaddr.sin_addr.S_un.S_un_b.s_b2;
				ip[2]=fromaddr.sin_addr.S_un.S_un_b.s_b3;
				ip[3]=fromaddr.sin_addr.S_un.S_un_b.s_b4;
				memcpy((char*)&DevInfo,(char*)RecvExtractor.GetData().c_str(),sizeof(DevInfo));
				unsigned char head[20];
				RecvExtractor.GetInterHead(head);
				info.append((char*)head,INTER_LEN);
				info.append((char*)ip,IP_LEN);
				
				memcpy((char*)&m_devinfo[t],(char*)info.c_str(),16);			

                RecvExtractor.StoreDevInfo(&DevInfo,&m_devinfo[t]);

				t++;

			} 
		}
		else
		{
			m_devcnt=t;
			break;
		}
	}

	ssa_error_code=ERROR_NO;
	//return TRUE;
	return m_devcnt;


}

BOOL CDevInformation::GetSWInformation(WORD dev,string& info)
{
	struct sockaddr_in DevMultiPollAddr;
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;

	int addrLen;
	addrLen = sizeof(DevMultiPollAddr);
	fd_set ReadSet;
	timeval TimeOut;

	DevMultiPollAddr.sin_family = AF_INET;
	DevMultiPollAddr.sin_port = htons(UDP_SENDDES);
	DevMultiPollAddr.sin_addr.s_addr = inet_addr(UDP_ADDR);


	DEV_INFORMATION DevInfo;
	SendExtractor.SetDevMsg(dev,MSG_REQUEST_INFO);
	SendExtractor.OrgPack(NULL,0);

	result = sendto(sockDTLMaint, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
		(struct sockaddr*)&DevMultiPollAddr,addrLen);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		return FALSE;
	}

	while (1)
	{
		FD_ZERO(&ReadSet);
		FD_SET(sockSW,&ReadSet);
		TimeOut.tv_sec = 0;
		TimeOut.tv_usec = 1500*1000;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			nread = recvfrom(sockSW, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			if(nread>0)
			{
				if(!RecvExtractor.Parser(RecvBuf,nread,MSG_ANSWER_INFO))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					return FALSE;
				}
				unsigned char ip[4];
				ip[0]=fromaddr.sin_addr.S_un.S_un_b.s_b1;
				ip[1]=fromaddr.sin_addr.S_un.S_un_b.s_b2;
				ip[2]=fromaddr.sin_addr.S_un.S_un_b.s_b3;
				ip[3]=fromaddr.sin_addr.S_un.S_un_b.s_b4;

				memcpy((char*)&DevInfo,(char*)RecvExtractor.GetData().c_str(),sizeof(DevInfo));
				unsigned char head[20];
				RecvExtractor.GetInterHead(head);
				info.append((char*)head,INTER_LEN);
				info.append((char*)&DevInfo,DEVINFO_LEN);
				info.append((char*)ip,IP_LEN);

			}
		}
		else
		{
			break;
		}
	}

	ssa_error_code=ERROR_NO;
	return TRUE;
}

BOOL CDevInformation::GetSWInformationNew(WORD dev,int t)
{
	struct sockaddr_in DevMultiPollAddr;
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;

	string info;

	int addrLen;
	addrLen = sizeof(DevMultiPollAddr);
	fd_set ReadSet;
	timeval TimeOut;

	DevMultiPollAddr.sin_family = AF_INET;
	DevMultiPollAddr.sin_port = htons(UDP_SENDDES);
	DevMultiPollAddr.sin_addr.s_addr = inet_addr(UDP_ADDR);


	DEV_INFORMATION DevInfo;
	SendExtractor.SetDevMsg(dev,MSG_REQUEST_INFO);
	SendExtractor.OrgPack(NULL,0);

	result = sendto(sockDTLMaint, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
		(struct sockaddr*)&DevMultiPollAddr,addrLen);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		return FALSE;
	}

	while (1)
	{
		FD_ZERO(&ReadSet);
		FD_SET(sockSW,&ReadSet);
		TimeOut.tv_sec = 0;
		//TimeOut.tv_usec = 5000*1000;
		TimeOut.tv_usec = 1500*1000;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			nread = recvfrom(sockSW, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			if(nread>0)
			{
				if(!RecvExtractor.Parser(RecvBuf,nread,MSG_ANSWER_INFO))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					return FALSE;
				}
				unsigned char ip[4];
				ip[0]=fromaddr.sin_addr.S_un.S_un_b.s_b1;
				ip[1]=fromaddr.sin_addr.S_un.S_un_b.s_b2;
				ip[2]=fromaddr.sin_addr.S_un.S_un_b.s_b3;
				ip[3]=fromaddr.sin_addr.S_un.S_un_b.s_b4;

				memcpy((char*)&DevInfo,(char*)RecvExtractor.GetData().c_str(),sizeof(DevInfo));
				unsigned char head[20];
				RecvExtractor.GetInterHead(head);
				info.clear();
				info.append((char*)head,INTER_LEN);
				//info.append((char*)&DevInfo,DEVINFO_LEN);
                //info.append((char*)&DevInfo,nread);
				info.append((char*)ip,IP_LEN);

				/*if (t==0)
				{
					m_devinfo=(DEV*)malloc(sizeof(DEV));					
				}
				else
				{
					m_devinfo=(DEV*)realloc(m_devinfo,sizeof(DEV)*(t+1));
				}*/

				//RecvExtractor.StoreInterhead(info,&m_devinfo[t]);
				memcpy((char*)&m_devinfo[t],(char*)info.c_str(),16);

				RecvExtractor.StoreDevInfo(&DevInfo,&m_devinfo[t]);


				t++;
			}
		}
		else
		{
			m_devcnt=t;
			break;
		}
	}

	ssa_error_code=ERROR_NO;
	return TRUE;
}

BOOL CDevInformation::CustomCommandControl(unsigned int dev,unsigned int macid,unsigned short headertype,string& msgdata,string& comstring,string& recvstring,int timeout,unsigned int devip)
{
	struct sockaddr_in DevMultiPollAddr;
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;

	int addrLen;
	addrLen = sizeof(DevMultiPollAddr);
	fd_set ReadSet;
	timeval TimeOut;

	DevMultiPollAddr.sin_family = AF_INET;
	DevMultiPollAddr.sin_port = htons(UDP_SENDDES);
	char ip[16];
	sprintf(ip,"%d.%d.%d.%d",((devip>>24)&0xff),(devip>>16)&0xff,(devip>>8)&0xff,devip&0xff);
	DevMultiPollAddr.sin_addr.s_addr = inet_addr(ip);

	SendUmsgcommand.OrgPacket(macid,headertype,msgdata,comstring);

	SendExtractor.SetDevMsg(dev,MSG_USER_DEF_MSG);
	SendExtractor.OrgPacket((char*)SendUmsgcommand.m_packet.c_str(),SendUmsgcommand.m_packet.size());

	if (m_Type==0)
	{
		result = sendto(sockDTLMaint, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
			(struct sockaddr*)&DevMultiPollAddr,addrLen);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			return FALSE;
		}
	}
	else if (m_Type==1)
	{
		result = sendto(sockDTLUpdate, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
			(struct sockaddr*)&DevMultiPollAddr,addrLen);
		if (result==SOCKET_ERROR)
		{
			ssa_error_code=ERROR_DATA_SEND_FAILED;
			return FALSE;
		}
	}	

	while (1)
	{
		FD_ZERO(&ReadSet);
		if (m_Type==0)
		{
			FD_SET(sockDTLMaint,&ReadSet);
		}
		else if (m_Type==1)
		{
			FD_SET(sockDTLUpdate,&ReadSet);
		}
		TimeOut.tv_sec = timeout;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			if (m_Type==0)
			{
				nread = recvfrom(sockDTLMaint, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			}
			else if (m_Type==1)
			{
				nread = recvfrom(sockDTLUpdate, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			}
			
			if(nread>0)
			{
				if(!RecvExtractor.Parser(RecvBuf,nread,MSG_USER_DEF_MSG_ACK))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					return FALSE;
				}	
				else
				{
					if (!RecvUmsgcommand.Parser((char*)RecvExtractor.GetData().c_str(),RecvExtractor.GetData().size(),headertype))
					{
						ssa_error_code=ERROR_RECV_UMSG_HEADERTYPE_WRONG;
						return FALSE;
					}
					else
					{
						recvstring.append((char*)RecvUmsgcommand.m_Msgdata.byData,RecvUmsgcommand.m_data_length);
						ssa_error_code=ERROR_NO;
						return TRUE;
					}
					
				}
			}
		}
		else
		{
			break;
		}
	}

	ssa_error_code=ERROR_RECV_NO_DATA;
	return FALSE;

}

BOOL CDevInformation::SetDevIP(unsigned int dev,unsigned char *info,unsigned int cnt)
{
	struct sockaddr_in DevMultiPollAddr;
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;

	int addrLen;
	addrLen = sizeof(DevMultiPollAddr);
	fd_set ReadSet;
	timeval TimeOut;

	DevMultiPollAddr.sin_family = AF_INET;
	DevMultiPollAddr.sin_port = htons(UDP_SENDDES);
	DevMultiPollAddr.sin_addr.s_addr = inet_addr(UDP_ADDR);

	SendExtractor.SetDevMsg(dev,MSG_SET_IP);
	SendExtractor.OrgPacket(info,cnt);

	result = sendto(sockDTLMaint, SendExtractor.GetPacket().c_str(), (int)SendExtractor.GetPacketSize(), 0, 
		(struct sockaddr*)&DevMultiPollAddr,addrLen);
	if (result==SOCKET_ERROR)
	{
		ssa_error_code=ERROR_DATA_SEND_FAILED;
		return FALSE;
	}

	while (1)
	{
		FD_ZERO(&ReadSet);
		FD_SET(sockDTLMaint,&ReadSet);
		TimeOut.tv_sec = 25;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			nread = recvfrom(sockDTLMaint, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			if(nread>0)
			{
				if(!RecvExtractor.Parser(RecvBuf,nread,MSG_SET_IP_ACK))
				{
					ssa_error_code=ERROR_RECV_CHECKWRONG_OR_MSGTYPEWRONG;
					return FALSE;
				}	
				else
				{
					ssa_error_code=ERROR_NO;
					return TRUE;
				}
			}
		}
		else
		{
			break;
		}
	}

	ssa_error_code=ERROR_RECV_NO_DATA;
	return FALSE;

}

int CDevInformation::GetAdapterInfoList(SINFO_ADAPTER* list)
{
	char   tempChar; 
	ULONG   uListSize=1; 
	PIP_ADAPTER_INFO   pAdapter;                       //   定义PIP_ADAPTER_INFO结构存储网卡信息 
	int   nAdapterIndex   =   0; 
	int i=0;

	DWORD   dwRet   =   GetAdaptersInfo((PIP_ADAPTER_INFO)&tempChar,   &uListSize);//关键函数
	

	if   (dwRet   ==   ERROR_BUFFER_OVERFLOW) 
	{ 
		PIP_ADAPTER_INFO   pAdapterListBuffer   =   (PIP_ADAPTER_INFO)new(char[uListSize]); 
		dwRet   =   GetAdaptersInfo(pAdapterListBuffer,   &uListSize); 
		if   (dwRet   ==   ERROR_SUCCESS) 
		{ 
			pAdapter   =   pAdapterListBuffer; 
			while   (pAdapter)                                                                                             //   枚举网卡然后将相关条目添加到List中 
			{ 
				i=0;
				//   网卡名字 
				memcpy(list[nAdapterIndex].szDeviceName,pAdapter->Description,sizeof(list[nAdapterIndex].szDeviceName));
				//   IP 
				memcpy(list[nAdapterIndex].info_ip.note_ip[i].szIPAddrStr,pAdapter->IpAddressList.IpAddress.String,sizeof(list[nAdapterIndex].info_ip.note_ip[i].szIPAddrStr));
                list[nAdapterIndex].ipnum=1;

				if(pAdapter->IpAddressList.Next!=NULL)
				{
                   i++;
				   while(pAdapter->IpAddressList.Next)
				   {
                      memcpy(list[nAdapterIndex].info_ip.note_ip[i].szIPAddrStr,pAdapter->IpAddressList.Next->IpAddress.String,sizeof(list[nAdapterIndex].info_ip.note_ip[i].szIPAddrStr));
                      i++;
                      list[nAdapterIndex].ipnum++;
                      pAdapter->IpAddressList.Next=pAdapter->IpAddressList.Next->Next;
				   }
				}
				else
				{
                   list[nAdapterIndex].ipnum=1;
				}

				//   MAC 
				sprintf_s(list[nAdapterIndex].szHWAddrStr,"%02X-%02X-%02X-%02X-%02X-%02X",pAdapter->Address[0],\
					pAdapter->Address[1],pAdapter->Address[2],pAdapter->Address[3],pAdapter->Address[4],pAdapter->Address[5]);
				
				//   网卡编号 
				list[nAdapterIndex].dwIndex=pAdapter->Index;                   

				pAdapter   =   pAdapter-> Next; 
				nAdapterIndex   ++; 
				i++;
			} 
			delete   pAdapterListBuffer; 
		} 
	} 
	return nAdapterIndex;

}

void CDevInformation::ClearSocketRecv()
{
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;

	int addrLen;
	addrLen = sizeof(fromaddr);
	fd_set ReadSet;
	timeval TimeOut;

	FD_ZERO(&ReadSet);
	FD_SET(sockSW,&ReadSet);
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 1;
	while(1)
	{
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			recvfrom(sockSW, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
		}
		else
		{
			break;
		}
	}

	FD_ZERO(&ReadSet);
	FD_SET(sockDTLMaint,&ReadSet);
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 1;
	while(1)
	{
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			recvfrom(sockDTLMaint, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
		}
		else
		{
			break;
		}
	}

	FD_ZERO(&ReadSet);
	FD_SET(sockDTLUpdate,&ReadSet);
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 1;
	while(1)
	{
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			recvfrom(sockDTLUpdate, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
		}
		else
		{
			break;
		}
	}
}

int CDevInformation::NewUpdateMessage(string &info)
{
	struct sockaddr_in fromaddr;
	char RecvBuf[2000];
	int result;
	int nread;

	int addrLen;
	addrLen = sizeof(fromaddr);
	fd_set ReadSet;
	timeval TimeOut;

	while (1)
	{
		FD_ZERO(&ReadSet);
		FD_SET(socksecondip,&ReadSet);
		TimeOut.tv_sec = 80;
		TimeOut.tv_usec = 0;
		result = select(0, &ReadSet, NULL,NULL, &TimeOut);
		if(result > 0) 
		{
			nread = recvfrom(socksecondip, RecvBuf, 2000, 0, (struct sockaddr*)&fromaddr, (socklen_t*)&addrLen);
			if(nread>0)
			{
			    if(RecvBuf[0]!=0x0a)
				{
                   info.append((char*)RecvBuf,1);
				}
			}
			if(RecvBuf[0]==0x0a)
			{
               break;
			}

		}
		else
		{
			//break;
            return FALSE;
		}
	}

	return TRUE;
}
