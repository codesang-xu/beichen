// Department.cpp: implementation of the CDepartment class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "DepartmentInfo.h"
#include "UserMgr.h"
#include "AgentMgr.h"
#include "AgentInfo.h"
#include "CClientMgr.h"
#include "Utility.h"
#include "public/GlobalOperation.h"
#include "public/ServerInfoDef.h"
#include "public/Spliter.h"
#include "Log.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "public/MemLog.h"

extern CMemLog   g_Logger;
const int CDepartmentInfo::MAX_TXT_CALL_QUEUE_SIZE = 500;

CDepartmentInfo::CDepartmentInfo(string managerid, string corpName, string deptid, string departmentName)
    :m_sManagerid(managerid), m_sCorpName(corpName), m_sDeptid(deptid), m_sDepartmentName(departmentName)
{
    m_sRedisAgentQueueName = m_sManagerid + "." + m_sDepartmentName + ".AgentQueue";

	txtCallQueueStatus = 0;
	IsmapChanged = false;
	m_iQueuingAlgorithm = 1;//默认最小工作量
    m_AgentFinderForTXTCall_ptr_.reset(new ACDMincalls<map<string, string> >(
        m_mapAgentsInDepart, boost::bind(&CDepartmentInfo::ACDGetTXTCallCount, this, _1)
        , boost::bind(&CDepartmentInfo::GetAgentInfo1, this, _1)));//默认最小工作量
    m_AgentFinderForEmailCall_ptr_.reset(new ACDMincalls<map<string, string> >(
        m_mapAgentsInDepart, boost::bind(&CDepartmentInfo::ACDGetEmailCallCount, this, _1)
        , boost::bind(&CDepartmentInfo::GetAgentInfo1, this, _1)));//默认最小工作量
}

CDepartmentInfo::~CDepartmentInfo()
{

}

bool CDepartmentInfo::AgentLogin(string agentID)
{
    if (m_mapAgentsInDepart.size() == 0) {
        LoadQueueFromRedis();
    }

    if (agentID != "") {
        this->m_mapAgentsInDepart[agentID] = agentID;
        IsmapChanged = true;
        return false;
    }
    return true;
}

void CDepartmentInfo::AgentLogout(string agentID)
{
    this->m_mapAgentsInDepart.erase(agentID);

    IsmapChanged = true;
}

void CDepartmentInfo::SetQueuingAlgorithm(int32_t iQueuingAlgorithm)
{
    if (iQueuingAlgorithm != 1 && iQueuingAlgorithm != 2)
        return;

    if (m_iQueuingAlgorithm != iQueuingAlgorithm)
    {
        if (iQueuingAlgorithm == 1)
        {
            m_AgentFinderForTXTCall_ptr_.reset(new ACDMincalls<map<string, string> >(
                m_mapAgentsInDepart, boost::bind(&CDepartmentInfo::ACDGetTXTCallCount, this, _1)
                , boost::bind(&CDepartmentInfo::GetAgentInfo1, this, _1)));//默认最小工作量
            m_AgentFinderForEmailCall_ptr_.reset(new ACDMincalls<map<string, string> >(
                m_mapAgentsInDepart, boost::bind(&CDepartmentInfo::ACDGetEmailCallCount, this, _1)
                , boost::bind(&CDepartmentInfo::GetAgentInfo1, this, _1)));//默认最小工作量
        }
        else
        {
            m_AgentFinderForTXTCall_ptr_.reset(new ACDPoll<std::string>(m_mapAgentsInDepart,
                boost::bind(&CDepartmentInfo::CanAcceptTXTCall, this, _1)
                , boost::bind(&CDepartmentInfo::GetAgentInfo2, this, _1)));//默认最小工作量
            m_AgentFinderForEmailCall_ptr_.reset(new ACDPoll<std::string>(m_mapAgentsInDepart,
                boost::bind(&CDepartmentInfo::CanAcceptEmailCall, this, _1)
                , boost::bind(&CDepartmentInfo::GetAgentInfo2, this, _1)));//默认最小工作量
        }
	    m_iQueuingAlgorithm = iQueuingAlgorithm;
    }
}

string CDepartmentInfo::GetAllAndFreeCount(int* pAll, int* pFree)
{
	*pAll = 0;
	*pFree = 0;

	ostringstream os;

	for (map<string, string>::iterator it = m_mapAgentsInDepart.begin()
		; it != m_mapAgentsInDepart.end(); ++it)
	{
		CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(it->first);
		if (pAgent != NULL && pAgent->GetCurState() != Agent_NoLogin)
		{
			int iState = pAgent->GetCurState();
			if (iState != Agent_Parking)
			{
				(*pAll)++;
			}
			
			if (pAgent->GetCurState() == Agent_Waiting)
			{
				(*pFree)++;
			}
			else if (pAgent->m_iTalkBeginTime != 0)
			{
				os << (CalTimeSpan(pAgent->m_iTalkBeginTime, gettickcount()) / 1000) << ";";
			}
		}
	}

	return os.str();
}

int CDepartmentInfo::GetWebAgentCount()
{
	int iRet = 0;
	for (map<string, string>::iterator it = m_mapAgentsInDepart.begin()
		; it != m_mapAgentsInDepart.end(); ++it)
	{
		CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(it->first);
		if (pAgent != NULL && pAgent->m_iLoginType != LoginType_OnlyPSTN)
		{
			iRet++;
		}
	}

	return iRet;
}

int CDepartmentInfo::GetMobileAgentCount()
{
	int iRet = 0;
	for (map<string, string>::iterator it = m_mapAgentsInDepart.begin()
		; it != m_mapAgentsInDepart.end(); ++it)
	{
		CAgentInfo* pAgent = CAgentMgr::GetInstance()->GetAgentInfo(it->first);
		if (pAgent != NULL && pAgent->m_iLoginType == LoginType_OnlyPSTN)
		{
			iRet++;
		}
	}

	return iRet;
}

void CDepartmentInfo::CheckOverflow()
{
	if (m_iOverflowTime <= 0 || m_sOverflowTarget.empty()
		|| (m_iOverFlowDestType != 0 && m_iOverFlowDestType != 1))
	{
		return;
	}

    while (!m_CallQueue.Empty()) {
        CPriorityQueue<CallerPartyInfo>::Iterator it = m_CallQueue.Begin();
		CUserInfo* pUser = CUserMgr::GetInstance()->GetUserInfo(it->m_Value.partyName);
		if ( !pUser ) {
			break;
		}

		if (IsTimeout(pUser)) {
			if (pUser->m_iUserType == CallParty_VoiceGateWay) {
				Overflow(pUser);
			}
		}
		else {
			break;
		}
	}
}

bool CDepartmentInfo::IsTimeout(CUserInfo* pUser)
{
	unsigned long  dwSpan = gettickcount() - pUser->m_iEnqueueTime;
	return dwSpan >= m_iOverflowTime;
}

