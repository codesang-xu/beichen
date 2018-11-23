#include "call.h"

#include "common/util.h"

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

using namespace ib_common;

namespace ACDServer
{

string to_str(device_type_t device_type)
{
    switch (device_type)
    {
	case E_DT_UnKnown:
        return "E_DT_UnKnown";
	case E_DT_User:
        return "E_DT_User";
	case E_DT_Agent:
        return "E_DT_Agent";
	case E_DT_VoiceGateWay:
        return "E_DT_VoiceGateWay";
	case E_DT_KeyWord:
        return "E_DT_KeyWord";
	case E_DT_Department:
        return "E_DT_Department";
	case E_DT_WebPstnAgent:
        return "E_DT_WebPstnAgent";
	case E_DT_IVR:
        return "E_DT_IVR";
    default:
        return "unknown device type";
    }
}

string to_str(call_type_t call_type)
{
    switch (call_type)
    {
    case AGENT_CALL:
        return "AGENT_CALL";
    case PSTN_CALL:
        return "PSTN_CALL";
    case CONFERENCE_CALL:
        return "CONFERENCE_CALL";
    case WEB_PSTN_CALL:
        return "WEB_PSTN_CALL";
    default:
        return "unknown call type";
    }
}

string to_str(call_direction_t direction)
{
	switch (direction)
	{
	case INBOUND_CALL:
		return "OUTBOUND CALL";
	case OUTBOUND_CALL:
		return "OUTBOUND CALL";
	default:
		return "unknown call direction";
	}
}

string callex::to_str()
{
	ostringstream oss;
	oss << "call id        :" << call_.call_id << endl
		<< "call trans id  :" << call_.call_trans_id << endl
		<< "call_type      :" << ACDServer::to_str(call_.call_type) << endl
		<< "call direction :" << ACDServer::to_str(call_.call_direction) << endl
		<< "calling        :" << call_.calling_device << endl
		<< "called         :" << call_.called_device << endl
		<< "connections:" << endl;

	oss << std::left;
	for (auto it = call_.connections.begin(); it != call_.connections.end(); it++)
	{
		oss << "    conn:";
		oss.width(25);
		oss << it->device.device_id;
		oss.width(15);
		oss << ", " << it->device.line_no;
		oss.width(15);
		oss << ", " << ACDServer::to_str(it->device.device_type);
		oss.width(15);
		oss << ", " << format_ip_addr(it->device.ip);
		oss.width(15);
		oss << ", " << ntohs(it->device.port) << endl;
	}

	return oss.str();
}

}
