/* hardware frame 
 *
 * Copyright (C) 2015 	 
 *
 * This code is part of Rhizome.	
 */

#include "RhiEncoderNVH264.h"

NvU32 NVH264::headerSize = 0;
unsigned char NVH264::ppssps_data[1024];

NVH264::NVH264()
{
	encoder=NULL;
	sysFramer=NULL;
	Init();
}

NVH264::~NVH264()
{
	Cleanup();
}

int NVH264::Init()
{
	NVFBCRESULT res = NVFBC_SUCCESS;

	Arguments.iBitrate = 500000;
	Arguments.iProfile = 77;
	Arguments.eRateControl = NVFBC_H264_ENC_PARAMS_RC_CBR;
	Arguments.iFPS = 30;
	Arguments.iGOPLength = 10*Arguments.iFPS;//I-frames every 10 seconds
	Arguments.ePresetConfig = NVFBC_H264_PRESET_LOW_LATENCY_HP;

	int ret = 0;
	firstFrame = true;
	DWORD maxWidth = -1, maxHeight = -1;

	nvfbc = new NvFBCLibrary();
	memset(&encodeConfig, 0, sizeof(encodeConfig));
	memset(&fbch264SetupParams, 0, sizeof(fbch264SetupParams));

	encodeConfig.dwVersion = NVFBC_H264HWENC_CONFIG_VER;

	//Play with these
	encodeConfig.dwProfile = Arguments.iProfile;// MAIN profile
	encodeConfig.eRateControl = (NVFBC_H264_ENC_PARAMS_RC_MODE)Arguments.eRateControl;
	encodeConfig.dwAvgBitRate = Arguments.iBitrate;
	// encodeConfig.eRateControl = NVFBC_H264_ENC_PARAMS_RC_VBR; // Set the rate control
	// encodeConfig.dwPeakBitRate = (NvU32)(args.iBitrate * 1.50); // Set the peak bitrate to 150% of the average

	//encodeConfig.dwMaxNumRefFrames = 0;//For Nvidia Default

	encodeConfig.dwFrameRateNum = Arguments.iFPS;
	encodeConfig.dwFrameRateDen = 1; // Set the target frame rate at 30

	encodeConfig.dwGOPLength = 100; // The I-Frame frequency

	encodeConfig.ePresetConfig = ( NVFBC_H264_PRESET )Arguments.ePresetConfig;
	encodeConfig.dwQP = 26; // Quantization parameter, between 0 and 51 

	encodeConfig.bOutBandSPSPPS = false; // Use inband SPSPPS, if you need to grab headers on demand use outband SPSPPS

	encodeConfig.bRecordTimeStamps = FALSE; // Don't record timestamps
	encodeConfig.stereoFormat = NVFBC_H264_STEREO_NONE; // No stereo

	fbch264SetupParams.dwVersion = NVFBC_H264_SETUP_PARAMS_VER;
	fbch264SetupParams.bWithHWCursor = TRUE;
	fbch264SetupParams.pEncodeConfig = &encodeConfig;

	//! Load the NvFBC library.
	if(!nvfbc->load())
	{
		fprintf(stderr, "Unable to load the H.264 encoder\n");
		return -1;
	}

	//! Create the NvFBCH264 instance
	//! Here we specify the Hardware H.264 Encoder (Kepler)
	encoder = (NvFBCToH264HWEncoder *)nvfbc->create(NVFBC_TO_H264_HW_ENCODER, &maxWidth, &maxHeight);

	if(!encoder)
	{
		fprintf(stderr, "Unable to load the H.264 encoder\n");
		return -1;
	}
	//! Setup a buffer to put the encoded frame in
	outputBuffer = (unsigned char *)malloc(4048 * 4048);

	res = encoder->NvFBCH264SetUp(&fbch264SetupParams);

	if (res != NVFBC_SUCCESS)
	{
		fprintf(stderr, "Failed to setup H264 encoder.. exiting!\n");
		ret = -1;
		Cleanup();
		return -1;
	}
	sizeBytes = new NvU32();
	return ret;
}

