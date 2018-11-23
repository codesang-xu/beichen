#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Linux-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AgentInfo.o \
	${OBJECTDIR}/AgentMgr.o \
	${OBJECTDIR}/CClientInfo.o \
	${OBJECTDIR}/CClientMgr.o \
	${OBJECTDIR}/CallInfo.o \
	${OBJECTDIR}/CallMgr.o \
	${OBJECTDIR}/CorporationInfo.o \
	${OBJECTDIR}/CorporationMgr.o \
	${OBJECTDIR}/DepartmentInfo.o \
	${OBJECTDIR}/DepartmentMgr.o \
	${OBJECTDIR}/IpccMsgLogTrans.o \
	${OBJECTDIR}/Log.o \
	${OBJECTDIR}/RedirectServer.o \
	${OBJECTDIR}/RedisClient.o \
	${OBJECTDIR}/RedisConnectionPool.o \
	${OBJECTDIR}/RedisServer_ACD.o \
	${OBJECTDIR}/RegisterServer.o \
	${OBJECTDIR}/SplitString.o \
	${OBJECTDIR}/SynRedisClient.o \
	${OBJECTDIR}/UserInfo.o \
	${OBJECTDIR}/UserMgr.o \
	${OBJECTDIR}/Utility.o \
	${OBJECTDIR}/call.o \
	${OBJECTDIR}/common/count_down_latch.o \
	${OBJECTDIR}/common/creditudp_poller.o \
	${OBJECTDIR}/common/creditudp_transport.o \
	${OBJECTDIR}/common/ipcc_message.o \
	${OBJECTDIR}/common/ipccmsg_dump.o \
	${OBJECTDIR}/common/ipccmsg_helper.o \
	${OBJECTDIR}/common/ipccmsg_loop.o \
	${OBJECTDIR}/common/ipccmsg_target.o \
	${OBJECTDIR}/common/log_init.o \
	${OBJECTDIR}/common/timer.o \
	${OBJECTDIR}/common/timer_queue.o \
	${OBJECTDIR}/common/timer_service.o \
	${OBJECTDIR}/common/transport_client.o \
	${OBJECTDIR}/common/util.o \
	${OBJECTDIR}/httpclient/asynchttpclient.o \
	${OBJECTDIR}/httpclient/httpclient.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/msgpush/MessagePushMgr.o \
	${OBJECTDIR}/msgpush/jsoncpp/json_reader.o \
	${OBJECTDIR}/msgpush/jsoncpp/json_value.o \
	${OBJECTDIR}/msgpush/jsoncpp/json_writer.o \
	${OBJECTDIR}/msgpush/jwt.o \
	${OBJECTDIR}/msgpush/md5.o \
	${OBJECTDIR}/msgpush/pmessage.o \
	${OBJECTDIR}/msgpush/xg_url.o \
	${OBJECTDIR}/muduo/base/AsyncLogging.o \
	${OBJECTDIR}/muduo/base/Condition.o \
	${OBJECTDIR}/muduo/base/CountDownLatch.o \
	${OBJECTDIR}/muduo/base/Exception.o \
	${OBJECTDIR}/muduo/base/LogFile.o \
	${OBJECTDIR}/muduo/base/LogStream.o \
	${OBJECTDIR}/muduo/base/Logging.o \
	${OBJECTDIR}/muduo/base/Thread.o \
	${OBJECTDIR}/muduo/base/Timestamp.o \
	${OBJECTDIR}/public/CreditUDP.o \
	${OBJECTDIR}/public/DES.o \
	${OBJECTDIR}/public/GlobalOperation.o \
	${OBJECTDIR}/public/IPCCMessage.o \
	${OBJECTDIR}/public/MemLog.o \
	${OBJECTDIR}/public/Rijndael.o \
	${OBJECTDIR}/public/ServerInfoDef.o \
	${OBJECTDIR}/public/Spliter.o \
	${OBJECTDIR}/public/StdAfx.o \
	${OBJECTDIR}/public/UDPSocket.o \
	${OBJECTDIR}/public/ini.o \
	${OBJECTDIR}/public/xmlParser.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Wl,-rpath,'/usr/local/curl_nossl/lib'

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/xuyc/netbean_bin/wipcc_acdserver

/home/xuyc/netbean_bin/wipcc_acdserver: ${OBJECTFILES}
	${MKDIR} -p /home/xuyc/netbean_bin
	${LINK.cc} -o /home/xuyc/netbean_bin/wipcc_acdserver ${OBJECTFILES} ${LDLIBSOPTIONS} -static -lc -static-libgcc -static-libstdc++ -lpthread /usr/local/curl_nghttp2/lib/libcurl.a /usr/local/nghttp2/lib/libnghttp2.a /usr/local/lib/libhiredis.a /usr/local/ssl/lib/libssl.a /usr/local/ssl/lib/libcrypto.a /usr/local/lib/libboost_system.a /usr/local/lib/libboost_filesystem.a /usr/local/lib/libboost_date_time.a /usr/local/lib/libboost_chrono.a /usr/local/lib/libboost_thread.a /usr/local/lib/libboost_locale.a -Wl,-ldl

