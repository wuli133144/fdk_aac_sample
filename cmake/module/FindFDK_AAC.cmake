find_path(FDK_AAC_INCLUDE_DIR 
          NAMES aacdecoder_lib.h  aacenc_lib.h
          HINTS  
          /usr/local/include 
          /usr/local/
          PATH_SUFFIXES include/fdk-aac 
          DOC "find sdl header files..."
          #NO_DEFAULT_PATH
          )          

mark_as_advanced(FDK_AAC_INCLUDE_DIR)


find_library(FDK_AAC_LIBRARIES 
             NAMES libfdk-aac.so
             HINTS 
             /usr/local/lib  /usr/lib /usr/local/
             PATH_SUFFIXES 
             DOC "sdl render library..."
             #NO_DEFAULT_PATH
             )

if(FDK_AAC_INCLUDE_DIR AND FDK_AAC_LIBRARIES)
    SET(FDK_AAC_FOUND TRUE)    
endif()