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
	// Read rhizome configuration file
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
		int rhiConfigVersion,
	 		rhiProfile,
	 		rhiFrameRateNum,
	 		rhiFrameRateDen,
	 		rhiAvgBitRate,
	 		rhiPeakBitRate,
	 		rhiGopLength,
	 		rhiQP,
	 		rhiRateControl,
	  		rhiPresetConfig,
	 		rhiOutBandSPSPPS,
	 		rhiNumBFrames,
	  		rhiRecordTimeStamps,
	  		rhiIntraFrameOnRequest,
	  		rhiUseMaxRCQP,
	 		rhiEnableSubframeEncoding, 		// Reserved. Should be 0.
	   		rhiEnableConstrainedEncoding,   // Reserved. Should be 0.
	  		rhiEnableIntraRefresh,
	  		rhiVBVBufferSize,
	   		rhiVBVInitialDelay,
	 		rhiMaxRCQP[3],
	 		rhiMaxNumRefFrames,
	 		rhiStereoFormat,
	  		rhiSlicingMode,
	 		rhiSlicingModeParam;
	 		// rhiSetupVersion,
	 		// rhiWithHWCursor,
	 		// rhiFrameGrabVersion,
	  		// rhiFrameGrabFlags,				
	  		// rhiGMode;
		//if(nvh264_cfg.lookupValue("iBitrate", iBitrate)) 
		//	encoderConfig.iBitrate = iBitrate;
			
	} catch (...) {
		std::cout<<"Rhi-Error -> Error reading encoder configuration file." <<std::endl;
		readEncoderParamDefault(); // Rollback
		return;
	}
};

void RhiInitManager::readEncoderParamDefault() {
	encoderConfig.rhiProfile = RHI_ENCODER_NV264_DEFAULT_PROFILE;
	encoderConfig.rhiFrameRateNum = RHI_ENCODER_NV264_DEFAULT_FRAMERATE;
	encoderConfig.rhiFrameRateDen = RHI_ENCODER_NV264_DEFAULT_FRAMERATE_DENSITY;
	encoderConfig.rhiAvgBitRate = RHI_ENCODER_NV264_DEFAULT_AVG_BITRATE;
	encoderConfig.rhiPeakBitRate = RHI_ENCODER_NV264_DEFAULT_PEAK_BITRATE;
	encoderConfig.rhiGopLength = RHI_ENCODER_NV264_DEFAULT_GOPLENGTH;
	encoderConfig.rhiQP = RHI_ENCODER_NV264_DEFAULT_QP;
	encoderConfig.rhiRateControl = RHI_ENCODER_NV264_DEFAULT_RATECONTROL;
	encoderConfig.rhiPresetConfig = RHI_ENCODER_NV264_DEFAULT_PRESETCONFIG;
	encoderConfig.rhiOutBandSPSPPS = RHI_ENCODER_NV264_DEFAULT_OUTBAND_SPSPPS;
	encoderConfig.rhiNumBFrames = RHI_ENCODER_NV264_DEFAULT_NUM_B_FRAMES;
	encoderConfig.rhiRecordTimeStamps = RHI_ENCODER_NV264_DEFAULT_RECORD_TIMESTAMPS;
	encoderConfig.rhiIntraFrameOnRequest = RHI_ENCODER_NV264_DEFAULT_INTRA_FRAME_ON_REQUEST;
	encoderConfig.rhiUseMaxRCQP = RHI_ENCODER_NV264_DEFAULT_USE_MAX_RCQP;
	encoderConfig.rhiEnableSubframeEncoding = RHI_ENCODER_NV264_DEFAULT_ENABLE_SUBFRAME_ENCODING;
	encoderConfig.rhiEnableConstrainedEncoding =  RHI_ENCODER_NV264_DEFAULT_ENABLE_CONSTRAINED_ENCODING;
	encoderConfig.rhiEnableIntraRefresh = RHI_ENCODER_NV264_DEFAULT_ENABLE_INTRA_REFRESH;
	encoderConfig.rhiVBVBufferSize = RHI_ENCODER_NV264_DEFAULT_VBV_BUFFER_SIZE;
	encoderConfig.rhiVBVInitialDelay = RHI_ENCODER_NV264_DEFAULT_VBV_INITIAL_DELAY;
	encoderConfig.rhiMaxRCQP[0] = RHI_ENCODER_NV264_DEFAULT_MAX_RCQP_0;
	encoderConfig.rhiMaxRCQP[1] = RHI_ENCODER_NV264_DEFAULT_MAX_RCQP_1;
	encoderConfig.rhiMaxRCQP[2] = RHI_ENCODER_NV264_DEFAULT_MAX_RCQP_2;
	encoderConfig.rhiMaxNumRefFrames = RHI_ENCODER_NV264_DEFAULT_NUM_REF_FRAMES;
	encoderConfig.rhiStereoFormat = RHI_ENCODER_NV264_DEFAULT_STEREO_FORMAT;
	encoderConfig.rhiSlicingMode = RHI_ENCODER_NV264_DEFAULT_SLICING_MODE;
	encoderConfig.rhiSlicingModeParam = RHI_ENCODER_NV264_DEFAULT_SLICING_MODE_PARAM;

	std::cout<< "Rhi -> Encoder initialized with default settings." << std::endl;
};

rhiEncoderArgNV RhiInitManager::getEncoderConfig() {
	return this->encoderConfig;
};
