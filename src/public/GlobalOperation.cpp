#include "GlobalOperation.h"

#include <sys/timeb.h>
#include <stdarg.h>

#ifdef _MSC_VER
#include <WinSock2.h>
#else
#include <sys/times.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <libgen.h>
#include <stdint.h>
#include <sys/time.h>
#endif

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;
using namespace std;

#include "base64.h"
#include "DES.h"

bool g_bIsACDServer = false;
std::string g_strMsgLogPath = "";
time_t ParseDateTime_CURRENTDATETIME(std::string strtime);
time_t ParseDateTime_CurrentTime3(std::string strtime);
time_t ParseDateTime_CURRENTDATE(std::string strtime);
time_t ParseDateTime_CurrentTime2(std::string strtime);
time_t ParseDateTime_CurrentDay(std::string strtime);
time_t ParseDateTime_CurrentTime1(std::string strtime);

boost::filesystem::path GetAppPath()
{

	char szFullPath[255];
	char szDir[255];
	char szDrive[255];

	memset(szFullPath, 0, 255);
	memset(szDir, 0, 255);
	memset(szDrive, 0, 255);

#if defined(_MSC_VER) || defined(__BORLANDC__)
	::GetModuleFileName(NULL,szFullPath,sizeof(szFullPath)-1);
#elif defined (__GNUC__)
	getModuleFileName(NULL,szFullPath,sizeof(szFullPath)-1);
#endif

	boost::filesystem::path p(szFullPath);
	return p.parent_path();
}

std::string NewGlobalInvokeID()
{
	static std::string sComputer = "";
	if ( sComputer == "" )
	{
		char szhost[128]={0};
		gethostname(szhost,128);
		sComputer=szhost;
		
	}

	static int iCount = 0;
	static long iTime = (long)time(NULL);

	char c[50] = {0};
	int iLen = sprintf(c,"%s%08X%08X", sComputer.c_str(), iTime, iCount );

	iCount++;

	return c;
}

unsigned int ConvertIP(std::string ip)
{
	if(ip.empty())
		return -1;

	int iIp = (int)inet_addr(ip.c_str());

	return iIp;
}

unsigned short ConvertPort(int port)
{
	return htons((unsigned short)port);
}

std::string IntToString(int ivalue)
{
	std::string svalue("");

	ostringstream ostream;
	ostream.str("");
	ostream << ivalue;
	svalue = ostream.str();

	return svalue;
}

bool IsMediaData(const char *buf, int size)
{
	//判断是否 CreditUDP 消息
	char headInfo[8];
	memset(headInfo, 0, 8);
	if(size>=8)
		memcpy(headInfo, buf, 8);
	else
		return false;

	if(0!=memcmp(headInfo, "HartPack", 8) && 0!=memcmp(headInfo, "CreditUD", 8))
		return true;

	return false;
}

std::string GetLocalIP()
{
	std::string sLocalIP = "127.0.0.1";
	return sLocalIP;
}


std::string ConvertNatIP2HostIP(boost::uint32_t ip)
{
	std::string out_validIpAddress = "";

	if(ip<0||ip>4294967295)  
	{  
		out_validIpAddress = "0.0.0.0"; //invalid IP Address  
	}  
	else  
	{  
		boost::uint32_t divisor = 256;   
		boost::uint32_t ip_section1 = 0;  
		boost::uint32_t ip_section2 = 0;  
		boost::uint32_t ip_section3 = 0;  
		boost::uint32_t ip_section4 = 0;  

		std::string ip_section_str1;  
		std::string ip_section_str2;  
		std::string ip_section_str3;  
		std::string ip_section_str4;  
		std::string dot = ".";  

		ip_section1 = ip%divisor;  
		ip = ip/divisor;  
		ip_section2 = ip%divisor;  
		ip = ip/divisor;  
		ip_section3 = ip%divisor;  
		ip = ip/divisor;  
		ip_section4 = ip%divisor;  

		ip_section_str1 = boost::lexical_cast <std::string> (ip_section1);  
		ip_section_str2 = boost::lexical_cast <std::string> (ip_section2);  
		ip_section_str3 = boost::lexical_cast <std::string> (ip_section3);  
		ip_section_str4 = boost::lexical_cast <std::string> (ip_section4);  
		out_validIpAddress = ip_section_str4+dot+ip_section_str3+dot+ip_section_str2+dot+ip_section_str1;  
	}  

	return out_validIpAddress;
}

int ConvertNatPort2HostPort(unsigned short port)
{
	return ntohs(port);
}

int GetLocalTimeInfo(int& hour, int& minute, int& second)
{
	time_t now;
	struct tm *t1;

	time(&now);
	t1 = localtime(&now);

	hour = t1->tm_hour;
	minute = t1->tm_min;
	second = t1->tm_sec;

	return 0;
}

