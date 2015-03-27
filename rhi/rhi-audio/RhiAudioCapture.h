#ifndef __RHI_AUDIOCAPTURE_H__
#define __RHI_AUDIOCAPTURE_H__

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "WindowsAudioInputDevice_noMixer.hh"


class AudioCapture
{
public:
    AudioCapture(void);
    ~AudioCapture(void);

public:
    void initilize();

};
#endif