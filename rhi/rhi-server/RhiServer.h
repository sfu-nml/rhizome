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

class RhiServer
{
private:
	UsageEnvironment* env;
public:
	RhiServer();
	~RhiServer();

	int startMediaServer();
};

#endif