#include "RhiServer.h"

NvU32 NVH264::headerSize = 0;
unsigned char NVH264::ppssps_data[1024];

RhiServer::RhiServer() {
	initializeWithDefault();
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

void RhiServer::serverInitialize() {
	using namespace libconfig;
	Config cfg;

	// Reading rhizome configuration file
	try {
		cfg.readFile("rhi.cfg");
	} catch(const libconfig::FileIOException &fioex) {
		std::cerr << "Rhi-Error -> I/O error while reading Rhizome configuration file. " << std::endl;
		initializeWithDefault();
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	} catch(const libconfig::ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		<< " - " << pex.getError() << std::endl;
		initializeWithDefault();
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	}

	Setting &root = cfg.getRoot();
	
	if(!root.exists("server")) {
		initializeWithDefault();
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	} 

	const Setting &live555 = root["server"]["live555"];
	const Setting &live555_cfg = live555[0];

	try {
		int outPacketMaxSize, rtpPortNum, rtcpPortNum, ttl, 
		payloadFormat, estimatedSessionBandwidth, serverPort;
		std::string streamName;
		
		if(live555_cfg.lookupValue("outPacketMaxSize",outPacketMaxSize)) 
			rhi_config.outPacketMaxSize = outPacketMaxSize;
		if(live555_cfg.lookupValue("rtpPortNum",rtpPortNum)) 
			rhi_config.rtpPortNum = rtpPortNum;
		if(live555_cfg.lookupValue("rtcpPortNum",rtcpPortNum))
			rhi_config.rtcpPortNum = rtcpPortNum;
		if(live555_cfg.lookupValue("ttl",ttl))
			rhi_config.ttl = ttl;
		if(live555_cfg.lookupValue("estimatedSessionBandwidth",estimatedSessionBandwidth))
			rhi_config.estimatedSessionBandwidth = estimatedSessionBandwidth;
		if(live555_cfg.lookupValue("serverPort",serverPort))
			rhi_config.serverPort = serverPort;
		if(live555_cfg.lookupValue("payloadFormat",payloadFormat))
			rhi_config.payloadFormat = payloadFormat;
		if(live555_cfg.lookupValue("streamName",streamName))
			rhi_config.streamName = streamName;
	} catch (...) {
		std::cout<<"Rhi-Error -> Error reading configuration file." <<std::endl;
		initializeWithDefault(); // Rollback
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	}
}

void RhiServer::initializeWithDefault() {
	rhi_config.outPacketMaxSize = RHI_SERVER_DEFAULT_OUT_PACKET_BUFFER_MAX_SIZE;
	rhi_config.rtpPortNum = RHI_SERVER_DEFAULT_RTP_PORT;
	rhi_config.rtcpPortNum = RHI_SERVER_DEFAULT_RTCP_PORT;
	rhi_config.ttl = RHI_SERVER_DEFAULT_TTL;
	rhi_config.estimatedSessionBandwidth = RHI_SERVER_DEFAULT_ESTIMATED_SESSION_BANDWIDTH;
	rhi_config.serverPort = RHI_SERVER_DEFAULT_PORT;
	rhi_config.payloadFormat = RHI_SERVER_DEFAULT_RTP_PAYLOAD_FORMAT;
	rhi_config.streamName = RHI_SERVER_DEFAULT_STREAM_NAME;
}





