#ifndef __GLOBALOPERATION__H__
#define __GLOBALOPERATION__H__

#include <string>
#include <cctype>

#include <boost/filesystem.hpp>

#include "public/intdef.h"

#define _Media_Video 0
#define _Media_Audio 1

#define WM_MEDIADATA_LOSTRATE WM_USER+601

long filesize(FILE *stream);

unsigned long gettickcount(void);
boost::filesystem::path GetAppPath();
std::string GetLocalIP();
std::string IntToString(int ivalue);
bool IsMediaData(const char *buf, int size);

void GetIPConfigInfo();
std::string GetPhysicalAddrByIP(std::string ip);
std::string GetGateWayeInfoByIP(std::string ip);
bool IsLanIP(std::string ip);

//void Multi_TerminateThread(HANDLE& handle);
//void Multi_TerminateTimer(HWND hWnd, int& nTimerID);

std::string ConvertNatIP2HostIP(boost::uint32_t ip);
int ConvertNatPort2HostPort(unsigned short port);
//unsigned long ConvertNatIP2HostIPi(unsigned long ip);

//void MySleep(int msCount);
unsigned short GetRandomPort();

int GetLocalTimeInfo(int& hour, int& minute, int& second);

class CCreditUDP;
struct SIPCCMessage;
void SendIPCCMsg(CCreditUDP* pSocket, SIPCCMessage* pMsg, std::string iIp,const unsigned short & iPort);

std::string getmac_one (int lana_num);

int CalTimeSpan(int begin, int end);
unsigned long CalTimeSpan(unsigned long begin, unsigned long end);

void convert8to16(unsigned char* pre_data, int size, short* post_data);
void convert16to8(short* pre_data, int size, unsigned char* post_data);

short search(
					int		val,				/* changed from "short" *drago* */
					short	*table,
					int		size);
unsigned char linear2alaw(int pcm_val);
int linear2pcma(unsigned char* pOut, short* pIn, int iSizeIn);


std::string NewGlobalInvokeID();


std::string DecryptBase64NDes(std::string sCode, char sKey[8]);
bool IsMobilePhone(std::string sPhone);
bool IsLocolPhone(std::string sPhone, std::string sLocolCode);

/*
#ifdef MAKEMSGLOG
void PushMsgLog(std::string sLog, char* pBuf, int iLen, unsigned long ip=0, unsigned short port=0);
void WriteMsgLog(std::string sLog, unsigned long ip=0, unsigned short port=0);
#endif
*/

// TestMsgError
struct AgentInfoStruct;
void AddAgentInfoToMap(std::string agentID, std::string corpName, unsigned long ip, unsigned short port);
void DelAgentInfoFromMap(std::string agentID);
AgentInfoStruct* FindAgentInfoByIP(unsigned long ip, unsigned short port);
AgentInfoStruct* FindAgentInfoByAgent(std::string agentID);
void WriteMsgError(std::string msg, std::string reason, unsigned long ip=0, unsigned short port=0, std::string agentID="");
void WriteMsgErrorAcd(std::string msg, std::string reason, int currentLen, unsigned long ip=0, unsigned short port=0, std::string agentID="");
void WriteMsgErrorAcd(std::string msg, std::string reason, int currentTotalLen, int realTotalLen, unsigned long ip=0, unsigned short port=0, std::string agentID="");
void WriteMsgError(std::string msg, std::string reason, int currentLen, unsigned long ip=0, unsigned short port=0);
void WriteMsgError(std::string msg, std::string reason, int currentTotalLen, int realTotalLen, unsigned long ip=0, unsigned short port=0);
std::string GetLocalTimeTemp();
int getModuleFileName( char* sModuleName, char* sFileName, int nSize);
//std::string GetMsgLogPath();
//bool IsACDServer();
/*
VerifyData:
	currentLen: 当前需要校验的字段长度
	maxLen: 当前字段最大长度
	msg: 信令名称
	field: 需要校验的字段名称
	ip: 源IP
	port: 源端口
	agentID: 发送信令的座席ID（不是必须的，主要是ACDServer）
*/
bool VerifyData(int currentLen, int maxLen, std::string msg="", std::string field="", unsigned long ip=0, unsigned short port=0, bool isAcd=false, std::string agentID="");
/*
VerifyData:
	currentTotalLen: 已解析的包长
	realPackLen: 实际包的总长
	msg: 信令名称
	field: 需要校验的字段名称
	ip: 源IP
	port: 源端口
	agentID: 发送信令的座席ID（不是必须的，主要是ACDServer）
*/
bool VerifyTotalLen(int currentTotalLen, int realPackLen, std::string msg, std::string field, unsigned long ip=0, unsigned short port=0, bool isAcd=false, std::string agentID="");
void FixPhoneNo(char* src, char* dest);

int encrypt_string(const char *str, int buflen, char*& encstring);
int decrypt_string(const char *str, int length, char*& decstring);

unsigned short ConvertPort(int port);
unsigned int ConvertIP(std::string ip); // Just in order to be compatible with the 32 bit windows

char* itoa(int value, char*  str, int radix);

inline char charToLower(char c)
{
	return std::tolower(c);
}

inline char charToUpper(char c)
{
	return std::toupper(c);
}

void ConvertNumberIpAddress(boost::uint32_t &in_ipNumberAddress,std::string &out_validIpAddress);
std::string ConvertPortToString(boost::uint16_t iPort, bool bIsHostPort = true);

void Format(std::string &s, const char *format, ...);
void Format2(std::string &sDes,const char *cpformat, ...);
void Replace(std::string & strBig, const std::string & strsrc, const std::string &strdst);
std::string GetNewVersionInvokeID(unsigned long myIP=0);

void MakeUpper(std::string &s);
void MakeLower(std::string &s);

int parseCmdLine(int argc, char * * argv, const char * pcVersion);

std::string GetLocalTimeForMessageCall();

std::string URLEncode(std::string para);
void base64url_encode(char* str);
uint64_t UnixTimeStamp();

#endif // __GLOBALOPERATION__H__