void CDepartmentInfo::Overflow(CUserInfo* pUser)
{
	string sUserName = pUser->m_sUserName;

	g_Logger.MakeLog("Overflow, UserName=" + sUserName + ";Target=" + m_sOverflowTarget);

	SIPCCMsg_ACD_CallLeaveVDN CallLeaveVDN;
    CallLeaveVDN.vdnNo = pUser->m_sQueueName;
	CallLeaveVDN.corpName = pUser->m_sCorpName;
	CallLeaveVDN.callID = pUser->m_CallID_sCallIn;	
	CallLeaveVDN.leaveTime =CLog::GetLocalTime();
	CallLeaveVDN.leaveCause = VDNEndCause_Overflow;
	CallLeaveVDN.managerid = this->m_sManagerid;
	CallLeaveVDN.deptid = this->m_sDeptid;
	CallLeaveVDN.LeaveQueueTime = CLog::GetLocalTime();
	CallLeaveVDN.agentID = "";
    CallLeaveVDN.CurQueueNum = this->GetCQueueSize();

	CUtility::GetInstance()->SendIPCCMsg_UDP(
		CallLeaveVDN, 
		CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, 
		CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

	CUtility::GetInstance()->SendToMonitorServer(CallLeaveVDN);

	SIPCCMsg_Client_UserMakeCall callInfo;
	callInfo.audioIP = pUser->m_audioIP;
	callInfo.audioPort = pUser->m_audioPort;
	callInfo.callTransID = pUser->m_CallID_sCallIn;//
	callInfo.comSymbol = pUser->m_sComSymbol;
	callInfo.gwPartNo = pUser->m_sPartNo;//
	callInfo.lanIP = pUser->m_lanIP;
	callInfo.SetKeyWord(pUser->m_sCalledKeyWord);
	callInfo.SetTheDepartment(m_sOverflowTarget);
	callInfo.SetUserName(pUser->m_sUserName);
	callInfo.SetUUIData(pUser->m_sUUIData);
	callInfo.userType = pUser->m_iUserType;

	unsigned long ip = pUser->m_ulUserIP;
	unsigned short port = pUser->m_usUserPort;

    RemoveFromCQueue(sUserName);

	CUtility::GetInstance()->OnUserMakeCall(&callInfo, ip, port, true);
}

void CDepartmentInfo::QueueToWAQueue(string agentID, int iQualification, bool writeRedis)
{
    if (agentID.length() <= 0) {
        g_Logger.MakeLog("agentid is empty");
        return;
    }
	
    //为避免座席重复登录引起的 WaitingList 重复座席的问题,先删除一下
    RemoveFromWAQueue(agentID, false);
    m_WaitingAgentQueue.PushBack(agentID, make_pair(agentID, iQualification));

    if (writeRedis) {
        RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
        if (acdRedisServer && 
            !acdRedisServer->PushBackToAgentQueue(m_sManagerid, m_sDepartmentName, agentID, iQualification)) {
            //todo error log
        }
    }

    //////ostringstream os;
    //////os << agentID << " PushBack to " << m_sDepartmentName;
    //////g_Logger.MakeLog(os.str());

    return;
}

string CDepartmentInfo::PopFromWAQueue(bool writeRedis)
{
    RedisServer_ACD * acdRedisServer = writeRedis ? 
        CUtility::GetInstance()->Get_RedisServer_ACD() : NULL;

    string id;
    pair<string, int> agentIDInfo;
    m_WaitingAgentQueue.PopFront(id, agentIDInfo);

    if (!id.empty() && acdRedisServer) {
        if (!acdRedisServer->RemoveFromAgentQueue(m_sManagerid, m_sDepartmentName, agentIDInfo.first, agentIDInfo.second)) {
            //todo error log
        }
    }

    return id;
}

string CDepartmentInfo::FrontOfWAQueue()
{
    string id;
    pair<string, int> agentIDInfo;
    m_WaitingAgentQueue.Front(id, agentIDInfo);
    return id;
}

int CDepartmentInfo::GetWAQueueSize()
{
    return m_WaitingAgentQueue.GetSize();
}

void CDepartmentInfo::RemoveFromWAQueue(string agentID, int iQualification, bool writeRedis)
{
    RedisServer_ACD * acdRedisServer = writeRedis ?
        CUtility::GetInstance()->Get_RedisServer_ACD() : NULL;

    if (m_WaitingAgentQueue.Remove(agentID) && acdRedisServer) {
        if (!acdRedisServer->RemoveFromAgentQueue(m_sManagerid, m_sDepartmentName, agentID, iQualification)){
            ;//error log
        }
    }
}

void CDepartmentInfo::QueueToCQueue(bool bPushFront, string callerID, int callerType, 
    int callerPriority, string calldata, bool writeRedis)
{
    m_CallQueue.Remove(callerID);

    CallerPartyInfo info;
    info.partyName = callerID;
    info.partyType = callerType;

    if (bPushFront) {
        m_CallQueue.PushFront(callerID, info, callerPriority);

        if (writeRedis && !calldata.empty()) {
            RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
            if (acdRedisServer && 
                acdRedisServer->PushFrontToCallQueue(m_sManagerid, m_sDepartmentName, callerID, calldata)) {
                //todo error log;
            }
        }

        ////ostringstream os;
        ////os << "user " << callerID << " PushFront to " << m_sDepartmentName << " call queue";
        ////g_Logger.MakeLog(os.str());
    }
    else {
        m_CallQueue.PushBack(callerID, info, callerPriority);

        if (writeRedis && !calldata.empty()) {
            RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
            if (acdRedisServer && 
                acdRedisServer->PushBackToCallQueue(m_sManagerid, m_sDepartmentName, callerID, calldata)) {
                //todo error log;
            }
        }

        ////ostringstream os;
        ////os << "user " << callerID << " PushBack to " << m_sDepartmentName << " call queue";
        ////g_Logger.MakeLog(os.str());
    }
}

int CDepartmentInfo::PopFromCQueue(string& userName, bool writeRedis)
{
    string id;
    CallerPartyInfo info;

    if (!m_CallQueue.PopFront(id, info)){
        return CallParty_UnKnown;
    }
    else {
        if (writeRedis) {
            RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
            if (acdRedisServer &&
                acdRedisServer->RemoveFromCQueue(m_sManagerid, m_sDepartmentName, info.partyName)) {
                //todo error log;
            }
        }

        ////ostringstream os;
        ////os << "user " << info.partyName
        ////    << " PopFront frome " << m_sDepartmentName << " call queue";
        ////g_Logger.MakeLog(os.str());

        userName = info.partyName;
        return info.partyType;
    }
}

void CDepartmentInfo::RemoveFromCQueue(string userName, bool writeRedis)
{
    m_CallQueue.Remove(userName);
    if (writeRedis) {
        RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
        if (acdRedisServer &&
            acdRedisServer->RemoveFromCQueue(m_sManagerid, m_sDepartmentName, userName)) {
            //todo error log;
        }
    }

    ////ostringstream os;
    ////os << "user " << userName 
    ////    << " PopFront frome " << m_sDepartmentName << " call queue";
    ////g_Logger.MakeLog(os.str());
}

int CDepartmentInfo::GetCQueueSize()
{
    return m_CallQueue.GetSize();
}

int CDepartmentInfo::GetAgentNum()
{
	return this->m_mapAgentsInDepart.size();
}

void CDepartmentInfo::func_SendIdleAgents(string invokeID, unsigned ip, unsigned port)
{
	string sTempStr("");
	sTempStr += this->m_sDepartmentName;
	sTempStr += ",";

    CPriorityQueue<pair<string, int> >::Iterator iter = m_WaitingAgentQueue.Begin();
	CAgentInfo* pAgentInfo = NULL;
	string sAgentID = "";
    while (iter != m_WaitingAgentQueue.End()) {
		pAgentInfo = NULL;
        sAgentID = iter->m_Value.first;
		pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
		if ( pAgentInfo == NULL ) {
			iter++;
			continue;
		}

		if (pAgentInfo->m_iLoginType != LoginType_OnlyWeb) {
			iter++;
			continue;
		}
		
        sTempStr += sAgentID;
		sTempStr += "(";

		if(pAgentInfo)
			sTempStr += pAgentInfo->m_sNickName;

		sTempStr += ")";
		sTempStr += ",";

		if(sTempStr.length() > 600)
		{
			SIPCCMsg_ACD_DepartNAgentInfo info;	
			info.invokeID = invokeID;
			info.SetTheInfo(sTempStr);
			info.deptNickname = this->m_sNickname;
			info.bEnd = 0;
			CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);

			sTempStr = "";
			sTempStr += this->m_sDepartmentName;
			sTempStr += ",";
		}
	
		iter++;
	}

	SIPCCMsg_ACD_DepartNAgentInfo info;	
	info.invokeID = invokeID;
	info.SetTheInfo(sTempStr);
	info.deptNickname = this->m_sNickname;
	info.bEnd = 0;
	CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);
}

void CDepartmentInfo::func_SendTXTIdleAgents(string invokeID, unsigned ip, unsigned port)
{
	vector<string> agents;

	string sTempStr;
	for (map<string, string>::iterator it = m_mapAgentsInDepart.begin();
		it != m_mapAgentsInDepart.end(); it++)
	{
		CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(it->first);
		if (pAgentInfo && (pAgentInfo->CheckForMakeCall(E_WEBCHAT_CALL) == AgentCalling_NoError))
		{
			sTempStr += pAgentInfo->m_sAgentID;
			sTempStr += "(";
			sTempStr += pAgentInfo->m_sNickName;
			sTempStr += ")";
			sTempStr += ",";

			if (sTempStr.length() > 600)
			{
				agents.push_back(sTempStr);
				sTempStr = "";
			}
		}
	}

	agents.push_back(sTempStr);
	for (size_t i = 0; i < agents.size(); i++)
	{
		SIPCCMsg_ACD_DepartNAgentInfo info;
		info.invokeID = invokeID;
		info.SetTheInfo(agents[i]);
		info.deptNickname = this->m_sNickname;
		info.bEnd = ((i + 1) < agents.size() ? 0 : 1);
		CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);
	}
}

