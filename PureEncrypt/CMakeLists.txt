
cmake_minimum_required(VERSION 3.2...3.31)

project(PureEncrypt)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PUREENCRYPT_SHARED_LIB "export shared" OFF)

if (PUREENCRYPT_SHARED_LIB)
	add_definitions(-DPUREENCRYPT_SHARED_LIB)
	add_definitions(-DPUREENCRYPT_SHARED_EXPORT)
endif()

if (PUREENCRYPT_ERRORCODE_BEGIN)
	add_definitions(-DPUREENCRYPT_ERRORCODE_BEGIN=${PUREENCRYPT_ERRORCODE_BEGIN})
endif ()

set( PureEncryptIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureEncryptSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB_RECURSE PureEncryptInc ${PureEncryptIncRoot}/PureEncrypt/*.h)
file(GLOB_RECURSE PureEncryptSrc ${PureEncryptSrcRoot}/*.cpp)

list(APPEND PureEncryptFullFiles ${PureEncryptInc}
							${PureEncryptSrc})


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

if (PUREENCRYPT_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureEncryptFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureEncryptFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureEncryptIncRoot}/PureEncrypt ${PureEncryptInc})
source_group_by_dir(src ${PureEncryptSrcRoot} ${PureEncryptSrc})
unset(PureEncryptInc)
unset(PureEncryptSrc)

list(APPEND PureEncryptDepLib PureCore zlib-ng minizip-ng lz4 snappy wjcrypt)
list(APPEND PureEncryptIncPath  ${PureCoreRootPath}/include
                                ${PureEncryptIncRoot}
								${PURE_MIMALLOC_INC}
                                ${ThirdPartyRootPath}/zlib-ng
								${CMAKE_BINARY_DIR}/ThirdParty/zlib-ng
                                ${ThirdPartyRootPath}/minizip-ng
                                ${ThirdPartyRootPath}/lz4/lib 
								${ThirdPartyRootPath}/snappy
                                ${ThirdPartyRootPath}/wjcrypt/include
                                )
								
include_directories( ${PureEncryptIncPath} )
add_dependencies(${PROJECT_NAME} ${PureEncryptDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureEncryptDepLib})
include(../Cmake/PureOutput.cmake)

unset(PureEncryptFullFiles)
unset(PureEncryptIncRoot)
unset(PureEncryptSrcRoot)
unset(PureEncryptIncPath)






