#pragma once
#include "hiredis/hiredis.h"
#include "public/Spliter.h"
#include "public/GlobalOperation.h"
#include <stdlib.h>
#include <string>
#include <map>
#include <sstream>
using namespace std;

typedef struct __CorporationInfo_Redis 
{
	string m_sCorporationName;
	string m_sManagerid;

	int m_iDynamicWebAgentLicence;
	int m_iDynamicMobileAgentLicence;
	int m_iCurWebAgentCount;
	int m_iCurMoblieAgentCount;

	__CorporationInfo_Redis()
	{

	}

	__CorporationInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, ";" );

		m_sCorporationName = cs[0];
		m_sManagerid = cs[1];
		m_iDynamicWebAgentLicence = atoi( cs[2].c_str() );
		m_iDynamicMobileAgentLicence = atoi( cs[3].c_str() );
		m_iCurWebAgentCount = atoi( cs[4].c_str() );
		m_iCurMoblieAgentCount = atoi( cs[5].c_str() );
	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_sCorporationName 
		   << ";" << m_sManagerid 
		   << ";" << m_iDynamicWebAgentLicence
		   << ";" << m_iDynamicMobileAgentLicence
		   << ";" << m_iCurWebAgentCount
		   << ";" << m_iCurMoblieAgentCount;

		return os.str();
	}

}CorporationInfo_Redis;

typedef struct __DepartmentInfo_Redis 
{
	string m_sDeptName;
	string m_sDeptId;
	string m_sDeptNickname;
	int m_iDeptType;
	string m_sManagerid;
	int m_iPSTNAgentRingMaxTime;
	int m_iNeedSms_OnHungup;
	string m_sAgentInfoKey;
	string m_sWaitingAgentListKey;
	string m_sUserCallerListKey;

	__DepartmentInfo_Redis()
	{

	}

	__DepartmentInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, ";" );

		m_sDeptName = cs[0];
		m_sDeptId = cs[1];
		m_sDeptNickname = cs[2];
		m_iDeptType = atoi( cs[3].c_str() );
		m_sManagerid = cs[4];
		m_iPSTNAgentRingMaxTime = atoi( cs[5].c_str() );
		m_iNeedSms_OnHungup = atoi( cs[6].c_str() );
		m_sAgentInfoKey = cs[7];
		m_sWaitingAgentListKey = cs[8];
		m_sUserCallerListKey = cs[9];
	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_sDeptName 
			<< ";" << m_sDeptId 
			<< ";" << m_sDeptNickname
			<< ";" << m_iDeptType
			<< ";" << m_sManagerid
			<< ";" << m_iPSTNAgentRingMaxTime
			<< ";" << m_iNeedSms_OnHungup
			<< ";" << m_sAgentInfoKey
			<< ";" << m_sWaitingAgentListKey
			<< ";" << m_sUserCallerListKey;

		return os.str();
	}

}DepartmentInfo_Redis;

typedef struct __SkillGroupInfo_Redis 
{
	string m_sSkillGroupName;
	string m_sSkillGroupId;
	string m_sManagerId;
	int m_iNeedSms_OnHungup;
	string m_sAgentInfoKey;
	string m_sWaitingAgentListKey;
	string m_sUserCallerListKey;	//[5];

	__SkillGroupInfo_Redis()
	{

	}

	__SkillGroupInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, ";" );

		m_sSkillGroupName = cs[0];
		m_sSkillGroupId = cs[1];
		m_sManagerId = cs[2];
		m_iNeedSms_OnHungup = atoi( cs[3].c_str() );
		m_sAgentInfoKey = cs[4];
		m_sWaitingAgentListKey = cs[5];
		/*m_sUserCallerListKey[0] = cs[6];
		for ( int i=1; i<5; i++ )
		{
			ostringstream os;
			os.str("");
			os << m_sUserCallerListKey[0] << "_" << i;
			m_sUserCallerListKey[i] = os.str(); 
		}*/
		m_sUserCallerListKey = cs[6];

	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_sSkillGroupName
		   << ";" << m_sSkillGroupId 
		   << ";" << m_sManagerId 
		   << ";" << m_iNeedSms_OnHungup 
		   << ";" << m_sAgentInfoKey 
		   << ";" << m_sWaitingAgentListKey
		   << ";" << m_sUserCallerListKey; 

		return os.str();
	}

}SkillGroupInfo_Redis;