void CDepartmentInfo::SendOutDialSys_DeptAgentsInfo()
{
	if (!this->m_OutDialSys_Info.bIsOutDialGroup
		|| !this->m_OutDialSys_Info.OutdialEngIp
		|| !this->m_OutDialSys_Info.OutdialEngPort)
	{
		return;
	}
	//若没有座席登入登出，且座席map为空则不发送
	if (!IsmapChanged && m_mapAgentsInDepart.empty())
	{
		return;
	}

	int freeCount = 0;
	int workCount = 0;
	GetAgentCount(freeCount, workCount);
	
	SIPCCMsg_OutDial_SetAgentCount OutDial_SetAgentCount;
	OutDial_SetAgentCount.GroupId = this->m_OutDialSys_Info.GroupId;
	OutDial_SetAgentCount.GroupName = this->m_sDeptid;
	OutDial_SetAgentCount.FreeAgentCount = freeCount;
	OutDial_SetAgentCount.WorkAgentCount = workCount;


	CUtility::GetInstance()->SendIPCCMsg_UDP(
		OutDial_SetAgentCount, m_OutDialSys_Info.OutdialEngIp, m_OutDialSys_Info.OutdialEngPort);

	if (m_mapAgentsInDepart.empty())
	{
		IsmapChanged = false;
	}
	
}

void CDepartmentInfo::GetAgentCount(int &freeCount, int &workCount)
{
	freeCount = 0;
	workCount = 0;

	map<string, string>::iterator pIter;
	string AgentName;
	CAgentInfo* pAgentInfo;
	for(pIter=m_mapAgentsInDepart.begin(); pIter!=m_mapAgentsInDepart.end(); pIter++)
	{
		pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(pIter->first);
		if(!pAgentInfo || pAgentInfo->bExitOutdial)
			continue;

		if(pAgentInfo->m_iCurState == Agent_Waiting)
		{
			freeCount++;
		}
		
		if(pAgentInfo->m_iCurState != Agent_Parking)
		{
			workCount++;
		}
	}
}

inline CAgentInfo* CDepartmentInfo::GetAgentInfo1(map<string, string>::iterator& agentiter) {
    return CAgentMgr::GetInstance()->GetAgentInfo(agentiter->second);
};

inline CAgentInfo* CDepartmentInfo::GetAgentInfo2(string& agentid) {
    return CAgentMgr::GetInstance()->GetAgentInfo(agentid);
};


int CDepartmentInfo::ACDGetTXTCallCount(map<string, string>::iterator& agentiter)
{
    CAgentInfo *pAgent = CAgentMgr::GetInstance()->GetAgentInfo(agentiter->first);
    if (pAgent != NULL && pAgent->CanAcceptTXTCall()) {
        return pAgent->txt_calls.size();
    }
    else {
        return -1;
    }
}

int CDepartmentInfo::ACDGetEmailCallCount(map<string, string>::iterator& agentiter)
{
    CAgentInfo *pAgent = CAgentMgr::GetInstance()->GetAgentInfo(agentiter->first);
    if (pAgent != NULL && pAgent->CanAcceptEmailCall()) {
        return pAgent->GetEmailCallCount();
    }
    else {
        return -1;
    }
}

bool CDepartmentInfo::CanAcceptTXTCall(string& agentid)
{
    CAgentInfo *pAgent = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
    return (pAgent != NULL && pAgent->CanAcceptTXTCall());
}

bool CDepartmentInfo::CanAcceptEmailCall(string& agentid)
{
    CAgentInfo *pAgent = CAgentMgr::GetInstance()->GetAgentInfo(agentid);
    return (pAgent != NULL && pAgent->CanAcceptEmailCall());
}


