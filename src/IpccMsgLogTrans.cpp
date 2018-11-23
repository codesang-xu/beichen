#include"public/StdAfx.h"
#include "IpccMsgLogTrans.h"
#include "public/IPCCMessage.h"
#include <sstream>
using namespace std;
#include "public/Spliter.h"

CIpccMsgLogTrans::CIpccMsgLogTrans(void)
{
}

CIpccMsgLogTrans::~CIpccMsgLogTrans(void)
{
}

bool CIpccMsgLogTrans::LogToMsg(string sLog, SIPCCMessage* msg)
{
	CSpliter sp(sLog, "|||");
	if (sp.GetSize() <= 3 || sp[0] != "IpccMsg")
	{
		return false;
	}

	if (msg->msgType == Type_ACD_CallEnterVDN)
	{		
		SIPCCMsg_ACD_CallEnterVDN* pMsg = (SIPCCMsg_ACD_CallEnterVDN*)msg;
		pMsg->corpName = sp[3];
		pMsg->vdnNo = sp[4];
		pMsg->vdnName = sp[5];
		pMsg->callID = sp[6];
		pMsg->callerID = sp[7];
		pMsg->calleeID = sp[8];
		pMsg->callSrc = sp[9];
		pMsg->extDevID = sp[10];
		pMsg->enterTime = sp[11];
		pMsg->status = atoi(sp[12].c_str());
		pMsg->callType = atoi(sp[13].c_str());
		pMsg->bIsOverflow = atoi(sp[14].c_str());
		pMsg->managerid = sp[15];
		pMsg->deptid = sp[16];
		pMsg->CurQueueNum = atoi(sp[17].c_str());
		pMsg->bInnerCall = atoi(sp[18].c_str());
		pMsg->bIntoQueue = atoi(sp[19].c_str());
		pMsg->SkillGroupID = sp[20];
		pMsg->SkillGroupName = sp[21];
		pMsg->UserLevel = atoi(sp[22].c_str());
		return true;
	}
	else if (msg->msgType == Type_ACD_CallLeaveVDN)
	{
		SIPCCMsg_ACD_CallLeaveVDN* pMsg = (SIPCCMsg_ACD_CallLeaveVDN*)msg;
		pMsg->vdnNo = sp[3];
		pMsg->callID = sp[4];
		pMsg->leaveTime = sp[5];
		pMsg->leaveCause = atoi(sp[6].c_str());
		pMsg->corpName = sp[7];
		pMsg->managerid = sp[8];
		pMsg->deptid = sp[9];
		pMsg->LeaveQueueTime = sp[10];
		pMsg->agentID = sp[11];
		pMsg->bIntoQueue = atoi(sp[12].c_str());
		pMsg->CurQueueNum = atoi(sp[13].c_str());
		pMsg->SkillGroupID = sp[14];
		pMsg->SkillGroupName = sp[15];
		pMsg->UserLevel = atoi(sp[16].c_str());
		return true;
	}
	else if (msg->msgType == Type_ACD_AgentStateInfo)
	{
		SIPCCMsg_ACD_AgentStateInfo* pMsg = (SIPCCMsg_ACD_AgentStateInfo*)msg;
		pMsg->SetCallID(sp[3]);
		pMsg->SetAgentID(sp[4]);
		pMsg->SetCorporationName(sp[5]);
		pMsg->SetTheDepartment(sp[6]);
		pMsg->SetUserName(sp[7]);
		pMsg->SetKeyWord(sp[8]);
		pMsg->SetCurTime(sp[9]);
		pMsg->agentState = atoi(sp[10].c_str());
		pMsg->bCallIn = atoi(sp[11].c_str());
		pMsg->bInnerCall = atoi(sp[12].c_str());
		pMsg->bPSTNCall = atoi(sp[13].c_str());
		pMsg->parkReasonID = atoi(sp[14].c_str());
		pMsg->SetParkReason(sp[15]);
		pMsg->callerID = sp[16];
		pMsg->calleeID = sp[17];
		pMsg->bAgentLogin = atoi(sp[18].c_str());
		pMsg->managerid = sp[19];
		pMsg->deptid = sp[20];
		pMsg->bCallOutTypeUnderHold = atoi(sp[21].c_str());
		pMsg->nickName = sp[22];
		pMsg->extNo = sp[23];
		pMsg->SkillGroupID = sp[24];
		pMsg->SkillGroupName = sp[25];
		pMsg->SkillGroupID_Original = sp[26];
		pMsg->SkillGroupName_Original = sp[27];
		pMsg->UserLevel = atoi(sp[28].c_str());
		return true;
	}
	else if (msg->msgType == Type_ACD_NotifyDataCollect)
	{
		SIPCCMsg_ACD_NotifyDataCollect* pMsg = (SIPCCMsg_ACD_NotifyDataCollect*)msg;
		pMsg->theMinute = atoi(sp[3].c_str());
		pMsg->CurrentHour= atoi(sp[4].c_str());
		pMsg->collectDate= sp[5];
		return true;
	}
	else
	{
		return NULL;
	}
}

