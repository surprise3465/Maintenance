#pragma once
#include <string>
#include <sstream>
#include "cdef.h"
class DevUmsgParser
{
public:
	DevUmsgParser(void);
public:
	~DevUmsgParser(void);

	void OrgPacket(unsigned int macid,unsigned short headertype,string msgdata,string comstring);
	BOOL Parser(char* msg, unsigned short cnt,unsigned short headertype); 

public:
	UMSG_HEADER m_Msgheader;
	UMSG_MSGDATA m_Msgdata;
	unsigned short m_length;
	unsigned short m_data_length;
	string m_packet;
};
