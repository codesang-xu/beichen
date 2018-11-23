//
// GlobalDefs.h
//
#include <map>
using namespace std;

#ifndef __H_GlobalDefs__
#define __H_GlobalDefs__

//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
//#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
//#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
//#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//#ifndef SAFE_DELETE
//#define SAFE_DELETE(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
//#endif
//
//#ifndef SAFE_DELETE_ARRAY
//#define SAFE_DELETE_ARRAY(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
//#endif
//
//#ifndef SAFE_FREE
//#define SAFE_FREE(x)	if( (x)!=NULL ) { free(x); (x)=NULL; }
//#endif
//
//#ifndef SAFE_RELEASE
//#define SAFE_RELEASE(x)	if( (x)!=NULL ) { (x)->Release(); (x)=NULL; }
//#endif
//
//// Attention: we place TCP listening on the client
//#define SERVER_UDP_PORT			10096
//#define CLIENT_UDP_PORT			10096
//#define RECV_COMMAND_PORT		10999 
//
//#define SERVER_VIDEO_UDP_PORT		12000
//#define SERVER_AUDIO_UDP_PORT		13000
//#define CLIENT_VIDEO_UDP_PORT		12000
//#define CLIENT_AUDIO_UDP_PORT		13000
//
//#ifdef __AudioFormat_20ms
//	#define	Audio_EncodeUnit		20	
//	#define AUDIODELAY              40 
//	#define Audio_EncodeDataSize	38
//	#define Audio_DecodeDataSize	320
//	#define Audio_DecodeShortSize	160
//#else	
//	#define	Audio_EncodeUnit		30
//	#define AUDIODELAY              30
//	#define Audio_EncodeDataSize	50
//	#define Audio_DecodeDataSize	480
//	#define Audio_DecodeShortSize	240
//#endif 
//
//#define AUDIOCOM                (AUDIODELAY/Audio_EncodeUnit*Audio_EncodeDataSize)	//采集压缩后的数据大小
//#define AUDIODECOM              (AUDIODELAY/Audio_EncodeUnit*Audio_DecodeDataSize)	//接收解压后的数据大小
//// Role definitions
//
//typedef enum 
//{
//	SERVER = 0, 
//	CLIENT
//} ROLE;
//
//// Device types
//typedef enum
//{
//	DT_DV = 0,
//	DT_USB_Camera,
//	DT_Audio_Capture,
//	DT_Unknown
//} Device_Type;
//
//// How devices config to the peer PCs
//typedef enum 
//{
//	Local_Has_Video  = 1, 
//	Local_Has_Audio  = 2,
//	Remote_Has_Video = 4,
//	Remote_Has_Audio = 8
//} Device_Config;
//
//const long Local_Device_Mask	= 0x03;
//const long Remote_Device_Mask	= 0x0c;
//
//// Preferred capture size
//const long Preferred_Width		= 320;//176;////240;
//const long Preferred_Height		= 240;//144;////180;
//
//// TCP stream header
//struct Pack_Header
//{
//	long pack_type;
//	long pack_size;
//
//	void my_hton(void) 
//	{
//		pack_type		= htonl(pack_type);
//		pack_size		= htonl(pack_size);
//	};
//
//	void my_ntoh(void) 
//	{
//		pack_type		= ntohl(pack_type);
//		pack_size		= ntohl(pack_size);
//	};
//};
//
//// TCP pack types
//const long PT_AudioMediaType	= 10001;
//const long PT_VideoMediaType	= 10002;
//const long PT_AudioPayload		= 10005;
//const long PT_VideoPayload		= 10006;
//const long PT_Payload			= 10003;
// 
//// Messages
//const long msg_FilterGraphError		= 'avct' + 1;
//const long msg_MediaTypeReceived	= 'avct' + 2;
//const long msg_TCPSocketAccepted	= 'avct' + 3;
//const long msg_UDPCommandReceived	= 'avct' + 4;
//const long msg_ModifyFilterGraph	= 'avct' + 5;
//
//// Let the main thread modify filter graph
//#define WM_ModifyFilterGraph		(WM_USER+123)
//
//// UDP command defines
//const long MAX_COMMAND_SIZE			= 100;
//const long cmd_ClientCalling		= 'avct' + 100;
//const long cmd_DeviceConfig			= 'avct' + 101;
//const long cmd_BuildFilterGraph		= 'avct' + 102;
//const long cmd_DisconnectRequest	= 'avct' + 103;
//
//// UDP command pack
//struct UDP_Pack
//{
//	long command;
//	long param1;
//	long param2;
//	long reserved;
//
//	void my_hton(void) 
//	{
//		command		= htonl(command);
//		param1		= htonl(param1);
//		param2		= htonl(param2);
//		reserved	= htonl(reserved);
//	};
//
//	void my_ntoh(void) 
//	{
//		command		= ntohl(command);
//		param1		= ntohl(param1);
//		param2		= ntohl(param2);
//		reserved	= ntohl(reserved);
//	};
//};
//
//#define UDPAUDIOLEN AUDIOCOM
//#define UDPVIDEOLEN 1024
//#define NO__VIDEO_RESNDMSG
//
//#define UDPREDUNDANCY_COUNT	1	//冗余个数
//#define UDPAUDIOLEN_MINE (UDPAUDIOLEN+UDPAUDIOLEN%sizeof(int))	//int 对齐
//
//class CAudioData	//音频数据包
//{
//public:
//	int iDataSeq;						//包序号
//	BYTE DataBuffer[(UDPREDUNDANCY_COUNT+1)*UDPAUDIOLEN_MINE];	//数据
//};
////by yan add
////class CVideoData
////{
////public:
////	int totalcount;    //总块数
////	int iLastLen;
////	int iDataSeq;
////	int iSequence;
////	int iLen;
////	BYTE DataBuffer[UDPVIDEOLEN];
////};
//
////by yan add
//struct CVideoData
//{
//public:
//	int totalcount;		//一次采集数据分包发送的包数
//	int iLastLen;		//一次采集数据分包发送的最后一个包大小
//	int iDataSeq;		//数据包大序号
//	int iSequence;		//一次采集数据分包的包序号
//	int iLen;			//一次采集数据分包的包大小
//	bool bIsKeyFrame;	//是否关键桢
//	char * DataBuffer;	//一次采集数据分包的包数据
//};
//
//struct VData
//{
//    int inum;
//    BYTE buffer[UDPVIDEOLEN];
//	int iRepirePos;	//补偿位置
//};
//
//class OneTimeVideoData
//{
//public:
//	VData buf[50];
//	int totalcount;
//	int iLastLen;
//	int iLastDataLen;
//	int iMaxData;
//	int iTime;
//	int iRealRecvCount;
//	bool bIsKeyFrame;
//	int iSeq;
//	OneTimeVideoData()
//	{
//		iRealRecvCount=0;
//		totalcount=0;
//		iLastLen=0;
//		iTime=0;
//		iLastDataLen=0;
//		iMaxData = 0;
//		bIsKeyFrame = false;
//		
//		for (int i=0;i<50;i++)
//		{
//			buf[i].inum=0;
//			//memset(buf[i].buffer,RGB(0,0,0),UDPVIDEOLEN);
//			buf[i].iRepirePos=-1;
//		}
//	}
//};
//
//class OneTimeAudioData
//{
//public:
//	BYTE buffer[UDPAUDIOLEN];
//	int iTime;
//	OneTimeAudioData()
//	{
//		memset(buffer,0,UDPAUDIOLEN);
//	}
//};
//
//#define LOCALAUDIOBUF_COUNT		3		//音频缓冲个数
//#define LOCALAUDIOREPAIR_COUNT	1		//补偿个数
//
////视频改进
//#define LOCALVIDEOBUF_COUNT		5		//接收视频数据缓冲buf个数
//#define RESND_VIDEOBUF_COUNT	32		//重发视频数据缓冲buf个数
//#define RESNDVID_REQ_INTERVAL	500		//重发请求间隔 ms
//
//typedef struct __ResendVideoDataMsg
//{
//	char	dataHead[8];
//	int		dataSeq_BeginPos;		//重发数据包开始序号
//	int		dataSeq_EndPos;			//重发数据包结束序号
//	int		subDataSeq_BeginPos;	//数据包内部开始序号
//	int		subDataSeq_EndPos;		//数据包内部结束序号
//
//	__ResendVideoDataMsg()
//	{
//		memcpy(dataHead, "VidReSnd", 8);
//		dataSeq_BeginPos = 0;
//		dataSeq_EndPos = 0;
//		subDataSeq_BeginPos = 0;
//		subDataSeq_EndPos = 0;
//	}
//}ResendVideoDataMsg;
//
//typedef struct __ResendSubVideoData	//视频分包小序号数据结构
//{
//	int		len;			//数据大小
//	char*	data;			//数据指针
//}ResendSubVideoData;
//
//typedef map<int, ResendSubVideoData*> MAP_RESNDSUBVIDDATA;
//typedef map<int, MAP_RESNDSUBVIDDATA*> MAP_RESNDVIDDATA;

//心跳频率
//#define __HEARTBEAT_INTERVAL	10000
#define __HEARTBEAT_INTERVAL	5000

typedef struct __OutDialSys_Info
{
	bool bIsOutDialGroup;
	int GroupId;
	int TaskId;
	int PhoneId;
	int DialSeq;
	unsigned long OutdialEngIp;
	unsigned short OutdialEngPort;
}OutDialSys_Info;

#endif // __H_GlobalDefs__