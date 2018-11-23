#include "common/util.h"

#ifdef _WIN32
#define localtime_r( arg_tm, arg_time_t )  localtime_s( (arg_time_t), (arg_tm) )
#else
	#include <net/if.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <boost/asio.hpp>

#ifndef NOPUBLIC
#include "public/base64.h"
#else
#include "./base64.h"
#endif

namespace ib
{
namespace common
{

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

vector<string> split(const string &str, const string &separator)
{
    vector<string> tokens;
    string::size_type index_b = 0;
    string::size_type index_e = str.find(separator);
    while(index_e != string::npos)
    {
		tokens.push_back(str.substr(index_b, index_e - index_b));
        index_b = index_e+separator.size();
        index_e = str.find(separator, index_b);
    }

    if (index_b < str.size())
    {
        tokens.push_back(str.substr(index_b, str.size()-index_b));
    }
    else
    {
        tokens.push_back("");
    }
    return tokens;
}

bool IsNumeric(string str)
{
	char szTmp[MAX_PATH] = "";
	const char *pTmp = str.c_str();
	if (!*pTmp)
		return false;

	*pTmp == '-' ? pTmp++ : pTmp;
	sscanf(pTmp, " %[.,0-9]", szTmp);
	if (strcmp(pTmp, szTmp) || !strcmp(pTmp, "."))
		return false;

	if (pTmp = strstr(szTmp, "."))
		if (pTmp = strstr(pTmp + 1, "."))
			return false;

	return true;
}

char skey[] = "jklsgfjklmzvm,>a3fjkl@sf4iwioer2.;349723fsdff";
int encrypt_string(const char *string, int buflen, char*& encstring)
{
	int i=0;
	int j=strlen(skey);

	for(;i<buflen;i++) 
	{
		int k = i/j;
		if(i>=j)
			encstring[i]= (string[i] ^ skey[i-j*k]) + i % j;
		else
			encstring[i]= (string[i] ^ skey[i]) + i;
	} 
	encstring[buflen] = '\0'; 
	return buflen;
}

int decrypt_string(const char *string, int length, char*& decstring)
{
	int j=strlen(skey);

	for(int i=0; i<length ; i++)
	{
		int k = i/j;
		if(i>=j)
			decstring[i]= (string[i]-i % j) ^ skey[i-j*k];
		else
			decstring[i]= (string[i] -i)^ skey[i];
	}
	decstring[length] = '\0';
	return length;
}

string EncryptPswd(string& sPassword)
{
	int nLen1 = sPassword.length();
	char* pData = new char[nLen1 + 1];
	memset(pData, 0, nLen1 + 1);
	int nLen2 = encrypt_string(sPassword.c_str(), nLen1, (char*&)pData);
	int nLen3 = (nLen2 + nLen2/2) + 5;
	char* szEncode = new char[nLen3];
	Base64Encode(szEncode, pData, nLen2);
	string sResult = szEncode;
	delete szEncode;
	delete pData;

	return szEncode;
}

string now()
{
    time_t rawtime;
    tm     tm_time;

    time (&rawtime);

    localtime_r (&rawtime, &tm_time);

    char buf[34] = {0};

#ifdef _WIN32
    _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%4d-%02d-%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#else
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#endif

    return buf;
}

string get_date()
{
    time_t rawtime;
    tm     tm_time;

    time (&rawtime);
    localtime_r (&rawtime, &tm_time);

    char buf[34] = {0};

#ifdef _WIN32
    _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%4d-%02d-%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday);
#else
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday);
#endif

    return buf;
}

