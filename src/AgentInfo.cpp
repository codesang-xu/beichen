// AgentInfo.cpp: implementation of the CAgentInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "AgentInfo.h"

#include <map>

#include "AgentMgr.h"
#include "CallMgr.h"
#include "CClientMgr.h"
#include "common/ipccmsg_helper.h"
#include "common/ipccmsg_dump.h"
#include "CorporationMgr.h"
#include "DepartmentInfo.h"
#include "public/GlobalOperation.h"
#include "public/MemLog.h"
#include "public/Spliter.h"
#include "UserMgr.h"
#include "Utility.h"

using namespace ACDServer;
using namespace ib_common;

extern CMemLog   g_Logger;
#include "Log.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgentInfo::CAgentInfo()
: m_iParkReasonID(0)
{
	m_sAgentID = "";
	//m_sDepartment = "";
	m_ulAgentIP = 0;
	m_usAgentPort = 0;
	m_sCallID = "";
	m_iCurState = 0;				//座席当前状态
	m_sCurStateBeginTime = "";		//当前状态开始时间
	m_iPreState = 0;				//座席前一状态
	m_sPreStateBeginTime = "";		//当前状态开始时间
	m_iPreHeartBeatMark = gettickcount();
	m_iRingStateMark_OnlyPSTN = -1;
	this->bAgentDormancy = false;
	
	this->m_iLoginType = LoginType_OnlyWeb;
	
	this->m_iCalledPartyType = CalledPartyType_UnKnown;

    m_bCallFromVDN = false;
    m_bCallFromQueue = false;
    m_iTalkBeginTime = 0;

	bExitOutdial = false;

    ClearConferenceParam();

	m_sCurSkillInfo.id = "";
	m_sCurSkillInfo.name = "";

	m_sOriSkillInfo.id = "";
	m_sOriSkillInfo.name = "";

	m_bSendHangupCall = false;
	m_bNeedHeartBeat = false;

	m_bNeedCheck = false;

	m_bLoadFromRedis = false;
	m_ulLoadInfoTime = 0;

	m_iTotalTXTCallCount = 0;
	m_bAllowAudioCall = true;
	m_bAllowTXTCall = true;
	m_iMaxTXTDialogCount = 5;
    m_iEmailCallCount = 0;

	m_iLoginPlatform = OSType_WindowsPC;
	txtSessionTimeout = 7220; //默认2小时
    PushFlag = 0;
}


CAgentInfo::~CAgentInfo()
{
}

void CAgentInfo::SetState(int state, bool bIsAgentLogin)
{
    if (m_iCurState == state) return;

    if (bIsAgentLogin) {
        CDepartmentInfo* pDepart =
            CCorporationMgr::GetInstance()->GetDepartmentInfo(this->m_sCorporation, m_sDepartment);	

        if (pDepart == NULL)
            return ;

        pDepart->AgentLogin(m_sAgentID);

        LoginAllSkillGroup();
    }

    if (m_iCurState == Agent_BackProcess)
    {
        CDepartmentInfo* pDept = IsInOutDialDepartment(m_sDepartment);
        if (pDept)
        {
            //结束处理时发送 OutDialSys 消息
            SIPCCMsg_OutDial_EndProcess	OutDial_EndProcess;
            OutDial_EndProcess.TaskId = this->m_OutDialSys_Info.TaskId;
            OutDial_EndProcess.PhoneId = this->m_OutDialSys_Info.PhoneId;
            OutDial_EndProcess.DialSeq = this->m_OutDialSys_Info.DialSeq;
            OutDial_EndProcess.AgentName = this->m_sAgentID;

            CUtility::GetInstance()->SendIPCCMsg_UDP(
                OutDial_EndProcess, pDept->m_OutDialSys_Info.OutdialEngIp, pDept->m_OutDialSys_Info.OutdialEngPort);
        }
    }

    this->m_iPreState = this->m_iCurState;
    this->m_iCurState = state;

    if (this->m_iPreState == Agent_Waiting || state == Agent_Waiting) {
        CUtility::GetInstance()->OutDial_SetAgentCount(this->m_sCorporation, m_sDepartment);
    }

    if (state == Agent_Ringing && this->m_iLoginType == LoginType_OnlyPSTN) {
        m_iRingStateMark_OnlyPSTN = gettickcount();
    }
    else {
        m_iRingStateMark_OnlyPSTN = -1;
    }

    if (state == Agent_Waiting || state == Agent_BackProcess)
    {
        m_sCalledDepart = "";
        m_sCalledParty = "";
        m_sTheOtherAgentID = "";
        m_sCalledNo = "";
        m_iCalledPartyType = CalledPartyType_UnKnown;
        m_userInfo.ClearCallFailed();
        m_sPrivateData = "";
        m_WebPstnAgentCurGwLine = "";
        m_CurGwUserLineNo = "";
        m_bSendHangupCall = false;
        m_bPSTNHungup = false;
        m_bCallFromVDN = false;
        m_bCallFromQueue = false;
        m_sCurSkillInfo.id = "";
        m_sCurSkillInfo.name = "";
    }

    //应答时长
    if ((state == Agent_Talking || state == Agent_OpTalking) && m_iPreState == Agent_Ringing)
    {
        this->m_iAnswerTimeSpan = CalTimeSpan(m_iStateBeginTime, gettickcount());
        m_iTalkBeginTime = gettickcount();
    }

    this->m_sPreStateBeginTime = this->m_sCurStateBeginTime;
    boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
    string strTime = boost::gregorian::to_iso_extended_string(pt.date()) + " " + boost::posix_time::to_simple_string(pt.time_of_day());
    this->m_sCurStateBeginTime = strTime;// CLog::GetLocalTime();
    m_iStateBeginTime = gettickcount();

    if (m_iPreState == Agent_Waiting && state != Agent_Waiting) {
        func_AgentBusy();
    }
    if (state == Agent_Waiting) {
        func_AgentWaiting();
    }

    SelectTXTCall();

    //[发送座席状态信息给数据采集服务器,通话状态需要带上用户名/关键字
    SIPCCMsg_ACD_AgentStateInfo agentStateInfo;
    agentStateInfo.SetAgentID(this->m_sAgentID);
    agentStateInfo.SetCorporationName(this->m_sCorporation);
    agentStateInfo.SetTheDepartment(this->m_sDepartment);
    agentStateInfo.managerid = this->m_sManagerid;
    agentStateInfo.deptid = this->m_sDeptid;
    agentStateInfo.agentState = state;
    agentStateInfo.bAgentLogin = bIsAgentLogin;
    agentStateInfo.SetCurTime(m_sCurStateBeginTime);

    if (m_iPreState == Agent_DialUnderHold || m_iPreState == Agent_TalkUnderHold || m_iPreState == Agent_Holding)
    {
        agentStateInfo.bCallIn = this->bCallin;
        agentStateInfo.bInnerCall = this->bInnerCall;
        agentStateInfo.bPSTNCall = this->bPSTNCall;
        agentStateInfo.bCallOutTypeUnderHold = this->bInnerCall_Hold;
    }
    else
    {
        agentStateInfo.bCallIn = this->bCallin;
        agentStateInfo.bInnerCall = this->bInnerCall;
        agentStateInfo.bPSTNCall = this->bPSTNCall;
    }

    if (state == Agent_Parking)
    {
        agentStateInfo.parkReasonID = this->m_iParkReasonID;
        agentStateInfo.SetParkReason(this->m_sParkReason);
    }

    agentStateInfo.SetCallID(this->m_sCallID);
    agentStateInfo.SetKeyWord("");
    agentStateInfo.SetUserName("");
    agentStateInfo.nickName = this->m_sNickName;
    agentStateInfo.extNo = this->m_sExtNo;
    agentStateInfo.SkillGroupName = this->m_sCurSkillInfo.name;
    agentStateInfo.SkillGroupID = this->m_sCurSkillInfo.id;
    agentStateInfo.UserLevel = -1;
    if (agentStateInfo.SkillGroupID != "")
    {
        if (state == Agent_Ringing || state == Agent_Talking || state == Agent_Holding || state == Agent_DialUnderHold || state == Agent_TalkUnderHold)
        {
            agentStateInfo.UserLevel = this->m_userInfo.m_iUserLevel;
        }
    }

    if (state != Agent_Talking)
    {
        this->m_sOriSkillInfo.id = "";
        this->m_sOriSkillInfo.name = "";
    }

    agentStateInfo.SkillGroupName_Original = this->m_sOriSkillInfo.name;
    agentStateInfo.SkillGroupID_Original = this->m_sOriSkillInfo.id;

    if (!((state == Agent_ConferencePreside) || (state == Agent_ConferenceJoin)))
    {
        //g_Logger.MakeLog(CUtility::GetInstance()->m_sLogTransSign2 + CUtility::GetInstance()->GetMsgLogTransContext(&agentStateInfo));
        agentStateInfo.sData = "";
        CUtility::GetInstance()->SendIPCCMsg_UDP(agentStateInfo, CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP, CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);
		CUtility::GetInstance()->SendToMonitorServer(agentStateInfo);
    }

    //]发送座席状态信息给数据采集服务器,通话状态需要带上用户名/关键字

    if (state == Agent_NoLogin) {
        ClearTXTCalls();

        CDepartmentInfo* pDepart =
            CCorporationMgr::GetInstance()->GetDepartmentInfo(this->m_sCorporation, m_sDepartment);

        if (pDepart != NULL) {
            pDepart->AgentLogout(m_sAgentID);
        }

        LogoutAllSkillGroup();
        LogoutAllTXTSkillGroup();
    }
}

int CAgentInfo::GetCurState()
{
    return m_iCurState;
}

void CAgentInfo::UpdateUserInfo(CUserInfo *pUserInfo)
{
    m_userInfo = *pUserInfo;
}

string CAgentInfo::MakeCorpNDepart()
{
    return this->m_sCorporation + "%^#" + this->m_sDepartment;
}

void CAgentInfo::SetDormancy(bool bDormancy)
{
    this->bAgentDormancy = bDormancy;
    if (bDormancy)
        this->m_iDormancyBeginTime = gettickcount();
}

