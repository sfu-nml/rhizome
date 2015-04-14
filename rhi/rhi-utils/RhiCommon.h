#ifndef __RHI_COMMON_H__
#define __RHI_COMMON_H__

/* Common file.
 *
 * Copyright (C) 2015 	 
 *
 * This code is part of Rhizome.	
 */

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

// Default software encoder x264 configuration
#define RHI_ENCODER_X264_DEFAULT_OPTION_SOFTWARE 0

// Default hardware encoder NVH264 configuration
#define RHI_ENCODER_NV264_DEFAULT_OPTION_HARDWARE 1
#define RHI_ENCODER_NV264_DEFAULT_CONFIG_VERSION NVFBC_H264HWENC_CONFIG_VER
#define RHI_ENCODER_NV264_DEFAULT_PROFILE 77 									// Main Profile
#define RHI_ENCODER_NV264_DEFAULT_FRAMERATE 30
#define RHI_ENCODER_NV264_DEFAULT_FRAMERATE_DENSITY 1
#define RHI_ENCODER_NV264_DEFAULT_AVG_BITRATE 500000
#define RHI_ENCODER_NV264_DEFAULT_PEAK_BITRATE RHI_ENCODER_NV264_DEFAULT_AVG_BITRATE * 1.5
#define RHI_ENCODER_NV264_DEFAULT_GOPLENGTH RHI_ENCODER_NV264_DEFAULT_FRAMERATE * 10
#define RHI_ENCODER_NV264_DEFAULT_QP 26
#define RHI_ENCODER_NV264_DEFAULT_RATECONTROL NVFBC_H264_ENC_PARAMS_RC_CBR
#define RHI_ENCODER_NV264_DEFAULT_PRESETCONFIG NVFBC_H264_PRESET_LOW_LATENCY_HP // Low latency with high performance
#define RHI_ENCODER_NV264_DEFAULT_OUTBAND_SPSPPS FALSE
#define RHI_ENCODER_NV264_DEFAULT_NUM_B_FRAMES 0
#define RHI_ENCODER_NV264_DEFAULT_RECORD_TIMESTAMPS 0							// Do not record timestamps
#define RHI_ENCODER_NV264_DEFAULT_INTRA_FRAME_ON_REQUEST TRUE
#define RHI_ENCODER_NV264_DEFAULT_USE_MAX_RCQP FALSE
#define RHI_ENCODER_NV264_DEFAULT_ENABLE_SUBFRAME_ENCODING 0					// Reserved;
#define RHI_ENCODER_NV264_DEFAULT_ENABLE_CONSTRAINED_ENCODING 0					// Reserved;
#define RHI_ENCODER_NV264_DEFAULT_ENABLE_INTRA_REFRESH 1
#define RHI_ENCODER_NV264_DEFAULT_VBV_BUFFER_SIZE RHI_ENCODER_NV264_DEFAULT_PEAK_BITRATE/RHI_ENCODER_NV264_DEFAULT_FRAMERATE
#define RHI_ENCODER_NV264_DEFAULT_VBV_INITIAL_DELAY RHI_ENCODER_NV264_DEFAULT_VBV_BUFFER_SIZE
#define RHI_ENCODER_NV264_DEFAULT_MAX_RCQP_0 0
#define RHI_ENCODER_NV264_DEFAULT_MAX_RCQP_1 0
#define RHI_ENCODER_NV264_DEFAULT_MAX_RCQP_2 0
#define RHI_ENCODER_NV264_DEFAULT_NUM_REF_FRAMES 0
#define RHI_ENCODER_NV264_DEFAULT_STEREO_FORMAT 0
#define RHI_ENCODER_NV264_DEFAULT_SLICING_MODE 0
#define RHI_ENCODER_NV264_DEFAULT_SLICING_MODE_PARAM 0
#define RHI_ENCODER_NV264_DEFAULT_SETUP_VERSION 0

// Server configuration structures
typedef struct _rhiServerConfig {
 	unsigned int outPacketMaxSize;
 	unsigned short rtpPortNum;
 	unsigned short rtcpPortNum;
 	unsigned char ttl;
 	unsigned estimatedSessionBandwidth;
 	unsigned serverPort;
 	unsigned int payloadFormat;
 	std::string streamName;
} rhiServerConfig;

static const std::string RHI_SERVER_DEFAULT_STREAM_NAME = "testStream";

// Hardware encoder NVH264 configuration structures
typedef struct _rhiEncoderArgNV {
	// 			Encoder config
 	int 		rhiConfigVersion;
 	int 		rhiProfile;
 	int 		rhiFrameRateNum;
 	int 		rhiFrameRateDen;
 	int 		rhiAvgBitRate;
 	int 		rhiPeakBitRate;
 	int 		rhiGopLength;
 	int 		rhiQP;
 	int 		rhiRateControl;
 	int  		rhiPresetConfig;
 	int 		rhiOutBandSPSPPS;
 	int 		rhiNumBFrames;
 	int  		rhiRecordTimeStamps;
 	int  		rhiIntraFrameOnRequest;
 	int  		rhiUseMaxRCQP;
	int 		rhiEnableSubframeEncoding; 		// Reserved. Should be 0.
	int   		rhiEnableConstrainedEncoding;   // Reserved. Should be 0.
	int  		rhiEnableIntraRefresh;
	int  		rhiVBVBufferSize;
	int   		rhiVBVInitialDelay;
	int 		rhiMaxRCQP[3];
	int 		rhiMaxNumRefFrames;
	int 		rhiStereoFormat;
	int  		rhiSlicingMode;
	int 		rhiSlicingModeParam;
	// 			Other setup parameters
	int 		rhiSetupVersion;
	int 		rhiWithHWCursor;
	// 			Frame grab parameters
	int 		rhiFrameGrabVersion;
	int  		rhiFrameGrabFlags;				// Control flags to the call
	int  		rhiGMode;
} rhiEncoderArgNV;

typedef struct _rhiEncoderConfigNV {
	int         iBitrate;  						// Bitrate to use
	int         iProfile;  						// H.264 encoding profile; BASELINE (66), MAIN (77) and HIGH (100)
	int			eRateControl; 					// CBR, VBR, etc
	int			iGOPLength;  					// The I-Frame frequency
	int			iFPS;
	int			ePresetConfig; 					//NVFBC_H264_PRESET_LOW_LATENCY_HP, NVFBC_H264_PRESET_LOW_LATENCY_HQ, ETC
	int			height;
	int			width;
	int			threads;
} rhiEncoderConfigNV;

typedef struct _videoFrame {
	unsigned int isIFrame;
	unsigned int sizeBytes;
	unsigned char *outputBuffer;
} VideoFrame;

#endif