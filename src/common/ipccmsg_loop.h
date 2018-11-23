#pragma once

#include "common/blocking_queue.h"
#include "common/callbacks.h"
#include "common/transport_client.h"

namespace ib
{
namespace common
{

class ipccmsg_loop /*: noncopyable*/
{
public:
    typedef function<void()> functor;

    ipccmsg_loop();

    ~ipccmsg_loop();

    void loop();
    void stop();

    /* receive ipccmsg from socket */
	void receive_ipccmsg_callback(const transport_client_ptr & credit_udp_ptr_,
            int ipccmsg_class,
            int ipccmsg_type,
			char *msg_buf,
            int buf_len,
            unsigned long src_ip,
            unsigned short src_port);

    void set_ipccmsg_callback(message_callback on_msg_cb)
    {
		mutex::scoped_lock lock(mutex_);
        message_callback_ = on_msg_cb;
    }

    void run_in_loop(const functor& cb);

    bool is_in_loop_thread() const { return my_thread_id_ == this_thread::get_id(); }

    void assert_in_loop_thread()
    {
        if (!is_in_loop_thread())
        {
            abort_not_in_loop_thread();
        }
    }

	// + [11/8/2016 lixg]
	ib_common::blocking_queue<ib_common::ipccmsg_loop::functor>& Funcs() { return funcs_; }
private:
    void queue_in_loop(const functor& cb);
    void abort_not_in_loop_thread();

private:
    blocking_queue<functor>  funcs_;
	
	mutex mutex_;
    message_callback message_callback_;
    thread::id my_thread_id_;
    bool looping_;

private:
    class ipccmsg_callback_wrapper
    {
        public:
            ipccmsg_callback_wrapper( char *arg_msg_buf, int arg_buf_len)
                : ipccmsg_buf(arg_msg_buf, bind(ipccmsg_callback_wrapper::external_buf_deletor, _1)),//warning: the buffer allocated by creditudp witch new[].
                  buf_len(arg_buf_len)
            { }

            void operator()(message_callback cb,
                    transport_client_ptr arg_cudp_ptr,
                    int arg_ipccmsg_class,
                    int arg_ipccmsg_type,
                    unsigned long arg_src_ip,
                    unsigned short arg_src_port)
            {
                if (cb)
                {
                    cb(arg_cudp_ptr,
                       arg_ipccmsg_class,
                       arg_ipccmsg_type,
                       ipccmsg_buf.get(),
                       buf_len,
                       arg_src_ip,
                       arg_src_port);
                }
            }
        private:
            static void external_buf_deletor(char* p){ delete[] p;};

        private:
            shared_ptr<char>  ipccmsg_buf;
            int               buf_len;
    };
};

}
}

namespace ib_common = ib::common;

