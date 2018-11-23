// UserInfo.h: interface for the CUserInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERINFO_H__36ACDAC6_B623_4434_80F8_485DA05CAAAD__INCLUDED_)
#define AFX_USERINFO_H__36ACDAC6_B623_4434_80F8_485DA05CAAAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <set>
using namespace std;

#include "public/IPCCMessage.h"

class CUserInfo  
{
public:
	CUserInfo();
	virtual ~CUserInfo();
    void ClearCallFailed();
    string ToString();
    bool FromString(string & str);
public:
	string m_CallID_sCallIn;       //1 ���л�˳��
	string m_sCallerNo;            //2
	string m_sUserName;            //3
	string m_sCalledNo;            //4
	string m_sCorpName;            //5
	string m_sOriginCalledTarget;  //6
	string m_sCalledKeyWord;       //7
    string m_sResponseAgentID;     //8
    string m_sUUIData;             //9
    int m_iUserLevel;              //10
    int m_iUserType;               //11
    int m_iCallType;               //12
    string m_sPartNo;              //13
    string m_sComSymbol;           //14
    CAudioStreamDescription m_asd; //15
    uint32_t m_lanIP;              //16
    uint32_t m_audioIP;            //17
    uint16_t m_audioPort;          //18
    uint16_t m_audioRtpPort;       //19
    uint32_t m_ulUserIP;           //20
    uint16_t m_usUserPort;         //21
    uint32_t m_iQueueType;         //22 �������ͣ�0δ�Ŷӣ�1���ţ�2������
    string   m_sQueueID;           //23 ����id������Ϊ����ʱ��Ϊ����id��Ϊ����ʱ��Ϊ������id
    string   m_sQueueName;         //24 ������������Ϊ����ʱ��Ϊ��������Ϊ����ʱ��Ϊ��������
   //�������л��ĳ�Ա                  
    set<string> m_CallFailedAgents;   
    uint32_t m_iEnqueueTime;		//�û��������ʱ��
    bool isQueueing;

};

#endif // !defined(AFX_USERINFO_H__36ACDAC6_B623_4434_80F8_485DA05CAAAD__INCLUDED_)
