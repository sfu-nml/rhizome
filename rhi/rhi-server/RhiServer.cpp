#include "RhiServer.h"

NvU32 NVH264::headerSize = 0;
unsigned char NVH264::ppssps_data[1024];

RhiServer::RhiServer() {
	rhi_config.outPacketMaxSize = RHI_SERVER_DEFAULT_OUT_PACKET_BUFFER_MAX_SIZE;
	rhi_config.rtpPortNum = RHI_SERVER_DEFAULT_RTP_PORT;
	rhi_config.rtcpPortNum = RHI_SERVER_DEFAULT_RTCP_PORT;
	rhi_config.ttl = RHI_SERVER_DEFAULT_TTL;
	rhi_config.estimatedSessionBandwidth = RHI_SERVER_DEFAULT_ESTIMATED_SESSION_BANDWIDTH;
	rhi_config.serverPort = RHI_SERVER_DEFAULT_PORT;
};

RhiServer::~RhiServer() {};

void RhiServer::startMediaServer() {
	OutPacketBuffer::maxSize = rhi_config.outPacketMaxSize;

	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	this->env = BasicUsageEnvironment::createNew(*scheduler);
	H264VideoRTPSink* videoSink;

	// Create 'groupsocks' for RTP and RTCP:
	struct in_addr destinationAddress;

	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
	// destinationAddress.s_addr = our_inet_addr("96.49.249.165");
	// Note: This is a multicast address.  If you wish instead to stream
	// using unicast, then you should use the "testOnDemandRTSPServer"
	// test program - not this test program - as a model.

	const unsigned short rtpPortNum = rhi_config.rtpPortNum;
	const unsigned short rtcpPortNum = rhi_config.rtcpPortNum;
	const unsigned char ttl = rhi_config.ttl;

	const Port rtpPort(rtpPortNum);
	const Port rtcpPort(rtcpPortNum);

	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
	rtpGroupsock.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
	rtcpGroupsock.multicastSendOnly(); // we're a SSM source

	// Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
	increaseSendBufferTo(*env,rtpGroupsock.socketNum(),550*1024);

	// Create (and start) a 'RTCP instance' for this RTP sink:
	const unsigned estimatedSessionBandwidth = rhi_config.estimatedSessionBandwidth; // in kbps; for RTCP b/w share
	unsigned char CNAME[RHI_SERVER_DEFAULT_MAXCNAME_LEN + 1];
	gethostname((char*)CNAME, RHI_SERVER_DEFAULT_MAXCNAME_LEN);
	CNAME[RHI_SERVER_DEFAULT_MAXCNAME_LEN] = '\0';
	RTCPInstance* rtcp
		= RTCPInstance::createNew(*env, &rtcpGroupsock,
		estimatedSessionBandwidth, CNAME,
		videoSink, NULL /* we're a server */,
		True /* we're a SSM source */);

	// Note: This starts RTCP running automatically
	RTSPServer* rtspServer = RTSPServer::createNew(*env, rhi_config.serverPort);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	ServerMediaSession* sms
		= ServerMediaSession::createNew(*env, "testStream", "",
		"Session streamed by \"testH264VideoStreamer\"",
		True /*SSM*/);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	//sms->addSubsession(PassiveServerMediaSubsession::createNew(*audioSink, rtcp));

	rtspServer->addServerMediaSession(sms);

	char* url = rtspServer->rtspURL(sms);
	*env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;

	// Start the streaming:
	*env << "Beginning streaming...\n";

	LiveSourceWithx264 *source = LiveSourceWithx264::createNew(*env);

	// Create a framer for the Video Elementary Stream:
	H264VideoStreamDiscreteFramer* dSource = H264VideoStreamDiscreteFramer::createNew(*env, source);

	// Finally, start playing:
	*env << "Beginning to read from framer...\n";
	videoSink->startPlaying(*dSource, NULL, videoSink);
	//audioSink->startPlaying(*audioSourceSwap,NULL,audioSink);
	env->taskScheduler().doEventLoop(); // does not return
};

void RhiServer::serverInitialize() {
	using namespace libconfig;
	Config cfg;

	// Reading rhizome configuration file
	try {
		cfg.readFile("rhi.cfg");
	} catch(const libconfig::FileIOException &fioex) {
		std::cerr << "I/O error while reading Rhizome configuration file. " << std::endl;
		initializeWithDefault();
		return;
	} catch(const libconfig::ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
			<< " - " << pex.getError() << std::endl;
		initializeWithDefault();
		return;
	}

	Setting &root = cfg.getRoot();
	
	if(!root.exists("server")) {
		initializeWithDefault();
		return;
	} 

	const Setting &live555 = root["server"]["live555"];
	const Setting &live555_cfg = live555[0];

	try {
		int outPacketMaxSize, rtpPortNum, rtcpPortNum, ttl, estimatedSessionBandwidth, serverPort;
		
		live555_cfg.lookupValue("outPacketMaxSize",outPacketMaxSize);
		live555_cfg.lookupValue("rtpPortNum",rtpPortNum);
		live555_cfg.lookupValue("rtcpPortNum",rtcpPortNum);
		live555_cfg.lookupValue("ttl",ttl);
		live555_cfg.lookupValue("estimatedSessionBandwidth",estimatedSessionBandwidth);
		live555_cfg.lookupValue("serverPort",serverPort);

		rhi_config.outPacketMaxSize = outPacketMaxSize;
		rhi_config.rtpPortNum = rtpPortNum;
		rhi_config.rtcpPortNum = rtcpPortNum;
		rhi_config.ttl = ttl;
		rhi_config.estimatedSessionBandwidth = estimatedSessionBandwidth;
		rhi_config.serverPort = serverPort;
	} catch (...) {
		std::cout<<"Error reading configuration file." <<std::endl;
		initializeWithDefault();
		return;
	}
}

void RhiServer::initializeWithDefault() {
	std::cout<< "Initialize with default settings." << std::endl;
}




