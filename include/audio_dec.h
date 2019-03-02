#ifndef ____AUDIO_DEC_H
#define ____AUDIO_DEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aadecoder_lib.h>

#include <libobject/core/utils/dbg/debug.h>
#include <libobject/core/obj.h>



typedef struct audio_dec_s AUDIO_DEC;

struct audio_dec_s {
    Obj obj;

    int (*construct)(AUDIO_DEC *,char *init_str);
	int (*deconstruct)(AUDIO_DEC *);
	int (*set)(AUDIO_DEC *, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);     

    
    /* data */

};



#endif 