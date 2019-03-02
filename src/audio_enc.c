/**
 * @buffer ADecodec.c
 * @Synopsis  
 * @author alan lin
 * @version 
 * @date 2019-01-13
 */
/* Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdio.h>
#include <libobject/core/utils/config/config.h>
#include <libobject/core/utils/timeval/timeval.h>
#include <libobject/core/utils/dbg/debug.h>
#include <libobject/event/event_base.h>
#include <libobject/core/utils/registry/registry.h>

#include <audio_enc.h>
#include <libobject/conf/configfile.h>


#define BITRATE_DEFAULT    24000

static int __construct(ADecodec *codec,char *init_str)
{
    allocator_t *allocator = ((Obj *)codec)->allocator;
    codec->handle_aacencoder = NULL;
    codec->bitrate           = BITRATE_DEFAULT;
    codec->bitrate_mode      = BITRATE_MODE_VBR;
    codec->channel_mode      = MODE_2;
    codec->aot               = 2;
    codec->sbr               = 0;

    return 0;
}

static int __deconstrcut(ADecodec *self)
{
    //self->close_aac();
    if (self->handle_aacencoder)
        aacEncClose(&self->handle_aacencoder);
    return 0;
}

static int __set(ADecodec *codec, char *attrib, void *value)
{
    if (strcmp(attrib, "set") == 0) {
        codec->set = value;
    } else if (strcmp(attrib, "get") == 0) {
        codec->get = value;
    } else if (strcmp(attrib, "construct") == 0) {
        codec->construct = value;
    } else if (strcmp(attrib, "deconstruct") == 0) {
        codec->deconstruct = value;
    } else if (strcmp(attrib, "init_audio_encoder") == 0) {
        codec->init_audio_encoder = value;
    } else if (strcmp(attrib, "init_audio_encoder") == 0) {
        codec->init_audio_encoder = value;
    } else if (strcmp(attrib, "encode_aac") == 0) {
        codec->encode_aac = value;
    } else if (strcmp(attrib, "close_aac") == 0) {
        codec->close_aac = value;
    } else if (strcmp(attrib, "get_handle") == 0) {
        codec->get_handle = value;
    } else if (strcmp(attrib, "set_bitrate") == 0) {
        codec->set_bitrate = value;
    } else if (strcmp(attrib, "set_bitrate_mode") == 0) {
        codec->set_bitrate_mode = value;
    } else if (strcmp(attrib, "set_channel_mode") == 0) {
        codec->set_channel_mode = value;
    } else if (strcmp(attrib, "set_srb") == 0) {
        codec->set_srb = value;
    } else if (strcmp(attrib, "set_aot") == 0) {
        codec->set_aot = value;
    }
    else {
        dbg_str(EV_DETAIL,"buffer set, not support %s setting",attrib);
    }

    return 0;
}

static void *__get(ADecodec *obj, char *attrib)
{
    if (strcmp(attrib, "") == 0) {
    } else {
        dbg_str(EV_WARNNING,"buffer get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __init_audio_encoder(ADecodec * self,int sample_rate,int channels)
{
    //AACENC_InfoStruct
    int ret     = 0;
    int bitrate = 24000;
    int aot     = self->aot;
    int sbr     = self->sbr;
    int mode    = self->channel_mode;

    CHANNEL_MODE channelmode = MODE_2;
    HANDLE_AACENCODER handle = NULL;
    AACENC_InfoStruct info   = {0};
    
    switch (mode)
    {
        case 1:
            mode = MODE_1;
            break;
        case 2:
            mode = MODE_2;
            break;
        default:
            break;
    }

    if (aacEncOpen(&self->handle_aacencoder,0,channels) != AACENC_OK) {
        dbg_str(DBG_ERROR,"aacEncOpen failed");
        return -1;
    }

    handle = self->handle_aacencoder;

    if (aacEncoder_SetParam(handle,AACENC_AOT,aot) != AACENC_OK) {
        dbg_str(DBG_ERROR,"aacEncoder_SetParam AOT failed handle addr:%p aot:%d",handle,aot);
        return -1;
    }

    if (aot == 39 && sbr) {
        if (aacEncoder_SetParam(handle,AACENC_SBR_MODE,1) != AACENC_OK) {
            dbg_str(DBG_ERROR,"aacEncoder_SetParam sbr failed handle addr:%p aot:%d",handle,aot);
            return -1;
        }
    }
    //sample_rate
    if (aacEncoder_SetParam(handle,AACENC_SAMPLERATE,sample_rate) != AACENC_OK) {
        dbg_str(DBG_ERROR,"aacEncoder_SetParam sample_rate failed handle:%p");
        return -1;
    }
    //channel mode
    if (aacEncoder_SetParam(handle,AACENC_CHANNELMODE,mode) != AACENC_OK) {
        dbg_str(DBG_ERROR,"aacEncoder_SetParam channel mode failed");
        return -1;
    }
   //设置channel loader adapter wav
    if (aacEncoder_SetParam(handle,AACENC_CHANNELORDER,1) != AACENC_OK) {
        dbg_str(DBG_ERROR,"aacEncoder_SetParam channel loader failed");
        return -1;
    }
    //设置bitrate 
    if (self->bitrate_mode == BITRATE_MODE_STATIC) {
       if (aacEncoder_SetParam(handle,AACENC_BITRATEMODE,0) != AACENC_OK) {
            dbg_str(DBG_ERROR,"aacEncoder_SetParam AACENC_BITRATEMODE failed");
            return -1;
        }

        if (aacEncoder_SetParam(handle,AACENC_BITRATE,self->bitrate) != AACENC_OK) {
            dbg_str(DBG_ERROR,"aacEncoder_SetParam AACENC_BITRATE failed");
            return -1;
        }
    } else {
        if (aacEncoder_SetParam(handle,AACENC_BITRATEMODE,1) != AACENC_OK) {
            dbg_str(DBG_ERROR,"aacEncoder_SetParam AACENC_BITRATEMODE failed");
            return -1;
        } 
    }

    if (aacEncoder_SetParam(handle, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK) {
		dbg_str(DBG_ERROR, "Unable to set the ADTS transmux");
		return -1;
	}

    if (aacEncEncode(handle,NULL,NULL,NULL,NULL) != AACENC_OK) {
        dbg_str(DBG_ERROR, "Unable to initilize handle");
		return -1;
    }

    if (aacEncInfo(handle,&info) != AACENC_OK) {
        dbg_str(DBG_ERROR, "Unable to initilize INFO struct");
		return -1;
    }

    self->per_sample_size = info.frameLength*channels*2;

    dbg_str(DBG_SUC, "audio frame_size:%d input channels:%d",info.frameLength,info.inputChannels);
    return 0; 
}   

static void __set_channel_mode(ADecodec * self,int mode)
{
    self-> channel_mode = mode;
}

static void __set_sbr(ADecodec * self,int sbr)
{
    self-> sbr = srb;
}

static void __set_aot(ADecodec * self,int aot)
{
    self->aot = aot;
}

static int __encode_aac(ADecodec * self,u_char * output_buf,int *output_size,u_char * input_buf,int input_size)
{
    AACENC_BufDesc in_buf  = {0};
    AACENC_BufDesc out_buf = {0};
    AACENC_InArgs  in_arg  = {0};
    AACENC_OutArgs out_arg = {0};

    int in_identifier = IN_AUDIO_DATA;
    int in_size = 0;
    int in_element_size = 0;

    int out_identifier = OUT_BITSTREAM_DATA;
    int out_size = 0;
    int out_element_size = 0;

    void *in_ptr ,*out_ptr;
    AACENC_ERROR err;

    in_ptr          =  input_buf;
    in_size         =  input_size;
    in_element_size = 1;

    in_arg.numInSamples      = input_size;
    in_buf.numBufs           = 1;
    in_buf.bufs              = &in_ptr;
    in_buf.bufferIdentifiers = &in_identifier;
    in_buf.bufElSizes        = &in_element_size;
    in_buf.bufSizes          = &in_size;

    out_ptr  = output_buf;
    out_size = *output_size;
    out_element_size = 1;

    out_buf.numBufs           = 1;
    out_buf.bufs              = &out_ptr;
    out_buf.bufferIdentifiers = &out_identifier;
    out_buf.bufSizes          = &out_size;
    out_buf.bufElSizes        = &out_element_size;
    
    err = aacEncEncode(self->handle_aacencoder,&in_buf,&out_buf,&in_arg,&out_arg);
    if (err != AACENC_OK) {
        if (err != AACENC_ENCODE_EOF) {
            dbg_str(DBG_ERROR,"aac encode failed!");
            return -1;
        } 
    }

    dbg_str(DBG_SUC,"aac encode success! valid out:%d",out_arg.numOutBytes);

    return out_arg.numOutBytes;

}

static class_info_entry_t audio_codec_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Stream","parent",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","set_bitrate",NULL,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_FUNC_POINTER,"","set_bitrate_mode",NULL,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_FUNC_POINTER,"","get_handle",NULL,sizeof(void *)},
    [8 ] = {ENTRY_TYPE_FUNC_POINTER,"","encode_aac",__encode_aac,sizeof(void *)},
    [9 ] = {ENTRY_TYPE_FUNC_POINTER,"","close_aac",NULL,sizeof(void *)},
    [10 ] = {ENTRY_TYPE_FUNC_POINTER,"","init_audio_encoder",__init_audio_encoder,sizeof(void *)},
    [11 ] = {ENTRY_TYPE_FUNC_POINTER,"","set_channel_mode",__set_channel_mode,sizeof(void *)},
    [12 ] = {ENTRY_TYPE_FUNC_POINTER,"","set_aot",__set_aot,sizeof(void *)},
    [13 ] = {ENTRY_TYPE_FUNC_POINTER,"","set_sbr",__set_sbr,sizeof(void *)},
    [14] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("ADecodec",audio_codec_class_info);

static int test_ADecodec(TEST_ENTRY *entry)
{
    int ret = 0;
    allocator_t *allocator = allocator_get_default_alloc();
    
    void * sample_rate ,* bitrate,*channel,*bitrate_mode = NULL;

    ADecodec   * codec = OBJECT_NEW(allocator,ADecodec,NULL);
    ConfigFile *cfg    = OBJECT_NEW(allocator,ConfigFile,NULL);
    
    ret = cfg->load_file(cfg,"encode.conf");

    if (ret < 0) {
        goto error;
    }
    
    if (((ret = cfg->get_value(cfg,"sample_rate",(void **)&sample_rate) >= 0)) &&
        ((ret = cfg->get_value(cfg,"bitrate",(void **)&bitrate) >= 0))         &&
        ((ret = cfg->get_value(cfg,"channel",(void **)&channel) >= 0))         &&
        ((ret = cfg->get_value(cfg,"bitrate_mode",(void **)&bitrate_mode)>= 0))        
       ) {
            
            char * sample   = (char *)sample_rate;
            char * channels = (char*)channel;

            dbg_str(DBG_SUC," %d %d",atoi(sample),atoi(channels));

            ret = codec->init_audio_encoder(codec,atoi(sample),atoi(channels));

            if (ret >= 0) {
                dbg_str(DBG_SUC,"create acodec success");
            } else {
                dbg_str(DBG_ERROR,"INIT audio encode failed");
            }
       } else {
                dbg_str(DBG_ERROR,"get config key-value failed! ");
       }

error:
    object_destroy(codec);
    object_destroy(cfg);

    return 1;
}
REGISTER_STANDALONE_TEST_FUNC(test_ADecodec);

static int test_encode_pcm_2_aac(TEST_ENTRY * entry)
{
    int ret = 0;
    int read_count = 0;
    void * sample_rate ,* bitrate,*channel,*bitrate_mode = NULL;

    allocator_t *allocator = allocator_get_default_alloc();
    ADecodec   * codec = OBJECT_NEW(allocator,ADecodec,NULL);

    codec->init_audio_encoder(codec,44100,2);

    FILE * in_raw_pcm =  fopen("./out.pcm","r");
    FILE * out_raw_aac = fopen("./tmp_out.aac","wb+");
    
    if (in_raw_pcm == NULL || out_raw_aac == NULL) {
        goto error;
    }

    u_char * buf = malloc(codec->per_sample_size);
    memset(buf,0,codec->per_sample_size);
    
    u_char out_buf[4096];
    memset(out_buf,0,4096);
    int output_size = 4096;

    while (!feof(in_raw_pcm)) {

        read_count = fread(buf,1,codec->per_sample_size,in_raw_pcm);

        if (read_count < codec->per_sample_size) {
            if (ferror(in_raw_pcm)) {
                dbg_str(DBG_ERROR,"fread error happen");
                goto error;
            }
        }
        
        dbg_str(DBG_ERROR,"fdk-aac encode before");

        ret = codec->encode_aac(codec,out_buf,&output_size,buf,read_count);
        if (ret < 0) {
            dbg_str(DBG_ERROR,"fdk-aac encode error happen");
            goto error;
        } else if (ret == 0) {
            continue;
        }

        dbg_str(DBG_ERROR,"fdk-aac encode outbuf size:%d",ret);

        fwrite(out_buf,1,ret,out_raw_aac);
        memset(buf,0,codec->per_sample_size);
        memset(out_buf,0,4096);

    }

    fclose(in_raw_pcm);
    fclose(out_raw_aac);
    object_destroy(codec);
    free(buf);
    return 0;

error:
    if(in_raw_pcm) {
        fclose(in_raw_pcm);
    }
    if (out_raw_aac) {
        fclose(out_raw_aac);
    }
    free(buf);
    object_destroy(codec);
    return -1;
}
REGISTER_STANDALONE_TEST_FUNC(test_encode_pcm_2_aac);
