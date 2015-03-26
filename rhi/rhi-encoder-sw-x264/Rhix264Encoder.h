#ifndef FFH264Encoder_H
#define FFH264Encoder_H

#ifdef __cplusplus
#define __STDINT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <iostream>
#include <concurrent_queue.h>
#include <queue>

extern "C" {
#include <stdint.h>
#include <inttypes.h>
#include <x264.h>
#include <libswscale\swscale.h>

}

class x264Encoder
{
public:
    x264Encoder(void);
    ~x264Encoder(void);

public:
    void initilize(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig,int width,int height,int threads);
    void unInitilize();
    unsigned int encodeFrame(unsigned char*,unsigned char *);
private:
    // Use this context to convert your BGR Image to YUV image since x264 do not support RGB input
    SwsContext* convertContext;
     x264_nal_t* nal;
    x264_param_t parameters;
    x264_picture_t picture_in,picture_out;
    x264_t* encoder;
	int  frame_num;
};
#endif