void CAgentInfo::AgentRelive()
{
    if (m_iCurState == Agent_Waiting)
    {
        SetState(Agent_Waiting);
        return;
    }

    //发waiting状态给报表和监控
    SIPCCMsg_ACD_AgentStateInfo agentStateInfo;
    agentStateInfo.SetAgentID(this->m_sAgentID);
    agentStateInfo.SetCorporationName(this->m_sCorporation);
    agentStateInfo.SetTheDepartment(this->m_sDepartment);
    agentStateInfo.managerid = this->m_sManagerid;
    agentStateInfo.deptid = this->m_sDeptid;
    agentStateInfo.agentState = Agent_Waiting;
    //agentStateInfo.SetCurTime(m_sCurStateBeginTime);
    agentStateInfo.SetCurTime(CLog::GetLocalTime());
    agentStateInfo.SetCallID("");
    agentStateInfo.SetKeyWord("");
    agentStateInfo.SetUserName("");
    agentStateInfo.nickName = this->m_sNickName;


    //begin  add code by hyj 2013-8-27
    //////string slogtrans = CUtility::GetInstance()->GetMsgLogTransContext(&agentStateInfo);
    agentStateInfo.sData = "";
    //////string strlog = CUtility::GetInstance()->m_sLogTransSign2 + slogtrans;
    //////g_Logger.MakeLog(strlog);
    //end --hyj
    CUtility::GetInstance()->SendIPCCMsg_UDP(
        agentStateInfo,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

	CUtility::GetInstance()->SendToMonitorServer(agentStateInfo);

    //当前状态发送
    SIPCCMsg_ACD_AgentStateInfo agentStateInfo1;
    agentStateInfo1.SetAgentID(this->m_sAgentID);
    agentStateInfo1.SetCorporationName(this->m_sCorporation);
    agentStateInfo1.SetTheDepartment(this->m_sDepartment);
    agentStateInfo.managerid = this->m_sManagerid;
    agentStateInfo.deptid = this->m_sDeptid;
    agentStateInfo1.agentState = m_iCurState;
    agentStateInfo1.SetCurTime(m_sCurStateBeginTime);

    /*if(m_iCurState==Agent_Ringing || m_iCurState==Agent_Dialing || m_iCurState==Agent_Talking || m_iCurState==Agent_OpTalking)
    {
    agentStateInfo1.bCallIn = this->bCallin;
    agentStateInfo1.bInnerCall = this->bInnerCall;
    agentStateInfo1.bPSTNCall = this->bPSTNCall;
    }*/

    if (m_iCurState == Agent_DialUnderHold || m_iCurState == Agent_TalkUnderHold)
    {
        agentStateInfo1.bCallIn = this->bCallin_Hold;
        agentStateInfo1.bInnerCall = this->bInnerCall_Hold;
        agentStateInfo1.bPSTNCall = this->bPSTNCall_Hold;
    }
    else
    {
        agentStateInfo1.bCallIn = this->bCallin;
        agentStateInfo1.bInnerCall = this->bInnerCall;
        agentStateInfo1.bPSTNCall = this->bPSTNCall;
    }

    agentStateInfo1.SetCallID("");
    agentStateInfo1.SetKeyWord("");
    agentStateInfo1.SetUserName("");
    agentStateInfo1.nickName = this->m_sNickName;


    //begin  add code by hyj 2013-8-27
    //string slogtrans1 = CUtility::GetInstance()->GetMsgLogTransContext(&agentStateInfo1);
    agentStateInfo1.sData = "";
    //string strlog1 = CUtility::GetInstance()->m_sLogTransSign2 + slogtrans;
    //g_Logger.MakeLog(strlog1);
    //end --hyj

    CUtility::GetInstance()->SendIPCCMsg_UDP(
        agentStateInfo1,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_IP,
        CUtility::GetInstance()->m_IPCCConfig.DataCollectServer_Port);

	CUtility::GetInstance()->SendToMonitorServer(agentStateInfo1);
}

void CAgentInfo::SetCallID(string callid)
{
    this->m_sCallID = callid;
}

void CAgentInfo::SetClientAgentState(int state, int operType, string gwAgentLine, string gwUserLine, string transedAgentID, string transedGwAgentLine, unsigned long gwAgentLineIP, unsigned short gwAgentLinePort, unsigned long gwUserLineIP, unsigned short gwUserLinePort, unsigned long transedGwAgentLineIP, unsigned short transedGwAgentLinePort)
{
    if (this->m_iLoginType == LoginType_OnlyPSTN)
        return;
    /*
    SIPCCMsg_ACD_AgentQueryStateConf agentQueryStateConf;
    agentQueryStateConf.agentState = state;
    agentQueryStateConf.bQuerySucess = 1;
    CUtility::GetInstance()->SendIPCCMsg_UDP(agentQueryStateConf, this->m_ulAgentIP, this->m_usAgentPort);
    */

    SIPCCMsg_ACD_SetClientAgentState SetClientAgentState;
    SetClientAgentState.agentState = state;
    SetClientAgentState.operType = operType;
    SetClientAgentState.gwAgentLine = gwAgentLine;
    SetClientAgentState.gwUserLine = gwUserLine;
    SetClientAgentState.transedAgentID = transedAgentID;
    SetClientAgentState.transedGwAgentLine = transedGwAgentLine;
    SetClientAgentState.gwAgentLineIp = gwAgentLineIP;
    SetClientAgentState.gwAgentLinePort = gwAgentLinePort;
    SetClientAgentState.gwUserLineIp = gwUserLineIP;
    SetClientAgentState.gwUserLinePort = gwUserLinePort;
    CUtility::GetInstance()->SendIPCCMsg_UDP(SetClientAgentState, this->m_ulAgentIP, this->m_usAgentPort);
}

void CAgentInfo::CheckPhoneNo()
{
    if (CUtility::GetInstance()->m_IPCCConfig.bIsEnterpriseVersion)
        return;

    string log = "CheckPhoneNo; phone=" + m_sAgentPhoneNo;

    //	g_Logger.MakeLog(log);

    if (m_sAgentPhoneNo.length() == 11 && m_sAgentPhoneNo.substr(0, 1) == "1")
    {
        m_sAgentPhoneNo = "0" + m_sAgentPhoneNo;
        log = "CheckPhoneNo1; phone=" + m_sAgentPhoneNo;
        //		g_Logger.MakeLog(log);
    }
}

//todo  选择呼叫
bool CAgentInfo::func_AgentWaiting()
{
    bool bFlag = false;

    CDepartmentInfo* pDepartmentInfo =
        CCorporationMgr::GetInstance()->GetDepartmentInfo(this->m_sCorporation, this->m_sDepartment);
    if (pDepartmentInfo == NULL)
    {
        //置座席状态为 Agent_NoLogin
        CAgentMgr::GetInstance()->SetAgentState(m_sAgentID, Agent_NoLogin);

        //返回 AgentLogoutConf消息
        SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
        agentLogoutConf.agentId = this->m_sAgentID;
        CUtility::GetInstance()->SendIPCCMsg_UDP(agentLogoutConf, this->m_ulAgentIP, this->m_usAgentPort);

        return false;
    }

    //禁止接入语音
    if (!this->m_bAllowAudioCall)
        return true;
    
    //1、选择呼叫
    string sCallerName;
    int iCallerType = CallParty_UnKnown;

    CPriorityQueue<AgentSkillInfo>::Iterator it = m_SkillList.Begin();
    for (; it != m_SkillList.End(); it++) {
        CSkillGroupInfo* pSkillGroupInfo =
            CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, it->m_Value.id);
        if (pSkillGroupInfo != NULL) {
            iCallerType = pSkillGroupInfo->PopFromCQueue(sCallerName);
            if (iCallerType != CallParty_UnKnown) {//无人排队
                m_sCurSkillInfo.id = pSkillGroupInfo->m_sSkillGroupid;
                m_sCurSkillInfo.name = pSkillGroupInfo->m_sSkillGroupName;
                m_bCallFromVDN = true;
                m_bCallFromQueue = true;
                return this->CallIn(sCallerName, iCallerType);
            }
        }
    }

    iCallerType = pDepartmentInfo->PopFromCQueue(sCallerName);
    if (iCallerType != CallParty_UnKnown) {//无人排队
        m_bCallFromVDN = true;
        m_bCallFromQueue = true;
        return this->CallIn(sCallerName, iCallerType);
    }

    //2、如果没有呼叫进入排队
    for (it = m_SkillList.Begin(); it != m_SkillList.End(); it++) {
        CSkillGroupInfo* pSkillGroupInfo =
            CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, it->m_Value.id);
        if (pSkillGroupInfo != NULL) {
            pSkillGroupInfo->QueueToWAQueue(m_sAgentID, it->m_Value.qualification);
        }
    }

    pDepartmentInfo->QueueToWAQueue(m_sAgentID);

    return true;
}

bool CAgentInfo::func_AgentBusy()
{
    CDepartmentInfo* pDepartmentInfo
        = CCorporationMgr::GetInstance()->GetDepartmentInfo(this->m_sCorporation, this->m_sDepartment);
    if (pDepartmentInfo != NULL) {
        pDepartmentInfo->RemoveFromWAQueue(this->m_sAgentID);
    }

    CPriorityQueue<AgentSkillInfo>::Iterator it = m_SkillList.Begin();
    for (; it != m_SkillList.End(); it++) {
        CSkillGroupInfo* pSkillGroupInfo =
            CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, it->m_Value.id);
        if (pSkillGroupInfo != NULL) {
            pSkillGroupInfo->RemoveFromWAQueue(m_sAgentID);
        }
    }

    return true;
}

bool CAgentInfo::IsAgentCallDepartOrAgent()
{
    if (this->m_iCalledPartyType == CalledPartyType_Department ||
        this->m_iCalledPartyType == CalledPartyType_WebAgent ||
        this->m_iCalledPartyType == CalledPartyType_WpAgent)
        return true;

    return false;
}

string CAgentInfo::GetAgentStateByStr(void)
{
    string sState("");
    switch (this->m_iCurState)
    {
    case Agent_NoLogin:
        sState = "未登录";
        break;
    case Agent_Waiting:
        sState = "等待";
        break;
    case Agent_Ringing:			//震铃
        sState = "震铃";
        break;
    case Agent_Parking:			//小休
        sState = "小休";
        break;
    case Agent_Talking:			//通话
        sState = "通话";
        break;
    case Agent_OpTalking:		//被动席间通话
        sState = "被动席间通话";
        break;
    case Agent_ForeProcess:		//话前处理
        sState = "话前处理";
        break;
    case Agent_BackProcess:		//话后处理
        sState = "话后处理";
        break;
    case Agent_Holding:			//保持
        sState = "保持";
        break;
    case Agent_Held:				//被保持
        sState = "被保持";
        break;
    case Agent_Dialing:			//拨号
        sState = "拨号";
        break;
    case Agent_DialUnderHold:	//保持下拨号
        sState = "保持下拨号";
        break;
    case Agent_TalkUnderHold:	//保持下通话
        sState = "保持下通话";
        break;
    case Agent_PreDial:			//预拨号
        sState = "预拨号";
        break;
    case Agent_ConnSucc_NoSend:
    case Agent_ConferencePreside:
    case Agent_ConferenceJoin:
    default:
        break;
    }

    return sState;
}

