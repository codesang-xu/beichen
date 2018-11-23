// AgentMgr.cpp: implementation of the CAgentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "public/StdAfx.h"
#include "AgentMgr.h"
#include "AgentInfo.h"
#include "UserMgr.h"
#include "public/IPCCMessage.h"
#include "DepartmentInfo.h"
#include "Utility.h"
#include "public/GlobalOperation.h"
#include "CorporationMgr.h"
#include "public/ServerInfoDef.h"
#include "public/Spliter.h"
#include "public/MemLog.h"
extern CMemLog   g_Logger;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAgentMgr* CAgentMgr::__AgentMgr = 0;

CAgentMgr::CAgentMgr()
{
	m_bCheckBak = false;
}

CAgentMgr::~CAgentMgr()
{
	m_mapCheckOnlineAgents.clear();
	m_mapCheckOnlineAgents_bak.clear();
	m_mapPSTNAgents.clear();

	map<string, CAgentInfo*>::iterator pIter;
	for(pIter=m_mapAgents.begin();pIter!=m_mapAgents.end();++pIter)
	{
		if(pIter->second != NULL)
			delete pIter->second;
	}
	m_mapAgents.clear();
}

CAgentMgr* CAgentMgr::GetInstance()
{
	if(0 == __AgentMgr)
		__AgentMgr = new CAgentMgr();

	return __AgentMgr;
}

void CAgentMgr::destroy()
{
	if(0 != __AgentMgr)
		delete __AgentMgr;

	__AgentMgr = 0;
}

CAgentInfo* CAgentMgr::AddAgent(string agentID, string nickName, string corpName, string departmentName, int natType,
	unsigned long ip, unsigned short port, unsigned long lanip, unsigned short lanMainSockPort,
	int loginType, string agentPhoneNo, int NeedDisposal, string managerid, string deptid,
	string extNo, string deptNickName, bool allowAudioCall, bool allowTXTCall, int maxTXTCallCount, int32_t loginPlatform,
	string skillGroupId, bool needHeartBeat)
{
	if(agentID.length() <= 0 || departmentName.length() <= 0)
		return NULL;

#ifdef MAKEMSGLOG
	AddAgentInfoToMap(agentID, corpName, ip, port);
#endif

    DelAgent(agentID);

	CAgentInfo* pAgentInfo = NULL;

	pAgentInfo = new CAgentInfo();
	pAgentInfo->m_sAgentID = agentID;
	pAgentInfo->m_sNickName = nickName;
	pAgentInfo->m_sCorporation = corpName;
	pAgentInfo->m_sDepartment = departmentName;
	pAgentInfo->m_sManagerid = managerid;
	pAgentInfo->m_sDeptid = deptid;
	pAgentInfo->m_sDeptNickName = deptNickName;
	pAgentInfo->m_iAgentNatType = natType;
	pAgentInfo->m_iLoginType = loginType;
	pAgentInfo->m_sAgentPhoneNo = agentPhoneNo;
	pAgentInfo->m_sExtNo = extNo;
	if ( NeedDisposal == 1 )
	{
		pAgentInfo->CheckPhoneNo();
	}
	pAgentInfo->m_ulAgentIP = ip;
	pAgentInfo->m_usAgentPort = port;
	pAgentInfo->m_ulLanIP = lanip;
	pAgentInfo->m_usLanMainSockPort = lanMainSockPort;
	pAgentInfo->m_iPreHeartBeatMark = gettickcount();
    pAgentInfo->m_bCallFromVDN = false;
    pAgentInfo->m_bCallFromQueue = false;
    pAgentInfo->bExitOutdial = false;
	pAgentInfo->m_bNeedHeartBeat = needHeartBeat;
	pAgentInfo->m_bAllowAudioCall = allowAudioCall;
	pAgentInfo->m_bAllowTXTCall = allowTXTCall;
	pAgentInfo->m_iMaxTXTDialogCount = maxTXTCallCount;
	pAgentInfo->m_iLoginPlatform = loginPlatform;
	pAgentInfo->m_sSkillList = skillGroupId;

	m_mapAgents[agentID] = pAgentInfo;

	if ( pAgentInfo->m_bNeedHeartBeat && pAgentInfo->m_iLoginType != LoginType_OnlyPSTN )
	{
		if ( !m_bCheckBak )
		{
			m_mapCheckOnlineAgents_bak[agentID] = pAgentInfo;	

			pAgentInfo->m_bNeedCheck = false;
		}
		else
		{
			m_mapCheckOnlineAgents[agentID] = pAgentInfo;	

			pAgentInfo->m_bNeedCheck = true;
		}
	}
	else if ( pAgentInfo->m_iLoginType == LoginType_OnlyPSTN )
	{
		m_mapPSTNAgents[agentID] = pAgentInfo;
	}

	return pAgentInfo;
}