void  CDepartmentInfo::send_SIPCCMsg_Client_MessageCallInfo(MakeCallInfo makeCallInfo)
{

	if (typeid(boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>> (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo((ECallType)makecallPtr->callType,
				makecallPtr->GetCallID(),
				makecallPtr->GetAgentID(),
				"",
				makeCallInfo.startTime,
				GetLocalTimeForMessageCall(),
				m_sManagerid,
				m_sDeptid,
				makecallPtr->callerType,
				makecallPtr->keyword,
				"", "", 0
				);
		}
	}
	else if (typeid(boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall>> (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo((ECallType)makecallPtr->callType,
				makecallPtr->callID,
				makecallPtr->account,
				"",
				makeCallInfo.startTime,
				GetLocalTimeForMessageCall(),
				m_sManagerid,
				m_sDeptid,
				CallParty_CClient,
				makecallPtr->keyword,
				"", "", 0
				);
		}
	}
}

//=================================================================================================
CSkillGroupInfo::CSkillGroupInfo(string managerid, string sCorpName, string sSkillGroupid, string sSkillGroupName)
    :m_sManagerid(managerid), m_sCorpName(sCorpName)
    , m_sSkillGroupid(sSkillGroupid), m_sSkillGroupName(sSkillGroupName)
{
    m_sRedisAgentQueueName = m_sManagerid + "." + m_sSkillGroupName + ".AgentQueue";
}

CSkillGroupInfo::~CSkillGroupInfo()
{
}

void CSkillGroupInfo::QueueToCQueue(bool bPushFront, string callerID, int callerType, int callerPriority, string calldata, bool writeRedis)
{
    m_CallQueue.Remove(callerID);

    CallerPartyInfo info;
    info.partyName = callerID;
    info.partyType = CallParty_User;

    if (bPushFront) {
        ////ostringstream os;
        ////os << "user " << callerID << " PushFront to " << m_sSkillGroupName << " call queue";
        ////g_Logger.MakeLog(os.str());

        m_CallQueue.PushFront(callerID, info, callerPriority);

        if (!calldata.empty()) {
            RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
            if (acdRedisServer) {
                acdRedisServer->PushFrontToCallQueue(m_sManagerid, m_sSkillGroupName, callerID, calldata);
            }
        }
    }
    else {
        ////ostringstream os;
        ////os << "user " << callerID << " PushBack to " << m_sSkillGroupName << " call queue";
        ////g_Logger.MakeLog(os.str());

        m_CallQueue.PushBack(callerID, info, callerPriority);

        if (!calldata.empty()) {
            RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
            if (acdRedisServer) {
                acdRedisServer->PushBackToCallQueue(m_sManagerid, m_sSkillGroupName, callerID, calldata);
            }
        }
    }
}

int CSkillGroupInfo::PopFromCQueue(string& userName, bool writeRedis)
{
    string id;
    CallerPartyInfo info;

    if (!m_CallQueue.PopFront(id, info)){
        return CallParty_UnKnown;
    }
    else {
        RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
        if (acdRedisServer) {
            acdRedisServer->RemoveFromCQueue(m_sManagerid, m_sSkillGroupName, userName);
        }

        ////ostringstream os;
        ////os << "user " << info.partyName
        ////    << " PopFront frome " << m_sSkillGroupName << " call queue";
        ////g_Logger.MakeLog(os.str());

        userName = info.partyName;
        return info.partyType;
    }
}

void CSkillGroupInfo::RemoveFromCQueue(string userName, bool writeRedis)
{	
    m_CallQueue.Remove(userName);
    if (writeRedis) {
        RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
        if (acdRedisServer && 
            acdRedisServer->RemoveFromCQueue(m_sManagerid, m_sSkillGroupName, userName)) {
            //todo error log;
        }
    }
}

void CSkillGroupInfo::QueueToWAQueue(string agentID, int iQualification, bool writeRedis)
{
    m_WaitingAgentQueue.Remove(agentID);
    m_WaitingAgentQueue.PushBack(agentID, make_pair(agentID, iQualification), iQualification);

    if (writeRedis) {
        RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
        if (acdRedisServer &&
            acdRedisServer->PushBackToAgentQueue(m_sManagerid, m_sSkillGroupName, agentID, iQualification)) {
            //todo error log;
        }
    }
}

void CSkillGroupInfo::RemoveFromWAQueue(string agentID, int iQualification, bool writeRedis)
{
    m_WaitingAgentQueue.Remove(agentID);

    if (writeRedis) {
        RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
        if (acdRedisServer &&
            acdRedisServer->RemoveFromAgentQueue(GetRedisAgentQueueName(), m_sSkillGroupName, agentID, iQualification)) {
            //todo error log;
        }
    }
}

string CSkillGroupInfo::PopFromWAQueue(bool writeRedis)
{
    RedisServer_ACD * acdRedisServer = writeRedis ?
        CUtility::GetInstance()->Get_RedisServer_ACD() : NULL;

    string id;
    pair<string, int> agentIDInfo;
    m_WaitingAgentQueue.PopFront(id, agentIDInfo);

    if (!id.empty() && acdRedisServer &&
        !acdRedisServer->RemoveFromAgentQueue(m_sManagerid, m_sSkillGroupName, agentIDInfo.first, agentIDInfo.second)) {
        //todo error log
    }

    return id;
}

bool CSkillGroupInfo::AgentLogin(string agentID)
{
    if (m_mapAgentsInSkillGroup.size() == 0) {
        LoadQueueFromRedis();
    }

    if (agentID != "") {
        this->m_mapAgentsInSkillGroup[agentID] = agentID;
        return true;
    }

    return false;
}

void CSkillGroupInfo::AgentLogout(string agentID)
{
    RemoveFromWAQueue(agentID);
    m_mapAgentsInSkillGroup.erase(agentID);
}

//顶层接口
pair<int, boost::any> CSkillGroupInfo::CallIn(string callerid, int callertype, void * caller, int priority)
{
    string sAgentID = PopFromWAQueue();
    CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
    while (!sAgentID.empty() && pAgentInfo == NULL) {
        sAgentID = PopFromWAQueue();
        pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
    }

    if (pAgentInfo == NULL) {
        if (caller != NULL) {
            switch (callertype) {
            case CallParty_User:{
                CUserInfo *puser = (CUserInfo*)caller;
                QueueToCQueue(false, callerid, callertype, priority, puser->ToString());
                break;
            }
            case CallParty_CClient: {
                CCClientInfo *pcuser = (CCClientInfo*)caller;
                QueueToCQueue(false, callerid, callertype, priority, pcuser->ToString());
                break;
            }
            default:
                QueueToCQueue(false, callerid, callertype, priority);
            }
        }
        else {
            QueueToCQueue(false, callerid, callertype, priority);
        }
        return make_pair(2, GetCQueueSize());
    }
    else {
        pAgentInfo->m_sCurSkillInfo.id = this->m_sSkillGroupid;
        pAgentInfo->m_sCurSkillInfo.name = this->m_sSkillGroupName;
        if (pAgentInfo->CallIn(callerid, callertype, caller)) {
            return make_pair(1, pAgentInfo);
        }
        else {
            return make_pair(0, 0);
        }
    }
}

int CSkillGroupInfo::GetCQueueSize()
{
    return m_CallQueue.GetSize();
}

int CSkillGroupInfo::GetAgentNum()
{
	return this->m_mapAgentsInSkillGroup.size();
}

string CSkillGroupInfo::FrontOfWAQueue()
{
    string id;
    pair<string, int> agentIDInfo;
    m_WaitingAgentQueue.PopFront(id, agentIDInfo);
    return id;
}

int CSkillGroupInfo::GetWAQueueSize()
{
    return m_WaitingAgentQueue.GetSize();
}

void CSkillGroupInfo::LoadQueueFromRedis()
{
    RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
    if (acdRedisServer) {
        vector<string> queue;
        acdRedisServer->GetCallQueue(m_sManagerid, m_sSkillGroupName, 0, queue);
        for (vector<string>::iterator it = queue.begin(); it != queue.end(); it++) {
            CUserInfo user;
            if (user.FromString(*it)) {
                QueueToCQueue(false, user.m_sUserName, CallParty_User, user.m_iUserLevel);
                CUserMgr::GetInstance()->AddUser(user);
            }
            else {
                CCClientInfo cuser;
                if (cuser.FromString(*it)) {
                    QueueToCQueue(false, user.m_sUserName, CallParty_CClient, cuser.m_iUserLevel);
                    CCClientMgr::GetInstance()->AddCClient(cuser);
                }
                else {
                    //todo log
                }
            }
        }
        queue.clear();

        vector<pair<string, int> > agentQueue;
        acdRedisServer->GetAgentQueue(m_sManagerid, m_sSkillGroupName, 0, agentQueue);
        for (vector<pair<string, int> >::iterator it = agentQueue.begin(); it != agentQueue.end(); it++) {
            QueueToWAQueue(it->first, it->second, false);
        }
    }
}


struct finder
{
	finder(int &minCallCount, string &agentID)
		:m_MinCallCount(minCallCount), m_AgentID(agentID)
	{}

	bool isFree(int state)
	{
		switch (state)
		{
		case Agent_Waiting: case Agent_ForeProcess: case Agent_BackProcess:
			return true;
		default:
			return false;
		}
	}

	void operator() (map<string, string>::value_type & item)
	{
		CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(item.first);

		if (pAgentInfo != NULL &&
			pAgentInfo->CanAcceptTXTCall() &&
			((m_MinCallCount == -1) || (pAgentInfo->m_iTotalTXTCallCount < m_MinCallCount))
			)
		{
			m_MinCallCount = pAgentInfo->m_iTotalTXTCallCount;
			m_AgentID = item.first;
		}
	}

private:
	int &m_MinCallCount;
	string &m_AgentID;
};

void CDepartmentInfo::NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg &notifyMsg)
{
	for (map<string, string>::iterator it = m_mapAgentsInDepart.begin(); it != m_mapAgentsInDepart.end(); it++)
	{
		CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(it->second);

		if (pAgentInfo != NULL)
		{
			pAgentInfo->NotifyMsg(notifyMsg);
		}
	}
}

void CDepartmentInfo::On_SIPCCMsg_CProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long ip, unsigned short port)
{
	if (makecall.callType == E_NORMAL_CALL)
		return;

	int error = MakeCall(makecall.account, CallParty_CClient,
		makecall.targetName, (__CallPartyType)makecall.targetType, makecall.keyword,
		makecall.callID, makecall.callType,
		makecall.uuiDate, "", makecall.audioStreamDescription, "", "",
		ip, port);

	if (error == AgentCalling_AgentBusy)
	{
		error = AgentCalling_AgentQueuing;
		PushBackToCallQueue(makecall, ip, port);
		if (GetTXTCallQueueSize() > MAX_TXT_CALL_QUEUE_SIZE)
		{
			BlockInbound(ip, port);
		}
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = error;
	agentMakeCallConf.SetCalledParty(makecall.targetName);
	agentMakeCallConf.callingAgentID = makecall.account;
	agentMakeCallConf.callID = makecall.callID;
	agentMakeCallConf.callType = makecall.callType;
	CUtility::GetInstance()->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
}

void CDepartmentInfo::On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long ip, unsigned short port)
{
	if (makecall.callType == E_NORMAL_CALL)
		return;

	int error = MakeCall(makecall.GetAgentID(),
		(__CallPartyType)makecall.callerType,
		makecall.GetTargetPosition(), (__CallPartyType)makecall.targetType, makecall.keyword,
		makecall.GetCallID(), makecall.callType,
		makecall.GetUUIData(), makecall.recentContacts,
		makecall.audioStreamDescription,
		makecall.orginalAgentID, makecall.orginalCallID,
		ip, port
		);

	if (error == AgentCalling_AgentBusy)
	{
		error = AgentCalling_AgentQueuing;
        PushBackToCallQueue(makecall, ip, port);      
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = error;
	agentMakeCallConf.SetCalledParty(makecall.GetTargetPosition());
	agentMakeCallConf.callingAgentID = makecall.GetAgentID();
	agentMakeCallConf.callID = makecall.GetCallID();
	agentMakeCallConf.callType = makecall.callType;
	CUtility::GetInstance()->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);
}

