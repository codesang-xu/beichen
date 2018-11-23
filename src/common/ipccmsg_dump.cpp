#include "common/ipccmsg_dump.h"

#include <vector>

#include "common/ipcc_message.h"
#include "common/ipccmsg_helper.h"
#include "common/util.h"
#ifdef INFOBIRD_CTI
#include "infoBirdCTIHelper.h"
#endif

namespace ib
{
namespace common
{

string to_str(const SIPCCMsg_Redirect_SearchPlatform &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_Redirect_SearchPlatform" << ": "
		<< "managerID=" << msg.managerID << ','
		<< "corpName=" << msg.corpName << ','
        << "agentID=" << msg.agentID << ','
        << "invokeID=" << msg.invokeID;
    return ss.str();
}

string to_str(const SIPCCMsg_Redirect_SearchPlatformConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Redirect_SearchPlatformConf" << ": "
        << "platformIP=" << msg.platformIP
        << ",invokeID=" << msg.invokeID;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_UserLogin &msg)
{
    char passwd[50] = { 0 };
    msg.GetThePassword(passwd);
    stringstream ss;
    ss << "SIPCCMsg_Client_UserLogin" << ": "
        << "localIP=" << msg.localIP << ','
        << "localPort=" << ntohs(msg.localPort) << ','
        << "userType=" << msg.userType << ','
        << "loginType=" << msg.loginType << ','
        << "corpInfo=" << msg.corpInfo << ','
        << "userOrigin=" << msg.userOrigin << ','
        << "versionType=" << msg.versionType << ','
        << "bMultiGroup=" << (int)msg.bMultiGroup << ','
        << "localtime=" << msg.localtime << ','
        << "userName=" << msg.GetUserName() << ','
        << "thePassword=" << passwd << ','
        << "token=" << msg.token;
    string ret = ss.str();
    return ss.str();
}

string to_str(const SIPCCMsg_Login_UserLoginSucc &ms)
{
    stringstream ss;
    ss << "SIPCCMsg_Login_UserLoginSucc" << ": "
        << "uStateServerIP=" << format_ip_addr(ms.uStateServerIP) << ','
        << "guideServerIP=" << format_ip_addr(ms.guideServerIP) << ','
        << "acdServerIP=" << format_ip_addr(ms.acdServerIP) << ','
        << "relayServerIP=" << format_ip_addr(ms.relayServerIP) << ','
        << "markServerIP=" << format_ip_addr(ms.markServerIP) << ','
        << "recModel=" <<  (int)ms.recModel<< ','
        << "natServer2IP=" << format_ip_addr(ms.natServer2IP) << ','
        << "transmitServerIP=" << format_ip_addr(ms.transmitServerIP) << ','
        << "routeServerIP=" << format_ip_addr(ms.routeServerIP) << ','
        << "gwMgrServerIP=" << format_ip_addr(ms.gwMgrServerIP) << ','
        << "textChatServerIP=" << format_ip_addr(ms.textChatServerIP) << ','
        << "dlyStrategyServerIP=" << format_ip_addr(ms.dlyStrategyServerIP) << ','
        << "callerNoForSend=" << ms.callerNoForSend << ','
        << "tollQueryServerIP=" << format_ip_addr(ms.tollQueryServerIP) << ','
        << "phoneBookServerIP=" << format_ip_addr(ms.phoneBookServerIP) << ','
        << "dataRouteServerIP=" << format_ip_addr(ms.dataRouteServerIP) << ','
        << "agentID=" << ms.agentID << ','
        << "agentPhoneNo=" << ms.agentPhoneNo << ','
        << "dataCenterServerIP=" << format_ip_addr(ms.dataCenterServerIP) << ','
        << "managerID=" << ms.managerID << ','
        << "spNumber=" << ms.spNumber << ','
        << "shortName=" << ms.shortName << ','
        << "autoFinishProcess=" << ms.autoFinishProcess << ','
        << "bIsSuper=" << (int)ms.bIsSuper << ','
        << "nickName=" << ms.nickName << ','
        << "outdialPrefixNo=" << ms.outdialPrefixNo << ','
        << "bAutoTransfer=" << (int)ms.bAutoTransfer << ','
        << "bHasSMS=" << (int)ms.bHasSMS << ','
        << "transSererIp=" << format_ip_addr(ms.transSererIp) << ','
        << "transServerPort=" << ntohs(ms.transServerPort) << ','
        << "bPermitPlayAgentID=" << (int)ms.bPermitPlayAgentID << ','
        << "bIsMasterSuper=" << (int)ms.bIsMasterSuper << ','
        << "agentName_Return=" << ms.agentName_Return << ','
        << "remindTalkTime=" << ms.remindTalkTime << ','
        << "remindProcessTime=" << ms.remindProcessTime << ','
        << "user_role=" << ms.user_role << ','
        << "agentType=" << ms.agentType << ','
        << "managerFlag=" << ms.managerFlag << ','
        << "isOutDial=" << ms.isOutDial << ','
        << "managerPassKey=" << ms.managerPassKey << ','
        << "iNeedDisposal=" << ms.iNeedDisposal << ','
        << "funcConfigure=" << ms.funcConfigure << ','
        << "extNo=" << ms.extNo << ','
        << "bClientNeedUpdate=" << (int)ms.bClientNeedUpdate << ','
        << "bOutDialOpenURL=" << (int)ms.bOutDialOpenURL << ','
        << "iSms_Type=" << ms.iSms_Type << ','
        << "mainproxyip=" << format_ip_addr(ms.mainproxyip) << ','
        << "mainproxyport=" << ntohs(ms.mainproxyport) << ','
        << "audioproxyip=" << format_ip_addr(ms.audioproxyip) << ','
        << "audioproxyport=" << ntohs(ms.audioproxyport) << ','
        << "bSelectSkillGroup=" << (int)ms.bSelectSkillGroup << ','
        << "iLoginSuccMode=" << ms.iLoginSuccMode << ','
        << "bCallInOpenURL=" << (int)ms.bCallInOpenURL << ','
        << "isPhoneNoEditBoxDisabled=" << (int)ms.isPhoneNoEditBoxDisabled << ','
        << "internationalCallUnallowed=" << (int)ms.internationalCallUnallowed << ','
        << "bHidePhone=" << (int)ms.bHidePhone << ','
        << "departmentID=" << ms.departmentID << ','
        << "samplerServerIP=" << format_ip_addr(ms.samplerServerIP) << ','
        << "iReDailUseBackupTG=" << ms.iReDailUseBackupTG << ','
        << "RegisterServerA_Ip=" << format_ip_addr(ms.RegisterServerA_Ip) << ','
        << "RegisterServerA_Port=" << ntohs(ms.RegisterServerA_Port) << ','
        << "RegisterServerB_Ip=" << format_ip_addr(ms.RegisterServerB_Ip) << ','
        << "RegisterServerB_Port=" << ntohs(ms.RegisterServerB_Port) << ','
        << "ACDServerID=" << ms.ACDServerID << ','
        << "MuiltACDServerID=" << ms.MuiltACDServerID << ','
        << "calloutByIAD=" << ms.calloutByIAD << ','
        << "SkillGroupID=" << ms.SkillGroupID << ','
        << "handleId=" << ms.handleId << ','
        << "departmentInfo=" << ((SIPCCMsg_Login_UserLoginSucc&)ms).GetDepartmentInfo() << ',';
    return ss.str();
}

string to_str(const SIPCCMsg_Login_UserLoginFailed &ms)
{
    stringstream ss;
    ss << "SIPCCMsg_Login_UserLoginFailed" << ": "
        << "failedCause=" << ms.GetFailedCause();
    return ss.str();
}

string to_str(const SIPCCMsg_Client_NotifyAgentLogin &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_NotifyAgentLogin" << ": "
        << "agentName=" << msg.agentName << ','
        << "managerID=" << msg.managerID << ','
        << "corpName=" << msg.corpName << ','
        << "departmentName=" << msg.departmentName << ',';
    return ss.str();
}

string to_str(const SIPCCMsg_Login_NotifyAgentLoginConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Login_NotifyAgentLoginConf" << ": "
        << "agentID=" << msg.agentID << ','
        << "departName=" << msg.departName << ','
        << "popScript=" << msg.popScript << ','
        << "smsServer=" << msg.smsServer << ','
        << "cIsAgentRecord=" << static_cast<int>(msg.cIsAgentRecord) << ','
        << "strAgentRecServerIP=" << msg.strAgentRecServerIP << ','
        << "strRecServerSymbol=" << msg.strRecServerSymbol << ','
        << "transserverList=" << msg.transserverList << ','
        << "threshold_packetlossrate=" << msg.threshold_packetlossrate << ','
        << "threshold_delay=" << msg.threshold_delay << ','
        << "threshold_jitter=" << msg.threshold_jitter << ','
        << "agentRecServerPort=" << msg.agentRecServerPort << ','
        << "managerTransIP=" << msg.managerTransIP << ','
        << "managerTransPort=" << msg.managerTransPort << ','
        << "CaptureNetInfoDataCollectIP=" << format_ip_addr(msg.CaptureNetInfoDataCollectIP) << ','
        << "CaptureNetInfoDataCollectPort=" << ntohs(msg.CaptureNetInfoDataCollectPort) << ','
        << "needCollectNetInfo=" << msg.needCollectNetInfo << ','
        << "CanConference=" << msg.CanConference << ','
        << "primary_username=" << msg.primary_username << ','
        << "TGRouteType=" << msg.TGRouteType << ','
        << "iHaveBlackList_Callout=" << msg.iHaveBlackList_Callout << ','
        << "canredundancy=" << msg.canredundancy << ','
        << "bProcedure_CallIn=" << (int)msg.bProcedure_CallIn << ','
        << "bProcedure_CallOut=" << (int)msg.bProcedure_CallOut << ','
        << "tcpProxyServerIP=" << format_ip_addr(msg.tcpProxyServerIP) << ','
        << "tcpProxyServerPort=" << ntohs(ntohs(msg.tcpProxyServerPort)) << ','
        << "defaultTG_DialUnderHold=" << msg.defaultTG_DialUnderHold << ','
        << "haveCallerNoQuestion=" << msg.haveCallerNoQuestion << ','
        << "HaveHBtoACD=" << msg.HaveHBtoACD << ','
        << "HaveHBtoHBServer=" << msg.HaveHBtoHBServer << ','
        << "RegisterServerAdd=" << msg.RegisterServerAdd << ','
        << "generalConfiguration=" << msg.generalConfiguration << ','
        << "xnum=" << msg.xnum << ','
        << "audioStreamDescription" << msg.audioStreamDescription.to_str();
	return ss.str();
}

string to_str(const SIPCCMsg_Client_GetParkReasons &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_Client_GetParkReasons" << ": "
		<< "managerid=" << msg.managerid << ','
		<< "invokeID=" << msg.invokeID << ',';
	return ss.str();
}

string to_str(const SIPCCMsg_Login_GetParkReasonsConf &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_Login_GetParkReasonsConf" << ": "
		<< "parkReasons=" << msg.parkReasons << ',';
	return ss.str();
}

string to_str(const SIPCCMsg_Client_QueryIncrementFunc &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_Client_QueryIncrementFunc" << ": "
		<< "agentName=" << msg.agentName << ','
		<< "departName=" << msg.departName << ','
		<< "corpName=" << msg.corpName;
	return ss.str();
}

string to_str(const SIPCCMsg_Login_QueryIncrementFuncConf &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_Login_QueryIncrementFuncConf" << ": "
		<< "funcID=" << msg.funcID << ','
		<< "funcName=" << msg.funcName << ','
		<< "defaultLangTypeID=" << msg.defaultLangTypeID << ','
		<< "defaultOperTypeID=" << msg.defaultOperTypeID << ','
		<< "ivrLangType=" << msg.ivrLangType << ','
		<< "operationType=" << msg.operationType;
	return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_GetService &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_OneKeySwitch_GetService" << ": "
		<< "InvokeId=" << msg.InvokeId << ','
		<< "ServiceID=" << msg.ServiceID << ','
		<< "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ','
		<< "GetServiceCount=" << msg.GetServiceCount << ','
		<< "Service1ID=" << msg.Service1ID << ','
		<< "Service1Type=" << ESeverType_to_str(msg.Service1Type) << ','
		<< "service1IP=" << format_ip_addr(msg.service1IP) << ','
		<< "service1Port=" << ntohs(msg.service1Port) << ','
		<< "Service2ID=" << msg.Service2ID << ','
		<< "Service2Type=" << ESeverType_to_str(msg.Service2Type) << ','
		<< "service2IP=" << format_ip_addr(msg.service2IP) << ','
		<< "service2Port=" << ntohs(msg.service2Port) << ','
		<< "Service3ID=" << msg.Service3ID << ','
		<< "Service3Type=" << ESeverType_to_str(msg.Service3Type) << ','
		<< "service3IP=" << format_ip_addr(msg.service3IP) << ','
		<< "service3Port=" << ntohs(msg.service3Port) << ','
		<< "Service4ID=" << msg.Service4ID << ','
		<< "Service4Type=" << ESeverType_to_str(msg.Service4Type) << ','
		<< "service4IP=" << format_ip_addr(msg.service4IP) << ','
		<< "service4Port=" << ntohs(msg.service4Port) << ','
		<< "serviceInfoList=" << msg.serviceInfoList;
	return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_GetServiceConf &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_OneKeySwitch_GetServiceConf" << ": "
		<< "InvokeId=" << msg.InvokeId << ','
		<< "SendServiceCount=" << msg.SendServiceCount << ','
		<< "Service1ID=" << msg.Service1ID << ','
		<< "Service1Type=" << ESeverType_to_str(msg.Service1Type) << ','
		<< "service1IP=" << format_ip_addr(msg.service1IP) << ','
		<< "service1Port=" << ntohs(msg.service1Port) << ','
		<< "Service2ID=" << msg.Service2ID << ','
		<< "Service2Type=" << ESeverType_to_str(msg.Service2Type) << ','
		<< "service2IP=" << format_ip_addr(msg.service2IP) << ','
		<< "service2Port=" << ntohs(msg.service2Port) << ','
		<< "Service3ID=" << msg.Service3ID << ','
		<< "Service3Type=" << ESeverType_to_str(msg.Service3Type) << ','
		<< "service3IP=" << format_ip_addr(msg.service3IP) << ','
		<< "service3Port=" << ntohs(msg.service3Port) << ','
		<< "Service4ID=" << msg.Service4ID << ','
		<< "Service4Type=" << ESeverType_to_str(msg.Service4Type) << ','
		<< "service4IP=" << format_ip_addr(msg.service4IP) << ','
		<< "service4Port=" << ntohs(msg.service4Port) << ','
		<< "serviceInfoList=" << msg.serviceInfoList;
	return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_GetHeartBeat &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_OneKeySwitch_GetHeartBeat" << ": "
		<< "InvokeId=" << msg.InvokeId << ','
		<< "HeartBeatServerIP=" << format_ip_addr(msg.HeartBeatServerIP) << ','
		<< "HeartBeatServerPort=" << ntohs(msg.HeartBeatServerPort) << ','
		<< "HeartBeatServerBackupIP=" << format_ip_addr(msg.HeartBeatServerBackupIP) << ','
		<< "HeartBeatServerBackupPort=" << ntohs(msg.HeartBeatServerBackupPort) << ','
		<< "ServiceID=" << msg.ServiceID << ','
		<< "ServiceType=" << ESeverType_to_str(msg.ServiceType);
	return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentLogin &msg)
{
	stringstream ss;
	ss << "SIPCCMsg_Client_AgentLogin" << ": "
		<< "agentID=" << msg.GetAgentID() << ','
		<< "theDepartment=" << msg.GetTheDepartment() << ','
		<< "SkillGroupID=" << msg.SkillGroupID << ','
		<< "loginType=" << msg.loginType << ','
		//<< "natType=" << msg.natType << ','
		<< "agentPhoneNo=" << msg.agentPhoneNo << ','
		<< "nickName=" << msg.nickName << ','
		<< "allowAudioCall=" << (msg.allowAudioCall ? 1 : 0) << ','
		<< "allowTXTCall=" << (msg.allowTXTCall ? 1 : 0) << ','
		<< "maxTXTDialogCount=" << ((int)msg.maxTXTDialogCount) << ','
		<< "loginPlatform=" << AppOSType_to_str(msg.loginPlatform) << ','
		<< "lanIP=" << format_ip_addr(msg.lanIP) << ','
		<< "lanMainSockPort=" << ntohs(msg.lanMainSockPort) << ','
		<< "iNeedDisposal=" << msg.iNeedDisposal << ','
		<< "iLoginSuccMode=" << agent_state_to_str(msg.iLoginSuccMode) << ','
		<< "connectPhoneNo=" << msg.connectPhoneNo << ','
		<< "handleId=" << msg.handleId << ','
		<< "extNo=" << msg.extNo << ','
		<< "HaveHBtoACD=" << msg.HaveHBtoACD << ','
		<< "txtSessionTimeout=" << msg.txtSessionTimeout << ','
        << "deviceToken=" << msg.deviceToken << ','
        << "resetServerState=" << (int)msg.resetServerState;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_QueryGWLanInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_QueryGWLanInfo" << ": "
        << "corpName=" << msg.corpName << ','
        << "deptName=" << msg.deptName;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentQueryState &msg)
{
    stringstream ss;

    ss << "SIPCCMsg_Client_AgentQueryState" << ": "
        << "agentState=" << agent_state_to_str(msg.agentState) << ','
        << "agentID=" << msg.GetAgentID() << ','
        << "bPSTNCall=" << (int)msg.bPSTNCall << ','
        << "bInnerCall=" << (int)msg.bInnerCall << ','
        << "parkReasonID=" << msg.parkReasonID << ','
        << "comSymbol=" << msg.comSymbol << ','
        << "newAgentPhoneNo=" << msg.newAgentPhoneNo << ','
        << "parkReason=" << msg.GetParkReason();

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AgentQueryStateConf &msg)
{
    stringstream ss;

    ss << "SIPCCMsg_ACD_AgentQueryStateConf" << ": "
        << "agentState=" << agent_state_to_str(msg.agentState) << ','
        << "bQuerySucess=" << (int)msg.bQuerySucess << ','
        << "callTransID=" << msg.callTransID << ','
        << "handleId=" << msg.handleId << ','
        << "additionalInfo=" << msg.GetAdditionalInfo();

    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_QueryGWLanInfoConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_QueryGWLanInfoConf" << ": "
        << "gwName=" << msg.gwName << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "lanPort=" << ntohs(msg.lanPort);
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AgentLoginSucc &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_AgentLoginSucc" << ": "
        << "bAgentExist=" << (int)msg.bAgentExist << ','
        << "agentId=" << msg.agentId << ','
        << "iLoginSuccMode=" << msg.iLoginSuccMode << ','
        << "handleId=" << msg.handleId << ','
        << "deptNickname=" << msg.deptNickname;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AgentLoginFailed &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_AgentLoginFailed" << ": "
        << "agentId=" << msg.agentId << ','
        << "failedCause=" << msg.failedCause << ','
        << "handleId=" << msg.handleId;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_ForceLogout &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_ForceLogout" << ": "
        << "agentName=" << msg.agentName << ','
        << "cause=" << msg.cause;

    return ss.str();
}

string to_str(const SIPCCMsg_HeartBeat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_HeartBeat" << ": "
        //<< "sipPhoneIP=" << format_ip_addr(msg.sipPhoneIP) << ','
        //<< "sipPhoneRtpPort=" << ntohs(msg.sipPhoneRtpPort) << ','
        //<< "sipPhoneRtcpPort=" << ntohs(msg.sipPhoneRtcpPort) << ','
        << "theSender=" << msg.GetTheSender() << ','
        << "clientType=" << msg.GetClientType() << ','
        << "redundancyRate=" << msg.GetRedundancyRate();

    return ss.str();
}

string to_str(const SIPCCMsg_HeartBeatResponse &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_HeartBeatResponse" << ": "
        << "ServiceCode=" << msg.ServiceCode;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_QueryGW &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_QueryGW" << ": "
        << "corporationName=" << msg.corporationName << ','
        << "departName=" << msg.departName << ','
        << "calledNo=" << msg.calledNo << ','
        << "sPrefix=" << msg.sPrefix << ','
        << "sPhoneNum=" << msg.sPhoneNum << ','
        << "sAreaCode=" << msg.sAreaCode << ','
        << "iMatchResult=" << msg.iMatchResult << ','
        << "internationalCallUnallowed=" << msg.internationalCallUnallowed << ','
        << "iUseBackupTG=" << msg.iUseBackupTG << ','
        << "TGRouteType=" << msg.TGRouteType << ','
        << "operatorsInfo=" << msg.operatorsInfo << ','
        << "agentID=" << msg.agentID << ','
        << "calloutByIAD=" << msg.calloutByIAD << ','
        << "haveCallerNoQuestion=" << msg.haveCallerNoQuestion << ','
        << "callerNoForSend=" << msg.callerNoForSend << ','
        << "gatewayName=" << msg.gatewayName;

    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_QueryGWConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_QueryGWConf" << ": "
        << "comSymbol=" << msg.comSymbol << ','
        << "gwName=" << msg.gwName << ','
        << "newPhoneNo=" << msg.newPhoneNo << ','
        << "gwPartNo=" << msg.gwPartNo << ','
        << "dtmfEN=" << msg.dtmfEN << ','
        << "agentID=" << msg.agentID << ','
        << "gwAreaCode=" << msg.gwAreaCode << ','
        << "failedCause=" << msg.failedCause << ','
        << "usedBackupTG=" << msg.usedBackupTG << ','
        << "calloutByIAD=" << msg.calloutByIAD << ','
        << "routeContent=" << msg.routeContent << ','
        << "callerNoResult=" << msg.callerNoResult << ','
        << "ManagerTel=" << msg.ManagerTel;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentMakeCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentMakeCall" << ": "
        << "calledNum=" << msg.GetCalledNum() << ','
        << "agentID=" << msg.GetAgentID() << ','
        << "callerNoForSend=" << msg.callerNoForSend << ','
        << "userLineNo=" << msg.userLineNo << ','
        << "uuiData=" << msg.GetUUIData() << ','
        << "corporationName=" << msg.GetCorporationName() << ','
        << "managerId=" << msg.managerId << ','
        << "departmentID=" << msg.departmentID << ','
        << "callID=" << msg.GetCallID() << ','
        << "comSymbol=" << msg.comSymbol << ','
        << "targetPosition=" << msg.GetTargetPosition() << ','
        << "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
        << "userLinePort=" << ntohs(msg.userLinePort) << ','
        << "mediaInfo=" << msg.audioStreamDescription.to_str() << ','
        << "targetType=" << device_type_to_str(msg.targetType) << ','
        << "agentUUIData=" << msg.AgentUUIData() << ','
        << "callType=" << to_str(msg.callType) << ','
        << "operatorsInfo=" << msg.operatorsInfo << ','
        << "outdialPrefixNo=" << msg.outdialPrefixNo << ','
        << "curVersion=" << msg.curVersion << ','
        << "wpAgentID=" << msg.wpAgentID << ','
        << "originalData=" << msg.originalData << ','
        << "managerId=" << msg.managerId << ","
        << "asd=" << msg.audioStreamDescription.to_str() << ","
        << "callType=" << to_str(msg.callType) << ","
        << "callerType=" << call_party_type_to_str(msg.callerType) << ","
        << "recentContacts=" << msg.recentContacts << ","
        << "orginalAgentID=" << msg.orginalAgentID << ","
        << "orginalCallID=" << msg.orginalCallID << ","
        << "keyword=" << msg.keyword << ","
        << "wpAgentPhoneNo=" << msg.wpAgentPhoneNo << ","
        << "trunkProviderID=" << msg.trunkProviderID << ","
        << "sipTrunkIP=" << msg.sipTrunkIP << ","
        << "sipTrunkPort=" << msg.sipTrunkPort;

    return ss.str();
}

string to_str(const SIPCCMsg_UState_AgentMakeCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_AgentMakeCallConf" << ": "
        << "callingState=" << agent_calling_state_to_str(msg.callingState);

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AgentMakeCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_AgentMakeCallConf" << ": "
        << "callingState=" << agent_calling_state_to_str(msg.callingState) << ','
        << "userNumInQueue=" << msg.userNumInQueue << ','
        << "audioIP=" << format_ip_addr(msg.audioIP) << ','
        << "audioPort=" << ntohs(msg.audioPort) << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "lanMainSockPort=" << ntohs(msg.lanMainSockPort) << ','
        << "agentExtNo=" << msg.agentExtNo << ','
        << "calledParty=" << msg.GetCalledParty() << ','
        << "callID=" << msg.callID << ','
        << "callingAgentID=" << msg.callingAgentID << ','
        << "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CallUserConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallUserConf" << ": "
        << "failedCause=" << msg.failedCause << ','
        << "bCallFailed=" << (int)msg.bCallFailed << ','
        << "audioIP=" << format_ip_addr(msg.audioIP) << ','
        << "audioPort=" << ntohs(msg.audioPort) << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "invokeID=" << msg.invokeID << ','
        //<< "sipBoardIP=" << format_ip_addr(msg.sipBoardIP) << ','
        //<< "sipBoardRtpPort=" << ntohs(msg.sipBoardRtpPort) << ','
        //<< "sipBoardRtcpPort=" << ntohs(msg.sipBoardRtcpPort) << ','
        //<< "sipTransIP=" << format_ip_addr(msg.sipTransIP) << ','
        //<< "sipTransRtpPort=" << ntohs(msg.sipTransRtpPort) << ','
        //<< "sipTransRtcpPort=" << ntohs(msg.sipTransRtcpPort) << ','
        //<< "conferencePort1="    << msg.conferencePort1  << ','
        //<< "conferencePort2="    << msg.conferencePort2  << ','
        << "userName=" << msg.GetUserName() << ','
        << "trans=" << msg.trans << ','
        << "codec=" << msg.codec << ','
        << "unit=" << msg.unit;

    return ss.str();
}

string to_str(const SIPCCMsg_DataTrans_StartTrans &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_DataTrans_StartTrans" << ": "
        << "agentId=" << msg.agentId << ','
        << "gwLineIp=" << format_ip_addr(msg.gwLineIp) << ','
        << "gwLinePort=" << ntohs(msg.gwLinePort);

    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentMakeCall2 &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentMakeCall2" << ": "
        << "uuiData=" << msg.GetUUIData() << ','
        << "agentID=" << msg.GetAgentID() << ','
        << "callerNoForSend=" << msg.callerNoForSend << ','
        << "targetPosition=" << msg.GetTargetPosition() << ','
        << "invokeID=" << msg.invokeID << ','
        << "usableTime=" << msg.usableTime << ','
        << "callTransID=" << msg.callTransID << ','
        << "corporationName=" << msg.GetCorporationName() << ','
        << "managerId=" << msg.managerId << ','
        << "departmentID=" << msg.departmentID << ','
        << "corpName=" << msg.corpName << ','
        << "wpAgentID=" << msg.wpAgentID << ','
        << "extPhoneNo=" << msg.extPhoneNo << ','
        << "nickName=" << msg.nickName << ','
        << "RecordMode=" << msg.RecordMode << ','
        << "callID=" << msg.GetCallID() << ','
        << "mediaInfo=" << msg.audioStreamDescription.to_str() << ','
        << "token=" << msg.token << ','
        << "wpAgentPhoneNo=" << msg.wpAgentPhoneNo << ','
        << "trunkProviderID=" << msg.trunkProviderID << ','
        << "sipTrunkIP=" << msg.sipTrunkIP << ','
        << "sipTrunkPort=" << msg.sipTrunkPort << ','
        << "calledOperType=" << msg.calledOperType << ','
        << "lanIp=" << format_ip_addr(msg.lanIp) << ','
        << "curVersion=" << msg.curVersion << ','
        << "targetType=" << msg.targetType << ','
        << "xnum=" << msg.xnum;

    return ss.str();
}

string to_str(const SIPCCMsg_DataTrans_StartTransConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_DataTrans_StartTransConf" << ": "
        << "channelIndex=" << msg.channelIndex << ','
        << "port=" << ntohs(msg.port);

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CallEstablished &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_ACD_CallEstablished" << ": "
		//<< "relayIP="              << format_ip_addr(msg.relayIP)<< ','
		//<< "relayPort="            << ntohs(msg.relayPort)   << ','
		//<< "remoteNatType="        << msg.remoteNatType      << ','
		<< "outDialType=" << msg.outDialType << ','
		<< "ipPhoneNo_User=" << msg.ipPhoneNo_User << ','
		<< "ipPhoneNo_AgentExt=" << msg.ipPhoneNo_AgentExt << ','
		<< "userLineNo=" << msg.userLineNo << ','
		<< "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
		<< "userLinePort=" << ntohs(msg.userLinePort) << ','
		<< "handleId=" << msg.handleId << ','
		<< "nickName=" << msg.nickName << ','
		<< "theOtherParty=" << msg.GetTheOtherParty() << ','
		<< "callID=" << msg.callID << ','
		<< "callingAgentID=" << msg.callingAgentID << ','
		<< "mediaInfo=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType) << ','
		<< "managerId=" << msg.managerId;

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_RecInfo2Client &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_RecInfo2Client" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "fullPathFileName=" << msg.fullPathFileName;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CallEstablished_Real &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallEstablished_Real" << ": "
        << "theotherPartyType=" << msg.theotherPartyType << ','
        << "outDialType=" << msg.outDialType << ','
        << "userLineNo=" << msg.userLineNo << ','
        << "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
        << "userLinePort=" << ntohs(msg.userLinePort) << ','
        << "userName=" << msg.GetUserName() << ','
        << "theOtherParty=" << msg.GetTheOTherParty();
    return ss.str();
}

string to_str(const SIPCCMsg_AreaQuery &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_AreaQuery" << ": "
        << "strPhoneNum=" << msg.strPhoneNum << ','
        << "strInvokeId=" << msg.strInvokeId << ','
        << "iOperationType=" << msg.iOperationType << ','
        << "managerid=" << msg.managerid << ','
        << "deptName=" << msg.deptName << ','
        << "agentName=" << msg.agentName;

    return ss.str();
}

string to_str(const SIPCCMsg_AreaAnswer &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_AreaAnswer" << ": "
        << "strPhoneInfo=" << msg.strPhoneInfo << ','
        << "strInvokeId=" << msg.strInvokeId << ','
        << "iDialAuthorize=" << msg.iDialAuthorize;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentHangupCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentHangupCall" << ": "
        << "agentJoinType=" << msg.agentJoinType << ','
        << "bAgentAssigned=" << (int)msg.bAgentAssigned << ','
        //<< "toAcdOrUserState=" << msg.toAcdOrUserState << ','
        << "agentID=" << msg.GetAgentID() << ','
        << "theOtherParty=" << msg.GetTheOtherParty() << ','
        << "callID=" << msg.GetCallID() << ','
        << "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_UState_CallReleased &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallReleased" << ": "
        << "theOtherParty=" << msg.GetTheOtherParty();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CallReleased &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallReleased" << ": "
        << "bIsPresideAgent=" << (int)msg.bIsPresideAgent << ','
        << "callID=" << msg.callID << ','
        << "agentName=" << msg.agentName << ','
        << "releasedParty=" << msg.GetReleasedParty() << ','
        << "toAgentID=" << msg.toAgentID << ','
        << "callType=" << to_str(msg.callType);
    return ss.str();
}

string to_str(const SIPCCMsg_DataTrans_StopTrans &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_DataTrans_StopTrans" << ": "
        << "channelIndex=" << msg.channelIndex;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentHoldCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentHoldCall" << ": "
        << "ManagerID=" << msg.ManagerID << ','
        //<< "toAcdOrUserState=" << msg.toAcdOrUserState << ','
        << "heldParty=" << msg.GetHeldParty();
    return ss.str();
}

string to_str(const SIPCCMsg_UState_AgentHoldCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_AgentHoldCallConf" << ": "
        << "bHoldSucc=" << (int)msg.bHoldSucc;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentUnHoldCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentUnHoldCall" << ": "
        << "heldParty=" << msg.GetHeldParty();
    return ss.str();
}

string to_str(const SIPCCMsg_Client_BeforeTransfer &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_BeforeTransfer" << ": "
        << "otherPartyType=" << msg.otherPartyType << ','
        << "theParty=" << msg.GetTheParty() << ','
        << "presideAgent=" << msg.GetPresideAgent() << ','
        << "otherParty=" << msg.GetOtherParty();
    return ss.str();
}

string to_str(const SIPCCMsg_Client_TransferCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_TransferCall" << ": "
        << "otherPartyType=" << call_party_type_to_str(msg.otherPartyType) << ','
        << "userLineNo=" << msg.userLineNo << ','
        << "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
        << "userLinePort=" << ntohs(msg.userLinePort) << ','
        << "thePartyType=" << call_party_type_to_str(msg.thePartyType) << ','
        << "agentUUIData=" << msg.agentUUIData << ','
        << "needReturn=" << (int)msg.needReturn << ','
        << "IsWebPSTNAgent=" << (int)msg.IsWebPSTNAgent << ','
        << "theParty=" << msg.GetTheParty() << ','
        << "presideAgent=" << msg.GetPresideAgent() << ','
        << "otherParty=" << msg.GetOtherParty() << ','
        << "trans=" << msg.trans << ','
        << "codec=" << msg.codec << ','
        << "unit=" << msg.unit << ','
        << "audioStreamDescription=" << msg.audioStreamDescription.to_str();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CallArriveToAgent &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_ACD_CallArriveToAgent" << ": "
		<< "lanIP=" << format_ip_addr(msg.lanIP) << ','
		<< "lanMainSockPort=" << ntohs(msg.lanMainSockPort) << ','
		//<< "sipAgentPhoneIP1=" << format_ip_addr(msg.sipAgentPhoneIP1) << ','
		//<< "sipAgentPhoneRtpPort1=" << ntohs(msg.sipAgentPhoneRtpPort1) << ','
		//<< "sipAgentPhoneRtcpPort1=" << ntohs(msg.sipAgentPhoneRtcpPort1) << ','
		<< "agentExtNo=" << msg.agentExtNo << ','
		<< "agentID=" << msg.GetAgentID() << ','
		<< "callID=" << msg.callID << ','
		<< "agentNickName=" << msg.agentNickName << ','
		<< "callingAgentID=" << msg.callingAgentID << ','
		<< "mediaInfo=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType);
    return ss.str();
}

string to_str(const SIPCCMsg_Record_AgentRecordQueryPort &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Record_AgentRecordQueryPort" << ": "
        << "strInvokeID=" << msg.strInvokeID << ','
        << "nSeq=" << msg.nSeq << ','
        << "strPresideAgentAccount=" << msg.strPresideAgentAccount;
    return ss.str();
}

string to_str(const SIPCCMsg_Record_AgentRecordQueryPortConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Record_AgentRecordQueryPortConf" << ": "
        << "strInvokeID=" << msg.strInvokeID << ','
        << "nSeq=" << msg.nSeq << ','
        << "strPresideAgentAccount=" << msg.strPresideAgentAccount << ','
        << "nPort=" << msg.nPort;
    return ss.str();
}


string to_str(const SIPCCMsg_Record_AgentRecordStart &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Record_AgentRecordStart" << ": "
        << "strInvokeID=" << msg.strInvokeID << ','
        << "nSeq=" << msg.nSeq << ','
        << "strPresideAgentAccount=" << msg.strPresideAgentAccount << ','
        << "strOpTalkingAgentAccount=" << msg.strOpTalkingAgentAccount << ','
        << "strStartTime=" << msg.strStartTime << ','
        << "strRecServerSymbol=" << msg.strRecServerSymbol << ','
        << "strPresideAgentNickname=" << msg.strPresideAgentNickname << ','
        << "strOpTalkingAgentNickname=" << msg.strOpTalkingAgentNickname << ','
        << "strManagerid=" << msg.strManagerid << ','
        << "PresidePhoneNo=" << msg.PresidePhoneNo << ','
        << "RecordMode=" << msg.RecordMode;
    return ss.str();
}

string to_str(const SIPCCMsg_Record_AgentRecordStop &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Record_AgentRecordStop" << ": "
        << "strInvokeID=" << msg.strInvokeID << ','
        << "nSeq=" << msg.nSeq << ','
        << "stopTime=" << msg.stopTime << ','
        << "strPresideAgentAccount=" << msg.strPresideAgentAccount;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_MediaRequestConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_MediaRequestConf" << ": "
        << "mediaType=" << msg.mediaType << ','
        << "callID=" << msg.GetCallID();
    return ss.str();
}

string to_str(const SIPCCMsg_Trans_TransmitRequest &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Trans_TransmitRequest" << ": "
        //<< "mediaHquest="              << msg.GetMediaHeadData()     << ','
        << "callID=" << msg.GetCallID() << ','
        << "nSeq=" << msg.nSeq << ','
        << "cIsAgentRecord=" << (int)msg.cIsAgentRecord << ','
        << "ulRecServerIP=" << format_ip_addr(msg.ulRecServerIP) << ','
        << "nRecServerPort=" << ntohs(msg.nRecServerPort) << ','
        << "strPresideAgentAccount=" << msg.strPresideAgentAccount << ','
        << "strCodec=" << msg.strCodec << ','
        << "bOldFormat=" << (int)msg.bOldFormat << ','
        << "PackedSize=" << msg.m_nPackedSize;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentTestNet &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentTestNet" << ": "
        << "OppName=" << msg.GetOppName();
    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentTestNetConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentTestNetConf" << ": "
        << "OppName=" << msg.GetOppName();
    return ss.str();
}

string to_str(const SIPCCMsg_Client_LanPortApprize &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_LanPortApprize" << ": "
        << "lanTextPort=" << ntohs(msg.lanTextPort) << ','
        << "lanAudioPort=" << ntohs(msg.lanAudioPort);
    return ss.str();
}

string to_str(const SIPCCMsg_PhoneSearch_ExtNoInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_PhoneSearch_ExtNoInfo" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "managerID=" << msg.managerID << ','
        << "extNo=" << msg.extNo << ','
        << "corpName=" << msg.corpName << ','
        << "manageConnectNo=" << msg.manageConnectNo;
    return ss.str();
}

string to_str(const SIPCCMsg_PhoneSearch_ExtNoInfoConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_PhoneSearch_ExtNoInfoConf" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "managerID=" << msg.managerID << ','
        << "realNoList=" << msg.realNoList << ','
        << "agentId=" << msg.agentId;
    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_IsAgentLogin &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_IsAgentLogin" << ": "
        << "agentID=" << msg.agentID << ','
        << "invokeType=" << msg.invokeType << ','
        << "invokeId=" << msg.invokeId << ','
        << "connectPhoneNo=" << msg.connectPhoneNo << ','
        << "handleId=" << msg.handleId;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_IsAgentLoginConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_IsAgentLoginConf" << ": "
        << "agentID=" << msg.agentID << ','
        << "bIsAgentLogin=" << (int)msg.bIsAgentLogin << ','
        << "agentState=" << agent_state_to_str(msg.agentState) << ','
        << "loginType=" << login_type_to_str(msg.loginType) << ','
        << "invokeType=" << msg.invokeType << ','
        << "invokeId=" << msg.invokeId << ','
        << "handleId=" << msg.handleId;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_GetDepartNAgentInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_GetDepartNAgentInfo" << ": "
        << "getMode=" << msg.getMode << ','
        << "invokeID=" << msg.invokeID << ','
        << "corporationName=" << msg.GetCorporationName() << ','
        << "theDepartment=" << msg.GetTheDepartment();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_DepartNAgentInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_DepartNAgentInfo" << ": "
        << "bEnd=" << (int)msg.bEnd << ','
        << "invokeID=" << msg.invokeID << ','
        << "theInfo=" << msg.GetTheInfo()
        << "deptNickname=" << msg.deptNickname;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_ConferenceStart &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_Client_ConferenceStart" << ": "
		<< "firstName=" << msg.firstName << ','
		<< "firstLineNo=" << msg.firstLineNo << ','
		<< "thirdName=" << msg.thirdName << ','
		<< "conferenceType=" << msg.conferenceType << ','
		<< "managerid=" << msg.managerid << ','
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_ConferenceStartConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_ConferenceStartConf" << ": "
        << "firstName=" << msg.firstName << ','
        << "firstLineNo=" << msg.firstLineNo << ','
        << "thirdName=" << msg.thirdName << ','
        << "conferenceType=" << msg.conferenceType << ','
        << "managerid=" << msg.managerid << ','
        << "bSucc=" << (int)msg.bSucc << ','
        << "ConferencePort=" << msg.ConferencePort << ','
        << "thePartyLineNo=" << msg.thePartyLineNo << ','
        << "theOtherPartyLineNo=" << msg.theOtherPartyLineNo;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_ConferenceEnd &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_ConferenceEnd" << ": "
        << "firstName=" << msg.firstName << ','
        << "firstLineNo=" << msg.firstLineNo << ','
        << "thirdName=" << msg.thirdName << ','
        << "conferenceType=" << msg.conferenceType << ','
        << "managerid=" << msg.managerid;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_ConferenceStart &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_ACD_ConferenceStart" << ": "
		<< "firstName=" << msg.firstName << ','
		<< "firstLineNo=" << msg.firstLineNo << ','
		<< "ConferencePort=" << msg.ConferencePort << ','
		<< "thePartyLineNo=" << msg.thePartyLineNo << ','
		<< "theOtherPartyLineNo=" << msg.theOtherPartyLineNo << ','
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_ConferenceEnd &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_ConferenceEnd" << ": "
        << "firstName=" << msg.firstName << ','
        << "firstLineNo=" << msg.firstLineNo;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_UserLeaveConference &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_UserLeaveConference" << ": "
        << "leaveUserName=" << msg.leaveUserName << ','
        << "initiateAgentName=" << msg.initiateAgentName << ','
        << "joinName=" << msg.joinName << ','
        << "conferenceType=" << msg.conferenceType << ','
        << "managerid=" << msg.managerid;
    return ss.str();
}
string to_str(const SIPCCMsg_ACD_CallDelivered &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_ACD_CallDelivered" << ": "
		<< "callerName=" << msg.GetCallerName() << ','
		<< "callerType=" << call_party_type_to_str(msg.callerType) << ','
		<< "keyWord=" << msg.GetKeyWord() << ','
		<< "uuiData=" << msg.GetUUIData() << ','
		<< "agentUUIData=" << msg.AgentUUIData() << ','
		<< "audioIP=" << format_ip_addr(msg.audioIP) << ','
		<< "audioPort=" << ntohs(msg.audioPort) << ','
		<< "lanIP=" << format_ip_addr(msg.lanIP) << ','
		<< "lanMainSockPort=" << ntohs(msg.lanMainSockPort) << ','
		<< "gwPartNo=" << msg.gwPartNo << ','
		<< "wPstnAgentLineNo=" << msg.wPstnAgentLineNo << ','
		<< "userLineNo=" << msg.userLineNo << ','
		<< "userLineIP=" << format_ip_addr(msg.userLineIP) << ','
		<< "userLinePort=" << ntohs(msg.userLinePort) << ','
		<< "comSymbol=" << msg.comSymbol << ','
		<< "uuiData2=" << msg.uuiData2 << ','
		<< "bIsFromDept=" << (int)msg.bIsFromDept << ','
		<< "originalData=" << msg.originalData << ','
		//<< "sipBoardIP=" << format_ip_addr(msg.sipBoardIP) << ','
		//<< "sipBoardRtpPort=" << ntohs(msg.sipBoardRtpPort) << ','
		//<< "sipBoardRtcpPort=" << ntohs(msg.sipBoardRtcpPort) << ','
		//<< "sipTransIP=" << format_ip_addr(msg.sipTransIP) << ','
		//<< "sipTransRtpPort=" << ntohs(msg.sipTransRtpPort) << ','
		//<< "sipTransRtcpPort=" << ntohs(msg.sipTransRtcpPort) << ','
		//<< "sipAgentPhoneIP1=" << format_ip_addr(msg.sipAgentPhoneIP1) << ','
		//<< "sipAgentPhoneRtpPort1=" << ntohs(msg.sipAgentPhoneRtpPort1) << ','
		//<< "sipAgentPhoneRtcpPort1=" << ntohs(msg.sipAgentPhoneRtcpPort1) << ','
		<< "conferencePort1=" << ntohs(msg.conferencePort1) << ','
		<< "conferencePort2=" << ntohs(msg.conferencePort2) << ','
		<< "SkillGroupID=" << msg.SkillGroupID << ','
		<< "SkillGroupName=" << msg.SkillGroupName << ','
		<< "callID=" << msg.GetCallID() << ','
		//<< "trans=" << msg.trans << ','
		//<< "codec=" << msg.codec << ','
		//<< "unit=" << msg.unit << ','
		<< "mediaInfo=" << msg.audioStreamDescription.to_str() << ','
		<< "audioRtpPort=" << ntohs(msg.audioRtpPort) << ','
		<< "callType=" << to_str(msg.callType) << ','
		<< "mediaRequest=" << msg.mediaRequest.to_str();

    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentCallReceived &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentCallReceived" << ": "
        << "lanIp=" << format_ip_addr(msg.lanIp) << ','
        << "agentExtNo=" << msg.agentExtNo << ','
        //<< "sipAgentPhoneIP1=" << format_ip_addr(msg.sipAgentPhoneIP1) << ','
        //<< "sipAgentPhoneRtpPort1=" << ntohs(msg.sipAgentPhoneRtpPort1) << ','
        //<< "sipAgentPhoneRtcpPort1=" << ntohs(msg.sipAgentPhoneRtcpPort1) << ','
        //<< "toAcdOrUserState=" << msg.toAcdOrUserState << ','
        << "nickName=" << msg.nickName << ','
        << "agentID=" << msg.GetAgentID() << ','
        << "theOtherParty=" << msg.GetTheOtherParty() << ','
		<< "mediaInfo=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentAnswerCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentAnswerCall" << ": "
        << "agentID=" << msg.GetAgentID() << ','
        << "theOtherParty=" << msg.GetTheOTherParty() << ','
        << "theotherPartyType=" << msg.theotherPartyType << ','
        << "callId=" << msg.callId << ','
        << "managerId=" << msg.managerId << ','
        << "corpName=" << msg.corpName << ','
        << "extPhoneNo=" << msg.extPhoneNo << ','
        << "recordFilePath=" << msg.recordFilePath << ','
        << "RecordMode=" << msg.RecordMode << ','
        << "token=" << msg.token << ','
        << "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_UState_CallEstablished &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallEstablished" << ": "
        << "theOtherParty=" << msg.GetTheOtherParty();

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_MediaRequest &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_MediaRequest" << ": "
        << "mediaType=" << msg.mediaType << ','
        << "seq=" << msg.seq << ','
        << "callID=" << msg.GetCallID() << ','
        << "account=" << msg.account << ','
        << "transmitServerIP=" << format_ip_addr(msg.transmitServerIP) << ','
        << "transmitServerPort=" << ntohs(msg.transmitServerPort) << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_GetTalkingAgent &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_GetTalkingAgent" << ": "
        << "Group=" << msg.Group << ','
        << "invokeID=" << msg.invokeID;

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_GetTalkingAgentConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_GetTalkingAgentConf" << ": "
        << "theList=" << msg.theList << ','
        << "bEnd=" << msg.bEnd << ','
        << "invokeID=" << msg.invokeID;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_MonitRequest &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_MonitRequest" << ": "
        << "ip=" << format_ip_addr(msg.ip) << ','
        << "port=" << ntohs(msg.port) << ','
        << "agentName=" << msg.agentName << ','
        << "agentName_Src=" << msg.agentName_Src << ','
        << "agentName_Dest=" << msg.agentName_Dest;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_MonitRequestConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_MonitRequestConf" << ": "
        << "bAllow=" << (int)msg.bAllow << ','
        << "agentName_Src=" << msg.agentName_Src << ','
        << "agentName_Dest=" << msg.agentName_Dest << ','
        << "bSucc=" << (int)msg.bSucc << ','
        << "CallId=" << msg.CallId;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_StopMonit &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_StopMonit" << ": "
        << "agentName=" << msg.agentName << ','
        << "agentName_Src=" << msg.agentName_Src << ','
        << "agentName_Dest=" << msg.agentName_Dest << ','
        << "trunkDn=" << msg.trunkDn;

    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_UserLeaveConference &msg)
{
    ostringstream ss;
    ss << "SIPCCMsg_Gateway_UserLeaveConference" << ": "
        << "leaveUserName=" << msg.leaveUserName << ','
        << "initiateAgentName=" << msg.initiateAgentName << ','
        << "joinName=" << msg.joinName << ','
        << "conferenceType=" << msg.conferenceType << ','
        << "managerid=" << msg.managerid;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_WebPSTNDial &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_WebPSTNDial" << ": "
        << "agentID=" << msg.agentID << ','
        << "corpName=" << msg.corpName << ','
        << "agentPhoneNo=" << msg.agentPhoneNo << ','
        << "userPhoneNo=" << msg.userPhoneNo << ','
        << "deptName=" << msg.deptName << ','
        << "managerDialNo=" << msg.managerDialNo << ','
        << "CallTransID=" << msg.CallTransID << ','
        << "managerId=" << msg.managerId << ','
        << "computerSymbol=" << msg.computerSymbol << ','
        << "NeedReturnClient=" << int(msg.NeedReturnClient) << ','
        << "handleId=" << msg.handleId << ','
        << "userData=" << msg.userData;

    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_WebPSTNDialConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_WebPSTNDialConf" << ": "
        << "agentID=" << msg.agentID << ','
        << "comSymbol=" << msg.comSymbol << ','
        << "gwName=" << msg.gwName << ','
        << "newUserPhoneNo=" << msg.newUserPhoneNo << ','
        << "newAgentPhoneNo=" << msg.newAgentPhoneNo << ','
        << "gwPartNo=" << msg.gwPartNo << ','
        << "dtmfEN=" << msg.dtmfEN << ','
        << "handleId=" << msg.handleId;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_TextChat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_TextChat" << ": "
        << "Sender=" << msg.Sender << ','
        << "Receiver=" << msg.Receiver << ','
        << "ChatWords=" << msg.ChatWords;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_LoginToText &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_LoginToText" << ": "
        << "UserName=" << msg.UserName;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_PrepareTransfer &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_PrepareTransfer" << ": "
        << "otherPartyType=" << msg.otherPartyType << ','
        << "AudioIp=" << format_ip_addr(msg.AudioIp) << ','
        << "AudioPort=" << ntohs(msg.AudioPort) << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "callID=" << msg.GetCallID() << ','
        << "theParty=" << msg.GetTheParty() << ','
        << "otherParty=" << msg.GetOtherParty();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_PrepareTransfer &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_PrepareTransfer" << ": "
        << "otherPartyType=" << msg.otherPartyType << ','
        << "AudioIp=" << format_ip_addr(msg.AudioIp) << ','
        << "AudioPort=" << ntohs(msg.AudioPort) << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "callID=" << msg.GetCallID() << ','
        << "otherParty=" << msg.GetOtherParty();
    return ss.str();
}

string to_str(const SIPCCMsg_Client_TransReady &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_TransReady" << ": "
        << "presideAgent=" << msg.GetPresideAgent() << ','
        << "readyParty=" << msg.GetReadyParty();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_TransferCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_TransferCall" << ": "
        << "otherPartyType=" << msg.otherPartyType << ','
        << "userLineNo=" << msg.userLineNo << ','
        << "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
        << "userLinePort=" << ntohs(msg.userLinePort) << ','
        << "theParty=" << msg.GetTheParty() << ','
        << "presideAgent=" << msg.GetPresideAgent() << ','
        << "otherParty=" << msg.GetOtherParty() << ','
        << "trans=" << msg.trans << ','
        << "codec=" << msg.codec << ','
        << "unit=" << msg.unit;

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_SetClientAgentState &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_SetClientAgentState" << ": "
        << "agentState=" << agent_state_to_str(msg.agentState) << ','
        << "gwAgentLine=" << msg.gwAgentLine << ','
        << "gwUserLine=" << msg.gwUserLine << ','
        << "transedAgentID=" << msg.transedAgentID << ','
        << "transedGwAgentLine=" << msg.transedGwAgentLine << ','
        << "gwAgentLineIp=" << format_ip_addr(msg.gwAgentLineIp) << ','
        << "gwAgentLinePort=" << ntohs(msg.gwAgentLinePort) << ','
        << "gwUserLineIp=" << format_ip_addr(msg.gwUserLineIp) << ','
        << "gwUserLinePort=" << ntohs(msg.gwUserLinePort) << ','
        << "operType=" << msg.operType;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_GetCallingQueueInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_GetCallingQueueInfo" << ": "
        << "agentState=" << msg.invokeID << ','
        << "theDepartment=" << msg.GetTheDepartment() << ','
        << "userName=" << msg.GetUserName();

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CallingQueueInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallingQueueInfo" << ": "
        << "userNumInQueue=" << msg.userNumInQueue << ','
        << "agentNumInTheDepartment=" << msg.agentNumInTheDepartment << ','
        << "invokeID=" << msg.invokeID << ','
        << "userNumInQueues=" << msg.userNumInQueues << ','
        << "agentNumInSkillGroups=" << msg.agentNumInSkillGroups;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_AgentLogout &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentLogout" << ": "
        << "loginType=" << msg.loginType << ','
        << "connectPhoneNo=" << msg.connectPhoneNo << ','
        << "handleId=" << msg.handleId << ','
        << "agentID=" << msg.GetAgentID();

    return ss.str();
}

string to_str(const SIPCCMsg_Client_UserLogoutStateSvr &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserLogoutStateSvr" << ": "
        << "userName=" << msg.GetUserName();

    return ss.str();
}

string to_str(const SIPCCMsg_CClient_CallArrived &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_CClient_CallArrived" << ": "
		<< "callID=" << msg.callID << ','
		<< "callerName=" << msg.callerName << ','
		<< "calledName=" << msg.calledName << ','
		<< "calledNickname=" << msg.calledNickname << ','
		<< "netPath=" << msg.netPath << ','
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_CClient_CallAnswered &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CClient_CallAnswered" << ": "
        << "callID=" << msg.callID << ','
        << "callerName=" << msg.callerName << ','
        << "calledName=" << msg.calledName << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_HeartBeatConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_HeartBeatConf" << ": "
        << "account=" << msg.account;

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CMakeCall &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_CProxy_CMakeCall" << ": "
		<< "callID=" << msg.callID << ','
		<< "account=" << msg.account << ','
		<< "targetName=" << msg.targetName << ','
		<< "targetType=" << device_type_to_str(msg.targetType) << ','
		<< "callerNoForSend=" << msg.callerNoForSend << ','
		<< "uuiDate=" << msg.uuiDate << ','
		<< "netPath=" << msg.netPath << ','
		<< "targetPosition=" << msg.targetPosition << ','
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType) << ','
		<< "keyword=" << msg.keyword << ','
		<< "mediaRequest=" << msg.mediaRequest.to_str();
			
    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CallDelivered &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_CProxy_CallDelivered" << ": "
		<< "callID=" << msg.callID << ','
		<< "callerName=" << msg.callerName << ','
		<< "keyword=" << msg.keyword << ','
		<< "uuiDate=" << msg.uuiDate << ','
		<< "netPath=" << msg.netPath << ','
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType);
    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CallArrived &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_CallArrived" << ": "
        << "callID=" << msg.callID << ','
        << "callerName=" << msg.callerName << ','
        << "calledName=" << msg.calledName << ','
        << "calledNickname=" << msg.calledNickname << ','
        << "netPath=" << msg.netPath << ','
        << "asd=" << msg.audioStreamDescription.to_str() << ','
        << "audioIP=" << format_ip_addr(msg.audioIP) << ','
		<< "audioPort=" << ntohs(msg.audioPort) << ','
		<< "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CallAnswered &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_CallAnswered" << ": "
        << "callID=" << msg.callID << ','
        << "callerName=" << msg.callerName << ','
        << "answeredPartyName=" << msg.answeredPartyName << ','
        << "answeredPartyType=" << device_type_to_str(msg.answeredPartyType) << ','
        << "answeredPartyManagerID=" << msg.answeredPartyManagerID;

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CallStart &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_CallStart" << ": "
        << "callID=" << msg.callID << ','
        << "callerName=" << msg.callerName << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_QueryGW &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_QueryGW" << ": "
        << "callerName=" << msg.callerName << ','
        << "callerNoForSend=" << msg.callerNoForSend << ','
        << "sPrefix=" << msg.sPrefix << ','
        << "sPhoneNum=" << msg.sPhoneNum << ','
        << "sAreaCode=" << msg.sAreaCode << ','
        << "iMatchResult=" << msg.iMatchResult;

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CMakeCall2 &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_CMakeCall2" << ": "
        << "callID=" << msg.callID << ','
        << "account=" << msg.account << ','
        << "targetName=" << msg.targetName << ','
        << "targetType=" << device_type_to_str(msg.targetType) << ','
        << "uuiDate=" << msg.uuiDate << ','
        << "nickname=" << msg.nickname << ','
        << "callerNoForSend=" << msg.callerNoForSend << ','
        << "invokeID=" << msg.invokeID << ','
        << "netPath=" << msg.netPath << ','
        << "targetPosition=" << msg.targetPosition << ','
        << "asd=" << (const_cast<SIPCCMsg_CProxy_CMakeCall2&>(msg)).audioStreamDescription.to_str();

