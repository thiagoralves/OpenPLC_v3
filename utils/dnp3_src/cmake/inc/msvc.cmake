if (WIN32)
    if(MSVC)
	add_definitions(-D_WIN32_WINNT=0x0501)
	add_definitions(-DASIO_HAS_STD_SYSTEM_ERROR)  
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS ${WINSOCKAPI} /wd4251 /wd4996 /wd4273 /wd4275 /wd4018 /wd4146 /EHsc")

	set_property(GLOBAL PROPERTY USE_FOLDERS ON) #allows the creation of solution folders
	set(LIB_TYPE STATIC) #default to static on MSVC
    else()
      if (MINGW)
        # MINGW is a hybred between the Windows system and GCC compiler
        add_definitions(-D_WIN32_WINNT=0x0501)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

        set(LIB_TYPE STATIC) #default to static on MinGW

        # We need to link the demos with these
        set(NET_LIBS "ws2_32" "wsock32") 
      endif()
    endif()
endif()