int CDepartmentInfo::MakeCall(string caller, __CallPartyType callertype,
	string called, __CallPartyType calledtype, string keyword,
	string callid, ECallType callType,
	string uuidata, string recentContacts, CAudioStreamDescription & asd,
	string originalagentid, string originalcallid,
	unsigned long ip, unsigned short port)
{
	if (callType == E_NORMAL_CALL)
		return AgentCalling_InternalError;

	int error = AgentCalling_AgentBusy;

	CAgentInfo* pAgentInfo_called = NULL;
	if (!recentContacts.empty())
	{
		pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(recentContacts);
	}

    ACDAlgorithm* agentfinderPtr = (callType == E_EMAIL_CALL ?
        m_AgentFinderForEmailCall_ptr_.get() : m_AgentFinderForTXTCall_ptr_.get());

	//若没有最近联系人或未找到，按其它规则查找
	if (NULL == pAgentInfo_called)
	{
        pAgentInfo_called = agentfinderPtr->find();
	}

	for (int i = 0; i < 10; i++)
	{
		if (pAgentInfo_called == NULL)
		{
			error = AgentCalling_AgentBusy;

			break;
		}
		else
		{
			error = pAgentInfo_called->MakeCall(caller, callertype, called, calledtype, keyword,
				callid, callType, uuidata, asd, originalagentid, originalcallid, ip, port, false);
			if (error == AgentCalling_AgentRinging || error == AgentCalling_Repeated)
			{
				break;
			}
		}

        pAgentInfo_called = agentfinderPtr->find();
    }

	return error;
}

void CDepartmentInfo::On_SIPCCMsg_CProxy_HangupCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port)
{
	CallQueue& callQueue = (hangupcall.callType == E_EMAIL_CALL ?
								m_EmailCallQueue : m_TXTCallQueue);
	RemoveFromCQueue(callQueue, hangupcall.callID);

}

void CDepartmentInfo::On_SIPCCMsg_Client_AgentHangupCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port)
{
    CallQueue& callQueue = (hangupcall.callType == E_EMAIL_CALL ? 
                                m_EmailCallQueue : m_TXTCallQueue);
    RemoveFromCQueue(callQueue, hangupcall.GetCallID());

	SIPCCMsg_ACD_CallReleased callReleased;
	callReleased.SetReleasedParty(hangupcall.GetAgentID());
	callReleased.callID = hangupcall.GetCallID();
	callReleased.callType = hangupcall.callType;
	CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased, ip, port);
	
}

void CDepartmentInfo::PushBackToCallQueue( const SIPCCMsg_Client_AgentMakeCall & makecall,
    unsigned long ip, unsigned short port)
{
    boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> msg_cpy(new SIPCCMsg_Client_AgentMakeCall);
    *msg_cpy = makecall;

	CallQueue& callQueue = (makecall.callType == E_EMAIL_CALL ?
								m_EmailCallQueue : m_TXTCallQueue);
    callQueue.push_back(MakeCallInfo(msg_cpy->GetCallID(), msg_cpy, ip, port));

	if (callQueue.size() > MAX_TXT_CALL_QUEUE_SIZE)
	{
		BlockInbound(ip, port);
	}

    stringstream ss;
    ss << "push call to call queue"
        << ", callid:" << makecall.GetCallID()
        << ", caller:" << makecall.GetAgentID()
        << ", called:" << makecall.GetTargetPosition()
		<< ", queue size:" << callQueue.size();
    g_Logger.MakeLog(ss.str());
	
}

void CDepartmentInfo::PushBackToCallQueue(const SIPCCMsg_CProxy_CMakeCall & makecall,
	unsigned long ip, unsigned short port)
{ 
	boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> msg_cpy(new SIPCCMsg_CProxy_CMakeCall);
	*msg_cpy = makecall;

	CallQueue& callQueue = (makecall.callType == E_EMAIL_CALL ?
								m_EmailCallQueue : m_TXTCallQueue);
	callQueue.push_back(MakeCallInfo(msg_cpy->callID, msg_cpy, ip, port));

	if (callQueue.size() > MAX_TXT_CALL_QUEUE_SIZE)
	{
		BlockInbound(ip, port);
	}

	stringstream ss;
	ss << "push call to call queue"
		<< ", callid:" << makecall.callID
		<< ", caller:" << makecall.account
		<< ", called:" << makecall.targetName
		<< ", queue size:" << callQueue.size();
	g_Logger.MakeLog(ss.str());

}

MakeCallInfo CDepartmentInfo::PopFrontFromCallQueue(CallQueue& callqueue)
{
 	
	MakeCallInfo ret;
    if (!callqueue.empty())
	{
        ret = callqueue.front();
        callqueue.pop_front();

		stringstream ss;
		ss << "pop call from call queue"
			<< ", callid:" << ret.callID
			<< ", deptname:" << m_sDepartmentName
            << ", queue size:" << callqueue.size();
		g_Logger.MakeLog(ss.str());
	}

	return ret;
}

bool CDepartmentInfo::RemoveFromCQueue(CallQueue& callqueue, string callID)
{
    for (CallQueue::iterator it = callqueue.begin(); it != callqueue.end(); it++)
	{
		if (it->callID == callID)
		{
			//呼叫记录入库
			this->send_SIPCCMsg_Client_MessageCallInfo(*it);

            callqueue.erase(it);
			stringstream ss;
			ss << "remove call from call queue"
				<< ", callid:" << callID
                << ", queue size:" << callqueue.size();
			g_Logger.MakeLog(ss.str());

			return true;
		}
	}
	return false;
}