void CAgentInfo::SetOutDialSysUUIDataInfo(string uuiData)
{
    //解析到结构 OutDialSys_Info m_OutDialSys_Info;
    //uuidata format: caller%^#callee%^#uuid%^#gatewayname
    CSpliter s1(uuiData, "%^#");
    if (s1.GetSize() >= 3)
    {
        CSpliter s2((string)s1[2], ";");

        m_OutDialSys_Info.TaskId = atoi(s2[1].c_str());
        m_OutDialSys_Info.PhoneId = atoi(s2[2].c_str());
        m_OutDialSys_Info.DialSeq = atoi(s2[3].c_str());
    }
}

// [ scq/start/2010-02-23 ]
/*
bool CAgentInfo::IsInOutDialDepartment(string sDepartment)
{
CDepartmentInfo* pDepart =
CCorporationMgr::GetInstance()->GetDepartmentInfo(this->m_sCorporation, this->m_sDepartment);

if(pDepart == NULL)
return false;

if(pDepart->m_OutDialSys_Info.bIsOutDialGroup)
return true;

return false;
}
*/
CDepartmentInfo* CAgentInfo::IsInOutDialDepartment(string sDepartment)
{
    CDepartmentInfo* pDepart =
        CCorporationMgr::GetInstance()->GetDepartmentInfo(this->m_sCorporation, sDepartment);	// [ scq/2010-02-23 ]

    if (pDepart == NULL)
        return NULL;

    if (pDepart->m_OutDialSys_Info.bIsOutDialGroup)
        return pDepart;

    return NULL;
}
// [ scq/end/2010-02-23 ]

//////void CAgentInfo::AddSkillToMap(string sSkillGroupName, string sSkillGroupid, int iSkillLevel, int iSkillQualification)
//////{
//////    m_SkillList.PushBack(sSkillGroupid, AgentSkillInfo(sSkillGroupName, sSkillGroupid, iSkillLevel, iSkillQualification));
//////    AddTXTSkillToMap(sSkillGroupid, sSkillGroupName, iSkillLevel, iSkillQualification);
//////}
//////
//////void CAgentInfo::DelSkillFromMap(string sSkillGroupName, string sSkillGroupid, int iSkillLevel)
//////{
//////    m_SkillList.Remove(sSkillGroupid);
//////}
//////
//////void CAgentInfo::ClearSkillMap()
//////{
//////    ClearTXTSkillMap();
//////
//////    CPriorityQueue<AgentSkillInfo>::Iterator it = m_SkillList.Begin();
//////    for (; it != m_SkillList.End(); it++) {
//////        CSkillGroupInfo* pSkillGroupInfo =
//////            CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, it->m_ID);
//////        if (pSkillGroupInfo != NULL) {
//////            pSkillGroupInfo->RemoveFromWAQueue(m_sAgentID);
//////            pSkillGroupInfo->DelAgentFromMap(m_sAgentID);
//////        }
//////    }
//////
//////    m_SkillList.Clear();
//////}
//////
//////void CAgentInfo::AddTXTSkillToMap(string sSkillGroupid, string sSkillGroupName, int iSkillLevel, int iSkillQualification)
//////{
//////    if (!sSkillGroupName.empty() && !sSkillGroupid.empty())
//////    {
//////        m_TXTSkillList.PushBack(sSkillGroupid, AgentSkillInfo(sSkillGroupid, sSkillGroupName, iSkillLevel, iSkillQualification));
//////
//////        stringstream ss;
//////        ss << "add txt skill group"
//////            << ", agentid:" << m_sAgentID
//////            << ", Skillid:" << sSkillGroupid
//////            << ", Skillname:" << sSkillGroupName
//////            << ", SkillLevel:" << iSkillLevel;
//////        g_Logger.MakeLog(ss.str());
//////    }
//////}
//////
//////void CAgentInfo::DelTXTSkillFromMap(int iSkillLevel, string sSkillGroupid)
//////{
//////    m_TXTSkillList.Remove(sSkillGroupid);
//////}
//////
//////void CAgentInfo::DelTXTSkillFromMap(string sSkillGroupid)
//////{
//////    m_TXTSkillList.Remove(sSkillGroupid);
//////}
//////
//////void CAgentInfo::ClearTXTSkillMap()
//////{
//////    CPriorityQueue<AgentSkillInfo>::Iterator it = m_TXTSkillList.Begin();
//////    for (; it != m_TXTSkillList.End(); it++) {
//////        CTXTSkillGroupPtr pTXTSkillGroupInfo = CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(m_sCorporation, it->m_ID);
//////        if (pTXTSkillGroupInfo) {
//////            pTXTSkillGroupInfo->AgentLogout(this);
//////        }
//////    }
//////    m_TXTSkillList.Clear();
//////}
//////
//////void CAgentInfo::LoginToTXTSkill()
//////{
//////    CCorporationInfo* corp = CCorporationMgr::GetInstance()->GetCorporation(this->m_sCorporation);
//////
//////    if (corp != NULL) {
//////        CPriorityQueue<AgentSkillInfo>::Iterator it = m_TXTSkillList.Begin();
//////        for (; it != m_TXTSkillList.End(); it++) {
//////            CTXTSkillGroupPtr sGroup = corp->GetTXTSkillGroupInfo(it->m_ID);
//////            if (sGroup) {
//////                sGroup->AgentLogin(this);
//////
//////                stringstream ss;
//////                ss << "login to txt skill group"
//////                    << ", agentid:" << m_sAgentID
//////                    << ", groupid:" << sGroup->m_sSkillGroupid
//////                    << ", groupname:" << sGroup->m_sSkillGroupName;
//////                g_Logger.MakeLog(ss.str());
//////            }
//////        }
//////    }
//////}

void CAgentInfo::ClearTXTCalls()
{
    //先去除推送失败的call,这些call都没有建立
    //ClearListCallDelivered();
	m_mapCallDelivered.clear();

    map<string, ACDServer::call_ptr>::iterator it = txt_calls.begin();
    while (it != txt_calls.end())
    {
        call_ptr releasing_call_ptr = it->second;
        device_t* remote_dev = releasing_call_ptr->get_device(releasing_call_ptr->call_.calling_device);
        if (remote_dev)
        {
			if (releasing_call_ptr->call_.established)
			{
				g_Logger.MakeLog("释放遗留的文本呼叫");
				SIPCCMsg_ACD_CallReleased callReleased_other;
				callReleased_other.toAgentID = remote_dev->device_id;
				callReleased_other.callID = releasing_call_ptr->call_.call_id;
				callReleased_other.callType = (ECallType)releasing_call_ptr->get_call_type();
				callReleased_other.callPartyType = (__CallPartyType)remote_dev->device_type;
				callReleased_other.agentName = m_sAgentID;

				CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, remote_dev->ip, remote_dev->msg_port);

			}          			
			
        }

		{
			//呼叫记录入库
			string skillGroupId("");
			string skillGroupName("");
			CTXTSkillGroupPtr pSkillGroupInfo =
				CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
			if (pSkillGroupInfo)
			{
				skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
				skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
			}
			string endTime = GetLocalTimeForMessageCall();
			if (IsSuspended())
			{
				//如果座席在后台，取进入后台时间
				time_duration span = time_from_string(latestSuspendTime) - time_from_string(releasing_call_ptr->call_.start_time);
				if (span.total_seconds() > 0)
				{
					endTime = latestSuspendTime;
				}
			}

			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
				(ECallType)releasing_call_ptr->get_call_type(),
				releasing_call_ptr->call_.call_id,
				releasing_call_ptr->call_.calling_device,
				m_sAgentID,
				releasing_call_ptr->call_.start_time,
				endTime,
				m_sManagerid,
				m_sDeptid,
				releasing_call_ptr->call_.caller_type,
				releasing_call_ptr->get_var<string>("keyword"),
				skillGroupId,
				skillGroupName,
				releasing_call_ptr->call_.established
				);
		}

        CCallMgr::GetInstance()->Remove(it->first);
        it++;
    }
    txt_calls.clear();
}


void CAgentInfo::SetTXTSessionTimeout(int32_t timeout)
{
    if (timeout <= 0)
    {
        timeout = 7200;//2小时
    }
    txtSessionTimeout = (timeout>60) ? (timeout + 20) : 80;
}

void CAgentInfo::UpdateTXTSessionTime(string callid, int32_t nomsgTime)
{
    map<string, ACDServer::call_ptr>::iterator it = txt_calls.find(callid);
    if (it != txt_calls.end())
    {
        it->second->update_sesstion_time(nomsgTime);
    }
}

