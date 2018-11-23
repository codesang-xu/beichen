#ifndef CALL_H
#define CALL_H

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
//using namespace boost::gregorian;

namespace ACDServer
{

typedef std::string device_id_t;

enum device_type_t {
	E_DT_UnKnown=-1,
	E_DT_User = 0,			//用户
	E_DT_Agent = 1,		    //座席
	E_DT_VoiceGateWay = 2,	//语音网关
	E_DT_KeyWord = 3,		//关键字
	E_DT_Department = 4,	//业务组
	E_DT_WebPstnAgent = 5,
	E_DT_IVR = 6,			//IVR流程
	E_DT_CClient = 7,
	E_DT_WebChatUser = 8,    //微信
	E_DT_WebIM = 9,          //
	E_DT_Weibo = 10,          //微博
	E_DT_Email = 11          //邮件
};

string to_str(device_type_t device_type);

typedef struct device_t {
	//设备id，坐席为坐席id，外线为网关线号
    device_id_t    device_id;

	//对应网关线号，当某一方需要使用网关资源时获得，如坐席加入会议后。
	//外线随呼叫建立而产生
    device_id_t    line_no;
    device_type_t  device_type;
    unsigned long  ip;     //用来保存远端ip在某些信令中使用，如：转移
    unsigned short port;   //用来保存远端语音端口
    unsigned short msg_port; //用来保存远端信令端口，在某些信令中使用，如：转移.
                             //这里只把端口区分信令和语音，ip并未区分，暂时认为相同，如果以后发现需要区分在分开。
} device_t;

typedef device_t calling_device_t;
typedef device_t called_device_t;
typedef device_t subject_device_t;

struct connection_id_t {
    string      call_id;
    device_id_t device_id;
};

enum local_connection_state_t {
    CS_NONE = -1,
    CS_NULL = 0,
    CS_INITIATE = 1,
    CS_ALERTING = 2,
    CS_CONNECT = 3,
    CS_HOLD = 4,
    CS_QUEUED = 5,
    CS_FAIL = 6
};

struct connection_t {
    connection_t()
        : local_connection_state(CS_NULL)
    { }

    connection_id_t  party;
    subject_device_t device;
    local_connection_state_t local_connection_state;
    map<string, boost::any> additional;
};

typedef string  call_id_t;
typedef string  time_id_t;

enum call_type_t {
    AGENT_CALL = 0,
    PSTN_CALL = 1,
	CONFERENCE_CALL = 2,
	WEB_PSTN_CALL = 3,
	MESSAGE_CALL = 4,
	WEBCHAT_CALL = 5,
	EMAIL_CALL = 6
};

string to_str(call_type_t call_type);

enum call_direction_t {
	INBOUND_CALL,
	OUTBOUND_CALL,
};

string to_str(call_direction_t direction);

struct call_t{
    call_id_t   call_id;
    call_id_t   call_trans_id;//信令中返回的一个值
	call_type_t call_type;
	device_id_t calling_device;
	device_id_t called_device;
	device_id_t alerting_device;
	device_id_t answer_device;
	device_type_t caller_type;
	device_type_t called_type;
	time_id_t   start_time;
	ptime      last_session_time;
	int32_t    established;      //会话是否建立
	std::vector<connection_t> connections;
	call_direction_t call_direction;
};

class callex
{
private:
    struct match_conn_with_dev_id;

public:
    typedef std::vector<connection_t>::iterator connection_it;

public:
	callex() 
    {
		call_.established = 0;
	}

	~callex()
    {
    }

    connection_t* get_connection(const device_id_t &dev_id)
    {
        connection_it it = find_if(call_.connections.begin(), call_.connections.end(), 
                                match_conn_with_dev_id(dev_id));
        if (it==call_.connections.end())
        {
            return NULL;
        }
        else
        {
            return &*it;
        }
    }

    void remove_connection(const device_id_t &dev_id)
    {
		std::remove_if(call_.connections.begin(), call_.connections.end(), 
            match_conn_with_dev_id(dev_id));
    }

    device_t* get_device(device_id_t dev_id)
    {
        connection_t *conn = get_connection(dev_id);
        if (conn!=NULL)
        {
            return &conn->device;
        }
        else
        {
            return NULL;
        }
    }

    template<class T>
    T get_conn_var(const device_id_t &dev_id, const string &key)
    {
        if (connection_t *conn = get_connection(dev_id))
        {
            map<string, boost::any>::iterator it;
            //防止[]操作插入空值，如果插入了空值可能会被保持相当长一段时间，
            //虽然内存消耗也许不大，也还是避免出现这样的情况吧。
            it = conn->additional.find(key);
            if (it != conn->additional.end() && !it->second.empty())
            {
                return any_cast<T>(it->second);
            }
        }

        return T();
    }

	bool set_conn_var(const device_id_t &dev_id, const string &key, boost::any &value)
    {
        connection_t *conn = get_connection(dev_id);
        if (conn!=NULL)
        {
            return conn->additional.insert(std::make_pair(key, value)).second;
        }
        else
        {
            return false;
        }
    }

    template<class T>
    T get_var(const string &key)
    {
        map<string, boost::any>::iterator it = additional.find(key);
        //防止[]操作插入空值，如果插入了空值可能会被保持相当长一段时间，
        //虽然内存消耗也许不大，也还是避免出现这样的情况吧。
        if (it != additional.end() && !it->second.empty())
        {
            return any_cast<T>(it->second);
        }
        else
        {
            return T();
        }
    }

	bool set_var(const string &key, boost::any value)
    {
        additional[key] = value;
		return true;
    }

    void remove_var(const string &key)
    {
        additional.erase(key);
    }

	bool is_inbound_call() 
	{
		return (call_.call_direction == INBOUND_CALL);
	}

	int get_call_type()
	{
		/*
		if (call_.call_type == AGENT_CALL)
		{
			return 3;
		}
		else
		{
			if (is_inbound_call())
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		*/
 
		if (call_.call_type == MESSAGE_CALL)
		{
			return 1;
		}
		else if (call_.call_type == WEBCHAT_CALL)
		{
			return 2;
		}
		else if (call_.call_type == EMAIL_CALL)
		{
			return 3;
		}

		return 0;

	}

	void update_sesstion_time(int32_t noMsgTime)
	{
		ptime now(second_clock::local_time());
		time_duration t(0, 0, noMsgTime);
		call_.last_session_time = now - t;
	}

	string to_str();

public:
    call_t call_;

private:
    map<string, boost::any> additional;

private:
    struct match_conn_with_dev_id
    {
        match_conn_with_dev_id(device_id_t dev_id)
            : dev_id_(dev_id)
        { }

        bool operator () (const connection_t &conn) const
        {
			return (conn.party.device_id == dev_id_) || (conn.device.line_no == dev_id_);
        }
        string dev_id_;
    };
};

typedef boost::shared_ptr<callex> call_ptr;

}
#endif