    return ss.str();
}

string to_str(const SIPCCMsg_Redirect_GetService &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Redirect_GetService" << ": "
        << "serviceType=" << ESeverType_to_str(msg.serviceType) << ','
        << "domainName=" << msg.domainName << ','
        << "queryServiceList=" << msg.queryServiceList << ','
        << "identifyingCode=" << msg.identifyingCode << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_Redirect_GetServiceConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Redirect_GetServiceConf" << ": "
        << "serviceList=" << msg.serviceList << ','
        << "redisList=" << msg.redisList << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_CClient_Login &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CClient_Login" << ": "
        << "account=" << msg.account;

    return ss.str();
}

string to_str(const SIPCCMsg_Login_CCLoginConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Login_CCLoginConf" << ": "
        << "account=" << msg.account << ','
        << "nickName=" << msg.nickName << ','
        << "cause=" << msg.cause << ','
        << "loginResult=" << msg.loginResult;

    return ss.str();
}

string to_str(const SIPCCMsg_CClient_CMakeCall &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_CClient_CMakeCall" << ": "
		<< "callID=" << msg.callID << ','
		<< "account=" << msg.account << ','
		<< "targetName=" << msg.targetName << ','
		<< "targetType=" << call_party_type_to_str(msg.targetType) << ','
		<< "callerNoForSend=" << msg.callerNoForSend << ','
		<< "uuiDate=" << msg.uuiDate << ','
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str() << ','
		<< "callType=" << to_str(msg.callType) << ','
		<< "mediaRequest=" << msg.mediaRequest.to_str();

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_CMakeCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_CMakeCallConf" << ": "
        << "callID=" << msg.callID << ','
        << "account=" << msg.account << ','
        << "result=" << msg.result << ','
        << "cause=" << msg.cause;

    return ss.str();
}

