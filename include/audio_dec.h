#ifndef ____AUDIO_DEC_H
#define ____AUDIO_DEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aacdecoder_lib.h>

#include <libobject/core/utils/dbg/debug.h>
#include <libobject/core/obj.h>

//参考部分ffmpeg fdk_aac设计
//author : wuyujie
//company: cloudsan


#define DMX_ANC_BUFFSIZE       128
#define DECODER_MAX_CHANNELS   6
#define DECODER_BUFFSIZE       2048 * sizeof(INT_PCM)


typedef enum fdk_aac_config_type_e{
    AAC_NORMAL          = 1, //只有ld eld 解码器特殊的处理
    AAC_LD              = 2,
    AAC_ELD             = 3
}config_type_t;

typedef struct audio_dec_s Audio_Dec;

struct audio_dec_s {
    Obj obj;

    int (*construct)(Audio_Dec *,char *init_str);
	int (*deconstruct)(Audio_Dec *);
	int (*set)(Audio_Dec *, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);     

    int (*init_decode_aac)(Audio_Dec *,config_type_t ext,TRANSPORT_TYPE type); // ext:0 normal 1:ld 2:eld
    int (*decode_aac)(Audio_Dec*,UCHAR *out_buf,int *out_size,UCHAR *in_buf,int len2);
    int (*decode_aac_frame)(Audio_Dec *,UCHAR *out_buf,int * got_frame,UCHAR *in,int len);
    void (*decode_aac_close)(Audio_Dec *);
    void (*decode_aac_flush)(Audio_Dec *);
    
    /*attributes*/
    HANDLE_AACDECODER  handle_aacencoder;
    TRANSPORT_TYPE     transport_type;
    UINT               conceal_method;
    CStreamInfo      * handle_stream_info;
    UINT               nlayers; 
    config_type_t      config_type;
};



#endif 