void CAgentMgr::DelAgent(string agentID)
{
	if(agentID.length() <= 0)
		return;
	
#ifdef MAKEMSGLOG
	DelAgentInfoFromMap(agentID);
#endif

	CAgentInfo* pAgentInfo = GetAgentInfo(agentID);

	if(pAgentInfo == NULL)
	{
		this->m_mapAgents.erase(agentID);
		return;
	}

	string sCorp;
	string sDept;
	if(pAgentInfo)
	{
		sCorp = pAgentInfo->m_sCorporation;
		sDept = pAgentInfo->m_sDepartment;	

        pAgentInfo->LogoutAllSkillGroup();
        pAgentInfo->LogoutAllTXTSkillGroup();
        pAgentInfo->ClearTXTCalls();
		if ( pAgentInfo->m_bNeedHeartBeat )
		{
			this->m_mapCheckOnlineAgents.erase(agentID);
			this->m_mapCheckOnlineAgents_bak.erase(agentID);
		}

		if ( pAgentInfo->m_iLoginType == LoginType_OnlyPSTN )
		{
			this->m_mapPSTNAgents.erase(agentID);
		}
	}

	map<string, CAgentInfo*>::iterator pIter = this->m_mapAgents.find(agentID);
	if(pIter == this->m_mapAgents.end())
		return;

	delete pIter->second;
	this->m_mapAgents.erase(agentID);

	CUtility::GetInstance()->OutDial_SetAgentCount(sCorp, sDept);

}

void CAgentMgr::SetAgentState(string agentID, int agentState, bool bIsAgentLogin)
{
	//设置座席状态，需要处理座席状态变化是 list 的变化情况，以及呼叫分配
	CAgentInfo* pAgentInfo = this->GetAgentInfo(agentID);
	if(NULL == pAgentInfo)
		return;

	pAgentInfo->SetState(agentState, bIsAgentLogin);
}

CAgentInfo* CAgentMgr::GetAgentInfo(string agentID)
{
	if(agentID.length() <= 0)
	{
		return NULL;
	}

	map<string, CAgentInfo*>::iterator pIter;
	if((pIter=this->m_mapAgents.find(agentID)) == m_mapAgents.end())
	{
		return NULL;
	}

	CAgentInfo* pAgent = (CAgentInfo*)pIter->second;
	if ( pAgent == NULL )
	{
		return NULL;
	}

	if (pAgent->bAgentDormancy && pAgent->IsPC())
	{
		return NULL;
	}
	
	return pAgent;
}

