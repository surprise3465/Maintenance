#include "StdAfx.h"
#include "DevUmsgParser.h"



DevUmsgParser::DevUmsgParser(void)
{
	memset(&m_Msgheader,0,sizeof(m_Msgheader));
	memset(&m_Msgdata,0,sizeof(m_Msgdata));
	memcpy(m_Msgheader.byMark,"UMSG",4);
	m_packet.resize(2000);
	m_packet.clear();
	m_length = 0;
	m_data_length = 0; 
}

DevUmsgParser::~DevUmsgParser(void)
{
	m_packet.resize(0);
}

void DevUmsgParser::OrgPacket(unsigned int macid,unsigned short headertype,string msgdata,string comstring)
{
	m_data_length=msgdata.size();
	m_length=m_data_length+64;
	memcpy((char*)&m_Msgheader.dwMac_id,(char*)&macid,4);
	memcpy((char*)&m_Msgheader.wLength,(char*)&m_length,2);
	memcpy((char*)&m_Msgheader.wHeadertype,(char*)&headertype,2);
	if (comstring.size())
	{
		memcpy(m_Msgheader.byComstring,comstring.c_str(),comstring.size());
	}
	memcpy(m_Msgdata.byData,msgdata.c_str(),msgdata.size());
	m_packet.clear();
	m_packet.append((char*)&m_Msgheader,64);
	m_packet.append((char*)m_Msgdata.byData,m_data_length);
}

BOOL DevUmsgParser::Parser(char* msg, unsigned short cnt,unsigned short headertype)
{
	m_length=cnt;
	m_data_length=m_length-64;
	memcpy((char*)&m_Msgheader,msg,64);
	memcpy((char*)&m_Msgdata,&msg[64],m_data_length);
	if (m_Msgheader.wHeadertype!=headertype)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}