int NVH264::ReInit(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig)
{
	Cleanup();
	NVFBCRESULT res = NVFBC_SUCCESS;

	Arguments.iBitrate = bitrate;
	Arguments.iProfile = profile;
	Arguments.eRateControl = eRateControl;
	//Arguments.iGOPLength = 0;//10*Arguments.iFPS;//Iframes every 10 seconds
	Arguments.iFPS =  fps;
	Arguments.ePresetConfig = ePresetConfig;


	int ret = 0;
	firstFrame = true;
	DWORD maxWidth = -1, maxHeight = -1;

	nvfbc = new NvFBCLibrary();
	memset(&encodeConfig, 0, sizeof(encodeConfig));
	memset(&fbch264SetupParams, 0, sizeof(fbch264SetupParams));

	encodeConfig.dwVersion = NVFBC_H264HWENC_CONFIG_VER;
	encodeConfig.dwProfile = Arguments.iProfile;// MAIN profile
	encodeConfig.dwFrameRateNum = Arguments.iFPS;
	encodeConfig.dwFrameRateDen = 1; // Set the target frame rate at iFPS/1

	//Play with these
	if (ePresetConfig == 4) {
		encodeConfig.dwVersion = NVFBC_H264HWENC_CONFIG_VER;
		encodeConfig.dwProfile = 100;

		encodeConfig.stereoFormat = NVFBC_H264_STEREO_NONE; // No stereo
		encodeConfig.ePresetConfig = NVFBC_H264_PRESET_LOSSLESS_HP;
		encodeConfig.eRateControl  = NVFBC_H264_ENC_PARAMS_RC_CONSTQP;
		encodeConfig.dwFrameRateNum = Arguments.iFPS;
		encodeConfig.dwFrameRateDen = 1; // Set the target frame rate at 30
		fbch264SetupParams.dwVersion = NVFBC_H264_SETUP_PARAMS_VER;
		fbch264SetupParams.bWithHWCursor = TRUE;
		fbch264SetupParams.pEncodeConfig = &encodeConfig;
	} else {
		encodeConfig.dwProfile = Arguments.iProfile;// MAIN profile
		encodeConfig.eRateControl = NVFBC_H264_ENC_PARAMS_RC_2_PASS_FRAMESIZE_CAP;// (NVFBC_H264_ENC_PARAMS_RC_MODE)Arguments.eRateControl;
		encodeConfig.dwSlicingMode = NVFBC_H264_ENC_SLICING_MODE_DISABLED;
		encodeConfig.dwAvgBitRate = Arguments.iBitrate;
		encodeConfig.dwPeakBitRate = (NvU32)(Arguments.iBitrate * 1); // Set the peak bitrate to 150% of the average
		encodeConfig.bEnableIntraRefresh = 1;
		//encodeConfig.dwMaxNumRefFrames = 0;//For Nvidia Default

		encodeConfig.dwFrameRateNum = Arguments.iFPS;
		encodeConfig.dwFrameRateDen = 1; // Set the target frame rate at 30

		encodeConfig.ePresetConfig = ( NVFBC_H264_PRESET )Arguments.ePresetConfig;
		encodeConfig.dwQP = 26; // Quantization parameter, between 0 and 51 
	}

	encodeConfig.bOutBandSPSPPS = false; // Use inband SPSPPS, if you need to grab headers on demand use outband SPSPPS

	encodeConfig.bRecordTimeStamps = FALSE; // Don't record timestamps
	encodeConfig.stereoFormat = NVFBC_H264_STEREO_NONE; // No stereo

	fbch264SetupParams.dwVersion = NVFBC_H264_SETUP_PARAMS_VER;
	fbch264SetupParams.bWithHWCursor = TRUE;
	fbch264SetupParams.pEncodeConfig = &encodeConfig;

	//! Load the NvFBC library.
	if(!nvfbc->load())
	{
		fprintf(stderr, "Unable to load the H.264 encoder\n");
		return -1;
	}

	//! Create the NvFBCH264 instance
	//! Here we specify the Hardware H.264 Encoder (Kepler)
	encoder = (NvFBCToH264HWEncoder *)nvfbc->create(NVFBC_TO_H264_HW_ENCODER, &maxWidth, &maxHeight);

	if(!encoder)
	{
		fprintf(stderr, "Unable to load the H.264 encoder\n");
		return -1;
	}
	//! Setup a buffer to put the encoded frame in
	outputBuffer = (unsigned char *)malloc(2048 * 2048);
	res = encoder->NvFBCH264SetUp(&fbch264SetupParams);

	if (res != NVFBC_SUCCESS)
	{
		fprintf(stderr, "Failed to setup H264 encoder.. exiting!\n");
		ret = -1;
		Cleanup();
		return -1;
	}
	sizeBytes = new NvU32();
	return ret;
}