//todo ?什么逻辑
void CAgentMgr::CleanUpAgentMap()
{
	map<string, CAgentInfo*>::iterator pIter;
	//超过5次
	unsigned long iCurTimeMark = gettickcount();
	unsigned long iAgentHeartInterval=0;
	unsigned long iOnlyPSTNAgentRingInterval=0;
	CAgentInfo* pAgentInfo = NULL;

	list<string> tempList;
	tempList.clear();

	if ( !m_bCheckBak )
	{
		for ( pIter=m_mapCheckOnlineAgents.begin();pIter!=m_mapCheckOnlineAgents.end(); )
		{
			pAgentInfo = NULL;
			pAgentInfo = pIter->second;
			if(pAgentInfo == NULL)
			{
				tempList.push_back(pIter->first);

				++pIter;

				continue;
			}

			iAgentHeartInterval = iCurTimeMark - pAgentInfo->m_iPreHeartBeatMark;

			//if(iAgentHeartInterval >= __HEARTBEAT_INTERVAL*18)
			//if(iAgentHeartInterval >= __HEARTBEAT_INTERVAL*5)
			if(iAgentHeartInterval >= __HEARTBEAT_INTERVAL*36)
			{
				///begin conference command  add code by hyj /////////////////////////////////////////////////////////////////////
				OnferenceinAgentTimeout(pAgentInfo);		    
				///end conference command///////////////////////////////////////////////////////////////////////

				//座席死掉
				CDepartmentInfo* pDepartmentInfo = NULL;
				pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
				if(pDepartmentInfo)
				{				

					if ( pAgentInfo->m_iCurState == Agent_Talking && pAgentInfo->bPSTNCall )
					{
						SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
						AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);

						if ( pAgentInfo->bCallin )
						{
							AgentHangupCall.SetTheOtherParty(pAgentInfo->m_userInfo.m_sUserName);
						}
						else
						{
							AgentHangupCall.SetTheOtherParty(pAgentInfo->m_sCalledParty);
						}

						CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);

					}

					if (CUtility::GetInstance()->IsAllowedPush() 
						&& !pAgentInfo->IsPC()
						&& (pAgentInfo->PushFlag)<2)
					{
						pAgentInfo->SetSuspend();					
						++pIter;
						continue;

					}

					//清队列
					pDepartmentInfo->RemoveFromWAQueue(pAgentInfo->m_sAgentID);

					SetAgentState(pAgentInfo->m_sAgentID, Agent_NoLogin);
					//log
					string sLog;
					ostringstream ostream;
					ostream.str("");
					ostream << "<网络故障>:与公司："<< pAgentInfo->m_sCorporation 
						<< " 技能组：" << pAgentInfo->m_sDepartment
						<< " 座席：" << pAgentInfo->m_sAgentID << " 的网络连接中断，请网管查看线路！"
						<< "\n" << "state=" << GetStateStr(pAgentInfo->m_iCurState)
						<< ";" << "natip=" << format_ip_addr(pAgentInfo->m_ulAgentIP)
						<< ";" << "natport=" << ntohs(pAgentInfo->m_usAgentPort);

					sLog = ostream.str();
					g_Logger.MakeLog( sLog);

					//pAgentInfo->SetDormancy(true);

				}

				tempList.push_back(pAgentInfo->m_sAgentID);

				++pIter;

				continue;

			}

			m_mapCheckOnlineAgents.erase(pIter++);

			m_mapCheckOnlineAgents_bak[pAgentInfo->m_sAgentID] = pAgentInfo;

			pAgentInfo->m_bNeedCheck = false;
		}

		m_bCheckBak = true;
	}
	else
	{
		for ( pIter=m_mapCheckOnlineAgents_bak.begin();pIter!=m_mapCheckOnlineAgents_bak.end(); )
		{
			pAgentInfo = NULL;
			pAgentInfo = pIter->second;
			if(pAgentInfo == NULL)
			{
				tempList.push_back(pIter->first);

				++pIter;

				continue;
			}

			iAgentHeartInterval = iCurTimeMark - pAgentInfo->m_iPreHeartBeatMark;

			//if(iAgentHeartInterval >= __HEARTBEAT_INTERVAL*18)
			//if(iAgentHeartInterval >= __HEARTBEAT_INTERVAL*5)
			if(iAgentHeartInterval >= __HEARTBEAT_INTERVAL*36)
			{
				///begin conference command  add code by hyj /////////////////////////////////////////////////////////////////////
				OnferenceinAgentTimeout(pAgentInfo);		    
				///end conference command///////////////////////////////////////////////////////////////////////

				//座席死掉
				CDepartmentInfo* pDepartmentInfo = NULL;
				pDepartmentInfo = CCorporationMgr::GetInstance()->GetDepartmentInfo(pAgentInfo->m_sCorporation, pAgentInfo->m_sDepartment);
				if(pDepartmentInfo)
				{
					
					if ( pAgentInfo->m_iCurState == Agent_Talking && pAgentInfo->bPSTNCall )
					{
						SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
						AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);

						if ( pAgentInfo->bCallin )
						{
							AgentHangupCall.SetTheOtherParty(pAgentInfo->m_userInfo.m_sUserName);
						}
						else
						{
							AgentHangupCall.SetTheOtherParty(pAgentInfo->m_sCalledParty);
						}

						CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
					}

					if (CUtility::GetInstance()->IsAllowedPush()
						&& !pAgentInfo->IsPC()
						&& (pAgentInfo->PushFlag) < 2)
					{
						pAgentInfo->SetSuspend();
						++pIter;
						continue;
					}

					//清队列
					pDepartmentInfo->RemoveFromWAQueue(pAgentInfo->m_sAgentID);

					SetAgentState(pAgentInfo->m_sAgentID, Agent_NoLogin);
					//log
					string sLog;
					ostringstream ostream;
					ostream.str("");
					ostream << "<网络故障>:与公司："<< pAgentInfo->m_sCorporation 
						<< " 技能组：" << pAgentInfo->m_sDepartment
						<< " 座席：" << pAgentInfo->m_sAgentID << " 的网络连接中断，请网管查看线路！"
						<< "\n" << "state=" << GetStateStr(pAgentInfo->m_iCurState)
						<< ";" << "natip=" << format_ip_addr(pAgentInfo->m_ulAgentIP)
						<< ";" << "natport=" << ntohs(pAgentInfo->m_usAgentPort);

					sLog = ostream.str();
					g_Logger.MakeLog( sLog);

					//pAgentInfo->SetDormancy(true);

				}

				tempList.push_back(pAgentInfo->m_sAgentID);

				++pIter;

				continue;

			}

			m_mapCheckOnlineAgents_bak.erase(pIter++);

			m_mapCheckOnlineAgents[pAgentInfo->m_sAgentID] = pAgentInfo;

			pAgentInfo->m_bNeedCheck = true;
		}

		m_bCheckBak = false;
	}

	for ( pIter=m_mapPSTNAgents.begin();pIter!=m_mapPSTNAgents.end(); )
	{
		pAgentInfo = NULL;
		pAgentInfo = pIter->second;
		if(pAgentInfo == NULL)
		{
			tempList.push_back(pIter->first);

			++pIter;

			continue;
		}

		++pIter;

		//if(pAgentInfo->m_iLoginType == LoginType_OnlyPSTN)
		{
			if ( pAgentInfo->m_iCurState != Agent_Ringing )
			{
				continue;
			}

			iOnlyPSTNAgentRingInterval = iCurTimeMark - pAgentInfo->m_iRingStateMark_OnlyPSTN;
			if(iOnlyPSTNAgentRingInterval >= 120000)	//2分钟振铃超时
			{
				//手机坐席挂断
				pAgentInfo->SetState(Agent_Waiting);
				pAgentInfo->m_iRingStateMark_OnlyPSTN = iCurTimeMark;
			}

			continue;
		}
	}

	while(!tempList.empty())
	{
		CAgentMgr::GetInstance()->DelAgent(tempList.front());
		tempList.pop_front();
	}
}

