#ifndef _DEV_INFORMATION_H_
#define _DEV_INFORMATION_H_

#include <sstream>
#include "cdef.h"
#include "DevMsgParser.h"
#include "DevUmsgParser.h"
using namespace std;

class CDevInformation
{
public:
	CDevInformation();
	~CDevInformation();

	BOOL DevInfoInit(WORD gen_prot,WORD pro_prot,WORD proID,int type,PSINFO_ADAPTER adaterinfo=NULL);//type=0:初始化sockDTLMaint，用于读版本工具；type=1:初始化sockDTLUpdate，用于更新工具；
    BOOL DevInfoInitNew(WORD gen_prot,WORD pro_prot,WORD pro_protsw,WORD proID,int type,PSINFO_ADAPTER adaterinfo=NULL);

	void SSADevDllClose();
	void DevInfoClose();

	void Dev368Close();
    BOOL Dev368Init();

	BOOL GetDevInformation(WORD dev,string& info);
	//BOOL GetDevInformation(WORD dev);
    int GetDevInformationNew(WORD dev,int t);
	BOOL GetSWInformation(WORD dev,string& info);
    //BOOL GetSWInformation(WORD dev);
	BOOL GetSWInformationNew(WORD dev,int t);

	BOOL SetDevIP(unsigned int dev,unsigned char *info,unsigned int cnt);

	BOOL CustomCommandControl(unsigned int dev,unsigned int macid,unsigned short headertype,string& msgdata,string& comstring,string& recvstring,int timeout ,unsigned int devip);

	int GetAdapterInfoList(SINFO_ADAPTER* list);//获得本机网卡信息

	int m_Type;//0:Maint 1:Update

	DEV* m_devinfo; 
	int m_devcnt;

	void ClearSocketRecv();
    int  NewUpdateMessage(string &info);

private:
	SOCKET sockDTLMaint;//用于读版本工具
	SOCKET sockSW;
	SOCKET sockDTLUpdate;//用于更新工具
    SOCKET socksecondip;
	

	CDevMsgParser SendExtractor;
	CDevMsgParser RecvExtractor;

	DevUmsgParser SendUmsgcommand;
	DevUmsgParser RecvUmsgcommand;

};
#endif