string to_str(const SIPCCMsg_CClient_HangupCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CClient_HangupCall" << ": "
        << "callID=" << msg.callID << ','
        << "account=" << msg.account << ','
        << "targetName=" << msg.targetName << ','
        << "targetType=" << device_type_to_str(msg.targetType) << ','
        << "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_HangupCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_HangupCall" << ": "
        << "callID=" << msg.callID << ','
        << "account=" << msg.account << ','
        << "targetName=" << msg.targetName << ','
        << "targetType=" << device_type_to_str(msg.targetType) << ','
        << "callType=" << to_str(msg.callType);

    return ss.str();
}

string to_str(const SIPCCMsg_CProxy_HangupCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_HangupCallConf" << ": "
        << "callID=" << msg.callID << ','
        << "account=" << msg.account;

    return ss.str();
}

string to_str(const SIPCCMsg_CClient_HangupCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CClient_HangupCallConf" << ": "
        << "callID=" << msg.callID << ','
        << "account=" << msg.account << ','
        << "result=" << msg.result << ','
        << "fromAgentID=" << msg.fromAgentID << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_CallUserConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_CallUserConf" << ": "
        << "managerId=" << msg.managerId << ','
        << "departmentID=" << msg.departmentID << ','
        << "userName=" << msg.GetUserName() << ','
        << "callerParty=" << msg.GetCallerParty() << ','
        << "calledNo=" << msg.GetCalledNo() << ','
        << "calleeNo=" << msg.calleeNo << ','
        << "connectPhoneNo=" << msg.connectPhoneNo << ','
        << "invokeID=" << msg.invokeID << ','
        << "failedCause=" << msg.failedCause << ','
        << "bCallFailed=" << (int)msg.bCallFailed << ','
        << "audioIP=" << format_ip_addr(msg.audioIP) << ','
        << "audioPort=" << ntohs(msg.audioPort) << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "outDialType=" << msg.outDialType << ','
        << "conferencePort1=" << ntohs(msg.conferencePort1) << ','
        << "conferencePort2=" << ntohs(msg.conferencePort2) << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_UserAnswerCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserAnswerCall" << ": "
        << "userName=" << msg.GetUserName() << ','
        << "theOtherParty=" << msg.GetTheOTherParty() << ','
        << "connectPhoneNo=" << msg.connectPhoneNo << ','
        << "departmentID=" << msg.departmentID << ','
        << "managerId=" << msg.managerId << ','
        << "handleId=" << msg.handleId << ','
        << "outDialType=" << msg.outDialType << ','
        << "userLineNo=" << msg.userLineNo << ','
        << "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
        << "userLinePort=" << ntohs(msg.userLinePort) << ','
        << "privateData=" << msg.privateData << ','
        << "lineType=" << msg.lineType << ','
        << "theotherPartyType=" << msg.theotherPartyType << ','
        << "asd=" << (const_cast<SIPCCMsg_Client_UserAnswerCall&>(msg)).audioStreamDescription.to_str();

    return ss.str();
}

string to_str(const SIPCCMsg_Client_UserAnswerCall_Real &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserAnswerCall_Real" << ": "
        << "userName=" << (const_cast<SIPCCMsg_Client_UserAnswerCall_Real&>(msg)).GetUserName() << ','
        << "theOtherParty=" << (const_cast<SIPCCMsg_Client_UserAnswerCall_Real&>(msg)).GetTheOTherParty() << ','
        << "connectPhoneNo=" << msg.connectPhoneNo << ','
        << "departmentID=" << msg.departmentID << ','
        << "managerId=" << msg.managerId << ','
        << "handleId=" << msg.handleId << ','
        << "outDialType=" << msg.outDialType << ','
        << "userLineNo=" << msg.userLineNo << ','
        << "userLineIp=" << format_ip_addr(msg.userLineIp) << ','
        << "userLinePort=" << ntohs(msg.userLinePort) << ','
        << "theotherPartyType=" << msg.theotherPartyType << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_UserHangupCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserHangupCall" << ": "
        << "userName=" << (const_cast<SIPCCMsg_Client_UserHangupCall&>(msg)).GetUserName() << ','
        << "theOtherParty=" << (const_cast<SIPCCMsg_Client_UserHangupCall&>(msg)).GetTheOtherParty() << ','
        << "connectPhoneNo=" << msg.connectPhoneNo << ','
        << "callID=" << msg.callID << ','
        << "departmentID=" << msg.departmentID << ','
        << "managerId=" << msg.managerId << ','
        << "bAgentAssigned=" << (int)msg.bAgentAssigned << ','
        << "outDialType=" << msg.outDialType << ','
        << "bIvrHangupCall=" << (int)msg.bIvrHangupCall << ','
        << "netPath=" << msg.netPath;

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_CHangupCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CHangupCallConf" << ": "
        << "callID=" << msg.callID << ','
        << "agentName=" << msg.agentName << ','
        << "keyword=" << msg.keyword << ','
        << "account=" << msg.account;

    return ss.str();
}

string to_str(const SIPCCMsg_Client_MediaRequest &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_MediaRequest" << ": "
        << "mediaType=" << msg.mediaType << ','
        << "seq=" << msg.seq << ','
        //<< "toAcdOrUserState=" << (int)msg.toAcdOrUserState << ','
        << "account=" << msg.account << ','
        << "transmitServerIP=" << format_ip_addr(msg.transmitServerIP) << ','
        << "transmitServerPort=" << ntohs(msg.transmitServerPort) << ','
        << "netPath=" << msg.netPath << ','
        << "callID=" << msg.GetCallID() << ','
        << "targetName=" << msg.GetTargetName();

    return ss.str();
}