typedef struct __AgentInfo_Redis
{
	string m_sAgentName;
	string m_sNickName;
	int m_iLoginType;
	int m_iAgentNatType;
	string m_sAgentPhoneNo;
	string m_sExtNo;
	unsigned int m_ulAgentIP;
	unsigned int m_ulLanIP;
	unsigned short m_usAgentPort;
	bool m_bNeedHeartBeat;
	string m_sManagerid;
	string m_sCorporation;
	string m_sDepartment;
	string m_sDeptNickName;
	string m_sDeptid;
	string m_sSkillList;
	
	int m_iCurState;
	string m_sCurStateBeginTime;
	int m_iPreState;
	string m_sPreStateBeginTime;
	int m_iParkReasonID;
	string m_sParkReason;
	bool m_bSendHangupCall;
	string m_sCallID;
	bool m_bCallin_Cur;
	bool m_bInnerCall_Cur;
	bool m_bPSTNCall_Cur;
	bool m_bCallin_Hold;
	bool m_bInnerCall_Hold;
	bool m_bPSTNCall_Hold;
	bool m_bPSTNHungup;
	string m_sUUIData;
	string m_sUUIData2;
	string m_sAgentUUIData;
	string m_sOriginalData;
	string m_sPrivateData;

	unsigned long m_iTalkBeginTime;
	unsigned long m_iRingStateMark_OnlyPSTN;
	string m_sTheOtherAgentID;
	string m_sCurSkillInfo;
	string m_sOriSkillInfo;

	string m_sUserInfoFieldName;		//Redis中UserInfoList下的Field名称
	bool m_bCallFromQueue;
	string m_sCallAgentID;
	unsigned long m_iAnswerTimeSpan;
	string m_sComSymbol;
	string m_sQueueArriveTime;

	int m_iCalledPartyType;
	string m_sCalledParty;
	string m_sCalledCorp;
	string m_sCalledDepart;
	unsigned long m_iAgentEnqueueTime;
	unsigned long m_iAgentQueueSpanTime;
	string m_sCalledNo;

	string m_sFirstName;
	string m_sFirstLineNo;
	string m_sThirdName;
	string m_sFirstPartyLineNo;
	string m_sSecondPartyLineNo;
	int m_iConferenceType;
	string m_sConferenceID;
	int m_iConferenceRole;
	bool m_bApplyforConference;
	unsigned long m_iStateBeginTime;
	string m_CurGwUserLineNo;
	unsigned int m_CurUserLineIP;
	unsigned short m_CurUserLinePort;
	string m_WebPstnAgentCurGwLine;
	unsigned int m_CurGwLineIP;
	unsigned short m_CurGwLinePort;
	unsigned short m_usLanMainSockPort;
	unsigned int sipAgentPhoneIP1;
	unsigned short sipAgentPhoneRtpPort1;
	unsigned short sipAgentPhoneRtcpPort1;
	string m_sIPNo_TheAgent;
	unsigned int m_gwCtrlIP;
	unsigned short m_gwCtrlPort;

	__AgentInfo_Redis()
	{

	}

	__AgentInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, ";" );

		int i = 0;
		m_sAgentName = cs[i++];
		m_sNickName = cs[i++];
		m_iLoginType = atoi( cs[i++].c_str() );
		m_iAgentNatType = atoi( cs[i++].c_str() );
		m_sAgentPhoneNo = cs[i++];
		m_sExtNo = cs[i++];
		m_ulAgentIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_ulLanIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_usAgentPort = atoi( cs[i++].c_str() );
		m_bNeedHeartBeat = false;
		m_sManagerid = cs[i++];
		m_sCorporation = cs[i++];
		m_sDepartment = cs[i++];
		m_sDeptNickName = cs[i++];
		m_sDeptid = cs[i++];
		m_sSkillList = cs[i++];
		
		/*{
			map<int, list<string> > m_SkillMap_Waiting;
			map<int, list<string> > m_SkillMap_Working;

			CSpliter spSkillGroupId(cs[13], "|", ",");
			string strSGIdTemp = "";
			string strSkillLevel = "";
			int iSkillLevel = 0;
			for ( int i=0; i<spSkillGroupId.GetSize(); i++ )
			{
				strSGIdTemp = spSkillGroupId[i][0];
				strSkillLevel = spSkillGroupId[i][2];
				iSkillLevel = atoi(strSkillLevel.c_str());

				if ( !strSGIdTemp.empty() && !strSGNameTemp.empty() && iSkillLevel > 0 && iSkillLevel < 10 )
				{
					string sWaitingKey = m_sManagerid;
					sWaitingKey += "^";
					sWaitingKey += strSGIdTemp;
					sWaitingKey += "@WaitingList";

					m_SkillMap_Waiting[iSkillLevel].push_back(sWaitingKey);

					string sWorkingKey = m_sManagerid;
					sWorkingKey += "^";
					sWorkingKey += "@UserList";
					for ( int j=1; j<5; j++ )
					{
						char ac[256] = {0};
						sprintf(ac, "%s_%d", sWorkingKey.c_str(), j);
						m_SkillMap_Working[iSkillLevel].push_back( string(ac) );
					}

				}
			}
		}*/

		m_iCurState = atoi( cs[i++].c_str() );
		m_sCurStateBeginTime = cs[i++];
		m_iPreState = atoi( cs[i++].c_str() );
		m_sPreStateBeginTime = cs[i++];
		m_iParkReasonID = atoi( cs[i++].c_str() );
		m_sParkReason = cs[i++];
		m_bSendHangupCall = (atoi( cs[i++].c_str() )==0) ? false : true;
		m_sCallID = cs[i++];
		m_bCallin_Cur = (atoi( cs[i++].c_str() )==0) ? false : true;

		switch ( atoi( cs[i++].c_str() ) )
		{
		case 0:
			{
				m_bInnerCall_Cur = false;
				m_bPSTNCall_Cur = false;

				break;
			}
		case 1:
			{
				m_bInnerCall_Cur = false;
				m_bPSTNCall_Cur = true;

				break;
			}
		case 2:
			{
				m_bInnerCall_Cur = true;
				m_bPSTNCall_Cur = false;

				break;
			}
		case 3:
			{
				m_bInnerCall_Cur = true;
				m_bPSTNCall_Cur = true;

				break;
			}
		default:
			break;
		}

		m_bCallin_Hold = (atoi( cs[i++].c_str() )==0) ? false : true;

		switch ( atoi( cs[i++].c_str() ) )
		{
		case 0:
			{
				m_bInnerCall_Hold = false;
				m_bPSTNCall_Hold = false;

				break;
			}
		case 1:
			{
				m_bInnerCall_Hold = false;
				m_bPSTNCall_Hold = true;

				break;
			}
		case 2:
			{
				m_bInnerCall_Hold = true;
				m_bPSTNCall_Hold = false;

				break;
			}
		case 3:
			{
				m_bInnerCall_Hold = true;
				m_bPSTNCall_Hold = true;

				break;
			}
		default:
			break;
		}

		m_bPSTNHungup = (atoi( cs[i++].c_str() )==0) ? false : true;
		m_sUUIData = cs[i++];
		m_sUUIData2 = cs[i++];
		m_sAgentUUIData = cs[i++];
		m_sOriginalData = cs[i++];
		m_sPrivateData = cs[i++];
		m_iTalkBeginTime = strtoul( cs[i++].c_str(), 0, 10 );
		m_iRingStateMark_OnlyPSTN = strtoul( cs[i++].c_str(), 0, 10 );
		m_sTheOtherAgentID = cs[i++];
		m_sCurSkillInfo = cs[i++];
		m_sOriSkillInfo = cs[i++];
		m_sUserInfoFieldName = cs[i++];
		m_bCallFromQueue = (atoi( cs[i++].c_str() )==0) ? false : true;
		m_sCallAgentID = cs[i++];
		m_iAnswerTimeSpan = strtoul( cs[i++].c_str(), 0, 10 );
		m_sComSymbol = cs[i++];
		m_sQueueArriveTime = cs[i++];

		m_iCalledPartyType = atoi( cs[i++].c_str() );
		m_sCalledParty = cs[i++];
		m_sCalledCorp = cs[i++];
		m_sCalledDepart = cs[i++];
		m_iAgentEnqueueTime = strtoul( cs[i++].c_str(), 0, 10 );
		m_iAgentQueueSpanTime = strtoul( cs[i++].c_str(), 0, 10 );
		m_sCalledNo = cs[i++];
		m_sFirstName = cs[i++];
		m_sFirstLineNo = cs[i++];
		m_sThirdName = cs[i++];
		m_sFirstPartyLineNo = cs[i++];
		m_sSecondPartyLineNo = cs[i++];
		m_iConferenceType = atoi( cs[i++].c_str() );
		m_sConferenceID = cs[i++];
		m_iConferenceRole = atoi( cs[i++].c_str() );
		m_bApplyforConference = (atoi( cs[i++].c_str() )==0) ? false : true;
		m_iStateBeginTime = strtoul( cs[i++].c_str(), 0, 10 );
		m_CurGwUserLineNo = cs[i++];
		m_CurUserLineIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_CurUserLinePort = atoi( cs[i++].c_str() );
		m_WebPstnAgentCurGwLine = cs[i++];
		m_CurGwLineIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_CurGwLinePort = atoi( cs[i++].c_str() );
		m_usLanMainSockPort = atoi( cs[i++].c_str() );
		sipAgentPhoneIP1 = strtoul( cs[i++].c_str(), 0, 10 );
		sipAgentPhoneRtpPort1 = atoi( cs[i++].c_str() );
		sipAgentPhoneRtcpPort1 = atoi( cs[i++].c_str() );
		m_sIPNo_TheAgent = cs[i++];
		m_gwCtrlIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_gwCtrlPort = atoi( cs[i++].c_str() );

	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_sAgentName
		   << ";" << m_sNickName
		   << ";" << m_iLoginType
		   << ";" << m_iAgentNatType
		   << ";" << m_sAgentPhoneNo
		   << ";" << m_sExtNo
		   << ";" << m_ulAgentIP
		   << ";" << m_ulLanIP
		   << ";" << m_usAgentPort
		   << ";" << m_sManagerid
		   << ";" << m_sCorporation
		   << ";" << m_sDepartment
		   << ";" << m_sDeptNickName
		   << ";" << m_sDeptid
		   << ";" << m_sSkillList
		   << ";" << m_iCurState
		   << ";" << m_sCurStateBeginTime
		   << ";" << m_iPreState
		   << ";" << m_sPreStateBeginTime
		   << ";" << m_iParkReasonID
		   << ";" << m_sParkReason
		   << ";" << (int)m_bSendHangupCall
		   << ";" << m_sCallID
		   << ";" << (int)m_bCallin_Cur;

		if ( m_bInnerCall_Cur && m_bPSTNCall_Cur )
		{
			os << ";3";
		}
		else if ( m_bInnerCall_Cur && !m_bPSTNCall_Cur )
		{
			os << ";2";
		}
		else if ( !m_bInnerCall_Cur && m_bPSTNCall_Cur )
		{
			os << ";1";
		}
		else if ( !m_bInnerCall_Cur && !m_bPSTNCall_Cur )
		{
			os << ";0";
		}

		os << ";" << (int)m_bCallin_Hold;

		if ( m_bInnerCall_Hold && m_bPSTNCall_Hold )
		{
			os << ";3";
		}
		else if ( m_bInnerCall_Hold && !m_bPSTNCall_Hold )
		{
			os << ";2";
		}
		else if ( !m_bInnerCall_Hold && m_bPSTNCall_Hold )
		{
			os << ";1";
		}
		else if ( !m_bInnerCall_Hold && !m_bPSTNCall_Hold )
		{
			os << ";0";
		}

		os << ";" << (int)m_bPSTNHungup
		   << ";" << m_sUUIData
		   << ";" << m_sUUIData2
		   << ";" << m_sAgentUUIData
		   << ";" << m_sOriginalData
		   << ";" << m_sPrivateData
		   << ";" << m_iTalkBeginTime
		   << ";" << m_iRingStateMark_OnlyPSTN
		   << ";" << m_sTheOtherAgentID
		   << ";" << m_sCurSkillInfo
		   << ";" << m_sOriSkillInfo
		   << ";" << m_sUserInfoFieldName
		   << ";" << (int)m_bCallFromQueue
		   << ";" << m_sCallAgentID
		   << ";" << m_iAnswerTimeSpan
		   << ";" << m_sComSymbol
		   << ";" << m_sQueueArriveTime
		   << ";" << m_iCalledPartyType
		   << ";" << m_sCalledParty
		   << ";" << m_sCalledCorp
		   << ";" << m_sCalledDepart
		   << ";" << m_iAgentEnqueueTime
		   << ";" << m_iAgentQueueSpanTime
		   << ";" << m_sCalledNo
		   << ";" << m_sFirstName
		   << ";" << m_sFirstLineNo
		   << ";" << m_sThirdName
		   << ";" << m_sFirstPartyLineNo
		   << ";" << m_sSecondPartyLineNo
		   << ";" << m_iConferenceType
		   << ";" << m_sConferenceID
		   << ";" << m_iConferenceRole
		   << ";" << (int)m_bApplyforConference
		   << ";" << m_iStateBeginTime
		   << ";" << m_CurGwUserLineNo
		   << ";" << m_CurUserLineIP
		   << ";" << m_CurUserLinePort
		   << ";" << m_WebPstnAgentCurGwLine
		   << ";" << m_CurGwLineIP
		   << ";" << m_CurGwLinePort
		   << ";" << m_usLanMainSockPort
		   << ";" << sipAgentPhoneIP1
		   << ";" << sipAgentPhoneRtpPort1
		   << ";" << sipAgentPhoneRtcpPort1
		   << ";" << m_sIPNo_TheAgent
		   << ";" << m_gwCtrlIP
		   << ";" << m_gwCtrlPort;

		return os.str();
	}

}AgentInfo_Redis;

