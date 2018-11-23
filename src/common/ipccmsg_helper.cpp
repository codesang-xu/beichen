#include "common/ipccmsg_helper.h"

#include <sstream>

namespace ib
{
namespace common
{

string id_to_name(id_name id_names[], int size, int id, bool readable)
{
    for (int i=0; i<size; i++)
    {
        if (id_names[i]._id == id)
        {
			return id_names[i].to_string(readable);
        }
    }
	ostringstream oss;
	oss << id;
    return oss.str();
}

string id_to_name(map<int, string> & id_names, int id, bool readable = false)
{
    stringstream ss;
    ss << id_names[id];
    if (readable)
        ss << "(" << id << ")";
    return ss.str();
}


string to_str(__EIPCCMsgClass c)
{
    static id_name msg_class[] = 
    {
        {Class_Login,                "Class_Login"},
        {Class_Guide,                "Class_Guide"},
        {Class_ACD,                  "Class_ACD"},
        {Class_DataCollection,       "Class_DataCollection"},
        {Class_UserMark,             "Class_UserMark"},
        {Class_Relay,                "Class_Relay"},
        {Class_SysManage,            "Class_SysManage"},
        {Class_Client,               "Class_Client"},
        {Class_P2P,                  "Class_P2P"},
        {Class_EndEgn,               "Class_EndEgn"},
        {Class_HeartBeat,            "Class_HeartBeat"},
        {Class_Transmit,             "Class_Transmit"},
        {Class_UState,               "Class_UState"},
        {Class_File,                 "Class_File"},
        {Class_Record,               "Class_Record"},
        {Class_SmartRoute,           "Class_SmartRoute"},
        {Class_Gateway,              "Class_Gateway"},
        {Class_TextChat,             "Class_TextChat"},
        {Class_GwMonit,              "Class_GwMonit"},
        {Class_Redirect,             "Class_Redirect"},
        {Class_DlyStrategy,          "Class_DlyStrategy"},
        {Class_TollQuery,            "Class_TollQuery"},
        {Class_SMS,                  "Class_SMS"},
        {Class_Toll,                 "Class_Toll"},
        {Class_PhoneSearch,          "Class_PhoneSearch"},
        {Class_Wap,                  "Class_Wap"},
        {Class_DataRoute,            "Class_DataRoute"},
        {Class_DataTrans,            "Class_DataTrans"},
        {Class_Alert,                "Class_Alert"},
        {Class_OutDial,              "Class_OutDial"},
        {Class_Notify,               "Class_Notify"},
        {Class_FuncCall,             "Class_FuncCall"},
        {Class_FuncReturn,           "Class_FuncReturn"},
        {Class_FuncEvent,            "Class_FuncEvent"},
        {Class_OCXHeart,             "Class_OCXHeart"},
        {Class_Autodetector,         "Class_Autodetector"},
        {Class_OCXService,           "Class_OCXService"},
        {Class_Monitor,              "Class_Monitor"},
        {Class_Log,                  "Class_Log"},
        {Class_AreaCode,             "Class_AreaCode"},
        {Class_alarm,                "Class_alarm"},
        {Class_AgentRecord,          "Class_AgentRecord"},
        {Class_ServerProxy,          "Class_ServerProxy"},
        {Class_SAMPLER,              "Class_SAMPLER"},
        {Class_CallControl,          "Class_CallControl"},
        {Class_DIDControl,           "Class_DIDControl"},
        {Class_OneKeySwitch,         "Class_OneKeySwitch"},
        {Class_License,              "Class_License"},
        {Class_GCTI,                 "Class_GCTI"},
        {Class_Board,                "Class_Board"},
//      {Class_Timer,                "Class_Timer"},
        {Class_CID,                  "Class_CID"},
//      {Class_ClientCapture,        "Class_ClientCapture"},
        {Class_VoiceRecognition,     "Class_VoiceRecognition"},
        {Class_TTS,                  "Class_TTS"},
        {Class_batExtRec,            "Class_batExtRec" },
        {Class_CClient,              "Class_CClient" },
        {Class_CProxy,               "Class_CProxy" },
        {Class_RouteSelect,          "Class_RouteSelect" },
        {Class_FileTransfer,         "Class_FileTransfer" }
    };
    return id_to_name(msg_class, ARRAY_LEN(msg_class), c);
}

string ipccmsg_class_to_str(int i)
{
	return to_str(__EIPCCMsgClass(i));
}

string to_str(__EIPCCMsgType t)
{
    class CMsgTypes
    {
    public:
        CMsgTypes()
        {
            m_Id2Nme.insert(make_pair(Type_Client_UserLogin, "Type_Client_UserLogin"));
            m_Id2Nme.insert(make_pair(Type_Client_UserQueryCall, "Type_Client_UserQueryCall"));
            m_Id2Nme.insert(make_pair(Type_Client_UserMakeCall, "Type_Client_UserMakeCall"));
            m_Id2Nme.insert(make_pair(Type_Client_UserHangupCall, "Type_Client_UserHangupCall"));
            m_Id2Nme.insert(make_pair(Type_Client_UserMark, "Type_Client_UserMark"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentLogin, "Type_Client_AgentLogin"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentLogout, "Type_Client_AgentLogout"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentPark, "Type_Client_AgentPark"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentUnPark, "Type_Client_AgentUnPark"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentAnswerCall, "Type_Client_AgentAnswerCall"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentHangupCall, "Type_Client_AgentHangupCall"));
            m_Id2Nme.insert(make_pair(Type_Client_TextChatWords, "Type_Client_TextChatWords"));
            m_Id2Nme.insert(make_pair(Type_Client_MediaRequest, "Type_Client_MediaRequest"));
            m_Id2Nme.insert(make_pair(Type_Client_MediaRequestConf, "Type_Client_MediaRequestConf"));
            m_Id2Nme.insert(make_pair(Type_Client_MediaReady, "Type_Client_MediaReady"));
            m_Id2Nme.insert(make_pair(Type_Login_UserLoginSucc, "Type_Login_UserLoginSucc"));
            m_Id2Nme.insert(make_pair(Type_Login_UserLoginFailed, "Type_Login_UserLoginFailed"));
            m_Id2Nme.insert(make_pair(Type_Guide_ACDInfo, "Type_Guide_ACDInfo"));
            m_Id2Nme.insert(make_pair(Type_Guide_AddKeyConf, "Type_Guide_AddKeyConf"));
            m_Id2Nme.insert(make_pair(Type_Guide_DelKeyConf, "Type_Guide_DelKeyConf"));
            m_Id2Nme.insert(make_pair(Type_Guide_LoadKeyWords, "Type_Guide_LoadKeyWords"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddKey, "Type_SysManage_AddKey"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelKey, "Type_SysManage_DelKey"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddDepartment, "Type_SysManage_AddDepartment"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelDepartment, "Type_SysManage_DelDepartment"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentLoginSucc, "Type_ACD_AgentLoginSucc"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentLoginFailed, "Type_ACD_AgentLoginFailed"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentLogoutConf, "Type_ACD_AgentLogoutConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentParkConf, "Type_ACD_AgentParkConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentUnParkConf, "Type_ACD_AgentUnParkConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_UserMakeCallConf, "Type_ACD_UserMakeCallConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallDelivered, "Type_ACD_CallDelivered"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallEstablished, "Type_ACD_CallEstablished"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallAnswered, "Type_ACD_CallAnswered"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallReleased, "Type_ACD_CallReleased"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallHanguped, "Type_ACD_CallHanguped"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentStateInfo, "Type_ACD_AgentStateInfo"));
            m_Id2Nme.insert(make_pair(Type_ACD_AddDepartmentConf, "Type_ACD_AddDepartmentConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_DelDepartmentConf, "Type_ACD_DelDepartmentConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_LoadDepartments, "Type_ACD_LoadDepartments"));
            m_Id2Nme.insert(make_pair(Type_ACD_MediaRequest, "Type_ACD_MediaRequest"));
            m_Id2Nme.insert(make_pair(Type_ACD_MediaRequestConf, "Type_ACD_MediaRequestConf"));
            m_Id2Nme.insert(make_pair(Type_P2P_ConnRequest, "Type_P2P_ConnRequest"));
            m_Id2Nme.insert(make_pair(Type_P2P_MakeHoleCmd, "Type_P2P_MakeHoleCmd"));
            m_Id2Nme.insert(make_pair(Type_P2P_MakeHoleCmdConf, "Type_P2P_MakeHoleCmdConf"));
            m_Id2Nme.insert(make_pair(Type_P2P_MakeHoleMsg, "Type_P2P_MakeHoleMsg"));
            m_Id2Nme.insert(make_pair(Type_P2P_ConnRequestConf, "Type_P2P_ConnRequestConf"));
            m_Id2Nme.insert(make_pair(Type_P2P_TestMsg, "Type_P2P_TestMsg"));
            m_Id2Nme.insert(make_pair(Type_P2P_TestMsgConf, "Type_P2P_TestMsgConf"));
            m_Id2Nme.insert(make_pair(Type_P2P_DirectConn, "Type_P2P_DirectConn"));
            m_Id2Nme.insert(make_pair(Type_HeartBeat, "Type_HeartBeat"));
            m_Id2Nme.insert(make_pair(Type_EndEgn, "Type_EndEgn"));
            m_Id2Nme.insert(make_pair(Type_P2P_ChechNatRequest, "Type_P2P_ChechNatRequest"));
            m_Id2Nme.insert(make_pair(Type_P2P_ChechNatConf, "Type_P2P_ChechNatConf"));
            m_Id2Nme.insert(make_pair(Type_P2P_ConnDifficultMsg, "Type_P2P_ConnDifficultMsg"));
            m_Id2Nme.insert(make_pair(Type_P2P_ReInitSockCmd, "Type_P2P_ReInitSockCmd"));
            m_Id2Nme.insert(make_pair(Type_P2P_ReInitSockFailed, "Type_P2P_ReInitSockFailed"));
            m_Id2Nme.insert(make_pair(Type_Trans_TransmitRequest, "Type_Trans_TransmitRequest"));
            m_Id2Nme.insert(make_pair(Type_Trans_TransmitRequestConf, "Type_Trans_TransmitRequestConf"));
            m_Id2Nme.insert(make_pair(Type_Trans_TransmitStopRequest, "Type_Trans_TransmitStopRequest"));
            m_Id2Nme.insert(make_pair(Type_Trans_TransmitRejected, "Type_Trans_TransmitRejected"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallArriveToAgent, "Type_ACD_CallArriveToAgent"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallingQueueInfo, "Type_ACD_CallingQueueInfo"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentQueryStateConf, "Type_ACD_AgentQueryStateConf"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentCallReceived, "Type_Client_AgentCallReceived"));
            m_Id2Nme.insert(make_pair(Type_Client_GetCallingQueueInfo, "Type_Client_GetCallingQueueInfo"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentQueryState, "Type_Client_AgentQueryState"));
            m_Id2Nme.insert(make_pair(Type_Client_UserQueryState, "Type_Client_UserQueryState"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentMakeCall, "Type_Client_AgentMakeCall"));
            m_Id2Nme.insert(make_pair(Type_Client_UserReset, "Type_Client_UserReset"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentReset, "Type_Client_AgentReset"));
            m_Id2Nme.insert(make_pair(Type_UState_AgentMakeCallConf, "Type_UState_AgentMakeCallConf"));
            m_Id2Nme.insert(make_pair(Type_UState_CallReceived, "Type_UState_CallReceived"));
            m_Id2Nme.insert(make_pair(Type_UState_CallArriveToAgent, "Type_UState_CallArriveToAgent"));
            m_Id2Nme.insert(make_pair(Type_UState_UserQueryStateConf, "Type_UState_UserQueryStateConf"));
            m_Id2Nme.insert(make_pair(Type_File_TransRequest, "Type_File_TransRequest"));
            m_Id2Nme.insert(make_pair(Type_File_TransRequestConf, "Type_File_TransRequestConf"));
            m_Id2Nme.insert(make_pair(Type_File_TransReady, "Type_File_TransReady"));
            m_Id2Nme.insert(make_pair(Type_P2P_MediaNegotiate, "Type_P2P_MediaNegotiate"));
            m_Id2Nme.insert(make_pair(Type_P2P_MediaNegotiateConf, "Type_P2P_MediaNegotiateConf"));
            m_Id2Nme.insert(make_pair(Type_Client_UserLoginStateSvr, "Type_Client_UserLoginStateSvr"));
            m_Id2Nme.insert(make_pair(Type_Client_UserLogoutStateSvr, "Type_Client_UserLogoutStateSvr"));
            m_Id2Nme.insert(make_pair(Type_Client_UserAnswerCall, "Type_Client_UserAnswerCall"));
            m_Id2Nme.insert(make_pair(Type_UState_UserLoginConf, "Type_UState_UserLoginConf"));
            m_Id2Nme.insert(make_pair(Type_UState_UserLogoutConf, "Type_UState_UserLogoutConf"));
            m_Id2Nme.insert(make_pair(Type_UState_CallHanguped, "Type_UState_CallHanguped"));
            m_Id2Nme.insert(make_pair(Type_UState_MediaRequest, "Type_UState_MediaRequest"));
            m_Id2Nme.insert(make_pair(Type_UState_CallAnswered, "Type_UState_CallAnswered"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentMakeCallConf, "Type_ACD_AgentMakeCallConf"));
            m_Id2Nme.insert(make_pair(Type_File_UnConnect, "Type_File_UnConnect"));
            m_Id2Nme.insert(make_pair(Type_UState_CallReleased, "Type_UState_CallReleased"));
            m_Id2Nme.insert(make_pair(Type_UState_CallEstablished, "Type_UState_CallEstablished"));
            m_Id2Nme.insert(make_pair(Type_Client_CallUserConf, "Type_Client_CallUserConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallUserConf, "Type_ACD_CallUserConf"));
            m_Id2Nme.insert(make_pair(Type_Record_Login, "Type_Record_Login"));
            m_Id2Nme.insert(make_pair(Type_Record_LoginConf, "Type_Record_LoginConf"));
            m_Id2Nme.insert(make_pair(Type_Record_StartRecord, "Type_Record_StartRecord"));
            m_Id2Nme.insert(make_pair(Type_Record_StopRecord, "Type_Record_StopRecord"));
            m_Id2Nme.insert(make_pair(Type_UState_UserNoResponse, "Type_UState_UserNoResponse"));
            m_Id2Nme.insert(make_pair(Type_File_FileRequest, "Type_File_FileRequest"));
            m_Id2Nme.insert(make_pair(Type_File_FileRequestConf, "Type_File_FileRequestConf"));
            m_Id2Nme.insert(make_pair(Type_File_FileReceived, "Type_File_FileReceived"));
            m_Id2Nme.insert(make_pair(Type_File_FileStopped, "Type_File_FileStopped"));
            m_Id2Nme.insert(make_pair(Type_File_FileReceiveInfo, "Type_File_FileReceiveInfo"));
            m_Id2Nme.insert(make_pair(Type_Record_StartRecordConf, "Type_Record_StartRecordConf"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentHoldCall, "Type_Client_AgentHoldCall"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentUnHoldCall, "Type_Client_AgentUnHoldCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentHoldCall, "Type_ACD_AgentHoldCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentUnHoldCall, "Type_ACD_AgentUnHoldCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentHoldCallConf, "Type_ACD_AgentHoldCallConf"));
            m_Id2Nme.insert(make_pair(Type_UState_AgentHoldCall, "Type_UState_AgentHoldCall"));
            m_Id2Nme.insert(make_pair(Type_UState_AgentUnHoldCall, "Type_UState_AgentUnHoldCall"));
            m_Id2Nme.insert(make_pair(Type_UState_AgentHoldCallConf, "Type_UState_AgentHoldCallConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_NotifyDataCollect, "Type_ACD_NotifyDataCollect"));
            m_Id2Nme.insert(make_pair(Type_Client_CallUserFailed, "Type_Client_CallUserFailed"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallUserFailed, "Type_ACD_CallUserFailed"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_QueryUserStatus, "Type_SmartRoute_QueryUserStatus"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_QueryUserStatusConf, "Type_SmartRoute_QueryUserStatusConf"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_LoadSmartRouteTactic, "Type_SmartRoute_LoadSmartRouteTactic"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_BlackListTactic, "Type_SmartRoute_BlackListTactic"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_NewUserTactic, "Type_SmartRoute_NewUserTactic"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_OldUserTactic, "Type_SmartRoute_OldUserTactic"));
            m_Id2Nme.insert(make_pair(Type_SmartRoute_RouteTactic, "Type_SmartRoute_RouteTactic"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryRouteTactic, "Type_Client_QueryRouteTactic"));
            m_Id2Nme.insert(make_pair(Type_Collect_RegisteConf, "Type_Collect_RegisteConf"));
            m_Id2Nme.insert(make_pair(Type_Collect_StatProcess, "Type_Collect_StatProcess"));
            m_Id2Nme.insert(make_pair(Type_Storage_Registe, "Type_Storage_Registe"));
            m_Id2Nme.insert(make_pair(Type_Storage_UnRegiste, "Type_Storage_UnRegiste"));
            m_Id2Nme.insert(make_pair(Type_Client_PrepareTransfer, "Type_Client_PrepareTransfer"));
            m_Id2Nme.insert(make_pair(Type_ACD_PrepareTransfer, "Type_ACD_PrepareTransfer"));
            m_Id2Nme.insert(make_pair(Type_UState_PrepareTransfer, "Type_UState_PrepareTransfer"));
            m_Id2Nme.insert(make_pair(Type_Client_TransReady, "Type_Client_TransReady"));
            m_Id2Nme.insert(make_pair(Type_ACD_TransReady, "Type_ACD_TransReady"));
            m_Id2Nme.insert(make_pair(Type_UState_TransReady, "Type_UState_TransReady"));
            m_Id2Nme.insert(make_pair(Type_Client_TransferCall, "Type_Client_TransferCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_TransferCall, "Type_ACD_TransferCall"));
            m_Id2Nme.insert(make_pair(Type_UState_TransferCall, "Type_UState_TransferCall"));
            m_Id2Nme.insert(make_pair(Type_Client_ClearPreConnection, "Type_Client_ClearPreConnection"));
            m_Id2Nme.insert(make_pair(Type_ACD_ClearPreConnection, "Type_ACD_ClearPreConnection"));
            m_Id2Nme.insert(make_pair(Type_UState_ClearPreConnection, "Type_UState_ClearPreConnection"));
            m_Id2Nme.insert(make_pair(Type_Gateway_Login, "Type_Gateway_Login"));
            m_Id2Nme.insert(make_pair(Type_Gateway_LoginConf, "Type_Gateway_LoginConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_LoadInfo, "Type_Gateway_LoadInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SetBaseInfo, "Type_Gateway_SetBaseInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AddPrefixNoInfo, "Type_Gateway_AddPrefixNoInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SetCorporationInfo, "Type_Gateway_SetCorporationInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_DelCorporationInfo, "Type_Gateway_DelCorporationInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_Logout, "Type_Gateway_Logout"));
            m_Id2Nme.insert(make_pair(Type_Gateway_QueryGWConf, "Type_Gateway_QueryGWConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallInQuery, "Type_Gateway_CallInQuery"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallInQueryConf, "Type_Gateway_CallInQueryConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SetTrunkState, "Type_Gateway_SetTrunkState"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SyncTrunkState, "Type_Gateway_SyncTrunkState"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentTestNetConf, "Type_Gateway_AgentTestNetConf"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryGW, "Type_Client_QueryGW"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentTestNet, "Type_Client_AgentTestNet"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentTestNetConf, "Type_Client_AgentTestNetConf"));
            m_Id2Nme.insert(make_pair(Type_Client_LanPortApprize, "Type_Client_LanPortApprize"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryGWLanInfo, "Type_Client_QueryGWLanInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_QueryGWLanInfoConf, "Type_Gateway_QueryGWLanInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Client_GetDepartNAgentInfo, "Type_Client_GetDepartNAgentInfo"));
            m_Id2Nme.insert(make_pair(Type_ACD_DepartNAgentInfo, "Type_ACD_DepartNAgentInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_ReConnect, "Type_Gateway_ReConnect"));
            m_Id2Nme.insert(make_pair(Type_Client_GetGroupRequest, "Type_Client_GetGroupRequest"));
            m_Id2Nme.insert(make_pair(Type_ACD_GroupInfo, "Type_ACD_GroupInfo"));
            m_Id2Nme.insert(make_pair(Type_ACD_NotifyAgentStateEvent, "Type_ACD_NotifyAgentStateEvent"));
            m_Id2Nme.insert(make_pair(Type_Login_ServerRegister, "Type_Login_ServerRegister"));
            m_Id2Nme.insert(make_pair(Type_Login_ServerRegisterConf, "Type_Login_ServerRegisterConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_VDNStatusInfo, "Type_ACD_VDNStatusInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_ModifyKeyword, "Type_SysManage_ModifyKeyword"));
            m_Id2Nme.insert(make_pair(Type_SysManage_ModifyKeyDepart, "Type_SysManage_ModifyKeyDepart"));
            m_Id2Nme.insert(make_pair(Type_SysManage_ModifyDepart, "Type_SysManage_ModifyDepart"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddGatewayInfo, "Type_SysManage_AddGatewayInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelGatewayInfo, "Type_SysManage_DelGatewayInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddGWCorp, "Type_SysManage_AddGWCorp"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelGWCorp, "Type_SysManage_DelGWCorp"));
            m_Id2Nme.insert(make_pair(Type_Client_LoginToText, "Type_Client_LoginToText"));
            m_Id2Nme.insert(make_pair(Type_Client_LogoutFromText, "Type_Client_LogoutFromText"));
            m_Id2Nme.insert(make_pair(Type_Client_TextChat, "Type_Client_TextChat"));
            m_Id2Nme.insert(make_pair(Type_Text_TextChat, "Type_Text_TextChat"));
            m_Id2Nme.insert(make_pair(Type_ACD_MediaReady, "Type_ACD_MediaReady"));
            m_Id2Nme.insert(make_pair(Type_UState_MediaReady, "Type_UState_MediaReady"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddGWTG, "Type_SysManage_AddGWTG"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddDeptTG, "Type_SysManage_AddDeptTG"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddDialRule, "Type_SysManage_AddDialRule"));
            m_Id2Nme.insert(make_pair(Type_HeartBeatResponse, "Type_HeartBeatResponse"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelACDCorp, "Type_SysManage_DelACDCorp"));
            m_Id2Nme.insert(make_pair(Type_GwMonit_GetGwInfo, "Type_GwMonit_GetGwInfo"));
            m_Id2Nme.insert(make_pair(Type_GwMonit_GetGwInfoConf, "Type_GwMonit_GetGwInfoConf"));
            m_Id2Nme.insert(make_pair(Type_GwMonit_RegMonit, "Type_GwMonit_RegMonit"));
            m_Id2Nme.insert(make_pair(Type_GwMonit_TrunkState, "Type_GwMonit_TrunkState"));
            m_Id2Nme.insert(make_pair(Type_GwMonit_ComputerInfo, "Type_GwMonit_ComputerInfo"));
            m_Id2Nme.insert(make_pair(Type_GwMonit_LicenceInfo, "Type_GwMonit_LicenceInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_LoadCorpInfo, "Type_Redirect_LoadCorpInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_LoadCorpInfoConf, "Type_Redirect_LoadCorpInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Redirect_LoadPlatformInfo, "Type_Redirect_LoadPlatformInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_LoadPlatformInfoConf, "Type_Redirect_LoadPlatformInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Redirect_SearchPlatform, "Type_Redirect_SearchPlatform"));
            m_Id2Nme.insert(make_pair(Type_Redirect_SearchPlatformConf, "Type_Redirect_SearchPlatformConf"));
            m_Id2Nme.insert(make_pair(Type_Redirect_GetPlatformList, "Type_Redirect_GetPlatformList"));
            m_Id2Nme.insert(make_pair(Type_Redirect_GetPlatformListConf, "Type_Redirect_GetPlatformListConf"));
            m_Id2Nme.insert(make_pair(Type_Redirect_CorpRegister, "Type_Redirect_CorpRegister"));
            m_Id2Nme.insert(make_pair(Type_Redirect_GetCorpInfo, "Type_Redirect_GetCorpInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_GetCorpInfoConf, "Type_Redirect_GetCorpInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Redirect_AddCorpInfo, "Type_Redirect_AddCorpInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_DelCorpInfo, "Type_Redirect_DelCorpInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_AddPlatformInfo, "Type_Redirect_AddPlatformInfo"));
            m_Id2Nme.insert(make_pair(Type_Redirect_DelPlatformInfo, "Type_Redirect_DelPlatformInfo"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_QueryStrategy, "Type_DlyStrategy_QueryStrategy"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_QueryStrategyConf, "Type_DlyStrategy_QueryStrategyConf"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_AddStrategy, "Type_DlyStrategy_AddStrategy"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_DelStrategy, "Type_DlyStrategy_DelStrategy"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_LoadStrategy, "Type_DlyStrategy_LoadStrategy"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_LoadStrategyConf, "Type_DlyStrategy_LoadStrategyConf"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_GetStrategy, "Type_DlyStrategy_GetStrategy"));
            m_Id2Nme.insert(make_pair(Type_DlyStrategy_GetStrategyConf, "Type_DlyStrategy_GetStrategyConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentTalkBegin, "Type_Gateway_AgentTalkBegin"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentTalkEnd, "Type_Gateway_AgentTalkEnd"));
            m_Id2Nme.insert(make_pair(Type_TollQuery_TollQuery, "Type_TollQuery_TollQuery"));
            m_Id2Nme.insert(make_pair(Type_TollQuery_TollQueryConf, "Type_TollQuery_TollQueryConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_ForceLogout, "Type_ACD_ForceLogout"));
            m_Id2Nme.insert(make_pair(Type_Client_BeforeTransfer, "Type_Client_BeforeTransfer"));
            m_Id2Nme.insert(make_pair(Type_UState_BeforeTransfer, "Type_UState_BeforeTransfer"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SetRecord, "Type_Gateway_SetRecord"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SetRecordConf, "Type_Gateway_SetRecordConf"));
            m_Id2Nme.insert(make_pair(Type_UState_CallUserConf, "Type_UState_CallUserConf"));
            m_Id2Nme.insert(make_pair(Type_UState_CallUserFailed, "Type_UState_CallUserFailed"));
            m_Id2Nme.insert(make_pair(Type_UState_UserMakeCallConf, "Type_UState_UserMakeCallConf"));
            m_Id2Nme.insert(make_pair(Type_Client_UserMakeCallPSTN, "Type_Client_UserMakeCallPSTN"));
            m_Id2Nme.insert(make_pair(Type_SMS_AddLinkman, "Type_SMS_AddLinkman"));
            m_Id2Nme.insert(make_pair(Type_SMS_DelLinkman, "Type_SMS_DelLinkman"));
            m_Id2Nme.insert(make_pair(Type_SMS_ModifyLinkman, "Type_SMS_ModifyLinkman"));
            m_Id2Nme.insert(make_pair(Type_SMS_Send, "Type_SMS_Send"));
            m_Id2Nme.insert(make_pair(Type_SMS_SendConf, "Type_SMS_SendConf"));
            m_Id2Nme.insert(make_pair(Type_SMS_QueryLinkmanList, "Type_SMS_QueryLinkmanList"));
            m_Id2Nme.insert(make_pair(Type_SMS_QueryLinkmanListConf, "Type_SMS_QueryLinkmanListConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetTrunkState, "Type_Gateway_GetTrunkState"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetTrunkStateConf, "Type_Gateway_GetTrunkStateConf"));
            m_Id2Nme.insert(make_pair(Type_TollQuery_GwTollQuery, "Type_TollQuery_GwTollQuery"));
            m_Id2Nme.insert(make_pair(Type_TollQuery_GwTollQueryConf, "Type_TollQuery_GwTollQueryConf"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentMakeCall2, "Type_Client_AgentMakeCall2"));
            m_Id2Nme.insert(make_pair(Type_UState_CallReceived2, "Type_UState_CallReceived2"));
            m_Id2Nme.insert(make_pair(Type_Gateway_ForceHangup, "Type_Gateway_ForceHangup"));
            m_Id2Nme.insert(make_pair(Type_Gateway_DisableSth, "Type_Gateway_DisableSth"));
            m_Id2Nme.insert(make_pair(Type_Client_UserMakeCallPSTN2, "Type_Client_UserMakeCallPSTN2"));
            m_Id2Nme.insert(make_pair(Type_Client_PreConfercene, "Type_Client_PreConfercene"));
            m_Id2Nme.insert(make_pair(Type_Client_PreConferceneConf, "Type_Client_PreConferceneConf"));
            m_Id2Nme.insert(make_pair(Type_Client_Conference, "Type_Client_Conference"));
            m_Id2Nme.insert(make_pair(Type_ACD_PreConfercene, "Type_ACD_PreConfercene"));
            m_Id2Nme.insert(make_pair(Type_ACD_Conference, "Type_ACD_Conference"));
            m_Id2Nme.insert(make_pair(Type_ACD_PreConferceneConf, "Type_ACD_PreConferceneConf"));
            m_Id2Nme.insert(make_pair(Type_UState_PreConfercene, "Type_UState_PreConfercene"));
            m_Id2Nme.insert(make_pair(Type_UState_Confercene, "Type_UState_Confercene"));
            m_Id2Nme.insert(make_pair(Type_UState_PreConferceneConf, "Type_UState_PreConferceneConf"));
            m_Id2Nme.insert(make_pair(Type_Toll_400TrunkConnected, "Type_Toll_400TrunkConnected"));
            m_Id2Nme.insert(make_pair(Type_Toll_400TrunkDisconnected, "Type_Toll_400TrunkDisconnected"));
            m_Id2Nme.insert(make_pair(Type_Toll_400P2PTalkBegin, "Type_Toll_400P2PTalkBegin"));
            m_Id2Nme.insert(make_pair(Type_Toll_400P2PTalkEnd, "Type_Toll_400P2PTalkEnd"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_IncomingCall, "Type_PhoneSearch_IncomingCall"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_IncomingCallConf, "Type_PhoneSearch_IncomingCallConf"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_ExtNoInfo, "Type_PhoneSearch_ExtNoInfo"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_ExtNoInfoConf, "Type_PhoneSearch_ExtNoInfoConf"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_400CallIn, "Type_PhoneSearch_400CallIn"));
            m_Id2Nme.insert(make_pair(Type_Gateway_400CallBackCmd, "Type_Gateway_400CallBackCmd"));
            m_Id2Nme.insert(make_pair(Type_Toll_400CBPreTalkBegin, "Type_Toll_400CBPreTalkBegin"));
            m_Id2Nme.insert(make_pair(Type_Toll_400CBPreTalkEnd, "Type_Toll_400CBPreTalkEnd"));
            m_Id2Nme.insert(make_pair(Type_Toll_400CBTransTalkBegin, "Type_Toll_400CBTransTalkBegin"));
            m_Id2Nme.insert(make_pair(Type_Toll_400CBTransTalkEnd, "Type_Toll_400CBTransTalkEnd"));
            m_Id2Nme.insert(make_pair(Type_Toll_TollInfo, "Type_Toll_TollInfo"));
            m_Id2Nme.insert(make_pair(Type_Toll_TollInfoAck, "Type_Toll_TollInfoAck"));
            m_Id2Nme.insert(make_pair(Type_Record_StartTrunkP2ARecord, "Type_Record_StartTrunkP2ARecord"));
            m_Id2Nme.insert(make_pair(Type_Record_StopTrunkP2ARecord, "Type_Record_StopTrunkP2ARecord"));
            m_Id2Nme.insert(make_pair(Type_Record_StartTrunkP2PRecord, "Type_Record_StartTrunkP2PRecord"));
            m_Id2Nme.insert(make_pair(Type_Record_StopTrunkP2PRecord, "Type_Record_StopTrunkP2PRecord"));
            m_Id2Nme.insert(make_pair(Type_Toll_AgentTransP2PTalkBegin, "Type_Toll_AgentTransP2PTalkBegin"));
            m_Id2Nme.insert(make_pair(Type_Toll_AgentTransP2PTalkEnd, "Type_Toll_AgentTransP2PTalkEnd"));
            m_Id2Nme.insert(make_pair(Type_Toll_UniversalInfo, "Type_Toll_UniversalInfo"));
            m_Id2Nme.insert(make_pair(Type_Record_LoadManageInfo, "Type_Record_LoadManageInfo"));
            m_Id2Nme.insert(make_pair(Type_Record_LoadManageInfoConf, "Type_Record_LoadManageInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Record_AddRecCorp, "Type_Record_AddRecCorp"));
            m_Id2Nme.insert(make_pair(Type_Record_DelRecCorp, "Type_Record_DelRecCorp"));
            m_Id2Nme.insert(make_pair(Type_ACD_UserMakeCallPSTN, "Type_ACD_UserMakeCallPSTN"));
            m_Id2Nme.insert(make_pair(Type_ACD_UserMakeCallPSTN2, "Type_ACD_UserMakeCallPSTN2"));
            m_Id2Nme.insert(make_pair(Type_ACD_RingAgentExt, "Type_ACD_RingAgentExt"));
            m_Id2Nme.insert(make_pair(Type_ACD_RingAgentExt2, "Type_ACD_RingAgentExt2"));
            m_Id2Nme.insert(make_pair(Type_ACD_QueryGW, "Type_ACD_QueryGW"));
            m_Id2Nme.insert(make_pair(Type_UState_RingAgentExt, "Type_UState_RingAgentExt"));
            m_Id2Nme.insert(make_pair(Type_UState_RingAgentExt2, "Type_UState_RingAgentExt2"));
            m_Id2Nme.insert(make_pair(Type_Gateway_RingAgentExtConf, "Type_Gateway_RingAgentExtConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_RingAgentExtFailed, "Type_Gateway_RingAgentExtFailed"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentAnswerCall, "Type_Gateway_AgentAnswerCall"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentHangupCall, "Type_Gateway_AgentHangupCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_StopRingAgentExt, "Type_ACD_StopRingAgentExt"));
            m_Id2Nme.insert(make_pair(Type_Gateway_StopRingAgentExtConf, "Type_Gateway_StopRingAgentExtConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_QueryAgentInfo, "Type_Gateway_QueryAgentInfo"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryAgentInfoConf, "Type_PhoneSearch_QueryAgentInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CancelRingAgentExt, "Type_Gateway_CancelRingAgentExt"));
            m_Id2Nme.insert(make_pair(Type_Gateway_IsAgentLogin, "Type_Gateway_IsAgentLogin"));
            m_Id2Nme.insert(make_pair(Type_ACD_IsAgentLoginConf, "Type_ACD_IsAgentLoginConf"));
            m_Id2Nme.insert(make_pair(Type_Wap_CallOut, "Type_Wap_CallOut"));
            m_Id2Nme.insert(make_pair(Type_Client_WebPSTNDial, "Type_Client_WebPSTNDial"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallAgentExtFailed, "Type_Gateway_CallAgentExtFailed"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentExtCallerFailed, "Type_Gateway_AgentExtCallerFailed"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AgentExtCallSucc, "Type_Gateway_AgentExtCallSucc"));
            m_Id2Nme.insert(make_pair(Type_ACD_NotifyAgent, "Type_ACD_NotifyAgent"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryBlackList, "Type_PhoneSearch_QueryBlackList"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryBlackListConf, "Type_PhoneSearch_QueryBlackListConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_AddCorpNDeptInfo, "Type_ACD_AddCorpNDeptInfo"));
            m_Id2Nme.insert(make_pair(Type_Toll_SMS, "Type_Toll_SMS"));
            m_Id2Nme.insert(make_pair(Type_Toll_SMSAck, "Type_Toll_SMSAck"));
            m_Id2Nme.insert(make_pair(Type_Gateway_UnAnsweredCall, "Type_Gateway_UnAnsweredCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_SetClientAgentState, "Type_ACD_SetClientAgentState"));
            m_Id2Nme.insert(make_pair(Type_Client_WebPSTNAgentHoldCall, "Type_Client_WebPSTNAgentHoldCall"));
            m_Id2Nme.insert(make_pair(Type_Client_WebPSTNAgentUnHoldCall, "Type_Client_WebPSTNAgentUnHoldCall"));
            m_Id2Nme.insert(make_pair(Type_UState_WebPSTNAgentHoldCall, "Type_UState_WebPSTNAgentHoldCall"));
            m_Id2Nme.insert(make_pair(Type_UState_WebPSTNAgentHoldCallConf, "Type_UState_WebPSTNAgentHoldCallConf"));
            m_Id2Nme.insert(make_pair(Type_UState_WebPSTNAgentUnHoldCall, "Type_UState_WebPSTNAgentUnHoldCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallAnswered_WebPstnAgent, "Type_ACD_CallAnswered_WebPstnAgent"));
            m_Id2Nme.insert(make_pair(Type_Client_WpAgentTrans2VoipAgent, "Type_Client_WpAgentTrans2VoipAgent"));
            m_Id2Nme.insert(make_pair(Type_UState_WpAgentTrans2VoipAgent, "Type_UState_WpAgentTrans2VoipAgent"));
            m_Id2Nme.insert(make_pair(Type_Gateway_UserHangupCall, "Type_Gateway_UserHangupCall"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallReleased_WpAgent, "Type_ACD_CallReleased_WpAgent"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallAgentConf, "Type_Gateway_CallAgentConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_RingWpAgentExtFailed, "Type_ACD_RingWpAgentExtFailed"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallAgentFailed, "Type_Gateway_CallAgentFailed"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallWpAgentConf, "Type_ACD_CallWpAgentConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_TransferCall_WpAgent, "Type_ACD_TransferCall_WpAgent"));
            m_Id2Nme.insert(make_pair(Type_ACD_BeforeTransferCall_WpAgent, "Type_ACD_BeforeTransferCall_WpAgent"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallWpAgentExtFailed, "Type_ACD_CallWpAgentExtFailed"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallUserFailed_WpAgent, "Type_Gateway_CallUserFailed_WpAgent"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryTestKeyword, "Type_PhoneSearch_QueryTestKeyword"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryTestKeywordConf, "Type_PhoneSearch_QueryTestKeywordConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallCleared, "Type_Gateway_CallCleared"));
            m_Id2Nme.insert(make_pair(Type_Client_EchoRemoveCtrl, "Type_Client_EchoRemoveCtrl"));
            m_Id2Nme.insert(make_pair(Type_UState_EchoRemoveCtrl, "Type_UState_EchoRemoveCtrl"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryManagerId, "Type_PhoneSearch_QueryManagerId"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryManagerIdConf, "Type_PhoneSearch_QueryManagerIdConf"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryTransServer, "Type_Client_QueryTransServer"));
            m_Id2Nme.insert(make_pair(Type_DataRoute_QueryTransServerConf, "Type_DataRoute_QueryTransServerConf"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_Register, "Type_DataTrans_Register"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_UnRegister, "Type_DataTrans_UnRegister"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_StartTrans, "Type_DataTrans_StartTrans"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_StartTransConf, "Type_DataTrans_StartTransConf"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_StopTrans, "Type_DataTrans_StopTrans"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentLoadURL, "Type_Client_AgentLoadURL"));
            m_Id2Nme.insert(make_pair(Type_Login_AgentLoadURLConf, "Type_Login_AgentLoadURLConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallEnterVDN, "Type_ACD_CallEnterVDN"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallLeaveVDN, "Type_ACD_CallLeaveVDN"));
            m_Id2Nme.insert(make_pair(Type_Collect_StatProcessConf, "Type_Collect_StatProcessConf"));
            m_Id2Nme.insert(make_pair(Type_Collect_BeatHeart, "Type_Collect_BeatHeart"));
            m_Id2Nme.insert(make_pair(Type_Client_GetTalkingAgent, "Type_Client_GetTalkingAgent"));
            m_Id2Nme.insert(make_pair(Type_ACD_GetTalkingAgentConf, "Type_ACD_GetTalkingAgentConf"));
            m_Id2Nme.insert(make_pair(Type_Client_MonitRequest, "Type_Client_MonitRequest"));
            m_Id2Nme.insert(make_pair(Type_Client_MonitRequestConf, "Type_Client_MonitRequestConf"));
            m_Id2Nme.insert(make_pair(Type_Record_RecInfo2ACD, "Type_Record_RecInfo2ACD"));
            m_Id2Nme.insert(make_pair(Type_ACD_RecInfo2Client, "Type_ACD_RecInfo2Client"));
            m_Id2Nme.insert(make_pair(Type_Client_StopMonit, "Type_Client_StopMonit"));
            m_Id2Nme.insert(make_pair(Type_Toll_TollTransation, "Type_Toll_TollTransation"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetDeptQueueNum, "Type_Gateway_GetDeptQueueNum"));
            m_Id2Nme.insert(make_pair(Type_ACD_GetDeptQueueNumConf, "Type_ACD_GetDeptQueueNumConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetAgentState, "Type_Gateway_GetAgentState"));
            m_Id2Nme.insert(make_pair(Type_ACD_GetAgentStateConf, "Type_ACD_GetAgentStateConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_MonitRequest, "Type_ACD_MonitRequest"));
            m_Id2Nme.insert(make_pair(Type_ACD_MonitRequestConf, "Type_ACD_MonitRequestConf"));
            m_Id2Nme.insert(make_pair(Type_Client_MediaRequest_Monit, "Type_Client_MediaRequest_Monit"));
            m_Id2Nme.insert(make_pair(Type_ACD_MediaRequest_Monit, "Type_ACD_MediaRequest_Monit"));
            m_Id2Nme.insert(make_pair(Type_Client_MediaReady_Monit, "Type_Client_MediaReady_Monit"));
            m_Id2Nme.insert(make_pair(Type_Alert_NotifyInfo, "Type_Alert_NotifyInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_Valid, "Type_Gateway_Valid"));
            m_Id2Nme.insert(make_pair(Type_Client_SuperDisconnect, "Type_Client_SuperDisconnect"));
            m_Id2Nme.insert(make_pair(Type_ACD_SuperDisconnect, "Type_ACD_SuperDisconnect"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetGetwayInfo, "Type_Gateway_GetGetwayInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetGetwayInfoConf, "Type_Gateway_GetGetwayInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_AddRecordForTH, "Type_Gateway_AddRecordForTH"));
            m_Id2Nme.insert(make_pair(Type_Gateway_DelRecordForTH, "Type_Gateway_DelRecordForTH"));
            m_Id2Nme.insert(make_pair(Type_Gateway_DelAllRecordForTH, "Type_Gateway_DelAllRecordForTH"));
            m_Id2Nme.insert(make_pair(Type_ACD_MediaReady_Monit, "Type_ACD_MediaReady_Monit"));
            m_Id2Nme.insert(make_pair(Type_ACD_StopMonit, "Type_ACD_StopMonit"));
            m_Id2Nme.insert(make_pair(Type_Client_GetParkReasons, "Type_Client_GetParkReasons"));
            m_Id2Nme.insert(make_pair(Type_Login_GetParkReasonsConf, "Type_Login_GetParkReasonsConf"));
            m_Id2Nme.insert(make_pair(Type_DataRoute_QueryTransServer_IP, "Type_DataRoute_QueryTransServer_IP"));
            m_Id2Nme.insert(make_pair(Type_DataRoute_QueryTransServerConf_IP, "Type_DataRoute_QueryTransServerConf_IP"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_StartTrans_IP, "Type_DataTrans_StartTrans_IP"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_StartTransConf_IP, "Type_DataTrans_StartTransConf_IP"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_StopTrans_IP, "Type_DataTrans_StopTrans_IP"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetDeptWaitingAgentNum, "Type_Gateway_GetDeptWaitingAgentNum"));
            m_Id2Nme.insert(make_pair(Type_ACD_GetDeptWaitingAgentNumConf, "Type_ACD_GetDeptWaitingAgentNumConf"));
            m_Id2Nme.insert(make_pair(Type_Client_SendDtmf, "Type_Client_SendDtmf"));
            m_Id2Nme.insert(make_pair(Type_ACD_DelDepartment, "Type_ACD_DelDepartment"));
            m_Id2Nme.insert(make_pair(Type_Client_GetTrunkCount, "Type_Client_GetTrunkCount"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetTrunkCountConf, "Type_Gateway_GetTrunkCountConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_SetTalkSpan, "Type_ACD_SetTalkSpan"));
            m_Id2Nme.insert(make_pair(Type_ACD_SetAgentCount, "Type_ACD_SetAgentCount"));
            m_Id2Nme.insert(make_pair(Type_Client_SetOutDialResult, "Type_Client_SetOutDialResult"));
            m_Id2Nme.insert(make_pair(Type_SysManage_NotifyACDLicence, "Type_SysManage_NotifyACDLicence"));
            m_Id2Nme.insert(make_pair(Type_Client_UserAnswerCall_Real, "Type_Client_UserAnswerCall_Real"));
            m_Id2Nme.insert(make_pair(Type_ACD_CallEstablished_Real, "Type_ACD_CallEstablished_Real"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetDeptOverflow, "Type_Gateway_GetDeptOverflow"));
            m_Id2Nme.insert(make_pair(Type_ACD_GetDeptOverflowConf, "Type_ACD_GetDeptOverflowConf"));
            m_Id2Nme.insert(make_pair(Type_Client_ChangePassword, "Type_Client_ChangePassword"));
            m_Id2Nme.insert(make_pair(Type_Login_ChangePasswordConf, "Type_Login_ChangePasswordConf"));
            m_Id2Nme.insert(make_pair(Type_Client_SetOutdialCase, "Type_Client_SetOutdialCase"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryDelieverNo, "Type_PhoneSearch_QueryDelieverNo"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_QueryDelieverNoConf, "Type_PhoneSearch_QueryDelieverNoConf"));
            m_Id2Nme.insert(make_pair(Type_Client_ReceiveDtmf, "Type_Client_ReceiveDtmf"));
            m_Id2Nme.insert(make_pair(Type_OutDial_SetAgentCount, "Type_OutDial_SetAgentCount"));
            m_Id2Nme.insert(make_pair(Type_OutDial_DialCmd, "Type_OutDial_DialCmd"));
            m_Id2Nme.insert(make_pair(Type_OutDial_DialResult, "Type_OutDial_DialResult"));
            m_Id2Nme.insert(make_pair(Type_OutDial_AnalyzeResult, "Type_OutDial_AnalyzeResult"));
            m_Id2Nme.insert(make_pair(Type_OutDial_TransResult, "Type_OutDial_TransResult"));
            m_Id2Nme.insert(make_pair(Type_OutDial_EndTalk, "Type_OutDial_EndTalk"));
            m_Id2Nme.insert(make_pair(Type_OutDial_EndProcess, "Type_OutDial_EndProcess"));
            m_Id2Nme.insert(make_pair(Type_OutDial_GwLineSynchronize, "Type_OutDial_GwLineSynchronize"));
            m_Id2Nme.insert(make_pair(Type_OutDial_TaskStateCtrl, "Type_OutDial_TaskStateCtrl"));
            m_Id2Nme.insert(make_pair(Type_OutDial_SetMaxBotherRate, "Type_OutDial_SetMaxBotherRate"));
            m_Id2Nme.insert(make_pair(Type_Notify_ErrorOccurred, "Type_Notify_ErrorOccurred"));
            m_Id2Nme.insert(make_pair(Type_Client_SendStrMsg, "Type_Client_SendStrMsg"));
            m_Id2Nme.insert(make_pair(Type_ACD_SendStrMsg, "Type_ACD_SendStrMsg"));
            m_Id2Nme.insert(make_pair(Type_OutDial_SetForecastIntensity, "Type_OutDial_SetForecastIntensity"));
            m_Id2Nme.insert(make_pair(Type_OutDial_SetForecastIntensityConf, "Type_OutDial_SetForecastIntensityConf"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryIncrementFunc, "Type_Client_QueryIncrementFunc"));
            m_Id2Nme.insert(make_pair(Type_Login_QueryIncrementFuncConf, "Type_Login_QueryIncrementFuncConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_LoadDataConf, "Type_Gateway_LoadDataConf"));
            m_Id2Nme.insert(make_pair(Type_OCXHeart_Send, "Type_OCXHeart_Send"));
            m_Id2Nme.insert(make_pair(Type_OCXHeart_Resp, "Type_OCXHeart_Resp"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Logon, "Type_Autodetector_Logon"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_LogonACK, "Type_Autodetector_LogonACK"));
            m_Id2Nme.insert(make_pair(Type_Client_ReturnToAgent, "Type_Client_ReturnToAgent"));
            m_Id2Nme.insert(make_pair(Type_ACD_ReturnToAgent, "Type_ACD_ReturnToAgent"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Heartbeat, "Type_Autodetector_Heartbeat"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_HeartbeatACK, "Type_Autodetector_HeartbeatACK"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_ReportError, "Type_Autodetector_ReportError"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_RebootProc, "Type_Autodetector_RebootProc"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_RebootProcACK, "Type_Autodetector_RebootProcACK"));
            m_Id2Nme.insert(make_pair(Type_SMS_OperServerLogin, "Type_SMS_OperServerLogin"));
            m_Id2Nme.insert(make_pair(Type_SMS_OperServerLoginConf, "Type_SMS_OperServerLoginConf"));
            m_Id2Nme.insert(make_pair(Type_SMS_OperServerLogout, "Type_SMS_OperServerLogout"));
            m_Id2Nme.insert(make_pair(Type_SMS_OperServerLogoutConf, "Type_SMS_OperServerLogoutConf"));
            m_Id2Nme.insert(make_pair(Type_SMS_Recved, "Type_SMS_Recved"));
            m_Id2Nme.insert(make_pair(Type_SMS_RecvedConf, "Type_SMS_RecvedConf"));
            m_Id2Nme.insert(make_pair(Type_OCXService_SubTrans, "Type_OCXService_SubTrans"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_ReportCompStat, "Type_Autodetector_ReportCompStat"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_heartbeatFromComp, "Type_Autodetector_heartbeatFromComp"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Log, "Type_Autodetector_Log"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Subscribe, "Type_Autodetector_Subscribe"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Subscribe_Resp, "Type_Autodetector_Subscribe_Resp"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Subscribe_Data, "Type_Autodetector_Subscribe_Data"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Unsubscri, "Type_Autodetector_Unsubscri"));
            m_Id2Nme.insert(make_pair(Type_Autodetector_Unsubscri_Resp, "Type_Autodetector_Unsubscri_Resp"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_GetCallTarget, "Type_PhoneSearch_GetCallTarget"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_GetCallTargetConf, "Type_PhoneSearch_GetCallTargetConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetDeptLoginAgentNum, "Type_Gateway_GetDeptLoginAgentNum"));
            m_Id2Nme.insert(make_pair(Type_ACD_GetDeptLoginAgentNumConf, "Type_ACD_GetDeptLoginAgentNumConf"));
            m_Id2Nme.insert(make_pair(Type_Operator2ACD_SendStrMsg, "Type_Operator2ACD_SendStrMsg"));
            m_Id2Nme.insert(make_pair(Type_Operator2Login_SendStrMsg, "Type_Operator2Login_SendStrMsg"));
            m_Id2Nme.insert(make_pair(Type_Operator2Login_GetStrMsg, "Type_Operator2Login_GetStrMsg"));
            m_Id2Nme.insert(make_pair(Type_Login_GetStrMsgRes, "Type_Login_GetStrMsgRes"));
            m_Id2Nme.insert(make_pair(Type_Operator2Login_CancelStrMsg, "Type_Operator2Login_CancelStrMsg"));
            m_Id2Nme.insert(make_pair(Type_Login_SendStrMsg, "Type_Login_SendStrMsg"));
            m_Id2Nme.insert(make_pair(Type_Gateway_TrunkStat, "Type_Gateway_TrunkStat"));
            m_Id2Nme.insert(make_pair(Type_Gateway_TrunkSeizeRate, "Type_Gateway_TrunkSeizeRate"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallInStat, "Type_Gateway_CallInStat"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallOutStat, "Type_Gateway_CallOutStat"));
            m_Id2Nme.insert(make_pair(Type_Monitor_SrvLogin, "Type_Monitor_SrvLogin"));
            m_Id2Nme.insert(make_pair(Type_Monitor_SrvLoginConf, "Type_Monitor_SrvLoginConf"));
            m_Id2Nme.insert(make_pair(Type_Monitor_SrvLogout, "Type_Monitor_SrvLogout"));
            m_Id2Nme.insert(make_pair(Type_Monitor_SrvLogoutConf, "Type_Monitor_SrvLogoutConf"));
            m_Id2Nme.insert(make_pair(Type_Monitor_UpdateCliCount, "Type_Monitor_UpdateCliCount"));
            m_Id2Nme.insert(make_pair(Type_Client_NotifyAgentLogin, "Type_Client_NotifyAgentLogin"));
            m_Id2Nme.insert(make_pair(Type_Login_NotifyAgentLoginConf, "Type_Login_NotifyAgentLoginConf"));
            m_Id2Nme.insert(make_pair(Type_OutDial_HeartBeat, "Type_OutDial_HeartBeat"));
            m_Id2Nme.insert(make_pair(Type_OutDial_ToGw, "Type_OutDial_ToGw"));
            m_Id2Nme.insert(make_pair(Type_OutDial_FromGw, "Type_OutDial_FromGw"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentLogoutDeparts, "Type_Client_AgentLogoutDeparts"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentLogoutDepartsConf, "Type_ACD_AgentLogoutDepartsConf"));
            m_Id2Nme.insert(make_pair(Type_OutDial_ResultStorage, "Type_OutDial_ResultStorage"));
            m_Id2Nme.insert(make_pair(Type_OutDial_TaskStatus, "Type_OutDial_TaskStatus"));
            m_Id2Nme.insert(make_pair(Type_OutDial_TaskStatusConf, "Type_OutDial_TaskStatusConf"));
            m_Id2Nme.insert(make_pair(Type_Log, "Type_Log"));
            m_Id2Nme.insert(make_pair(Type_Client_WpAgentHanupCall, "Type_Client_WpAgentHanupCall"));
            m_Id2Nme.insert(make_pair(Type_UState_WpAgentHanupCall, "Type_UState_WpAgentHanupCall"));
            m_Id2Nme.insert(make_pair(Type_UState_WpAgentHanupCallConf, "Type_UState_WpAgentHanupCallConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_WpAgent_LineNo, "Type_Gateway_WpAgent_LineNo"));
            m_Id2Nme.insert(make_pair(Type_ACD_WpAgent_LineNo, "Type_ACD_WpAgent_LineNo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_WpAgentRelease, "Type_Gateway_WpAgentRelease"));
            m_Id2Nme.insert(make_pair(Type_Gateway_WebPSTNDialConf, "Type_Gateway_WebPSTNDialConf"));
            m_Id2Nme.insert(make_pair(Type_Client_WpAgentMakeCall, "Type_Client_WpAgentMakeCall"));
            m_Id2Nme.insert(make_pair(Type_UState_WpAgentMakeCall, "Type_UState_WpAgentMakeCall"));
            m_Id2Nme.insert(make_pair(Type_Gateway_WpAgentCallAnswerred, "Type_Gateway_WpAgentCallAnswerred"));
            m_Id2Nme.insert(make_pair(Type_ACD_ResetDataCollect, "Type_ACD_ResetDataCollect"));
            m_Id2Nme.insert(make_pair(Type_ACD_ResetMonitor, "Type_ACD_ResetMonitor"));
            m_Id2Nme.insert(make_pair(Type_AreaCode_AreaQuery, "Type_AreaCode_AreaQuery"));
            m_Id2Nme.insert(make_pair(Type_AreaCode_AreaAnswer, "Type_AreaCode_AreaAnswer"));
            m_Id2Nme.insert(make_pair(Type_OutDial_GwLogin, "Type_OutDial_GwLogin"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryNowVersion, "Type_Client_QueryNowVersion"));
            m_Id2Nme.insert(make_pair(Type_Client_UserReLoginStateSvr, "Type_Client_UserReLoginStateSvr"));
            m_Id2Nme.insert(make_pair(Type_AreaCode_QueryNowVersionConf, "Type_AreaCode_QueryNowVersionConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_IVRStat, "Type_Gateway_IVRStat"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_GetWaitingMusic, "Type_PhoneSearch_GetWaitingMusic"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_GetWaitingMusicConf, "Type_PhoneSearch_GetWaitingMusicConf"));
            m_Id2Nme.insert(make_pair(Type_alarm_alarmRequest, "Type_alarm_alarmRequest"));
            m_Id2Nme.insert(make_pair(Type_Gateway_CallInOutStat, "Type_Gateway_CallInOutStat"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentRecordQueryPort, "Type_Record_AgentRecordQueryPort"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentRecordQueryPortConf, "Type_Record_AgentRecordQueryPortConf"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentRecordStart, "Type_Record_AgentRecordStart"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentRecordStop, "Type_Record_AgentRecordStop"));
            m_Id2Nme.insert(make_pair(Type_ServerProxy_LoadServerInfo, "Type_ServerProxy_LoadServerInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_LoadServerInfoConf, "Type_SysManage_LoadServerInfoConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_SAMPLER_Query, "Type_ACD_SAMPLER_Query"));
            m_Id2Nme.insert(make_pair(Type_ACD_SAMPLER_Agreed, "Type_ACD_SAMPLER_Agreed"));
            m_Id2Nme.insert(make_pair(Type_SAMPLER_Query, "Type_SAMPLER_Query"));
            m_Id2Nme.insert(make_pair(Type_SAMPLER_Agreed, "Type_SAMPLER_Agreed"));
            m_Id2Nme.insert(make_pair(Type_SAMPLER_REPORT, "Type_SAMPLER_REPORT"));
            m_Id2Nme.insert(make_pair(Type_SAMPLER_NetDelay, "Type_SAMPLER_NetDelay"));
            m_Id2Nme.insert(make_pair(Type_OutDial_SetOutdialGroup, "Type_OutDial_SetOutdialGroup"));
            m_Id2Nme.insert(make_pair(Type_CallControl_GetTrunkGroupSetting, "Type_CallControl_GetTrunkGroupSetting"));
            m_Id2Nme.insert(make_pair(Type_CallControl_SetTrunkGroupSetting, "Type_CallControl_SetTrunkGroupSetting"));
            m_Id2Nme.insert(make_pair(Type_CallControl_KeepAlive, "Type_CallControl_KeepAlive"));
            m_Id2Nme.insert(make_pair(Type_CallControl_CallDetail, "Type_CallControl_CallDetail"));
            m_Id2Nme.insert(make_pair(Type_OutDial_ExitOutDial, "Type_OutDial_ExitOutDial"));
            m_Id2Nme.insert(make_pair(Type_OutDial_JoinOutDial, "Type_OutDial_JoinOutDial"));
            m_Id2Nme.insert(make_pair(Type_SysManage_MoveAgent, "Type_SysManage_MoveAgent"));
            m_Id2Nme.insert(make_pair(Type_ACD_SwitchDept, "Type_ACD_SwitchDept"));
            m_Id2Nme.insert(make_pair(Type_DIDControl_QueryGatewayConfig, "Type_DIDControl_QueryGatewayConfig"));
            m_Id2Nme.insert(make_pair(Type_DIDControl_Response, "Type_DIDControl_Response"));
            m_Id2Nme.insert(make_pair(Type_DIDControl_QueryDIDConfig, "Type_DIDControl_QueryDIDConfig"));
            m_Id2Nme.insert(make_pair(Type_Gateway_NotifyTrunkState, "Type_Gateway_NotifyTrunkState"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_HeartBeatRegister, "Type_OneKeySwitch_HeartBeatRegister"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetServerHeartBeatServer, "Type_OneKeySwitch_GetServerHeartBeatServer"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetServerHeartBeatServerConf, "Type_OneKeySwitch_GetServerHeartBeatServerConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetClientHeartBeatServer, "Type_OneKeySwitch_GetClientHeartBeatServer"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetClientHeartBeatServerConf, "Type_OneKeySwitch_GetClientHeartBeatServerConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServerHeartBeat, "Type_OneKeySwitch_ServerHeartBeat"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ClientHeartBeat, "Type_OneKeySwitch_ClientHeartBeat"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ClientHeartBeatConf, "Type_OneKeySwitch_ClientHeartBeatConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetGwList, "Type_OneKeySwitch_GetGwList"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetGwListConf, "Type_OneKeySwitch_GetGwListConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetAcdList, "Type_OneKeySwitch_GetAcdList"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetAcdListConf, "Type_OneKeySwitch_GetAcdListConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchCenter_Ctrl, "Type_OneKeySwitch_SwitchCenter_Ctrl"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchAcd_Ctrl, "Type_OneKeySwitch_SwitchAcd_Ctrl"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchRedirect_Ctrl, "Type_OneKeySwitch_SwitchRedirect_Ctrl"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchCenter, "Type_OneKeySwitch_SwitchCenter"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchAcd, "Type_OneKeySwitch_SwitchAcd"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchRedirect, "Type_OneKeySwitch_SwitchRedirect"));
            m_Id2Nme.insert(make_pair(Type_Record_ACK, "Type_Record_ACK"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_UserLoginStateSvrConf, "Type_OneKeySwitch_UserLoginStateSvrConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ConfirmGW, "Type_OneKeySwitch_ConfirmGW"));
            m_Id2Nme.insert(make_pair(Type_Gateway_ConfirmGWConf, "Type_Gateway_ConfirmGWConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_HeartBeatConfirm, "Type_Gateway_HeartBeatConfirm"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_HeartBeatConfirmConf, "Type_OneKeySwitch_HeartBeatConfirmConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GetHeartBeat, "Type_Gateway_GetHeartBeat"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetHeartBeatConf, "Type_OneKeySwitch_GetHeartBeatConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GWLogout, "Type_OneKeySwitch_GWLogout"));
            m_Id2Nme.insert(make_pair(Type_UState_GWLogoutConf, "Type_UState_GWLogoutConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GWLogoutConf, "Type_Gateway_GWLogoutConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GWMRegister, "Type_Gateway_GWMRegister"));
            m_Id2Nme.insert(make_pair(Type_UState_USRegister, "Type_UState_USRegister"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_RegisterConf, "Type_OneKeySwitch_RegisterConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GWLogin, "Type_OneKeySwitch_GWLogin"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_UserLogin, "Type_OneKeySwitch_UserLogin"));
            m_Id2Nme.insert(make_pair(Type_Client_GetUSnGWM, "Type_Client_GetUSnGWM"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetUSnGWMConf, "Type_OneKeySwitch_GetUSnGWMConf"));
            m_Id2Nme.insert(make_pair(Type_Client_UStateConfirm, "Type_Client_UStateConfirm"));
            m_Id2Nme.insert(make_pair(Type_Client_GWManageConfirm, "Type_Client_GWManageConfirm"));
            m_Id2Nme.insert(make_pair(Type_UState_ConfirmConf, "Type_UState_ConfirmConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_GWMConfirmConf, "Type_Gateway_GWMConfirmConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_LoadGWCorpInfo, "Type_Gateway_LoadGWCorpInfo"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_LoadGWInfo, "Type_OneKeySwitch_LoadGWInfo"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServerHeartBeatConf, "Type_OneKeySwitch_ServerHeartBeatConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_Synchronization, "Type_OneKeySwitch_Synchronization"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_USRegister, "Type_OneKeySwitch_USRegister"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GWMRegister, "Type_OneKeySwitch_GWMRegister"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GWLoginConf, "Type_OneKeySwitch_GWLoginConf"));
            m_Id2Nme.insert(make_pair(Type_Licence_Query, "Type_Licence_Query"));
            m_Id2Nme.insert(make_pair(Type_Licence_QueryConf, "Type_Licence_QueryConf"));
            m_Id2Nme.insert(make_pair(Type_Client_LoginOverrun, "Type_Client_LoginOverrun"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SetTrunkDeviecList, "Type_OneKeySwitch_SetTrunkDeviecList"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SetTrunkDeviecListConf, "Type_Gateway_SetTrunkDeviecListConf"));
            m_Id2Nme.insert(make_pair(Type_GCTI_MonitorDevice, "Type_GCTI_MonitorDevice"));
            m_Id2Nme.insert(make_pair(Type_GCTI_GCTI_AnswerCall, "Type_GCTI_GCTI_AnswerCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_GCTI_MakeCall, "Type_GCTI_GCTI_MakeCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ClearConnection, "Type_GCTI_ClearConnection"));
            m_Id2Nme.insert(make_pair(Type_GCTI_IVRReturnTransferToAgent, "Type_GCTI_IVRReturnTransferToAgent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_HoldCall, "Type_GCTI_HoldCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_MonitorConfEvent, "Type_GCTI_MonitorConfEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_MakeCallConfEvent, "Type_GCTI_MakeCallConfEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_UniversalFailureConfEvent, "Type_GCTI_UniversalFailureConfEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DeliveredEvent, "Type_GCTI_DeliveredEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_EstablishedEvent, "Type_GCTI_EstablishedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_NetworkReachedEvent, "Type_GCTI_NetworkReachedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ServiceInitiatedEvent, "Type_GCTI_ServiceInitiatedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_HeldEvent, "Type_GCTI_HeldEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_RetrievedEvent, "Type_GCTI_RetrievedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_TransferredEvent, "Type_GCTI_TransferredEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ConnectionClearedEvent, "Type_GCTI_ConnectionClearedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_FailedEvent, "Type_GCTI_FailedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_OriginatedEvent, "Type_GCTI_OriginatedEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ForceHangupEvent, "Type_GCTI_ForceHangupEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_SetAgentEvent, "Type_GCTI_SetAgentEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_TransferredToIVREvent, "Type_GCTI_TransferredToIVREvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ResetHoldCall, "Type_GCTI_ResetHoldCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaPlay, "Type_GCTI_DriverMediaPlay"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaPlayWithASR, "Type_GCTI_DriverMediaPlayWithASR"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaRecord, "Type_GCTI_DriverMediaRecord"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaSendDTMF, "Type_GCTI_DriverMediaSendDTMF"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaReceiveDTMF, "Type_GCTI_DriverMediaReceiveDTMF"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaSendFax, "Type_GCTI_DriverMediaSendFax"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaReceiveFax, "Type_GCTI_DriverMediaReceiveFax"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaCLRBuf, "Type_GCTI_DriverMediaCLRBuf"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverMediaStopWork, "Type_GCTI_DriverMediaStopWork"));
            m_Id2Nme.insert(make_pair(Type_GCTI_DriverSetCostStatus, "Type_GCTI_DriverSetCostStatus"));
            m_Id2Nme.insert(make_pair(Type_GCTI_TransferCall, "Type_GCTI_TransferCall"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverConnectConfEvent, "Type_Board_DriverConnectConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverDisconnectConfEvent, "Type_Board_DriverDisconnectConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverTrunkOffHookConfEvent, "Type_Board_DriverTrunkOffHookConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverTrunkOnHookConfEvent, "Type_Board_DriverTrunkOnHookConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverTrunkResetConfEvent, "Type_Board_DriverTrunkResetConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverConferenceDestroyConfEvent, "Type_Board_DriverConferenceDestroyConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverUniversalFailureConfEvent, "Type_Board_DriverUniversalFailureConfEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverReportTrunkStateEvent, "Type_Board_DriverReportTrunkStateEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverInBoundCallEvent, "Type_Board_DriverInBoundCallEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverUserHangupEvent, "Type_Board_DriverUserHangupEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverDialRingingEvent, "Type_Board_DriverDialRingingEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverDialSuccessEvent, "Type_Board_DriverDialSuccessEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverDialSuccessTollEvent, "Type_Board_DriverDialSuccessTollEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverTrunkSeizedEvent, "Type_Board_DriverTrunkSeizedEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverFailedEvent, "Type_Board_DriverFailedEvent"));
            m_Id2Nme.insert(make_pair(Type_Board_DriverMediaEvent, "Type_Board_DriverMediaEvent"));
            m_Id2Nme.insert(make_pair(Type_GCTI_AgentTransferToIVR, "Type_GCTI_AgentTransferToIVR"));
            m_Id2Nme.insert(make_pair(Type_GCTI_AlternateCall, "Type_GCTI_AlternateCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ClearCall, "Type_GCTI_ClearCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_ReconnectCall, "Type_GCTI_ReconnectCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_RejectCall, "Type_GCTI_RejectCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_RetrieveCall, "Type_GCTI_RetrieveCall"));
            m_Id2Nme.insert(make_pair(Type_GCTI_MonitorStop, "Type_GCTI_MonitorStop"));
            m_Id2Nme.insert(make_pair(Type_GCTI_SnapshotDevice, "Type_GCTI_SnapshotDevice"));
            m_Id2Nme.insert(make_pair(Type_Inner_AlertingRequest, "Type_Inner_AlertingRequest"));
            m_Id2Nme.insert(make_pair(Type_Inner_MakeCallRequest, "Type_Inner_MakeCallRequest"));
            m_Id2Nme.insert(make_pair(Type_Inner_TimerEvent, "Type_Inner_TimerEvent"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverConnect, "Type_Bord_DriverConnect"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverDisconnect, "Type_Bord_DriverDisconnect"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverDial, "Type_Bord_DriverDial"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverTrunkOffHook, "Type_Bord_DriverTrunkOffHook"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverTrunkOnHook, "Type_Bord_DriverTrunkOnHook"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverTrunkReset, "Type_Bord_DriverTrunkReset"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverEchoRemoveCtrl, "Type_Bord_DriverEchoRemoveCtrl"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverAddRecordForTH, "Type_Bord_DriverAddRecordForTH"));
            m_Id2Nme.insert(make_pair(Type_Bord_DriverDelRecordForTH, "Type_Bord_DriverDelRecordForTH"));
            m_Id2Nme.insert(make_pair(Type_GCTI_Meaage, "Type_GCTI_Meaage"));
            m_Id2Nme.insert(make_pair(Type_Session_Meaage, "Type_Session_Meaage"));
            m_Id2Nme.insert(make_pair(Type_SysManage_SwitchAcdConf, "Type_SysManage_SwitchAcdConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddTDDistributeServer, "Type_SysManage_AddTDDistributeServer"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddGWtoTDDS, "Type_SysManage_AddGWtoTDDS"));
            m_Id2Nme.insert(make_pair(Type_TrunkData_DSLoadInfo, "Type_TrunkData_DSLoadInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddTDCollectServer, "Type_SysManage_AddTDCollectServer"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddComptoTDCS, "Type_SysManage_AddComptoTDCS"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddGWtoTDCS, "Type_SysManage_AddGWtoTDCS"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddComptoConnNo, "Type_SysManage_AddComptoConnNo"));
            m_Id2Nme.insert(make_pair(Type_TrunkData_NotifyTrunkState, "Type_TrunkData_NotifyTrunkState"));
            m_Id2Nme.insert(make_pair(Type_OutDial_CheckAlive, "Type_OutDial_CheckAlive"));
            m_Id2Nme.insert(make_pair(Type_OutDial_CheckAliveConf, "Type_OutDial_CheckAliveConf"));
            m_Id2Nme.insert(make_pair(Type_Timer_Synchronization, "Type_Timer_Synchronization"));
            m_Id2Nme.insert(make_pair(Type_Timer_SynchronizationConf, "Type_Timer_SynchronizationConf"));
            m_Id2Nme.insert(make_pair(Type_Client_ConferenceCall, "Type_Client_ConferenceCall"));
            m_Id2Nme.insert(make_pair(Type_Client_ConferenceCallConf, "Type_Client_ConferenceCallConf"));
            m_Id2Nme.insert(make_pair(Type_Client_ConferenceCallEnd, "Type_Client_ConferenceCallEnd"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServiceRegister, "Type_OneKeySwitch_ServiceRegister"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServiceRegisterConf, "Type_OneKeySwitch_ServiceRegisterConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServiceConfirm, "Type_OneKeySwitch_ServiceConfirm"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServiceConfirmConf, "Type_OneKeySwitch_ServiceConfirmConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetHeartBeat, "Type_OneKeySwitch_GetHeartBeat"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetService, "Type_OneKeySwitch_GetService"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetServiceConf, "Type_OneKeySwitch_GetServiceConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServiceTimeout, "Type_OneKeySwitch_ServiceTimeout"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SynchronizationRegister, "Type_OneKeySwitch_SynchronizationRegister"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SynchronizationRegisterConf, "Type_OneKeySwitch_SynchronizationRegisterConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_LoadRegisterInfo, "Type_OneKeySwitch_LoadRegisterInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddRegisterInfo, "Type_SysManage_AddRegisterInfo"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_AddRegisterInfoConf, "Type_OneKeySwitch_AddRegisterInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Client_ConferenceStart, "Type_Client_ConferenceStart"));
            m_Id2Nme.insert(make_pair(Type_ACD_ConferenceStart, "Type_ACD_ConferenceStart"));
            m_Id2Nme.insert(make_pair(Type_UState_ConferenceStart, "Type_UState_ConferenceStart"));
            m_Id2Nme.insert(make_pair(Type_Client_ConferenceEnd, "Type_Client_ConferenceEnd"));
            m_Id2Nme.insert(make_pair(Type_ACD_ConferenceEnd, "Type_ACD_ConferenceEnd"));
            m_Id2Nme.insert(make_pair(Type_UState_ConferenceEnd, "Type_UState_ConferenceEnd"));
            m_Id2Nme.insert(make_pair(Type_SysManage_GetServerList, "Type_SysManage_GetServerList"));
            m_Id2Nme.insert(make_pair(Type_SysManage_GetServerListConf, "Type_SysManage_GetServerListConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_HotAddService, "Type_OneKeySwitch_HotAddService"));
            m_Id2Nme.insert(make_pair(Type_CID_DialCmd, "Type_CID_DialCmd"));
            m_Id2Nme.insert(make_pair(Type_CID_Result, "Type_CID_Result"));
            m_Id2Nme.insert(make_pair(Type_Gateway_ConferenceStartConf, "Type_Gateway_ConferenceStartConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_ConferenceStartConf, "Type_ACD_ConferenceStartConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_UserLeaveConference, "Type_Gateway_UserLeaveConference"));
            m_Id2Nme.insert(make_pair(Type_ACD_UserLeaveConference, "Type_ACD_UserLeaveConference"));
            m_Id2Nme.insert(make_pair(Type_Record_StartConferenceRecord, "Type_Record_StartConferenceRecord"));
            m_Id2Nme.insert(make_pair(Type_Record_StopConferenceRecord, "Type_Record_StopConferenceRecord"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GWLogOutConf, "Type_OneKeySwitch_GWLogOutConf"));
            m_Id2Nme.insert(make_pair(Type_CID_CheckAlive, "Type_CID_CheckAlive"));
            m_Id2Nme.insert(make_pair(Type_CID_CheckAliveConf, "Type_CID_CheckAliveConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchService_Ctrl, "Type_OneKeySwitch_SwitchService_Ctrl"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchService_CtrlConf, "Type_OneKeySwitch_SwitchService_CtrlConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchService, "Type_OneKeySwitch_SwitchService"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SwitchServiceConf, "Type_OneKeySwitch_SwitchServiceConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_SubscribeTGStatus, "Type_Gateway_SubscribeTGStatus"));
            m_Id2Nme.insert(make_pair(Type_TrunkData_NotifyTGStatus, "Type_TrunkData_NotifyTGStatus"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddDeptTGRoute, "Type_SysManage_AddDeptTGRoute"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelDeptTGRoute, "Type_SysManage_DelDeptTGRoute"));
            m_Id2Nme.insert(make_pair(Type_UnAnsweredAgentCall, "Type_UnAnsweredAgentCall"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_SwitchPort, "Type_DataTrans_SwitchPort"));
            m_Id2Nme.insert(make_pair(Type_DataTrans_SwitchPortConf, "Type_DataTrans_SwitchPortConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddGWIADG, "Type_SysManage_AddGWIADG"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelGWIADG, "Type_SysManage_DelGWIADG"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddIAD, "Type_SysManage_AddIAD"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelIAD, "Type_SysManage_DelIAD"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SynchronizationTimeout, "Type_OneKeySwitch_SynchronizationTimeout"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SynchronizationTimeoutConf, "Type_OneKeySwitch_SynchronizationTimeoutConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddSkillGroup, "Type_SysManage_AddSkillGroup"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelSkillGroup, "Type_SysManage_DelSkillGroup"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SyncHeatBeatRelation, "Type_OneKeySwitch_SyncHeatBeatRelation"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SyncHeatBeatRelationConfirm, "Type_OneKeySwitch_SyncHeatBeatRelationConfirm"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_SyncHeatBeatRelationConfirmconf, "Type_OneKeySwitch_SyncHeatBeatRelationConfirmconf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetServerHeartBeatServerInfo, "Type_OneKeySwitch_GetServerHeartBeatServerInfo"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetServerHeartBeatServerInfoConf, "Type_OneKeySwitch_GetServerHeartBeatServerInfoConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddBanCall, "Type_SysManage_AddBanCall"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelBanCall, "Type_SysManage_DelBanCall"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddAreaBan, "Type_SysManage_AddAreaBan"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelAreaBan, "Type_SysManage_DelAreaBan"));
            m_Id2Nme.insert(make_pair(Type_ClientCapture_SessionEnd, "Type_ClientCapture_SessionEnd"));
            m_Id2Nme.insert(make_pair(Type_ClientCapture_SRSend, "Type_ClientCapture_SRSend"));
            m_Id2Nme.insert(make_pair(Type_ClientCapture_RRSend, "Type_ClientCapture_RRSend"));
            m_Id2Nme.insert(make_pair(Type_ClientCapture_GetRtpStatistics, "Type_ClientCapture_GetRtpStatistics"));
            m_Id2Nme.insert(make_pair(Type_WebService_QueryCalloutInfo, "Type_WebService_QueryCalloutInfo"));
            m_Id2Nme.insert(make_pair(Type_WebService_QueryCalloutInfoConf, "Type_WebService_QueryCalloutInfoConf"));
            m_Id2Nme.insert(make_pair(Type_WebService_HangupCall, "Type_WebService_HangupCall"));
            m_Id2Nme.insert(make_pair(Type_WebService_RecordInfo, "Type_WebService_RecordInfo"));
            m_Id2Nme.insert(make_pair(Type_WebService_DialingCall, "Type_WebService_DialingCall"));
            m_Id2Nme.insert(make_pair(Type_WebService_RingingCall, "Type_WebService_RingingCall"));
            m_Id2Nme.insert(make_pair(Type_WebService_TalkingCall, "Type_WebService_TalkingCall"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddRecInfo, "Type_SysManage_AddRecInfo"));
            m_Id2Nme.insert(make_pair(Type_SysManage_DelRecInfo, "Type_SysManage_DelRecInfo"));
            m_Id2Nme.insert(make_pair(Type_Gateway_InsertDateOperation, "Type_Gateway_InsertDateOperation"));
            m_Id2Nme.insert(make_pair(Type_VoiceRecognition_RequestRecognition, "Type_VoiceRecognition_RequestRecognition"));
            m_Id2Nme.insert(make_pair(Type_VoiceRecognition_ResponseRecognition, "Type_VoiceRecognition_ResponseRecognition"));
            m_Id2Nme.insert(make_pair(Type_WebService_InterfaceToMessage, "Type_WebService_InterfaceToMessage"));
            m_Id2Nme.insert(make_pair(Type_WebService_InterfaceToMessageConf, "Type_WebService_InterfaceToMessageConf"));
            m_Id2Nme.insert(make_pair(Type_WebService_RequestService, "Type_WebService_RequestService"));
            m_Id2Nme.insert(make_pair(Type_WebService_RequestServiceConf, "Type_WebService_RequestServiceConf"));
            m_Id2Nme.insert(make_pair(Type_WebService_HeartBeat, "Type_WebService_HeartBeat"));
            m_Id2Nme.insert(make_pair(Type_WebService_HeartBeatConf, "Type_WebService_HeartBeatConf"));
            m_Id2Nme.insert(make_pair(Type_voiceRecognition_AudioPacket, "Type_voiceRecognition_AudioPacket"));
            m_Id2Nme.insert(make_pair(Type_ACD_SMSInfo, "Type_ACD_SMSInfo"));
            m_Id2Nme.insert(make_pair(Type_PhoneSearch_SMSInfoConf, "Type_PhoneSearch_SMSInfoConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_UnAnsweredCallConf, "Type_Gateway_UnAnsweredCallConf"));
            m_Id2Nme.insert(make_pair(Type_UnAnsweredAgentCallConf, "Type_UnAnsweredAgentCallConf"));
            m_Id2Nme.insert(make_pair(Type_Gateway_InsertDateOperationConf, "Type_Gateway_InsertDateOperationConf"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentTestStartRecord, "Type_Record_AgentTestStartRecord"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentTestStartRecordConf, "Type_Record_AgentTestStartRecordConf"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentTestStartPlayback, "Type_Record_AgentTestStartPlayback"));
            m_Id2Nme.insert(make_pair(Type_Record_AgentTestStartPlaybackConf, "Type_Record_AgentTestStartPlaybackConf"));
            m_Id2Nme.insert(make_pair(Type_Client_SendErrCodeInfo, "Type_Client_SendErrCodeInfo"));
            m_Id2Nme.insert(make_pair(Type_ACD_RefreshSkillGroupConf, "Type_ACD_RefreshSkillGroupConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_NotifyACDLicenceConf, "Type_ACD_NotifyACDLicenceConf"));
            m_Id2Nme.insert(make_pair(Type_Guide_RefreshKeywordConf, "Type_Guide_RefreshKeywordConf"));
            m_Id2Nme.insert(make_pair(Type_ServerProxy_LoadServerInfoConfConf, "Type_ServerProxy_LoadServerInfoConfConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_RefreshGatewayInfoConf, "Type_OneKeySwitch_RefreshGatewayInfoConf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_RefreshRegisterInfoConf, "Type_OneKeySwitch_RefreshRegisterInfoConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_LoadGWCorpInfoConf, "Type_SysManage_LoadGWCorpInfoConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_LoadConfigInfoConf, "Type_SysManage_LoadConfigInfoConf"));
            m_Id2Nme.insert(make_pair(Type_File_Register, "Type_File_Register"));
            m_Id2Nme.insert(make_pair(Type_File_SendData, "Type_File_SendData"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentToken, "Type_Client_AgentToken"));
            m_Id2Nme.insert(make_pair(Type_File_Request, "Type_File_Request"));
            m_Id2Nme.insert(make_pair(Type_File_Respond, "Type_File_Respond"));
            m_Id2Nme.insert(make_pair(Type_TTS_Request, "Type_TTS_Request"));
            m_Id2Nme.insert(make_pair(Type_TTS_Result, "Type_TTS_Result"));
            m_Id2Nme.insert(make_pair(Type_batExtRec_searchRecByInvokeId, "Type_batExtRec_searchRecByInvokeId"));
            m_Id2Nme.insert(make_pair(Type_batExtRec_foundByInvokeId, "Type_batExtRec_foundByInvokeId"));
            m_Id2Nme.insert(make_pair(Type_TTS_Cancel, "Type_TTS_Cancel"));
            m_Id2Nme.insert(make_pair(Type_batExtRec_transferFileToMe, "Type_batExtRec_transferFileToMe"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_GetServerHeartBeatServerInfoConf, "Type_OneKeySwitch_GetServerHeartBeatServerInfoConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AgentInfoChange, "Type_SysManage_AgentInfoChange"));
            m_Id2Nme.insert(make_pair(Type_Redirect_GetService, "Type_Redirect_GetService"));
            m_Id2Nme.insert(make_pair(Type_Redirect_GetServiceConf, "Type_Redirect_GetServiceConf"));
            m_Id2Nme.insert(make_pair(Type_CClient_Login, "Type_CClient_Login"));
            m_Id2Nme.insert(make_pair(Type_Login_CCLoginConf, "Type_Login_CCLoginConf"));
            m_Id2Nme.insert(make_pair(Type_CClient_HeartBeat, "Type_CClient_HeartBeat"));
            m_Id2Nme.insert(make_pair(Type_CProxy_HeartBeatConf, "Type_CProxy_HeartBeatConf"));
            m_Id2Nme.insert(make_pair(Type_CClient_CMakeCall, "Type_CClient_CMakeCall"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CMakeCallConf, "Type_CProxy_CMakeCallConf"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CMakeCall, "Type_CProxy_CMakeCall"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CallDelivered, "Type_CProxy_CallDelivered"));
            m_Id2Nme.insert(make_pair(Type_CClient_CallArrived, "Type_CClient_CallArrived"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CallArrived, "Type_CProxy_CallArrived"));
            m_Id2Nme.insert(make_pair(Type_CClient_CallAnswered, "Type_CClient_CallAnswered"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CallAnswered, "Type_CProxy_CallAnswered"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CallStart, "Type_CProxy_CallStart"));
            m_Id2Nme.insert(make_pair(Type_CProxy_QueryGW, "Type_CProxy_QueryGW"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CMakeCall2, "Type_CProxy_CMakeCall2"));
            m_Id2Nme.insert(make_pair(Type_SysManage_RefreshRedis, "Type_SysManage_RefreshRedis"));
            m_Id2Nme.insert(make_pair(Type_SysManage_RefreshRedisConf, "Type_SysManage_RefreshRedisConf"));
            m_Id2Nme.insert(make_pair(Type_RouteSelect_RouteSelect, "Type_RouteSelect_RouteSelect"));
            m_Id2Nme.insert(make_pair(Type_RouteSelect_RouteSelectConf, "Type_RouteSelect_RouteSelectConf"));
            m_Id2Nme.insert(make_pair(Type_RouteSelect_RouteRequest, "Type_RouteSelect_RouteRequest"));
            m_Id2Nme.insert(make_pair(Type_RouteSelect_RouteRequestConf, "Type_RouteSelect_RouteRequestConf"));
            m_Id2Nme.insert(make_pair(Type_RouteSelect_RouteStop, "Type_RouteSelect_RouteStop"));
            m_Id2Nme.insert(make_pair(Type_RouteSelect_RouteStopConf, "Type_RouteSelect_RouteStopConf"));
            m_Id2Nme.insert(make_pair(Type_CClient_HangupCall, "Type_CClient_HangupCall"));
            m_Id2Nme.insert(make_pair(Type_CProxy_HangupCall, "Type_CProxy_HangupCall"));
            m_Id2Nme.insert(make_pair(Type_CClient_HangupCallConf, "Type_CClient_HangupCallConf"));
            m_Id2Nme.insert(make_pair(Type_CProxy_HangupCallConf, "Type_CProxy_HangupCallConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_CHangupCallConf, "Type_ACD_CHangupCallConf"));
            m_Id2Nme.insert(make_pair(Type_CProxy_CallHanguped, "Type_CProxy_CallHanguped"));
            m_Id2Nme.insert(make_pair(Type_CClient_ToCProxyHeartBeat, "Type_CClient_ToCProxyHeartBeat"));
            m_Id2Nme.insert(make_pair(Type_FileTransfer, "Type_FileTransfer"));
            m_Id2Nme.insert(make_pair(Type_FileTransfer_UploadRequest, "Type_FileTransfer_UploadRequest"));
            m_Id2Nme.insert(make_pair(Type_FileTransfer_StartUpload, "Type_FileTransfer_StartUpload"));
            m_Id2Nme.insert(make_pair(Type_FileTransfer_UploadCompleted, "Type_FileTransfer_UploadCompleted"));
            m_Id2Nme.insert(make_pair(Type_FileTransfer_FileData, "Type_FileTransfer_FileData"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_NotifyMsg, "Type_OneKeySwitch_NotifyMsg"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_NotifyMsgConf, "Type_OneKeySwitch_NotifyMsgConf"));
            m_Id2Nme.insert(make_pair(Type_Client_RunTimeInfo, "Type_Client_RunTimeInfo"));
            m_Id2Nme.insert(make_pair(Type_Client_MessageCallInfo, "Type_Client_MessageCallInfo"));
            m_Id2Nme.insert(make_pair(Type_CProxy_Login, "Type_CProxy_Login"));
            m_Id2Nme.insert(make_pair(Type_CProxy_LoginConf, "Type_CProxy_LoginConf"));
            m_Id2Nme.insert(make_pair(Type_CProxy_Logout, "Type_CProxy_Logout"));
            m_Id2Nme.insert(make_pair(Type_Client_NotifyAgentLogin2, "Type_Client_NotifyAgentLogin2"));
            m_Id2Nme.insert(make_pair(Type_Login_NotifyAgentLogin2Conf, "Type_Login_NotifyAgentLogin2Conf"));
            m_Id2Nme.insert(make_pair(Type_OneKeySwitch_ServiceUnavailable, "Type_OneKeySwitch_ServiceUnavailable"));
            m_Id2Nme.insert(make_pair(Type_ACD_LoadNotify, "Type_ACD_LoadNotify"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryAgentsState, "Type_Client_QueryAgentsState"));
            m_Id2Nme.insert(make_pair(Type_Client_QueryAgentsStateConf, "Type_Client_QueryAgentsStateConf"));
            m_Id2Nme.insert(make_pair(Type_Client_WebPSTNHangup_Temp, "Type_Client_WebPSTNHangup_Temp"));
            m_Id2Nme.insert(make_pair(Type_Redirect_AppServerList, "Type_Redirect_AppServerList"));
            m_Id2Nme.insert(make_pair(Type_Redirect_AppServerListConf, "Type_Redirect_AppServerListConf"));
            m_Id2Nme.insert(make_pair(Type_ACD_UniversalFailure, "Type_ACD_UniversalFailure"));
            m_Id2Nme.insert(make_pair(Type_Client_AgentSwitchToFrontDesk, "Type_Client_AgentSwitchToFrontDesk"));
            m_Id2Nme.insert(make_pair(Type_ACD_AgentSwitchToFrontDeskConf, "Type_ACD_AgentSwitchToFrontDeskConf"));
            m_Id2Nme.insert(make_pair(Type_SysManage_AddDeptToService, "Type_SysManage_AddDeptToService"));
            m_Id2Nme.insert(make_pair(Type_MakeCallViaClient, "Type_MakeCallViaClient"));
        }

    public:
        map<int, string> m_Id2Nme;
    };

    static CMsgTypes msg_types;

    return id_to_name(msg_types.m_Id2Nme, (int)t);
}

string ipccmsg_type_to_str(int i)
{
	return to_str(__EIPCCMsgType(i));
}

string ipccmsg_type_ocx_to_str( int i )
{
    static id_name msg_types_ocx[] = 
    {
        {dispidActionLogon                                ,"dispidActionLogon"},      
        {dispidActionLogon2                               ,"dispidActionLogon2"},      
        {dispidActionReset                                ,"dispidActionReset"},      
        {dispidActionLogout                               ,"dispidActionLogout"},      
        {dispidActionLogonAgain                           ,"dispidActionLogonAgain"},      
        {dispidActionCallDepartment                       ,"dispidActionCallDepartment"},      
        {dispidActionCallAgent                            ,"dispidActionCallAgent"},      
        {dispidActionCallOtherAgent                       ,"dispidActionCallOtherAgent"},      
        {dispidActionCallPstn                             ,"dispidActionCallPstn"},      
        {dispidActionCallExtNum                           ,"dispidActionCallExtNum"},      
        {dispidActionCallOtherDepartment                  ,"dispidActionCallOtherDepartment"},      
        {dispidActionOnHook                               ,"dispidActionOnHook"},      
        {dispidActionOffHook                              ,"dispidActionOffHook"},      
        {dispidActionPark                                 ,"dispidActionPark"},      
        {dispidActionUnPark                               ,"dispidActionUnPark"},      
        {dispidActionProcess                              ,"dispidActionProcess"},      
        {dispidActionUnProcess                            ,"dispidActionUnProcess"},      
        {dispidActionHold                                 ,"dispidActionHold"},      
        {dispidActionUnHold                               ,"dispidActionUnHold"},      
        {dispidActionTransfer                             ,"dispidActionTransfer"},      
        {dispidActionAutoTransfer                         ,"dispidActionAutoTransfer"},      
        {dispidActionStartMonit                           ,"dispidActionStartMonit"},      
        {dispidActionStopMonit                            ,"dispidActionStopMonit"},      
        {dispidActionForceDisconnect                      ,"dispidActionForceDisconnect"},      
        {dispidActionGetFullFreeAgentInfo                 ,"dispidActionGetFullFreeAgentInfo"},      
        {dispidActionGetGroupInfo                         ,"dispidActionGetGroupInfo"},      
        {dispidActionGetOneGroupFreeAgentInfo             ,"dispidActionGetOneGroupFreeAgentInfo"},      
        {dispidActionGetTalkingAgent                      ,"dispidActionGetTalkingAgent"},      
        {dispidActionSendDTMF                             ,"dispidActionSendDTMF"},      
        {dispidActionChangePassword                       ,"dispidActionChangePassword"},      
        {dispidActionChangeEarphoneVolume                 ,"dispidActionChangeEarphoneVolume"},      
        {dispidActionChangeMicVolume                      ,"dispidActionChangeMicVolume"},      
        {dispidActionSendSMS                              ,"dispidActionSendSMS"},      
        {dispidActionSendChat                             ,"dispidActionSendChat"},      
        {dispidConfigPortSettings                         ,"dispidConfigPortSettings"},      
        {dispidConfigRecSettings                          ,"dispidConfigRecSettings"},      
        {dispidConfigOutDialSettings                      ,"dispidConfigOutDialSettings"},      
        {dispidConfigSoundSettings                        ,"dispidConfigSoundSettings"},      
        {dispidGetLoginManner                             ,"dispidGetLoginManner"},      
        {dispidGetDeptment                                ,"dispidGetDeptment"},      
        {dispidGetCorp                                    ,"dispidGetCorp"},      
        {dispidGetCallType                                ,"dispidGetCallType"},      
        {dispidGetCallFrom                                ,"dispidGetCallFrom"},      
        {dispidGetPrivateData                             ,"dispidGetPrivateData"},      
        {dispidGetRecInvokeID                             ,"dispidGetRecInvokeID"},      
        {dispidGetClientState                             ,"dispidGetClientState"},      
        {dispidGetLogonType                               ,"dispidGetLogonType"},      
        {dispidGetCallPartyType                           ,"dispidGetCallPartyType"},      
        {dispidGetAgentHasLogon                           ,"dispidGetAgentHasLogon"},      
        {dispidInitialize                                 ,"dispidInitialize"},      
        {dispidExit                                       ,"dispidExit"},      
        {dispidGetRoleInCompany                           ,"dispidGetRoleInCompany"},      
        {dispidGetAgentAccount                            ,"dispidGetAgentAccount"},      
        {dispidGetIsCCNUser                               ,"dispidGetIsCCNUser"},      
        {dispidGetAgentType                               ,"dispidGetAgentType"},      
        {dispidGetIsSMSEnabled                            ,"dispidGetIsSMSEnabled"},      
        {dispidGetAgentExtNum                             ,"dispidGetAgentExtNum"},      
        {dispidGetAgentPhoneNo                            ,"dispidGetAgentPhoneNo"},      
        {dispidGetManagerID                               ,"dispidGetManagerID"},      
        {dispidGetIsSuperAgent                            ,"dispidGetIsSuperAgent"},      
        {dispidGetIsMasterSuperAgent                      ,"dispidGetIsMasterSuperAgent"},      
        {dispidActionSendStrMsg                           ,"dispidActionSendStrMsg"},      
        {dispidGetRemindTime                              ,"dispidGetRemindTime"},      
        {dispidActionSettings                             ,"dispidActionSettings"},      
        {dispidActionMonit                                ,"dispidActionMonit"},      
        {dispidActionAgentOutDial                         ,"dispidActionAgentOutDial"},      
        {dispidGetIsMonit                                 ,"dispidGetIsMonit"},      
        {dispidGetSMSParamsInfo                           ,"dispidGetSMSParamsInfo"},      
        {dispidGetAllSettings                             ,"dispidGetAllSettings"},      
        {dispidGetBriefStatisticInfo                      ,"dispidGetBriefStatisticInfo"},      
        {dispidActionSaveBriefStatisticInfo               ,"dispidActionSaveBriefStatisticInfo"},      
        {dispidActionSetMuteMode                          ,"dispidActionSetMuteMode"},      
        {dispidActionTransferToIVR                        ,"dispidActionTransferToIVR"},      
        {dispidGetTransferToIVRParamsInfo                 ,"dispidGetTransferToIVRParamsInfo"},      
        {dispidGetIsCallFromACD                           ,"dispidGetIsCallFromACD"},      
        {dispidActionUpdateSettings                       ,"dispidActionUpdateSettings"},      
        {dispidGetIsOutDialOpenURL                        ,"dispidGetIsOutDialOpenURL"},      
        {dispidGetIVRReturn                               ,"dispidGetIVRReturn"},      
        {dispidSetProductVersion                          ,"dispidSetProductVersion"},      
        {dispidGetRecInvokePath                           ,"dispidGetRecInvokePath"},      
        {dispidGetAgentID                                 ,"dispidGetAgentID"},      
        {dispidSetOriginalData                            ,"dispidSetOriginalData"},      
        {dispidActionOutDial                              ,"dispidActionOutDial"},      
        {dispidSetSelectGroup                             ,"dispidSetSelectGroup"},      
        {dispidActionAutoTransferEx                       ,"dispidActionAutoTransferEx"},      
        {dispidActionOutDialWithRecSetting                ,"dispidActionOutDialWithRecSetting"},      
        {dispidActionOffHookWithRecSetting                ,"dispidActionOffHookWithRecSetting"},      
        {dispidGetIsCallInOpenURL                         ,"dispidGetIsCallInOpenURL"},      
        {dispidGetParkReasons                             ,"dispidGetParkReasons"},      
        {dispidSetParkReason                              ,"dispidSetParkReason"},      
        {dispidSetCallerSendingInDialing                  ,"dispidSetCallerSendingInDialing"},      
        {dispidActionJoinOutDialMode                      ,"dispidActionJoinOutDialMode"},      
        {dispidGetIsOutDialMode                           ,"dispidGetIsOutDialMode"},      
        {dispidGetMoudleFilePath                          ,"dispidGetMoudleFilePath"},      
        {dispidActionSetMuteModeEx                        ,"dispidActionSetMuteModeEx"},      
        {dispidActionStartConference                      ,"dispidActionStartConference"},      
        {dispidActionEndConference                        ,"dispidActionEndConference"},      
        {dispidGetCanConference                           ,"dispidGetCanConference"},      
        {dispidStopPlayAgentID                            ,"dispidStopPlayAgentID"},      
        {dispidActionACDAutoLogin                         ,"dispidActionACDAutoLogin"},      
    };
    return id_to_name(msg_types_ocx, ARRAY_LEN(msg_types_ocx), i);
}

string ocx_eventid_to_str( int eventid )
{
    static id_name ocx_eventids[] = 
    {
	    {eventidOnLogin,                       "eventidOnLogin"                   },
	    {eventidOnAgentLoginSucc,              "eventidOnAgentLoginSucc"          },
	    {eventidOnAgentLogoutConf,             "eventidOnAgentLogoutConf"         },
	    {eventidOnAgentMakeCallConf,           "eventidOnAgentMakeCallConf"       },
	    {eventidOnClientStateChange,           "eventidOnClientStateChange"       },
	    {eventidOnCallDelivered,               "eventidOnCallDelivered"           },
	    {eventidOnSystemStatusInfo,            "eventidOnSystemStatusInfo"        },
	    {eventidOnChatMsgDelivered,            "eventidOnChatMsgDelivered"        },
	    {eventidOnQueueSize,                   "eventidOnQueueSize"               },
	    {eventidOnGetGroupInfo,                "eventidOnGetGroupInfo"            },
	    {eventidOnGetOneGroupFreeAgentInfo,    "eventidOnGetOneGroupFreeAgentInfo"},
	    {eventidOnGetFullFreeAgentInfo,        "eventidOnGetFullFreeAgentInfo"    },
	    {eventidOnGetFullTalkingAgentInfo,     "eventidOnGetFullTalkingAgentInfo" },
	    {eventidOnAudioLostRate,               "eventidOnAudioLostRate"           },
	    {eventidOnAgentLoginFailed,            "eventidOnAgentLoginFailed"        },
	    {eventidOnServiceStateChange,          "eventidOnServiceStateChange"      },
	    {eventidOnPreOutDialEnd,               "eventidOnPreOutDialEnd"           },
	    {eventidOnUserLoginInfo,               "eventidOnUserLoginInfo"           },
	    {eventidOnAgentOutdialConf,            "eventidOnAgentOutdialConf"        },
	    {eventidOnACDSendStrMsg,               "eventidOnACDSendStrMsg"           },
	    {eventidOnLoginSendStrMsg,             "eventidOnLoginSendStrMsg"         },
	    {eventidOnIVRReturnToAgentConf,        "eventidOnIVRReturnToAgentConf"    },
	    {eventidOnNotifyAgentLoginConf,        "eventidOnNotifyAgentLoginConf"    },
    };

    return id_to_name(ocx_eventids, ARRAY_LEN(ocx_eventids), eventid);
}

string to_str(ESeverType t)
{
	static id_name service_types[] =
	{
		{ EST_ACDServer, "EST_ACDServer" },
		{ EST_ACDProxyServer, "EST_ACDProxyServer" },
		{ EST_UStateServer, "EST_UStateServer" },
		{ EST_GWManageServer, "EST_GWManageServer" },
		{ EST_LoginServer, "EST_LoginServer" },
		{ EST_TollServer, "EST_TollServer" },
		{ EST_DataCollectServe, "EST_DataCollectServe" },
		{ EST_EnterpriseTrunkDataCollectServer, "EST_EnterpriseTrunkDataCollectServer" },
		{ EST_GatewayTrunkDataCollectServer, "EST_GatewayTrunkDataCollectServer" },
		{ EST_MonitiorServer, "EST_MonitiorServer" },
		{ EST_DataCenterServer, "EST_DataCenterServer" },
		{ EST_TrunkDataDistributeServer, "EST_TrunkDataDistributeServer" },
		{ EST_IPCCDataDistributeServer, "EST_IPCCDataDistributeServer" },
		{ EST_GuideServer, "EST_GuideServer" },
		{ EST_AreaCodeServer, "EST_AreaCodeServer" },
		{ EST_ConfigurationServer, "EST_ConfigurationServer" },
		{ EST_OneKeyWitchCtrl, "EST_OneKeyWitchCtrl" },
		{ ESI_RedirectServer, "ESI_RedirectServer" },
		{ EST_RegisterServer, "EST_RegisterServer" },
		{ EST_ClientHeartBeatServer, "EST_ClientHeartBeatServer" },
		{ EST_GW_ServerHeartBeatServer, "EST_GW_ServerHeartBeatServer" },
		{ EST_CC_ServerHeartBeatServer, "EST_CC_ServerHeartBeatServer" },
		{ EST_Softacd, "EST_Softacd" },
		{ EST_InfobirdAcd, "EST_InfobirdAcd" },
		{ EST_TollTransactionServer, "EST_TollTransactionServer" },
		{ EST_GlobalCTI, "EST_GlobalCTI" },
		{ EST_DataGWServer, "EST_DataGWServer" },
		{ EST_CaptureNetInfoDataCollect, "EST_CaptureNetInfoDataCollect" },
		{ EST_IPCCRecorder, "EST_IPCCRecorder" },
		{ EST_Client, "EST_Client" },
		{ EST_TcpProxyServer, "EST_TcpProxyServer" },
		{ EST_CClinet, "EST_CClinet" },
		{ EST_CProxyServer, "EST_CProxyServer" },
		{ EST_DataTransServer, "EST_DataTransServer" },
		{ EST_envInfoColServer, "EST_envInfoColServer" },
		{ EST_AgentRecordServer, "EST_AgentRecordServer" },
		{ EST_MainRedisA_Write, "EST_MainRedisA_Write" },
		{ EST_MainRedisA_Read, "EST_MainRedisA_Read" },
		{ EST_UpdateRedisA_Write, "EST_UpdateRedisA_Write" },
		{ EST_UpdateRedisA_Read, "EST_UpdateRedisA_Read" },
		{ EST_MainRedisB_Write, "EST_MainRedisB_Write" },
		{ EST_MainRedisB_Read, "EST_MainRedisB_Read" },
		{ EST_UpdateRedisB_Write, "EST_UpdateRedisB_Write" },
		{ EST_UpdateRedisB_Read, "EST_UpdateRedisB_Read" },
		{ EST_MainRedisC_Write, "EST_MainRedisC_Write" },
		{ EST_MainRedisC_Read, "EST_MainRedisC_Read" },
		{ EST_UpdateRedisC_Write, "EST_UpdateRedisC_Write" },
		{ EST_UpdateRedisC_Read, "EST_UpdateRedisC_Read" },
		{ EST_TcpTransServer, "EST_TcpTransServer" },
		{ EST_MainRedis_TimeOut, "EST_MainRedis_TimeOut" },
		{ EST_REDIS_MAX, "EST_REDIS_MAX" },
	};
	return id_to_name(service_types, ARRAY_LEN(service_types), t);
}

string ESeverType_to_str(int i)
{
	return to_str(ESeverType(i));
}

string ipccmsg_type_to_str( int clazz, int type )
{
	if (clazz == (int)Class_FuncCall ||
		clazz == (int)Class_FuncReturn)
	{
		return ipccmsg_type_ocx_to_str(type);
	}
	else if (clazz == (int)Class_FuncEvent)
	{
		return ocx_eventid_to_str(type);
	}
	else
	{
		return ipccmsg_type_to_str(type);
	}
}

string to_str(__AgentState s)
{
	static id_name states[] =
	{
		{ Agent_NoLogin, "Agent_NoLogin" },
		{ Agent_Waiting, "Agent_Waiting" },
		{ Agent_Ringing, "Agent_Ringing" },
		{ Agent_Parking, "Agent_Parking" },
		{ Agent_Talking, "Agent_Talking" },
		{ Agent_OpTalking, "Agent_OpTalking" },
		{ Agent_ForeProcess, "Agent_ForeProcess" },
		{ Agent_BackProcess, "Agent_BackProcess" },
		{ Agent_Holding, "Agent_Holding" },
		{ Agent_Held, "Agent_Held" },
		{ Agent_Dialing, "Agent_Dialing" },
		{ Agent_DialUnderHold, "Agent_DialUnderHold" },
		{ Agent_TalkUnderHold, "Agent_TalkUnderHold" },
		{ Agent_ConnSucc_NoSend, "Agent_ConnSucc_NoSend" },
		{ Agent_PreDial, "Agent_PreDial" },
		{ Agent_ConferencePreside, "Agent_ConferencePreside" },
		{ Agent_ConferenceJoin, "Agent_ConferenceJoin" },
		{ Agent_IVRnWait, "Agent_IVRnWait" }
	};
	return id_to_name(states, ARRAY_LEN(states), s);
}

string agent_state_to_str( int i )
{
	return to_str(__AgentState(i));
}

string to_str(__AgentCallingState s)
{
	static id_name states[] =
	{
		{ AgentCalling_UserNoLogin, "AgentCalling_UserNoLogin" },
		{ AgentCalling_UserBusy, "AgentCalling_UserBusy" },
		{ AgentCalling_UserRinging, "AgentCalling_UserRinging" },
		{ AgentCalling_UserQueuing, "AgentCalling_UserQueuing" },
		{ AgentCalling_AgentNoLogin, "AgentCalling_AgentNoLogin" },
		{ AgentCalling_AgentBusy, "AgentCalling_AgentBusy" },
		{ AgentCalling_AgentQueuing, "AgentCalling_AgentQueuing" },
		{ AgentCalling_AgentRinging, "AgentCalling_AgentRinging" },
		{ AgentCalling_DepartmentInexistence, "AgentCalling_DepartmentInexistence" },
		{ AgentCalling_MakeCallNoAllowed, "AgentCalling_MakeCallNoAllowed" },
        { AgentCalling_InternalError, "AgentCalling_InternalError" },
        { AgentCalling_ServiceBusy, "AgentCalling_ServiceBusy" },
        { AgentCalling_Repeated, "AgentCalling_Repeated" },
    };
	return id_to_name(states, ARRAY_LEN(states), s);
}

string agent_calling_state_to_str( int i )
{
	return to_str(__AgentCallingState(i));
}

string to_str(__UserCallingState s)
{
	static id_name states[] =
	{
		{ UserCalling_Failed, "UserCalling_Failed" },
		{ UserCalling_Queuing, "UserCalling_Queuing" },
		{ UserCalling_Ringing, "UserCalling_Ringing" },
		{ UserCalling_Nobody, "UserCalling_Nobody" },
		{ UserCalling_AgentNoLogon, "UserCalling_AgentNoLogon" },
		{ UserCalling_AgentBusy, "UserCalling_AgentBusy" },
	};
	return id_to_name(states, ARRAY_LEN(states), s);
}

string user_calling_state_to_str(int i)
{
	return to_str(__UserCallingState(i));
}

string to_str(LoginType t)
{
	static id_name states[] =
	{
		{ LoginType_OnlyWeb, "LoginType_OnlyWeb" },
		{ LoginType_WebPSTN, "LoginType_WebPSTN" },
		{ LoginType_OnlyPSTN, "LoginType_OnlyPSTN" },
	};
	return id_to_name(states, ARRAY_LEN(states), t);
}

string login_type_to_str(int i)
{
	return to_str(LoginType(i));
}

string to_str(__CallPartyType t)
{
	static id_name states[] =
	{
		{ CallParty_User, "User" },
		{ CallParty_Agent, "Agent" },
		{ CallParty_VoiceGateWay, "VoiceGateWay" },
		{ CallParty_KeyWord, "KeyWord" },
		{ CallParty_Department, "Department" },
		{ CallParty_WebPstnAgent, "WebPstnAgent" },
		{ CallParty_IVR, "IVR" },
		{ CallParty_CClient, "CClient" },
		{ CallParty_WebchatUser, "WebchatUser"},
		{ CallParty_WebIM, "WebIM" },
		{ CallParty_Weibo, "Weibo" },
		{ CallParty_Email, "Email" },
		{ CallParty_WebchatXCX, "WebchatXCX" },
	};
	return id_to_name(states, ARRAY_LEN(states), t);
}

string device_type_to_str(int i)
{
	return to_str(__CallPartyType(i));
}

string call_party_type_to_str(int i)
{
	return to_str(__CallPartyType(i));
}

string to_str(ECallType t)
{
    static id_name states[] =
    {
        { E_NORMAL_CALL, "E_NORMAL_CALL" },
        { E_MESSAGE_CALL, "E_MESSAGE_CALL" },
        { E_WEBCHAT_CALL, "E_WEBCHAT_CALL" },
        { E_EMAIL_CALL, "E_EMAIL_CALL" },
        { E_VIDEO_CALL, "E_VIDEO_CALL" },
    };
    return id_to_name(states, ARRAY_LEN(states), t);
}

string to_str(_OutDialType t)
{
    static id_name states[] =
    {
        { OutDialType_Normal, "OutDialType_Normal" },
        { OutDialType_RingAgentExt, "OutDialType_RingAgentExt" },
        { OutDialType_WebPSTNDial, "OutDialType_WebPSTNDial" },
        { OutDialType_WapDial, "OutDialType_WapDial" },
        { OutDialType_DialAgentExt, "OutDialType_DialAgentExt" },
    };
    return id_to_name(states, ARRAY_LEN(states), t);
}

string outdial_type_to_str(int t)
{
    return to_str((_OutDialType)t);
}

string ECallType_to_str(int i)
{
	return to_str((ECallType)(i));
}

string to_str(AppOSType t)
{
	static id_name states[] =
	{
		{ OSType_WindowsPC, "OSType_WindowsPC" },
		{ OSType_Andriod, "OSType_Andriod" },
		{ OSType_IOS, "OSType_IOS" },
	};
	return id_to_name(states, ARRAY_LEN(states), t);
}

string AppOSType_to_str(int i)
{
    return to_str((AppOSType)(i));
}

string CallTargetType_to_str(int i)
{
    static id_name states[] =
    {
        { 1, "depart" },
        { 2, "skill" },
        { 3, "agent" },
    };
    return id_to_name(states, ARRAY_LEN(states), i);
}


}
}

