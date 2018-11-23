#include <cstdlib>
#include <iostream>
using namespace std;

#include <boost/thread.hpp>

#include "Utility.h"

extern int parseCmdLine( int argc, char * * argv, const char * pcVersion );

const char * VERSION = "1.1.1.217";

/*
 * 
 */
int main(int argc, char** argv) 
{
    if (!parseCmdLine(argc, argv, VERSION))
	{
        return 0; //
	}
    
    CUtility::GetInstance()->Init();

    while(true)
	{
	    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}

    return 0;
}

