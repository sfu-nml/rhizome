#include "RhiServer.h"

RhiServer::RhiServer(RhiInitManager* init_mgr) {
	rhi_config = init_mgr->getServerConfig();
};

RhiServer::~RhiServer() {};

void RhiServer::startMediaServer() {
	OutPacketBuffer::maxSize = rhi_config.outPacketMaxSize;

	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	this->env = BasicUsageEnvironment::createNew(*scheduler);
	H264VideoRTPSink* videoSink;

	// Create 'groupsocks' for RTP and RTCP
	struct in_addr destinationAddress;

	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);

	const unsigned short rtpPortNum = rhi_config.rtpPortNum;
	const unsigned short rtcpPortNum = rhi_config.rtcpPortNum;
	const unsigned char ttl = rhi_config.ttl;

	const Port rtpPort(rtpPortNum);
	const Port rtcpPort(rtcpPortNum);

	// Both are SSM source; Possibly add Unicast here
	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
	rtpGroupsock.multicastSendOnly(); 
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
	rtcpGroupsock.multicastSendOnly();

	// Create a 'H264 Video RTP' sink from the RTP 'groupsock'
	videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, rhi_config.payloadFormat);
	increaseSendBufferTo(*env,rtpGroupsock.socketNum(),550*1024);

	// Create (and start) a 'RTCP instance' for this RTP sink
	const unsigned estimatedSessionBandwidth = rhi_config.estimatedSessionBandwidth; // in kbps; for RTCP b/w share
	unsigned char CNAME[RHI_SERVER_DEFAULT_MAXCNAME_LEN + 1];
	gethostname((char*)CNAME, RHI_SERVER_DEFAULT_MAXCNAME_LEN);
	CNAME[RHI_SERVER_DEFAULT_MAXCNAME_LEN] = '\0';
	RTCPInstance* rtcp
	= RTCPInstance::createNew(*env, &rtcpGroupsock,
		estimatedSessionBandwidth, CNAME,
		videoSink, NULL,
		True);

	// Create RTSP Server; RTCP server will run automatically
	RTSPServer* rtspServer = RTSPServer::createNew(*env, rhi_config.serverPort);
	if (rtspServer == NULL) {
		*env << "Rhi-Error -> Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	// Create media session, add it to the server; RTCP subsession is necessary
	ServerMediaSession* sms
	= ServerMediaSession::createNew(*env, rhi_config.streamName.c_str(), "",
		"Rhi -> Session streamed by \"testH264VideoStreamer\"",
		True);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));

	rtspServer->addServerMediaSession(sms);

	// Inform the url 
	char* url = rtspServer->rtspURL(sms);
	*env << "Rhi -> Play this stream using the URL \"" << url << "\"\n";
	delete[] url;

	// Create a live video source and start streaming
	*env << "Rhi -> Start streaming...\n";

	LiveSourceWithx264 *source = LiveSourceWithx264::createNew(*env);

	// Create a DISCRETE framer for the Video stream from the source
	H264VideoStreamDiscreteFramer* dSource = H264VideoStreamDiscreteFramer::createNew(*env, source);

	// Finally, start playing:
	*env << "Rhi -> Start to read from framer...\n";
	videoSink->startPlaying(*dSource, NULL, videoSink);
	env->taskScheduler().doEventLoop(); // does not return
};





