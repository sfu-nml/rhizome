/* Rhizome Server header file.
*
* Copyright (C) 2015 	 
*
* This code is part of Rhizome.	
*/

#ifndef __RHISERVER_H__
#define __RHISERVER_H__

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "OnDemandServerMediaSubsession.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "WindowsAudioInputDevice_noMixer.hh"
#include "RhiLiveSourceWithx264.h"
#include "libconfig.h++"

#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "RhiCommon.h"

class RhiServer
{
private:
	UsageEnvironment* env;
	struct config {
		unsigned int outPacketMaxSize;
		unsigned short rtpPortNum;
		unsigned short rtcpPortNum;
		unsigned char ttl;
		unsigned estimatedSessionBandwidth;
		unsigned serverPort;
		unsigned int payloadFormat;
		std::string streamName;
	} rhi_config;
public:
	RhiServer();
	~RhiServer();

	void startMediaServer();
	void serverInitialize();
	void initializeWithDefault();
};

#endif