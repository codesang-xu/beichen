// ServerInfoDef.h: interface for the ServerInfoDef class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERINFODEF_H__00F66660_7841_416F_A980_731A371056E3__INCLUDED_)
#define AFX_SERVERINFODEF_H__00F66660_7841_416F_A980_731A371056E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"stddef.h"
class ServerInfoDef  
{
public:
	ServerInfoDef(){};
	virtual ~ServerInfoDef(){};
	static void Load(const char* file = NULL);
public:
	static unsigned short Port_LoginServer;
	static unsigned short Port_GuideServer;
	static unsigned short Port_ACDServer;
	static unsigned short Port_RelayServer;
	static unsigned short Port_CheckNatServerA;
	static unsigned short Port_CheckNatServerB;
	
	static unsigned short Port_DataCollectServer;
	static unsigned short Port_SysManageServer;
	static unsigned short Port_TransmitServer;

	static unsigned short Port_UStateServer;
	static unsigned short Port_RecordServer;
	static unsigned short Port_SmartRouteServer;
	static unsigned short Port_SmartRouteDataServer;
	static unsigned short Port_GatewayManageServer;
	static unsigned short Port_GatewayManageDataServer;
	
	static unsigned short Port_TextChatServer;
	static unsigned short Port_RedirectServer;
	static unsigned short Port_DlyStrategyServer;
	static unsigned short Port_TollServer;
	static unsigned short Port_EchoServer;
	static unsigned short Port_TollQueryServer;
	static unsigned short Port_PhoneBookServer;
	static unsigned short Port_CallTransCtrlServer;
	static unsigned short Port_PhoneNoCtrlServer;
	static unsigned short Port_TollTransationServer;
	static unsigned short Port_RecCtrlServer;
	static unsigned short Port_SMSServer;
	static unsigned short Port_DataRouteServer;
	static unsigned short Port_ACDProxyServerA;
	static unsigned short Port_ACDProxyServerB;

	static unsigned short Port_AreaCodeServer;
    static unsigned short Port_WSCProxyServer;
    static unsigned short Port_AProxyServer;

	static char* Local_IP_LoginServer;
	static char* Local_IP_GuideServer;
	static char* Local_IP_ACDServer;
	static char* Local_IP_RelayServer;
	static char* Local_IP_CheckNatServerA;
	static char* Local_IP_CheckNatServerB;
	static char* Local_IP_DataCollectServer;
	static char* Local_IP_SysManageServer;
	static char* Local_IP_DataRouteServer;
};

#endif // !defined(AFX_SERVERINFODEF_H__00F66660_7841_416F_A980_731A371056E3__INCLUDED_)
