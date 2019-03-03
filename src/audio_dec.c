/**
 * @buffer Audio_Dec.c
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

#include "audio_dec.h"
//#include <libobject/conf/configfile.h>


static int __construct(Audio_Dec *codec,char *init_str)
{
    allocator_t *allocator   = ((Obj *)codec)->allocator;
    codec->conceal_method    = 2;
    codec->handle_aacencoder = NULL;
    return 0;
}

static int __deconstrcut(Audio_Dec *self)
{
    if (self->handle_aacencoder) {
        aacDecoder_Close(self->handle_aacencoder);
        self->handle_aacencoder = NULL;
    }
    return 0;
}

static int __set(Audio_Dec *codec, char *attrib, void *value)
{
    if (strcmp(attrib, "set") == 0) {
        codec->set = value;
    } else if (strcmp(attrib, "get") == 0) {
        codec->get = value;
    } else if (strcmp(attrib, "construct") == 0) {
        codec->construct = value;
    } else if (strcmp(attrib, "deconstruct") == 0) {
        codec->deconstruct = value;
    } else if (strcmp(attrib, "init_decode_aac") == 0) {
        codec->init_decode_aac = value;
    } else if (strcmp(attrib, "decode_aac") == 0) {
        codec->decode_aac = value;
    } else if (strcmp(attrib, "decode_aac_frame") == 0) {
        codec->decode_aac_frame = value;
    } else if (strcmp(attrib, "decode_aac_close") == 0) {
        codec->decode_aac_close = value;
    } else if (strcmp(attrib, "decode_aac_flush") == 0) {
        codec->decode_aac_flush = value;
    } 
    else {
        dbg_str(EV_DETAIL,"buffer set, not support %s setting",attrib);
    }

    return 0;
}

static void *__get(Audio_Dec *obj, char *attrib)
{
    if (strcmp(attrib, "") == 0) {
    } else {
        dbg_str(EV_WARNNING,"buffer get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __init_decode_aac(Audio_Dec *codec,config_type_t ext,TRANSPORT_TYPE type)
{
    int   ret                  = 0;
    UCHAR conf[4]              = {0};
    UINT  conf_mark            = 0;
    UINT  conf_len             = 0;
    int   err                  = AAC_DEC_OK;
    HANDLE_AACDECODER decoder  = NULL;

    codec->handle_aacencoder = aacDecoder_Open(type,1);

    if (!codec->handle_aacencoder) {
        dbg_str(DBG_ERROR,"aac decode open failed");
        goto error;
    }

    switch (ext)
    {
        case AAC_NORMAL:
            /* code */
            break;
        case AAC_LD:
            //0xBA, 0x88, 0x00, 0x00
            conf[0] = (UCHAR)0xBA;
            conf[1] = (UCHAR)0x88;
            conf[2] = (UCHAR)0x00;
            conf[3] = (UCHAR)0x00;
            conf_mark = 1;
            break;
        case AAC_ELD:
            //0xF8, 0xE8, 0x50, 0x00 
            conf[0] = (UCHAR)0xF8;
            conf[1] = (UCHAR)0xE8;
            conf[2] = (UCHAR)0x50;
            conf[3] = (UCHAR)0x00;
            conf_mark = 1;
            break;
        default:
            dbg_str(DBG_ERROR,"invalid config_type %d",ext);
            break;
    }

   
    UCHAR * config_coll[] = {conf};
    conf_len = sizeof(config_coll);
    
    
    switch (type)
    {
        case TT_MP4_RAW:
            /* code */
            if (ext != AAC_NORMAL)
                err = aacDecoder_ConfigRaw(codec->handle_aacencoder,config_coll,&conf_len);
            break;
        case TT_MP4_ADTS:
            break;
        default:
            break;
    }

    if (err != AAC_DEC_OK) {
        dbg_str(DBG_ERROR,"aac decode aacDecoder_ConfigRaw failed");
        goto error;
    }

    err = aacDecoder_SetParam(codec->handle_aacencoder, 
                              AAC_CONCEAL_METHOD,
                              codec->conceal_method);
    if (err != AAC_DEC_OK) {
        dbg_str(DBG_ERROR,"aac decode set canceal method failed");
        goto error;
    }

    err = aacDecoder_SetParam(codec->handle_aacencoder, 
                              AAC_PCM_MAX_OUTPUT_CHANNELS,
                              2);
    if (err != AAC_DEC_OK) {
        dbg_str(DBG_ERROR,"aac decode set max channels failed");
        goto error;
    }

    err = aacDecoder_SetParam(codec->handle_aacencoder, 
                              AAC_PCM_MIN_OUTPUT_CHANNELS,
                              1);
    if (err != AAC_DEC_OK) {
        dbg_str(DBG_ERROR,"aac decode set min channels failed");
        goto error;
    }

    codec->handle_stream_info = aacDecoder_GetStreamInfo(codec->handle_aacencoder);
    if (codec->handle_stream_info == NULL) {
        dbg_str(DBG_ERROR,"aac decode get streaminfo failed");
        goto error;
    }
    return 0;

error:
    if (codec->handle_aacencoder) {
        aacDecoder_Close(codec->handle_aacencoder);
        codec->handle_aacencoder = NULL;
    }
    return -1;
}

static void __decode_aac_close(Audio_Dec * codec) {
    if (codec->handle_aacencoder) {
        aacDecoder_Close(codec->handle_aacencoder);
        codec->handle_aacencoder = NULL;
    }
}