unsigned short GetRandomPort()
{
	unsigned short iTemp = 0;
	while ( 1 )
	{
		iTemp = rand();
		iTemp += rand();
		if( ( iTemp > 1024 ) && ( iTemp%1000 != 0 )
			&& ( iTemp > 30000 || iTemp < 10000 )
			&& (iTemp != 12581 /*ServerInfoDef::Port_AreaCodeServer*/))
/*			&& iTemp != ServerInfoDef::Port_ACDServer
			&& iTemp != ServerInfoDef::Port_CheckNatServerA
			&& iTemp != ServerInfoDef::Port_CheckNatServerB
			&& iTemp != ServerInfoDef::Port_DataCollectServer
			&& iTemp != ServerInfoDef::Port_GatewayManageDataServer
			&& iTemp != ServerInfoDef::Port_GatewayManageServer
			&& iTemp != ServerInfoDef::Port_GuideServer
			&& iTemp != ServerInfoDef::Port_LoginServer
			&& iTemp != ServerInfoDef::Port_RecordServer
			&& iTemp != ServerInfoDef::Port_RelayServer
			&& iTemp != ServerInfoDef::Port_SmartRouteDataServer
			&& iTemp != ServerInfoDef::Port_SmartRouteServer
			&& iTemp != ServerInfoDef::Port_SysManageServer
			&& iTemp != ServerInfoDef::Port_TransmitServer
			&& iTemp != ServerInfoDef::Port_UStateServer )
			*/
			break;
	}
	return iTemp;
}

#define MAXINTERFACES 16

int CalTimeSpan(int begin, int end)
{
	long timeSpan = end - begin;
	if(timeSpan <0)
		timeSpan = 0;
//		timeSpan = timeSpan + 4294967695;

	return timeSpan;
}

unsigned long CalTimeSpan(unsigned long begin, unsigned long end)
{
	unsigned long timeSpan = end - begin;
	if(timeSpan <0)
		timeSpan = 0;

	return timeSpan;
}

void convert8to16(unsigned char* pre_data, int size, short* post_data)
{
	int S = 0;
	while (S<size)
	{
		unsigned char alaw;
		memcpy(&alaw, (pre_data + S), 1);

		alaw ^= 0x55;  // A-law has alternate bits inverted for transmission

    	unsigned short sign = alaw&0x80;

    	short linear = alaw&0x1f;
    	linear <<= 4;
    	linear += 8;  // Add a 'half' bit (0x08) to place PCM value in middle of range

    	alaw &= 0x7f;
    	if(alaw>=0x20)
			{
			linear |= 0x100;  // Put in MSB
			unsigned short shift = (alaw>>4)-1;
			linear <<= shift;
			}

		if(!sign)
		{
			linear = -linear;
			memcpy(post_data+S, &linear,2);
		}
		else
		memcpy(post_data+S, &linear,2);

		S++;
	}
}
void convert16to8(short* pre_data, int size, unsigned char* post_data)
{
	int S = 0;
	while (S<size)
	{
		short p;
		memcpy(&p, (pre_data + S), 2);
		unsigned short	a;  // A-law value we are forming
		unsigned char b;
		if(p<0)
			{
			// -ve value
			// Note, ones compliment is used here as this keeps encoding symetrical
			// and equal spaced around zero cross-over, (it also matches the standard).
			p = ~p;
			a = 0x00; // sign = 0
			}
		else
			{
			// +ve value
			a = 0x80; // sign = 1
			}

		// Calculate segment and interval numbers
		p >>= 4;
		if(p>=0x20)
			{
			if(p>=0x100)
				{
				p >>= 4;
				a += 0x40;
				}
			if(p>=0x40)
				{
				p >>= 2;
				a += 0x20;
				}
			if(p>=0x20)
				{
				p >>= 1;
				a += 0x10;
				}
			}
		// a&0x70 now holds segment value and 'p' the interval number

		a += p;  // a now equal to encoded A-law value

		a = a^0x55;
		b = (unsigned char)(a);
		memcpy(post_data+S, &b,1);
		S++;
	}

}

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)			/* Number of A-law segments. */
#define	SEG_SHIFT	(4)			/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */
static short seg_aend[8] = {0x1F, 0x3F, 0x7F, 0xFF,
				0x1FF, 0x3FF, 0x7FF, 0xFFF};
static short seg_uend[8] = {0x3F, 0x7F, 0xFF, 0x1FF,
				0x3FF, 0x7FF, 0xFFF, 0x1FFF};


short search(
					int		val,				/* changed from "short" *drago* */
					short	*table,
					int		size)				/* changed from "short" *drago* */
{
    int		i;									/* changed from "short" *drago* */

    for (i = 0; i < size; i++) {
        if (val <= *table++)
            return (i);
    }
    return (size);
}