VideoFrameNV NVH264::GrabFrameCompressed()
{
	static unsigned int frames = 0;

	NvFBCFrameGrabInfo grabInfo = {0};
	NVFBC_H264_GRAB_FRAME_PARAMS fbch264GrabFrameParams = {0};
	NvFBC_H264HWEncoder_FrameInfo frameInfo = {0};
	NvFBC_H264HWEncoder_EncodeParams fbch264EncodeFrameParams = {0};
	DWORD maxWidth = -1, maxHeight = -1;
	//DWORD maxWidth = 1280, maxHeight = 720;
	VideoFrameNV frame;
	frame.outputBuffer = NULL;
	frame.sizeBytes = 0;
	frame.isIFrame = 0;

	int ret = 0;
	NVFBCRESULT res = NVFBC_SUCCESS;
	memset(&grabInfo, 0, sizeof(grabInfo));
	memset(&frameInfo, 0, sizeof(frameInfo));
	memset(&fbch264GrabFrameParams, 0, sizeof(fbch264GrabFrameParams));
	memset(&fbch264EncodeFrameParams, 0, sizeof(fbch264EncodeFrameParams));
	fbch264GrabFrameParams.dwVersion = NVFBC_H264_GRAB_FRAME_PARAMS_VER;
	fbch264GrabFrameParams.dwFlags   = NVFBC_TOH264_NOWAIT;
	//fbch264GrabFrameParams.dwFlags=NVFBC_TOH264_NOFLAGS;
	fbch264EncodeFrameParams.dwVersion =  NVFBC_H264HWENC_ENCODE_PARAMS_VER;
	static int count = 0;

	if(count == 600)
	{
		count = 0;
		frame.isIFrame = 1;
		fbch264EncodeFrameParams.bStartIntraRefresh = 1;
		fbch264EncodeFrameParams.dwIntraRefreshCnt = 1;
	}
	count++;
	fbch264GrabFrameParams.pEncodeParams = &fbch264EncodeFrameParams;
	fbch264GrabFrameParams.pNvFBCFrameGrabInfo = &grabInfo;
	fbch264GrabFrameParams.pFrameInfo = &frameInfo;

	fbch264GrabFrameParams.pBitStreamBuffer = outputBuffer;
	//! Grab and encode the frame
	res = encoder->NvFBCH264GrabFrame(&fbch264GrabFrameParams);

	frame.outputBuffer = outputBuffer;
	frame.sizeBytes = frameInfo.dwByteSize;
	if (frame.isIFrame == 0)
		frame.isIFrame = frameInfo.bIsIFrame;

	//fwrite(frame.outputBuffer, frame.sizeBytes, 1, outputFile);
	if (res == NVFBC_SUCCESS)
	{
		if(0 == frameInfo.dwByteSize)
		{
			fprintf(stderr, "Stream ended...exiting!\n");
			ret = -1;
			return frame;
		}
		if(firstFrame)
		{
			NVFBC_H264_GET_HEADER_PARAMS headerParams;
			headerParams.pBuffer = ppssps_data;
			headerParams.pSize = &headerSize;

			res = encoder->NvFBCH264GetHeader(&headerParams);
			firstFrame = false;
		}
		if (res != NVFBC_SUCCESS)
		{
			fprintf(stderr, "Failed to get H264 header.. exiting!\n");
			ret = -1;
			Cleanup();
			return frame;
		}
		return frame;
	}
	else
	{
		if (res == NVFBC_ERROR_INVALIDATED_SESSION)
		{
			encoder->NvFBCH264Release();
			encoder = (NvFBCToH264HWEncoder *)nvfbc->create(NVFBC_TO_H264_HW_ENCODER, &maxWidth, &maxHeight);
			//! Setup the grab and encode
			res = encoder->NvFBCH264SetUp(&fbch264SetupParams);
			if (res == NVFBC_SUCCESS)
			{
				//! Grab and encode the frame
				res = encoder->NvFBCH264GrabFrame(&fbch264GrabFrameParams);
				return frame;
			}
		}
	}
}

