#include "RhiLiveSourceWithx264.h"
using std::cout;
using std::endl;

LiveSourceWithx264* LiveSourceWithx264::createNew(UsageEnvironment& env)
{
	return new LiveSourceWithx264(env);
}

EventTriggerId LiveSourceWithx264::eventTriggerId = 0;

unsigned LiveSourceWithx264::referenceCount = 0;

LiveSourceWithx264::LiveSourceWithx264(UsageEnvironment& env):FramedSource(env)
{
	if(referenceCount == 0)
	{
		firstFrame = true;
		softwareMode=false;
		raw=false;
		encoder = new NVH264();
		if (softwareMode || raw)
		encoder->ReInitToMemory(4000000,77,2,60,0,1280,720,8);//8 mbits, main profile, cbr,fps fps,high performance preset
	else
		encoder->ReInit(2000000,77,2,30,0);

}
++referenceCount;
if(eventTriggerId == 0)
{
	eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
}
cout << "Rhi -> Constructing an x264 StreamFramer:" << (unsigned long int) this << endl; 

}

LiveSourceWithx264::~LiveSourceWithx264(void)
{
	cout << "Destroyed Stream framer:" << (unsigned long int) this << endl; 
	--referenceCount;
	encoder->Cleanup();
	envir().taskScheduler().deleteEventTrigger(eventTriggerId);
	eventTriggerId = 0;
}

void LiveSourceWithx264::deliverFrame0(void* clientData)
{
	((LiveSourceWithx264*)clientData)->deliverFrame();
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
// Define a structure to receive the current Windows filetime
	FILETIME ft;

// Initialize the present time to 0 and the timezone to UTC
	unsigned __int64 tmpres = 0;
	static int tzflag = 0;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

// The GetSystemTimeAsFileTime returns the number of 100 nanosecond 
// intervals since Jan 1, 1601 in a structure. Copy the high bits to 
// the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

// Convert to microseconds by dividing by 10
		tmpres /= 10;

// The Unix epoch starts on Jan 1 1970.  Need to subtract the difference 
// in seconds from Jan 1 1601.
		tmpres -= DELTA_EPOCH_IN_MICROSECS;

// Finally change microseconds to seconds and place in the seconds value. 
// The modulus picks up the microseconds.
		tv->tv_sec = (long)((tmpres) / 1000000UL);
		tv->tv_usec = (long)((tmpres) % 1000000UL);
	}


	return 0;
} 

unsigned __int64 getMicroTime()
{
// Define a structure to receive the current Windows filetime
	FILETIME ft;

// Initialize the present time to 0 and the timezone to UTC
	unsigned __int64 tmpres = 0;
	static int tzflag = 0;


	GetSystemTimeAsFileTime(&ft);

// The GetSystemTimeAsFileTime returns the number of 100 nanosecond 
// intervals since Jan 1, 1601 in a structure. Copy the high bits to 
// the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
	tmpres |= ft.dwHighDateTime;
	tmpres <<= 32;
	tmpres |= ft.dwLowDateTime;

// Convert to microseconds by dividing by 10
	tmpres /= 10;

// The Unix epoch starts on Jan 1 1970.  Need to subtract the difference 
// in seconds from Jan 1 1601.
	tmpres -= DELTA_EPOCH_IN_MICROSECS;

// Finally change microseconds to seconds and place in the seconds value. 
// The modulus picks up the microseconds.



	return tmpres;
} 


int increaseTime(struct timeval *tv,unsigned int interval)
{

// Initialize the present time to 0 and the timezone to UTC
	unsigned __int64 tmpres = tv->tv_sec*1000000+tv->tv_usec+interval;



// Finally change microseconds to seconds and place in the seconds value. 
// The modulus picks up the microseconds.
	tv->tv_sec = (long)((tmpres) / 1000000UL);
	tv->tv_usec = (long)((tmpres) % 1000000UL);

	return 0;
} 

void LiveSourceWithx264::doGetNextFrame()
{
	
	if(firstFrame) 
	{
		gettimeofday(&fPresentationTime,NULL);
		nextFrame=getMicroTime();

		firstFrame=false;
	}
	
	deliverFrame();
}


void LiveSourceWithx264::deliverFrame()
{
	fDurationInMicroseconds = 15000;
	nextFrame = nextFrame+fDurationInMicroseconds;

	NVH264::VideoFrame currentFrame;

	if (softwareMode)
		currentFrame = encoder->GrabFrameRawToSys();
	else if(raw)
	{
		currentFrame.sizeBytes=0;
	}
	else
		currentFrame = encoder->GrabFrameCompressed();

	fFrameSize = currentFrame.sizeBytes;
	int trancate = 4;

	if(currentFrame.sizeBytes > fMaxSize)
	{
		fFrameSize = fMaxSize-trancate-NVH264::headerSize;
		fNumTruncatedBytes =fFrameSize - fMaxSize;

		if(currentFrame.sizeBytes > 15000)
		{
			fFrameSize = fFrameSize-NVH264::headerSize;
		}
	}
	else
	{
		fFrameSize = currentFrame.sizeBytes;
	}
	 //fwrite(currentFrame.outputBuffer, currentFrame.sizeBytes, 1, outputFile);
	
	//increaseTime(&fPresentationTime,fDurationInMicroseconds);
	
	gettimeofday(&fPresentationTime,NULL);
	 //fPresentationTime = currentTime;
	 //std::cout << "Encoded Frame: " << (fPresentationTime.tv_sec) << std::endl;
	 //std::cout << "Encoded Frame: " << (size_t)currentFrame.sizeBytes << std::endl;
	  //fwrite(currentFrame.outputBuffer, currentFrame.sizeBytes, 1, outputFile);
	static int count = 0;
	if (softwareMode)
	{
		memcpy(fTo,(currentFrame.outputBuffer+trancate),fFrameSize);
		fFrameSize=fFrameSize;
	}
	else
	{
		if(currentFrame.isIFrame == 1 )

		{
			count++;
			trancate=4;
			memcpy(fTo,(NVH264::ppssps_data+trancate),NVH264::headerSize-trancate);
			memcpy(fTo+(NVH264::headerSize-trancate),(currentFrame.outputBuffer),(currentFrame.sizeBytes));
			fFrameSize=(NVH264::headerSize-trancate)+(currentFrame.sizeBytes);
		}
		else
		{
			trancate=4;
			memcpy(fTo,(currentFrame.outputBuffer+trancate),currentFrame.sizeBytes-trancate);
			fFrameSize=currentFrame.sizeBytes-trancate;
		}
	}
	FramedSource::afterGetting(this);
}  