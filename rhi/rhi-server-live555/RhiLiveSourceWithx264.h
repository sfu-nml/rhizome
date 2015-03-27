#ifndef __RHISERVER_SOURCE_H__
#define __RHISERVER_SOURCE_H__

#include <queue>
#include <time.h>
#include <H264VideoStreamFramer.hh>
#include "RhiNVH264.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
 
#define DEBUG 1
#define RTSP_SERVER_VERBOSITY_LEVEL 1
class LiveSourceWithx264:public FramedSource
{
public:
    static LiveSourceWithx264* createNew(UsageEnvironment& env);
    static EventTriggerId eventTriggerId;
protected:
    LiveSourceWithx264(UsageEnvironment& env);
    virtual ~LiveSourceWithx264(void);
private:
    virtual void doGetNextFrame();
    static void deliverFrame0(void* clientData);
    void deliverFrame();
    static unsigned referenceCount;
	bool firstFrame;
    timeval currentTime;
	unsigned int timeStamp;
    NVH264 *encoder;
	FILE *outputFile;
	timeval fLastPlayTime;
	unsigned __int64 nextFrame;
	unsigned __int64 lastFrame;
	bool softwareMode;
	bool raw;
}; 

#endif