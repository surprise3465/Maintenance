#include "stdafx.h"
#include "DevMsgParser.h"

CDevMsgParser::CDevMsgParser()
{
	m_DataBuffer.resize(1000);
	m_DataBuffer.clear();
	m_PacketMsg.resize(1000);
	m_PacketMsg.clear();
	m_DataCnt=0;
}

CDevMsgParser::~CDevMsgParser()
{
	m_DataBuffer.resize(0);
	m_PacketMsg.resize(0);

}

void CDevMsgParser::SetProt(WORD gen_prot,WORD pro_prot,WORD proID)
{
	m_ProtocolType=gen_prot;
	m_ProjectProtocol=pro_prot;
	m_ProjectNo=proID;
}

void CDevMsgParser::SetProtNew(WORD gen_prot,WORD pro_prot,WORD proID,WORD pro_protsw)
{
	m_ProtocolType=gen_prot;
	m_ProjectProtocol=pro_prot;
	m_ProjectNo=proID;
	m_ProjectProtocolSw=pro_protsw;
}


BYTE CDevMsgParser::CheckSum()
{
	BYTE checksum;
	string inter;
	inter.append((char*)&m_InterHeader,INTER_LEN);
	checksum=inter[0];
	for (unsigned int i=1;i<inter.size();i++)
	{
		checksum^=inter[i];
	}
	for (unsigned int i=0;i<m_DataBuffer.size();i++)
	{
		checksum^=m_DataBuffer[i];
	}

	return checksum;
}

WORD CDevMsgParser::GetPacketSize()
{
	//return m_DataCnt+COMM_LEN+INTER_LEN;
	return m_DataCnt+COMM_LEN+INTER_LEN;

}

void CDevMsgParser::SetDevMsg(WORD dev,DWORD msg)
{
	m_DeviceNo=dev;
	m_MsgType=msg;
}

void CDevMsgParser::CommHeader()
{
	m_CommHeader.wLenth=GetPacketSize();
	m_CommHeader.wProtocolType=m_ProtocolType;
	m_CommHeader.dyCheckSum=CheckSum();
	memcpy((char*)m_CommHeader.byRes,"SSA",strlen("SSA"));
}

void CDevMsgParser::InterHeader()
{
	m_InterHeader.wProjectNo=m_ProjectNo;
	m_InterHeader.wProjectProtocol=m_ProjectProtocol;
	m_InterHeader.wDeviceNo=m_DeviceNo;
	memset(m_InterHeader.byRes,0,2);
	m_InterHeader.dwMsgType=m_MsgType;
	
}

void CDevMsgParser::InterHead()
{
	m_InterHeader.wProjectNo=m_ProjectNo;
	//m_InterHeader.wProjectProtocol=m_ProjectProtocol;
    m_InterHeader.wProjectProtocol=m_ProjectProtocolSw;
	m_InterHeader.wDeviceNo=m_DeviceNo;
	memset(m_InterHeader.byRes,0,2);
	m_InterHeader.dwMsgType=m_MsgType;

}
void CDevMsgParser::SetDataBuf(void *src,unsigned int cnt)
{
	if (src==NULL)
	{
		m_DataBuffer.clear();
		return ;
	}
	m_DataBuffer.clear();
	m_DataBuffer.append((char*)src,cnt);//string 别用memcpy
}

void CDevMsgParser::OrgPacket(void *src,unsigned int cnt)
{
	SetDataBuf(src,cnt);
	SetDataCnt(cnt);
	InterHeader();
	CommHeader();

	m_PacketMsg.clear();
	m_PacketMsg.append((char*)&m_CommHeader,COMM_LEN);
	m_PacketMsg.append((char*)&m_InterHeader,INTER_LEN);
	m_PacketMsg.append((char*)m_DataBuffer.c_str(),m_DataBuffer.size());

}
void CDevMsgParser::OrgPack(void *src,unsigned int cnt)
{
	SetDataBuf(src,cnt);
	SetDataCnt(cnt);
	InterHead();
	CommHeader();

	m_PacketMsg.clear();
	m_PacketMsg.append((char*)&m_CommHeader,COMM_LEN);
	m_PacketMsg.append((char*)&m_InterHeader,INTER_LEN);
	m_PacketMsg.append((char*)m_DataBuffer.c_str(),m_DataBuffer.size());

}

const string& CDevMsgParser::GetPacket() const
{
	return m_PacketMsg;
}

const string& CDevMsgParser::GetData() const
{
	return m_DataBuffer;
}

void CDevMsgParser::GetInterHead(unsigned char *dest)
{
	memcpy((char*)dest,(char*)&m_InterHeader,INTER_LEN);
	return;
}