${OBJECTDIR}/AgentInfo.o: AgentInfo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AgentInfo.o AgentInfo.cpp

${OBJECTDIR}/AgentMgr.o: AgentMgr.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AgentMgr.o AgentMgr.cpp

${OBJECTDIR}/CClientInfo.o: CClientInfo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CClientInfo.o CClientInfo.cpp

${OBJECTDIR}/CClientMgr.o: CClientMgr.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CClientMgr.o CClientMgr.cpp

${OBJECTDIR}/CallInfo.o: CallInfo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CallInfo.o CallInfo.cpp

${OBJECTDIR}/CallMgr.o: CallMgr.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CallMgr.o CallMgr.cpp

${OBJECTDIR}/CorporationInfo.o: CorporationInfo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CorporationInfo.o CorporationInfo.cpp

${OBJECTDIR}/CorporationMgr.o: CorporationMgr.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CorporationMgr.o CorporationMgr.cpp

${OBJECTDIR}/DepartmentInfo.o: DepartmentInfo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DepartmentInfo.o DepartmentInfo.cpp

${OBJECTDIR}/DepartmentMgr.o: DepartmentMgr.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DepartmentMgr.o DepartmentMgr.cpp

${OBJECTDIR}/IpccMsgLogTrans.o: IpccMsgLogTrans.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IpccMsgLogTrans.o IpccMsgLogTrans.cpp

${OBJECTDIR}/Log.o: Log.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Log.o Log.cpp

${OBJECTDIR}/RedirectServer.o: RedirectServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RedirectServer.o RedirectServer.cpp

${OBJECTDIR}/RedisClient.o: RedisClient.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RedisClient.o RedisClient.cpp

${OBJECTDIR}/RedisConnectionPool.o: RedisConnectionPool.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RedisConnectionPool.o RedisConnectionPool.cpp

${OBJECTDIR}/RedisServer_ACD.o: RedisServer_ACD.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RedisServer_ACD.o RedisServer_ACD.cpp

${OBJECTDIR}/RegisterServer.o: RegisterServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RegisterServer.o RegisterServer.cpp

${OBJECTDIR}/SplitString.o: SplitString.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SplitString.o SplitString.cpp

${OBJECTDIR}/SynRedisClient.o: SynRedisClient.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SynRedisClient.o SynRedisClient.cpp

${OBJECTDIR}/UserInfo.o: UserInfo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UserInfo.o UserInfo.cpp

${OBJECTDIR}/UserMgr.o: UserMgr.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UserMgr.o UserMgr.cpp

${OBJECTDIR}/Utility.o: Utility.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utility.o Utility.cpp

${OBJECTDIR}/call.o: call.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/call.o call.cpp

${OBJECTDIR}/common/count_down_latch.o: common/count_down_latch.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/count_down_latch.o common/count_down_latch.cpp

${OBJECTDIR}/common/creditudp_poller.o: common/creditudp_poller.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/creditudp_poller.o common/creditudp_poller.cpp

${OBJECTDIR}/common/creditudp_transport.o: common/creditudp_transport.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/creditudp_transport.o common/creditudp_transport.cpp

${OBJECTDIR}/common/ipcc_message.o: common/ipcc_message.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/ipcc_message.o common/ipcc_message.cpp

${OBJECTDIR}/common/ipccmsg_dump.o: common/ipccmsg_dump.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/ipccmsg_dump.o common/ipccmsg_dump.cpp

${OBJECTDIR}/common/ipccmsg_helper.o: common/ipccmsg_helper.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/ipccmsg_helper.o common/ipccmsg_helper.cpp

${OBJECTDIR}/common/ipccmsg_loop.o: common/ipccmsg_loop.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/ipccmsg_loop.o common/ipccmsg_loop.cpp

${OBJECTDIR}/common/ipccmsg_target.o: common/ipccmsg_target.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/ipccmsg_target.o common/ipccmsg_target.cpp

${OBJECTDIR}/common/log_init.o: common/log_init.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/log_init.o common/log_init.cpp

${OBJECTDIR}/common/timer.o: common/timer.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/timer.o common/timer.cpp

${OBJECTDIR}/common/timer_queue.o: common/timer_queue.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/timer_queue.o common/timer_queue.cpp

${OBJECTDIR}/common/timer_service.o: common/timer_service.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/timer_service.o common/timer_service.cpp

${OBJECTDIR}/common/transport_client.o: common/transport_client.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/transport_client.o common/transport_client.cpp

${OBJECTDIR}/common/util.o: common/util.cpp
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/util.o common/util.cpp

${OBJECTDIR}/httpclient/asynchttpclient.o: httpclient/asynchttpclient.cpp
	${MKDIR} -p ${OBJECTDIR}/httpclient
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/httpclient/asynchttpclient.o httpclient/asynchttpclient.cpp