void CDepartmentInfo::OnAgentFreeForTXTCall(CAgentInfo * pAgent)
{

    MakeCallInfo makeCallInfo = PopFrontFromCallQueue(m_TXTCallQueue);
	if (makeCallInfo.makeCallMsgPtr.empty())
	{
		//todo
		//if (LoadTXTCallQueueFromRedis() < MAX_TXT_CALL_QUEUE_SIZE &&
		//	txtCallQueueStatus == 1)
		//{
		//	UnblockInbound();
		//}
	}
	
	if (makeCallInfo.makeCallMsgPtr.empty())
	{
		return ;
	}

	int error = AgentCalling_NoError;
	if (typeid(boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> > (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			error = pAgent->MakeCall(*makecallPtr, makeCallInfo.clientIP, makeCallInfo.clientPort);
		}
	}
	else if (typeid(boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> > (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			error = pAgent->MakeCall(*makecallPtr, makeCallInfo.clientIP, makeCallInfo.clientPort);
		}
	}
	else
	{
		error = AgentCalling_InternalError;
	}

	if (error != AgentCalling_AgentRinging)
	{
		stringstream ss;
		ss << "OnAgentFreeForTXTCall error:" << error;
		g_Logger.MakeLog(ss.str());
	}
}

MakeCallInfo::MakeCallInfo(string callID, 
	boost::any makeCallMsgPtr, uint32_t clientIP, uint16_t clientPort)
{
	this->callID = callID;
	this->makeCallMsgPtr = makeCallMsgPtr;
	this->clientIP = clientIP;
	this->clientPort = clientPort;
	this->startTime = GetLocalTimeForMessageCall();
}

boost::shared_ptr<vector<char> > MakeCallInfo::serialize()
{
	char * msgbuf = NULL;
	int msglen = 0;
	if (typeid(boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>) == this->makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> > (this->makeCallMsgPtr);
		if (makecallPtr)
		{
			msglen = makecallPtr->FillBuf(msgbuf);
		}
	}
	else if (typeid(boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall>) == makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> > (this->makeCallMsgPtr);
		if (makecallPtr)
		{
			msglen = makecallPtr->FillBuf(msgbuf);
		}
	}
	else
	{
		g_Logger.MakeLog("MakeCallInfo::serialize error");
	}

	int bufsize = 0;
	bufsize += this->callID.length()+1; //callID
	bufsize += sizeof(uint32_t); //msg size
	bufsize += msglen; //msg
	bufsize += sizeof(uint32_t); //ip
	bufsize += sizeof(uint16_t); //port

	boost::shared_ptr<vector<char> > ret(new vector<char>(bufsize));
	char * bufhead = &(*ret)[0];

	//callID
	memcpy(bufhead, callID.c_str(), callID.length()+1);
	bufhead += callID.length() + 1;

	//msglen
	*reinterpret_cast<uint32_t*>(bufhead) = htonl(msglen);
	bufhead += sizeof(uint32_t);

	//msg
	memcpy(bufhead, msgbuf, msglen);
	bufhead += msglen;

	*reinterpret_cast<uint32_t*>(bufhead) = this->clientIP;
	bufhead += sizeof(uint32_t);

	*reinterpret_cast<uint16_t*>(bufhead) = this->clientPort;
	bufhead += sizeof(uint16_t);

	//startTime
	memcpy(bufhead, startTime.c_str(), startTime.length() + 1);
	bufhead += startTime.length() + 1;

	return ret;
}

bool MakeCallInfo::deserialize(char * buf, int buf_size)
{
	char * bufhead = buf;

	//callID
	callID = bufhead;
	bufhead += callID.length() + 1;

	//msglen
	uint32_t msgsize = ntohl(*reinterpret_cast<uint32_t*>(bufhead));
	bufhead += sizeof(uint32_t);

	//msg
	CIPCCMsgExchanger msgExchanger;
	__EIPCCMsgClass msgClass;
	__EIPCCMsgType msgType;
	msgExchanger.GetMsgHeader(bufhead, msgsize, msgClass, msgType);
	switch (msgType)
	{
	case Type_Client_AgentMakeCall:
	{
		boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> msgptr(new SIPCCMsg_Client_AgentMakeCall);
		msgptr->AnalyzeBuf(bufhead, msgsize);
		makeCallMsgPtr = msgptr;
		break;
	}
	case Type_CProxy_CMakeCall:
	{
		boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> msgptr(new SIPCCMsg_CProxy_CMakeCall);
		msgptr->AnalyzeBuf(bufhead, msgsize);
		makeCallMsgPtr = msgptr;
		break;
	}
	default:
	{
		g_Logger.MakeLog("MakeCallInfo::deserialize error");
		return false;
	}
	}
	bufhead += msgsize;

	this->clientIP = *reinterpret_cast<uint32_t*>(bufhead);
	bufhead += sizeof(uint32_t);

	this->clientPort = *reinterpret_cast<uint16_t*>(bufhead);
	bufhead += sizeof(uint16_t);

	this->startTime = bufhead;
	bufhead += startTime.length() + 1;

	return true;
}

void CDepartmentInfo::BlockInbound(uint32_t ip, uint16_t port)
{
	////////m_cproxyServers.insert(make_pair(ip, port));
	////////txtCallQueueStatus = 1;
}

void CDepartmentInfo::UnblockInbound()
{
	////////set<pair<uint32_t, uint16_t> >::iterator it = m_cproxyServers.begin();
	////////for (; it != m_cproxyServers.end(); it++)
	////////{
	////////	SIPCCMsg_ACD_LoadNotify loadNotify;
	////////	loadNotify.loadLevel = SIPCCMsg_ACD_LoadNotify::FREE;

	////////	CUtility::GetInstance()->SendIPCCMsg_UDP(loadNotify, it->first, it->second);
	////////}
	////////m_cproxyServers.clear();
	////////txtCallQueueStatus = 0;
}

int CDepartmentInfo::LoadTXTCallQueueFromRedis()
{
	////////RedisReplyPtr result;
	////////bool ret = CUtility::GetInstance()->Get_RedisServer_ACD()
	////////	->GetCallQueue(m_sDepartmentName, MAX_TXT_CALL_QUEUE_SIZE, result);
	////////
	////////if (!ret) return 0;

	////////if (result->type == REDIS_REPLY_STRING)
	////////{
	////////	for (int i = 0; i < result->elements; i++)
	////////	{
	////////		string strcallinfo(result->element[i]->str);
	////////		CSpliter sp_callinfo(strcallinfo, "$:%");
	////////		SIPCCMsg_Client_AgentMakeCall makecall;
	////////		makecall.SetCallID(sp_callinfo[0]);
	////////		makecall.callType = (ECallType)boost::lexical_cast<int32_t>((string)sp_callinfo[1]);
	////////		makecall.SetTargetPosition(sp_callinfo[2]);
	////////		makecall.targetType = boost::lexical_cast<int32_t>((string)sp_callinfo[3]);
	////////		makecall.SetAgentID(sp_callinfo[4]);
	////////		makecall.SetUUIData(sp_callinfo[6]);
	////////		makecall.uuiData2 = sp_callinfo[7];
 ////////           PushBackToCallQueue(makecall, 0, 0);
	////////	}
	////////	return result->elements;
	////////}
	return 0;
}
void CDepartmentInfo::LoadQueueFromRedis()
{
    RedisServer_ACD * acdRedisServer = CUtility::GetInstance()->Get_RedisServer_ACD();
    if (acdRedisServer) {
        vector<string> queue;
        if (acdRedisServer->GetCallQueue(m_sManagerid, m_sDepartmentName, 0, queue)) {
            m_CallQueue.Clear();

            stringstream ss;
            ss << "load " << queue.size()
                << " from redis queue " << m_sDepartmentName;
            g_Logger.MakeLog(ss.str());

            for (vector<string>::iterator it = queue.begin(); it != queue.end(); it++) {
                ss.str("");
                ss << "" << m_sDepartmentName << " " << *it;
                g_Logger.MakeLog(ss.str());

                CUserInfo user;
                if (user.FromString(*it)) {
                    g_Logger.MakeLog("````````LoadQueueFromRedis 11111111");
                    QueueToCQueue(false, user.m_sUserName, CallParty_User, user.m_iUserLevel);
                    CUserMgr::GetInstance()->AddUser(user);
                }
                else {
                    g_Logger.MakeLog("````````LoadQueueFromRedis 22222222," + user.ToString());

                    CCClientInfo cuser;
                    if (cuser.FromString(*it)) {
                        QueueToCQueue(false, user.m_sUserName, CallParty_CClient, cuser.m_iUserLevel);
                        CCClientMgr::GetInstance()->AddCClient(cuser);
                    }
                    else {
                        //todo log
                    }
                }
            }
        }

        vector<pair<string, int>> agentQueue;
        acdRedisServer->GetAgentQueue(m_sManagerid, m_sDepartmentName, 0, agentQueue);
        for (vector<pair<string, int>>::iterator it = agentQueue.begin(); it != agentQueue.end(); it++) {
            QueueToWAQueue(it->first, it->second, false);
        }
    }
}

//返回值: 0:参数或内部错误， 1：呼叫分配成功； 2：呼叫排队
//  first：0，second:空
//  first：1，second:CAgentInfo* 分配到的座席
//  first：2，second:size_t 队列长度
//  
pair<int, boost::any> CDepartmentInfo::CallIn(string callerid, int callertype, void * caller, int priority)
{
    string sAgentID = PopFromWAQueue();
    CAgentInfo* pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
    while (!sAgentID.empty() && pAgentInfo == NULL) {
        sAgentID = PopFromWAQueue();
        pAgentInfo = CAgentMgr::GetInstance()->GetAgentInfo(sAgentID);
    }

    if (pAgentInfo == NULL) {
        if (caller != NULL) {
            switch (callertype) {
            case CallParty_User:{
                CUserInfo *puser = (CUserInfo*)caller;
                QueueToCQueue(false, callerid, callertype, priority, puser->ToString());
                break;
            }
            case CallParty_CClient: {
                CCClientInfo *pcuser = (CCClientInfo*)caller;
                QueueToCQueue(false, callerid, callertype, priority, pcuser->ToString());
                break;
            }
            default:
                QueueToCQueue(false, callerid, callertype, priority);
            }
        }
        else {
            QueueToCQueue(false, callerid, callertype, priority);
        }
        return make_pair(2, GetCQueueSize());
    }
    else {
        if (pAgentInfo->CallIn(callerid, callertype, caller)) {
            return make_pair(1, pAgentInfo);
        }
        else {
            return make_pair(0, 0);
        }
    }
}

CSkillGroupTXTCall::CSkillGroupTXTCall()
{
	m_iQueuingAlgorithm = 1;
    m_AgentFinderForTXTCall_ptr_.reset(new ACDMincalls<map<string, CAgentInfo*> >(
        m_AgentMap, boost::bind(&CSkillGroupTXTCall::ACDGetTXTCallCount, this, _1)
        , boost::bind(&CSkillGroupTXTCall::GetAgentInfo1, this, _1)));
    m_AgentFinderForEmailCall_ptr_.reset(new ACDMincalls<map<string, CAgentInfo*> >(
        m_AgentMap, boost::bind(&CSkillGroupTXTCall::ACDGetEmailCallCount, this, _1)
        , boost::bind(&CSkillGroupTXTCall::GetAgentInfo1, this, _1)));
}

void CSkillGroupTXTCall::AgentLogin(CAgentInfo * agent_ptr)
{
	m_AgentMap[agent_ptr->m_sAgentID] = agent_ptr;
}

void CSkillGroupTXTCall::AgentLogout(CAgentInfo * agent_ptr)
{
	m_AgentMap.erase(agent_ptr->m_sAgentID);
}

void CSkillGroupTXTCall::SetQueuingAlgorithm(int32_t iQueuingAlgorithm)
{
    if (m_iQueuingAlgorithm != iQueuingAlgorithm)
    {
        if (iQueuingAlgorithm == 1)
        {
            m_AgentFinderForTXTCall_ptr_.reset(new ACDMincalls<map<string, CAgentInfo*> >(
                m_AgentMap, boost::bind(&CSkillGroupTXTCall::ACDGetTXTCallCount, this, _1)
                , boost::bind(&CSkillGroupTXTCall::GetAgentInfo1, this, _1)));
            m_AgentFinderForEmailCall_ptr_.reset(new ACDMincalls<map<string, CAgentInfo*> >(
                m_AgentMap, boost::bind(&CSkillGroupTXTCall::ACDGetEmailCallCount, this, _1)
                , boost::bind(&CSkillGroupTXTCall::GetAgentInfo1, this, _1)));
        }
        else
        {
            m_AgentFinderForTXTCall_ptr_.reset(new ACDPoll<CAgentInfo*>(m_AgentMap,
                boost::bind(&CSkillGroupTXTCall::CanAcceptTXTCall, this, _1)
                , boost::bind(&CSkillGroupTXTCall::GetAgentInfo2, this, _1)));
            m_AgentFinderForEmailCall_ptr_.reset(new ACDPoll<CAgentInfo*>(m_AgentMap,
                boost::bind(&CSkillGroupTXTCall::CanAcceptEmailCall, this, _1)
                , boost::bind(&CSkillGroupTXTCall::GetAgentInfo2, this, _1)));
        }
	    m_iQueuingAlgorithm = iQueuingAlgorithm;
    }
}

int CSkillGroupTXTCall::func_CallIn(SIPCCMsg_CProxy_CMakeCall & makecall,
	unsigned long ip, unsigned short port)
{
	int error = AgentCalling_InternalError;
	if (makecall.callType != E_NORMAL_CALL)
	{
		error = MakeCall(makecall.account, CallParty_CClient,
			makecall.targetName, (__CallPartyType)makecall.targetType, makecall.keyword,
			makecall.callID, makecall.callType, makecall.uuiDate, "", "", "",
			ip, port);

		if (error == AgentCalling_AgentBusy)
		{
			error = AgentCalling_AgentQueuing;
            PushBackToCallQueue(makecall, ip, port);
		}
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = error;
	agentMakeCallConf.SetCalledParty(makecall.targetName);
	agentMakeCallConf.callingAgentID = makecall.account;
	agentMakeCallConf.callID = makecall.callID;
	agentMakeCallConf.callType = makecall.callType;
	CUtility::GetInstance()->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

	return error;
}

int CSkillGroupTXTCall::func_CallIn(SIPCCMsg_Client_AgentMakeCall & makecall,
	unsigned long ip, unsigned short port)
{
	int error = AgentCalling_InternalError;
	if (makecall.callType != E_NORMAL_CALL)
	{
		error = MakeCall(makecall.GetAgentID(), (__CallPartyType)makecall.callerType,
			makecall.GetTargetPosition(), (__CallPartyType)makecall.targetType, makecall.keyword,
			makecall.GetCallID(), makecall.callType, makecall.GetUUIData(), makecall.recentContacts,
			makecall.orginalAgentID, makecall.orginalCallID,
			ip, port
			);

		if (error == AgentCalling_AgentBusy)
		{
			error = AgentCalling_AgentQueuing;
            PushBackToCallQueue(makecall, ip, port);
		}
	}

	SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
	agentMakeCallConf.callingState = error;
	agentMakeCallConf.SetCalledParty(makecall.GetTargetPosition());
	agentMakeCallConf.callingAgentID = makecall.GetAgentID();
	agentMakeCallConf.callID = makecall.GetCallID();
	agentMakeCallConf.callType = makecall.callType;
	CUtility::GetInstance()->SendIPCCMsg_UDP(agentMakeCallConf, ip, port);

	return error;
}

int CSkillGroupTXTCall::MakeCall(string caller, __CallPartyType callertype,
	string called, __CallPartyType calledtype, string keyword,
	string callid, ECallType callType, string uuidata, string recentContacts,
	string originalagentid, string originalcallid,
	unsigned long ip, unsigned short port)
{
	int error = AgentCalling_NoError;

	CAgentInfo* pAgentInfo_called = NULL;
	if (!recentContacts.empty())
	{
		pAgentInfo_called = CAgentMgr::GetInstance()->GetAgentInfo(recentContacts);
	}

    ACDAlgorithm* agentfinderPtr = (callType == E_EMAIL_CALL ? 
        m_AgentFinderForEmailCall_ptr_.get() : m_AgentFinderForTXTCall_ptr_.get());
	
    //若没有最近联系人或未找到，按其它规则查找
	if (NULL == pAgentInfo_called)
	{
        pAgentInfo_called = agentfinderPtr->find();
	}

	CAudioStreamDescription asd;
	for (int i = 0; i < 10; i++)
	{
		if (pAgentInfo_called == NULL)
		{
			error = AgentCalling_AgentBusy;

			break;
		}
		else
		{
			error = pAgentInfo_called->MakeCall(caller, callertype, called, calledtype, keyword,
				callid, callType, uuidata, asd, originalagentid, originalcallid, ip, port, false);
			if (error == AgentCalling_AgentRinging || error == AgentCalling_Repeated)
			{
				break;
			}
		}

        pAgentInfo_called = agentfinderPtr->find();
    }

	return error;
}

void CSkillGroupTXTCall::On_SIPCCMsg_CProxy_HangupCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long ip, unsigned short port)
{
	CallQueue& callQueue = (hangupcall.callType == E_EMAIL_CALL ?
								m_EmailCallQueue : m_TXTCallQueue);
	RemoveFromCQueue(callQueue, hangupcall.callID);

}

void CSkillGroupTXTCall::On_SIPCCMsg_Client_AgentHangupCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long ip, unsigned short port)
{
    CallQueue& callQueue = (hangupcall.callType == E_EMAIL_CALL ?
                                m_EmailCallQueue : m_TXTCallQueue);
    RemoveFromCQueue(callQueue, hangupcall.GetCallID());


	//???? CallReleased ???????????
	SIPCCMsg_ACD_CallReleased callReleased;
	callReleased.SetReleasedParty(hangupcall.GetAgentID());
	callReleased.callID = hangupcall.GetCallID();
	callReleased.callType = hangupcall.callType;
	CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased, ip, port);
}