BOOL CDevMsgParser::Parser(char *buf,unsigned int cnt,DWORD msg)
{
	memcpy((char*)&m_CommHeader,buf,COMM_LEN);
	memcpy((char*)&m_InterHeader,&buf[COMM_LEN],INTER_LEN);
	m_DataBuffer.clear();
	m_DataBuffer.append(&buf[COMM_LEN+INTER_LEN],cnt-INTER_LEN-COMM_LEN);

	BYTE check=CheckSum();

	if (check!=m_CommHeader.dyCheckSum)
	{
		return FALSE;
	}
	if (msg!=GetMsgType())
	{
		return FALSE;
	}

	return TRUE;
}

int CDevMsgParser::Parser2(char *buf,unsigned int cnt,DWORD msg)
{
	memcpy((char*)&m_CommHeader,buf,COMM_LEN);
	memcpy((char*)&m_InterHeader,&buf[COMM_LEN],INTER_LEN);
	m_DataBuffer.clear();
	m_DataBuffer.append(&buf[COMM_LEN+INTER_LEN],cnt-INTER_LEN-COMM_LEN);

	BYTE check=CheckSum();

	if (check!=m_CommHeader.dyCheckSum)
	{
		return 0;
	}
	if (msg==GetMsgType())
	{
		return 2;
	}

	return 1;
}
DWORD CDevMsgParser::GetMsgType()
{
	return m_InterHeader.dwMsgType;
}

void CDevMsgParser::SetDataCnt(unsigned int cnt)
{
	m_DataCnt=cnt;
}

