#ifndef __RHI_NvFBCH264_H__
#define __RHI_NvFBCH264_H__

#include <NvFBCLibrary.h>
#include <nvFBCH264.h>
#include <nvFBCToSys.h>
#include <nvFBC.h>
#include <string>
#include <windows.h>
#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <Util.h>
#include "Rhix264Encoder.h"
#include "RhiCommon.h"

class NVH264
{
private:
	NvFBCLibrary* nvfbc;
	NvFBCToH264HWEncoder *encoder;
	NvFBCToSys *sysFramer;
	x264Encoder* sEncoder;
	
	NvFBC_H264HWEncoder_Config encodeConfig;
	NVFBC_H264_SETUP_PARAMS fbch264SetupParams;
	
	int Init();
public:
	//static const char * getHeader();
	NVH264();
	~NVH264();
	int Cleanup();

	unsigned char *outputBuffer;
	unsigned char *rawBuffer;

   	VideoFrame GrabFrameCompressed();
   	VideoFrame GrabFrameRawToSys();
   	
   	static unsigned char ppssps_data[1024];
   	static NvU32 headerSize;
   	bool firstFrame;
   	NvU32* sizeBytes;
   	rhiEncoderConfig Arguments;
   	int ReInit(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig);
   	int ReInitToMemory(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig,int height,int width,int threads);
	int BenchMark(int numFrames,int fps); //numFrames, fps
	int BenchMarkSoftware(int,int);
   };

#endif