string CAgentMgr::GetStateStr(int state)
{
	switch(state)
	{
	case Agent_NoLogin:
		return "Agent_NoLogin";
	case Agent_Waiting:
		return "Agent_Waiting";
	case Agent_Ringing:
		return "Agent_Ringing";
	case Agent_Parking:
		return "Agent_Parking";
	case Agent_Talking:
		return "Agent_Talking";
	default:
		break;
	}

	return "";
}

void CAgentMgr::SetAgentDormancy(string agentid, bool dormancy)
{
	CAgentInfo* pAgentInfo = this->GetAgentInfo(agentid);
	if(NULL == pAgentInfo)
		return;

	pAgentInfo->SetDormancy(dormancy);
}

CAgentInfo* CAgentMgr::GetAnyAgent(string agentID)
{
	if(agentID.length() <= 0)
		return NULL;	

	map<string, CAgentInfo*>::iterator pIter;
	if((pIter=this->m_mapAgents.find(agentID)) == m_mapAgents.end())
		return NULL;

	CAgentInfo* pAgent = (CAgentInfo*)pIter->second;

	return pAgent;
}

bool CAgentMgr::IsAgentExist(string agentID)
{
	if ( agentID.empty() )
	{
		return false;
	}

	/*if ( CUtility::GetInstance()->m_bHaveRedis )
	{
		CAgentInfo* pAgentInfo = CreateAgentInfo_Redis(agentID);

		if ( pAgentInfo )
		{
			return true;
		}
	}*/

	if(this->m_mapAgents.find(agentID) == m_mapAgents.end())
	{
		return false;
	}

	return true;
}