${OBJECTDIR}/httpclient/httpclient.o: httpclient/httpclient.cpp
	${MKDIR} -p ${OBJECTDIR}/httpclient
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/httpclient/httpclient.o httpclient/httpclient.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/msgpush/MessagePushMgr.o: msgpush/MessagePushMgr.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/MessagePushMgr.o msgpush/MessagePushMgr.cpp

${OBJECTDIR}/msgpush/jsoncpp/json_reader.o: msgpush/jsoncpp/json_reader.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush/jsoncpp
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/jsoncpp/json_reader.o msgpush/jsoncpp/json_reader.cpp

${OBJECTDIR}/msgpush/jsoncpp/json_value.o: msgpush/jsoncpp/json_value.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush/jsoncpp
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/jsoncpp/json_value.o msgpush/jsoncpp/json_value.cpp

${OBJECTDIR}/msgpush/jsoncpp/json_writer.o: msgpush/jsoncpp/json_writer.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush/jsoncpp
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/jsoncpp/json_writer.o msgpush/jsoncpp/json_writer.cpp

${OBJECTDIR}/msgpush/jwt.o: msgpush/jwt.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/jwt.o msgpush/jwt.cpp

${OBJECTDIR}/msgpush/md5.o: msgpush/md5.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/md5.o msgpush/md5.cpp

${OBJECTDIR}/msgpush/pmessage.o: msgpush/pmessage.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/pmessage.o msgpush/pmessage.cpp

${OBJECTDIR}/msgpush/xg_url.o: msgpush/xg_url.cpp
	${MKDIR} -p ${OBJECTDIR}/msgpush
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/msgpush/xg_url.o msgpush/xg_url.cpp

${OBJECTDIR}/muduo/base/AsyncLogging.o: muduo/base/AsyncLogging.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/AsyncLogging.o muduo/base/AsyncLogging.cc

${OBJECTDIR}/muduo/base/Condition.o: muduo/base/Condition.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/Condition.o muduo/base/Condition.cc

${OBJECTDIR}/muduo/base/CountDownLatch.o: muduo/base/CountDownLatch.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/CountDownLatch.o muduo/base/CountDownLatch.cc

${OBJECTDIR}/muduo/base/Exception.o: muduo/base/Exception.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/Exception.o muduo/base/Exception.cc

${OBJECTDIR}/muduo/base/LogFile.o: muduo/base/LogFile.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/LogFile.o muduo/base/LogFile.cc

${OBJECTDIR}/muduo/base/LogStream.o: muduo/base/LogStream.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/LogStream.o muduo/base/LogStream.cc

${OBJECTDIR}/muduo/base/Logging.o: muduo/base/Logging.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/Logging.o muduo/base/Logging.cc

${OBJECTDIR}/muduo/base/Thread.o: muduo/base/Thread.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/Thread.o muduo/base/Thread.cc

${OBJECTDIR}/muduo/base/Timestamp.o: muduo/base/Timestamp.cc
	${MKDIR} -p ${OBJECTDIR}/muduo/base
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/muduo/base/Timestamp.o muduo/base/Timestamp.cc

${OBJECTDIR}/public/CreditUDP.o: public/CreditUDP.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/CreditUDP.o public/CreditUDP.cpp

${OBJECTDIR}/public/DES.o: public/DES.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/DES.o public/DES.cpp

${OBJECTDIR}/public/GlobalOperation.o: public/GlobalOperation.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/GlobalOperation.o public/GlobalOperation.cpp

${OBJECTDIR}/public/IPCCMessage.o: public/IPCCMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/IPCCMessage.o public/IPCCMessage.cpp

${OBJECTDIR}/public/MemLog.o: public/MemLog.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/MemLog.o public/MemLog.cpp

${OBJECTDIR}/public/Rijndael.o: public/Rijndael.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/Rijndael.o public/Rijndael.cpp

${OBJECTDIR}/public/ServerInfoDef.o: public/ServerInfoDef.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/ServerInfoDef.o public/ServerInfoDef.cpp

${OBJECTDIR}/public/Spliter.o: public/Spliter.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/Spliter.o public/Spliter.cpp

${OBJECTDIR}/public/StdAfx.o: public/StdAfx.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/StdAfx.o public/StdAfx.cpp

${OBJECTDIR}/public/UDPSocket.o: public/UDPSocket.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/UDPSocket.o public/UDPSocket.cpp

${OBJECTDIR}/public/ini.o: public/ini.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/ini.o public/ini.cpp

${OBJECTDIR}/public/xmlParser.o: public/xmlParser.cpp
	${MKDIR} -p ${OBJECTDIR}/public
	${RM} "$@.d"
	$(COMPILE.cc) -g -I./ -Imsgpush/jsoncpp/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/public/xmlParser.o public/xmlParser.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
