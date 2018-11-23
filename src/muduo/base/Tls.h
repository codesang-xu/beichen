#ifndef MUDUO_BASE_TLS_H
#define MUDUO_BASE_TLS_H

#include <boost/thread/tss.hpp> 

template <typename T>
T & tls_get(boost::thread_specific_ptr<T>& var_ptr)
{
	if (var_ptr.get() == NULL)
		var_ptr.reset(new T());
	return *var_ptr;
}

template <typename T>
void tls_set(boost::thread_specific_ptr<T>& var_ptr, T & val)
{
	if (var_ptr.get() == NULL)
		var_ptr.reset(new T());
	*var_ptr = val;
}

#endif
