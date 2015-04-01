#include "RhiInitManager.h"

RhiInitManager::RhiInitManager() {}

RhiInitManager::~RhiInitManager() {}

void RhiInitManager::readParamAll() {
	if (readConfigFile()) {
		readServerParam();
		readEncoderParam();
	} else {
		readServerParamDefault();
		readEncoderParamDefault();
	}
}

bool RhiInitManager::readConfigFile() {
	using namespace libconfig;
	// Reading rhizome configuration file
	try {
		cfg.readFile("rhi.cfg");
	} catch(const libconfig::FileIOException &fioex) {
		std::cerr << "Rhi-Error -> I/O error while reading Rhizome configuration file. " << std::endl;
		return 0;
	} catch(const libconfig::ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
			<< " - " << pex.getError() << std::endl;
		return 0;
	}
	return 1;
}

void RhiInitManager::readServerParam() {
	using namespace libconfig;

	Setting &root = cfg.getRoot();

	if(!root.exists("server")) {
		readServerParamDefault();
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
		std::cout<<"Rhi-Error -> Error reading server configuration file." <<std::endl;
		readServerParamDefault(); // Rollback
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
	std::cout<< "Rhi -> Server initialized with default settings." << std::endl;
}

rhiServerConfig RhiInitManager::getServerConfig() {
	return this->serverConfig;
}

void RhiInitManager::readEncoderParam() {
	using namespace libconfig;

	Setting &root = cfg.getRoot();

	if(!root.exists("encoder")) {
		readEncoderParamDefault();
		return;
	} 

	const Setting &nvh264 = root["encoder"]["nvh264"];
	const Setting &nvh264_cfg = nvh264[0];

	try {
		int iBitrate, iProfile, eRateControl, iGOPLength, iFPS, ePresetConfig;

		//if(nvh264_cfg.lookupValue("iBitrate", iBitrate)) 
		//	encoderConfig.iBitrate = iBitrate;
		//if(nvh264_cfg.lookupValue("iProfile", iProfile)) 
		//	encoderConfig.iProfile = iProfile;
		//if(nvh264_cfg.lookupValue("eRateControl", eRateControl)) 
		//	encoderConfig.eRateControl = eRateControl;
		//if(nvh264_cfg.lookupValue("iGOPLength", iGOPLength)) 
		//	encoderConfig.iGOPLength = iGOPLength;
		//if(nvh264_cfg.lookupValue("iFPS", iFPS)) 
		//	encoderConfig.iFPS = iFPS;
		//if(nvh264_cfg.lookupValue("ePresetConfig", ePresetConfig)) 
		//	encoderConfig.ePresetConfig = ePresetConfigS;
	} catch (...) {
		std::cout<<"Rhi-Error -> Error reading encoder configuration file." <<std::endl;
		readEncoderParamDefault(); // Rollback
		return;
	}
};

void RhiInitManager::readEncoderParamDefault() {
	encoderConfig.iBitrate = RHI_ENCODER_DEFAULT_BITRATE;  // Bitrate to use
	encoderConfig.iProfile = RHI_ENCODER_DEFAULT_PROFILE;  // H.264 encoding profile; BASELINE (66), MAIN (77) and HIGH (100)
	encoderConfig.eRateControl = RHI_ENCODER_DEFAULT_RATECONTROL; // CBR, VBR, etc
	encoderConfig.iGOPLength = RHI_ENCODER_DEFAULT_GOPLENGTH;  // The I-Frame frequency
	encoderConfig.iFPS = RHI_ENCODER_DEFAULT_FRAMERATE;
	encoderConfig.ePresetConfig = RHI_ENCODER_DEFAULT_PRESETCONFIG; //NVFBC_H264_PRESET_LOW_LATENCY_HP, NVFBC_H264_PRESET_LOW_LATENCY_HQ, ETC
	  

	//encoderConfig.height;
	//encoderConfig.width;
	//encoderConfig.threads;
	std::cout<< "Rhi -> Encoder initialized with default settings." << std::endl;
};

rhiEncoderConfigNV RhiInitManager::getEncoderConfig() {
	return this->encoderConfig;
};
