
cmake_minimum_required(VERSION 3.2...3.31)

project(PureNet)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PURENET_SHARED_LIB "export shared" OFF)

if (PURENET_SHARED_LIB)
	add_definitions(-DPURENET_SHARED_LIB)
	add_definitions(-DPURENET_SHARED_EXPORT)
endif()

if (PURENET_ERRORCODE_BEGIN)
	add_definitions(-DPURENET_ERRORCODE_BEGIN=${PURENET_ERRORCODE_BEGIN})
endif ()

set( PureNetIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureNetSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB_RECURSE PureNetInc ${PureNetIncRoot}/PureNet/*.h)
file(GLOB_RECURSE PureNetSrc ${PureNetSrcRoot}/*.h ${PureNetSrcRoot}/*.cpp)

list(APPEND PureNetFullFiles ${PureNetInc}
							${PureNetSrc}
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

if(NOT WIN32)
	add_compile_options(-fPIC)
endif()

if (PURENET_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureNetFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureNetFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureNetIncRoot}/PureNet ${PureNetInc})
source_group_by_dir(src ${PureNetSrcRoot} ${PureNetSrc})
unset(PureNetInc)
unset(PureNetSrc)

list(APPEND PureNetDepLib PureCore PureMsg PureEncrypt uv)
list(APPEND PureNetIncPath ${PureNetIncRoot} 
							${PURE_MIMALLOC_INC}
                            ${PureCoreRootPath}/include 
							${PureMsgRootPath}/include 
							${PureEncryptRootPath}/include 
                            ${ThirdPartyRootPath}/libuv/include
                            ${ThirdPartyRootPath}/ada
                            ${ThirdPartyRootPath}/llhttp/include
							${ThirdPartyRootPath}/kcp
                            ${ThirdPartyRootPath}/wjcrypt/include
                            )

include_directories( ${PureNetIncPath} ${PureOpenSSLInc} )
add_dependencies(${PROJECT_NAME} ${PureNetDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureNetDepLib} ${PureOpenSSLLib})
include(../Cmake/PureOutput.cmake)

unset(PureNetFullFiles)
unset(PureNetIncRoot)
unset(PureNetSrcRoot)
unset(PureNetIncRoot)






