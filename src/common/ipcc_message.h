#ifndef IB_COMMON_MESSAGE_H
#define IB_COMMON_MESSAGE_H

#include "common/memory.h"

#ifndef NOPUBLIC
#include "public/IPCCMessage.h"
#include "public/ServerInfoDef.h"
#else
#include "IPCCMessage.h"
#include "ServerInfoDef.h"
#endif

namespace ib
{
namespace common
{

enum agent_action {
	    //{{AFX_DISP_ID(CAgentPanelOcxCtrl)
    dispidActionLogon = 1L,
    dispidActionLogon2 = 2L,
    dispidActionReset = 3L,
    dispidActionLogout = 4L,
    dispidActionLogonAgain = 5L,
    dispidActionCallDepartment = 6L,
    dispidActionCallAgent = 7L,
    dispidActionCallOtherAgent = 8L,
    dispidActionCallPstn = 9L,
    dispidActionCallExtNum = 10L,
    dispidActionCallOtherDepartment = 11L,
    dispidActionOnHook = 12L,
    dispidActionOffHook = 13L,
    dispidActionPark = 14L,
    dispidActionUnPark = 15L,
    dispidActionProcess = 16L,
    dispidActionUnProcess = 17L,
    dispidActionHold = 18L,
    dispidActionUnHold = 19L,
    dispidActionTransfer = 20L,
    dispidActionAutoTransfer = 21L,
    dispidActionStartMonit = 22L,
    dispidActionStopMonit = 23L,
    dispidActionForceDisconnect = 24L,
    dispidActionGetFullFreeAgentInfo = 25L,
    dispidActionGetGroupInfo = 26L,
    dispidActionGetOneGroupFreeAgentInfo = 27L,
    dispidActionGetTalkingAgent = 28L,
    dispidActionSendDTMF = 29L,
    dispidActionChangePassword = 30L,
    dispidActionChangeEarphoneVolume = 31L,
    dispidActionChangeMicVolume = 32L,
    dispidActionSendSMS = 33L,
    dispidActionSendChat = 34L,
    dispidConfigPortSettings = 35L,
    dispidConfigRecSettings = 36L,
    dispidConfigOutDialSettings = 37L,
    dispidConfigSoundSettings = 38L,
    dispidGetLoginManner = 39L,
    dispidGetDeptment = 40L,
    dispidGetCorp = 41L,
    dispidGetCallType = 42L,
    dispidGetCallFrom = 43L,
    dispidGetPrivateData = 44L,
    dispidGetRecInvokeID = 45L,
    dispidGetClientState = 46L,
    dispidGetLogonType = 47L,
    dispidGetCallPartyType = 48L,
    dispidGetAgentHasLogon = 49L,
    dispidInitialize = 50L,
    dispidExit = 51L,
    dispidGetRoleInCompany = 52L,
    dispidGetAgentAccount = 53L,
    dispidGetIsCCNUser = 54L,
    dispidGetAgentType = 55L,
    dispidGetIsSMSEnabled = 56L,
    dispidGetAgentExtNum = 57L,
    dispidGetAgentPhoneNo = 58L,
    dispidGetManagerID = 59L,
    dispidGetIsSuperAgent = 60L,
    dispidGetIsMasterSuperAgent = 61L,
    dispidActionSendStrMsg = 62L,
    dispidGetRemindTime = 63L,
    dispidActionSettings = 64L,
    dispidActionMonit = 65L,
    dispidActionAgentOutDial = 66L,
    dispidGetIsMonit = 67L,
    dispidGetSMSParamsInfo = 68L,
    dispidGetAllSettings = 69L,
    dispidGetBriefStatisticInfo = 70L,
    dispidActionSaveBriefStatisticInfo = 71L,
    dispidActionSetMuteMode = 72L,
    dispidActionTransferToIVR = 73L,
    dispidGetTransferToIVRParamsInfo = 74L,
    dispidGetIsCallFromACD = 75L,
    dispidActionUpdateSettings = 76L,
    dispidGetIsOutDialOpenURL = 77L,
    dispidGetIVRReturn = 78L,
    dispidSetProductVersion = 79L,
    dispidGetRecInvokePath = 80L,
    dispidGetAgentID = 81L,
    dispidSetOriginalData = 82L,
    dispidActionOutDial = 83L,
    dispidSetSelectGroup = 84L,
    dispidActionAutoTransferEx = 85L,
    dispidActionOutDialWithRecSetting = 86L,
    dispidActionOffHookWithRecSetting = 87L,
    dispidGetIsCallInOpenURL = 88L,
    dispidGetParkReasons = 89L,
    dispidSetParkReason = 90L,
    dispidSetCallerSendingInDialing = 91L,
    dispidActionJoinOutDialMode = 92L,
    dispidGetIsOutDialMode = 93L,
    dispidGetMoudleFilePath = 94L,
    dispidActionSetMuteModeEx = 95L,
    // add by lh.wang date:2012.11.08 µÚÈý·½
    dispidActionStartConference = 96L,
    dispidActionEndConference = 97L,
    dispidGetCanConference    = 98L,
    dispidStopPlayAgentID = 99L,
    dispidActionACDAutoLogin=100L,
    // end add 
    eventidOnLogin = 1L,
    eventidOnAgentLoginSucc = 2L,
    eventidOnAgentLogoutConf = 3L,
    eventidOnAgentMakeCallConf = 4L,
    eventidOnClientStateChange = 5L,
    eventidOnCallDelivered = 6L,
    eventidOnSystemStatusInfo = 7L,
    eventidOnChatMsgDelivered = 8L,
    eventidOnQueueSize = 9L,
    eventidOnGetGroupInfo = 10L,
    eventidOnGetOneGroupFreeAgentInfo = 11L,
    eventidOnGetFullFreeAgentInfo = 12L,
    eventidOnGetFullTalkingAgentInfo = 13L,
    eventidOnAudioLostRate = 14L,
    eventidOnAgentLoginFailed = 15L,
    eventidOnServiceStateChange = 16L,
    eventidOnPreOutDialEnd = 17L,
    eventidOnUserLoginInfo = 18L,
    eventidOnAgentOutdialConf = 19L,
    eventidOnACDSendStrMsg = 20L,
    eventidOnLoginSendStrMsg = 21L,
    eventidOnIVRReturnToAgentConf = 22L,
    eventidOnNotifyAgentLoginConf = 23L,
    //}}AFX_DISP_ID
};

struct SIPCCMsg_Wrapper : public SIPCCMessage
{
public:
	SIPCCMsg_Wrapper()
	{
		msgClass = -1;
		msgType = -1;
		msg_buf_ptr_ = NULL;
		msg_buf_size_ = 0;
	}

	SIPCCMsg_Wrapper(const char * msg_buf, int msg_buf_size)
	{
		msgClass = -1;
		msgType = -1;
		if (msg_buf_size > 0)
		{
			msg_buf_ptr_ = new char[msg_buf_size];
			memmove(msg_buf_ptr_, msg_buf, msg_buf_size);
			msg_buf_size_ = msg_buf_size;
		}
	}
	~SIPCCMsg_Wrapper()
	{
		delete msg_buf_ptr_;
	}

	virtual int32_t FillBuf(char*& thebuf)
	{ 
		return 0;
	};
	virtual void AnalyzeBuf(char* thebuf, int32_t msgsize, uint32_t ip, uint16_t port) 
	{ 
	};
	virtual int32_t GetMyMemSize() 
	{ 
		return 0;
	};
	
	char * msg_buf_ptr_;
	int msg_buf_size_;
};

typedef shared_ptr<SIPCCMessage> ipcc_msg_ptr;
typedef shared_ptr<SIPCCMessage> ipccmessage_ptr;

ipcc_msg_ptr parse_ipccmsg(const char * msg_buf, int msg_buf_size);


}
}

namespace ib_common = ib::common;

#endif