//清理超时无消息的会话
void CAgentInfo::ClearTXTSessionTimeout()
{
    map<string, ACDServer::call_ptr>::iterator it = txt_calls.begin();
    while (it != txt_calls.end())
    {
        ptime now(posix_time::second_clock::local_time());
        time_duration timespan = now - it->second->call_.last_session_time;
        if (timespan.total_seconds() > txtSessionTimeout)
        {
            g_Logger.MakeLog("释放超时无消息的文本呼叫");

            call_ptr releasing_call_ptr = it->second;
            device_t* remote_dev = releasing_call_ptr->get_device(releasing_call_ptr->call_.calling_device);
            if (remote_dev)
            {
                //发送 CallReleased 消息给源				
                SIPCCMsg_ACD_CallReleased callReleased_other;
                callReleased_other.toAgentID = remote_dev->device_id;
                callReleased_other.callID = releasing_call_ptr->call_.call_id;
                callReleased_other.callType = (ECallType)releasing_call_ptr->get_call_type();
                callReleased_other.callPartyType = (__CallPartyType)remote_dev->device_type;
                callReleased_other.agentName = m_sAgentID;

                CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, remote_dev->ip, remote_dev->msg_port); 

            }

			device_t* local_dev = releasing_call_ptr->get_device(m_sAgentID);
			if (local_dev)
			{
				//发送 CallReleased 消息给目的座席
				SIPCCMsg_ACD_CallReleased callReleased_other;
				callReleased_other.SetReleasedParty(m_sAgentID);
				callReleased_other.callID = releasing_call_ptr->call_.call_id;
				callReleased_other.callType = (ECallType)releasing_call_ptr->get_call_type();
				callReleased_other.agentName = m_sAgentID;
				CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, m_ulAgentIP, m_usAgentPort);
			}


			{
				//呼叫记录入库
				string skillGroupId("");
				string skillGroupName("");
				CTXTSkillGroupPtr pSkillGroupInfo =
					CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
				if (pSkillGroupInfo)
				{
					skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
					skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
				}
				string endTime = GetLocalTimeForMessageCall();
				if (IsSuspended())
				{
					//如果座席在后台，取进入后台时间
					time_duration span = time_from_string(latestSuspendTime) - time_from_string(releasing_call_ptr->call_.start_time);
					if (span.total_seconds() > 0)
					{
						endTime = latestSuspendTime;
					}
				}

				CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
					(ECallType)releasing_call_ptr->get_call_type(),
					releasing_call_ptr->call_.call_id,
					releasing_call_ptr->call_.calling_device,
					m_sAgentID,
					releasing_call_ptr->call_.start_time,
					endTime,
					m_sManagerid,
					m_sDeptid,
					releasing_call_ptr->call_.caller_type,
					releasing_call_ptr->get_var<string>("keyword"),
					skillGroupId,
					skillGroupName,
					releasing_call_ptr->call_.established
					);
			}

            CCallMgr::GetInstance()->Remove(it->first);
            txt_calls.erase(it++);
        }
        else
        {
            it++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////
void    CAgentInfo::ClearConferenceParam()
{

    firstName = "";//　　　//主控方座席ID
    firstLineNo = "";//第一外线UserName
    thirdName = "";//第三参会方，如果是座席就填座席ID，如果是中继就填UserName
    conferenceType = 0;//　三方类型，1表示两座席一外线，2表示一座席两外线，3表示一座席一外线，4表示两外线，5表示两座席；
    ConferenceRole = AgentJoin_UnKnown;//0 不在会议中，1是否为主持人，2参与者
    strConferenceID = "";//会议ID
    bApplyforConference = false;
    firstPartyLineNo = "";
    secondPartyLineNo = "";
}

void CAgentInfo::DeliverCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long src_ip, unsigned short src_port)
{
    call_ptr new_call_ptr(new callex);

    new_call_ptr->call_.call_id = makecall.callID;
    new_call_ptr->call_.call_trans_id = "";
    new_call_ptr->call_.call_type = MESSAGE_CALL;
    new_call_ptr->call_.call_direction = INBOUND_CALL;
    new_call_ptr->call_.calling_device = makecall.account;
    new_call_ptr->call_.called_device = makecall.targetName;
    new_call_ptr->call_.alerting_device = m_sAgentID;
	new_call_ptr->call_.caller_type =  E_DT_CClient;
	new_call_ptr->call_.called_type = (device_type_t)makecall.targetType;
    new_call_ptr->call_.start_time = GetLocalTimeForMessageCall();
    new_call_ptr->update_sesstion_time(0);

    connection_t calling_conn;
    calling_conn.party.call_id = new_call_ptr->call_.call_id;
    calling_conn.party.device_id = makecall.account;
    calling_conn.device.device_id = makecall.account;
    calling_conn.device.device_type = E_DT_CClient;
    calling_conn.device.ip = src_ip;
    calling_conn.device.port = 0;
    calling_conn.device.msg_port = src_port;
    calling_conn.local_connection_state = CS_ALERTING;
    new_call_ptr->call_.connections.push_back(calling_conn);

    connection_t alerting_conn;
    alerting_conn.party.call_id = new_call_ptr->call_.call_id;
    alerting_conn.party.device_id = m_sAgentID;
    alerting_conn.device.device_id = m_sAgentID;
    alerting_conn.device.device_type = E_DT_Agent;
    alerting_conn.device.ip = 0;
    alerting_conn.device.port = 0;
    alerting_conn.device.msg_port = 0;
    alerting_conn.local_connection_state = CS_INITIATE;

    new_call_ptr->call_.connections.push_back(alerting_conn);
    txt_calls[makecall.callID] = new_call_ptr;
    m_iTotalTXTCallCount++;
    CCallMgr::GetInstance()->Add(new_call_ptr);

    //发送 Delivered
    boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered());
    callDelivered->SetCallerName(makecall.account);
    callDelivered->callerType = CallParty_CClient;
    callDelivered->SetCallID(makecall.callID);
    callDelivered->SetUUIData(makecall.uuiDate);
    callDelivered->audioStreamDescription = makecall.audioStreamDescription;
    callDelivered->callType = makecall.callType;

	if (this->IsAllowedPush())
    {
        addListCallDelivered(callDelivered);
        CUtility::GetInstance()->Push_Message(m_sAgentID,makecall.callType,makecall.callID);
    }

    CUtility::GetInstance()->SendIPCCMsg_UDP(*callDelivered.get(), m_ulAgentIP, m_usAgentPort);

}

void CAgentInfo::On_SIPCCMsg_Client_AgentMakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long src_ip, unsigned short src_port)
{
    //呼入
    int error = MakeCall(makecall, src_ip, src_port);

    SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
    agentMakeCallConf.callingState = error;
    agentMakeCallConf.SetCalledParty(makecall.GetTargetPosition());
    agentMakeCallConf.callingAgentID = makecall.GetAgentID();
    agentMakeCallConf.callID = makecall.GetCallID();
    agentMakeCallConf.callType = makecall.callType;

    CUtility::GetInstance()->SendIPCCMsg_UDP(agentMakeCallConf, src_ip, src_port);
}

void CAgentInfo::On_SIPCCMsg_CProxy_CMakeCall(SIPCCMsg_CProxy_CMakeCall & makecall, unsigned long src_ip, unsigned short src_port)
{
    //呼入
    int error = MakeCall(makecall, src_ip, src_port);

    SIPCCMsg_ACD_AgentMakeCallConf agentMakeCallConf;
    agentMakeCallConf.callingState = error;
    agentMakeCallConf.SetCalledParty(makecall.targetName);
    agentMakeCallConf.callingAgentID = makecall.account;
    agentMakeCallConf.callID = makecall.callID;
    agentMakeCallConf.callType = makecall.callType;

    CUtility::GetInstance()->SendIPCCMsg_UDP(agentMakeCallConf, src_ip, src_port);
}

int CAgentInfo::CheckForMakeCall(ECallType callType, bool ignoreCallCountLimt)
{
	switch (callType)
	{
	case E_MESSAGE_CALL:case E_WEBCHAT_CALL:
	{
		if (!CanAcceptTXTCall(ignoreCallCountLimt))
			return AgentCalling_UserBusy;
	}
	break;
	case E_EMAIL_CALL:
	{
		if (!CanAcceptEmailCall(ignoreCallCountLimt))
			return AgentCalling_UserBusy;
	}
	break;
	default:
		return AgentCalling_InternalError;
	}

    //后台模式，且上一次推送没有响应
    if (IsSuspended() && PushFlag)
    {
        return AgentCalling_UserBusy;
    }

    return AgentCalling_NoError;
}

int CAgentInfo::MakeCall(SIPCCMsg_Client_AgentMakeCall & makecall, unsigned long src_ip, unsigned short src_port)
{
    return MakeCall(makecall.GetAgentID(), (__CallPartyType)makecall.callerType,
        makecall.GetTargetPosition(), (__CallPartyType)makecall.targetType, makecall.keyword,
        makecall.GetCallID(), makecall.callType,
        makecall.GetUUIData(), makecall.audioStreamDescription,
        makecall.orginalAgentID, makecall.orginalCallID,
        src_ip, src_port);
}


int CAgentInfo::MakeCall(SIPCCMsg_CProxy_CMakeCall & makecall,
    unsigned long src_ip, unsigned short src_port)
{
    return MakeCall(makecall.account, CallParty_CClient,
        makecall.targetName, (__CallPartyType)makecall.targetType, makecall.keyword,
        makecall.callID, makecall.callType,
        makecall.uuiDate, makecall.audioStreamDescription,
        "", "",
        src_ip, src_port);
}

int CAgentInfo::MakeCall(string caller, __CallPartyType callertype,
    string called, __CallPartyType calledtype, string keyword,
    string callid, ECallType callType,
    string uuidata, CAudioStreamDescription & asd,
    string originalagentid, string originalcallid,
    unsigned long src_ip, unsigned short src_port, bool ignoreCallCountLimt)
{
    switch (callType) {
    case E_MESSAGE_CALL: case E_WEBCHAT_CALL:
        return MakeTXTCall(caller, callertype, called, calledtype, keyword,
            callid, callType, uuidata, originalagentid, originalcallid,
            src_ip, src_port, ignoreCallCountLimt);
    case E_EMAIL_CALL:
        return MakeEmailCall(caller, callertype, called, calledtype, keyword,
            callid, uuidata, originalagentid, originalcallid, src_ip, src_port);
    default:
        return AgentCalling_InternalError;
    }
}

int CAgentInfo::MakeTXTCall(string caller, __CallPartyType callertype,
    string called, __CallPartyType calledtype, string keyword,
    string callid, ECallType callType, string uuidata, 
    string originalagentid, string originalcallid,
    unsigned long src_ip, unsigned short src_port, bool ignoreCallCountLimt)
{
    int error = AgentCalling_InternalError;
    //呼入
    if (caller != this->m_sAgentID)
    {
        error = CheckForMakeCall(callType, ignoreCallCountLimt);
        if (error == AgentCalling_NoError)
        {
            call_ptr new_call_ptr(new callex);

            new_call_ptr->call_.call_id = callid;
            new_call_ptr->call_.call_trans_id = "";
            new_call_ptr->call_.call_type = to_call_type_t(callType);
            new_call_ptr->call_.call_direction = INBOUND_CALL;
            new_call_ptr->call_.calling_device = caller;
            new_call_ptr->call_.called_device = called;
            new_call_ptr->call_.alerting_device = m_sAgentID;
			new_call_ptr->call_.caller_type = (device_type_t)callertype;
			new_call_ptr->call_.called_type = (device_type_t)calledtype;
            new_call_ptr->set_var("keyword", keyword);
            new_call_ptr->call_.start_time = GetLocalTimeForMessageCall();
            new_call_ptr->update_sesstion_time(0);

            connection_t calling_conn;
            calling_conn.party.call_id = new_call_ptr->call_.call_id;
            calling_conn.party.device_id = caller;
            calling_conn.device.device_id = caller;
            calling_conn.device.device_type = (device_type_t)callertype;
            calling_conn.device.ip = src_ip;
            calling_conn.device.port = 0;
            calling_conn.device.msg_port = src_port;
            calling_conn.local_connection_state = CS_ALERTING;
            new_call_ptr->call_.connections.push_back(calling_conn);

            connection_t alerting_conn;
            alerting_conn.party.call_id = new_call_ptr->call_.call_id;
            alerting_conn.party.device_id = m_sAgentID;
            alerting_conn.device.device_id = m_sAgentID;
            alerting_conn.device.device_type = E_DT_Agent;
            alerting_conn.device.ip = m_ulAgentIP;
            alerting_conn.device.port = 0;
            alerting_conn.device.msg_port = m_usAgentPort;
            alerting_conn.local_connection_state = CS_ALERTING;

            new_call_ptr->call_.connections.push_back(alerting_conn);

            if (txt_calls.find(callid) != txt_calls.end())
            {
                ostringstream oss;
                oss << "error callid:" << callid << " is exist";
                g_Logger.MakeLog(oss.str());
                error = AgentCalling_InternalError;
            }
            else
            {
                if (callType == E_WEBCHAT_CALL)
                {
                    //检测发起方呼叫限制
                    bool is_transfer = originalcallid.empty() ? false : true;
                    if (!is_transfer && CCallMgr::GetInstance()->GetCount(caller) > 1) {
                        ostringstream oss;
                        oss << "error caller:" << caller << " is exist";
                        g_Logger.MakeLog(oss.str());

                        error = AgentCalling_Repeated;
                        return error;
                    }
                }

                txt_calls[callid] = new_call_ptr;
                m_iTotalTXTCallCount++;
                CCallMgr::GetInstance()->Add(new_call_ptr);

                //发送 Delivered
                boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered());
                callDelivered->SetCallerName(caller);
                callDelivered->callerType = callertype;
                callDelivered->SetCallID(callid);
                callDelivered->SetUUIData(uuidata);
                callDelivered->callType = callType;
                callDelivered->SetKeyWord(keyword);
                callDelivered->originalAgentID = originalagentid;
                callDelivered->originalCallID = originalcallid;

				if (this->IsAllowedPush())
                {
                    addListCallDelivered(callDelivered);
                    CUtility::GetInstance()->Push_Message(m_sAgentID,callType,callid);
                }

                CUtility::GetInstance()->SendIPCCMsg_UDP(*callDelivered.get(), m_ulAgentIP, m_usAgentPort);
                error = AgentCalling_AgentRinging;
            }
        }
    }
    return error;
}