void CSkillGroupTXTCall::PushBackToCallQueue(const SIPCCMsg_Client_AgentMakeCall & makecall,
	unsigned long ip, unsigned short port)
{
	boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> msg_cpy(new SIPCCMsg_Client_AgentMakeCall);
	*msg_cpy = makecall;

	CallQueue& callQueue = (makecall.callType == E_EMAIL_CALL ?
								m_EmailCallQueue : m_TXTCallQueue);
	callQueue.push_back(MakeCallInfo(msg_cpy->GetCallID(), msg_cpy, ip, port));

	if (callQueue.size() > MAX_TXT_CALL_QUEUE_SIZE)
	{
		BlockInbound(ip, port);
	}
	stringstream ss;
	ss << "push call to call queue"
		<< ", callid:" << makecall.GetCallID()
		<< ", caller:" << makecall.GetAgentID()
		<< ", called:" << makecall.GetTargetPosition()
		<< ", queueid:" << m_sSkillGroupid
		<< ", queuename:" << m_sSkillGroupName
		<< ", queue size:" << callQueue.size();
	g_Logger.MakeLog(ss.str());

}

void CSkillGroupTXTCall::PushBackToCallQueue(const SIPCCMsg_CProxy_CMakeCall & makecall,
	unsigned long ip, unsigned short port)
{
	boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> msg_cpy(new SIPCCMsg_CProxy_CMakeCall);
	*msg_cpy = makecall;

	CallQueue& callQueue = (makecall.callType == E_EMAIL_CALL ?
								m_EmailCallQueue : m_TXTCallQueue);
	callQueue.push_back(MakeCallInfo(msg_cpy->callID, msg_cpy, ip, port));

	if (callQueue.size() > MAX_TXT_CALL_QUEUE_SIZE)
	{
		BlockInbound(ip, port);
	}

	stringstream ss;
	ss << "push call to call queue"
		<< ", callid:" << makecall.callID
		<< ", caller:" << makecall.account
		<< ", called:" << makecall.targetName
		<< ", queueid:" << m_sSkillGroupid
		<< ", queuename:" << m_sSkillGroupName
		<< ", queue size:" << callQueue.size();
	g_Logger.MakeLog(ss.str());

}