string to_str(const SIPCCMsg_Client_MediaRequestConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_MediaRequestConf" << ": "
        << "mediaType   " << msg.mediaType << ','
        << "account     " << msg.account << ','
        << "targetName  " << msg.targetName << ','
        << "netPath     " << msg.netPath << ','
        << "callID      " << msg.GetCallID();

    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_ServiceRegisterConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServiceRegisterConf" << ": "
        << "HBServerIp=" << format_ip_addr(msg.HeartBeatServerIp) << ','
        << "HBServerPort=" << ntohs(msg.HeartBeatServerPort) << ','
        << "HBServerBackupIp=" << format_ip_addr(msg.HeartBeatServerBackupIp) << ','
        << "HBServerBackupIPort=" << ntohs(msg.HeartBeatServerBackupIPort) << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ','
        << "Service1ID=" << msg.Service1ID << ','
        << "Service1Type=" << ESeverType_to_str(msg.Service1Type) << ','
        << "service1IP=" << format_ip_addr(msg.service1IP) << ','
        << "service1Port=" << ntohs(msg.service1Port) << ','
        << "Service2ID=" << msg.Service2ID << ','
        << "Service2Type=" << ESeverType_to_str(msg.Service2Type) << ','
        << "service2IP=" << format_ip_addr(msg.service2IP) << ','
        << "service2Port=" << ntohs(msg.service2Port) << ','
        << "HBServerID=" << msg.HeartBeatServerID << ','
        << "HBServerBackupID=" << msg.HeartBeatServerBackupID << ','
        << "operationMode=" << msg.operationMode << ','
        << "RegServiceID=" << msg.RegServiceID;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_ServiceConfirmConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServiceConfirmConf" << ": "
        << "InvokeId=" << msg.InvokeId << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType);
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_GetHeartBeatConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_GetHeartBeatConf" << ": "
        << "InvokeId=" << msg.InvokeId << ','
        << "HeartBeatServerIp=" << format_ip_addr(msg.HeartBeatServerIP) << ','
        << "HeartBeatServerPort=" << ntohs(msg.HeartBeatServerPort) << ','
        << "HeartBeatServerBackupIp=" << format_ip_addr(msg.HeartBeatServerBackupIP) << ','
        << "HeartBeatServerBackupPort=" << ntohs(msg.HeartBeatServerBackupPort) << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ','
        << "HeartBeatServerID=" << msg.HeartBeatServerID << ','
        << "HeartBeatServerBackupID=" << msg.HeartBeatServerBackupID << ','
        << "operationMode=" << msg.operationMode;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_ServiceConfirm &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServiceConfirm" << ": "
        << "InvokeId=" << msg.InvokeId << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ','
        << "serviceWanIP=" << format_ip_addr(msg.serviceWanIP) << ','
        << "serviceLanIP=" << format_ip_addr(msg.serviceLanIP) << ','
        << "servicePort=" << ntohs(msg.servicePort);
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_ServerHeartBeatConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServerHeartBeatConf" << ": "
        << "resultCode=" << msg.resultCode << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "sendtime=" << msg.sendtime;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_ServiceTimeout &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServiceTimeout" << ": "
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "SelfServiceType=" << ESeverType_to_str(msg.SelfServiceType) << ','
        << "SelfServiceID=" << msg.SelfServiceID;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_SwitchService_Ctrl &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_SwitchService_Ctrl" << ": "
        << "InvokeId=" << msg.InvokeId << ','
        << "ManagerId=" << msg.ManagerId << ','
        << "ServerIp=" << format_ip_addr(msg.ServerIp) << ','
        << "ServerPort=" << ntohs(msg.ServerPort) << ','
        << "ServiceID_Old=" << msg.ServiceID_Old << ','
        << "ServiceID_New=" << msg.ServiceID_New << ','
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ','
        << "gatewayName=" << msg.gatewayName << ','
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string to_str(const SIPCCMsg_Client_UserLoginStateSvr &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserLoginStateSvr" << ": "
        << "natType=" << msg.natType << ','
        << "localIP=" << format_ip_addr(msg.localIP) << ','
        << "localPort=" << ntohs(msg.localPort) << ','
        << "GwName=" << msg.GwName << ','
        << "ComputerSymbol=" << msg.ComputerSymbol << ','
        << "allUserName=" << msg.allUserName << ','
        << "indexNum=" << msg.indexNum << ','
        << "wanIP=" << format_ip_addr(msg.wanIP) << ','
        << "wanPort=" << ntohs(msg.wanPort) << ','
        << "handleId=" << msg.handleId << ','
        << "totalIndex=" << msg.totalIndex << ','
        << "userName=" << msg.GetUserName();
    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_Login &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_Login" << ": "
        << "trunkCount=" << msg.trunkCount << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "lanPort=" << ntohs(msg.lanPort) << ','
        << "gwPartNo=" << msg.gwPartNo << ','
        << "natType=" << msg.natType << ','
        << "wanIP=" << format_ip_addr(msg.wanIP) << ','
        << "wanPort=" << ntohs(msg.wanPort) << ','
        << "ConferenceAgentCount=" << msg.ConferenceAgentCount << ','
        << "isProxyRegister=" << (msg.isProxyRegister ? "true" : "false") << ','
        << "gatewayName=" << msg.GetGatewayName() << ','
        << "computerSymbol=" << msg.GetComputerSymbol() << ','
        << "theAccount=" << msg.GetTheAccount() << ','
        << "thePassword=" << msg.GetThePassword();
    return ss.str();
}

string to_str(const SIPCCMsg_Gateway_LoginConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_LoginConf" << ": "
        << "bSucc=" << (msg.bSucc ? "true" : "false") << ','
        << "HeartBeatServerIP=" << format_ip_addr(msg.HeartBeatServerIP) << ','
        << "HeartBeatServerPort=" << ntohs(msg.HeartBeatServerPort) << ','
        << "HeartBeatServerBackupIP=" << format_ip_addr(msg.HeartBeatServerBackupIP) << ','
        << "HeartBeatServerBackupPort=" << ntohs(msg.HeartBeatServerBackupPort) << ','
        << "TrunkDataDistributeServerIp=" << format_ip_addr(msg.TrunkDataDistributeServerIp) << ','
        << "TrunkDataDistributeServerPort=" << ntohs(msg.TrunkDataDistributeServerPort) << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "HeartBeatServerID=" << msg.HeartBeatServerID << ','
        << "HeartBeatServerBackupID=" << msg.HeartBeatServerBackupID << ','
        << "recServerID=" << msg.recServerID << ','
        << "recSymbol=" << msg.recSymbol << ','
        << "recServerIP=" << msg.recServerIP << ','
        << "recFilePath=" << msg.recFilePath << ','
        << "recPathMode=" << msg.recPathMode << ','
        << "recServerID_Backup=" << msg.recServerID_Backup << ','
        << "recSymbol_Backup=" << msg.recSymbol_Backup << ','
        << "recServerIP_Backup=" << msg.recServerIP_Backup << ','
        << "recFilePath_Backup=" << msg.recFilePath_Backup << ','
        << "recPathMode_Backup=" << msg.recPathMode_Backup << ','
        << "operationMode=" << msg.operationMode << ','
        << "needProxyRegister=" << msg.needProxyRegister << ','
        << "gatewayName=" << msg.gatewayName << ','
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_ServiceRegister &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServiceRegister" << ": "
        << "ServerName=" << msg.ServerName << ','
        << "ServiceType=" << ESeverType_to_str(msg.ServerType) << ','
        << "wanIP=" << format_ip_addr(msg.wanIP) << ','
        << "wanPort=" << ntohs(msg.wanPort) << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "lanPort=" << ntohs(msg.lanPort) << ','
        << "ServerID=" << msg.ServerID;
    return ss.str();
}

string to_str(const SIPCCMsg_Trans_TransmitStopRequest &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Trans_TransmitStopRequest" << ": "
        << "callID=" << (const_cast<SIPCCMsg_Trans_TransmitStopRequest&>(msg)).GetCallID();

    return ss.str();
}

string to_str(const SIPCCMsg_FileTransfer_UploadRequest &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_FileTransfer_UploadRequest" << ": "
        << "agentID=" << msg.agentID << ','
        << "description=" << msg.description << ','
        << "address=" << msg.address << ','
        << "managerID=" << msg.managerID;

    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_UserLoginStateSvrConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_UserLoginStateSvrConf" << ": "
        << "GwName=" << msg.GwName << ','
        << "ComputerSymbol=" << msg.ComputerSymbol << ','
        << "resultCode=" << msg.resultCode << ','
        << "ServerName=" << msg.ServerName << ','
        << "ServiceType=" << msg.ServiceType << ','
        << "ServiceID=" << msg.ServiceID << ','
        << "handleId=" << msg.handleId << ','
        << "originalIP=" << format_ip_addr(msg.originalIP) << ','
        << "originalPort=" << ntohs(msg.originalPort);
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_GWLogout &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_GWLogout" << ": "
        << "GwName=" << msg.GwName << ','
        << "ComputerSymbol=" << msg.ComputerSymbol;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_GWLogin &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_GWLogin" << ": "
        << "gwName=" << msg.gwName << ','
        << "computerSymbol=" << msg.computerSymbol << ','
        << "trunkCount=" << msg.trunkCount << ','
        << "lanIP=" << format_ip_addr(msg.lanIP) << ','
        << "lanPort=" << ntohs(msg.lanPort) << ','
        << "gwPartNo=" << msg.gwPartNo << ','
        << "natType=" << msg.natType << ','
        << "natIP=" << format_ip_addr(msg.natIP) << ','
        << "natPort=" << ntohs(msg.natPort) << ','
        << "ConferenceAgentCount=" << msg.ConferenceAgentCount;
    return ss.str();
}

string to_str(const SIPCCMsg_OneKeySwitch_UserLogin &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_UserLogin" << ": "
        << "localIP=" << format_ip_addr(msg.localIP) << ','
        << "localPort=" << ntohs(msg.localPort) << ','
        << "GwName=" << msg.GwName << ','
        << "ComputerSymbol=" << msg.ComputerSymbol << ','
        << "allUserName=" << msg.allUserName << ','
        << "indexNum=" << msg.indexNum << ','
        << "natIP=" << format_ip_addr(msg.natIP) << ','
        << "natPort=" << ntohs(msg.natPort) << ','
        << "handleId=" << msg.handleId << ','
        << "totalIndex=" << msg.totalIndex;
    return ss.str();
}

string to_str(const SIPCCMsg_UState_CallReceived &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallReceived" << ": "
        << "callerParty=" << msg.GetCallerParty() << ','
        << "calledNum=" << msg.GetCalledNum() << ','
        << "callID=" << msg.GetCallID() << ','
        << "corporationName=" << msg.GetCorporationName() << ','
        << "callerNoForSend=" << msg.callerNoForSend << ','
        << "callerPartyType=" << call_party_type_to_str(msg.callerPartyType) << ','
        << "wpAgentID=" << msg.wpAgentID << ','
        << "outdialPrefixNo=" << msg.outdialPrefixNo << ','
        << "uuiData=" << msg.uuiData << ','
        << "operatorsInfo=" << msg.operatorsInfo << ','
        << "managerId=" << msg.managerId << ','
        << "gatewayName=" << msg.gatewayName << ','
        << "computerSymbol=" << msg.computerSymbol << ','
        << "netPath=" << msg.netPath << ','
        << "wpAgentPhoneNo=" << msg.wpAgentPhoneNo << ','
        << "trunkProviderID=" << msg.trunkProviderID << ','
        << "sipTrunkIP=" << msg.sipTrunkIP << ','
        << "sipTrunkPort=" << msg.sipTrunkPort << ','
        << "departmentID=" << msg.departmentID;

    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AddCorpNDeptInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_AddCorpNDeptInfo" << ": "
        << "corpName=" << msg.corpName << ','
        << "deptName=" << msg.deptName;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_LoadDepartments &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_LoadDepartments" << ": "
        << "acdserverID=" << msg.acdserverID << ": "
        << "managerID=" << msg.managerID << ": "
        << "corpName=" << msg.corpName;
    return ss.str();
}

string to_str(const SIPCCMsg_SysManage_AddDepartment &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_SysManage_AddDepartment" << ": "
        << "timeoutValue=" << msg.timeoutValue << ','
        << "overFlowDest=" << msg.overFlowDest << ','
        << "overFlowDestType=" << msg.overFlowDestType << ','
        << "maxRingTime=" << msg.maxRingTime << ','
        << "deptType=" << msg.deptType << ','
        << "isOutDial=" << msg.isOutDial << ','
        << "outdialGroupID=" << msg.outdialGroupID << ','
        << "invokeID=" << msg.invokeID << ','
        << "managerid=" << msg.managerid << ','
        << "deptid=" << msg.deptid << ','
        << "nickname=" << msg.nickname << ','
        << "generalConfiguration=" << msg.generalConfiguration << ','
        << "queuingAlgorithm=" << msg.queuingAlgorithm << ','
        << "diaEngineIP=" << msg.dialEngineIP << ','
        << "diaEnginePort=" << msg.dialEnginePort;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AddDepartmentConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_AddDepartmentConf" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "Department=" << msg.GetTheDepartment();
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_ResetDataCollect &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_ResetDataCollect" << ": "
        << "curTime=" << msg.curTime;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_ResetMonitor &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_ResetMonitor" << ": "
        << "curTime=" << msg.curTime;
    return ss.str();
}

string to_str(const SIPCCMsg_ACD_AgentStateInfo &msg)
{
    return "";
    stringstream ss;
    ss << "SIPCCMsg_ACD_AgentStateInfo" << ": "
        << "callerID=" << msg.callerID << ','
        << "calleeID=" << msg.calleeID << ','
        << "bAgentLogin=" << (msg.bAgentLogin ? "1" : "0") << ','
        << "managerid=" << msg.managerid << ','
        << "deptid=" << msg.deptid << ','
        << "bCallOutTypeUnderHold=" << (msg.bCallOutTypeUnderHold ? "1" : "0") << ','
        << "nickName=" << msg.nickName << ','
        << "extNo=" << msg.extNo << ','
        << "SkillGroupID=" << msg.SkillGroupID << ','
        << "SkillGroupName=" << msg.SkillGroupName << ','
        << "SkillGroupID_Original=" << msg.SkillGroupID_Original << ','
        << "SkillGroupName_Original=" << msg.SkillGroupName_Original << ','
        << "UserLevel=" << msg.UserLevel << ','
        << "sData=" << msg.sData << ','
        << "curTime=" << msg.GetCurTime() << ','
        << "callID=" << msg.GetCallID() << ','
        << "agentID=" << msg.GetAgentID() << ','
        << "corporationName=" << msg.GetCorporationName() << ','
        << "theDepartment=" << msg.GetTheDepartment() << ','
        << "keyWord=" << msg.GetKeyWord() << ','
        << "userName=" << msg.GetUserName() << ','
        << "parkReason=" << msg.GetParkReason();
    return ss.str();
}

string to_str(const SIPCCMsg_Trans_TransmitRequestConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Trans_TransmitRequestConf" << ": "
        << "textPort=" << ntohs(msg.textPort) << ','
		<< "audioPort=" << ntohs(msg.audioPort) << ','
		<< "callID=" << msg.callID;
    return ss.str();
}