int CAgentInfo::MakeEmailCall(string caller, __CallPartyType callertype,
    string called, __CallPartyType calledtype, string keyword,
    string callid, string uuidata, string originalagentid, string originalcallid,
    unsigned long src_ip, unsigned short src_port)
{
    int error = AgentCalling_NoError;
    //呼入
    if (caller != this->m_sAgentID)
    {
        error = CheckForMakeCall(E_EMAIL_CALL, true);
        if (error == AgentCalling_NoError)
        {
			call_ptr new_call_ptr(new callex);

			new_call_ptr->call_.call_id = callid;
			new_call_ptr->call_.call_trans_id = "";
			new_call_ptr->call_.call_type =  EMAIL_CALL;
			new_call_ptr->call_.call_direction = INBOUND_CALL;
			new_call_ptr->call_.calling_device = caller;
			new_call_ptr->call_.called_device = called;
			new_call_ptr->call_.alerting_device = m_sAgentID;
			new_call_ptr->set_var("keyword", keyword);
			new_call_ptr->call_.caller_type = (device_type_t)callertype;
			new_call_ptr->call_.called_type = (device_type_t)calledtype;
			new_call_ptr->call_.start_time = GetLocalTimeForMessageCall();
			new_call_ptr->update_sesstion_time(0);

			connection_t calling_conn;
			calling_conn.party.call_id = new_call_ptr->call_.call_id;
			calling_conn.party.device_id = caller;
			calling_conn.device.device_id = caller;
			calling_conn.device.device_type = (device_type_t)callertype;
			calling_conn.device.ip = src_ip;
			calling_conn.device.port = 0;
			calling_conn.device.msg_port = src_port;
			calling_conn.local_connection_state = CS_ALERTING;
			new_call_ptr->call_.connections.push_back(calling_conn);

			connection_t alerting_conn;
			alerting_conn.party.call_id = new_call_ptr->call_.call_id;
			alerting_conn.party.device_id = m_sAgentID;
			alerting_conn.device.device_id = m_sAgentID;
			alerting_conn.device.device_type = E_DT_Agent;
			alerting_conn.device.ip = m_ulAgentIP;
			alerting_conn.device.port = 0;
			alerting_conn.device.msg_port = m_usAgentPort;
			alerting_conn.local_connection_state = CS_ALERTING;

			new_call_ptr->call_.connections.push_back(alerting_conn);

			if (email_calls.find(callid) != email_calls.end())
			{
				ostringstream oss;
				oss << "error email callid:" << callid << " is exist";
				g_Logger.MakeLog(oss.str());
				return  AgentCalling_InternalError;
			}

			email_calls[callid] = new_call_ptr;
			CCallMgr::GetInstance()->Add(new_call_ptr);
            m_iEmailCallCount++;

            //发送 Delivered
            boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered());
            callDelivered->SetCallerName(caller);
            callDelivered->callerType = callertype;
            callDelivered->SetCallID(callid);
            callDelivered->SetUUIData(uuidata);
            callDelivered->callType = E_EMAIL_CALL;
            callDelivered->SetKeyWord(keyword);
            callDelivered->originalAgentID = originalagentid;
            callDelivered->originalCallID = originalcallid;

			if (this->IsAllowedPush())
            {
                addListCallDelivered(callDelivered);
				CUtility::GetInstance()->Push_Message(m_sAgentID, E_EMAIL_CALL,callid);
            }

            CUtility::GetInstance()->SendIPCCMsg_UDP(*callDelivered.get(), m_ulAgentIP, m_usAgentPort);

            error = AgentCalling_AgentRinging;
        }
    }
    return error;
}

void CAgentInfo::OnCClientHangupTXTCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long src_ip, unsigned short src_port)
{
    SIPCCMsg_ACD_CHangupCallConf changupCallConf;
    changupCallConf.callID = hangupcall.callID;
    changupCallConf.agentName = m_sAgentID;
    changupCallConf.account = hangupcall.account;
    CUtility::GetInstance()->SendIPCCMsg_UDP(changupCallConf, src_ip, src_port);

    //删除尚未推送的来电消息
    this->delCallDelivered(hangupcall.callID);

    map<string, ACDServer::call_ptr>::iterator it = txt_calls.find(hangupcall.callID);
    if (it != txt_calls.end())
    {
        call_ptr releasing_call_ptr = it->second;
        device_t* remote_dev = releasing_call_ptr->get_device(m_sAgentID);
        if (remote_dev)
        {
            //发送 CallReleased 消息给目的座席
            SIPCCMsg_ACD_CallReleased callReleased_other;
            callReleased_other.SetReleasedParty(hangupcall.account);
            callReleased_other.callID = hangupcall.callID;
            callReleased_other.callType = hangupcall.callType;
            callReleased_other.agentName = m_sAgentID;
            CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, m_ulAgentIP, m_usAgentPort);
        }

		{
			//呼叫记录入库
			string skillGroupId("");
			string skillGroupName("");
			CTXTSkillGroupPtr pSkillGroupInfo =
				CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
			if (pSkillGroupInfo)
			{
				skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
				skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
			}
			string endTime = GetLocalTimeForMessageCall();
			if (IsSuspended())
			{
				//如果座席在后台，取进入后台时间
				time_duration span = time_from_string(latestSuspendTime) - time_from_string(releasing_call_ptr->call_.start_time);
				if (span.total_seconds() > 0)
				{
					endTime = latestSuspendTime;
				}
			}

			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
				hangupcall.callType,
				hangupcall.callID,
				hangupcall.account,
				m_sAgentID,
				releasing_call_ptr->call_.start_time,
				endTime,
				m_sManagerid,
				m_sDeptid,
				CallParty_CClient,
				releasing_call_ptr->get_var<string>("keyword"),
				skillGroupId,
				skillGroupName,
				releasing_call_ptr->call_.established
				);
		}

        CCallMgr::GetInstance()->Remove(it->first);
        txt_calls.erase(it);

    }

    SelectTXTCall();
}

void CAgentInfo::OnHangupTXTCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long src_ip, unsigned short src_port)
{
    //删除尚未推送的来电消息
    this->delCallDelivered(hangupcall.GetCallID());

    map<string, ACDServer::call_ptr>::iterator it = txt_calls.find(hangupcall.GetCallID());
    if (it != txt_calls.end())
    {
        call_ptr releasing_call_ptr = it->second;
        device_t* local_dev = releasing_call_ptr->get_device(m_sAgentID);
        if (local_dev)
        {
            //发送 CallReleased 消息给目的座席
            SIPCCMsg_ACD_CallReleased callReleased_other;
            callReleased_other.SetReleasedParty(m_sAgentID);
            callReleased_other.callID = hangupcall.GetCallID();
            callReleased_other.callType = hangupcall.callType;
            callReleased_other.agentName = m_sAgentID;
            CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, m_ulAgentIP, m_usAgentPort);
        }

        device_t* remote_dev = releasing_call_ptr->get_device(hangupcall.GetTheOtherParty());
        if (remote_dev)
        {

            //发送 CallReleased 消息给C端
            if (hangupcall.callType == E_MESSAGE_CALL)
            {
                SIPCCMsg_ACD_CallReleased callReleased;
                callReleased.SetReleasedParty(hangupcall.GetTheOtherParty());
                callReleased.callID = hangupcall.GetCallID();
                callReleased.callType = hangupcall.callType;
                callReleased.toAgentID = hangupcall.GetTheOtherParty();
                CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased, remote_dev->ip, remote_dev->msg_port);
            }

        }

		{
			//呼叫记录入库
			string skillGroupId("");
			string skillGroupName("");
			CTXTSkillGroupPtr pSkillGroupInfo =
				CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
			if (pSkillGroupInfo)
			{
				skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
				skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
			}
			string endTime = GetLocalTimeForMessageCall();
			if (IsSuspended())
			{
				//如果座席在后台，取进入后台时间
				time_duration span = time_from_string(latestSuspendTime) - time_from_string(releasing_call_ptr->call_.start_time);
				if (span.total_seconds() > 0)
				{
					endTime = latestSuspendTime;
				}
			}

			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
				hangupcall.callType,
				hangupcall.GetCallID(),
				releasing_call_ptr->call_.calling_device,
				m_sAgentID,
				releasing_call_ptr->call_.start_time,
				endTime,
				m_sManagerid,
				m_sDeptid,
				releasing_call_ptr->call_.caller_type,
				releasing_call_ptr->get_var<string>("keyword"),
				skillGroupId,
				skillGroupName,
				releasing_call_ptr->call_.established
				);
		}

		CCallMgr::GetInstance()->Remove(it->first);
        txt_calls.erase(it);     
    }

    SelectTXTCall();
}