//void CDevMsgParser::StoreInterhead(string cp,DEV* devinfo)
//{	
//	memcpy((char*)(*devinfo).byProjectno,&cp[0],2);
//	memcpy((char*)(*devinfo).byprotol,&cp[2],2);
//	memcpy((char*)(*devinfo).bydevno,&cp[4],2);
//	memcpy((char*)(*devinfo).byRes3,&cp[6],2);
//	memcpy((char*)(*devinfo).message,&cp[8],4);
//    memcpy((char*)(*devinfo).byIp,&cp[12],4);
//
//}
void CDevMsgParser::StoreDevInfo(DEV_INFORMATION* de,DEV* devinfo)
{	
	memcpy((char*)(*devinfo).byMAC,(char*)(de->byMAC),6);
	memcpy((char*)(*devinfo).byRes,(char*)(de->byRes),2);
    (*devinfo).byMask=de->byMask;
    (*devinfo).byFpgaB=de->byFpgaB;
	(*devinfo).byFpgaA=de->byFpgaA;
	(*devinfo).byUboot=de->byUboot;
	(*devinfo).byUimage=de->byUimage;
	(*devinfo).byFileSys=de->byFileSys;
	(*devinfo).byApp=de->byApp;
	(*devinfo).byDaemon=de->byDaemon;
	(*devinfo).byHardVer=de->byHardVer;
	memcpy((char*)(*devinfo).byRes2,(char*)(de->byRes2),8);

	memset((*devinfo).proname,0,16);
	memset((*devinfo).devicename,0,16);
	memset((*devinfo).APP,0,32);
	memset((*devinfo).Sys_MP,0,32);
	memset((*devinfo).sn,0,21);
	memset((*devinfo).otherinf,0,32);

    
	string additversion;
	for(int i=0;i<sizeof(de->byinfo);i++)
	{
       if(de->byinfo[i]!=0)
	   {
          additversion+=de->byinfo[i];
	   }
	   else
	   {
		   break;
	   }
	}

    if(additversion!="")
	{
		int n=additversion.find("Prj");
		if(n!=-1)
		{
			n=additversion.find('\\');
            string aaaa=additversion.substr(4,n-4);
			for(int i=0;i<n-4;i++)
			{
               (*devinfo).proname[i]=aaaa[i];
			}
		}
		additversion.erase(0,n+4);
		n=additversion.find("Dev");
		if(n!=-1)
		{
			n=additversion.find('\\');
			string aaaa=additversion.substr(4,n-4);
			for(int i=0;i<n-4;i++)
			{
				(*devinfo).devicename[i]=aaaa[i];
			}
		}
		additversion.erase(0,n+4);

		n=additversion.find("Sys_MP");
		if(n!=-1)//有daemon
		{
			n=additversion.find('\\');
			string aaaa=additversion.substr(7,n-7);
			for(int i=0;i<n-7;i++)
			{
				(*devinfo).Sys_MP[i]=aaaa[i];
			}
		}
		additversion.erase(0,n+4);
		n=additversion.find("App");
		if(n!=-1)//有APP
		{
			n=additversion.find('\\');
			//(*devinfo).APP=additversion.substr(4,n-4);
			string aaaa=additversion.substr(4,n-4);
			for(int i=0;i<n-4;i++)
			{
				(*devinfo).APP[i]=aaaa[i];
			}
		    additversion.erase(0,n+4);

			n=additversion.find("SN");
			if(n!=-1)
			{
				n=additversion.find('\\');
				string aaaa=additversion.substr(3,n-3);
				for(int i=0;i<n-3;i++)
				{
					(*devinfo).sn[i]=aaaa[i];
				}
				additversion.erase(0,n+4);
		              
				n=additversion.find('\\');
				if(n!=0)
				{
					string aaaa=additversion.substr(0,n);
					for(int i=0;i<n;i++)
					{
						(*devinfo).otherinf[i]=aaaa[i];
					}
				}

			}
			else
			{
				n=additversion.find('\\');
				if(n!=0)
				{
					string aaaa=additversion.substr(0,n);
					for(int i=0;i<n;i++)
					{
						(*devinfo).otherinf[i]=aaaa[i];
					}
				}
			}


		}
		else//没有APP
		{
		   n=additversion.find("SN");
		   if(n!=-1)
		   {
			   n=additversion.find('\\');
			   //(*devinfo).Sys_MP=additversion.substr(7,n-7);
			   string aaaa=additversion.substr(3,n-3);
			   for(int i=0;i<n-3;i++)
			   {
				   (*devinfo).sn[i]=aaaa[i];
			   }
			   additversion.erase(0,n+4);

			   n=additversion.find('\\');
			   if(n!=0)
			   {
				   string aaaa=additversion.substr(0,n);
				   for(int i=0;i<n;i++)
				   {
					   (*devinfo).otherinf[i]=aaaa[i];
				   }
			   }

		   }
		   else
		   {
		       n=additversion.find('\\');
			   if(n!=0)
			   {
				   //(*devinfo).otherinf=additversion.substr(0);
				   string aaaa=additversion.substr(0,n);
				   for(int i=0;i<n;i++)
				   {
					   (*devinfo).otherinf[i]=aaaa[i];
				   }

			   }
		   }
		}


		//if(n!=-1)//有daemon
		//{
		//	n=additversion.find('\\');
		//	//(*devinfo).APP=additversion.substr(4,n-4);
		//	string aaaa=additversion.substr(7,n-7);
		//	for(int i=0;i<n-7;i++)
		//	{
		//		(*devinfo).Sys_MP[i]=aaaa[i];
		//	}
  //          additversion.erase(0,n+4);

		//	n=additversion.find("App");
		//	if(n!=-1)
		//	{
		//		n=additversion.find('\\');
		//		//(*devinfo).Sys_MP=additversion.substr(7,n-7);
		//		string aaaa=additversion.substr(4,n-4);
		//		for(int i=0;i<n-4;i++)
		//		{
		//			(*devinfo).APP[i]=aaaa[i];
		//		}
		//		additversion.erase(0,n+4);
  //              
		//		n=additversion.find('\\');
		//		if(n!=0)
		//		{
		//			string aaaa=additversion.substr(0,n);
		//			for(int i=0;i<n;i++)
		//			{
		//				(*devinfo).otherinf[i]=aaaa[i];
		//			}
		//		}

		//	}
		//	else
		//	{
		//		n=additversion.find('\\');
		//		if(n!=0)
		//		{
		//			string aaaa=additversion.substr(0,n);
		//			for(int i=0;i<n;i++)
		//			{
		//				(*devinfo).otherinf[i]=aaaa[i];
		//			}
		//		}
		//	}


		//}
		//else//没有daemon
		//{
  //         n=additversion.find("App");
		//   if(n!=-1)
		//   {
		//	   n=additversion.find('\\');
		//	   //(*devinfo).Sys_MP=additversion.substr(7,n-7);
		//	   string aaaa=additversion.substr(4,n-4);
		//	   for(int i=0;i<n-4;i++)
		//	   {
		//		   (*devinfo).APP[i]=aaaa[i];
		//	   }
		//	   additversion.erase(0,n+4);

		//	   n=additversion.find('\\');
		//	   if(n!=0)
		//	   {
		//		   string aaaa=additversion.substr(0,n);
		//		   for(int i=0;i<n;i++)
		//		   {
		//			   (*devinfo).otherinf[i]=aaaa[i];
		//		   }
		//	   }

		//   }
		//   else
		//   {
  //             n=additversion.find('\\');
		//	   if(n!=0)
		//	   {
		//		   //(*devinfo).otherinf=additversion.substr(0);
		//		   string aaaa=additversion.substr(0,n);
		//		   for(int i=0;i<n;i++)
		//		   {
		//			   (*devinfo).otherinf[i]=aaaa[i];
		//		   }

		//	   }
		//   }
		//}

     
		//additversion.erase(0,n+4);
		//n=additversion.find("Sys_MP");
		//if(n!=-1)
		//{
		//	n=additversion.find('\\');
		//	//(*devinfo).Sys_MP=additversion.substr(7,n-7);
		//	string aaaa=additversion.substr(7,n-7);
		//	for(int i=0;i<n-7;i++)
		//	{
		//		(*devinfo).Sys_MP[i]=aaaa[i];
		//	}
		//}
		//additversion.erase(0,n+4);
		//n=additversion.find("\r");
		//if(n!=0)
		//{
		//	//(*devinfo).otherinf=additversion.substr(0);
		//	string aaaa=additversion.substr(0,n);
		//	for(int i=0;i<n;i++)
		//	{
		//		(*devinfo).otherinf[i]=aaaa[i];
		//	}
		//}
	}
	
}


