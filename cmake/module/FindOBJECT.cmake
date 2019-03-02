
#find_path
#mark_as_a
find_path(OBJECT_INCLUDE_DIR 
          NAMES libobject.h
          HINTS
          /usr/local/include /usr/include
          PATH_SUFFIXES libobject 
          DOC "find object"
          )

mark_as_advanced(OBJECT_INCLUDE_DIR)


find_library(OBJECT_LIBRARIES 
             NAMES libobject.so 
             HINTS
             /usr/lib /usr/local/lib
             PATH_SUFFIXES
             )


if(OBJECT_INCLUDE_DIR AND OBJECT_LIBRARIES)
    SET(OBJECT_FOUND TRUE)    
endif()
