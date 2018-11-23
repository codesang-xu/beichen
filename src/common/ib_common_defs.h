#ifndef IB_COMMON_DEFS_H
#define IB_COMMON_DEFS_H

#include "common/memory.h"

struct SIPCCMessage;
namespace ib_common
{

#define BUFSIZ_INFOBIRD 4096

class creditudp_transport;
typedef shared_ptr<creditudp_transport> creditudp_transport_ptr;

class transport_client;
typedef shared_ptr<transport_client> transport_client_ptr;

class transaction;


}

#endif