#include "common/ipcc_message.h"

namespace ib
{
namespace common
{
    ipcc_msg_ptr parse_ipccmsg_(const char * msg_buf, int msg_buf_size)
    {
		int msg_class = *(int*)msg_buf;
		int msg_type = ((int*)msg_buf)[1];

		switch (msg_class)
		{
        case Class_ACD:
            switch (msg_type)
            {
            case Type_ACD_AgentMakeCallConf:
                return shared_ptr<SIPCCMsg_ACD_AgentMakeCallConf>(new SIPCCMsg_ACD_AgentMakeCallConf());
            case Type_ACD_CallArriveToAgent:
                return shared_ptr<SIPCCMsg_ACD_CallArriveToAgent>(new SIPCCMsg_ACD_CallArriveToAgent());
            case Type_ACD_CallEstablished:
                return shared_ptr<SIPCCMsg_ACD_CallEstablished>(new SIPCCMsg_ACD_CallEstablished());
            case Type_ACD_CallReleased:
                return shared_ptr<SIPCCMsg_ACD_CallReleased>(new SIPCCMsg_ACD_CallReleased());
            case Type_ACD_CallUserConf:
				return shared_ptr<SIPCCMsg_ACD_CallUserConf>(new SIPCCMsg_ACD_CallUserConf());
            case Type_ACD_CHangupCallConf:
                return shared_ptr<SIPCCMsg_ACD_CHangupCallConf>(new SIPCCMsg_ACD_CHangupCallConf());
            case Type_ACD_DepartNAgentInfo:
                return shared_ptr<SIPCCMsg_ACD_DepartNAgentInfo>(new SIPCCMsg_ACD_DepartNAgentInfo());
            }
            break;
        case Class_AreaCode:
            switch (msg_type)
            {
            case Type_AreaCode_AreaAnswer:
                return shared_ptr<SIPCCMsg_AreaAnswer>(new SIPCCMsg_AreaAnswer());
            }
            break;
        case Class_CClient:
            switch (msg_type)
            {
            case Type_CClient_CMakeCall:
                return shared_ptr<SIPCCMsg_CClient_CMakeCall>(new SIPCCMsg_CClient_CMakeCall());
            case Type_CClient_CallArrived:
                return shared_ptr<SIPCCMsg_CClient_CallArrived>(new SIPCCMsg_CClient_CallArrived());
            case Type_CClient_CallAnswered:
                return shared_ptr<SIPCCMsg_CClient_CallAnswered>(new SIPCCMsg_CClient_CallAnswered());
            case Type_CClient_HangupCall:
                return shared_ptr<SIPCCMsg_CClient_HangupCall>(new SIPCCMsg_CClient_HangupCall());
            case Type_CClient_HangupCallConf:
                return shared_ptr<SIPCCMsg_CClient_HangupCallConf>(new SIPCCMsg_CClient_HangupCallConf());
            case Type_CClient_HeartBeat:
                return shared_ptr<SIPCCMsg_CClient_HeartBeat>(new SIPCCMsg_CClient_HeartBeat());
            }
            break;
        case Class_Client:
            switch (msg_type)
            {
            case Type_Client_UserHangupCall:
                return shared_ptr<SIPCCMsg_Client_UserHangupCall>(new SIPCCMsg_Client_UserHangupCall());
            case Type_Client_AgentMakeCall:
                return shared_ptr<SIPCCMsg_Client_AgentMakeCall>(new SIPCCMsg_Client_AgentMakeCall());
            case Type_Client_CallUserConf:
                return shared_ptr<SIPCCMsg_Client_CallUserConf>(new SIPCCMsg_Client_CallUserConf());
            case Type_Client_UserAnswerCall:
                return shared_ptr<SIPCCMsg_Client_UserAnswerCall>(new SIPCCMsg_Client_UserAnswerCall());
            case Type_Client_UserAnswerCall_Real:
                return shared_ptr<SIPCCMsg_Client_UserAnswerCall_Real>(new SIPCCMsg_Client_UserAnswerCall_Real());
            case Type_Client_MediaRequest:
                return shared_ptr<SIPCCMsg_Client_MediaRequest>(new SIPCCMsg_Client_MediaRequest());
            case Type_Client_MediaRequestConf:
                return shared_ptr<SIPCCMsg_Client_MediaRequestConf>(new SIPCCMsg_Client_MediaRequestConf());
            case Type_Client_AgentHangupCall:
                return shared_ptr<SIPCCMsg_Client_AgentHangupCall>(new SIPCCMsg_Client_AgentHangupCall());
            }
            break;
        case Class_CProxy:
			switch (msg_type)
			{
			case Type_CProxy_HeartBeatConf:
				return shared_ptr<SIPCCMsg_CProxy_HeartBeatConf>(new SIPCCMsg_CProxy_HeartBeatConf());
			case Type_CProxy_CMakeCall:
				return shared_ptr<SIPCCMsg_CProxy_CMakeCall>(new SIPCCMsg_CProxy_CMakeCall());
			case Type_CProxy_CMakeCallConf:
				return shared_ptr<SIPCCMsg_CProxy_CMakeCallConf>(new SIPCCMsg_CProxy_CMakeCallConf());
			case Type_CProxy_CMakeCall2:
				return shared_ptr<SIPCCMsg_CProxy_CMakeCall2>(new SIPCCMsg_CProxy_CMakeCall2());
			case Type_CProxy_QueryGW:
				return shared_ptr<SIPCCMsg_CProxy_QueryGW>(new SIPCCMsg_CProxy_QueryGW());
			case Type_CProxy_CallArrived:
				return shared_ptr<SIPCCMsg_CProxy_CallArrived>(new SIPCCMsg_CProxy_CallArrived());
			case Type_CProxy_CallAnswered:
				return shared_ptr<SIPCCMsg_CProxy_CallAnswered>(new SIPCCMsg_CProxy_CallAnswered());
			case Type_CProxy_CallDelivered:
				return shared_ptr<SIPCCMsg_CProxy_CallDelivered>(new SIPCCMsg_CProxy_CallDelivered());
			case Type_CProxy_CallStart:
				return shared_ptr<SIPCCMsg_CProxy_CallStart>(new SIPCCMsg_CProxy_CallStart());
			case Type_CProxy_HangupCall:
				return shared_ptr<SIPCCMsg_CProxy_HangupCall>(new SIPCCMsg_CProxy_HangupCall());
			case Type_CProxy_Login:
				return shared_ptr<SIPCCMsg_CProxy_Login>(new SIPCCMsg_CProxy_Login());
			case Type_CProxy_LoginConf:
				return shared_ptr<SIPCCMsg_CProxy_LoginConf>(new SIPCCMsg_CProxy_LoginConf());
			case Type_CProxy_Logout:
				return shared_ptr<SIPCCMsg_CProxy_Logout>(new SIPCCMsg_CProxy_Logout());
			case Type_CProxy_LogoutConf:
				return shared_ptr<SIPCCMsg_CProxy_LogoutConf>(new SIPCCMsg_CProxy_LogoutConf());
			}
			break;
        case Class_Gateway:
            switch (msg_type)
            {
            case Type_Gateway_QueryGWConf:
                return shared_ptr<SIPCCMsg_Gateway_QueryGWConf>(new SIPCCMsg_Gateway_QueryGWConf());
            }
            break;
        case Class_Redirect:
            switch (msg_type)
            {
            case Type_Redirect_GetServiceConf:
                return shared_ptr<SIPCCMsg_Redirect_GetServiceConf>(new SIPCCMsg_Redirect_GetServiceConf());
            }
            break;
        case Class_HeartBeat:
            switch (msg_type)
            {
            case Type_CClient_ToCProxyHeartBeat:
                return shared_ptr<SIPCCMsg_CClientToCProxyHeartBeat>(new SIPCCMsg_CClientToCProxyHeartBeat());
            }
            break;
        case Class_OneKeySwitch:
            switch (msg_type)
            {
            case Type_OneKeySwitch_ServiceRegisterConf:
                return shared_ptr<SIPCCMsg_OneKeySwitch_ServiceRegisterConf>(new SIPCCMsg_OneKeySwitch_ServiceRegisterConf());
            case Type_OneKeySwitch_ServiceConfirmConf:
                return shared_ptr<SIPCCMsg_OneKeySwitch_ServiceConfirmConf>(new SIPCCMsg_OneKeySwitch_ServiceConfirmConf());
            case Type_OneKeySwitch_GetHeartBeatConf:
                return shared_ptr<SIPCCMsg_OneKeySwitch_GetHeartBeatConf>(new SIPCCMsg_OneKeySwitch_GetHeartBeatConf());
            case Type_OneKeySwitch_ServiceConfirm:
                return shared_ptr<SIPCCMsg_OneKeySwitch_ServiceConfirm>(new SIPCCMsg_OneKeySwitch_ServiceConfirm());
            case Type_OneKeySwitch_ServerHeartBeatConf:
                return shared_ptr<SIPCCMsg_OneKeySwitch_ServerHeartBeatConf>(new SIPCCMsg_OneKeySwitch_ServerHeartBeatConf());
            case Type_OneKeySwitch_ServiceTimeout:
                return shared_ptr<SIPCCMsg_OneKeySwitch_ServiceTimeout>(new SIPCCMsg_OneKeySwitch_ServiceTimeout());
            case Type_OneKeySwitch_GetServiceConf:
                return shared_ptr<SIPCCMsg_OneKeySwitch_GetServiceConf>(new SIPCCMsg_OneKeySwitch_GetServiceConf());
            case Type_OneKeySwitch_SwitchService_Ctrl:
                return shared_ptr<SIPCCMsg_OneKeySwitch_SwitchService_Ctrl>(new SIPCCMsg_OneKeySwitch_SwitchService_Ctrl());
            case Type_OneKeySwitch_ClientHeartBeat:
                return shared_ptr<SIPCCMsg_OneKeySwitch_ClientHeartBeat>(new SIPCCMsg_OneKeySwitch_ClientHeartBeat());
            case Type_OneKeySwitch_SwitchAcd_Ctrl:
                return shared_ptr<SIPCCMsg_OneKeySwitch_SwitchAcd_Ctrl>(new SIPCCMsg_OneKeySwitch_SwitchAcd_Ctrl());
            case Type_OneKeySwitch_SwitchServiceConf:
                return shared_ptr<SIPCCMsg_OneKeySwitch_SwitchServiceConf>(new SIPCCMsg_OneKeySwitch_SwitchServiceConf());
			case Type_OneKeySwitch_NotifyMsgConf:
				return shared_ptr<SIPCCMsg_OneKeySwitch_NotifyMsgConf>(new SIPCCMsg_OneKeySwitch_NotifyMsgConf());
			}
            break;
        case Class_FileTransfer:
            switch (msg_type)
            {
            case Type_FileTransfer_UploadRequest:
                return shared_ptr<SIPCCMsg_FileTransfer_UploadRequest>(new SIPCCMsg_FileTransfer_UploadRequest());
            }
            break;
        }
		return shared_ptr<SIPCCMsg_Wrapper>(new SIPCCMsg_Wrapper(msg_buf, msg_buf_size));
	}

    ipcc_msg_ptr parse_ipccmsg(const char * msg_buf, int msg_buf_size)
    {
        ipcc_msg_ptr ret = parse_ipccmsg_(msg_buf, msg_buf_size);
        ret->AnalyzeBuf(const_cast<char *>(msg_buf), msg_buf_size, 0, 0);
        return ret;
    }

}
}