string get_time()
{
    time_t rawtime;
    tm     tm_time;

    time (&rawtime);
    localtime_r (&rawtime, &tm_time);

    char buf[34] = {0};

#ifdef _WIN32
    _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%02d%02d%02d",
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#else
    snprintf(buf, sizeof(buf), "%02d%02d%02d",
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#endif

    return buf;
}

//uint32_t timestamp()
//{
//    return timeGetTime();
//}

#ifdef __GNUC__
int GetModuleFileName( char* sModuleName, char* sFileName, int nSize)
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
				 size_t len;

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
#endif

string get_app_root()
{
    char szExePath[MAX_PATH + 1];
    ::memset(szExePath, 0, sizeof szExePath);

    GetModuleFileName(NULL, szExePath, MAX_PATH);

    int i = strlen(szExePath) - 1;
    while (i >= 0 && (szExePath[i] != '\\') && (szExePath[i] != '/'))
        i--;
    assert(i >= 0);
    szExePath[i] = '\0';

    return szExePath;
}

wstring to_wstr(const string &str)
{
    size_t wstr_size = mbstowcs(0, str.c_str(), 0);  
    if (wstr_size > 0)
    {
        wstring ret;
        ret.resize(wstr_size);
        mbstowcs(&ret[0], str.c_str(), str.size());
        return ret;
    }
    else
    {
        return L"";
    }
}

string to_str(const wstring &wstr)
{
    size_t str_size = wcstombs(0, wstr.c_str(), 0);  
    if (str_size > 0)
    {
        string ret;
        ret.resize(str_size);
		wcstombs(&ret[0], wstr.c_str(), ret.size());
        return ret;
    }
    else
    {
        return "";
    }
}

string format_ip_addr( unsigned long ip )
{
    return boost::asio::ip::address_v4(ntohl(ip)).to_string();
}

string get_file_name(string path)
{
	if (!path.empty())
	{
		int i = path.length() - 1;
		while (i >= 0 && path[i] != '\\' && path[i] != '/')
		{
			i--;
		}
		if (i >= 0)
		{
			i++;
			return &path[i];
		}
	}

	return "";
}

const char* get_file_name(const char * path)
{
    if (path == NULL) {
        return "";
    }

    int i = strlen(path) - 1;
    while (i >= 0 && path[i] != '\\' && path[i] != '/') {
        i--;
    }

    if (i >= 0) {
        i++;
        return &path[i];
    }

    return "";
}

void to_upper(string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void to_lower(string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

}

bool equal_ignore_case(const string & str1, const string & str2)
{
    if (str1.length() != str2.length()) {
        return false;
    }
    else {
        for (size_t i = 0; i < str1.length(); i++) {
            if (std::toupper(str1.at(i)) != std::toupper(str2.at(i))) {
                return false;
            }
        }
    }

    return true;
}

bool is_mobile_phone_num(string &phone_number)
{
    return
        (phone_number.length() == 11 && phone_number[0] == '1') ||
        (phone_number.length() == 12 && phone_number.substr(0, 2) == "01");
}

bool is_telephone_num(string &phone_number)
{
    return
        (phone_number.length() >= 7) && 
        (
            (phone_number[0] > '1')/*不带区号*/ || 
            (
                (phone_number[0] == '0')/*带区号*/ && 
                (
                    (phone_number[1] > '1')/*不是带0手机号*/ || 
                    (
                        (phone_number[1] == '1') && 
                        (phone_number[2] == '0')
                    )/*010*/
                )
            )
        );
}


bool is_IDD(string &phone_number)//国际长途直拨号码
{
    return (phone_number.length() > 2 && phone_number[0] == '0' && phone_number[1] == '0');
}

pair<E_PHONE_NUMBER_TYPE, bool> check_phone_num_type(string &phone_number)
{
    if (is_mobile_phone_num(phone_number)) {
        return make_pair(PHONE_NUMBER_MOBILE, (phone_number[0] == '0'));
    }
    else if (is_telephone_num(phone_number)) {
        return make_pair(PHONE_NUMBER_TELEPHONE, (phone_number[0] == '0'));
    }
    else {
        string str = phone_number.substr(0, 3);
        if (str == "400") {
            return make_pair(PHONE_NUMBER_400, true);
        }
        else if (str == "800") {
            return make_pair(PHONE_NUMBER_800, true);
        }
        else {
            //str.pop_back();
			str = str.substr(0, str.length() - 1);
            if (str.find("00") == 0) {
                return make_pair(PHONE_NUMBER_IDD, true);
            }
        }
    }

    return make_pair(PHONE_NUMBER_UNKNOWN, true);
}

}
}