#ifndef IB_COMMON_MESSAGE_MAPPING_H
#define IB_COMMON_MESSAGE_MAPPING_H

#include "common/creditudp_transport.h"
#include "common/ipcc_message.h"
#include "common/transport_client.h"

namespace ib
{
    namespace common
    {

        //inline void _ipccmsg_handler_null(CCreditUDP&,
        //        SIPCCMessage&,
        //        unsigned long ip,
        //        unsigned short port)
        //{
        //};
        //
        //#define ipccmsg_handler_null bind(&_ipccmsg_handler_null, _1, _2, _3, _4)

        class ipccmsg_target;
        typedef bool (ipccmsg_target::*ipccmsg_function)(const transport_client_ptr &,
            int,
            int,
            const ipcc_msg_ptr &,
            uint32_t,
            uint16_t);

        struct ipccmsg_entry
        {
            int msg_class;
            int msg_type;
            ipccmsg_function handler;
        };

        struct ipccmsg_map
        {
            const ipccmsg_map* (*get_base_map)();
            const ipccmsg_entry* entries;
        };
    }
}

namespace ib_common = ib::common;

#define DECLARE_IPCCMSG_MAP()                                       \
protected:                                                          \
	static const ib::common::ipccmsg_map* get_this_ipccmsg_map();               \
	virtual const ib::common::ipccmsg_map* get_ipccmsg_map() const;             \

#define BEGIN_IPCCMSG_MAP(theClass, baseClass)                      \
	const ib::common::ipccmsg_map* theClass::get_ipccmsg_map() const            \
		{ return get_this_ipccmsg_map(); }                          \
	const ib::common::ipccmsg_map* theClass::get_this_ipccmsg_map()             \
	{                                                               \
		typedef theClass ThisClass;						            \
		typedef baseClass TheBaseClass;					            \
		static const ib::common::ipccmsg_entry _messageEntries[] =    \
		{                                                           \

#define ON_IPCCMSG(msg_class, msg_type, handler)                    \
            { msg_class, msg_type, static_cast<ipccmsg_function>(handler) },                \

#define END_IPCCMSG_MAP()                                           \
		    {-1, -1, 0}                                             \
	    };                                                          \
		static const ib::common::ipccmsg_map messageMap =                       \
		{ &TheBaseClass::get_this_ipccmsg_map, &_messageEntries[0] };  \
		return &messageMap;                                         \
	}								                                \


#endif
