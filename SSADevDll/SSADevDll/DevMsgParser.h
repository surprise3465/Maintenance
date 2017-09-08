#ifndef _DEV_MSG_PARSER_H_
#define _DEV_MSG_PARSER_H_

#include <string>
#include <sstream>
#include "cdef.h"

using namespace std;

class CDevMsgParser
{
public:
	CDevMsgParser();
	~CDevMsgParser();
	void SetProt(WORD gen_prot,WORD pro_prot,WORD proID);
    void SetProtNew(WORD gen_prot,WORD pro_prot,WORD proID,WORD pro_protsw);

	void SetDevMsg(WORD dev,DWORD msg);
	BYTE CheckSum();
	void SetDataBuf(void *src,unsigned int cnt);

	void CommHeader();
	void InterHeader();

    void InterHead();

	void OrgPacket(void *src,unsigned int cnt);
	void OrgPack(void *src,unsigned int cnt);
	const string& GetPacket() const;
	const string& GetData() const;

	void GetInterHead(unsigned char *dest);

	WORD GetPacketSize();
	DWORD GetMsgType();
	void SetDataCnt(unsigned int cnt);


	BOOL Parser(char *buf,unsigned int cnt,DWORD msg);
    int Parser2(char *buf,unsigned int cnt,DWORD msg);

	//void StoreInterhead(string cp,DEV* devinfo);

	void StoreDevInfo(DEV_INFORMATION* de,DEV* devinfo);


private:
	COMM_HEAD m_CommHeader;
	INTER_HEAD m_InterHeader;

	WORD m_ProtocolType;
	WORD m_ProjectProtocol;
    WORD m_ProjectProtocolSw;

	WORD m_ProjectNo;
	WORD m_DeviceNo;
	DWORD m_MsgType;

	string m_DataBuffer;
	unsigned int m_DataCnt;
	string m_PacketMsg;


};
#endif