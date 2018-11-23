#ifndef _COMMON_MEMORY_H_
#define _COMMON_MEMORY_H_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pointer_cast.hpp>

namespace ib
{
    using boost::shared_ptr;
    using boost::weak_ptr;
    using boost::scoped_ptr;
    using boost::enable_shared_from_this;
    using boost::static_pointer_cast;
    using boost::make_shared;

    typedef boost::scoped_array<unsigned char> unique_ptr_uchar_array;
}


#endif