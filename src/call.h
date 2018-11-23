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
	E_DT_User = 0,			//�û�
	E_DT_Agent = 1,		    //��ϯ
	E_DT_VoiceGateWay = 2,	//��������
	E_DT_KeyWord = 3,		//�ؼ���
	E_DT_Department = 4,	//ҵ����
	E_DT_WebPstnAgent = 5,
	E_DT_IVR = 6,			//IVR����
	E_DT_CClient = 7,
	E_DT_WebChatUser = 8,    //΢��
	E_DT_WebIM = 9,          //
	E_DT_Weibo = 10,          //΢��
	E_DT_Email = 11          //�ʼ�
};

string to_str(device_type_t device_type);

typedef struct device_t {
	//�豸id����ϯΪ��ϯid������Ϊ�����ߺ�
    device_id_t    device_id;

	//��Ӧ�����ߺţ���ĳһ����Ҫʹ��������Դʱ��ã�����ϯ��������
	//��������н���������
    device_id_t    line_no;
    device_type_t  device_type;
    unsigned long  ip;     //��������Զ��ip��ĳЩ������ʹ�ã��磺ת��
    unsigned short port;   //��������Զ�������˿�
    unsigned short msg_port; //��������Զ������˿ڣ���ĳЩ������ʹ�ã��磺ת��.
                             //����ֻ�Ѷ˿����������������ip��δ���֣���ʱ��Ϊ��ͬ������Ժ�����Ҫ�����ڷֿ���
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
    call_id_t   call_trans_id;//�����з��ص�һ��ֵ
	call_type_t call_type;
	device_id_t calling_device;
	device_id_t called_device;
	device_id_t alerting_device;
	device_id_t answer_device;
	device_type_t caller_type;
	device_type_t called_type;
	time_id_t   start_time;
	ptime      last_session_time;
	int32_t    established;      //�Ự�Ƿ���
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
            //��ֹ[]���������ֵ����������˿�ֵ���ܻᱻ�����൱��һ��ʱ�䣬
            //��Ȼ�ڴ�����Ҳ����Ҳ���Ǳ����������������ɡ�
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
        //��ֹ[]���������ֵ����������˿�ֵ���ܻᱻ�����൱��һ��ʱ�䣬
        //��Ȼ�ڴ�����Ҳ����Ҳ���Ǳ����������������ɡ�
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