// For capturing raw frames into memory; and software encoding
int NVH264::ReInitToMemory(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig,int width,int height,int threads)
{
	rawBuffer = NULL;
	Cleanup();
	NVFBCRESULT res = NVFBC_SUCCESS;

	Arguments.iBitrate = bitrate;
	Arguments.iProfile = profile;
	Arguments.eRateControl = eRateControl;
	Arguments.iGOPLength = 5*Arguments.iFPS;//Iframes every 10 seconds
	Arguments.iFPS =  fps;
	Arguments.ePresetConfig = ePresetConfig;
	Arguments.height = height;
	Arguments.width = width;
	Arguments.threads= threads;
	outputBuffer = (unsigned char *)malloc(2048 * 2048);

	int ret = 0;
	firstFrame = true;
	DWORD maxWidth = -1, maxHeight = -1;
	nvfbc = new NvFBCLibrary();

	//! Load the NvFBC library.
	if(!nvfbc->load())
	{
		fprintf(stderr, "Unable to load the H.264 encoder\n");
		return -1;
	}

	//! Create the NvFBCH264 instance
	//! Here we specify the Hardware H.264 Encoder (Kepler)
	sysFramer = (NvFBCToSys*) nvfbc->create(NVFBC_TO_SYS, &maxWidth, &maxHeight);

	NVFBC_TOSYS_SETUP_PARAMS setupParams = {0};
	setupParams.dwVersion = NVFBC_TOSYS_SETUP_PARAMS_VER;
	setupParams.bWithHWCursor = FALSE;
	setupParams.bDiffMap = FALSE;

	setupParams.eMode =   NVFBC_TOSYS_YYYYUV420p;
	setupParams.eMode = NVFBC_TOSYS_ARGB;
	setupParams.ppBuffer = (void **)&rawBuffer;
	setupParams.ppDiffMap = NULL;

	if(!sysFramer)
	{
		fprintf(stderr, "Unable to load the sysFramer encoder\n");
		return -1;
	}
	//! Setup a buffer to put the encoded frame in
	// outputBuffer = (unsigned char *)malloc(2048 * 2048);

	res = sysFramer->NvFBCToSysSetUp(&setupParams);

	if (res != NVFBC_SUCCESS)
	{
		fprintf(stderr, "Failed to setup H264 sysFramer.. exiting!\n");
		ret = -1;
		Cleanup();
		return -1;
	}
	sEncoder = new x264Encoder();
	sEncoder->initilize(bitrate,profile,eRateControl,fps,ePresetConfig,width,height,threads);
	return ret;
}

VideoFrameNV NVH264::GrabFrameRawToSys()
{
	VideoFrameNV frame;
	frame.outputBuffer = NULL;
	frame.sizeBytes = 0;

	NvFBCFrameGrabInfo frameGrabInfo;
	NVFBC_TOSYS_GRAB_FRAME_PARAMS fbcSysGrabParams= {0};
	fbcSysGrabParams.dwVersion = NVFBC_TOSYS_GRAB_FRAME_PARAMS_VER;
	//fbcSysGrabParams.dwFlags = NVFBC_TOSYS_NOFLAGS;
	fbcSysGrabParams.dwFlags = NVFBC_TOSYS_NOWAIT;
	fbcSysGrabParams.dwTargetWidth =Arguments.width;
	fbcSysGrabParams.dwTargetHeight = Arguments.height;
	fbcSysGrabParams.dwStartX = 0;
	fbcSysGrabParams.dwStartY = 0;
	fbcSysGrabParams.eGMode = NVFBC_TOSYS_SOURCEMODE_FULL;
	fbcSysGrabParams.pNvFBCFrameGrabInfo = &frameGrabInfo;

	NVFBCRESULT res;

	res = sysFramer->NvFBCToSysGrabFrame(&fbcSysGrabParams);

	static int frameNumber=0;
	frameNumber++;
	std::ostringstream convert;
	convert << "out-" << frameNumber <<".bmp";
	//@@ SaveARGB(convert.str().c_str(), rawBuffer,1280 ,720);

	//int size = sEncoder->encodeFrame(rawBuffer,outputBuffer);

	frame.outputBuffer = outputBuffer;
	//frame.sizeBytes = size;
	frame.sizeBytes = 0;
	return frame;
}

