#include "common/creditudp_transport.h"

#include <stdlib.h>
#include <sstream>

#include "common/ipccmsg_helper.h"
#include "common/util.h"

namespace ib
{
namespace common
{

const int creditudp_transport::send_buf_size = 4096;
const int creditudp_transport::receive_buf_size = 4096;
const int creditudp_transport::CREDITUDP_BUFFER_SIZE = 800;

creditudp_transport::creditudp_transport(string local_port, string tname, bool use_credit)
	: credit_udp_ptr_(new CCreditUDP()),
	name_(tname),
	proxy_ip_(0),
	proxy_port_(0)
{
	logfilter_.disable_class(Class_OCXHeart);
	logfilter_.disable_class(Class_HeartBeat);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ServerHeartBeat);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ServerHeartBeatConf);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ClientHeartBeat);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ClientHeartBeatConf);
	logfilter_.disable(Class_ACD, Type_ACD_ForceLogout);

	make_log_func_ = bind(&creditudp_transport::default_make_log, this, _1);

	string error;
	if (!credit_udp_ptr_->Init(local_port.c_str(), send_buf_size, receive_buf_size,
		"creditudp_transport", false, error, use_credit))
	{
		throw error;
	}

	//DEBUG ("transport %s opend, port= %s ", name_.c_str(), local_port.c_str());
}

creditudp_transport::creditudp_transport(string local_port, string tname, make_log_function makelogfunc, bool use_credit)
	: credit_udp_ptr_(new CCreditUDP()),
	name_(tname),
	proxy_ip_(0),
	proxy_port_(0),
	make_log_func_(makelogfunc)
{
	logfilter_.disable_class(Class_OCXHeart);
	logfilter_.disable_class(Class_HeartBeat);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ServerHeartBeat);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ServerHeartBeatConf);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ClientHeartBeat);
	logfilter_.disable(Class_OneKeySwitch, Type_OneKeySwitch_ClientHeartBeatConf);
	logfilter_.disable(Class_ACD, Type_ACD_ForceLogout);

	string error;
	if (!credit_udp_ptr_->Init(local_port.c_str(), send_buf_size, receive_buf_size,
		"creditudp_transport", false, error, use_credit))
	{
		throw error;
	}

	//DEBUG ("transport %s opend, port= %s ", name_.c_str(), local_port.c_str());
}

creditudp_transport::~creditudp_transport(void)
{
    //DEBUG ("transport %s closed",name_.c_str());
}

CCreditUDP& creditudp_transport::get_socket()
{
    return *credit_udp_ptr_;
}

bool creditudp_transport::send_ipccmsg(SIPCCMessage& ipccmsg,
        unsigned long  dest_ip, 
        unsigned short dest_port,
		string file, int line,
		bool refuse_proxy)
{
	CIPCCMsgExchanger msgExchanger;	
	char* theBuf;
	int theBufContent = 0;
	msgExchanger.SerializeMsg(ipccmsg, theBuf, theBufContent);
	
	if (theBufContent < 0)
	{
		ostringstream oss;
		oss << "消息序列化失败： " << ipccmsg_to_str(ipccmsg)
			<< " ;" << get_file_name(file)
			<< ":" << line;
		make_log_func_(oss.str());
		return false;
	}

    send(theBuf, theBufContent, dest_ip, dest_port, refuse_proxy);
	
	if(theBuf != NULL)
		delete[] theBuf;

	if (logfilter_.filter(ipccmsg.msgClass, ipccmsg.msgType))
	{
		ostringstream oss;
		oss << "sent"
			<< ", type=" << ipccmsg_type_to_str(ipccmsg.msgClass, ipccmsg.msgType)
			<< ", to=" << format_ip_addr(dest_ip) << "@" << ntohs(dest_port)
			<< ", transport=" << name_
			<< ", class=" <<  ipccmsg_class_to_str(ipccmsg.msgClass);
		if (!file.empty())
		{
			oss << ", " << get_file_name(file) << ":" << line << endl;
		}
		else
		{
			oss << endl;
		}
		oss << ipccmsg_to_str(ipccmsg);
		make_log_func_(oss.str());
	}

	return true;
}

