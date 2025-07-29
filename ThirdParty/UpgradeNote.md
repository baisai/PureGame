# yyjson
    1. `CMakeLists.txt`
        remove 
        ```
        if(XCODE OR MSVC)
            set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
        endif()
        ```