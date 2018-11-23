#ifndef _IPCCSERVICE_MSGHELPER_H_
#define _IPCCSERVICE_MSGHELPER_H_

#include <string>

#include "common/ipcc_message.h"

namespace ib
{
namespace common
{
	struct id_name
	{
		int    _id;
		string _name;

		string to_string(bool readable = true)
		{
			stringstream ss;
			ss << _name;
			if (readable)
				ss << "(" << _id << ")";
			return ss.str();
		}
	};
	string id_to_name(id_name id_names[], int size, int id, bool readable = true);

string ipccmsg_class_to_str( int clazz );
string ipccmsg_type_to_str( int i );
string ipccmsg_type_to_str( int clazz, int type );
string ipccmsg_type_ocx_to_str( int i );
string agent_state_to_str( int i );
string agent_calling_state_to_str( int i );
string user_calling_state_to_str( int i );
string login_type_to_str( int i );
string device_type_to_str( int i );
string call_party_type_to_str(int i);
string ESeverType_to_str(int i);
string ECallType_to_str(int i);
string AppOSType_to_str(int i);
string outdial_type_to_str(int t);
string CallTargetType_to_str(int i);

string to_str(__EIPCCMsgClass c);
string to_str(__EIPCCMsgType t);
string to_str(__AgentState s);
string to_str(__AgentCallingState s);
string to_str(__CallPartyType t);
string to_str(__UserCallingState s);
string to_str(ESeverType t);
string to_str(LoginType t);
string to_str(ECallType t);
string to_str(AppOSType t);

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

}
}

namespace ib_common = ib::common;

#endif

#include "common/ipccmsg_dump.h"