bool creditudp_transport::send_ipccmsg(shared_ptr<SIPCCMessage> ipccmsg,
	unsigned long  dest_ip,
	unsigned short dest_port,
	string file, int line,
	bool refuse_proxy)
{
	return send_ipccmsg(*ipccmsg, dest_ip, dest_port, file, line, refuse_proxy);
}

/*
 * @dest_ip network byte order
 * @dest_port network byte order
 */
bool creditudp_transport::send(char *data, int size,
      unsigned long  dest_ip, 
      unsigned short dest_port,
	  bool refuse_proxy)
{
	bool ret = true;
	string sErrInfo("");

    //dest_ip = ntohl(dest_ip);
    //dest_port = ntohs(dest_port);

	// 判断是否需要分包
	string errorInfo;
	int package_count = (size + CREDITUDP_BUFFER_SIZE) / CREDITUDP_BUFFER_SIZE;
	if (package_count)
	{
		if (refuse_proxy)
		{
			ret = credit_udp_ptr_->SendData(dest_ip, dest_port,
                data, size, sErrInfo) > 0;
		}
		else 
		{
			ret = credit_udp_ptr_->SendData(dest_ip, dest_port,
                data, size, sErrInfo, 10, false, 500, proxy_ip_, proxy_port_)>0;
		}
	}
	else
	{
		// 分包传输
		static unsigned int g_nPackID = 0;
		g_nPackID++;
		for (int i=0; i<package_count; i++)
		{
			SIPCCMsg_OCXService_SubTrans subTrans;
			subTrans.PackID = g_nPackID;
			// 当前包数
			subTrans.NowNum = i+1;
			// 总包数
			subTrans.TotNum = package_count;
			// 数据长度
			subTrans.DataLen = min(CREDITUDP_BUFFER_SIZE, size-i*CREDITUDP_BUFFER_SIZE);
			// 数据内容
			memmove(subTrans.DataBuf, data+i*CREDITUDP_BUFFER_SIZE, subTrans.DataLen);
			// 发送数据
			char* pDataBuf = NULL;
			int nSendLen = subTrans.FillBuf(pDataBuf);
			if (refuse_proxy)
			{
				ret = credit_udp_ptr_->SendData(dest_ip, dest_port,
                    pDataBuf, nSendLen, sErrInfo) > 0;
			}
			else
			{
				ret = credit_udp_ptr_->SendData(dest_ip, dest_port,
                    pDataBuf, nSendLen, sErrInfo, 10, false, 500, proxy_ip_, proxy_port_) > 0;
			}
			delete[] pDataBuf;	
		}
	}
	return ret;
}

int creditudp_transport::get_ipccmsg(char*& buffer, int buffer_size, int &msg_class, int &msg_type, unsigned long &ip, unsigned short &port, string& error, int defer_time)
{
    int ret = credit_udp_ptr_->ReceiveData(buffer,
                    buffer_size,
                    ip,
                    port,
                    error,
                    defer_time);

    CIPCCMsgExchanger msgExchanger;
    __EIPCCMsgClass class_;
    __EIPCCMsgType  type;
    if (ret > 0)
    {
        if (msgExchanger.GetMsgHeader(buffer, 
                ret, 
                class_, 
                type))
        {
            msg_class = class_;
            msg_type  = type;
        }
        else
        {
			ostringstream oss;
			oss << "recieve error msg, error" << error;
			make_log_func_(oss.str());

            ret = -1;
        }

		if (logfilter_.filter(msg_class, msg_type))
		{
			ostringstream oss;
			oss << "received"
				<< ", type=" << ipccmsg_type_to_str(msg_class, msg_type)
				<< ", from=" << format_ip_addr(ip) << "@" << ntohs(port)
				<< ", name=" << name_;
			make_log_func_(oss.str());
		}
    }

    return ret;
}

void creditudp_transport::default_make_log(string logmsg)
{
}

}
}