string SIPCCMsg_Redirect_SearchPlatform_to_str(const SIPCCMsg_Redirect_SearchPlatform &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Redirect_SearchPlatformConf_to_str(const SIPCCMsg_Redirect_SearchPlatformConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_UserLogin_to_str(const SIPCCMsg_Client_UserLogin &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Login_UserLoginSucc_to_str(const SIPCCMsg_Login_UserLoginSucc &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Login_UserLoginFailed_to_str(const SIPCCMsg_Login_UserLoginFailed &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_NotifyAgentLogin_to_str(const SIPCCMsg_Client_NotifyAgentLogin &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Login_NotifyAgentLoginConf_to_str(const SIPCCMsg_Login_NotifyAgentLoginConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_GetParkReasons_to_str(const SIPCCMsg_Client_GetParkReasons &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Login_GetParkReasonsConf_to_str(const SIPCCMsg_Login_GetParkReasonsConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_QueryIncrementFunc_to_str(const SIPCCMsg_Client_QueryIncrementFunc &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Login_QueryIncrementFuncConf_to_str(const SIPCCMsg_Login_QueryIncrementFuncConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_GetService_to_str(const SIPCCMsg_OneKeySwitch_GetService &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_GetServiceConf_to_str(const SIPCCMsg_OneKeySwitch_GetServiceConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_GetHeartBeat_to_str(const SIPCCMsg_OneKeySwitch_GetHeartBeat &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentLogin_to_str(const SIPCCMsg_Client_AgentLogin &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_QueryGWLanInfo_to_str(const SIPCCMsg_Client_QueryGWLanInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentQueryState_to_str(const SIPCCMsg_Client_AgentQueryState &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AgentQueryStateConf_to_str(const SIPCCMsg_ACD_AgentQueryStateConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_QueryGWLanInfoConf_to_str(const SIPCCMsg_Gateway_QueryGWLanInfoConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AgentLoginSucc_to_str(const SIPCCMsg_ACD_AgentLoginSucc &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AgentLoginFailed_to_str(const SIPCCMsg_ACD_AgentLoginFailed &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_ForceLogout_to_str(const SIPCCMsg_ACD_ForceLogout &msg)
{
    return to_str(msg);
}
string SIPCCMsg_HeartBeat_to_str(const SIPCCMsg_HeartBeat &msg)
{
    return to_str(msg);
}
string SIPCCMsg_HeartBeatResponse_to_str(const SIPCCMsg_HeartBeatResponse &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_QueryGW_to_str(const SIPCCMsg_Client_QueryGW &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_QueryGWConf_to_str(const SIPCCMsg_Gateway_QueryGWConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentMakeCall_to_str(const SIPCCMsg_Client_AgentMakeCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_UState_AgentMakeCallConf_to_str(const SIPCCMsg_UState_AgentMakeCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AgentMakeCallConf_to_str(const SIPCCMsg_ACD_AgentMakeCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallUserConf_to_str(const SIPCCMsg_ACD_CallUserConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_DataTrans_StartTrans_to_str(const SIPCCMsg_DataTrans_StartTrans &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentMakeCall2_to_str(const SIPCCMsg_Client_AgentMakeCall2 &msg)
{
    return to_str(msg);
}
string SIPCCMsg_DataTrans_StartTransConf_to_str(const SIPCCMsg_DataTrans_StartTransConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallEstablished_to_str(const SIPCCMsg_ACD_CallEstablished &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_RecInfo2Client_to_str(const SIPCCMsg_ACD_RecInfo2Client &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallEstablished_Real_to_str(const SIPCCMsg_ACD_CallEstablished_Real &msg)
{
    return to_str(msg);
}
string SIPCCMsg_AreaQuery_to_str(const SIPCCMsg_AreaQuery &msg)
{
    return to_str(msg);
}
string SIPCCMsg_AreaAnswer_to_str(const SIPCCMsg_AreaAnswer &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentHangupCall_to_str(const SIPCCMsg_Client_AgentHangupCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_UState_CallReleased_to_str(const SIPCCMsg_UState_CallReleased &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallReleased_to_str(const SIPCCMsg_ACD_CallReleased &msg)
{
    return to_str(msg);
}
string SIPCCMsg_DataTrans_StopTrans_to_str(const SIPCCMsg_DataTrans_StopTrans &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentHoldCall_to_str(const SIPCCMsg_Client_AgentHoldCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_UState_AgentHoldCallConf_to_str(const SIPCCMsg_UState_AgentHoldCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentUnHoldCall_to_str(const SIPCCMsg_Client_AgentUnHoldCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_BeforeTransfer_to_str(const SIPCCMsg_Client_BeforeTransfer &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_TransferCall_to_str(const SIPCCMsg_Client_TransferCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallArriveToAgent_to_str(const SIPCCMsg_ACD_CallArriveToAgent &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Record_AgentRecordQueryPort_to_str(const SIPCCMsg_Record_AgentRecordQueryPort &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Record_AgentRecordQueryPortConf_to_str(const SIPCCMsg_Record_AgentRecordQueryPortConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Record_AgentRecordStart_to_str(const SIPCCMsg_Record_AgentRecordStart &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Record_AgentRecordStop_to_str(const SIPCCMsg_Record_AgentRecordStop &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_MediaRequestConf_to_str(const SIPCCMsg_ACD_MediaRequestConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Trans_TransmitRequest_to_str(const SIPCCMsg_Trans_TransmitRequest &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentTestNet_to_str(const SIPCCMsg_Client_AgentTestNet &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentTestNetConf_to_str(const SIPCCMsg_Client_AgentTestNetConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_LanPortApprize_to_str(const SIPCCMsg_Client_LanPortApprize &msg)
{
    return to_str(msg);
}
string SIPCCMsg_PhoneSearch_ExtNoInfo_to_str(const SIPCCMsg_PhoneSearch_ExtNoInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_PhoneSearch_ExtNoInfoConf_to_str(const SIPCCMsg_PhoneSearch_ExtNoInfoConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_IsAgentLogin_to_str(const SIPCCMsg_Gateway_IsAgentLogin &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_IsAgentLoginConf_to_str(const SIPCCMsg_ACD_IsAgentLoginConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_GetDepartNAgentInfo_to_str(const SIPCCMsg_Client_GetDepartNAgentInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_DepartNAgentInfo_to_str(const SIPCCMsg_ACD_DepartNAgentInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_ConferenceStart_to_str(const SIPCCMsg_Client_ConferenceStart &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_ConferenceStartConf_to_str(const SIPCCMsg_ACD_ConferenceStartConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_ConferenceEnd_to_str(const SIPCCMsg_Client_ConferenceEnd &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_ConferenceStart_to_str(const SIPCCMsg_ACD_ConferenceStart &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_ConferenceEnd_to_str(const SIPCCMsg_ACD_ConferenceEnd &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_UserLeaveConference_to_str(const SIPCCMsg_ACD_UserLeaveConference &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallDelivered_to_str(const SIPCCMsg_ACD_CallDelivered &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentCallReceived_to_str(const SIPCCMsg_Client_AgentCallReceived &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentAnswerCall_to_str(const SIPCCMsg_Client_AgentAnswerCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_UState_CallEstablished_to_str(const SIPCCMsg_UState_CallEstablished &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_MediaRequest_to_str(const SIPCCMsg_ACD_MediaRequest &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_GetTalkingAgent_to_str(const SIPCCMsg_Client_GetTalkingAgent &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_GetTalkingAgentConf_to_str(const SIPCCMsg_ACD_GetTalkingAgentConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_MonitRequest_to_str(const SIPCCMsg_Client_MonitRequest &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_MonitRequestConf_to_str(const SIPCCMsg_Client_MonitRequestConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_StopMonit_to_str(const SIPCCMsg_Client_StopMonit &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_UserLeaveConference_to_str(const SIPCCMsg_Gateway_UserLeaveConference &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_WebPSTNDial_to_str(const SIPCCMsg_Client_WebPSTNDial &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_WebPSTNDialConf_to_str(const SIPCCMsg_Gateway_WebPSTNDialConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_TextChat_to_str(const SIPCCMsg_Client_TextChat &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_LoginToText_to_str(const SIPCCMsg_Client_LoginToText &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_PrepareTransfer_to_str(const SIPCCMsg_Client_PrepareTransfer &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_PrepareTransfer_to_str(const SIPCCMsg_ACD_PrepareTransfer &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_TransReady_to_str(const SIPCCMsg_Client_TransReady &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_TransferCall_to_str(const SIPCCMsg_ACD_TransferCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_SetClientAgentState_to_str(const SIPCCMsg_ACD_SetClientAgentState &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_GetCallingQueueInfo_to_str(const SIPCCMsg_Client_GetCallingQueueInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CallingQueueInfo_to_str(const SIPCCMsg_ACD_CallingQueueInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_AgentLogout_to_str(const SIPCCMsg_Client_AgentLogout &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_UserLogoutStateSvr_to_str(const SIPCCMsg_Client_UserLogoutStateSvr &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CClient_CallArrived_to_str(const SIPCCMsg_CClient_CallArrived &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CClient_CallAnswered_to_str(const SIPCCMsg_CClient_CallAnswered &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_HeartBeatConf_to_str(const SIPCCMsg_CProxy_HeartBeatConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CMakeCall_to_str(const SIPCCMsg_CProxy_CMakeCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CallDelivered_to_str(const SIPCCMsg_CProxy_CallDelivered &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CallArrived_to_str(const SIPCCMsg_CProxy_CallArrived &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CallAnswered_to_str(const SIPCCMsg_CProxy_CallAnswered &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CallStart_to_str(const SIPCCMsg_CProxy_CallStart &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_QueryGW_to_str(const SIPCCMsg_CProxy_QueryGW &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CMakeCall2_to_str(const SIPCCMsg_CProxy_CMakeCall2 &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Redirect_GetService_to_str(const SIPCCMsg_Redirect_GetService &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Redirect_GetServiceConf_to_str(const SIPCCMsg_Redirect_GetServiceConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CClient_Login_to_str(const SIPCCMsg_CClient_Login &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Login_CCLoginConf_to_str(const SIPCCMsg_Login_CCLoginConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CClient_CMakeCall_to_str(const SIPCCMsg_CClient_CMakeCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_CMakeCallConf_to_str(const SIPCCMsg_CProxy_CMakeCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CClient_HangupCall_to_str(const SIPCCMsg_CClient_HangupCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_HangupCall_to_str(const SIPCCMsg_CProxy_HangupCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CProxy_HangupCallConf_to_str(const SIPCCMsg_CProxy_HangupCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_CClient_HangupCallConf_to_str(const SIPCCMsg_CClient_HangupCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_CallUserConf_to_str(const SIPCCMsg_Client_CallUserConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_UserAnswerCall_to_str(const SIPCCMsg_Client_UserAnswerCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_UserAnswerCall_Real_to_str(const SIPCCMsg_Client_UserAnswerCall_Real &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_UserHangupCall_to_str(const SIPCCMsg_Client_UserHangupCall &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_CHangupCallConf_to_str(const SIPCCMsg_ACD_CHangupCallConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_MediaRequest_to_str(const SIPCCMsg_Client_MediaRequest &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_MediaRequestConf_to_str(const SIPCCMsg_Client_MediaRequestConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_ServiceRegisterConf_to_str(const SIPCCMsg_OneKeySwitch_ServiceRegisterConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_ServiceConfirmConf_to_str(const SIPCCMsg_OneKeySwitch_ServiceConfirmConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_GetHeartBeatConf_to_str(const SIPCCMsg_OneKeySwitch_GetHeartBeatConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_ServiceConfirm_to_str(const SIPCCMsg_OneKeySwitch_ServiceConfirm &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_ServerHeartBeatConf_to_str(const SIPCCMsg_OneKeySwitch_ServerHeartBeatConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_ServiceTimeout_to_str(const SIPCCMsg_OneKeySwitch_ServiceTimeout &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_SwitchService_Ctrl_to_str(const SIPCCMsg_OneKeySwitch_SwitchService_Ctrl &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Client_UserLoginStateSvr_to_str(const SIPCCMsg_Client_UserLoginStateSvr &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_Login_to_str(const SIPCCMsg_Gateway_Login &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Gateway_LoginConf_to_str(const SIPCCMsg_Gateway_LoginConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_ServiceRegister_to_str(const SIPCCMsg_OneKeySwitch_ServiceRegister &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Trans_TransmitStopRequest_to_str(const SIPCCMsg_Trans_TransmitStopRequest &msg)
{
    return to_str(msg);
}
string SIPCCMsg_FileTransfer_UploadRequest_to_str(const SIPCCMsg_FileTransfer_UploadRequest &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_UserLoginStateSvrConf_to_str(const SIPCCMsg_OneKeySwitch_UserLoginStateSvrConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_GWLogout_to_str(const SIPCCMsg_OneKeySwitch_GWLogout &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_GWLogin_to_str(const SIPCCMsg_OneKeySwitch_GWLogin &msg)
{
    return to_str(msg);
}
string SIPCCMsg_OneKeySwitch_UserLogin_to_str(const SIPCCMsg_OneKeySwitch_UserLogin &msg)
{
    return to_str(msg);
}
string SIPCCMsg_UState_CallReceived_to_str(const SIPCCMsg_UState_CallReceived &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AddCorpNDeptInfo_to_str(const SIPCCMsg_ACD_AddCorpNDeptInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_LoadDepartments_to_str(const SIPCCMsg_ACD_LoadDepartments &msg)
{
    return to_str(msg);
}
string SIPCCMsg_SysManage_AddDepartment_to_str(const SIPCCMsg_SysManage_AddDepartment &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AddDepartmentConf_to_str(const SIPCCMsg_ACD_AddDepartmentConf &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_ResetDataCollect_to_str(const SIPCCMsg_ACD_ResetDataCollect &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_ResetMonitor_to_str(const SIPCCMsg_ACD_ResetMonitor &msg)
{
    return to_str(msg);
}
string SIPCCMsg_ACD_AgentStateInfo_to_str(const SIPCCMsg_ACD_AgentStateInfo &msg)
{
    return to_str(msg);
}
string SIPCCMsg_Trans_TransmitRequestConf_to_str(const SIPCCMsg_Trans_TransmitRequestConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_SysManage_AddGWTG &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_SysManage_AddGWTG" << ": "
        << "tgName=" << msg.tgName << ','
        << "gwName=" << msg.gwName << ','
        << "corpName=" << msg.corpName << ','
        << "calledNo=" << msg.calledNo << ','
        << "callinLicence=" << msg.callinLicence << ','
        << "calloutLicence=" << msg.calloutLicence << ','
        << "mappingName=" << msg.mappingName << ','
        << "invokeID=" << msg.invokeID << ','
        << "tgPrefix=" << msg.tgPrefix << ','
        << "computerSymbol=" << msg.computerSymbol << ','
        << "areaCode=" << msg.areaCode << ','
        << "operatorsInfo=" << msg.operatorsInfo << ','
        << "allowCallType=" << msg.allowCallType << ','
        << "trunkCount=" << msg.trunkCount << ','
        << "lowerLimitNum=" << msg.lowerLimitNum;
    return ss.str();
}

string SIPCCMsg_SysManage_AddGWTG_to_str(const SIPCCMsg_SysManage_AddGWTG &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_SysManage_AddDeptTG &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_SysManage_AddDeptTG" << ": "
        << "corpName=" << msg.corpName << ','
        << "deptName=" << msg.deptName << ','
        << "specialTGName=" << msg.specialTGName << ','
        << "innerTGName=" << msg.innerTGName << ','
        << "backupTGName=" << msg.backupTGName << ','
        << "longDisTGName=" << msg.longDisTGName << ','
        << "enableDialNoRule=" << msg.enableDialNoRule << ','
        << "invokeID=" << msg.invokeID << ','
        << "defaultIADGName=" << msg.defaultIADGName;
    return ss.str();
}

string SIPCCMsg_SysManage_AddDeptTG_to_str(const SIPCCMsg_SysManage_AddDeptTG &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_CProxy_Login &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_Login" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "account=" << msg.account;
    return ss.str();
}

string SIPCCMsg_CProxy_Login_to_str(const SIPCCMsg_CProxy_Login &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_CProxy_LoginConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_LoginConf" << ": "
        << "requestInvokeID=" << msg.requestInvokeID << ','
        << "account=" << msg.account << ','
        << "error=" << msg.error << ','
        << "description=" << msg.description;
    return ss.str();
}

string SIPCCMsg_CProxy_LoginConf_to_str(const SIPCCMsg_CProxy_LoginConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_CProxy_Logout &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_Logout" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "account=" << msg.account;
    return ss.str();
}

string SIPCCMsg_CProxy_Logout_to_str(const SIPCCMsg_CProxy_Logout &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_CProxy_LogoutConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_CProxy_LogoutConf" << ": "
        << "requestInvokeID=" << msg.requestInvokeID << ','
        << "account=" << msg.account << ','
        << "error=" << msg.error << ','
        << "description=" << msg.description;
    return ss.str();
}

string SIPCCMsg_CProxy_LogoutConf_to_str(const SIPCCMsg_CProxy_LogoutConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OneKeySwitch_NotifyMsg &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_NotifyMsg" << ": "
        << "invokeID=" << msg.invokeID << ','
        << "contentType=" << msg.contentType << ','
        << "content=" << msg.content << ','
        << "targets=[";
    for (int i = 0; i < msg.targets.size(); i++)
    {
        ss << msg.targets[i].type << ":";
        if (msg.targets[i].targetNames.size() > 0)
        {
            ss << msg.targets[i].targetNames[0];
        }
        for (int j = 1; j < msg.targets[i].targetNames.size(); j++)
        {
            ss << "," << msg.targets[i].targetNames[j];
        }
        ss << ";";
    }
    ss << "]";
    return ss.str();
}

string SIPCCMsg_OneKeySwitch_NotifyMsg_to_str(const SIPCCMsg_OneKeySwitch_NotifyMsg &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_SysManage_LoadServerInfoConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_SysManage_LoadServerInfoConf: "
        << "ManagerID=" << msg.ManagerID << ","
        << "CorpName=" << msg.CorpName << ","
        << "ACDServerIP=" << msg.ACDServerIP << ","
        << "PhoneNo=" << msg.PhoneNo << ","
        << "DataCollectServerIP=" << msg.DataCollectServerIP << ","
        << "DataCollectServerPort=" << msg.DataCollectServerPort << ","
        << "bLoad=" << (msg.bLoad ? 1 : 0) << ","
        << "InvokeId=" << msg.InvokeId << ","
        << "indexnum=" << msg.indexnum;
    return ss.str();
}

string SIPCCMsg_SysManage_LoadServerInfoConf_to_str(const SIPCCMsg_SysManage_LoadServerInfoConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OneKeySwitch_GWLoginConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_GWLoginConf: "
        << "GwName=" << msg.GwName << ","
        << "ComputerSymbol=" << msg.ComputerSymbol << ","
        << "ServerName=" << msg.ServerName << ","
        << "ServiceType=" << ESeverType_to_str(msg.ServiceType) << ","
        << "ServiceID=" << msg.ServiceID;
    return ss.str();
}

string SIPCCMsg_OneKeySwitch_GWLoginConf_to_str(const SIPCCMsg_OneKeySwitch_GWLoginConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_GCTI_Message &msg)
{
    string context = msg.context;

#ifdef INFOBIRD_CTI
    if (context.size() > 0 && context[0] == '[')
    {
        int b = context.find('|');
        int e = context.find('|', b + 1);
        if (e > 0)
        {
            context = to_str(E_IBCTI_MessageType(StringToNumber<int>(context.substr(b + 1, e - 1 - b)))) +
                context;
        }
    }
#endif

    stringstream ss;
    ss << "SIPCCMsg_GCTI_Message: "
        << "context=" << context << ","
        << "InvokeId=" << msg.InvokeId << ","
        << "uinvokeID=" << msg.uinvokeID << ","
        << "strorigin=" << msg.strorigin;
    return ss.str();
}

string SIPCCMsg_GCTI_Message_to_str(const SIPCCMsg_GCTI_Message &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_CallReceived2 &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallReceived2: "
        << "uuiData=" << msg.GetUUIData() << ","
        << "userName=" << msg.userName << ","
        << "callerParty=" << msg.GetCallerParty() << ","
        << "callerNoForSend=" << msg.callerNoForSend << ","
        << "calleeAreaCode=" << msg.calleeAreaCode << ","
        << "callID=" << msg.GetCallID() << ","
        << "corporationName=" << msg.GetCorporationName() << ","
        << "callerPartyType=" << msg.callerPartyType << ","
        << "invokeID=" << msg.invokeID << ","
        << "usableTime=" << msg.usableTime << ","
        << "lanIp=" << format_ip_addr(msg.lanIp) << ","
        << "wanIp=" << format_ip_addr(msg.wanIp) << ","
        << "outDialType=" << msg.outDialType << ","
        << "calledOperType=" << msg.calledOperType << ","
        << "wpAgentID=" << msg.wpAgentID << ","
        << "callTransID=" << msg.callTransID << ","
        << "managerId=" << msg.managerId << ","
        << "departmentID=" << msg.departmentID << ","
        << "corpName=" << msg.corpName << ","
        << "extPhoneNo=" << msg.extPhoneNo << ","
        << "nickName=" << msg.nickName << ","
        << "RecordMode=" << msg.RecordMode << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol << ","
        << "asd=" << msg.audioStreamDescription.to_str() << ","
        << "token=" << msg.token << ","
        << "wpAgentPhoneNo=" << msg.wpAgentPhoneNo << ','
        << "trunkProviderID=" << msg.trunkProviderID << ','
        << "sipTrunkIP=" << msg.sipTrunkIP << ','
        << "sipTrunkPort=" << msg.sipTrunkPort << ','
        << "xnum=" << msg.xnum;

    return ss.str();
}

string SIPCCMsg_UState_CallReceived2_to_str(const SIPCCMsg_UState_CallReceived2 &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_CallHanguped &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallHanguped: "
        << "userName=" << msg.GetUserName() << ","
        << "callID=" << msg.GetCallID() << ","
        << "releasedParty=" << msg.GetReleasedParty() << ","
        << "handleId=" << msg.handleId << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string SIPCCMsg_UState_CallHanguped_to_str(const SIPCCMsg_UState_CallHanguped &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_LoadGWCorpInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_LoadGWCorpInfo: "
        << "ServerName=" << msg.ServerName;
    return ss.str();
}

string SIPCCMsg_Gateway_LoadGWCorpInfo_to_str(const SIPCCMsg_Gateway_LoadGWCorpInfo &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_SysManage_AddGWCorp &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_SysManage_AddGWCorp: "
        << "corpName=" << msg.corpName << ","
        << "defaultTG=" << msg.defaultTG << ","
        << "enableDialNoRule=" << ((int)msg.enableDialNoRule) << ","
        << "invokeID=" << msg.invokeID;
    return ss.str();
}

string SIPCCMsg_SysManage_AddGWCorp_to_str(const SIPCCMsg_SysManage_AddGWCorp &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OneKeySwitch_ServiceUnavailable &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OneKeySwitch_ServiceUnavailable: "
        << "invokeID=" << msg.invokeID << ","
        << "unavailableSID=" << msg.unavailableServiceID << ","
        << "unavailableSType=" << ESeverType_to_str(msg.unavailableServiceType);
    return ss.str();
}

string SIPCCMsg_OneKeySwitch_ServiceUnavailable_to_str(const SIPCCMsg_OneKeySwitch_ServiceUnavailable &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_CallAgentFailed &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_CallAgentFailed: "
        << "bCallFailed=" << (msg.bCallFailed ? 1 : 0) << ","
        << "calledNo=" << msg.calledNo << ","
        << "originAgentID=" << msg.originAgentID << ","
        << "CalledAgentID=" << msg.CalledAgentID << ","
        << "originAgentLineNo=" << msg.originAgentLineNo << ","
        << "calledAgentLineNo=" << msg.calledAgentLineNo << ","
        << "originAgentIp=" << format_ip_addr(msg.originAgentIp) << ","
        << "originAgentPort=" << htons(msg.originAgentPort) << ","
        << "calledAgentLineIp=" << msg.calledAgentLineIp << ","
        << "calledAgentLinePort=" << msg.calledAgentLinePort << ","
        << "failedCause=" << msg.failedCause << ","
        << "managerId=" << msg.managerId << ","
        << "connectPhoneNo=" << msg.connectPhoneNo << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_Gateway_CallAgentFailed_to_str(const SIPCCMsg_Gateway_CallAgentFailed &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Client_UserQueryCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserQueryCall: "
        << "invokeID=" << msg.invokeID << ","
        << "keyWord=" << msg.GetKeyWord();
    return ss.str();
}

string SIPCCMsg_Client_UserQueryCall_to_str(const SIPCCMsg_Client_UserQueryCall &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Guide_ACDInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Guide_ACDInfo: "
        << "invokeID=" << msg.invokeID << ","
        << "theDepartment=" << msg.GetTheDepartment() << ","
        << "managerID=" << msg.managerID << ","
        << "departmentID=" << msg.departmentID << ","
        << "UserLevel=" << msg.UserLevel << ","
        << "acdServerIP=" << format_ip_addr(msg.acdServerIP) << ","
        << "acdServerPort=" << ntohs(msg.acdServerPort) << ","
        << "CallTargetType=" << CallTargetType_to_str(msg.CallTargetType);
    return ss.str();
}

string SIPCCMsg_Guide_ACDInfo_to_str(const SIPCCMsg_Guide_ACDInfo &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_CallHanguped &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallHanguped: "
        << "userName=" << msg.GetUserName() << ","
        << "releasedParty=" << msg.GetReleasedParty() << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_ACD_CallHanguped_to_str(const SIPCCMsg_ACD_CallHanguped &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_UserMakeCallConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_UserMakeCallConf: "
        << "userName=" << msg.GetUserName() << ","
        << "agentID=" << msg.GetAgentID() << ","
        << "callingState=" << msg.callingState << ","
        << "userNumInQueue=" << msg.userNumInQueue << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_ACD_UserMakeCallConf_to_str(const SIPCCMsg_ACD_UserMakeCallConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Client_UserMakeCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_UserMakeCall: "
        << "userName=" << msg.GetUserName() << ","
        << "theDepartment=" << msg.GetTheDepartment() << ","
        << "keyWord=" << msg.GetKeyWord() << ","
        << "uuiData=" << msg.GetUUIData() << ","
        << "SkillGroupID=" << msg.SkillGroupID << ","
        << "departmentID=" << msg.departmentID << ","
        << "managerId=" << msg.managerId << ","
        << "connectPhoneNo=" << msg.connectPhoneNo << ","
        << "UserLevel=" << msg.UserLevel << ","
        << "CallTargetType=" << CallTargetType_to_str(msg.CallTargetType) << ","
        << "gwPartNo=" << msg.gwPartNo << ","
        << "comSymbol=" << msg.comSymbol << ","
        << "handleId=" << msg.handleId << ","
        << "callTransID=" << msg.callTransID << ","
        << "userType=" << msg.userType << ","
        << "audioIP=" << format_ip_addr(msg.audioIP) << ","
        << "audioPort=" << ntohs(msg.audioPort) << ","
        << "lanIP=" << format_ip_addr(msg.lanIP) << ","
        << "audioRtpPort=" << ntohs(msg.audioRtpPort) << ","
        << "conferencePort1=" << format_ip_addr(msg.conferencePort1) << ","
        << "conferencePort2=" << format_ip_addr(msg.conferencePort2) << ","
        << "asd=" << msg.audioStreamDescription.to_str();

    return ss.str();
}

string SIPCCMsg_Client_UserMakeCall_to_str(const SIPCCMsg_Client_UserMakeCall &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Redirect_AppServerListConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Redirect_AppServerListConf: "
        << "server=" << msg.server;
    return ss.str();
}

string SIPCCMsg_Redirect_AppServerListConf_to_str(const SIPCCMsg_Redirect_AppServerListConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_TransferCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_TransferCall: "
        << "presideAgent=" << msg.GetPresideAgent() << ","
        << "theParty=" << msg.GetTheParty() << ","
        << "thePartyType=" << call_party_type_to_str(msg.thePartyType) << ","
        << "otherParty=" << msg.GetOtherParty() << ","
        << "otherPartyType=" << call_party_type_to_str(msg.otherPartyType) << ","
        << "agentUUIData=" << msg.agentUUIData << ","
        << "needReturn=" << (msg.needReturn == 1 ? "true" : "false") << ","
        << "IsWebPSTNAgent=" << (msg.IsWebPSTNAgent == 1 ? "true" : "false") << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string SIPCCMsg_UState_TransferCall_to_str(const SIPCCMsg_UState_TransferCall &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_TrunkSeizeRate &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_TrunkSeizeRate: "
        << "ComputerSymbol=" << msg.ComputerSymbol << ","
        << "GwName=" << msg.GwName << ","
        << "LineCount=" << msg.LineCount << ","
        << "MaxSeizeCount=" << msg.MaxSeizeCount << ","
        << "StartTime=" << msg.StartTime << ","
        << "EndTime=" << msg.EndTime;
    return ss.str();
}

string SIPCCMsg_Gateway_TrunkSeizeRate_to_str(const SIPCCMsg_Gateway_TrunkSeizeRate &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_TrunkStat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_TrunkStat: "
        << "ComputerSymbol=" << msg.ComputerSymbol << ","
        << "GwName=" << msg.GwName << ","
        << "GroupId=" << msg.GroupId << ","
        << "StartTime=" << msg.StartTime << ","
        << "EndTime=" << msg.EndTime << ","
        << "Stat=" << msg.Stat;
    return ss.str();
}

string SIPCCMsg_Gateway_TrunkStat_to_str(const SIPCCMsg_Gateway_TrunkStat &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_CallInStat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_CallInStat: "
        << "ComputerSymbol=" << msg.ComputerSymbol << ","
        << "GwName=" << msg.GwName << ","
        << "CalleeNum=" << msg.CalleeNum << ","
        << "Times=" << msg.Times << ","
        << "TotalSpan=" << msg.TotalSpan << ","
        << "StartTime=" << msg.StartTime << ","
        << "EndTime=" << msg.EndTime << ","
        << "HookTimes=" << msg.HookTimes << ","
        << "MaxSeizeCount=" << msg.MaxSeizeCount;
    return ss.str();
}

string SIPCCMsg_Gateway_CallInStat_to_str(const SIPCCMsg_Gateway_CallInStat &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_CallOutStat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_CallOutStat: "
        << "ComputerSymbol=" << msg.ComputerSymbol << ","
        << "GwName=" << msg.GwName << ","
        << "ManagerId=" << msg.ManagerId << ","
        << "Times=" << msg.Times << ","
        << "TotalSpan=" << msg.TotalSpan << ","
        << "StartTime=" << msg.StartTime << ","
        << "EndTime=" << msg.EndTime << ","
        << "HookTimes=" << msg.HookTimes << ","
        << "MaxSeizeCount=" << msg.MaxSeizeCount;
    return ss.str();
}

string SIPCCMsg_Gateway_CallOutStat_to_str(const SIPCCMsg_Gateway_CallOutStat &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_CallInOutStat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_CallInOutStat: "
        << "ComputerSymbol=" << msg.ComputerSymbol << ","
        << "GwName=" << msg.GwName << ","
        << "CalleeNum=" << msg.CalleeNum << ","
        << "Times=" << msg.Times << ","
        << "TotalSpan=" << msg.TotalSpan << ","
        << "StartTime=" << msg.StartTime << ","
        << "EndTime=" << msg.EndTime << ","
        << "HookTimes=" << msg.HookTimes << ","
        << "MaxSeizeCount=" << msg.MaxSeizeCount;
    return ss.str();
}

string SIPCCMsg_Gateway_CallInOutStat_to_str(const SIPCCMsg_Gateway_CallInOutStat &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_GetHeartBeat &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_GetHeartBeat: "
        << "InvokeId     =" << msg.InvokeId << ","
        << "HBServerIP   =" << format_ip_addr(msg.HeartBeatServerIP) << ","
        << "HBPort       =" << ntohs(msg.HeartBeatServerPort) << ","
        << "HBSBackupIP  =" << format_ip_addr(msg.HeartBeatServerBackupIP) << ","
        << "HBSBackupPort=" << ntohs(msg.HeartBeatServerBackupPort) << ","
        << "ServerID     =" << msg.ServerID << ","
        << "ServerType   =" << ESeverType_to_str(msg.ServerType) << ","
        << "GatewayName  =" << msg.GatewayName;
    return ss.str();
}

string SIPCCMsg_Gateway_GetHeartBeat_to_str(const SIPCCMsg_Gateway_GetHeartBeat &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OutDial_EndProcess &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OutDial_EndProcess: "
        << "TaskId=" << msg.TaskId << ","
        << "PhoneId=" << msg.PhoneId << ","
        << "DialSeq=" << msg.DialSeq << ","
        << "AgentName=" << msg.AgentName;
    return ss.str();
}

string SIPCCMsg_OutDial_EndProcess_to_str(const SIPCCMsg_OutDial_EndProcess &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Client_MessageCallInfo &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_Client_MessageCallInfo: "
		<< "callType=" << ECallType_to_str(msg.callType) << ","
		<< "callID=" << msg.callID << ","
		<< "phoneNumber=" << msg.phoneNumber << ","
		<< "callerName=" << msg.callerName << ","
		<< "agentname=" << msg.agentname << ","
		<< "startTime=" << msg.startTime << ","
		<< "endTime=" << msg.endTime << ","
		<< "managerID=" << msg.managerID << ","
		<< "deptName=" << msg.deptName << ","
		<< "callerType=" << call_party_type_to_str(msg.callerType) << ","
		<< "calledKeyword=" << msg.calledKeyword << ","
		<< "skillGroupId=" << msg.skillGroupId << ","
		<< "skillGroupName=" << msg.skillGroupName << ","
		<< "established=" << msg.established;
    return ss.str();
}

string SIPCCMsg_Client_MessageCallInfo_to_str(const SIPCCMsg_Client_MessageCallInfo &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_RingAgentExt &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_RingAgentExt: "
        << "agentID=" << msg.GetAgentID() << ","
        << "uuiData=" << msg.GetUUIData() << ","
        << "callID=" << msg.GetCallID() << ","
        << "corporationName=" << msg.GetCorporationName() << ","
        << "callerNoForSend=" << msg.callerNoForSend << ","
        << "callerPartyType=" << call_party_type_to_str(msg.callerPartyType) << ","
        << "ipPhoneNo_User=" << msg.ipPhoneNo_User << ","
        << "IsOutCall=" << (int)msg.IsOutCall << ","
        << "managerId=" << msg.managerId << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string SIPCCMsg_UState_RingAgentExt_to_str(const SIPCCMsg_UState_RingAgentExt &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_RingAgentExt2 &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_RingAgentExt2: "
        << "agentID=" << msg.GetAgentID() << ","
        << "uuiData=" << msg.GetUUIData() << ","
        << "callID=" << msg.GetCallID() << ","
        << "corporationName=" << msg.GetCorporationName() << ","
        << "callerNoForSend=" << msg.callerNoForSend << ","
        << "callerPartyType=" << call_party_type_to_str(msg.callerPartyType) << ","
        << "invokeID=" << msg.invokeID << ","
        << "usableTime=" << msg.usableTime << ","
        << "userName=" << msg.userName << ","
        << "lanIp=" << format_ip_addr(msg.lanIp) << ","
        << "wanIp=" << format_ip_addr(msg.wanIp) << ","
        << "outDialType=" << msg.outDialType << ","
        << "callTransID=" << msg.callTransID << ","
        << "managerId=" << msg.managerId << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string SIPCCMsg_UState_RingAgentExt2_to_str(const SIPCCMsg_UState_RingAgentExt2 &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_SetTrunkState &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_SetTrunkState: "
        << "gatewayName=" << msg.GetGatewayName() << ","
        << "computerSymbol=" << msg.GetComputerSymbol() << ","
        << "corporationName=" << msg.GetCorporationName() << ","
        << "bIsCallin=" << (int)msg.bIsCallin << ","
        << "bIdle=" << (int)msg.bIdle;
    return ss.str();
}

string SIPCCMsg_Gateway_SetTrunkState_to_str(const SIPCCMsg_Gateway_SetTrunkState &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_CallAnswered &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallAnswered: "
        << "userName=" << msg.GetUserName() << ","
        << "callId=" << msg.callId << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_ACD_CallAnswered_to_str(const SIPCCMsg_ACD_CallAnswered &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_CallAnswered &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallAnswered: "
        << "userName=" << msg.GetUserName() << ","
        << "callId=" << msg.callId << ","
        << "managerId=" << msg.managerId << ","
        << "corpName=" << msg.corpName << ","
        << "agentId=" << msg.agentId << ","
        << "extPhoneNo=" << msg.extPhoneNo << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol << ","
        << "RecordMode=" << msg.RecordMode;
    return ss.str();
}

string SIPCCMsg_UState_CallAnswered_to_str(const SIPCCMsg_UState_CallAnswered &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_CallArriveToAgent &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_CallArriveToAgent: "
        << "agentID=" << msg.GetAgentID() << ","
        << "userName=" << msg.GetUserName() << ","
        << "lanIp=" << format_ip_addr(msg.lanIp) << ","
        << "wanIp=" << format_ip_addr(msg.wanIp) << ","
        << "nickName=" << msg.nickName << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol << ","
        << "asd=" << msg.audioStreamDescription.to_str();
    return ss.str();
}

string SIPCCMsg_UState_CallArriveToAgent_to_str(const SIPCCMsg_UState_CallArriveToAgent &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Client_AgentSwitchToFrontDesk &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Client_AgentSwitchToFrontDesk: "
        << "invokeID=" << msg.invokeID << ","
        << "agentID=" << msg.agentID;

    return ss.str();
}

string SIPCCMsg_Client_AgentSwitchToFrontDesk_to_str(const SIPCCMsg_Client_AgentSwitchToFrontDesk &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_AgentSwitchToFrontDeskConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_AgentSwitchToFrontDeskConf: "
        << "invokeID=" << msg.invokeID << ","
        << "agentID=" << msg.agentID << ","
        << "count=" << msg.count << ","
        << "seq=" << msg.seq << ","
        << "callIdList=" << msg.callIdList;

    return ss.str();
}

string SIPCCMsg_ACD_AgentSwitchToFrontDeskConf_to_str(const SIPCCMsg_ACD_AgentSwitchToFrontDeskConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ServerProxy_LoadServerInfo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ServerProxy_LoadServerInfo: "
        << "InvokeID=" << msg.InvokeID << ","
        << "managerid=" << msg.managerid << ","
        << "acdserverid=" << msg.acdserverid << ","
        << "indexnum=" << msg.indexnum;
    return ss.str();
}

string SIPCCMsg_ServerProxy_LoadServerInfo_to_str(const SIPCCMsg_ServerProxy_LoadServerInfo &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_UniversalFailure &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_UniversalFailure: "
        << "errorCode=" << msg.errorCode;
    return ss.str();
}

string SIPCCMsg_ACD_UniversalFailure_to_str(const SIPCCMsg_ACD_UniversalFailure &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_ConferenceEnd &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_ConferenceEnd: "
        << "firstName=" << msg.firstName << ","
        << "firstLineNo=" << msg.firstLineNo << ","
        << "thirdName=" << msg.thirdName << ","
        << "conferenceType=" << msg.conferenceType << ","
        << "managerid=" << msg.managerid << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string SIPCCMsg_UState_ConferenceEnd_to_str(const SIPCCMsg_UState_ConferenceEnd &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_ConferenceStart &msg)
{
    stringstream ss;
	ss << "SIPCCMsg_UState_ConferenceStart: "
		<< "firstName=" << msg.firstName << ","
		<< "firstLineNo=" << msg.firstLineNo << ","
		<< "thirdName=" << msg.thirdName << ","
		<< "conferenceType=" << msg.conferenceType << ","
		<< "managerid=" << msg.managerid << ","
		<< "gatewayName=" << msg.gatewayName << ","
		<< "computerSymbol=" << msg.computerSymbol << ","
		<< "audioStreamDescription=" << msg.audioStreamDescription.to_str();
    return ss.str();
}

string SIPCCMsg_UState_ConferenceStart_to_str(const SIPCCMsg_UState_ConferenceStart &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_ConferenceStartConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_ConferenceStartConf: "
        << "firstName=" << msg.firstName << ","
        << "firstLineNo=" << msg.firstLineNo << ","
        << "thirdName=" << msg.thirdName << ","
        << "conferenceType=" << msg.conferenceType << ","
        << "managerid=" << msg.managerid << ","
        << "bSucc=" << (int)msg.bSucc << ","
        << "firstConferencePort=" << ntohs(msg.firstConferencePort) << ","
        << "secondConferencePort=" << ntohs(msg.secondConferencePort) << ","
        << "firstPartyLineNo=" << msg.firstPartyLineNo << ","
        << "secondPartyLineNo=" << msg.secondPartyLineNo << ","
        << "audioStreamDescription=" << msg.audioStreamDescription.to_str();
    return ss.str();
}

string SIPCCMsg_Gateway_ConferenceStartConf_to_str(const SIPCCMsg_Gateway_ConferenceStartConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OutDial_DialResult &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OutDial_DialResult: "
        << "TaskId=" << msg.TaskId << ","
        << "PhoneId=" << msg.PhoneId << ","
        << "DialSeq=" << msg.DialSeq << ","
        << "IsSucc=" << (msg.IsSucc>0) << ","
        << "ResultCode=" << msg.ResultCode << ","
        << "InvokeId=" << msg.InvokeId;
    return ss.str();
}

string SIPCCMsg_OutDial_DialResult_to_str(const SIPCCMsg_OutDial_DialResult &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OutDial_TransResult &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OutDial_TransResult: "
        << "TaskId=" << msg.TaskId << ","
        << "PhoneId=" << msg.PhoneId << ","
        << "DialSeq=" << msg.DialSeq << ","
        << "IsSucc=" << (msg.IsSucc>0) << ","
        << "AgentName=" << msg.AgentName;
    return ss.str();
}

string SIPCCMsg_OutDial_TransResult_to_str(const SIPCCMsg_OutDial_TransResult &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_OutDial_EndTalk &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_OutDial_EndTalk: "
        << "TaskId=" << msg.TaskId << ","
        << "PhoneId=" << msg.PhoneId << ","
        << "DialSeq=" << msg.DialSeq << ","
        << "AgentName=" << msg.AgentName;
    return ss.str();
}

string SIPCCMsg_OutDial_EndTalk_to_str(const SIPCCMsg_OutDial_EndTalk &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Toll_TollTransation &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Toll_TollTransation: "
        << "transation=" << msg.transation;
    return ss.str();
}

string SIPCCMsg_Toll_TollTransation_to_str(const SIPCCMsg_Toll_TollTransation &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_CallReleased_WpAgent &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallReleased_WpAgent: "
        << "userLineNo=" << msg.userLineNo << ","
        << "agentLineNo=" << msg.agentLineNo << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_ACD_CallReleased_WpAgent_to_str(const SIPCCMsg_ACD_CallReleased_WpAgent &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_StopRingAgentExt &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_StopRingAgentExt: "
        << "ipNo_AgentExt=" << msg.ipNo_AgentExt << ","
        << "agentID=" << msg.agentID << ","
        << "userName=" << msg.userName << ","
        << "handleId=" << msg.handleId << ","
        << "primarySourceIp=" << format_ip_addr(msg.primarySourceIp) << ","
        << "primarySourcePort=" << ntohs(msg.primarySourcePort);
    return ss.str();
}

string SIPCCMsg_ACD_StopRingAgentExt_to_str(const SIPCCMsg_ACD_StopRingAgentExt &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_AgentHangupCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_AgentHangupCall: "
        << "ipNo_AgentExt=" << msg.ipNo_AgentExt << ","
        << "agentID=" << msg.agentID << ","
        << "outDialType=" << outdial_type_to_str(msg.outDialType) << ","
        << "connectPhoneNo=" << msg.connectPhoneNo << ","
        << "managerId=" << msg.managerId << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_Gateway_AgentHangupCall_to_str(const SIPCCMsg_Gateway_AgentHangupCall &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_UserHangupCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_UserHangupCall: "
        << "userLineNo=" << msg.userLineNo << ","
        << "agentID=" << msg.agentID << ","
        << "connectPhoneNo=" << msg.connectPhoneNo << ","
        << "outDialType=" << outdial_type_to_str(msg.outDialType) << ","
        << "managerId=" << msg.managerId << ","
        << "handleId=" << msg.handleId;
    return ss.str();
}

string SIPCCMsg_Gateway_UserHangupCall_to_str(const SIPCCMsg_Gateway_UserHangupCall &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_UState_AgentHoldCall &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_UState_AgentHoldCall: "
        << "heldParty=" << msg.GetHeldParty() << ","
        << "ManagerID=" << msg.ManagerID << ","
        << "gatewayName=" << msg.gatewayName << ","
        << "computerSymbol=" << msg.computerSymbol;
    return ss.str();
}

string SIPCCMsg_UState_AgentHoldCall_to_str(const SIPCCMsg_UState_AgentHoldCall &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_CallAgentExtFailed &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_CallAgentExtFailed: "
        << "agentID=" << msg.agentID << ","
        << "managerId=" << msg.managerId;
    return ss.str();
}

string SIPCCMsg_Gateway_CallAgentExtFailed_to_str(const SIPCCMsg_Gateway_CallAgentExtFailed &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_CallUserFailed_WpAgent &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_CallUserFailed_WpAgent: "
        << "agentID=" << msg.agentID << ","
        << "managerId=" << msg.managerId;
    return ss.str();
}

string SIPCCMsg_Gateway_CallUserFailed_WpAgent_to_str(const SIPCCMsg_Gateway_CallUserFailed_WpAgent &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_WpAgent_LineNo &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_WpAgent_LineNo: "
        << "agentID" << msg.agentID << ","
        << "gwAgentLine" << msg.gwAgentLine << ","
        << "gwUserLine" << msg.gwUserLine << ","
        << "managerId" << msg.managerId << ","
        << "connectPhoneNo" << msg.connectPhoneNo;

    return ss.str();
}

string SIPCCMsg_Gateway_WpAgent_LineNo_to_str(const SIPCCMsg_Gateway_WpAgent_LineNo &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_Gateway_RingAgentExtConf &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_Gateway_RingAgentExtConf: "
        << "failedCause=" << msg.failedCause << ","
        << "bCallFailed=" << (msg.bCallFailed ? "true" : "false") << ","
        << "invokeID=" << msg.invokeID << ","
        << "outDialType=" << outdial_type_to_str(msg.outDialType) << ","
        << "ipNo_AgentExt=" << msg.ipNo_AgentExt << ","
        << "agentID=" << msg.agentID << ","
        << "calledNo=" << msg.calledNo << ","
        << "CalledAgentID=" << msg.CalledAgentID << ","
        << "calledAgentLineNo=" << msg.calledAgentLineNo << ","
        << "calledAgentLineIp=" << format_ip_addr(msg.calledAgentLineIp) << ","
        << "calledAgentLinePort=" << ntohs(msg.calledAgentLinePort) << ","
        << "managerId=" << msg.managerId << ","
        << "connectPhoneNo=" << msg.connectPhoneNo << ","
        << "primarySourceIp=" << format_ip_addr(msg.primarySourceIp) << ","
        << "primarySourcePort=" << ntohs(msg.primarySourcePort);

    return ss.str();
}

string SIPCCMsg_Gateway_RingAgentExtConf_to_str(const SIPCCMsg_Gateway_RingAgentExtConf &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_CallEnterVDN &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallEnterVDN: "
        << "corpName="       << msg.corpName       << ","
        << "vdnNo="          << msg.vdnNo          << ","
        << "vdnName="        << msg.vdnName        << ","
        << "callID="         << msg.callID         << ","
        << "callerID="       << msg.callerID       << ","
        << "calleeID="       << msg.calleeID       << ","
        << "callSrc="        << msg.callSrc        << ","
        << "extDevID="       << msg.extDevID       << ","
        << "enterTime="      << msg.enterTime      << ","
        << "status="         << msg.status         << ","
        << "callType="       << msg.callType       << ","
        << "bIsOverflow="    << (int)msg.bIsOverflow    << ","
        << "managerid="      << msg.managerid      << ","
        << "deptid="         << msg.deptid         << ","
        << "CurQueueNum="    << msg.CurQueueNum    << ","
        << "bInnerCall="     << (int)msg.bInnerCall     << ","
        << "bIntoQueue="     << (int)msg.bIntoQueue     << ","
        << "SkillGroupID="   << msg.SkillGroupID   << ","
        << "SkillGroupName=" << msg.SkillGroupName << ","
        << "UserLevel="      << msg.UserLevel      << ","
        << "sData="          << msg.sData;
    return ss.str();
}

string SIPCCMsg_ACD_CallEnterVDN_to_str(const SIPCCMsg_ACD_CallEnterVDN &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_ACD_CallLeaveVDN &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_ACD_CallLeaveVDN: "
        << "vdnNo=" << msg.vdnNo << ","
        << "callID=" << msg.callID << ","
        << "leaveTime=" << msg.leaveTime << ","
        << "leaveCause=" << msg.leaveCause << ","
        << "corpName=" << msg.corpName << ","
        << "managerid=" << msg.managerid << ","
        << "deptid=" << msg.deptid << ","
        << "LeaveQueueTime=" << msg.LeaveQueueTime << ","
        << "agentID=" << msg.agentID << ","
        << "bIntoQueue=" << (int)msg.bIntoQueue << ","
        << "CurQueueNum=" << msg.CurQueueNum << ","
        << "SkillGroupID=" << msg.SkillGroupID << ","
        << "SkillGroupName=" << msg.SkillGroupName << ","
        << "UserLevel=" << msg.UserLevel << ","
        << "sData=" << msg.sData;
    return ss.str();
}

string SIPCCMsg_ACD_CallLeaveVDN_to_str(const SIPCCMsg_ACD_CallLeaveVDN &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_SysManage_AddDeptToService &msg)
{
    typedef SIPCCMsg_SysManage_AddDeptToService::DepartToServiceMap::const_iterator const_iterator;
    stringstream ss;
    ss << "SIPCCMsg_SysManage_AddDeptToService: "
        << "invokeid=" << msg.invokeid << ","
        << "sequence=" << msg.sequence;
    for (const_iterator it = msg.departToServiceVect.begin(); 
        it != msg.departToServiceVect.end(); it++) {
        ss << endl << "managerid=" << it->first.first << ","
            << "departmentid=" << it->first.second;
        for (size_t i = 0; i < it->second.size(); i++) {
            ss << ", acdip=" << format_ip_addr(it->second[i].ip) << ","
                << "acdport=" << ntohs(it->second[i].port);
        }
    }
    return ss.str();
}

string SIPCCMsg_SysManage_AddDeptToService_to_str(const SIPCCMsg_SysManage_AddDeptToService &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_SysManage_AddKey &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_SysManage_AddKey: "
        "managerID="     << msg.managerID << ","
        "departmentID="  << msg.departmentID << ","
        "keyWord="       << msg.GetKeyWord() << ","
        "theDepartment=" << msg.GetTheDepartment() << ","
        "SkillGroupID="  << msg.SkillGroupID << ","
        "InvokeID=" << msg.InvokeID;
    return ss.str();
}

string SIPCCMsg_SysManage_AddKey_to_str(const SIPCCMsg_SysManage_AddKey &msg)
{
    return to_str(msg);
}

string to_str(const SIPCCMsg_MakeCallViaClient &msg)
{
    stringstream ss;
    ss << "SIPCCMsg_MakeCallViaClient: "
        << "calllingDevice=" << msg.calllingDevice << ","
        << "calledDevice=" << msg.calledDevice << ","
        << "managerID=" << msg.managerID;
    return ss.str();
}

string SIPCCMsg_MakeCallViaClient_to_str(const SIPCCMsg_MakeCallViaClient &msg)
{
    return to_str(msg);
}

class CMsgDumpers
{
public:
    CMsgDumpers()
    {
        m_msg2logfunc.insert(make_pair(Type_MakeCallViaClient, reinterpret_cast<void*>(SIPCCMsg_MakeCallViaClient_to_str)));
        m_msg2logfunc.insert(make_pair(Type_SysManage_AddKey, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddKey_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallLeaveVDN, reinterpret_cast<void*>(SIPCCMsg_ACD_CallLeaveVDN_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallEnterVDN, reinterpret_cast<void*>(SIPCCMsg_ACD_CallEnterVDN_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_RingAgentExtConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_RingAgentExtConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_WpAgent_LineNo, reinterpret_cast<void*>(SIPCCMsg_Gateway_WpAgent_LineNo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_CallUserFailed_WpAgent, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallUserFailed_WpAgent_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_CallAgentExtFailed, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallAgentExtFailed_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_AgentHoldCall, reinterpret_cast<void*>(SIPCCMsg_UState_AgentHoldCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_UserHangupCall, reinterpret_cast<void*>(SIPCCMsg_Gateway_UserHangupCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_AgentHangupCall, reinterpret_cast<void*>(SIPCCMsg_Gateway_AgentHangupCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_StopRingAgentExt, reinterpret_cast<void*>(SIPCCMsg_ACD_StopRingAgentExt_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallReleased_WpAgent, reinterpret_cast<void*>(SIPCCMsg_ACD_CallReleased_WpAgent_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Toll_TollTransation, reinterpret_cast<void*>(SIPCCMsg_Toll_TollTransation_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OutDial_EndTalk, reinterpret_cast<void*>(SIPCCMsg_OutDial_EndTalk_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OutDial_TransResult, reinterpret_cast<void*>(SIPCCMsg_OutDial_TransResult_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OutDial_DialResult, reinterpret_cast<void*>(SIPCCMsg_OutDial_DialResult_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_ConferenceStartConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_ConferenceStartConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_ConferenceStart, reinterpret_cast<void*>(SIPCCMsg_UState_ConferenceStart_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_ConferenceEnd, reinterpret_cast<void*>(SIPCCMsg_UState_ConferenceEnd_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_UniversalFailure, reinterpret_cast<void*>(SIPCCMsg_ACD_UniversalFailure_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ServerProxy_LoadServerInfo, reinterpret_cast<void*>(SIPCCMsg_ServerProxy_LoadServerInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallArriveToAgent, reinterpret_cast<void*>(SIPCCMsg_UState_CallArriveToAgent_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_UState_CallAnswered_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_ACD_CallAnswered_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_SetTrunkState, reinterpret_cast<void*>(SIPCCMsg_Gateway_SetTrunkState_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_RingAgentExt2, reinterpret_cast<void*>(SIPCCMsg_UState_RingAgentExt2_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_RingAgentExt, reinterpret_cast<void*>(SIPCCMsg_UState_RingAgentExt_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OutDial_EndProcess, reinterpret_cast<void*>(SIPCCMsg_OutDial_EndProcess_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_GetHeartBeat, reinterpret_cast<void*>(SIPCCMsg_Gateway_GetHeartBeat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_CallInOutStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallInOutStat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_CallOutStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallOutStat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_CallInStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallInStat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_TrunkStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_TrunkStat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_TrunkSeizeRate, reinterpret_cast<void*>(SIPCCMsg_Gateway_TrunkSeizeRate_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_TransferCall, reinterpret_cast<void*>(SIPCCMsg_UState_TransferCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_SysManage_AddDepartment, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddDepartment_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserMakeCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserMakeCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_UserMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_ACD_UserMakeCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallHanguped, reinterpret_cast<void*>(SIPCCMsg_ACD_CallHanguped_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Guide_ACDInfo, reinterpret_cast<void*>(SIPCCMsg_Guide_ACDInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserQueryCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserQueryCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_CallAgentFailed, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallAgentFailed_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServiceUnavailable, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceUnavailable_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserLogoutStateSvr, reinterpret_cast<void*>(SIPCCMsg_Client_UserLogoutStateSvr_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentLogout, reinterpret_cast<void*>(SIPCCMsg_Client_AgentLogout_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallingQueueInfo, reinterpret_cast<void*>(SIPCCMsg_ACD_CallingQueueInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_GetCallingQueueInfo, reinterpret_cast<void*>(SIPCCMsg_Client_GetCallingQueueInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_SetClientAgentState, reinterpret_cast<void*>(SIPCCMsg_ACD_SetClientAgentState_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_TransferCall, reinterpret_cast<void*>(SIPCCMsg_ACD_TransferCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_TransReady, reinterpret_cast<void*>(SIPCCMsg_Client_TransReady_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_PrepareTransfer, reinterpret_cast<void*>(SIPCCMsg_ACD_PrepareTransfer_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_PrepareTransfer, reinterpret_cast<void*>(SIPCCMsg_Client_PrepareTransfer_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_HangupCall, reinterpret_cast<void*>(SIPCCMsg_CClient_HangupCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_CMakeCall, reinterpret_cast<void*>(SIPCCMsg_CClient_CMakeCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Login_CCLoginConf, reinterpret_cast<void*>(SIPCCMsg_Login_CCLoginConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_Login, reinterpret_cast<void*>(SIPCCMsg_CClient_Login_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Redirect_GetService, reinterpret_cast<void*>(SIPCCMsg_Redirect_GetService_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Redirect_GetServiceConf, reinterpret_cast<void*>(SIPCCMsg_Redirect_GetServiceConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Redirect_SearchPlatform, reinterpret_cast<void*>(SIPCCMsg_Redirect_SearchPlatform_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Redirect_SearchPlatformConf, reinterpret_cast<void*>(SIPCCMsg_Redirect_SearchPlatformConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserLogin, reinterpret_cast<void*>(SIPCCMsg_Client_UserLogin_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_NotifyAgentLogin, reinterpret_cast<void*>(SIPCCMsg_Client_NotifyAgentLogin_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_GetParkReasons, reinterpret_cast<void*>(SIPCCMsg_Client_GetParkReasons_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_QueryIncrementFunc, reinterpret_cast<void*>(SIPCCMsg_Client_QueryIncrementFunc_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentLogin, reinterpret_cast<void*>(SIPCCMsg_Client_AgentLogin_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_QueryGWLanInfo, reinterpret_cast<void*>(SIPCCMsg_Client_QueryGWLanInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_QueryGWLanInfoConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_QueryGWLanInfoConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentQueryState, reinterpret_cast<void*>(SIPCCMsg_Client_AgentQueryState_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_AgentQueryStateConf, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentQueryStateConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Login_UserLoginSucc, reinterpret_cast<void*>(SIPCCMsg_Login_UserLoginSucc_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Login_UserLoginFailed, reinterpret_cast<void*>(SIPCCMsg_Login_UserLoginFailed_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Login_NotifyAgentLoginConf, reinterpret_cast<void*>(SIPCCMsg_Login_NotifyAgentLoginConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Login_GetParkReasonsConf, reinterpret_cast<void*>(SIPCCMsg_Login_GetParkReasonsConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Login_QueryIncrementFuncConf, reinterpret_cast<void*>(SIPCCMsg_Login_QueryIncrementFuncConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GetService, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetService_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GetServiceConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetServiceConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GetHeartBeat, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetHeartBeat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GetHeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetHeartBeatConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_AgentLoginSucc, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentLoginSucc_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_AgentLoginFailed, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentLoginFailed_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_ForceLogout, reinterpret_cast<void*>(SIPCCMsg_ACD_ForceLogout_to_str)));
        m_msg2logfunc.insert(make_pair(Type_HeartBeat, reinterpret_cast<void*>(SIPCCMsg_HeartBeat_to_str)));
        m_msg2logfunc.insert(make_pair(Type_HeartBeatResponse, reinterpret_cast<void*>(SIPCCMsg_HeartBeatResponse_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_QueryGW, reinterpret_cast<void*>(SIPCCMsg_Client_QueryGW_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_QueryGWConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_QueryGWConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentMakeCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentMakeCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_AgentMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_UState_AgentMakeCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallUserConf, reinterpret_cast<void*>(SIPCCMsg_ACD_CallUserConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_DataTrans_StartTrans, reinterpret_cast<void*>(SIPCCMsg_DataTrans_StartTrans_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentMakeCall2, reinterpret_cast<void*>(SIPCCMsg_Client_AgentMakeCall2_to_str)));
        m_msg2logfunc.insert(make_pair(Type_DataTrans_StartTransConf, reinterpret_cast<void*>(SIPCCMsg_DataTrans_StartTransConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallEstablished, reinterpret_cast<void*>(SIPCCMsg_ACD_CallEstablished_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallEstablished_Real, reinterpret_cast<void*>(SIPCCMsg_ACD_CallEstablished_Real_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_RecInfo2Client, reinterpret_cast<void*>(SIPCCMsg_ACD_RecInfo2Client_to_str)));
        m_msg2logfunc.insert(make_pair(Type_AreaCode_AreaQuery, reinterpret_cast<void*>(SIPCCMsg_AreaQuery_to_str)));
        m_msg2logfunc.insert(make_pair(Type_AreaCode_AreaAnswer, reinterpret_cast<void*>(SIPCCMsg_AreaAnswer_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentHangupCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentHangupCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallReleased, reinterpret_cast<void*>(SIPCCMsg_UState_CallReleased_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallReleased, reinterpret_cast<void*>(SIPCCMsg_ACD_CallReleased_to_str)));
        m_msg2logfunc.insert(make_pair(Type_DataTrans_StopTrans, reinterpret_cast<void*>(SIPCCMsg_DataTrans_StopTrans_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentHoldCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentHoldCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_AgentHoldCallConf, reinterpret_cast<void*>(SIPCCMsg_UState_AgentHoldCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_BeforeTransfer, reinterpret_cast<void*>(SIPCCMsg_Client_BeforeTransfer_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_TransferCall, reinterpret_cast<void*>(SIPCCMsg_Client_TransferCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallArriveToAgent, reinterpret_cast<void*>(SIPCCMsg_ACD_CallArriveToAgent_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Record_AgentRecordQueryPort, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordQueryPort_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Record_AgentRecordQueryPortConf, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordQueryPortConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Record_AgentRecordStart, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordStart_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Record_AgentRecordStop, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordStop_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_MediaRequest, reinterpret_cast<void*>(SIPCCMsg_Client_MediaRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Trans_TransmitRequest, reinterpret_cast<void*>(SIPCCMsg_Trans_TransmitRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Trans_TransmitRequestConf, reinterpret_cast<void*>(SIPCCMsg_Trans_TransmitRequestConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_AgentMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentMakeCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentTestNet, reinterpret_cast<void*>(SIPCCMsg_Client_AgentTestNet_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentTestNetConf, reinterpret_cast<void*>(SIPCCMsg_Client_AgentTestNetConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_LanPortApprize, reinterpret_cast<void*>(SIPCCMsg_Client_LanPortApprize_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_GetDepartNAgentInfo, reinterpret_cast<void*>(SIPCCMsg_Client_GetDepartNAgentInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_ConferenceStart, reinterpret_cast<void*>(SIPCCMsg_Client_ConferenceStart_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_ConferenceEnd, reinterpret_cast<void*>(SIPCCMsg_Client_ConferenceEnd_to_str)));
        m_msg2logfunc.insert(make_pair(Type_PhoneSearch_ExtNoInfo, reinterpret_cast<void*>(SIPCCMsg_PhoneSearch_ExtNoInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_PhoneSearch_ExtNoInfoConf, reinterpret_cast<void*>(SIPCCMsg_PhoneSearch_ExtNoInfoConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_IsAgentLogin, reinterpret_cast<void*>(SIPCCMsg_Gateway_IsAgentLogin_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_IsAgentLoginConf, reinterpret_cast<void*>(SIPCCMsg_ACD_IsAgentLoginConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_DepartNAgentInfo, reinterpret_cast<void*>(SIPCCMsg_ACD_DepartNAgentInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_ConferenceStartConf, reinterpret_cast<void*>(SIPCCMsg_ACD_ConferenceStartConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_ConferenceStart, reinterpret_cast<void*>(SIPCCMsg_ACD_ConferenceStart_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_ConferenceEnd, reinterpret_cast<void*>(SIPCCMsg_ACD_ConferenceEnd_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_UserLeaveConference, reinterpret_cast<void*>(SIPCCMsg_ACD_UserLeaveConference_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_CallDelivered, reinterpret_cast<void*>(SIPCCMsg_ACD_CallDelivered_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentCallReceived, reinterpret_cast<void*>(SIPCCMsg_Client_AgentCallReceived_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentAnswerCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentAnswerCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallEstablished, reinterpret_cast<void*>(SIPCCMsg_UState_CallEstablished_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_MediaRequest, reinterpret_cast<void*>(SIPCCMsg_ACD_MediaRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_GetTalkingAgentConf, reinterpret_cast<void*>(SIPCCMsg_ACD_GetTalkingAgentConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_GetTalkingAgent, reinterpret_cast<void*>(SIPCCMsg_Client_GetTalkingAgent_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_MonitRequest, reinterpret_cast<void*>(SIPCCMsg_Client_MonitRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_MonitRequestConf, reinterpret_cast<void*>(SIPCCMsg_Client_MonitRequestConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_StopMonit, reinterpret_cast<void*>(SIPCCMsg_Client_StopMonit_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_GetTalkingAgentConf, reinterpret_cast<void*>(SIPCCMsg_ACD_GetTalkingAgentConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_UserLeaveConference, reinterpret_cast<void*>(SIPCCMsg_Gateway_UserLeaveConference_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_WebPSTNDial, reinterpret_cast<void*>(SIPCCMsg_Client_WebPSTNDial_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_WebPSTNDialConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_WebPSTNDialConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_TextChat, reinterpret_cast<void*>(SIPCCMsg_Client_TextChat_to_str)));
        //m_msg2logfunc.insert(make_pair(Type_Text_TextChat                   , reinterpret_cast<void*>(SIPCCMsg_Text_TextChat_to_str)));            
        m_msg2logfunc.insert(make_pair(Type_Client_LoginToText, reinterpret_cast<void*>(SIPCCMsg_Client_LoginToText_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_CMakeCall, reinterpret_cast<void*>(SIPCCMsg_CClient_CMakeCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_CallArrived, reinterpret_cast<void*>(SIPCCMsg_CClient_CallArrived_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_CClient_CallAnswered_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_HeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_HeartBeatConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CMakeCall, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CallDelivered, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallDelivered_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CallArrived, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallArrived_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallAnswered_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CallStart, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallStart_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_QueryGW, reinterpret_cast<void*>(SIPCCMsg_CProxy_QueryGW_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_CMakeCall2, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCall2_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_HangupCall, reinterpret_cast<void*>(SIPCCMsg_CProxy_HangupCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_HangupCallConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_HangupCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CClient_HangupCallConf, reinterpret_cast<void*>(SIPCCMsg_CClient_HangupCallConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_CallUserConf, reinterpret_cast<void*>(SIPCCMsg_Client_CallUserConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserAnswerCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserAnswerCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserAnswerCall_Real, reinterpret_cast<void*>(SIPCCMsg_Client_UserAnswerCall_Real_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserHangupCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserHangupCall_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_MediaRequest, reinterpret_cast<void*>(SIPCCMsg_Client_MediaRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_MediaRequestConf, reinterpret_cast<void*>(SIPCCMsg_Client_MediaRequestConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServiceRegisterConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceRegisterConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServiceConfirmConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceConfirmConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GetHeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetHeartBeatConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServiceConfirm, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceConfirm_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServerHeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServerHeartBeatConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServiceTimeout, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceTimeout_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GetServiceConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetServiceConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_SwitchService_Ctrl, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_SwitchService_Ctrl_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_UserLoginStateSvr, reinterpret_cast<void*>(SIPCCMsg_Client_UserLoginStateSvr_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_Login, reinterpret_cast<void*>(SIPCCMsg_Gateway_Login_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_LoginConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_LoginConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_ServiceRegister, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceRegister_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Trans_TransmitStopRequest, reinterpret_cast<void*>(SIPCCMsg_Trans_TransmitStopRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_FileTransfer_UploadRequest, reinterpret_cast<void*>(SIPCCMsg_FileTransfer_UploadRequest_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_UserLoginStateSvrConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_UserLoginStateSvrConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GWLogout, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GWLogout_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GWLogin, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GWLogin_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_UserLogin, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_UserLogin_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallReceived, reinterpret_cast<void*>(SIPCCMsg_UState_CallReceived_to_str)));
        m_msg2logfunc.insert(make_pair(Type_SysManage_AddGWTG, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddGWTG_to_str)));
        m_msg2logfunc.insert(make_pair(Type_SysManage_AddDeptTG, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddDeptTG_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_AgentStateInfo, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentStateInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_Login, reinterpret_cast<void*>(SIPCCMsg_CProxy_Login_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_LoginConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_LoginConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_Logout, reinterpret_cast<void*>(SIPCCMsg_CProxy_Logout_to_str)));
        m_msg2logfunc.insert(make_pair(Type_CProxy_LogoutConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_LogoutConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_SysManage_LoadServerInfoConf, reinterpret_cast<void*>(SIPCCMsg_SysManage_LoadServerInfoConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_OneKeySwitch_GWLoginConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GWLoginConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_GCTI_Meaage, reinterpret_cast<void*>(SIPCCMsg_GCTI_Message_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallReceived2, reinterpret_cast<void*>(SIPCCMsg_UState_CallReceived2_to_str)));
        m_msg2logfunc.insert(make_pair(Type_UState_CallHanguped, reinterpret_cast<void*>(SIPCCMsg_UState_CallHanguped_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Gateway_LoadGWCorpInfo, reinterpret_cast<void*>(SIPCCMsg_Gateway_LoadGWCorpInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Redirect_AppServerListConf, reinterpret_cast<void*>(SIPCCMsg_Redirect_AppServerListConf_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_MessageCallInfo, reinterpret_cast<void*>(SIPCCMsg_Client_MessageCallInfo_to_str)));
        m_msg2logfunc.insert(make_pair(Type_Client_AgentSwitchToFrontDesk, reinterpret_cast<void*>(SIPCCMsg_Client_AgentSwitchToFrontDesk_to_str)));
        m_msg2logfunc.insert(make_pair(Type_ACD_AgentSwitchToFrontDeskConf, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentSwitchToFrontDeskConf_to_str)));

    }
public:
    map<int, void*> m_msg2logfunc;
};

//////
//////struct{
//////    int first;
//////    void* second;
//////} msg_dumpers[] =
//////{
//////    { Type_MakeCallViaClient, reinterpret_cast<void*>(SIPCCMsg_MakeCallViaClient_to_str) },
//////    { Type_SysManage_AddKey, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddKey_to_str) },
//////    { Type_ACD_CallLeaveVDN, reinterpret_cast<void*>(SIPCCMsg_ACD_CallLeaveVDN_to_str) },
//////    { Type_ACD_CallEnterVDN, reinterpret_cast<void*>(SIPCCMsg_ACD_CallEnterVDN_to_str) },
//////    { Type_Gateway_RingAgentExtConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_RingAgentExtConf_to_str) },
//////    { Type_Gateway_WpAgent_LineNo, reinterpret_cast<void*>(SIPCCMsg_Gateway_WpAgent_LineNo_to_str) },
//////    { Type_Gateway_CallUserFailed_WpAgent, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallUserFailed_WpAgent_to_str) },
//////    { Type_Gateway_CallAgentExtFailed, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallAgentExtFailed_to_str) },
//////    { Type_UState_AgentHoldCall, reinterpret_cast<void*>(SIPCCMsg_UState_AgentHoldCall_to_str) },
//////    { Type_Gateway_UserHangupCall, reinterpret_cast<void*>(SIPCCMsg_Gateway_UserHangupCall_to_str) },
//////    { Type_Gateway_AgentHangupCall, reinterpret_cast<void*>(SIPCCMsg_Gateway_AgentHangupCall_to_str) },
//////    { Type_ACD_StopRingAgentExt, reinterpret_cast<void*>(SIPCCMsg_ACD_StopRingAgentExt_to_str) },
//////    { Type_ACD_CallReleased_WpAgent, reinterpret_cast<void*>(SIPCCMsg_ACD_CallReleased_WpAgent_to_str) },
//////    { Type_Toll_TollTransation, reinterpret_cast<void*>(SIPCCMsg_Toll_TollTransation_to_str) },
//////    { Type_OutDial_EndTalk, reinterpret_cast<void*>(SIPCCMsg_OutDial_EndTalk_to_str) },
//////    { Type_OutDial_TransResult, reinterpret_cast<void*>(SIPCCMsg_OutDial_TransResult_to_str) },
//////    { Type_OutDial_DialResult, reinterpret_cast<void*>(SIPCCMsg_OutDial_DialResult_to_str) },
//////    { Type_Gateway_ConferenceStartConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_ConferenceStartConf_to_str) },
//////    { Type_UState_ConferenceStart, reinterpret_cast<void*>(SIPCCMsg_UState_ConferenceStart_to_str) },
//////    { Type_UState_ConferenceEnd, reinterpret_cast<void*>(SIPCCMsg_UState_ConferenceEnd_to_str) },
//////    { Type_ACD_UniversalFailure, reinterpret_cast<void*>(SIPCCMsg_ACD_UniversalFailure_to_str) },
//////    { Type_ServerProxy_LoadServerInfo, reinterpret_cast<void*>(SIPCCMsg_ServerProxy_LoadServerInfo_to_str) },
//////    { Type_UState_CallArriveToAgent, reinterpret_cast<void*>(SIPCCMsg_UState_CallArriveToAgent_to_str) },
//////    { Type_UState_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_UState_CallAnswered_to_str) },
//////    { Type_ACD_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_ACD_CallAnswered_to_str) },
//////    { Type_Gateway_SetTrunkState, reinterpret_cast<void*>(SIPCCMsg_Gateway_SetTrunkState_to_str) },
//////    { Type_UState_RingAgentExt2, reinterpret_cast<void*>(SIPCCMsg_UState_RingAgentExt2_to_str) },
//////    { Type_UState_RingAgentExt, reinterpret_cast<void*>(SIPCCMsg_UState_RingAgentExt_to_str) },
//////    { Type_OutDial_EndProcess, reinterpret_cast<void*>(SIPCCMsg_OutDial_EndProcess_to_str) },
//////    { Type_Gateway_GetHeartBeat, reinterpret_cast<void*>(SIPCCMsg_Gateway_GetHeartBeat_to_str) },
//////    { Type_Gateway_CallInOutStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallInOutStat_to_str) },
//////    { Type_Gateway_CallOutStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallOutStat_to_str) },
//////    { Type_Gateway_CallInStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallInStat_to_str) },
//////    { Type_Gateway_TrunkStat, reinterpret_cast<void*>(SIPCCMsg_Gateway_TrunkStat_to_str) },
//////    { Type_Gateway_TrunkSeizeRate, reinterpret_cast<void*>(SIPCCMsg_Gateway_TrunkSeizeRate_to_str) },
//////    { Type_UState_TransferCall, reinterpret_cast<void*>(SIPCCMsg_UState_TransferCall_to_str) },
//////    { Type_SysManage_AddDepartment, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddDepartment_to_str) },
//////    { Type_Client_UserMakeCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserMakeCall_to_str) },
//////    { Type_ACD_UserMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_ACD_UserMakeCallConf_to_str) },
//////    { Type_ACD_CallHanguped, reinterpret_cast<void*>(SIPCCMsg_ACD_CallHanguped_to_str) },
//////    { Type_Guide_ACDInfo, reinterpret_cast<void*>(SIPCCMsg_Guide_ACDInfo_to_str) },
//////    { Type_Client_UserQueryCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserQueryCall_to_str) },
//////    { Type_Gateway_CallAgentFailed, reinterpret_cast<void*>(SIPCCMsg_Gateway_CallAgentFailed_to_str) },
//////    { Type_OneKeySwitch_ServiceUnavailable, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceUnavailable_to_str) },
//////    { Type_Client_UserLogoutStateSvr, reinterpret_cast<void*>(SIPCCMsg_Client_UserLogoutStateSvr_to_str) },
//////    { Type_Client_AgentLogout, reinterpret_cast<void*>(SIPCCMsg_Client_AgentLogout_to_str) },
//////    { Type_ACD_CallingQueueInfo, reinterpret_cast<void*>(SIPCCMsg_ACD_CallingQueueInfo_to_str) },
//////    { Type_Client_GetCallingQueueInfo, reinterpret_cast<void*>(SIPCCMsg_Client_GetCallingQueueInfo_to_str) },
//////    { Type_ACD_SetClientAgentState, reinterpret_cast<void*>(SIPCCMsg_ACD_SetClientAgentState_to_str) },
//////    { Type_ACD_TransferCall, reinterpret_cast<void*>(SIPCCMsg_ACD_TransferCall_to_str) },
//////    { Type_Client_TransReady, reinterpret_cast<void*>(SIPCCMsg_Client_TransReady_to_str) },
//////    { Type_ACD_PrepareTransfer, reinterpret_cast<void*>(SIPCCMsg_ACD_PrepareTransfer_to_str) },
//////    { Type_Client_PrepareTransfer, reinterpret_cast<void*>(SIPCCMsg_Client_PrepareTransfer_to_str) },
//////    { Type_CClient_HangupCall, reinterpret_cast<void*>(SIPCCMsg_CClient_HangupCall_to_str) },
//////    { Type_CProxy_CMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCallConf_to_str) },
//////    { Type_CClient_CMakeCall, reinterpret_cast<void*>(SIPCCMsg_CClient_CMakeCall_to_str) },
//////    { Type_Login_CCLoginConf, reinterpret_cast<void*>(SIPCCMsg_Login_CCLoginConf_to_str) },
//////    { Type_CClient_Login, reinterpret_cast<void*>(SIPCCMsg_CClient_Login_to_str) },
//////    { Type_Redirect_GetService, reinterpret_cast<void*>(SIPCCMsg_Redirect_GetService_to_str) },
//////    { Type_Redirect_GetServiceConf, reinterpret_cast<void*>(SIPCCMsg_Redirect_GetServiceConf_to_str) },
//////    { Type_Redirect_SearchPlatform, reinterpret_cast<void*>(SIPCCMsg_Redirect_SearchPlatform_to_str) },
//////    { Type_Redirect_SearchPlatformConf, reinterpret_cast<void*>(SIPCCMsg_Redirect_SearchPlatformConf_to_str) },
//////    { Type_Client_UserLogin, reinterpret_cast<void*>(SIPCCMsg_Client_UserLogin_to_str) },
//////    { Type_Client_NotifyAgentLogin, reinterpret_cast<void*>(SIPCCMsg_Client_NotifyAgentLogin_to_str) },
//////    { Type_Client_GetParkReasons, reinterpret_cast<void*>(SIPCCMsg_Client_GetParkReasons_to_str) },
//////    { Type_Client_QueryIncrementFunc, reinterpret_cast<void*>(SIPCCMsg_Client_QueryIncrementFunc_to_str) },
//////    { Type_Client_AgentLogin, reinterpret_cast<void*>(SIPCCMsg_Client_AgentLogin_to_str) },
//////    { Type_Client_QueryGWLanInfo, reinterpret_cast<void*>(SIPCCMsg_Client_QueryGWLanInfo_to_str) },
//////    { Type_Gateway_QueryGWLanInfoConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_QueryGWLanInfoConf_to_str) },
//////    { Type_Client_AgentQueryState, reinterpret_cast<void*>(SIPCCMsg_Client_AgentQueryState_to_str) },
//////    { Type_ACD_AgentQueryStateConf, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentQueryStateConf_to_str) },
//////    { Type_Login_UserLoginSucc, reinterpret_cast<void*>(SIPCCMsg_Login_UserLoginSucc_to_str) },
//////    { Type_Login_UserLoginFailed, reinterpret_cast<void*>(SIPCCMsg_Login_UserLoginFailed_to_str) },
//////    { Type_Login_NotifyAgentLoginConf, reinterpret_cast<void*>(SIPCCMsg_Login_NotifyAgentLoginConf_to_str) },
//////    { Type_Login_GetParkReasonsConf, reinterpret_cast<void*>(SIPCCMsg_Login_GetParkReasonsConf_to_str) },
//////    { Type_Login_QueryIncrementFuncConf, reinterpret_cast<void*>(SIPCCMsg_Login_QueryIncrementFuncConf_to_str) },
//////    { Type_OneKeySwitch_GetService, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetService_to_str) },
//////    { Type_OneKeySwitch_GetServiceConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetServiceConf_to_str) },
//////    { Type_OneKeySwitch_GetHeartBeat, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetHeartBeat_to_str) },
//////    { Type_OneKeySwitch_GetHeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetHeartBeatConf_to_str) },
//////    { Type_ACD_AgentLoginSucc, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentLoginSucc_to_str) },
//////    { Type_ACD_AgentLoginFailed, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentLoginFailed_to_str) },
//////    { Type_ACD_ForceLogout, reinterpret_cast<void*>(SIPCCMsg_ACD_ForceLogout_to_str) },
//////    { Type_HeartBeat, reinterpret_cast<void*>(SIPCCMsg_HeartBeat_to_str) },
//////    { Type_HeartBeatResponse, reinterpret_cast<void*>(SIPCCMsg_HeartBeatResponse_to_str) },
//////    { Type_Client_QueryGW, reinterpret_cast<void*>(SIPCCMsg_Client_QueryGW_to_str) },
//////    { Type_Gateway_QueryGWConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_QueryGWConf_to_str) },
//////    { Type_Client_AgentMakeCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentMakeCall_to_str) },
//////    { Type_UState_AgentMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_UState_AgentMakeCallConf_to_str) },
//////    { Type_ACD_CallUserConf, reinterpret_cast<void*>(SIPCCMsg_ACD_CallUserConf_to_str) },
//////    { Type_DataTrans_StartTrans, reinterpret_cast<void*>(SIPCCMsg_DataTrans_StartTrans_to_str) },
//////    { Type_Client_AgentMakeCall2, reinterpret_cast<void*>(SIPCCMsg_Client_AgentMakeCall2_to_str) },
//////    { Type_DataTrans_StartTransConf, reinterpret_cast<void*>(SIPCCMsg_DataTrans_StartTransConf_to_str) },
//////    { Type_ACD_CallEstablished, reinterpret_cast<void*>(SIPCCMsg_ACD_CallEstablished_to_str) },
//////    { Type_ACD_CallEstablished_Real, reinterpret_cast<void*>(SIPCCMsg_ACD_CallEstablished_Real_to_str) },
//////    { Type_ACD_RecInfo2Client, reinterpret_cast<void*>(SIPCCMsg_ACD_RecInfo2Client_to_str) },
//////    { Type_AreaCode_AreaQuery, reinterpret_cast<void*>(SIPCCMsg_AreaQuery_to_str) },
//////    { Type_AreaCode_AreaAnswer, reinterpret_cast<void*>(SIPCCMsg_AreaAnswer_to_str) },
//////    { Type_Client_AgentHangupCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentHangupCall_to_str) },
//////    { Type_UState_CallReleased, reinterpret_cast<void*>(SIPCCMsg_UState_CallReleased_to_str) },
//////    { Type_ACD_CallReleased, reinterpret_cast<void*>(SIPCCMsg_ACD_CallReleased_to_str) },
//////    { Type_DataTrans_StopTrans, reinterpret_cast<void*>(SIPCCMsg_DataTrans_StopTrans_to_str) },
//////    { Type_Client_AgentHoldCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentHoldCall_to_str) },
//////    { Type_UState_AgentHoldCallConf, reinterpret_cast<void*>(SIPCCMsg_UState_AgentHoldCallConf_to_str) },
//////    { Type_Client_BeforeTransfer, reinterpret_cast<void*>(SIPCCMsg_Client_BeforeTransfer_to_str) },
//////    { Type_Client_TransferCall, reinterpret_cast<void*>(SIPCCMsg_Client_TransferCall_to_str) },
//////    { Type_ACD_CallArriveToAgent, reinterpret_cast<void*>(SIPCCMsg_ACD_CallArriveToAgent_to_str) },
//////    { Type_Record_AgentRecordQueryPort, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordQueryPort_to_str) },
//////    { Type_Record_AgentRecordQueryPortConf, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordQueryPortConf_to_str) },
//////    { Type_Record_AgentRecordStart, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordStart_to_str) },
//////    { Type_Record_AgentRecordStop, reinterpret_cast<void*>(SIPCCMsg_Record_AgentRecordStop_to_str) },
//////    { Type_Client_MediaRequest, reinterpret_cast<void*>(SIPCCMsg_Client_MediaRequest_to_str) },
//////    { Type_Trans_TransmitRequest, reinterpret_cast<void*>(SIPCCMsg_Trans_TransmitRequest_to_str) },
//////    { Type_Trans_TransmitRequestConf, reinterpret_cast<void*>(SIPCCMsg_Trans_TransmitRequestConf_to_str) },
//////    { Type_ACD_AgentMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentMakeCallConf_to_str) },
//////    { Type_Client_AgentTestNet, reinterpret_cast<void*>(SIPCCMsg_Client_AgentTestNet_to_str) },
//////    { Type_Client_AgentTestNetConf, reinterpret_cast<void*>(SIPCCMsg_Client_AgentTestNetConf_to_str) },
//////    { Type_Client_LanPortApprize, reinterpret_cast<void*>(SIPCCMsg_Client_LanPortApprize_to_str) },
//////    { Type_Client_GetDepartNAgentInfo, reinterpret_cast<void*>(SIPCCMsg_Client_GetDepartNAgentInfo_to_str) },
//////    { Type_Client_ConferenceStart, reinterpret_cast<void*>(SIPCCMsg_Client_ConferenceStart_to_str) },
//////    { Type_Client_ConferenceEnd, reinterpret_cast<void*>(SIPCCMsg_Client_ConferenceEnd_to_str) },
//////    { Type_PhoneSearch_ExtNoInfo, reinterpret_cast<void*>(SIPCCMsg_PhoneSearch_ExtNoInfo_to_str) },
//////    { Type_PhoneSearch_ExtNoInfoConf, reinterpret_cast<void*>(SIPCCMsg_PhoneSearch_ExtNoInfoConf_to_str) },
//////    { Type_Gateway_IsAgentLogin, reinterpret_cast<void*>(SIPCCMsg_Gateway_IsAgentLogin_to_str) },
//////    { Type_ACD_IsAgentLoginConf, reinterpret_cast<void*>(SIPCCMsg_ACD_IsAgentLoginConf_to_str) },
//////    { Type_ACD_DepartNAgentInfo, reinterpret_cast<void*>(SIPCCMsg_ACD_DepartNAgentInfo_to_str) },
//////    { Type_ACD_ConferenceStartConf, reinterpret_cast<void*>(SIPCCMsg_ACD_ConferenceStartConf_to_str) },
//////    { Type_ACD_ConferenceStart, reinterpret_cast<void*>(SIPCCMsg_ACD_ConferenceStart_to_str) },
//////    { Type_ACD_ConferenceEnd, reinterpret_cast<void*>(SIPCCMsg_ACD_ConferenceEnd_to_str) },
//////    { Type_ACD_UserLeaveConference, reinterpret_cast<void*>(SIPCCMsg_ACD_UserLeaveConference_to_str) },
//////    { Type_ACD_CallDelivered, reinterpret_cast<void*>(SIPCCMsg_ACD_CallDelivered_to_str) },
//////    { Type_Client_AgentCallReceived, reinterpret_cast<void*>(SIPCCMsg_Client_AgentCallReceived_to_str) },
//////    { Type_Client_AgentAnswerCall, reinterpret_cast<void*>(SIPCCMsg_Client_AgentAnswerCall_to_str) },
//////    { Type_UState_CallEstablished, reinterpret_cast<void*>(SIPCCMsg_UState_CallEstablished_to_str) },
//////    { Type_ACD_MediaRequest, reinterpret_cast<void*>(SIPCCMsg_ACD_MediaRequest_to_str) },
//////    { Type_ACD_GetTalkingAgentConf, reinterpret_cast<void*>(SIPCCMsg_ACD_GetTalkingAgentConf_to_str) },
//////    { Type_Client_GetTalkingAgent, reinterpret_cast<void*>(SIPCCMsg_Client_GetTalkingAgent_to_str) },
//////    { Type_Client_MonitRequest, reinterpret_cast<void*>(SIPCCMsg_Client_MonitRequest_to_str) },
//////    { Type_Client_MonitRequestConf, reinterpret_cast<void*>(SIPCCMsg_Client_MonitRequestConf_to_str) },
//////    { Type_Client_StopMonit, reinterpret_cast<void*>(SIPCCMsg_Client_StopMonit_to_str) },
//////    { Type_ACD_GetTalkingAgentConf, reinterpret_cast<void*>(SIPCCMsg_ACD_GetTalkingAgentConf_to_str) },
//////    { Type_Gateway_UserLeaveConference, reinterpret_cast<void*>(SIPCCMsg_Gateway_UserLeaveConference_to_str) },
//////    { Type_Client_WebPSTNDial, reinterpret_cast<void*>(SIPCCMsg_Client_WebPSTNDial_to_str) },
//////    { Type_Gateway_WebPSTNDialConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_WebPSTNDialConf_to_str) },
//////    { Type_Client_TextChat, reinterpret_cast<void*>(SIPCCMsg_Client_TextChat_to_str) },
//////    //{Type_Text_TextChat                   , reinterpret_cast<void*>(SIPCCMsg_Text_TextChat_to_str)            },
//////    { Type_Client_LoginToText, reinterpret_cast<void*>(SIPCCMsg_Client_LoginToText_to_str) },
//////    { Type_CClient_CMakeCall, reinterpret_cast<void*>(SIPCCMsg_CClient_CMakeCall_to_str) },
//////    { Type_CClient_CallArrived, reinterpret_cast<void*>(SIPCCMsg_CClient_CallArrived_to_str) },
//////    { Type_CClient_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_CClient_CallAnswered_to_str) },
//////    { Type_CProxy_HeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_HeartBeatConf_to_str) },
//////    { Type_CProxy_CMakeCallConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCallConf_to_str) },
//////    { Type_CProxy_CMakeCall, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCall_to_str) },
//////    { Type_CProxy_CallDelivered, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallDelivered_to_str) },
//////    { Type_CProxy_CallArrived, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallArrived_to_str) },
//////    { Type_CProxy_CallAnswered, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallAnswered_to_str) },
//////    { Type_CProxy_CallStart, reinterpret_cast<void*>(SIPCCMsg_CProxy_CallStart_to_str) },
//////    { Type_CProxy_QueryGW, reinterpret_cast<void*>(SIPCCMsg_CProxy_QueryGW_to_str) },
//////    { Type_CProxy_CMakeCall2, reinterpret_cast<void*>(SIPCCMsg_CProxy_CMakeCall2_to_str) },
//////    { Type_CProxy_HangupCall, reinterpret_cast<void*>(SIPCCMsg_CProxy_HangupCall_to_str) },
//////    { Type_CProxy_HangupCallConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_HangupCallConf_to_str) },
//////    { Type_CClient_HangupCallConf, reinterpret_cast<void*>(SIPCCMsg_CClient_HangupCallConf_to_str) },
//////    { Type_Client_CallUserConf, reinterpret_cast<void*>(SIPCCMsg_Client_CallUserConf_to_str) },
//////    { Type_Client_UserAnswerCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserAnswerCall_to_str) },
//////    { Type_Client_UserAnswerCall_Real, reinterpret_cast<void*>(SIPCCMsg_Client_UserAnswerCall_Real_to_str) },
//////    { Type_Client_UserHangupCall, reinterpret_cast<void*>(SIPCCMsg_Client_UserHangupCall_to_str) },
//////    { Type_Client_MediaRequest, reinterpret_cast<void*>(SIPCCMsg_Client_MediaRequest_to_str) },
//////    { Type_Client_MediaRequestConf, reinterpret_cast<void*>(SIPCCMsg_Client_MediaRequestConf_to_str) },
//////    { Type_OneKeySwitch_ServiceRegisterConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceRegisterConf_to_str) },
//////    { Type_OneKeySwitch_ServiceConfirmConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceConfirmConf_to_str) },
//////    { Type_OneKeySwitch_GetHeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetHeartBeatConf_to_str) },
//////    { Type_OneKeySwitch_ServiceConfirm, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceConfirm_to_str) },
//////    { Type_OneKeySwitch_ServerHeartBeatConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServerHeartBeatConf_to_str) },
//////    { Type_OneKeySwitch_ServiceTimeout, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceTimeout_to_str) },
//////    { Type_OneKeySwitch_GetServiceConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GetServiceConf_to_str) },
//////    { Type_OneKeySwitch_SwitchService_Ctrl, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_SwitchService_Ctrl_to_str) },
//////    { Type_Client_UserLoginStateSvr, reinterpret_cast<void*>(SIPCCMsg_Client_UserLoginStateSvr_to_str) },
//////    { Type_Gateway_Login, reinterpret_cast<void*>(SIPCCMsg_Gateway_Login_to_str) },
//////    { Type_Gateway_LoginConf, reinterpret_cast<void*>(SIPCCMsg_Gateway_LoginConf_to_str) },
//////    { Type_OneKeySwitch_ServiceRegister, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_ServiceRegister_to_str) },
//////    { Type_Trans_TransmitStopRequest, reinterpret_cast<void*>(SIPCCMsg_Trans_TransmitStopRequest_to_str) },
//////    { Type_FileTransfer_UploadRequest, reinterpret_cast<void*>(SIPCCMsg_FileTransfer_UploadRequest_to_str) },
//////    { Type_OneKeySwitch_UserLoginStateSvrConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_UserLoginStateSvrConf_to_str) },
//////    { Type_OneKeySwitch_GWLogout, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GWLogout_to_str) },
//////    { Type_OneKeySwitch_GWLogin, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GWLogin_to_str) },
//////    { Type_OneKeySwitch_UserLogin, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_UserLogin_to_str) },
//////    { Type_UState_CallReceived, reinterpret_cast<void*>(SIPCCMsg_UState_CallReceived_to_str) },
//////    { Type_SysManage_AddGWTG, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddGWTG_to_str) },
//////    { Type_SysManage_AddDeptTG, reinterpret_cast<void*>(SIPCCMsg_SysManage_AddDeptTG_to_str) },
//////    { Type_ACD_AgentStateInfo, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentStateInfo_to_str) },
//////    { Type_CProxy_Login, reinterpret_cast<void*>(SIPCCMsg_CProxy_Login_to_str) },
//////    { Type_CProxy_LoginConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_LoginConf_to_str) },
//////    { Type_CProxy_Logout, reinterpret_cast<void*>(SIPCCMsg_CProxy_Logout_to_str) },
//////    { Type_CProxy_LogoutConf, reinterpret_cast<void*>(SIPCCMsg_CProxy_LogoutConf_to_str) },
//////    { Type_SysManage_LoadServerInfoConf, reinterpret_cast<void*>(SIPCCMsg_SysManage_LoadServerInfoConf_to_str) },
//////    { Type_OneKeySwitch_GWLoginConf, reinterpret_cast<void*>(SIPCCMsg_OneKeySwitch_GWLoginConf_to_str) },
//////    { Type_GCTI_Meaage, reinterpret_cast<void*>(SIPCCMsg_GCTI_Message_to_str) },
//////    { Type_UState_CallReceived2, reinterpret_cast<void*>(SIPCCMsg_UState_CallReceived2_to_str) },
//////    { Type_UState_CallHanguped, reinterpret_cast<void*>(SIPCCMsg_UState_CallHanguped_to_str) },
//////    { Type_Gateway_LoadGWCorpInfo, reinterpret_cast<void*>(SIPCCMsg_Gateway_LoadGWCorpInfo_to_str) },
//////    { Type_Redirect_AppServerListConf, reinterpret_cast<void*>(SIPCCMsg_Redirect_AppServerListConf_to_str) },
//////    { Type_Client_MessageCallInfo, reinterpret_cast<void*>(SIPCCMsg_Client_MessageCallInfo_to_str) },
//////    { Type_Client_AgentSwitchToFrontDesk, reinterpret_cast<void*>(SIPCCMsg_Client_AgentSwitchToFrontDesk_to_str) },
//////    { Type_ACD_AgentSwitchToFrontDeskConf, reinterpret_cast<void*>(SIPCCMsg_ACD_AgentSwitchToFrontDeskConf_to_str) },
//////
//////	{-1, 0 }
//////}; 

string to_str(const SIPCCMessage &ms)
{
    return ipccmsg_to_str(ms);
}

//////string ipccmsg_to_str(const SIPCCMessage &msg)
//////{
//////    typedef string (msg_dumper) (const SIPCCMessage &) ; 
//////    for (int i=0; msg_dumpers[i].first!=-1; i++)
//////    {
//////        if (msg_dumpers[i].first == msg.msgType)
//////        {
//////            return reinterpret_cast<msg_dumper*>(msg_dumpers[i].second)(msg);
//////        }
//////    }
//////    return "";
//////}

string ipccmsg_to_str(const SIPCCMessage &msg)
{
    static CMsgDumpers msg_dumpers;
    typedef string(msg_dumper) (const SIPCCMessage &);

    if(msg_dumpers.m_msg2logfunc.find(msg.msgType) != msg_dumpers.m_msg2logfunc.end()){
        return reinterpret_cast<msg_dumper*>(msg_dumpers.m_msg2logfunc[msg.msgType])(msg);
    }
    return "";
}

string call_type_to_str(int i)
{
    return to_str(__CallPartyType(i));
}


}
}