MakeCallInfo CSkillGroupTXTCall::PopFrontFromCallQueue(CallQueue& callqueue)
{
	MakeCallInfo ret;
    if (!callqueue.empty())
	{
        ret = callqueue.front();
        callqueue.pop_front();

		stringstream ss;
		ss << "pop call from call queue"
			<< ", callid:" << ret.callID
			<< ", queueid:" << m_sSkillGroupid
			<< ", queuename:" << m_sSkillGroupName
            << ", queue size:" << callqueue.size();
		g_Logger.MakeLog(ss.str());
	}

	return ret;
}

bool CSkillGroupTXTCall::RemoveFromCQueue(CallQueue& callqueue, string callID)
{
    for (CallQueue::iterator it = callqueue.begin(); it != callqueue.end(); it++)
	{
		if (it->callID == callID)
		{
			//呼叫记录入库
			this->send_SIPCCMsg_Client_MessageCallInfo(*it);

            callqueue.erase(it);
			stringstream ss;
			ss << "remove call from call queue"
				<< ", queueid:" << m_sSkillGroupid
				<< ", queuename:" << m_sSkillGroupName
				<< ", callid:" << callID
                << ", queue size:" << callqueue.size();
			g_Logger.MakeLog(ss.str());

			return true;
		}
	}
	return false;
}

void CSkillGroupTXTCall::BlockInbound(uint32_t ip, uint16_t port)
{
	//////m_cproxyServers.insert(make_pair(ip, port));
	//////txtCallQueueStatus = 1;
}

void CSkillGroupTXTCall::UnblockInbound()
{
	//////set<pair<uint32_t, uint16_t> >::iterator it = m_cproxyServers.begin();
	//////for (; it != m_cproxyServers.end(); it++)
	//////{
	//////	SIPCCMsg_ACD_LoadNotify loadNotify;
	//////	loadNotify.loadLevel = SIPCCMsg_ACD_LoadNotify::FREE;

	//////	CUtility::GetInstance()->SendIPCCMsg_UDP(loadNotify, it->first, it->second);
	//////}
	//////m_cproxyServers.clear();
	//////txtCallQueueStatus = 0;
}

void CSkillGroupTXTCall::AgentWait(CAgentInfo * pAgent)
{
    MakeCallInfo makeCallInfo = PopFrontFromCallQueue(m_TXTCallQueue);
	if (makeCallInfo.makeCallMsgPtr.empty())
	{
		//todo
		//if (LoadTXTCallQueueFromRedis() < MAX_TXT_CALL_QUEUE_SIZE &&
		//	txtCallQueueStatus == 1)
		//{
		//	UnblockInbound();
		//}
	}

	if (makeCallInfo.makeCallMsgPtr.empty())
	{
		return;
	}

	int error = AgentCalling_NoError;
	if (typeid(boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> > (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			error = pAgent->MakeCall(*makecallPtr, makeCallInfo.clientIP, makeCallInfo.clientPort);			
		}
	}
	else if (typeid(SIPCCMsg_CProxy_CMakeCall) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> > (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			error = pAgent->MakeCall(*makecallPtr, makeCallInfo.clientIP, makeCallInfo.clientPort);
		}
	}
	else
	{
		error = AgentCalling_InternalError;
	}

	if (error != AgentCalling_AgentRinging)
	{
		stringstream ss;
		ss << "OnAgentFreeForTXTCall error:" << error;
		g_Logger.MakeLog(ss.str());
	}
}

void CSkillGroupTXTCall::func_SendIdleAgents(string invokeID, unsigned ip, unsigned port)
{
	vector<string> agents;

	string sTempStr;
	for (map<string, CAgentInfo*>::iterator it = m_AgentMap.begin();
		it != m_AgentMap.end(); it++)
	{
		CAgentInfo* pAgentInfo = it->second;
		if (pAgentInfo && (pAgentInfo->CheckForMakeCall(E_WEBCHAT_CALL) == AgentCalling_NoError))
		{
			sTempStr += pAgentInfo->m_sAgentID;
			sTempStr += "(";
			sTempStr += pAgentInfo->m_sNickName;
			sTempStr += ")";
			sTempStr += ",";

			if (sTempStr.length() > 600)
			{
				agents.push_back(sTempStr);
				sTempStr = "";
			}
		}
	}

	if (!sTempStr.empty())
	{
		agents.push_back(sTempStr);
	}

	for (size_t i = 0; i < agents.size(); i++)
	{
		SIPCCMsg_ACD_DepartNAgentInfo info;
		info.invokeID = invokeID;
		info.SetTheInfo(agents[i]);
		info.deptNickname = this->m_sSkillGroupName;
		info.bEnd = ((i + 1) < agents.size() ? 0 : 1);
		CUtility::GetInstance()->SendIPCCMsg_UDP(info, ip, port);
	}
}

inline CAgentInfo* CSkillGroupTXTCall::GetAgentInfo1(map<string, CAgentInfo*>::iterator& agentIter)
{
    return agentIter->second;
}

inline CAgentInfo* CSkillGroupTXTCall::GetAgentInfo2(CAgentInfo*& agentPtr)
{
    return agentPtr;
}

inline int CSkillGroupTXTCall::ACDGetTXTCallCount(map<string, CAgentInfo*>::iterator& agentIter)
{
    if (agentIter->second == NULL || !agentIter->second->CanAcceptTXTCall()) {
        return -1;
    }
    else {
        return agentIter->second->txt_calls.size();
    }
}

inline int CSkillGroupTXTCall::ACDGetEmailCallCount(map<string, CAgentInfo*>::iterator& agentIter)
{
    if (agentIter->second == NULL || !agentIter->second->CanAcceptEmailCall()) {
        return -1;
    }
    else {
        return agentIter->second->GetEmailCallCount();
    }
}

inline bool CSkillGroupTXTCall::CanAcceptTXTCall(CAgentInfo*& agentid)
{
    return (agentid != NULL && agentid->CanAcceptTXTCall());
}

inline bool CSkillGroupTXTCall::CanAcceptEmailCall(CAgentInfo*& agentid)
{
    return (agentid != NULL && agentid->CanAcceptEmailCall());
}

void  CSkillGroupTXTCall::send_SIPCCMsg_Client_MessageCallInfo(MakeCallInfo makeCallInfo)
{

	if (typeid(boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_Client_AgentMakeCall>> (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo((ECallType)makecallPtr->callType,
				makecallPtr->GetCallID(),
				makecallPtr->GetAgentID(),
				"",
				makeCallInfo.startTime,
				GetLocalTimeForMessageCall(),
				m_sManagerid,
				m_sSkillGroupid,
				makecallPtr->callerType,
				makecallPtr->keyword,
				m_sSkillGroupid, m_sSkillGroupName, 0
				);
		}
	}
	else if (typeid(boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall>) == makeCallInfo.makeCallMsgPtr.type())
	{
		boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall> makecallPtr =
			boost::any_cast<boost::shared_ptr<SIPCCMsg_CProxy_CMakeCall>> (makeCallInfo.makeCallMsgPtr);
		if (makecallPtr)
		{
			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo((ECallType)makecallPtr->callType,
				makecallPtr->callID,
				makecallPtr->account,
				"",
				makeCallInfo.startTime,
				GetLocalTimeForMessageCall(),
				m_sManagerid,
				m_sSkillGroupid,
				CallParty_CClient,
				makecallPtr->keyword,
				m_sSkillGroupid, m_sSkillGroupName, 0
				);
		}
	}
}