void CAgentInfo::OnAnswerTXTCall(SIPCCMsg_Client_AgentAnswerCall & answercall, unsigned long src_ip, unsigned short src_port)
{
    //发送 CallEstablished 消息给本座席
    SIPCCMsg_ACD_CallEstablished callEstablished_toMain;
    callEstablished_toMain.SetTheOtherParty(answercall.GetTheOTherParty());
    callEstablished_toMain.nickName = answercall.GetTheOTherParty();
    callEstablished_toMain.callType = answercall.callType;
	callEstablished_toMain.managerId = this->m_sManagerid;
    CUtility::GetInstance()->SendIPCCMsg_UDP(callEstablished_toMain, src_ip, src_port);

    map<string, ACDServer::call_ptr>::iterator it = txt_calls.find(answercall.callId);
    if (it != txt_calls.end())
    {
        call_ptr answering_call_ptr = it->second;
		answering_call_ptr->call_.established = 1;
        device_t* remote_dev = answering_call_ptr->get_device(answercall.GetTheOTherParty());
        if (remote_dev)
        {
            SIPCCMsg_ACD_CallEstablished callEstablished_toOther;
            callEstablished_toOther.SetTheOtherParty(answercall.GetAgentID());
            callEstablished_toOther.nickName = this->m_sNickName;
            callEstablished_toOther.callingAgentID = answercall.GetTheOTherParty();
            callEstablished_toOther.callID = answercall.callId;
            callEstablished_toOther.callType = answercall.callType;
			callEstablished_toOther.managerId = this->m_sManagerid;
            CUtility::GetInstance()->SendIPCCMsg_UDP(callEstablished_toOther, remote_dev->ip, remote_dev->msg_port);
        }
    }

    //删除calldeliveredmap,如果有
    delCallDelivered(answercall.callId);
}

void CAgentInfo::OnHangupEmailCall(SIPCCMsg_CProxy_HangupCall & hangupcall, unsigned long src_ip, unsigned short src_port)
{
	SIPCCMsg_ACD_CHangupCallConf changupCallConf;
	changupCallConf.callID = hangupcall.callID;
	changupCallConf.agentName = m_sAgentID;
	changupCallConf.account = hangupcall.account;
	CUtility::GetInstance()->SendIPCCMsg_UDP(changupCallConf, src_ip, src_port);

	//删除尚未推送的来电消息
	this->delCallDelivered(hangupcall.callID);

	map<string, ACDServer::call_ptr>::iterator it = email_calls.find(hangupcall.callID);
	if (it != email_calls.end())
	{
		call_ptr releasing_call_ptr = it->second;
		device_t* remote_dev = releasing_call_ptr->get_device(m_sAgentID);
		if (remote_dev)
		{
			//发送 CallReleased 消息给目的座席
			SIPCCMsg_ACD_CallReleased callReleased_other;
			callReleased_other.SetReleasedParty(hangupcall.account);
			callReleased_other.callID = hangupcall.callID;
			callReleased_other.callType = hangupcall.callType;
			callReleased_other.agentName = m_sAgentID;
			CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, m_ulAgentIP, m_usAgentPort);
		}

		{
			//呼叫记录入库
			string skillGroupId("");
			string skillGroupName("");
			CTXTSkillGroupPtr pSkillGroupInfo =
				CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
			if (pSkillGroupInfo)
			{
				skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
				skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
			}

			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
				hangupcall.callType,
				hangupcall.callID,
				releasing_call_ptr->call_.calling_device,
				m_sAgentID,
				releasing_call_ptr->call_.start_time,
				GetLocalTimeForMessageCall(),
				m_sManagerid,
				m_sDeptid,
				releasing_call_ptr->call_.caller_type,
				releasing_call_ptr->get_var<string>("keyword"),
				skillGroupId,
				skillGroupName,
				releasing_call_ptr->call_.established
				);
		}

		CCallMgr::GetInstance()->Remove(it->first);
		email_calls.erase(it);

	}
}

void CAgentInfo::OnHangupEmailCall(SIPCCMsg_Client_AgentHangupCall & hangupcall, unsigned long src_ip, unsigned short src_port)
{
	this->delCallDelivered(hangupcall.GetCallID());

	map<string, ACDServer::call_ptr>::iterator it = email_calls.find(hangupcall.GetCallID());
	if (it != email_calls.end())
	{
		call_ptr releasing_call_ptr = it->second;
		device_t* local_dev = releasing_call_ptr->get_device(m_sAgentID);
		if (local_dev)
		{
			//发送 CallReleased 消息给目的座席
			SIPCCMsg_ACD_CallReleased callReleased_other;
			callReleased_other.SetReleasedParty(m_sAgentID);
			callReleased_other.callID = hangupcall.GetCallID();
			callReleased_other.callType = hangupcall.callType;
			callReleased_other.agentName = m_sAgentID;
			CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased_other, m_ulAgentIP, m_usAgentPort);
		}

		device_t* remote_dev = releasing_call_ptr->get_device(hangupcall.GetTheOtherParty());
		if (remote_dev)
		{
			//发送 CallReleased 消息给C端
			SIPCCMsg_ACD_CallReleased callReleased;
			callReleased.SetReleasedParty(hangupcall.GetTheOtherParty());
			callReleased.callID = hangupcall.GetCallID();
			callReleased.callType = hangupcall.callType;
			callReleased.toAgentID = hangupcall.GetTheOtherParty();
			CUtility::GetInstance()->SendIPCCMsg_UDP(callReleased, remote_dev->ip, remote_dev->msg_port);

		}

		{
			//呼叫记录入库
			string skillGroupId("");
			string skillGroupName("");
			CTXTSkillGroupPtr pSkillGroupInfo =
				CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
			if (pSkillGroupInfo)
			{
				skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
				skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
			}

			CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
				hangupcall.callType,
				hangupcall.GetCallID(),
				releasing_call_ptr->call_.calling_device,
				m_sAgentID,
				releasing_call_ptr->call_.start_time,
				GetLocalTimeForMessageCall(),
				m_sManagerid,
				m_sDeptid,
				releasing_call_ptr->call_.caller_type,
				releasing_call_ptr->get_var<string>("keyword"),
				skillGroupId,
				skillGroupName,
				releasing_call_ptr->call_.established
				);
		}

		CCallMgr::GetInstance()->Remove(it->first);
		email_calls.erase(it);
		
	}
}

void CAgentInfo::OnAnswerEmailCall(SIPCCMsg_Client_AgentAnswerCall & answercall, unsigned long src_ip, unsigned short src_port)
{
	//发送 CallEstablished 消息给本座席
	SIPCCMsg_ACD_CallEstablished callEstablished_toMain;
	callEstablished_toMain.SetTheOtherParty(answercall.GetTheOTherParty());
	callEstablished_toMain.nickName = answercall.GetTheOTherParty();
	callEstablished_toMain.callType = answercall.callType;
	callEstablished_toMain.managerId = this->m_sManagerid;
	CUtility::GetInstance()->SendIPCCMsg_UDP(callEstablished_toMain, src_ip, src_port);

	map<string, ACDServer::call_ptr>::iterator it = email_calls.find(answercall.callId);
	if (it != email_calls.end())
	{
		call_ptr answering_call_ptr = it->second;
		answering_call_ptr->call_.established = 1;
		device_t* remote_dev = answering_call_ptr->get_device(answercall.GetTheOTherParty());
		if (remote_dev)
		{
			SIPCCMsg_ACD_CallEstablished callEstablished_toOther;
			callEstablished_toOther.SetTheOtherParty(answercall.GetAgentID());
			callEstablished_toOther.nickName = this->m_sNickName;
			callEstablished_toOther.callingAgentID = answercall.GetTheOTherParty();
			callEstablished_toOther.callID = answercall.callId;
			callEstablished_toOther.callType = answercall.callType;
			callEstablished_toOther.managerId = this->m_sManagerid;
			CUtility::GetInstance()->SendIPCCMsg_UDP(callEstablished_toOther, remote_dev->ip, remote_dev->msg_port);
		}
	}

	//删除calldeliveredmap,如果有
	delCallDelivered(answercall.callId);
}

void CAgentInfo::OnAgentSwitchToFrontDesk(SIPCCMsg_Client_AgentSwitchToFrontDesk &switchtofrontdesk, unsigned long src_ip, unsigned short src_port)
{
    //返回未挂机的txtcall
    SIPCCMsg_ACD_AgentSwitchToFrontDeskConf conf;
    conf.invokeID = switchtofrontdesk.invokeID;
    conf.agentID = m_sAgentID;

    //去掉正在推送的呼叫
    map<string, ACDServer::call_ptr> tmp_txt_calls;
    for (map<string, ACDServer::call_ptr>::iterator iter = txt_calls.begin(); iter != txt_calls.end(); iter++)
    {
        if (m_mapCallDelivered.find(iter->first) == m_mapCallDelivered.end())
        {
            tmp_txt_calls[iter->first] = iter->second;
        }

    }

    if (tmp_txt_calls.size()>0 && tmp_txt_calls.size() % 20 == 0)
    {
        conf.count = tmp_txt_calls.size() / 20;
    }
    else
    {
        conf.count = 1 + tmp_txt_calls.size() / 20;
    }

    int i = 1;
    map<string, ACDServer::call_ptr>::iterator it = tmp_txt_calls.begin();
    while (it != tmp_txt_calls.end())
    {
        conf.callIdList += it->first;
        conf.callIdList += ";";
        if ((i++) % 20 == 0)
        {
            CUtility::GetInstance()->SendIPCCMsg_UDP(conf, src_ip, src_port);
            conf.callIdList = "";
            conf.seq++;
        }
        it++;
    }
    if (tmp_txt_calls.size() == 0 || tmp_txt_calls.size() % 20 != 0)
    {
        CUtility::GetInstance()->SendIPCCMsg_UDP(conf, src_ip, src_port);
    }

    if (!IsPC())
    {
        //发送来电消息
        PushFlag = 0;
        SendCallDelivered();
    }

}

void CAgentInfo::NotifyMsg(SIPCCMsg_OneKeySwitch_NotifyMsg &notifyMsg)
{
    SIPCCMsg_OneKeySwitch_NotifyMsg newmsg = notifyMsg;
    SIPCCMsg_OneKeySwitch_NotifyMsg::Target t = SIPCCMsg_OneKeySwitch_NotifyMsg::Target();
    t.type = "AGENT";
    t.targetNames.clear();
    newmsg.targets.push_back(t);
    CUtility::GetInstance()->SendIPCCMsg_UDP(newmsg, m_ulAgentIP, m_usAgentPort);
}

void CAgentInfo::OnDeptmartmentRemoved()
{
    //置座席状态为 Agent_NoLogin
    CAgentMgr::GetInstance()->SetAgentState(m_sAgentID, Agent_NoLogin);

    //返回 AgentLogoutConf消息
    SIPCCMsg_ACD_AgentLogoutConf agentLogoutConf;
    agentLogoutConf.agentId = this->m_sAgentID;
    CUtility::GetInstance()->SendIPCCMsg_UDP(agentLogoutConf, this->m_ulAgentIP, this->m_usAgentPort);

    //从 agentMap 中删除座席
    CAgentMgr::GetInstance()->DelAgent(m_sAgentID);
}

