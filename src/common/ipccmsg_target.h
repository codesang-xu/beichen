#ifndef IB_COMMON_IPCCMSG_TARGET_H
#define IB_COMMON_IPCCMSG_TARGET_H

#include "common/callbacks.h"
#include "common/ipccmsg_mapping.h"

namespace ib
{
namespace common
{

	class ipccmsg_target/* : noncopyable*/
	{
	public:
		virtual bool on_ipccmsg(const transport_client_ptr & credit_udp_ptr_,
			int ipccmsg_class,
			int ipccmsg_type,
			const ipcc_msg_ptr & ipccmsg_ptr,
			unsigned long src_ip,
			unsigned short src_port);
	protected:
		string print_msg_map();

	private:
    static const ipccmsg_map* get_base_map()
    { 
        return NULL; 
    }

DECLARE_IPCCMSG_MAP()
};

}
}

namespace ib_common = ib::common;


#endif
