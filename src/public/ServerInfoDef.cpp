
#include "ServerInfoDef.h"
#include "xmlParser.h"
using namespace MyXML;
#include <boost/lexical_cast.hpp>
#include "public/intdef.h"
using boost::lexical_cast;


#ifdef WIN32
#include <Shlobj.h>
#endif

unsigned short ServerInfoDef::Port_LoginServer = 11000;
unsigned short ServerInfoDef::Port_GuideServer = 12000;
unsigned short ServerInfoDef::Port_ACDServer = 13000;
unsigned short ServerInfoDef::Port_RelayServer = 14000;
unsigned short ServerInfoDef::Port_CheckNatServerA = 15000;
unsigned short ServerInfoDef::Port_CheckNatServerB = 16000;

unsigned short ServerInfoDef::Port_DataCollectServer = 17000;
unsigned short ServerInfoDef::Port_SysManageServer = 18000;
unsigned short ServerInfoDef::Port_TransmitServer = 19000;

unsigned short ServerInfoDef::Port_UStateServer = 21000;
unsigned short ServerInfoDef::Port_RecordServer = 22000;
unsigned short ServerInfoDef::Port_SmartRouteServer = 23000;
unsigned short ServerInfoDef::Port_SmartRouteDataServer = 24000;

unsigned short ServerInfoDef::Port_GatewayManageServer = 25000;
unsigned short ServerInfoDef::Port_GatewayManageDataServer = 26000;

unsigned short ServerInfoDef::Port_TextChatServer = 27000;
unsigned short ServerInfoDef::Port_RedirectServer = 28000;
unsigned short ServerInfoDef::Port_DlyStrategyServer = 29000;
unsigned short ServerInfoDef::Port_TollServer = 31000;
unsigned short ServerInfoDef::Port_EchoServer = 32000;
unsigned short ServerInfoDef::Port_TollQueryServer = 33000;
unsigned short ServerInfoDef::Port_PhoneBookServer = 34000;
unsigned short ServerInfoDef::Port_CallTransCtrlServer = 35000;
unsigned short ServerInfoDef::Port_PhoneNoCtrlServer = 36000;
unsigned short ServerInfoDef::Port_TollTransationServer = 37000;
unsigned short ServerInfoDef::Port_RecCtrlServer = 38000;
unsigned short ServerInfoDef::Port_SMSServer = 39000;
unsigned short ServerInfoDef::Port_DataRouteServer = 40000;
unsigned short ServerInfoDef::Port_ACDProxyServerA=41000;
unsigned short ServerInfoDef::Port_ACDProxyServerB=42000;

unsigned short ServerInfoDef::Port_AreaCodeServer = 12581;

unsigned short ServerInfoDef::Port_WSCProxyServer = 45000;
unsigned short ServerInfoDef::Port_AProxyServer = 46000;

char* ServerInfoDef::Local_IP_LoginServer = "";
char* ServerInfoDef::Local_IP_GuideServer = "";
char* ServerInfoDef::Local_IP_ACDServer = "";
char* ServerInfoDef::Local_IP_RelayServer = "";
char* ServerInfoDef::Local_IP_CheckNatServerA = "";
char* ServerInfoDef::Local_IP_CheckNatServerB = "";
char* ServerInfoDef::Local_IP_DataCollectServer = "";
char* ServerInfoDef::Local_IP_SysManageServer = "";
char* ServerInfoDef::Local_IP_DataRouteServer = "";

void GetIniFile(char* pOut)
{
#ifdef WIN32
	CHAR szExePath[_MAX_PATH];
	memset(szExePath, 0, _MAX_PATH);
	SHGetSpecialFolderPath(NULL, szExePath, CSIDL_PERSONAL, FALSE);
	strcat(szExePath, "\\ServerInfoDef.xml");
	strcpy(pOut, szExePath);
#else
	strcpy(pOut, "/home/ServerInfoDef.xml");
#endif
}

void ServerInfoDef::Load(const char* file)
{
	if (file == NULL)
	{
#ifdef WIN32
		char c[_MAX_PATH];
#else  
                char c[24];
#endif
		GetIniFile(c);
		file = c;
	}
	XMLResults result;
	XMLNode nodeDoc;
	nodeDoc = XMLNode::openFileHelper(file, NULL, &result);
	if (nodeDoc.isEmpty() || result.error != eXMLErrorNone)
	{
		return;
	}

	XMLNode nodeRoot = nodeDoc.getChildNode();
	if ( nodeRoot.isEmpty())
	{
		return;
	}

	XMLCSTR chTemp = NULL;

	XMLNode nodeServerPort = nodeRoot.getChildNode("Port_LoginServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_LoginServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_GuideServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_GuideServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_ACDServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_ACDServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_RelayServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_RelayServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_CheckNatServerA");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_CheckNatServerA = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_CheckNatServerB");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_CheckNatServerB = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_DataCollectServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_DataCollectServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_SysManageServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_SysManageServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_TransmitServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_TransmitServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_UStateServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_UStateServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_RecordServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_RecordServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_SmartRouteServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_SmartRouteServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_SmartRouteDataServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_SmartRouteDataServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_GatewayManageServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_GatewayManageServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_GatewayManageDataServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_GatewayManageDataServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_TextChatServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_TextChatServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_RedirectServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_RedirectServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_DlyStrategyServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_DlyStrategyServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_TollServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_TollServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_EchoServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_EchoServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_TollQueryServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_TollQueryServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_PhoneBookServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_PhoneBookServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_CallTransCtrlServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_CallTransCtrlServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_PhoneNoCtrlServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_PhoneNoCtrlServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_TollTransationServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_TollTransationServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_RecCtrlServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_RecCtrlServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_SMSServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_SMSServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_DataRouteServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_DataRouteServer = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_ACDProxyServerA");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_ACDProxyServerA = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_ACDProxyServerB");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_ACDProxyServerB = lexical_cast<uint16_t>(chTemp);
		}
	}

	nodeServerPort = nodeRoot.getChildNode("Port_AreaCodeServer");
	if (!nodeServerPort.isEmpty())
	{
		chTemp = nodeServerPort.getAttribute("Port");
		if (chTemp != NULL)
		{
			Port_AreaCodeServer = lexical_cast<uint16_t>(chTemp);
		}
	}
}