void CAgentInfo::SelectTXTCall()
{
    if (!CanAcceptTXTCall()) {
        return;
    }

    CCorporationInfo* corp = CCorporationMgr::GetInstance()->GetCorporation(this->m_sCorporation);
    if (corp == NULL)
        return;

    CDepartmentInfo* pDepartmentInfo =
        CCorporationMgr::GetInstance()->GetDepartmentInfo(m_sCorporation, m_sDepartment);
    if (pDepartmentInfo != NULL)
    {
        pDepartmentInfo->OnAgentFreeForTXTCall(this);
    }

    CPriorityQueue<AgentSkillInfo>::Iterator it = m_TXTSkillList.Begin();
    while (CanAcceptTXTCall() && (it != m_TXTSkillList.End())) {
        CTXTSkillGroupPtr sGroupPtr = corp->GetTXTSkillGroupInfo(it->m_ID);
        if (sGroupPtr) {
            sGroupPtr->AgentWait(this);
        }
        ++it;
    }
}

void CAgentInfo::addListCallDelivered(boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered)
{
    m_mapCallDelivered[callDelivered->GetCallID()] = callDelivered;
}

void CAgentInfo::delCallDelivered(string callid)
{
    m_mapCallDelivered.erase(callid);
}

void CAgentInfo::SendCallDelivered()
{
    map<string, ACDServer::call_ptr>::iterator iter;
    map<string, boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> >::iterator pIter = m_mapCallDelivered.begin();
    while (pIter != m_mapCallDelivered.end())
    {
        iter = txt_calls.find(pIter->first);
		if (iter != txt_calls.end() || pIter->second->callType == E_NORMAL_CALL || pIter->second->callType == E_EMAIL_CALL)
        {
            CUtility::GetInstance()->SendIPCCMsg_UDP(*pIter->second, m_ulAgentIP, m_usAgentPort);
        }
        pIter++;
    }
    m_mapCallDelivered.clear();
}

void CAgentInfo::ClearListCallDelivered()
{
    //推送失败时，清除txtcall,
    map<string, ACDServer::call_ptr>::iterator iter;
    map<string, boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> >::iterator pIter = m_mapCallDelivered.begin();
    while (pIter != m_mapCallDelivered.end())
    {
        iter = txt_calls.find(pIter->first);
        if (iter != txt_calls.end())
        {
			call_ptr releasing_call_ptr = iter->second;

			{
				string skillGroupId("");
				string skillGroupName("");
				CTXTSkillGroupPtr pSkillGroupInfo =
					CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(releasing_call_ptr->call_.called_device);
				if (pSkillGroupInfo)
				{
					 skillGroupId = pSkillGroupInfo->m_sSkillGroupid;
					 skillGroupName = pSkillGroupInfo->m_sSkillGroupName;
				}

				CUtility::GetInstance()->send_SIPCCMsg_Client_MessageCallInfo(
					(ECallType)releasing_call_ptr->get_call_type(),
					releasing_call_ptr->call_.call_id,
					releasing_call_ptr->call_.calling_device,
					m_sAgentID,
					releasing_call_ptr->call_.start_time,
					GetLocalTimeForMessageCall(),
					m_sManagerid,
					m_sDeptid,
					releasing_call_ptr->call_.caller_type,
					releasing_call_ptr->get_var<string>("keyword"),
					skillGroupId,
					skillGroupName,
					releasing_call_ptr->call_.established
					);
				
			}

            txt_calls.erase(iter);
            CCallMgr::GetInstance()->Remove(pIter->first);

            stringstream ss;
            ss << "remove txt_call for push failed"
                << ", agentid:" << m_sAgentID
                << ", callid:" << pIter->first;
            g_Logger.MakeLog(ss.str());

        }
        pIter++;
    }

    m_mapCallDelivered.clear();
}

void CAgentInfo::SetSuspend()
{
	if (bAgentDormancy)
	{
		return;
	}

	bAgentDormancy = true;
	latestSuspendTime = GetLocalTimeForMessageCall();

	//log
	string sLog;
	ostringstream ostream;
	ostream.str("");
	ostream << " 移动座席：" << m_sCorporation<<"|"<<m_sDepartment<<"|"<<m_sAgentID<<" 进入后台模式,"
		<< "\n" << "state=" << m_iCurState
		<< ";" << "ostype=" << m_iLoginPlatform
		<< ";" << "natip=" << format_ip_addr(m_ulAgentIP)
		<< ";" << "natport=" << ntohs(m_usAgentPort);

	sLog = ostream.str();
	g_Logger.MakeLog(sLog);

}

bool CAgentInfo::IsAllowedPush()
{
	return CUtility::GetInstance()->IsAllowedPush()
		&& !IsPC()
		&& PushFlag < 3;
}

bool CAgentInfo::CallIn(CUserInfo* pCallerUser)
{
    CUserInfo* pUserInfo = pCallerUser;
    if (pUserInfo == NULL) {
        g_Logger.MakeLog("user is null");
        return false;
    }

    //1、设置本地状态
    //2、给座席发消息
    //3、清理内存

    //1、设置本地状态
    string sCallID = pUserInfo->m_CallID_sCallIn;
    this->m_sCallID = sCallID;
    this->bCallin = true;
    this->bInnerCall = false;
    this->bPSTNCall = true;
    this->m_sQueueArriveTime = CLog::GetLocalTime();

    this->UpdateUserInfo(pUserInfo);
    this->m_userInfo.m_sResponseAgentID = this->m_sAgentID;
    CUserMgr::GetInstance()->SetResponseAgentID(pUserInfo->m_sUserName, this->m_sAgentID);
    this->m_CurGwUserLineNo = pUserInfo->m_sUserName;
    this->m_CurUserLineIP = pUserInfo->m_audioIP;
    this->m_CurUserLinePort = pUserInfo->m_audioPort;

    CAgentMgr::GetInstance()->m_mapUserCallAgent[pUserInfo->m_sUserName] = this->m_sAgentID;

    this->SetState(Agent_Ringing);

    //2、给座席发消息
    if (this->m_iLoginType != LoginType_OnlyPSTN)
    {
        //发送 CallDelivered 消息给座席
        //增加 comSymbol 设置
        boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered);
        callDelivered->SetCallerName(pUserInfo->m_sUserName);
        callDelivered->SetCallID(sCallID);
        callDelivered->SetKeyWord(pUserInfo->m_sCalledKeyWord);
        callDelivered->SetUUIData(pUserInfo->m_sUUIData);
        callDelivered->callerType = pUserInfo->m_iUserType;
        callDelivered->audioIP = pUserInfo->m_audioIP;
        callDelivered->audioPort = pUserInfo->m_audioPort;
        callDelivered->audioRtpPort = pUserInfo->m_audioRtpPort;
        callDelivered->lanIP = pUserInfo->m_lanIP;
        callDelivered->gwPartNo = pUserInfo->m_sPartNo;
        callDelivered->userLineNo = pUserInfo->m_sUserName;
        callDelivered->userLineIP = pUserInfo->m_audioIP;
        callDelivered->userLinePort = pUserInfo->m_audioPort;
        callDelivered->comSymbol = pUserInfo->m_sComSymbol;
        callDelivered->bIsFromDept = true;
        callDelivered->audioStreamDescription = pUserInfo->m_asd;

        if (this->m_iLoginType == LoginType_WebPSTN)
        {
            callDelivered->audioIP = this->m_CurGwLineIP;
            callDelivered->audioPort = this->m_CurGwLinePort;
            callDelivered->wPstnAgentLineNo = this->m_WebPstnAgentCurGwLine;
        }

        this->SetOutDialSysUUIDataInfo(pUserInfo->m_sUUIData);


        if (this->IsAllowedPush())
        {
            this->addListCallDelivered(callDelivered);
            CUtility::GetInstance()->Push_Message(this->m_sAgentID, E_NORMAL_CALL, sCallID);
        }

        CUtility::GetInstance()->SendIPCCMsg_UDP(*callDelivered.get(), this->m_ulAgentIP, this->m_usAgentPort);
    }

    if (this->m_iLoginType != LoginType_OnlyWeb) {
        SIPCCMsg_ACD_RingAgentExt RingAgentExt;
        RingAgentExt.SetAgentID(this->m_sAgentID);
        RingAgentExt.curVersion = "9.9.9";
        RingAgentExt.SetTargetPosition(pUserInfo->m_sComSymbol);
        RingAgentExt.SetCorporationName(this->m_sCorporation);
        RingAgentExt.SetUUIData(this->m_sAgentPhoneNo);
        RingAgentExt.ipPhoneNo_User = this->m_userInfo.m_sUserName;
        RingAgentExt.managerId = this->m_sManagerid;
        CUtility::GetInstance()->SendIPCCMsg_UDP(RingAgentExt, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
    }

    //3、清理内存
    CUserMgr::GetInstance()->DelUser(pUserInfo->m_sUserName);

    return true;
}