int NVH264::Cleanup()
{

	if (encoder)
	{
		free(outputBuffer);
		encoder->NvFBCH264Release();
		encoder=NULL;
	}
	else if(sysFramer)
	{
		sysFramer->NvFBCToSysRelease();
		sysFramer=NULL;
	}
	Sleep(1000);
	return 0;
}


// Benchmarking code
int NVH264::BenchMark(int numFrames,int fps )
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	double interval;
	FILE *outputH264;
	FILE *outputStats;
	NvFBCFrameGrabInfo grabInfo = {0};
	NVFBC_H264_GRAB_FRAME_PARAMS fbch264GrabFrameParams = {0};
	NvFBC_H264HWEncoder_FrameInfo frameInfo = {0};
	DWORD maxWidth = -1, maxHeight = -1;
	VideoFrameNV frame;
	frame.outputBuffer = NULL;
	frame.sizeBytes = 0;
	double timer;
	int ret = 0;
	std::ostringstream convert;
	convert << "FPS-" << Arguments.iFPS << "-Bitrate-" << Arguments.iBitrate << "-Profile-" << Arguments.iProfile  <<  "-Rate-Control-"  << Arguments.eRateControl << "-Preset-" << Arguments.ePresetConfig;
	outputH264 = fopen((convert.str()+".h264").c_str(), "wb");
	outputStats = fopen((convert.str()+".csv").c_str(), "wb");
	convert.str("");
	convert.clear();

	int currentFrames = 0;
	std::string encodeStats;
	double frameTime = 0;
	if (Arguments.iFPS > 0)//If we have a fps specfied great! Other wise we are going for a speed run.
		frameTime = 1.0/Arguments.iFPS;

	while(currentFrames < numFrames)
	{
		NVFBCRESULT res = NVFBC_SUCCESS;
		memset(&grabInfo, 0, sizeof(grabInfo));
		memset(&frameInfo, 0, sizeof(frameInfo));
		memset(&fbch264GrabFrameParams, 0, sizeof(fbch264GrabFrameParams));
		fbch264GrabFrameParams.dwVersion = NVFBC_H264_GRAB_FRAME_PARAMS_VER;
		fbch264GrabFrameParams.dwFlags   = NVFBC_TOH264_NOWAIT;

		//fbch264GrabFrameParams.dwFlags=NVFBC_TOH264_NOFLAGS;
		fbch264GrabFrameParams.pNvFBCFrameGrabInfo = &grabInfo;
		fbch264GrabFrameParams.pFrameInfo = &frameInfo;

		fbch264GrabFrameParams.pBitStreamBuffer = outputBuffer;
		//! Grab and encode the frame
		QueryPerformanceCounter(&start);
		res = encoder->NvFBCH264GrabFrame(&fbch264GrabFrameParams);
		QueryPerformanceCounter(&end);
		interval = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
		convert << currentFrames << "," << interval << "\n"; 
		timer = (static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart);
		while((interval+frameTime) > timer )
		{
			timer = (static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart);
			if ((interval+frameTime)-timer > 0.002)
			{
				Sleep(1);		
			}
			QueryPerformanceCounter(&end);
		}
		frame.outputBuffer = outputBuffer;
		frame.sizeBytes = frameInfo.dwByteSize;
		frame.isIFrame = frameInfo.bIsIFrame;

		fwrite(frame.outputBuffer, frame.sizeBytes, 1,outputH264);
		if (res == NVFBC_SUCCESS)
		{
			if(0 == frameInfo.dwByteSize)
			{
				fprintf(stderr, "Stream ended...exiting!\n");
				ret = -1;
				Cleanup();
				return ret;

			}
			if(firstFrame)
			{
				NVFBC_H264_GET_HEADER_PARAMS headerParams;
				headerParams.pBuffer = ppssps_data;
				headerParams.pSize = &headerSize;


				res = encoder->NvFBCH264GetHeader(&headerParams);

				firstFrame = false;
			}
			if (res != NVFBC_SUCCESS)
			{
				fprintf(stderr, "Failed to get H264 header.. exiting!\n");
				ret = -1;
				Cleanup();
				return ret;

			}
		}
		else
		{
			if (res == NVFBC_ERROR_INVALIDATED_SESSION)
			{
				encoder->NvFBCH264Release();
				encoder = (NvFBCToH264HWEncoder *)nvfbc->create(NVFBC_TO_H264_HW_ENCODER, &maxWidth, &maxHeight);
				//! Setup the grab and encode
				res = encoder->NvFBCH264SetUp(&fbch264SetupParams);
				if (res == NVFBC_SUCCESS)
				{
					//! Grab and encode the frame
					res = encoder->NvFBCH264GrabFrame(&fbch264GrabFrameParams);

				}
			}
		}
		currentFrames++;
	}
	fwrite(convert.str().c_str(), convert.str().size(), 1,outputStats);
	fclose(outputH264);
	fclose(outputStats);
	return ret;
}

