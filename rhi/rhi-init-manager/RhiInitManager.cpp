#include "RhiInitManager.h"

RhiInitManager::RhiInitManager() {}

RhiInitManager::~RhiInitManager() {}

void RhiInitManager::readServerParam() {
	using namespace libconfig;
	Config cfg;

	// Reading rhizome configuration file
	try {
		cfg.readFile("rhi.cfg");
	} catch(const libconfig::FileIOException &fioex) {
		std::cerr << "Rhi-Error -> I/O error while reading Rhizome configuration file. " << std::endl;
		readServerParamDefault();
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	} catch(const libconfig::ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
			<< " - " << pex.getError() << std::endl;
		readServerParamDefault();
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	}

	Setting &root = cfg.getRoot();

	if(!root.exists("server")) {
		readServerParamDefault();
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
			serverConfig.outPacketMaxSize = outPacketMaxSize;
		if(live555_cfg.lookupValue("rtpPortNum",rtpPortNum)) 
			serverConfig.rtpPortNum = rtpPortNum;
		if(live555_cfg.lookupValue("rtcpPortNum",rtcpPortNum))
			serverConfig.rtcpPortNum = rtcpPortNum;
		if(live555_cfg.lookupValue("ttl",ttl))
			serverConfig.ttl = ttl;
		if(live555_cfg.lookupValue("estimatedSessionBandwidth",estimatedSessionBandwidth))
			serverConfig.estimatedSessionBandwidth = estimatedSessionBandwidth;
		if(live555_cfg.lookupValue("serverPort",serverPort))
			serverConfig.serverPort = serverPort;
		if(live555_cfg.lookupValue("payloadFormat",payloadFormat))
			serverConfig.payloadFormat = payloadFormat;
		if(live555_cfg.lookupValue("streamName",streamName))
			serverConfig.streamName = streamName;
	} catch (...) {
		std::cout<<"Rhi-Error -> Error reading configuration file." <<std::endl;
		readServerParamDefault(); // Rollback
		std::cout<< "Rhi -> Initialized with default settings." << std::endl;
		return;
	}
};

void RhiInitManager::readServerParamDefault() {
	serverConfig.outPacketMaxSize = RHI_SERVER_DEFAULT_OUT_PACKET_BUFFER_MAX_SIZE;
	serverConfig.rtpPortNum = RHI_SERVER_DEFAULT_RTP_PORT;
	serverConfig.rtcpPortNum = RHI_SERVER_DEFAULT_RTCP_PORT;
	serverConfig.ttl = RHI_SERVER_DEFAULT_TTL;
	serverConfig.estimatedSessionBandwidth = RHI_SERVER_DEFAULT_ESTIMATED_SESSION_BANDWIDTH;
	serverConfig.serverPort = RHI_SERVER_DEFAULT_PORT;
	serverConfig.payloadFormat = RHI_SERVER_DEFAULT_RTP_PAYLOAD_FORMAT;
	serverConfig.streamName = RHI_SERVER_DEFAULT_STREAM_NAME;
}

rhiServerConfig RhiInitManager::getServerConfig() {
	return this->serverConfig;
}

void RhiInitManager::readEncoderParam() {};

void RhiInitManager::readEncoderParamDefault() {};

void RhiInitManager::getEncoderConfig() {};
