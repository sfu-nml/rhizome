#include "RhiH264LiveServerMediaSession.h"


H264LiveServerMediaSession* H264LiveServerMediaSession::createNew(UsageEnvironment& env, bool reuseFirstSource)
{
	  OutPacketBuffer::maxSize = 100000;
    return new H264LiveServerMediaSession(env, reuseFirstSource);
}

H264LiveServerMediaSession::H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource):OnDemandServerMediaSubsession(env,reuseFirstSource),fAuxSDPLine(NULL), fDoneFlag(0), fDummySink(NULL)
{

}


H264LiveServerMediaSession::~H264LiveServerMediaSession(void)
{
    delete[] fAuxSDPLine;
}


static void afterPlayingDummy(void* clientData)
{
    H264LiveServerMediaSession *session = (H264LiveServerMediaSession*)clientData;
    session->afterPlayingDummy1();
}

void H264LiveServerMediaSession::afterPlayingDummy1()
{
    envir().taskScheduler().unscheduleDelayedTask(nextTask());
    setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData)
{
    H264LiveServerMediaSession* session = (H264LiveServerMediaSession*)clientData;
    session->checkForAuxSDPLine1();
}

void H264LiveServerMediaSession::checkForAuxSDPLine1()
{

  char const* dasl;

  if (fAuxSDPLine != NULL) {
    // Signal the event loop that we're done:
    setDoneFlag();
  } else if (fDummySink  != NULL && (dasl = fDummySink ->auxSDPLine()) != NULL) {
    fAuxSDPLine = strDup(dasl);
    fDummySink  = NULL;

    // Signal the event loop that we're done:
    setDoneFlag();
  } else {
    // try again after a brief delay:
    int uSecsToDelay = 100000; // 100 ms
    nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)checkForAuxSDPLine, this);
  }
}

char const* H264LiveServerMediaSession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
{
 if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

  if (fDummySink  == NULL) { // we're not already setting it up for another, concurrent stream
    // Note: For H264 video files, the 'config' information ("profile-level-id" and "sprop-parameter-sets") isn't known
    // until we start reading the file.  This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
    // and we need to start reading data from our file until this changes.
    fDummySink  = rtpSink;

    // Start reading the file:
    fDummySink ->startPlaying(*inputSource, afterPlayingDummy, this);

    // Check whether the sink's 'auxSDPLine()' is ready:
    checkForAuxSDPLine(this);
  }

  envir().taskScheduler().doEventLoop(&fDoneFlag);

  return fAuxSDPLine;
}


FramedSource* H264LiveServerMediaSession::createNewStreamSource(unsigned clientSessionID, unsigned& estBitRate)
{
    // Based on encoder configuration i kept it 90000
    estBitRate = 900;
		//OutPacketBuffer::maxSize=1;
    LiveSourceWithx264 *source = LiveSourceWithx264::createNew(envir());
    // are you trying to keep the reference of the source somewhere? you shouldn't.  
    // Live555 will create and delete this class object many times. if you store it somewhere  
    // you will get memory access violation. instead you should configure you source to always read from your data source
	//H264VideoStreamFramer *source2 = H264VideoStreamFramer::createNew(envir(),source,false);
	
	//return source2;
   return H264VideoStreamDiscreteFramer::createNew(envir(),source);
}

RTPSink* H264LiveServerMediaSession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	const char* header = (const char*) malloc(sizeof(unsigned char)*NVH264::headerSize);
	strncpy((char*)header,(const char*)NVH264::ppssps_data,NVH264::headerSize);
	std::cout << std::hex << header << std::endl;

	H264VideoRTPSink* sink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic,header);
	

    return sink;
}