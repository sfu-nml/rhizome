#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

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