typedef struct __UserInfo_Redis 
{
	string m_sUserName;
	int m_iUserLevel;
	int m_iUserType;
	int m_iUserNatType;
	unsigned long m_iPreHeartBeatMark;
	int m_iUserState;
	string m_sOriginCalledTarget;
	string m_sCalledKeyWord;
	string m_sCorpName;
	string m_sCalledDepartment;
	string m_sCalledSkillGroup;
	bool m_bCallSkillGroup;
	string m_sUUIData;
	unsigned long m_iEnqueueTime;
	unsigned long m_iQueueSpanTime;
	bool m_CalledIsDept;
	string m_sCallFailedAgents;		//呼叫失败座席名列表，以逗号分隔
	unsigned int m_audioIP;
	unsigned short m_audioPort;
	unsigned int m_lanIP;
	unsigned int m_ulUserIP;
	unsigned short m_usUserPort;
	string m_sPartNo;
	string m_sComSymbol;
	unsigned int m_SipBoardIP;
	unsigned short m_SipBoardRtpPort;
	unsigned short m_SipBoardRtcpPort;
	unsigned int m_SipTransIP;
	unsigned short m_SipTransRtpPort;
	unsigned short m_SipTransRtcpPort;
	string m_GatewayTrans;
	string m_GatewayCodec;
	int m_GatewayUnit;
	int m_iCallType;
	string m_CallID_sCallIn;
	string m_sCallerNo;
	string m_sCalledNo;
	string m_sResponseAgentID;
	string m_sManagerid;
	unsigned short m_audioRtpPort;

	__UserInfo_Redis()
	{

	}

	__UserInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, ";" );

		int i = 0;
		m_sUserName = cs[i++];
		m_iUserLevel = atoi( cs[i++].c_str() );
		m_iUserType = atoi( cs[i++].c_str() );
		m_iUserNatType = atoi( cs[i++].c_str() );
		m_iPreHeartBeatMark = strtoul( cs[i++].c_str(), 0, 10 );
		m_iUserState = atoi( cs[i++].c_str() );
		m_sOriginCalledTarget = cs[i++];
		m_sCalledKeyWord = cs[i++];
		m_sCorpName = cs[i++];
		m_sCalledDepartment = cs[i++];
		m_sCalledSkillGroup = cs[i++];
		m_bCallSkillGroup = (atoi( cs[i++].c_str() )==0) ? false : true;
		m_sUUIData = cs[i++];
		m_iEnqueueTime = strtoul( cs[i++].c_str(), 0, 10 );
		m_iQueueSpanTime = strtoul( cs[i++].c_str(), 0, 10 );
		m_CalledIsDept = (atoi( cs[i++].c_str() )==0) ? false : true;
		m_sCallFailedAgents = cs[i++];
		m_audioIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_audioPort = atoi( cs[i++].c_str() );
		m_lanIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_ulUserIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_usUserPort = atoi( cs[i++].c_str() );
		m_sPartNo = cs[i++];
		m_sComSymbol = cs[i++];
		m_SipBoardIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_SipBoardRtpPort = atoi( cs[i++].c_str() );
		m_SipBoardRtcpPort = atoi( cs[i++].c_str() );
		m_SipTransIP = strtoul( cs[i++].c_str(), 0, 10 );
		m_SipTransRtpPort = atoi( cs[i++].c_str() );
		m_SipTransRtcpPort = atoi( cs[i++].c_str() );
		m_GatewayTrans = cs[i++];
		m_GatewayCodec = cs[i++];
		m_GatewayUnit = atoi( cs[i++].c_str() );
		m_iCallType = atoi( cs[i++].c_str() );
		m_CallID_sCallIn = cs[i++];
		m_sCallerNo = cs[i++];
		m_sCalledNo = cs[i++];
		m_sResponseAgentID = cs[i++];
		m_sManagerid = cs[i++];
		m_audioRtpPort = atoi( cs[i++].c_str() );

	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_sUserName
		   << ";" << m_iUserLevel
		   << ";" << m_iUserType
		   << ";" << m_iUserNatType
		   << ";" << m_iPreHeartBeatMark
		   << ";" << m_iUserState
		   << ";" << m_sOriginCalledTarget
		   << ";" << m_sCalledKeyWord
		   << ";" << m_sCorpName
		   << ";" << m_sCalledDepartment
		   << ";" << m_sCalledSkillGroup
		   << ";" << (int)m_bCallSkillGroup
		   << ";" << m_sUUIData
		   << ";" << m_iEnqueueTime
		   << ";" << m_iQueueSpanTime
		   << ";" << (int)m_CalledIsDept
		   << ";" << m_sCallFailedAgents
		   << ";" << m_audioIP
		   << ";" << m_audioPort
		   << ";" << m_lanIP
		   << ";" << m_ulUserIP
		   << ";" << m_usUserPort
		   << ";" << m_sPartNo
		   << ";" << m_sComSymbol
		   << ";" << m_SipBoardIP
		   << ";" << m_SipBoardRtpPort
		   << ";" << m_SipBoardRtcpPort
		   << ";" << m_SipTransIP
		   << ";" << m_SipTransRtpPort
		   << ";" << m_SipTransRtcpPort
		   << ";" << m_GatewayTrans
		   << ";" << m_GatewayCodec
		   << ";" << m_GatewayUnit
		   << ";" << m_iCallType
		   << ";" << m_CallID_sCallIn
		   << ";" << m_sCallerNo
		   << ";" << m_sCalledNo
		   << ";" << m_sResponseAgentID
		   << ";" << m_sManagerid
		   << ";" << m_audioRtpPort;

		return os.str();
	}

}UserInfo_Redis;

