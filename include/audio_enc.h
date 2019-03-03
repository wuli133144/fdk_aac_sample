#ifndef ____AUDIO_DECODEC_H
#define ____AUDIO_DECODEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aacenc_lib.h>

#include <libobject/core/utils/dbg/debug.h>
#include <libobject/core/obj.h>


typedef struct adecodec_s ADecodec;

typedef enum bitrate_mode {
      BITRATE_MODE_STATIC = 0,
      BITRATE_MODE_VBR  
}bitrate_mode_t;

struct adecodec_s {
    Obj obj;

    int (*construct)(ADecodec *,char *init_str);
	int (*deconstruct)(ADecodec *);
	int (*set)(ADecodec *, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);     

    /*interface*/
    HANDLE_AACENCODER (*get_handle)(ADecodec *);

    int (*init_audio_encoder)(ADecodec *,int sample_rate,int channels);

    int (*encode_aac)(ADecodec *,
                      u_char *output,
                      int *out_size,
                      u_char *inbuf,
                      int in_size);

    void (*close_aac)(ADecodec *,
                      const AACENC_BufDesc *inBufDesc,
                      const AACENC_BufDesc *outBufDesc,
                      const AACENC_InArgs *inargs, 
                      AACENC_OutArgs *outargs);


    void (*set_bitrate_mode)(ADecodec *,bitrate_mode_t );
    void (*set_bitrate)(ADecodec *,int );
    void (*set_channel_mode)(ADecodec * ,int mode);
    void (*set_aot)(ADecodec * ,int aot);
    void (*set_sbr)(ADecodec * ,int sbr);

    /*attributes*/
    
    HANDLE_AACENCODER   handle_aacencoder;
    bitrate_mode_t      bitrate_mode;
    UINT                bitrate;
    UINT                per_sample_size ;
    UINT                channel_mode;
    UINT                aot;
    UINT                sbr;                  

};

#endif 