void CAgentMgr::SendTalkingAgents(string invokeID, string corpName, string deptName, string skillName, unsigned long ip, unsigned short port)
{
	if(corpName == "")
		return;

	SIPCCMsg_ACD_GetTalkingAgentConf GetTalkingAgentConf;
	GetTalkingAgentConf.invokeID = invokeID;

	ostringstream os;
	os.str("");

	string sAgentList = "";

	map<string, CAgentInfo*>::iterator pIter;
	CAgentInfo* pAgentInfo = NULL;

	os.str("");

	for(pIter=m_mapAgents.begin();pIter!=m_mapAgents.end();++pIter)
	{
		pAgentInfo = NULL;
		pAgentInfo = pIter->second;
		if(pAgentInfo == NULL)
			continue;
		
		if(pAgentInfo->m_sCorporation!=corpName)
			continue;
		
		if(pAgentInfo->m_iCurState != Agent_Talking)
			continue;
		
		if(deptName == "")
		{
			if ( skillName == "" )
			{
				os  << pAgentInfo->m_sDepartment << "/" << pAgentInfo->m_sDeptNickName << ","
					<< pAgentInfo->m_sAgentID << "(" << pAgentInfo->m_sNickName << ";" 
					<< pAgentInfo->m_iLoginType << ")"
					//<< "," << pAgentInfo->m_ulAgentIP
					//<< "," << pAgentInfo->m_usAgentPort
					<< "," << pAgentInfo->m_ulLanIP
					<< "," << pAgentInfo->m_usLanMainSockPort
					<< "|";
			}
			else if(pAgentInfo->m_sCurSkillInfo.name == skillName)
			{
				os << pAgentInfo->m_sAgentID << "(" << pAgentInfo->m_sNickName << ";" 
					<< pAgentInfo->m_iLoginType << ")"
					//<< "," << pAgentInfo->m_ulAgentIP
					//<< "," << pAgentInfo->m_usAgentPort
					<< "," << pAgentInfo->m_ulLanIP
					<< "," << pAgentInfo->m_usLanMainSockPort
					<< "|";
			}
			else
			{
				continue;
			}
			
		}
		else if(pAgentInfo->m_sDepartment == deptName)
		{
			os << pAgentInfo->m_sAgentID << "(" << pAgentInfo->m_sNickName << ";" 
				<< pAgentInfo->m_iLoginType << ")"
				//<< "," << pAgentInfo->m_ulAgentIP
				//<< "," << pAgentInfo->m_usAgentPort
				<< "," << pAgentInfo->m_ulLanIP
				<< "," << pAgentInfo->m_usLanMainSockPort
				<< "|";
		}
		else
			continue;
		
		sAgentList += os.str();
		
		if(sAgentList.length() > 600)
		{
			GetTalkingAgentConf.theList = sAgentList;
			CUtility::GetInstance()->SendIPCCMsg_UDP(GetTalkingAgentConf, ip, port);
		g_Logger.MakeLog("Monitor:" + sAgentList);
			sAgentList = "";
		}
		
		os.str("");
	}

	GetTalkingAgentConf.theList = sAgentList;
	GetTalkingAgentConf.bEnd = 1;
	CUtility::GetInstance()->SendIPCCMsg_UDP(GetTalkingAgentConf, ip, port);
g_Logger.MakeLog("Monitor:" + sAgentList);
}