typedef struct __CallerPartyInfo_Redis 
{
	int m_iCallerPartyType;
	string m_sCallerPartyName;

	__CallerPartyInfo_Redis()
	{

	}

	__CallerPartyInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, "," );

		m_iCallerPartyType = atoi( cs[0].c_str() );
		m_sCallerPartyName = cs[1];
		
	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_iCallerPartyType
		   << "," << m_sCallerPartyName; 

		return os.str();
	}

}CallerPartyInfo_Redis;

typedef struct __ServiceInfo_Redis
{
	int               m_iServiceID;
	string            m_sServerName;
	int               m_iServerType;
	unsigned long     m_ulWanIP;
	unsigned long     m_ulLanIP;
	unsigned short    m_usPort;
	char              m_cLoad;			    //0为删除，1为加载
	int               m_iOperationMode;		//默认为2，表示须有两个心跳服务；为0表示无心跳服务；
	int               m_iNeedProxyRegister;	//int型，默认为0：	0表示直接注册，1表示代理注册；

	__ServiceInfo_Redis()
	{
		m_iServiceID = 0;
	}

	__ServiceInfo_Redis(string sContent)
	{
		CSpliter cs( sContent, ";" );

		int i = 0;
		m_iServiceID = atoi( cs[i++].c_str() );
		m_sServerName = cs[i++];
		m_iServerType = atoi( cs[i++].c_str() );

		string sTempIP = cs[i++];
		m_ulWanIP = ConvertIP(sTempIP);
		sTempIP = cs[i++];
		m_ulLanIP = ConvertIP(sTempIP);

		m_usPort = atoi( cs[i++].c_str() );
		m_cLoad = (atoi( cs[i++].c_str() )==0) ? '0' : '1';
		m_iOperationMode = atoi( cs[i++].c_str() );
		m_iNeedProxyRegister = atoi( cs[i++].c_str() );

	}

	string ToString()
	{
		ostringstream os;
		os.str("");
		os << m_iServiceID 
			<< ";" << m_sServerName 
			<< ";" << m_iServerType 
			<< ";" << m_ulWanIP 
			<< ";" << m_ulLanIP 
			<< ";" << m_usPort 
			<< ";" << m_cLoad 
			<< ";" << m_iOperationMode 
			<< ";" << m_iNeedProxyRegister;
	}
}ServiceInfo_Redis;

