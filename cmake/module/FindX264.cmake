find_path(X264_INCLUDE_DIR 
          NAMES x264.h 
          HINTS  
          /usr/local/include /usr/local/
          PATH_SUFFIXES x264 
          DOC "find sdl header files..."
          #NO_DEFAULT_PATH
          )          

mark_as_advanced(X264_INCLUDE_DIR)


find_library(X264_LIBRARIES 
             NAMES libx264.a
             HINTS 
             /usr/local/lib  /usr/lib /usr/local/
             PATH_SUFFIXES x264  
             DOC "sdl render library..."
             #NO_DEFAULT_PATH
             )

if(X264_INCLUDE_DIR AND X264_LIBRARIES)
    SET(X264_FOUND TRUE)    
endif()