int CAgentMgr::GetAgentCount()
{
	int iAgentCount = m_mapAgents.size();

	if ( ( iAgentCount%100 == 0 ) && ( iAgentCount != 0 ) )
	{
		string sLog;
		ostringstream ostream;
		ostream.str("");

		ostream << "----- 当前座席登录数已达到 " << iAgentCount << " -----";

		sLog = ostream.str();
	g_Logger.MakeLog( sLog);
	}

	return iAgentCount;
	//return m_mapAgents.size();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  CAgentMgr::OnferenceinAgentTimeout(CAgentInfo* pAgentInfo)
{
	if(pAgentInfo==NULL)
	{
	
	  return ;
	
	}

	if(pAgentInfo->ConferenceRole!=AgentJoin_UnKnown)
	{

	g_Logger.MakeLog( "OnferenceinAgentTimeout");
		switch(pAgentInfo->conferenceType)
		{

		case 1:
			{
				string tstrLog= "timeout:会议挂机  会议类型=1 ，会议ID="+pAgentInfo->strConferenceID+",(angetid="+pAgentInfo->m_sAgentID+")";
				if(pAgentInfo->ConferenceRole==AgentJoin_Active)//主控
				{
					tstrLog.append("坐席为主控坐席：");
					int nConferecneNum=0;
					if(pAgentInfo->firstLineNo !="")
					{
						nConferecneNum+=1;

					}
					if(pAgentInfo->thirdName !="")
					{
						nConferecneNum+=1;

					}
					{

					ostringstream ostream;
					ostream.str("");
					ostream <<"time out在会议中ID"<<pAgentInfo->strConferenceID<<";在线数="<<nConferecneNum;						
				g_Logger.MakeLog( ostream.str());

				     }		
					//g_Logger.MakeLog( "timout: 主控" );
					if(nConferecneNum != 1)
					{
						SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
						AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);
						AgentHangupCall.SetTheOtherParty(pAgentInfo->firstPartyLineNo);
						AgentHangupCall.agentJoinType=AgentJoin_Active;
						unsigned long ip = CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP;
						CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
					g_Logger.MakeLog( "tineout 主控SIPCCMsg_Client_AgentHangupCall  到UStatus" );
						///
					}
					else
					{

						SIPCCMsg_Client_ConferenceEnd  ConferenceEnd;
						ConferenceEnd.firstName=pAgentInfo->firstPartyLineNo;
						ConferenceEnd.firstLineNo=pAgentInfo->firstLineNo;
						ConferenceEnd.thirdName=pAgentInfo->secondPartyLineNo;
						CUtility::GetInstance()->SendIPCCMsg_UDP(ConferenceEnd, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
					g_Logger.MakeLog( " timout只剩一条 主控SIPCCMsg_Client_ConferenceEnd  到UStatus" );


					}
					tstrLog.append("会议类型=1，参与方="+pAgentInfo->thirdName);
					if(pAgentInfo->thirdName !="")//查看参与方
					{
						CAgentInfo*  pTmpAgentInfo=CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->thirdName);
						if(pTmpAgentInfo!=NULL)
						{ 
							//发送参与议
							SIPCCMsg_ACD_CallReleased ACD_CallReleased;
							ACD_CallReleased.SetReleasedParty(pAgentInfo->m_sAgentID);
							ACD_CallReleased.bIsPresideAgent=1;
							CUtility::GetInstance()->SendIPCCMsg_UDP(ACD_CallReleased, pTmpAgentInfo->m_ulAgentIP, pTmpAgentInfo->m_usAgentPort);
							pTmpAgentInfo->ConferenceRole=AgentJoin_Active;//参与方，改为主控方
							pTmpAgentInfo->firstName=pAgentInfo->thirdName;//角色转化
							pTmpAgentInfo->thirdName="";
							pTmpAgentInfo->firstPartyLineNo=pAgentInfo->secondPartyLineNo;
							pTmpAgentInfo->secondPartyLineNo="";

						}
						else
						{

							tstrLog.append("参与方=NULL");


						}
					}
					else
					{

						tstrLog.append("参与方为空值");

					}
				g_Logger.MakeLog(tstrLog);
					pAgentInfo->ClearConferenceParam();
				}
				else
				{
					tstrLog.append( "坐席为 参与方" );
					int nConferecneNum=0;
					if(pAgentInfo->firstLineNo !="")
					{
						nConferecneNum+=1;

					}
					if(pAgentInfo->firstName !="")
					{
						nConferecneNum+=1;

					}
					{

						ostringstream ostream;
						ostream.str("");
						ostream <<"time out在会议中ID"<<pAgentInfo->strConferenceID<<";在线数="<<nConferecneNum;						
					g_Logger.MakeLog( ostream.str());

					}		
					if(nConferecneNum !=1)
					{
						SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
						AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);
						AgentHangupCall.SetTheOtherParty(pAgentInfo->secondPartyLineNo);
						AgentHangupCall.agentJoinType=AgentJoin_Passive;
						unsigned long ip = CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP;
						CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
					g_Logger.MakeLog( "tineout 主控SIPCCMsg_Client_AgentHangupCall  到UStatus" );
						///
					}
					else
					{

						SIPCCMsg_Client_ConferenceEnd  ConferenceEnd;
						ConferenceEnd.firstName=pAgentInfo->firstPartyLineNo;
						ConferenceEnd.firstLineNo=pAgentInfo->firstLineNo;
						ConferenceEnd.thirdName=pAgentInfo->secondPartyLineNo;
						CUtility::GetInstance()->SendIPCCMsg_UDP(ConferenceEnd, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
					g_Logger.MakeLog( " timout只剩一条 主控SIPCCMsg_Client_ConferenceEnd  到UStatus" );


					}
					tstrLog.append("firstName="+pAgentInfo->firstName);
					CAgentInfo*  pTmpAgentInfo=CAgentMgr::GetInstance()->GetAgentInfo(pAgentInfo->firstName);
					if(pTmpAgentInfo!=NULL)
					{ 
						//发送 CallReleased 消息给给主控坐席
						SIPCCMsg_ACD_CallReleased ACD_CallReleased;
						ACD_CallReleased.SetReleasedParty(pAgentInfo->thirdName);
						ACD_CallReleased.bIsPresideAgent=0;
						CUtility::GetInstance()->SendIPCCMsg_UDP(ACD_CallReleased, pTmpAgentInfo->m_ulAgentIP, pTmpAgentInfo->m_usAgentPort);							  
						pTmpAgentInfo->thirdName="";

					}
					else
					{

						tstrLog.append( "主控方为NULL" );

					}
				g_Logger.MakeLog(tstrLog);
					pAgentInfo->ClearConferenceParam();	//清除会议记录
				}//else



			}
			break;
		case 2://2表示一座席两外线
			{
			g_Logger.MakeLog( "timout 2表示一座席两外线" );
				int nConferecneNum=0;
				if(pAgentInfo->firstLineNo !="")
				{
					nConferecneNum+=1;

				}
				if(pAgentInfo->thirdName !="")
				{
					nConferecneNum+=1;

				}
				if(nConferecneNum !=1)
				{
					SIPCCMsg_Client_AgentHangupCall AgentHangupCall;
					AgentHangupCall.SetAgentID(pAgentInfo->m_sAgentID);
					AgentHangupCall.SetTheOtherParty(pAgentInfo->firstPartyLineNo);
					AgentHangupCall.agentJoinType=AgentJoin_Active;
					unsigned long ip = CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP;
					CUtility::GetInstance()->SendIPCCMsg_UDP(AgentHangupCall, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
				g_Logger.MakeLog( "tineout 主控SIPCCMsg_Client_AgentHangupCall  到UStatus" );
					pAgentInfo->ClearConferenceParam();
					string tstrLog= "timeout:会议挂机  会议类型=2，会议ID="+pAgentInfo->strConferenceID+",angetid="+pAgentInfo->m_sAgentID;
				g_Logger.MakeLog( tstrLog );
				}
				else
				{

					SIPCCMsg_Client_ConferenceEnd  ConferenceEnd;
					ConferenceEnd.firstName=pAgentInfo->firstPartyLineNo;
					ConferenceEnd.firstLineNo=pAgentInfo->firstLineNo;
					ConferenceEnd.thirdName=pAgentInfo->secondPartyLineNo;
					CUtility::GetInstance()->SendIPCCMsg_UDP(ConferenceEnd, CUtility::GetInstance()->m_IPCCConfig.UStateServer_IP, CUtility::GetInstance()->m_IPCCConfig.UStateServer_Port);
				g_Logger.MakeLog( "timeout 只剩一条 主控SIPCCMsg_Client_ConferenceEnd  到UStatus" );
				}

			}
			break;
		default:
			break;
		}


	}//if(pAgentInfo->ConferenceRole!=AgentJoin_UnKnown)


}

//////void CAgentMgr::RemoveFromWAQueue(string agentID)
//////{
//////	//设置座席状态，需要处理座席状态变化是 list 的变化情况，以及呼叫分配
//////	CAgentInfo* pAgentInfo = this->GetAgentInfo(agentID);
//////	if(NULL == pAgentInfo)
//////		return;
//////
//////	pAgentInfo->RemoveFromWAQueue();
//////}

void CAgentMgr::ClearTXTSessionTimeout()
{
	CAgentInfo* pAgentInfo = NULL;
	map<string, CAgentInfo*>::iterator pIter = m_mapAgents.begin();
	while (pIter != m_mapAgents.end())
	{
		pAgentInfo = pIter->second;
		if (NULL == pAgentInfo)
		{
			pIter++;
			continue;
		}
		pAgentInfo->ClearTXTSessionTimeout();
		pIter++;
	}

}