bool CAgentInfo::CallIn(CAgentInfo* callerAgent)
{
    CAgentInfo* pCallingAgentInfo = callerAgent;

    if (pCallingAgentInfo == NULL)
    {
        //todo
        return false;
    }

    //1、设置本地状态
    //2、给座席发消息

    //1、设置本地状态
    //1.1、设置主叫座席状态
    pCallingAgentInfo->m_sTheOtherAgentID = this->m_sAgentID;
    pCallingAgentInfo->m_iAgentQueueSpanTime = CalTimeSpan(pCallingAgentInfo->m_iAgentEnqueueTime, gettickcount());
    pCallingAgentInfo->m_sCalledParty = this->m_sAgentID;

    //1.2、设置本叫座席状态
    this->m_sCallID = pCallingAgentInfo->m_sCallID;
    this->bCallin = true;
    this->bInnerCall = true;
    this->bPSTNCall = false;
    this->m_sQueueArriveTime = CLog::GetLocalTime();

    this->m_sTheOtherAgentID = pCallingAgentInfo->m_sAgentID;

    this->SetState(Agent_Ringing);

    //2、给座席发消息
    //发送 CallDelivered 消息给座席
    boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered());
    callDelivered->SetCallerName(pCallingAgentInfo->m_sAgentID);
    callDelivered->SetCallID(pCallingAgentInfo->m_sCallID);
    callDelivered->SetKeyWord("");
    callDelivered->SetUUIData(pCallingAgentInfo->m_sUUIData);
    if (pCallingAgentInfo->m_iLoginType == LoginType_WebPSTN)
    {
        callDelivered->callerType = CallParty_WebPstnAgent;
        callDelivered->audioIP = pCallingAgentInfo->m_CurGwLineIP;
        callDelivered->audioPort = pCallingAgentInfo->m_CurGwLinePort;
        callDelivered->wPstnAgentLineNo = pCallingAgentInfo->m_WebPstnAgentCurGwLine;
        callDelivered->userLineNo = pCallingAgentInfo->m_CurGwUserLineNo;
    }
    else
    {
        callDelivered->callerType = CallParty_Agent;
        callDelivered->userLineNo = pCallingAgentInfo->m_CurGwUserLineNo;
        callDelivered->userLineIP = pCallingAgentInfo->m_CurGwLineIP;
        callDelivered->userLinePort = pCallingAgentInfo->m_CurGwLinePort;
    }
    callDelivered->AgentUUIData(pCallingAgentInfo->agentUUIData); // [2/23/2009 lixg]
    callDelivered->userLineNo = pCallingAgentInfo->m_CurGwUserLineNo;
    callDelivered->userLineIP = pCallingAgentInfo->m_CurUserLineIP;
    callDelivered->userLinePort = pCallingAgentInfo->m_CurUserLinePort;
    callDelivered->lanIP = pCallingAgentInfo->m_ulLanIP;
    callDelivered->lanMainSockPort = pCallingAgentInfo->m_usLanMainSockPort;
    callDelivered->gwPartNo = pCallingAgentInfo->m_userInfo.m_sPartNo;
    callDelivered->bIsFromDept = true;
    callDelivered->sipAgentPhoneIP1 = pCallingAgentInfo->sipAgentPhoneIP1;
    callDelivered->sipAgentPhoneRtpPort1 = pCallingAgentInfo->sipAgentPhoneRtpPort1;
    callDelivered->sipAgentPhoneRtcpPort1 = pCallingAgentInfo->sipAgentPhoneRtcpPort1;
    callDelivered->audioStreamDescription = pCallingAgentInfo->audioStreamDescription;
    callDelivered->originalAgentID = pCallingAgentInfo->originalAgentID;
    callDelivered->originalCallID = pCallingAgentInfo->originalCallID;
    callDelivered->originalData = pCallingAgentInfo->originalData;
    callDelivered->comSymbol = pCallingAgentInfo->comSymbol;
    callDelivered->callType = (ECallType)pCallingAgentInfo->m_iCallType;

    if (this->IsAllowedPush())
    {
        this->addListCallDelivered(callDelivered);
        CUtility::GetInstance()->Push_Message(this->m_sAgentID, E_NORMAL_CALL, callDelivered->GetCallID());
    }

    CUtility::GetInstance()->SendIPCCMsg_UDP(*callDelivered.get(), this->m_ulAgentIP, this->m_usAgentPort);

    return true;
}

bool CAgentInfo::CallIn(CCClientInfo* callerCAgent)
{

    CCClientInfo* pCallingCClientInfo = callerCAgent;

    if (pCallingCClientInfo == NULL) {
        //将座席放回到 List
        return false;
    }

    //1、设置本地状态
    //2、给座席发消息

    //1.1、设置主叫座席状态
    pCallingCClientInfo->m_iQueueSpanTime = CalTimeSpan(pCallingCClientInfo->m_iEnqueueTime, gettickcount());
    CCClientMgr::GetInstance()->SetResponseAgentID(pCallingCClientInfo->m_sAccount, m_sAgentID, __FILE__, __LINE__);

    //1.2、设置本叫座席状态

	string sAccount_caller = "CClient_" + pCallingCClientInfo->m_sAccount;

    this->m_sCallID = pCallingCClientInfo->m_CallID_sCallIn;
    this->bCallin = true;
    this->bInnerCall = false;
    this->bPSTNCall = false;
    this->m_sQueueArriveTime = CLog::GetLocalTime();

	this->m_sTheOtherAgentID = sAccount_caller;

    this->SetState(Agent_Ringing);

    //2、给座席发消息
    boost::shared_ptr<SIPCCMsg_ACD_CallDelivered> callDelivered(new SIPCCMsg_ACD_CallDelivered());
	callDelivered->SetCallerName(sAccount_caller);
    callDelivered->SetCallID(pCallingCClientInfo->m_CallID_sCallIn);
    callDelivered->SetKeyWord("");
    callDelivered->SetUUIData(pCallingCClientInfo->m_sUUIData);
    callDelivered->callerType = CallParty_CClient;
    callDelivered->bIsFromDept = true;
    callDelivered->audioStreamDescription = pCallingCClientInfo->audioStreamDescription;
    callDelivered->callType = pCallingCClientInfo->callType;
    callDelivered->mediaRequest = pCallingCClientInfo->mediaRequest;

    if (this->IsAllowedPush()) {
        this->addListCallDelivered(callDelivered);
        //若座席进入后台，推送来电信息
        CUtility::GetInstance()->Push_Message(this->m_sAgentID, E_NORMAL_CALL, callDelivered->GetCallID());
    }

    CUtility::GetInstance()->SendIPCCMsg_UDP(*callDelivered.get(), this->m_ulAgentIP, this->m_usAgentPort);

    return true;
}

bool CAgentInfo::CallIn(string callerid, int callertype, void * caller)
{
    ostringstream os;
    os << "CAgentInfo::CallIn " << m_sAgentID
        << " " << callerid << " " << callertype;
    g_Logger.MakeLog(os.str());

    bool ret = false;

    if (callertype == CallParty_User) {
        if (caller == NULL) {
            return CallIn(CUserMgr::GetInstance()->GetUserInfo(callerid));
        }
        else {
            return CallIn((CUserInfo*)caller);
        }
    }
    else if (callertype == CallParty_Agent) {
        if (caller == NULL) {
            return CallIn(CAgentMgr::GetInstance()->GetAgentInfo(callerid));
        }
        else {
            return CallIn((CAgentInfo*)caller);
        }
    }
    else if (callertype == CallParty_CClient) {
        if (caller == NULL) {
            return CallIn(CCClientMgr::GetInstance()->GetCClientInfo(callerid));
        }
        else {
            return CallIn((CCClientInfo*)caller);
        }
    }
    return false;
}

void CAgentInfo::LoginAllSkillGroup()
{
    CSpliter spSkillGroupId(m_sSkillList, "|", ",");
    string strSGIdTemp = "";
    string strSkillLevel = "";
    string strSkillQualification = "";
    int iSkillLevel = 0;
    int iSkillQualification = 0;
    for (int i = 0; i<spSkillGroupId.GetSize(); i++)
    {
        strSGIdTemp = spSkillGroupId[i][0];
        strSkillLevel = spSkillGroupId[i][1];
        iSkillLevel = atoi(strSkillLevel.c_str());
        strSkillQualification = spSkillGroupId[i][2];
        iSkillQualification = atoi(strSkillQualification.c_str());

        //[如果其中一个为0，赋值为另一个，如果都为0，则默认都为1
        if (iSkillLevel == 0) {
            iSkillLevel = iSkillQualification != 0 ? iSkillQualification : 1;
        }

        if (iSkillQualification == 0) {
            iSkillQualification = iSkillLevel;
        }
        //]

        if (!strSGIdTemp.empty()) {
            CSkillGroupInfo* pSkillGroupInfo =
                CCorporationMgr::GetInstance()->GetSkillGroupInfo(m_sCorporation, strSGIdTemp);
            if (NULL != pSkillGroupInfo) {
                LoginSkillGroup(pSkillGroupInfo->m_sSkillGroupid, pSkillGroupInfo->m_sSkillGroupName, iSkillLevel, iSkillQualification);
                LoginTXTSkillGroup(pSkillGroupInfo->m_sSkillGroupid, pSkillGroupInfo->m_sSkillGroupName, iSkillLevel, iSkillQualification);
                ostringstream oss;
                oss << "corp=" << m_sCorporation
                    << "sgid=" << strSGIdTemp;
                g_Logger.MakeLog(oss.str());
            }
        }
    }

}

void CAgentInfo::LoginSkillGroup(string id, string name, int level, int qualification)
{
    CSkillGroupInfo* pSkillGroupInfo =
        CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, id);
    if (pSkillGroupInfo != NULL) {
        m_SkillList.PushBack(id, AgentSkillInfo(id, name, level, qualification), level);

        stringstream ss;
        ss << "add skill group"
            << ", agentid:" << m_sAgentID
            << ", Skillid:" << id
            << ", Skillname:" << name
            << ", SkillLevel:" << level
            << ", SkillQualification:" << qualification;
        g_Logger.MakeLog(ss.str());

        pSkillGroupInfo->AgentLogin(m_sAgentID);
        pSkillGroupInfo->QueueToWAQueue(m_sAgentID, qualification);
    }
}

void CAgentInfo::LogoutSkillGroup(string id)
{
    CSkillGroupInfo* pSkillGroupInfo =
        CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, id);
    if (pSkillGroupInfo != NULL) {
        pSkillGroupInfo->AgentLogout(m_sAgentID);
        m_SkillList.Remove(id);
    }
}

void CAgentInfo::LogoutAllSkillGroup()
{
    CPriorityQueue<AgentSkillInfo>::Iterator it = m_SkillList.Begin();
    for (; it != m_SkillList.End(); it++) {
        CSkillGroupInfo* pSkillGroupInfo =
            CCorporationMgr::GetInstance()->GetSkillGroupInfo(this->m_sCorporation, it->m_ID);
        if (pSkillGroupInfo != NULL) {
            pSkillGroupInfo->AgentLogout(m_sAgentID);
        }
    }
    m_SkillList.Clear();
}

void CAgentInfo::LoginTXTSkillGroup(string id, string name, int level, int qualification)
{
    CTXTSkillGroupPtr pSkillGroupInfo =
        CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(this->m_sCorporation, id);
    if (pSkillGroupInfo != NULL) {
        m_TXTSkillList.PushBack(id, AgentSkillInfo(id, name, level, qualification), level);

        stringstream ss;
        ss << "add txt skill group"
            << ", agentid:" << m_sAgentID
            << ", Skillid:" << id
            << ", Skillname:" << name
            << ", SkillLevel:" << level
            << ", SkillQualification:" << qualification;
        g_Logger.MakeLog(ss.str());

        pSkillGroupInfo->AgentLogin(this);
        pSkillGroupInfo->AgentWait(this);
    }
}

void CAgentInfo::LogoutTXTSkillGroup(string id)
{
    CTXTSkillGroupPtr pSkillGroupInfo =
        CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(this->m_sCorporation, id);
    if (pSkillGroupInfo != NULL) {
        pSkillGroupInfo->AgentLogout(this);
        m_TXTSkillList.Remove(id);
    }
}

void CAgentInfo::LogoutAllTXTSkillGroup()
{
    CPriorityQueue<AgentSkillInfo>::Iterator it = m_TXTSkillList.Begin();
    for (; it != m_TXTSkillList.End(); it++) {
        CTXTSkillGroupPtr pSkillGroupInfo =
            CCorporationMgr::GetInstance()->GetTXTSkillGroupInfo(this->m_sCorporation, it->m_ID);
        if (pSkillGroupInfo != NULL) {
            pSkillGroupInfo->AgentLogout(this);
        }
    }
    m_TXTSkillList.Clear();
}
