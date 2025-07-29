
cmake_minimum_required(VERSION 3.2)

project (PureGame)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set( PureGameIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureGameSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB_RECURSE PureGameInc ${PureGameIncRoot}/*.h)
file(GLOB_RECURSE PureGameSrc ${PureGameSrcRoot}/*.h ${PureGameSrcRoot}/*.cpp)
list(APPEND PureGameFullFiles ${PureGameInc}
                            ${PureGameSrc})

list(APPEND PureGameIncPath ${PureGameIncRoot}
                            ${PureAppRootPath}/include
                            ${PureCoreRootPath}/include 
                            ${PureDbRootPath}/include 
							${PureEncryptRootPath}/include 
                            ${PureLuaRootPath}/include 
							${PureMsgRootPath}/include 
                            ${PureNetRootPath}/include 
                            ${ThirdPartyRootPath}/lua/src
                            ${ThirdPartyRootPath}/wjcrypt/include
                            )

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
set(BuildTypeValid FALSE)
foreach(var Debug Release RelWithDebInfo MinSizeRel)
	if(CMAKE_BUILD_TYPE MATCHES ${var})
		set(BuildTypeValid TRUE)
	endif()
endforeach()
if (NOT BuildTypeValid)
	message(FATAL_ERROR "unknow build type " ${CMAKE_BUILD_TYPE})
endif()
unset(BuildTypeValid)

if (PUREAPP_SHARED_LIB)
	add_definitions("-DPUREAPP_SHARED_LIB")
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories( ${PureGameIncPath} )
add_executable( ${PROJECT_NAME} ${PURE_MIMALLOC_OUT} ${PureGameFullFiles} )
add_dependencies(${PROJECT_NAME} PureApp)
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} PureApp )
include(../Cmake/PureOutput.cmake)

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureGameIncRoot} ${PureGameInc})
source_group_by_dir(src ${PureGameSrcRoot} ${PureGameSrc})

unset(PureGameFullFiles)
unset(PureGameIncRoot)
unset(PureGameSrcRoot)