unsigned char linear2alaw(int pcm_val)	/* 2's complement (16-bit range) */
    /* changed from "short" *drago* */
{
    int		mask;	/* changed from "short" *drago* */
    int		seg;	/* changed from "short" *drago* */
    unsigned char	aval;

    pcm_val = pcm_val >> 3;

    if (pcm_val >= 0) {
        mask = 0xD5;		/* sign (7th) bit = 1 */
    } else {
        mask = 0x55;		/* sign bit = 0 */
        pcm_val = -pcm_val - 1;
    }

    /* Convert the scaled magnitude to segment number. */
    seg = search(pcm_val, seg_aend, 8);

    /* Combine the sign, segment, and quantization bits. */

    if (seg >= 8)		/* out of range, return maximum value. */
        return (unsigned char) (0x7F ^ mask);
    else {
        aval = (unsigned char) seg << SEG_SHIFT;
        if (seg < 2)
            aval |= (pcm_val >> 1) & QUANT_MASK;
        else
            aval |= (pcm_val >> seg) & QUANT_MASK;
        return (aval ^ mask);
    }
}

int linear2pcma(unsigned char* pOut, short* pIn, int iSizeIn)
{
	int iTemp = iSizeIn;
	while ( --iTemp )
	{
		(*pOut++) = linear2alaw(*pIn++);
	}
	return iSizeIn;
}

long filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
}

std::string DecryptBase64NDes(std::string sCode, char sKey[8])
{
	char ch[256] = {0};
	int i = Base64Decode( ch, sCode.c_str(), sCode.length() );
	char ch2[256] = {0};
	CDES::RunDes( 1, false, ch, ch2, i, sKey, 8);

	return ch2;
}

bool IsMobilePhone(std::string sPhone)
{
	if ( sPhone == "" )
	{
		return false;
	}

	if ( (sPhone.length() == 11 && sPhone[0] == '1') || (sPhone.length() == 12 && sPhone.substr(0, 2) == "01") )
	{
		return true;
	}

	return false;
}