enum ERedisCommandResult
{
	REPLY_STRING = 1,
	REPLY_ARRAY = 2,
	REPLY_INTEGER = 3,
	REPLY_NIL = 4,
	REPLY_STATUS = 5,
	REPLY_ERROR = 6

};

class RedisReply
{
public:
	RedisReply()
		:resultType(0), resultValues(0), elements(0)
	{}

	RedisReply & operator = (redisReply & reply)
	{
		resultType = reply.type;
		switch (resultType)
		{
			case REDIS_REPLY_ARRAY:
			{
				for (int i = 0; i < reply.elements; i++)
				{
					elements.push_back(RedisReply());
					*elements.rbegin() = *reply.element[i];
				}

				break;
			}
			default:
			{
				resultValues.resize(reply.len);
				memcpy(&resultValues[0], reply.str, reply.len);
				break;
			}
		}

		return *this;
	}

public:
	int resultType;
	vector<char> resultValues;
	vector<RedisReply> elements;
};

class CRedisClient
{
public:
	CRedisClient(string name);
public:
	~CRedisClient(void);
public:
	int		ConnectRedisServer(string strip = "", int nport = 0, int nServerindex = 0, string sRedisPwd = "");
	int		EexcRedisCommand(string strCommand, char* pOutStr, long long* pOutInt);
	void	EexcRedisCommand(string strCommand, RedisReply & result);
	void    EexcRedisCommandArgv(vector<vector<char> > cmd, RedisReply & result);
	void	FreeRedisReplyObject(void *reply);

	bool	InitLoadSrcipt();
	string	loadScript(string sCommend);
	bool	evalsha(string sScriptType, char** argv, int iKeyCount, int iArgvCount);
	void	clearBuff();

	string  m_sname;
	string  m_strredisip;
	int     m_nredisport;
	int     m_nrdisidex;
	string  m_strRedisPwd;

	map<string, string> m_redisScriptMap;
	map<string, string> m_redisSHAMap;
	int					m_iResultType;
	map<int, string>	m_redisStringResultMap;
	map<int, int>		m_redisIntResultMap;
	string				m_iRedisErrorCode;
	//int					m_iExecutionTimes;

private:
	redisContext*  m_redisConnet;				//redis
	string         m_strdescriptioninfo;
};
