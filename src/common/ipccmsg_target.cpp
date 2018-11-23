#include "common/ipccmsg_target.h"

#include "common/ipccmsg_helper.h"

namespace ib
{
namespace common
{

	const ipccmsg_map* ipccmsg_target::get_ipccmsg_map() const
	{
		return get_this_ipccmsg_map();
	}

	const ipccmsg_map* ipccmsg_target::get_this_ipccmsg_map()
	{
		static const ipccmsg_entry _messageEntries[] = { { -1, -1, 0 } };
		static const ipccmsg_map messageMap = { &ipccmsg_target::get_base_map, &_messageEntries[0] };
		return &messageMap;
	}

	bool ipccmsg_target::on_ipccmsg(const transport_client_ptr & credit_udp_ptr_,
		int ipccmsg_class,
		int ipccmsg_type,
		const ipcc_msg_ptr & ipccmsg_ptr,
		unsigned long src_ip,
		unsigned short src_port)
	{
		//FINEST("find msg handlor, class= %s, type=%s" , ipccmsg_class_to_str(ipccmsg_class).c_str(), ipccmsg_type_to_str(ipccmsg_class, ipccmsg_type).c_str());

		for (const ipccmsg_map *ipccmsg_map_ = get_ipccmsg_map(); ipccmsg_map_ != NULL; ipccmsg_map_ = ipccmsg_map_->get_base_map())
		{
			const ipccmsg_entry *const entry = ipccmsg_map_->entries;
			for (int i = 0; entry && entry[i].msg_class != -1; i++)
			{
				//FINEST( "match msg, class= %s, type=%s, callable=%d" , ipccmsg_class_to_str(entry[i].msg_class).c_str(), ipccmsg_type_to_str(entry[i].msg_class, entry[i].msg_type).c_str(),(ipccmsg_function)entry[i].handler);

				if (entry[i].msg_class == ipccmsg_class  &&  entry[i].msg_type == ipccmsg_type)
				{
					//FINEST ( "!!!!!!!!!!!!!!!!!!!!! matched  1");

					ipccmsg_function ipccmsg_handler = (ipccmsg_function)entry[i].handler;
					if (ipccmsg_handler && (this->*ipccmsg_handler)(credit_udp_ptr_,
						ipccmsg_class,
						ipccmsg_type,
						ipccmsg_ptr,
						src_ip,
						src_port))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	string ipccmsg_target::print_msg_map()
	{
		ostringstream oss;
		oss << "{\r\n";
		//FINEST ( "dump_msg_map");
		for (const ipccmsg_map *ipccmsg_map_ = get_ipccmsg_map(); ipccmsg_map_ != NULL; ipccmsg_map_ = ipccmsg_map_->get_base_map())
		{
			const ipccmsg_entry *const entry = ipccmsg_map_->entries;
			for (int i = 0; entry && entry[i].msg_class != -1; i++)
			{
				int ipccmsg_class = entry[i].msg_class;
				int ipccmsg_type = entry[i].msg_type;
				//FINEST ("all maped msg, class= %s, type= %s, callable= %d", ipccmsg_class_to_str(ipccmsg_class).c_str(), ipccmsg_type_to_str(ipccmsg_class, ipccmsg_type).c_str(), (ipccmsg_function)entry[i].handler);
			}
		}
		oss << "}";
		return oss.str();
	}

}
}

