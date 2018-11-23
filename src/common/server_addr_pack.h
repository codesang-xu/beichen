#ifndef IPCC_SERVICE_SERVER_H
#define IPCC_SERVICE_SERVER_H
#include <list>
#include <algorithm>
#include "endpoint.h"
#include "Resolve.hpp"

namespace ib
{
namespace common
{
	class server_addr_pack
	{
	public:

	public:
		server_addr_pack() {};

		server_addr_pack(string domain)
		{
			resolve(domain);
		};

		server_addr_pack(unsigned int ip, unsigned short port)
		{
			addrs_.push_back(endpoint(ip, port));
		};

		server_addr_pack(vector<endpoint> & addrs)
		{
			addrs_.resize(addrs.size());
			copy(addrs.begin(), addrs.end(), addrs_.begin());
		};

		server_addr_pack(server_addr_pack & addrs)
		{
			addrs_ = addrs.addrs_;
		};

		void resolve(string domain)
		{
			vector<endpoint> resolved_addrs_;
			net::ip::Resolver().resolve(domain, resolved_addrs_);
			addrs_.resize(resolved_addrs_.size());
			copy(resolved_addrs_.begin(), resolved_addrs_.end(), addrs_.begin());
		};

		endpoint get_addr()
		{
			if (addrs_.size() > 0)
			{
				return addrs_.front();
			}
			else
			{
				return endpoint();
			}
		};

		unsigned ip()
		{
			if (addrs_.size() > 0)
			{
				return addrs_.front().ip;
			}
			else
			{
				return 0;
			}
		};

		unsigned port()
		{
			if (addrs_.size() > 0)
			{
				return addrs_.front().port;
			}
			else
			{
				return 0;
			}
		};

		endpoint next()
		{
			if (!empty())
			{
				addrs_.push_back(addrs_.front());
				addrs_.pop_front();
			}

			return get_addr();
		};

		bool empty()
		{
			return addrs_.empty();
		}

		void add(unsigned int ip, unsigned short port)
		{
			endpoint ep(ip, port);
			if (find(addrs_.begin(), addrs_.end(), ep) == addrs_.end())
				addrs_.push_back(ep);
		}

		void erase_current()
		{
			if (!empty())
				addrs_.pop_front();
		}

		void erase(unsigned int ip, unsigned short port)
		{
			endpoint ep(ip, port);
			addrs_.erase(find(addrs_.begin(), addrs_.end(), ep));
		}

		size_t size()
		{
			return addrs_.size();
		}

	public:
		~server_addr_pack(void) {};


	private:
		std::list<endpoint> addrs_;
	};


}
}

namespace ib_common = ib::common;


#endif