string CIpccMsgLogTrans::MsgToLog(SIPCCMessage* pMsg)
{
	ostringstream os;
	os << "IpccMsg" << "|||";
	os << pMsg->msgClass << "|||";
	os << pMsg->msgType << "|||";
	switch (pMsg->msgType)
	{
	case Type_ACD_CallEnterVDN:
		{
			SIPCCMsg_ACD_CallEnterVDN* pMsg1 = (SIPCCMsg_ACD_CallEnterVDN*)pMsg;
			os << pMsg1->corpName << "|||";
			os << pMsg1->vdnNo << "|||";
			os << pMsg1->vdnName << "|||";
			os << pMsg1->callID << "|||";
			os << pMsg1->callerID << "|||";
			os << pMsg1->calleeID << "|||";
			os << pMsg1->callSrc << "|||";
			os << pMsg1->extDevID << "|||";
			os << pMsg1->enterTime << "|||";
			os << pMsg1->status << "|||";
			os << pMsg1->callType << "|||";
			os << (int)pMsg1->bIsOverflow << "|||";
			os << pMsg1->managerid << "|||";
			os << pMsg1->deptid << "|||";
			os << pMsg1->CurQueueNum << "|||";
			os << (int)pMsg1->bInnerCall << "|||";
			os << (int)pMsg1->bIntoQueue << "|||";
			os << pMsg1->SkillGroupID << "|||";
			os << pMsg1->SkillGroupName << "|||";
			os << pMsg1->UserLevel << "|||";
		}
		break;
	case Type_ACD_CallLeaveVDN:
		{
			SIPCCMsg_ACD_CallLeaveVDN* pMsg1 = (SIPCCMsg_ACD_CallLeaveVDN*)pMsg;
			os << pMsg1->vdnNo << "|||";
			os << pMsg1->callID << "|||";
			os << pMsg1->leaveTime << "|||";
			os << pMsg1->leaveCause << "|||";
			os << pMsg1->corpName << "|||";
			os << pMsg1->managerid << "|||";
			os << pMsg1->deptid << "|||";
			os << pMsg1->LeaveQueueTime << "|||";
			os << pMsg1->agentID << "|||";
			os << (int)pMsg1->bIntoQueue << "|||";
			os << pMsg1->CurQueueNum << "|||";
			os << pMsg1->SkillGroupID << "|||";
			os << pMsg1->SkillGroupName << "|||";
			os << pMsg1->UserLevel << "|||";
		}
		break;
	case Type_ACD_AgentStateInfo:
		{
			SIPCCMsg_ACD_AgentStateInfo* pMsg1 = (SIPCCMsg_ACD_AgentStateInfo*)pMsg;
			os << pMsg1->GetCallID() << "|||";
			os << pMsg1->GetAgentID() << "|||";
			os << pMsg1->GetCorporationName() << "|||";
			os << pMsg1->GetTheDepartment() << "|||";
			os << pMsg1->GetUserName() << "|||";
			os << pMsg1->GetKeyWord() << "|||";
			os << pMsg1->GetCurTime() << "|||";
			os << pMsg1->agentState << "|||";
			os << (int)pMsg1->bCallIn << "|||";
			os << (int)pMsg1->bInnerCall << "|||";
			os << (int)pMsg1->bPSTNCall << "|||";
			os << pMsg1->parkReasonID << "|||";
			os << pMsg1->GetParkReason() << "|||";
			os << pMsg1->callerID << "|||";
			os << pMsg1->calleeID << "|||";
			os << (int)pMsg1->bAgentLogin << "|||";
			os << pMsg1->managerid << "|||";
			os << pMsg1->deptid << "|||";
			os << (int)pMsg1->bCallOutTypeUnderHold << "|||";
			os << pMsg1->nickName << "|||";
			os << pMsg1->extNo << "|||";
			os << pMsg1->SkillGroupID << "|||";
			os << pMsg1->SkillGroupName << "|||";
			os << pMsg1->SkillGroupID_Original << "|||";
			os << pMsg1->SkillGroupName_Original << "|||";
			os << pMsg1->UserLevel << "|||";
		}
		break;
	case Type_ACD_NotifyDataCollect:
		{
		
			SIPCCMsg_ACD_NotifyDataCollect* pMsg1 = (SIPCCMsg_ACD_NotifyDataCollect*)pMsg;
			os << pMsg1->theMinute << "|||";
			os << pMsg1->CurrentHour << "|||";
			os << pMsg1->collectDate << "|||";
				
		}
		break;
	default:
		break;
	}

	return os.str();
}