bool IsLocolPhone(std::string sPhone, std::string sLocolCode)
{
	if ( sPhone.substr( 0, 1 ) == "0" )
	{
		if ( sPhone.substr( 0, sLocolCode.length() ) == sLocolCode )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

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
bool VerifyData(int currentLen, int maxLen, std::string msg, std::string field, unsigned long ip, unsigned short port, bool isAcd, std::string agentID)
{
	if ( (currentLen>maxLen)||(currentLen<0) )
	{
		if ( !(msg.empty()) )
		{
			if ( isAcd )
			{
				//WriteMsgErrorAcd(msg, field + "is error", currentLen, ip, port, agentID);
			}
			else
			{
				//WriteMsgError(msg, field + "is error", currentLen, ip ,port);
			}
		}
		return false;
	}

	return true;
}

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
bool VerifyTotalLen(int currentTotalLen, int realPackLen, std::string msg, std::string field, unsigned long ip, unsigned short port, bool isAcd, std::string agentID)
{
	if ( (currentTotalLen>realPackLen)||(realPackLen<=0) )
	{
		if ( isAcd )
		{
			//WriteMsgErrorAcd(msg, field + "is error(currentTotalLen>realTotalLen)", currentTotalLen, realPackLen, ip, port, agentID);
		}
		else
		{
			//WriteMsgError(msg, field + "is error(currentTotalLen>realTotalLen)", currentTotalLen, realPackLen, ip ,port);
		}
		return false;
	}

	return true;
}
// TestMsgError

// +字符串取Right的封装函数 [6/17/2008 lixg]
std::string Right(const std::string &strSrc, unsigned int nCount)
{
	unsigned int unSrcCount = strSrc.length();
	if (nCount <=0)
		return std::string("");
	if (nCount > unSrcCount)
		return strSrc;
	else
		return strSrc.substr(unSrcCount-nCount, nCount);
}

//#ifndef _MSC_VER
char* strrev(char* szT)
{
   if ( !szT )                 // 处理传入的空串
        return "";
    int i = strlen(szT);
    int t = !(i%2)? 1 : 0;      // 检查串长度
    for(int j = i-1 , k = 0 ; j > (i/2 -t) ; j-- )
    {
        char ch  = szT[j];
        szT[j]   = szT[k];
        szT[k++] = ch;
    }
    return szT;
}
char* itoa(int value, char*  str, int radix)
{
	return (char*)boost::lexical_cast<std::string>(value).c_str();
}
//#endif

int getModuleFileName( char* sModuleName, char* sFileName, int nSize)
{
	int ret = 0;
	char sLine[1024] = { 0 };
	void* pSymbol = (void*)"";
	FILE *fp;
	char *pPath;

	fp = fopen ("/proc/self/maps", "r");
	if ( fp != NULL )
	{
	   while (!feof (fp))
	   {
			unsigned long start, end;

			if ( !fgets (sLine, sizeof (sLine), fp))
			 continue;
			if ( !strstr (sLine, " r-xp ") || !strchr (sLine, '/'))
			 continue;

			sscanf (sLine, "%lx-%lx ", &start, &end);
			if (pSymbol >= (void *) start && pSymbol < (void *) end)
			{
				 char *tmp;

				 /* Extract the filename; it is always an absolute path */
				 pPath = strchr (sLine, '/');

				 /* Get rid of the newline */
				 tmp = strrchr (pPath, '\n');
				 if (tmp) *tmp = 0;

				 /* Get rid of "(deleted)" */
				 //len = strlen (pPath);
				 //if (len > 10 && strcmp (pPath + len - 10, " (deleted)") == 0)
				 //{
				 // tmp = pPath + len - 10;
				 // *tmp = 0;
				 //}
				 strcpy( sFileName, pPath );
				 ret = strlen(sFileName);
			}
	   }
	   fclose (fp);
	}
	return ret;
}

std::string FormatTime_t(const time_t &t, const std::string &strForm )
{
	tm *pTm = localtime(&t);
	char ac[BUFSIZ] = {0};

	strftime(ac, BUFSIZ, strForm.c_str(),
		pTm);
	return ac;
}

#define	MAXFD	64

unsigned long gettickcount(void)
{
//	struct timeval tv;
//	gettimeofday(&tv, NULL);
//	return tv.tv_sec * 1000L + tv.tv_usec/1000L;
#ifdef _MSC_VER
	   return ::GetTickCount();
#else
	struct tms tm;

	//return times(&tm)*10;
	return times(&tm)*1000/sysconf(_SC_CLK_TCK);
#endif
}

void MakeUpper(std::string &s)
{
	char *temp;
    char *_p = (char *)malloc(s.length() + 1);
    strncpy(_p, s.c_str(), s.length() + 1);
	temp = _p;
    for(; *temp != '\0'; temp++)
		*temp = toupper(*temp);
    s = _p;
    free(_p);
}

void MakeLower(std::string &s)
{
	char *temp;
	char *_p = (char *)malloc(s.length() + 1);
    strncpy(_p, s.c_str(), s.length() + 1);
	temp = _p;
    for(; *temp != '\0'; temp++)
		*temp = tolower(*temp);
    s = _p;
    free(_p);
}

void Format2(std::string &sDes,const char *cpformat, ...)
{
	boost::format fmt(cpformat);
	va_list arList;
	va_start(arList, cpformat);

	int nFindPos = 0;
	std::string sFormat(cpformat);
	while (-1 != (nFindPos=sFormat.find("%")))
	{
		switch (sFormat[nFindPos+1])
		{
		case 's':
			fmt % (va_arg(arList,char*));
			break;
		case 'd':
#if defined(_MSC_VER) || defined(__BORLANDC__)
			fmt % (va_arg(arList,int));
#elif defined (__GNUC__)
			fmt % (va_arg(arList,int32_t));
#endif
			break;
		case 'f':
			fmt % (va_arg(arList,double));
			break;
		case '0': 	
			if (sFormat[nFindPos+4] == 'f')		// %0.2f
				fmt % (va_arg(arList,double));
			else
				//
			break;
		default:
			break;
		}
		sFormat.replace(0,nFindPos+1,"");
	}
	va_end(arList);
	sDes = fmt.str();
}

//格式化std::string
void Format(std::string &s,const char *format, ...)
{
	int bufsize = 1024;
	char *buf = new char[bufsize];
    memset(buf,0,bufsize);
	va_list args;
	va_start(args, format);

	// vsnprintf(); let the last char to the '\0'
	// so, std::string is the length of (bufsize-1), will run while(){}, again!
	vsnprintf(buf, bufsize, format, args);
      
	while ((strlen(buf) < 0) || (strlen(buf) >= (bufsize - 1)))
	{
        va_end(args);
        va_start(args, format);
		delete []buf;
		bufsize = bufsize * 2;
		buf = new char[bufsize]; 
		memset(buf,0,bufsize);
		vsnprintf(buf, bufsize, format, args);
	}
	va_end(args);
	buf[bufsize - 1] = '\0';
	s = buf;
	delete []buf;
}

void TrimLeft(std::string &s)
{
        s.erase(0,s.find_first_not_of("   \t\r\n"));
}
void TrimRight(std::string &s)
{
        s.erase(s.find_last_not_of("   \t\r\n")+1);
}

std::string GetLocalTime()
{
	ostringstream strStream;

	time_t now;
	struct tm *t1;

	time(&now);
	t1 = localtime(&now);

	strStream.str("");

	char year[5];
	char month[3];
	char day[3];
	char hour[3];
	char minute[3];
	char second[3];
	memset(year, 0, 5);
	memset(month, 0, 3);
	memset(day, 0, 3);
	memset(hour, 0, 3);
	memset(minute, 0, 3);
	memset(second, 0, 3);

	sprintf(year, "%4d", 1900+t1->tm_year);
	sprintf(month, "%02d", t1->tm_mon + 1);
	sprintf(day, "%02d", t1->tm_mday);
	sprintf(hour, "%02d", t1->tm_hour);
	sprintf(minute, "%02d", t1->tm_min);
	sprintf(second, "%02d", t1->tm_sec);

	strStream << year << "-" << month << "-" << day << " ";
	strStream << hour << ":" << minute << ":" << second;

	std::string sTime = strStream.str();

	return sTime;
}

void Replace(std::string & strBig, const std::string & strsrc, const std::string &strdst) {
        std::string::size_type pos=0;
        std::string::size_type srclen=strsrc.size();
        std::string::size_type dstlen=strdst.size();
        while( (pos=strBig.find(strsrc, pos)) != std::string::npos){
                strBig.replace(pos, srclen, strdst);
                pos += dstlen;
        }
}
bool checkstr(std::string strTime)
{
	int isize = strTime.length();
	for( int i = 0 ; i < isize ; i++ )
	{
		if(!isdigit(strTime[i]))
		{
			return false;
		}
	}
	return true;
}
time_t ParseDateTime(std::string strtime)
{
	int ilen = strtime.length();
	if(ilen == 19)
	{ // YYYY-mm-dd HH:MM:SS
		return ParseDateTime_CURRENTDATETIME(strtime);
	}
	else if(ilen == 14)
	{ // yyyymmddhhmmdd
		return ParseDateTime_CurrentTime3(strtime);
	}
	else if(ilen == 10)
	{ // YYYY-mm-dd
		return ParseDateTime_CURRENTDATE(strtime);
	}
	else if(ilen == 6)
	{ // hhmmdd
		return ParseDateTime_CurrentTime2(strtime);
	}
	else if(ilen == 8)
	{
		int pos = strtime.find(':');
		if(pos == -1)
		{ // yyyymmdd
			return ParseDateTime_CurrentDay(strtime);
		}
		else
		{ // HH:MM:SS or hh:mm:dd
			return ParseDateTime_CurrentTime1(strtime);
		}
	}
	else
	{
		return -1;
	}
}

time_t ParseDateTime_CURRENTTIME(std::string strtime)
{	// HH:MM:SS   			(24小时)  8
	std::string stmp;
	std::string stimetmp = strtime;
	char split = ':';
	int val[3] = {0,0,0};
	int pos[2] = {2,2};
	int i = 0;
	for( ; i < 2 && stimetmp.find(split) == pos[i] ; i++)
	{
		stmp = stimetmp.substr(0,pos[i]);
		val[i] = atoi(stmp.c_str());
		stimetmp = stimetmp.substr(pos[i]+1,stimetmp.length());
	}
	if(i != 2)
	{
		return -1;
	}

	val[2] = atoi(stimetmp.c_str());

//	m_tm.tm_year 	= 0;
//	m_tm.tm_mon 	= 0;
//	m_tm.tm_mday 	= 0;
//	m_tm.tm_hour 	= val[0];
//	m_tm.tm_min 	= val[1];
//	m_tm.tm_sec 	= val[2];

	//没有年月日的时间，认为是同一天的时间
	time_t t = val[0] * 60 * 60 + val[1] * 60 + val[2];
	// m_tm = *(localtime(&(m_ttime)));
//		m_tm.tm_hour = (m_tm.tm_hour - m_tm.tm_gmtoff/3600 + 24) % 24;
//	return true;
	return t;
}

time_t ParseDateTime_CURRENTDATE(std::string strtime)
{	// YYYY-mm-dd					  10
	std::string stmp;
	std::string stimetmp = strtime;
	char split = '-';
	int val[3] = {0,0,0};
	int pos[2] = {4,2};
	int i = 0;
	for( ; i < 2 && stimetmp.find(split) == pos[i] ; i++)
	{
		stmp = stimetmp.substr(0,pos[i]);
		if(!checkstr(stmp))
		{
			return -1;
		}
		val[i] = atoi(stmp.c_str());
		stimetmp = stimetmp.substr(pos[i]+1,stimetmp.length());
	}
	if(i != 2)
	{
		return -1;
	}

	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[2] = atoi(stimetmp.c_str());
	struct tm tmTemp;
	tmTemp.tm_year 	= val[0] - 1900;
	tmTemp.tm_mon 	= val[1] - 1;
	tmTemp.tm_mday 	= val[2];
	tmTemp.tm_hour 	= 0;
	tmTemp.tm_min 	= 0;
	tmTemp.tm_sec 	= 0;

	//t = mktime(&(tmTemp));
//	tmTemp = *(localtime(&(m_ttime)));
//		m_tm.tm_hour = (m_tm.tm_hour - m_tm.tm_gmtoff/3600 + 24) % 24;
	return mktime(&(tmTemp));// true;
}
bool isleap_year(int iyear)
{  // return true if the num of year is leap year
	int mod = iyear % 4;
	if( mod != 0 )
	{
		return false;
	}
	mod = iyear % 100;
	if( mod == 0 )
	{ // 可以被100整除
		int imod = iyear % 400;
		if( imod != 0 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}
int day(int iyear,int imon)
{
	int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

	if(isleap_year(iyear))
		days[1] = 29;
	return days[imon - 1];
}
time_t ParseDateTime_CURRENTDATETIME(std::string strtime)
{	// YYYY-mm-dd HH:MM:SS  (24小时)  19
	std::string stmp;
	std::string stimetmp = strtime;
	char split[5] = {'-','-',' ',':',':',};
	int val[6] = {0,0,0,0,0,0};
	int pos[5] = {4,2,2,2,2};
	int i = 0;
	for(i = 0;i < 5 && stimetmp.find(split[i]) == pos[i];i++)
	{
		stmp = stimetmp.substr(0,pos[i]);
		if(!checkstr(stmp))
		{
			return -1;
		}
		val[i] = atoi(stmp.c_str());
		stimetmp = stimetmp.substr(pos[i]+1,stimetmp.length());
	}
	if(i != 5)
	{
		return -1;
	}

	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[5] = atoi(stimetmp.c_str());

	if( ( val[1] < 0 ) || ( val[1] > 12 ) )
	{ // mon err
		return -1;
	}

	if( (val[2] < 0) || (val[2]) > day(val[0],val[1]) )
	{ // day err
		return -1;
	}

	if( ( val[3] > 23 ) || ( val[3] < 0 ) )
	{ // hours err
		return -1;
	}

	if( ( val[4] > 59 ) || ( val[4] < 0 ) )
	{ // min err
		return -1;
	}

	if( ( val[5] > 59 ) || ( val[5] < 0 ) )
	{
		return -1;
	}
	struct tm tmTemp;
	tmTemp.tm_year 	= val[0] - 1900;
	tmTemp.tm_mon 	= val[1] - 1;
	tmTemp.tm_mday 	= val[2];
	tmTemp.tm_hour 	= val[3];
	tmTemp.tm_min 	= val[4];
	tmTemp.tm_sec 	= val[5];

//	m_ttime = mktime(&(tmTemp));
//	tmTemp = *(localtime(&(m_ttime)));
//		tmTemp.tm_hour = (tmTemp.tm_hour - tmTemp.tm_gmtoff/3600 + 24) % 24;
	return mktime(&(tmTemp));//true;
}

time_t ParseDateTime_CurrentDay(std::string strtime)
{	// yyyymmdd    				 	8
	int val[3] = {0,0,0};
//		int pos = 0;
	std::string stmp = strtime;
	std::string stimetmp;

	stimetmp = stmp.substr(0,4);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[0] = atoi(stimetmp.c_str());
	stmp = stmp.substr(4,stmp.length());

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[1] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	if(!checkstr(stmp))
	{
		return -1;
	}
	val[2] = atoi(stmp.c_str());

	struct tm tmTemp;
	tmTemp.tm_year 	= val[0] - 1900;
	tmTemp.tm_mon 	= val[1] - 1;
	tmTemp.tm_mday 	= val[2];
	tmTemp.tm_hour 	= 0;
	tmTemp.tm_min 	= 0;
	tmTemp.tm_sec 	= 0;

//	m_ttime = mktime(&(tmTemp));
//	tmTemp = *(localtime(&(m_ttime)));
//		tmTemp.tm_hour = (tmTemp.tm_hour - tmTemp.tm_gmtoff/3600 + 24) % 24;
	return mktime(&(tmTemp));//true;
}

time_t ParseDateTime_CurrentTime1(std::string strtime)
{	// hh:mm:dd				(24小时) 8
	return ParseDateTime_CURRENTTIME(strtime);
}

time_t ParseDateTime_CurrentTime2(std::string strtime)
{	// hhmmdd				(24小时) 6
	int val[3] = {0,0,0};
//		int pos = 0;
	std::string stmp = strtime;
	std::string stimetmp;

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[0] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[1] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	if(!checkstr(stmp))
	{
		return -1;
	}
	val[2] = atoi(stmp.c_str());

//	m_ttime = val[0] * 60 * 60 + val[1] * 60 + val[2];
//	tmTemp = *(localtime(&(m_ttime)));
//		tmTemp.tm_hour = (tmTemp.tm_hour - tmTemp.tm_gmtoff/3600 + 24) % 24;
	return val[0] * 60 * 60 + val[1] * 60 + val[2];//true;
}

time_t ParseDateTime_CurrentTime3(std::string strtime)
{	// yyyymmddhhmmdd		(24小时) 14
	int val[6] = {0,0,0,0,0,0};
	std::string stmp = strtime;
	std::string stimetmp;

	stimetmp = stmp.substr(0,4);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[0] = atoi(stimetmp.c_str());
	stmp = stmp.substr(4,stmp.length());

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[1] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[2] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[3] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	stimetmp = stmp.substr(0,2);
	if(!checkstr(stimetmp))
	{
		return -1;
	}
	val[4] = atoi(stimetmp.c_str());
	stmp = stmp.substr(2,stmp.length());

	if(!checkstr(stmp))
	{
		return -1;
	}
	val[5] = atoi(stmp.c_str());
	struct tm tmTemp;
	tmTemp.tm_year 	= val[0] - 1900;
	tmTemp.tm_mon 	= val[1] -1;
	tmTemp.tm_mday 	= val[2];
	tmTemp.tm_hour 	= val[3];
	tmTemp.tm_min 	= val[4];
	tmTemp.tm_sec 	= val[5];

//	m_ttime = mktime(&(tmTemp));
//	m_ttime = m_ttime;
//	tmTemp = *(localtime(&(m_ttime)));
//		tmTemp.tm_hour = (tmTemp.tm_hour - tmTemp.tm_gmtoff/3600 + 24) % 24;
	return  mktime(&(tmTemp));// true;
}

int parseCmdLine( int argc, char * * argv, const char * pcVersion )
{
	if (argc >= 2)
	{
		// #./InfoBirdACD -d  // input this command run this
		if (strcmp(argv[1], "-d") == 0)
		{
			//daemon_init();
		}
		else if (	strcmp(argv[1], "--help") == 0 )
		{
			printf("Usage : ./ExeName [-v|-V|--version|--help] \n");
			return 0;
		}
		else if (		strcmp(argv[1], "-v") == 0
			|| strcmp(argv[1], "-V") == 0
			|| strcmp(argv[1], "--version") == 0 )
		{
			printf("Product Version : %s Compiled in  %s %s \n", pcVersion, __DATE__, __TIME__);
			return 0;
		}
		else
		{
			printf("Usage : ./ExeName [-v|-V|--version|--help] \n");
			return 0;
		}
	}
	return 1; // 继续
}

std::string GetNewVersionInvokeID(unsigned long myIP)
{
	static unsigned long loopup = 0;

      #if defined(_MSC_VER) || defined(__BORLANDC__)
	    _timeb systime;
	   _ftime(&systime);

     #elif defined (__GNUC__)
	     timeb systime;
	     ftime(&systime);	
      #endif
	
	static long lTime = (long)systime.time;
	static short iMillitm = (short)systime.millitm;

	if ( myIP == 0 )
	{
		srand(lTime);
	}
	else
	{
		srand(myIP);
	}
	static short iRand = rand() % 1000;

	char c[30] = {0};
	int iLen = sprintf(c,"N%08X%04X%04X%08X", lTime, iMillitm, iRand, loopup );

	loopup++;

	return c;
}
#ifdef MAKEMSGLOG
struct LogStruct
{
	std::string sLog;
	//char pBuf[1500];
	unsigned char pBuf[1500];	// [scq/2010-09-17]
	int iLen;
	unsigned long ip;
	unsigned short port;
};
static queue<LogStruct> g_queueLog;
static CRITICAL_SECTION g_LogQueueLock;
static bool g_bIsInit = false;

void PushMsgLog(std::string sLog, char* pBuf, int iLen, unsigned long ip, unsigned short port)
{
	if (iLen > 1500)
	{
		return;
	}

	if (!g_bIsInit)
	{
		InitializeCriticalSection(&g_LogQueueLock);
		g_bIsInit = true;
	}

	EnterCriticalSection(&g_LogQueueLock);
	if (g_queueLog.size() >= 20)
	{
		g_queueLog.pop();
	}
	LogStruct s;
	s.sLog = sLog;
	memcpy(s.pBuf, pBuf, iLen);
	s.iLen = iLen;
	s.port = port;
	s.ip = ip;
	g_queueLog.push(s);
	LeaveCriticalSection(&g_LogQueueLock);
}

void WriteMsgLog(std::string sLog, unsigned long ip, unsigned short port)
{
	FILE * fp = fopen("c:\\IbLog.txt","a");
	if(fp == NULL)
	{
		return;
	}

	fprintf(fp, "\r\n\r\n%s(IP:%d; Port:%d)\r\n\r\n", sLog.c_str(), ip, port);

	EnterCriticalSection(&g_LogQueueLock);
	while(!g_queueLog.empty())
	{
		LogStruct& s = g_queueLog.front();

		ostringstream os;
		os << s.sLog << endl;
		fprintf(fp, "%s(IP:%d; Port:%d)\r\n", os.str().c_str(), s.ip, s.port);	// [scq/2010-09-17]
		//char c[4000] = {0};
		unsigned char c[4000] = {0};	// [scq/2010-09-17]
		for (int i = 0; i < s.iLen; i++)
		{
			int iLen = sprintf((char *)(c + i * 2), "%02X", (unsigned int)s.pBuf[i]);
		}
		//os << c << endl << endl;
		//fprintf(fp, "%s\r\n", os.str().c_str());
		fprintf(fp, "%s\r\n", c);	// [scq/2010-09-17]

		g_queueLog.pop();
	}
	LeaveCriticalSection(&g_LogQueueLock);

	fclose(fp);
}

#endif

void ConvertNumberIpAddress(boost::uint32_t &in_ipNumberAddress,std::string &out_validIpAddress)
{
    if(in_ipNumberAddress<0||in_ipNumberAddress>4294967295)
   {
       out_validIpAddress = "0.0.0.0"; //invalid IP Address
    }
    else
   {
        boost::uint32_t divisor = 256;
        boost::uint32_t ip_section1 = 0;
        boost::uint32_t ip_section2 = 0;
        boost::uint32_t ip_section3 = 0;
        boost::uint32_t ip_section4 = 0;
        std::string ip_section_str1;
        std::string ip_section_str2;
        std::string ip_section_str3;
        std::string ip_section_str4;
        std::string dot = ".";

        ip_section1 = in_ipNumberAddress%divisor;
        in_ipNumberAddress = in_ipNumberAddress/divisor;
        ip_section2 = in_ipNumberAddress%divisor;
        in_ipNumberAddress = in_ipNumberAddress/divisor;
        ip_section3 = in_ipNumberAddress%divisor;
        in_ipNumberAddress = in_ipNumberAddress/divisor;
        ip_section4 = in_ipNumberAddress%divisor;
        ip_section_str1 = boost::lexical_cast <std::string> (ip_section1);
        ip_section_str2 = boost::lexical_cast <std::string> (ip_section2);
        ip_section_str3 = boost::lexical_cast <std::string> (ip_section3);
        ip_section_str4 = boost::lexical_cast <std::string> (ip_section4);
//        out_validIpAddress = ip_section_str4+dot+ip_section_str3+dot+ip_section_str2+dot+ip_section_str1;
		out_validIpAddress = ip_section_str1+dot+ip_section_str2+dot+ip_section_str3+dot+ip_section_str4;
    }
}

std::string ConvertPortToString(boost::uint16_t iPort, bool bIsHostPort)
{
	if (!bIsHostPort)
	{
		iPort = ntohs(iPort);
	}
	return boost::lexical_cast<std::string>(iPort);
}

void FixPhoneNo(char* src, char* dest) {
    char* p = NULL;

    if ((src == NULL) || (dest == NULL))
        return;

    strcpy(dest, src);

    size_t srclen = strlen(src);

    if ((srclen < 1) || (src[0] == '0') || (srclen <= 8) || (srclen > 12))
        return;

    if (srclen == 12)
        p = &src[1];
    else
        p = src;

    if ((strlen(p) == 11) &&
            (src[0] == '1') &&
            (src[1] != '0'))
        return;

    dest[0] = '0';

    strcpy(&dest[1], src);
}

std::string GetLocalTimeForMessageCall()
{
	time_t ltime;
	ltime = time(NULL);
	char str[32];
	strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", localtime(&ltime));
	return std::string(str);
}


static unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

std::string URLEncode(std::string para)
{
	std::string strTemp = "";
	size_t length = para.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)para[i]) ||
			(para[i] == '-') ||
			(para[i] == '_') ||
			(para[i] == '.') ||
			(para[i] == '~'))
			strTemp += para[i];
		else if (para[i] == ' ')
			strTemp += "%20";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)para[i] >> 4);
			strTemp += ToHex((unsigned char)para[i] % 16);
		}
	}
	return strTemp;
}

void base64url_encode(char* str)
{
	int len = strlen(str);
	int i, t;

	for (i = t = 0; i < len; i++) {
		switch (str[i]) {
		case '+':
			str[t++] = '-';
			break;
		case '/':
			str[t++] = '_';
			break;
		case '=':
			break;
		default:
			str[t++] = str[i];
		}
	}
	str[t] = '\0';
}

uint64_t UnixTimeStamp()
{
	ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
	time_duration time_from_epoch = second_clock::universal_time() - epoch;

	return time_from_epoch.total_seconds();
}