int NVH264::BenchMarkSoftware(int numFrames,int fps )
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER start;
	LARGE_INTEGER mid;
	LARGE_INTEGER end;
	double interval;
	FILE *outputH264;
	FILE *outputStats;

	VideoFrameNV frame;
	frame.outputBuffer = NULL;
	frame.sizeBytes = 0;

	double timer;
	int ret = 0;
	std::ostringstream convert;
	convert << "S-FPS-" << Arguments.iFPS << "-Bitrate-" << Arguments.iBitrate << "-Profile-" << Arguments.iProfile  <<  "-Rate-Control-"  << Arguments.eRateControl << "-Preset-" << Arguments.ePresetConfig<<"-threads-" << Arguments.threads ;
	outputH264 = fopen((convert.str()+".h264").c_str(), "wb");
	outputStats = fopen((convert.str()+".csv").c_str(), "wb");
	convert.str("");
	convert.clear();

	int currentFrames = 0;
	std::string encodeStats;
	double frameTime = 0;
	if (Arguments.iFPS > 0)//If we have a fps specfied great! Other wise we are going for a speed run.
		frameTime = 1.0/Arguments.iFPS;

	while(currentFrames < numFrames)
	{	 

		NvFBCFrameGrabInfo frameGrabInfo;
		NVFBC_TOSYS_GRAB_FRAME_PARAMS fbcSysGrabParams= {0};
		fbcSysGrabParams.dwVersion = NVFBC_TOSYS_GRAB_FRAME_PARAMS_VER;
		//fbcSysGrabParams.dwFlags = NVFBC_TOSYS_NOFLAGS;
		fbcSysGrabParams.dwFlags = NVFBC_TOSYS_NOWAIT;
		fbcSysGrabParams.dwTargetWidth =Arguments.width;
		fbcSysGrabParams.dwTargetHeight = Arguments.height;
		fbcSysGrabParams.dwStartX = 0;
		fbcSysGrabParams.dwStartY = 0;
		fbcSysGrabParams.eGMode = NVFBC_TOSYS_SOURCEMODE_FULL;
		fbcSysGrabParams.pNvFBCFrameGrabInfo = &frameGrabInfo;

		NVFBCRESULT res;

		//! Grab and encode the frame
		QueryPerformanceCounter(&start);
		res = sysFramer->NvFBCToSysGrabFrame(&fbcSysGrabParams);
		QueryPerformanceCounter(&end);
		interval = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;


		convert << currentFrames << "," << interval; 
		QueryPerformanceCounter(&mid);
		int size = sEncoder->encodeFrame(rawBuffer,outputBuffer);

		QueryPerformanceCounter(&end);

		frame.outputBuffer = outputBuffer;
		frame.sizeBytes = size;

		interval = static_cast<double>(end.QuadPart - mid.QuadPart) / frequency.QuadPart;
		convert << currentFrames << "," << interval;
		interval = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
		convert << currentFrames << "," << interval << "\n"; 
		timer = (static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart);
		fwrite(frame.outputBuffer, frame.sizeBytes, 1,outputH264);
		while((interval+frameTime) > timer )
		{
			timer = (static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart);
			if ((interval+frameTime)-timer > 0.002)
			{
				Sleep(1);		
			}
			QueryPerformanceCounter(&end);
		}


		currentFrames++;
	}
	fwrite(convert.str().c_str(), convert.str().size(), 1,outputStats);
	fclose(outputH264);
	fclose(outputStats);
	return ret;
}

