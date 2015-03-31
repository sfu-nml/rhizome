#ifndef __RHI_COMMON_H__
#define __RHI_COMMON_H__

#include <iostream>
#include <iomanip>
#include <cstdlib>

// Default Server configuration
#define RHI_SERVER_DEFAULT_OUT_PACKET_BUFFER_MAX_SIZE 500000
#define RHI_SERVER_DEFAULT_RTP_PORT 20000
#define RHI_SERVER_DEFAULT_RTCP_PORT RHI_SERVER_DEFAULT_RTP_PORT + 1
#define RHI_SERVER_DEFAULT_TTL 225
#define RHI_SERVER_DEFAULT_ESTIMATED_SESSION_BANDWIDTH 6000
#define RHI_SERVER_DEFAULT_MAXCNAME_LEN 100
#define RHI_SERVER_DEFAULT_PORT 8554
#define RHI_SERVER_DEFAULT_RTP_PAYLOAD_FORMAT 96

struct rhiServerConfig {
	unsigned int outPacketMaxSize;
	unsigned short rtpPortNum;
	unsigned short rtcpPortNum;
	unsigned char ttl;
	unsigned estimatedSessionBandwidth;
	unsigned serverPort;
	unsigned int payloadFormat;
	std::string streamName;
};

// Default software encoder configuration
#define RHI_ENCODER_DEFAULT_OPTION_SOFTWARE 0
#define RHI_ENCODER_DEFAULT_OPTION RHI_ENCODER_DEFAULT_OPTION_SOFTWARE

// Default hardware encoder configuration
#define RHI_ENCODER_DEFAULT_OPTION_HARDWARE 1
#define RHI_ENCODER_DEFAULT_BITRATE 500000;
#define RHI_ENCODER_DEFAULT_PROFILE 66 // BASELINE
#define RHI_ENCODER_DEFAULT_RATECONTROL NVFBC_H264_ENC_PARAMS_RC_CBR
#define RHI_ENCODER_DEFAULT_FRAMERATE 30
#define RHI_ENCODER_DEFAULT_GOPLENGTH RHI_ENCODER_DEFAULT_FRAMERATE * 10
#define RHI_ENCODER_DEFAULT_PRESETCONFIG NVFBC_H264_PRESET_LOW_LATENCY_HP // Low latency with high performance

static const std::string RHI_SERVER_DEFAULT_STREAM_NAME = "testStream";

#endif