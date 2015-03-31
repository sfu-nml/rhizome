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

#include "RhiCommon.h"
#include "RhiInitManager.h"

class RhiInitManager;

class RhiServer
{
private:
	UsageEnvironment* env;
	rhiServerConfig rhi_config;
	RhiServer();
public:
	RhiServer(RhiInitManager*);
	~RhiServer();

	void startMediaServer();
};

#endif