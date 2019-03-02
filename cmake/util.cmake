
function(cmake_init )
    message(STATUS "start cmake ...")
endfunction(cmake_init )


FUNCTION(find_files files)
    include_directories(${PROJECT_SOURCE_DIR}/include 
                        /usr/local/include
                        /usr/include
                        )

    FILE(GLOB_RECURSE tmp ${PROJECT_SOURCE_DIR}/src/*.c)
    set(${files}  ${tmp} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(find_libs libs)
    
    SET(LOCAL_LIBS)
    FIND_PACKAGE(X264 REQUIRED)
    IF (X264_FOUND)
        MESSAGE(STATUS " x264 library found")
        INCLUDE_DIRECTORIES(${X264_INCLUDE_DIR})
        #SET(libs  PARENT_SCOPE)
        LIST(APPEND LOCAL_LIBS ${X264_LIBRARIES} )
        MESSAGE(STATUS " x264 library found ${X264_LIBRARIES} ${LOCAL_LIBS}")
    ELSE()
        MESSAGE(FATAL_ERROR,"x264 library not found")
    ENDIF()

    FIND_PACKAGE(FDK_AAC REQUIRED)
    IF (FDK_AAC_FOUND)
        MESSAGE(STATUS " FDK_AAC library found")
        INCLUDE_DIRECTORIES(${FDK_AAC_INCLUDE_DIR})
        #SET(libs  ${FDK_AAC_LIBRARIES} PARENT_SCOPE)
        LIST(APPEND LOCAL_LIBS ${FDK_AAC_LIBRARIES}  )
        MESSAGE(STATUS " FDK_AAC library found current libs ${LOCAL_LIBS}")
    ELSE()
        MESSAGE(FATAL_ERROR,"FDK_AAC library not found")
    ENDIF()

    FIND_PACKAGE(OBJECT REQUIRED)
    IF (OBJECT_FOUND)
        MESSAGE(STATUS " OBJECT library found")
        INCLUDE_DIRECTORIES(${OBJECT_INCLUDE_DIR})
        #SET(libs ${OBJECT_LIBRARIES} PARENT_SCOPE)
        LIST(APPEND LOCAL_LIBS ${OBJECT_LIBRARIES})
        MESSAGE(STATUS " OBJECT library found current libs ${LOCAL_LIBS}")
    ELSE()
        MESSAGE(FATAL_ERROR,"OBJECT library not found")
    ENDIF()

    set(${libs} ${LOCAL_LIBS} PARENT_SCOPE)
ENDFUNCTION()