static int __decode_aac(Audio_Dec*codec,UCHAR *out_buf,int * outbuf_size,UCHAR *in_buf,int len2)
{
    int ret = 0;
    AAC_DECODER_ERROR err;
    UCHAR *input_buf[1] = {in_buf};
    UINT valid_size     = len2;
    UINT input_size     = len2;

    err = aacDecoder_Fill(codec->handle_aacencoder,input_buf,&input_size,&valid_size);
    if (err != AAC_DEC_OK) {
        dbg_str(DBG_ERROR,"aac decode fill failed!");
        ret = - 2;
        goto end;
    }

    err = aacDecoder_DecodeFrame(codec->handle_aacencoder,
                                 (INT_PCM*)out_buf,
                                // (DECODER_BUFFSIZE*DECODER_MAX_CHANNELS),
                                 2048,
                                 0);

    if (err == AAC_DEC_NOT_ENOUGH_BITS) { // 不够解一帧 loop
        *outbuf_size = 0;
        ret = len2 - valid_size;
        dbg_str(DBG_WARNNING,"not enough data");
        goto end;
    }

    if (err != AAC_DEC_OK) { //不知道的异常
        *outbuf_size = 0;
        ret = -3;
        dbg_str(DBG_ERROR,"decode frame error");
        goto end;
    }

    *outbuf_size = 1;
    ret = len2 - valid_size;

end:
    return ret;
}

static int __decode_aac_frame(Audio_Dec *codec,UCHAR *out_buf,int * got_frame,UCHAR *in,int len) {
    int ret = 0;
    ret = codec->decode_aac(codec,out_buf,got_frame,in,len);
    if (ret < 0) {
        return -1;
    } else {
        if (*got_frame == 0) {
            return 0;
        } 
        return 1;
    }

}

static class_info_entry_t audio_decoder_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Stream","parent",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","init_decode_aac",__init_decode_aac,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_FUNC_POINTER,"","decode_aac_close",__decode_aac_close,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_FUNC_POINTER,"","decode_aac_frame",__decode_aac_frame,sizeof(void *)},
    [8 ] = {ENTRY_TYPE_FUNC_POINTER,"","decode_aac",__decode_aac,sizeof(void *)},
    [9 ] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Audio_Dec",audio_decoder_class_info);

static int test_aac_dec_init(TEST_ENTRY *entry)
{
    int ret                 = 0;
    allocator_t * allocator = allocator_get_default_alloc();
    Audio_Dec * dec         = OBJECT_NEW(allocator,Audio_Dec,NULL);

    ret = dec->init_decode_aac(dec,1,TT_MP4_RAW);
    if (ret < 0) {
        dbg_str(DBG_ERROR,"init_audio_decode failed");
        goto error;
    }

    dbg_str(DBG_SUC,"init_audio_decode success");
    dec->decode_aac_close(dec);
  
    object_destroy(dec);
    return 1;
error:
    object_destroy(dec);
    return -1;
}
REGISTER_STANDALONE_TEST_FUNC(test_aac_dec_init);

static int test_aac_dec(TEST_ENTRY *entry)
{
    int ret                 = 0;
    int got_frame           = 0;
    int read_count          = 0;
    int index               = 0;
    int valid               = 0;
    int dec_count           = 0;
    UCHAR * p               = NULL;
    int frame_size          = 0;

    allocator_t * allocator = allocator_get_default_alloc();
    Audio_Dec * dec         = OBJECT_NEW(allocator,Audio_Dec,NULL);

    ret = dec->init_decode_aac(dec,1,TT_MP4_ADTS);
    if (ret < 0) {
        dbg_str(DBG_ERROR,"init_audio_decode failed");
        goto error;
    }

    dbg_str(DBG_SUC,"init_audio_decode success");
    UCHAR * output = malloc(sizeof(UCHAR)*4096);
    memset(output,0,4096);

    UCHAR buffer[1024];
    memset(buffer,0,1024);

    FILE * fp_in = fopen("tmp_out.aac","r");
    FILE * fp_out = fopen("tmp_out.pcm","wb+");
    p = output;

    while (!feof(fp_in)) {
        memset(buffer,0,1024);
        read_count = fread(buffer,1,1024,fp_in);

        if (ret < 1024) {
            if (ferror(fp_in)) {
                goto error;
            }
        }
        
        dec_count = dec->decode_aac(dec,p,&got_frame,buffer,read_count);

        if (dec_count < 0) {
            dbg_str(DBG_ERROR,"decode frame error");
            break;
        }
        
        p += read_count - dec_count;
        frame_size += read_count - dec_count;

        if (got_frame == 1) { //get complete a frame
            fwrite(output,1,frame_size,fp_out);
            dbg_str(DBG_SUC,"decode frame success size:%d",frame_size);
            p         = output;
            frame_size = 0;
            memset(output,0,4096);
        }
        
        dbg_str(DBG_SUC,"while loop %d  read_count:%d" ,index++,read_count);

    }

    dec->decode_aac_close(dec);
    if (output)
        free(output);

    if (fp_in)
        fclose(fp_in);
    if (fp_out)
        fclose(fp_out);
    return 1;
error:
    object_destroy(dec);
    free(output);
    fclose(fp_in);
    fclose(fp_out);
    return -1;
}
REGISTER_STANDALONE_TEST_FUNC(test_aac_dec);