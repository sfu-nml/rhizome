#ifndef __RHI_INIT_MANAGER_H__
#define __RHI_INIT_MANAGER_H__

/* Rhizome initialization manager header file.
 * This manager is in charge of managing reading the configuration from .cfg files
 * Other modules should read their configuration from this manager instead of reading
 * from the files themselves.
 *
 * Copyright (C) 2015 	 
 *
 * This code is part of Rhizome.	
 */

#include "RhiCommon.h"
#include "RhiServer.h"
#include "Rhix264Encoder.h"
#include "RhiNVH264.h"
#include "libconfig.h++"

class RhiInitManager {
private:
	int encoderType;
	rhiServerConfig serverConfig;
	rhiEncoderConfig encoderConfig;
	libconfig::Config cfg;
public:
	RhiInitManager();
	~RhiInitManager();
	
	void readParamAll();
	bool readConfigFile();

	// Server parameters reading utilities
	void readServerParam();
	void readServerParamDefault();
	rhiServerConfig getServerConfig();

	// Encoder parameters reading utilities
	void readEncoderParam();
	void readEncoderParamDefault();
	rhiEncoderConfig getEncoderConfig();

	// Unit test
	void rhiInitMgrTest();
};


#endif