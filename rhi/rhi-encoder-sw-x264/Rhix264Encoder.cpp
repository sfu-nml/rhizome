#include "Rhix264Encoder.h"


x264Encoder::x264Encoder(void)
{

}


x264Encoder::~x264Encoder(void)
{

}

void x264Encoder::initilize(int bitrate,int profile,int eRateControl,int fps,int ePresetConfig,int width,int height,int threads)
{
	
	x264_param_default_preset(&parameters, "veryfast", "zerolatency");

    parameters.i_log_level = X264_LOG_INFO;
    parameters.i_threads = threads;
    parameters.i_width = width;
    parameters.i_height = height;
    parameters.i_fps_num = fps;
    parameters.i_fps_den = 1;
    parameters.i_keyint_max = parameters.i_fps_num;
    parameters.b_intra_refresh = 1;
    parameters.rc.i_rc_method = X264_RC_CRF;
	  parameters.rc.i_bitrate=bitrate;
    parameters.rc.i_vbv_buffer_size = 1.50*bitrate;
    parameters.rc.i_vbv_max_bitrate = 1.50*bitrate;
    parameters.rc.f_rf_constant = 25;
    parameters.rc.f_rf_constant_max = 35;
    parameters.i_sps_id = 7;
    // the following two value you should keep 1
    parameters.b_repeat_headers = 1;    // to get header before every I-Frame
    parameters.b_annexb = 1; // put start code in front of nal. we will remove start code later
	if(profile == 100)
	{
    x264_param_apply_profile(&parameters, "high");
	}
	else if(profile == 77)
	{
		x264_param_apply_profile(&parameters, "main");
	}
	else if(profile == 66)
	{
	x264_param_apply_profile(&parameters, "base");
	}



    encoder = x264_encoder_open(&parameters);
    x264_picture_alloc(&picture_in, X264_CSP_I420, parameters.i_width, parameters.i_height);
	x264_picture_alloc(&picture_out, X264_CSP_I420, parameters.i_width, parameters.i_height);
	  picture_in.img.i_plane = 3;
	frame_num = 0;
	nal = NULL;
	//convertContext = sws_getContext(parameters.i_width, parameters.i_height, AV_PIX_FMT_RGB24, parameters.i_width , parameters.i_height, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
}

void x264Encoder::unInitilize()
{
    x264_encoder_close(encoder);
   // sws_freeContext(convertContext);
}

unsigned int x264Encoder::encodeFrame(unsigned char *buf,unsigned char *outBuffer)
{


size_t nbytes_y = (parameters.i_width *parameters.i_height);
size_t nbytes_uv = nbytes_y / 4;

 

 
  picture_in.img.plane[0] = buf;
  picture_in.img.plane[1] = &buf[nbytes_y];
  picture_in.img.plane[2] = &buf[nbytes_y + nbytes_uv];
 
  picture_in.img.i_stride[0] = parameters.i_width;
  picture_in.img.i_stride[1] = parameters.i_width >> 1;
  picture_in.img.i_stride[2] = parameters.i_width >> 1;
  picture_in.i_pts = frame_num;
  frame_num++;
 
  
  int nals_count = 0;
  int frame_size = x264_encoder_encode(encoder, &nal, &nals_count, &picture_in, &picture_out);
   if(frame_size < 0) {
    printf("error: x264_encoder_encode failed.\n");
    return false;
  }
 
  if(!nal) {
    printf("error: x264_encoder_encode returned no valid nals.\n");
    return false;
  }
   memcpy(outBuffer, nal->p_payload, frame_size);

  return frame_size;
  
	/*
    int srcstride = parameters.i_width*parameters.i_height;
    sws_scale(convertContext, &surfaceData, &srcstride,0, 768, picture_in.img.plane, picture_in.img.i_stride);
    x264_nal_t* nals = NULL;
    int i_nals = 0;
    int frame_size = -1;


    frame_size = x264_encoder_encode(encoder, &nals, &i_nals, &picture_in, &picture_out);

    static bool finished = false;

    if (frame_size >= 0)
    {
    static bool alreadydone = false;
    if(!alreadydone)
    {

        x264_encoder_headers(encoder, &nals, &i_nals);
        alreadydone = true;
    }
    for(int i = 0; i < i_nals; ++i)
    {
        outputQueue.push(nals[i]);
    }
    }
    delete [] surfaceData;
    surfaceData = nullptr;
	*/

}


