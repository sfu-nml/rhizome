#ifndef NvFBCH264_H
#define NvFBCH264_H


#include <NvFBCLibrary.h>
#include <nvFBCH264.h>
#include <nvFBCToSys.h>
#include <nvFBC.h>
#include <string>
#include <windows.h>
#include <stdio.h> 
#include <iostream>
#include <sstream>
#include "x264Encoder.h"
#include <Util.h>


class NVH264
{

private:
    NvFBCLibrary* nvfbc;
    NvFBC_H264HWEncoder_Config encodeConfig;
    NVFBC_H264_SETUP_PARAMS fbch264SetupParams;
	NvFBCToH264HWEncoder *encoder;
	NvFBCToSys *sysFramer;
	x264Encoder* sEncoder;
	int Init();
public:
	//static const char * getHeader();
	NVH264();
	~NVH264();
	int BenchMarkSoftware(int,int);

typedef struct _AppArguments
{
    int         iBitrate;  // Bitrate to use
    int         iProfile;  // H.264 encoding profile; BASELINE (66), MAIN (77) and HIGH (100)
	int			eRateControl; // CBR, VBR, etc
	int			iGOPLength;  // The I-Frame frequency
	int			iFPS;
	int			ePresetConfig; //NVFBC_H264_PRESET_LOW_LATENCY_HP, NVFBC_H264_PRESET_LOW_LATENCY_HQ, ETC
	int			height;
	int			width;
	int			threads;
	
} AppArguments;

	typedef struct _videoFrame
	{
	unsigned int isIFrame;
	unsigned int sizeBytes;
	unsigned char *outputBuffer;

	} VideoFrame;

	unsigned char *outputBuffer;
	unsigned char *rawBuffer;
   int Cleanup();
   int BenchMark(int numFrames,int fps); //numFrames, fps
   VideoFrame GrabFrame();
   VideoFrame GrabFrameSoftware();
     static unsigned char ppssps_data[1024];
  //static NVFBC_H264_GET_HEADER_PARAMS headerParams ={0};
  static NvU32 headerSize;
  bool firstFrame;
  NvU32* sizeBytes;
  AppArguments Arguments;
  int ReInit(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig);
   int ReInitToMemory(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig,int height,int width,int threads);
};

#endif