#ifndef __RHI_INIT_MANAGER_H__
#define __RHI_INIT_MANAGER_H__

#include "RhiCommon.h"
#include "RhiServer.h"
#include "Rhix264Encoder.h"
#include "RhiNVH264.h"
#include "libconfig.h++"

class RhiInitManager {
private:
	int encoderType;
	rhiServerConfig serverConfig;
public:
	RhiInitManager();
	~RhiInitManager();
	
	void readServerParam();
	void readServerParamDefault();
	rhiServerConfig getServerConfig();

	void readEncoderParam();
	void readEncoderParamDefault();
	void getEncoderConfig();
};


#endif