
cmake_minimum_required(VERSION 3.2...3.31)

project(PureApp)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PUREAPP_SHARED_LIB "export shared" OFF)

if (PUREAPP_SHARED_LIB)
	add_definitions(-DPUREAPP_SHARED_LIB)
	add_definitions(-DPUREAPP_SHARED_EXPORT)
endif()

if (PUREAPP_ERRORCODE_BEGIN)
	add_definitions(-DPUREAPP_ERRORCODE_BEGIN=${PUREDB_ERRORCODE_BEGIN})
endif ()

set( PureAppIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureAppSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB_RECURSE PureAppInc ${PureAppIncRoot}/PureApp/*.h)
file(GLOB_RECURSE PureAppSrc ${PureAppSrcRoot}/*.h ${PureAppSrcRoot}/*.cpp)
list(APPEND PureAppFullFiles ${PureAppInc}
                            ${PureAppSrc})

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

if (PUREAPP_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureAppFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureAppFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureAppIncRoot}/PureApp ${PureAppInc})
source_group_by_dir(src ${PureAppSrcRoot} ${PureAppSrc})
unset(PureAppInc)
unset(PureAppSrc)

list(APPEND PureAppDepLib PureCore PureEncrypt PureMsg PureDb PureLua PureNet)

list(APPEND PureAppIncPath ${PureAppIncRoot} 
							${PURE_MIMALLOC_INC}
                            ${PureCoreRootPath}/include 
                            ${ThirdPartyRootPath}/yyjson/src
                            ${ThirdPartyRootPath}/pugixml/src
                            ${PureDbRootPath}/include 
                            ${PureEncryptRootPath}/include 
                            ${ThirdPartyRootPath}/zlib-ng
							${CMAKE_BINARY_DIR}/ThirdParty/zlib-ng
                            ${ThirdPartyRootPath}/minizip-ng
                            ${ThirdPartyRootPath}/lz4/lib 
                            ${ThirdPartyRootPath}/snappy
                            ${PureLuaRootPath}/include 
                            ${ThirdPartyRootPath}/lua/src 
                            ${PureMsgRootPath}/include 
                            ${PureNetRootPath}/include 
                            ${ThirdPartyRootPath}/libuv/include
                            ${ThirdPartyRootPath}/ada
                            ${ThirdPartyRootPath}/llhttp/include
							${ThirdPartyRootPath}/kcp
                            ${ThirdPartyRootPath}/wjcrypt/include
                            ${PureOpenSSLInc}
                            )

if (PURE_REDIS)
	list(APPEND PureAppIncPath ${ThirdPartyRootPath}/hiredis)
endif()
if (PURE_MYSQL)
	list(APPEND PureAppIncPath ${ThirdPartyRootPath}/mariadb-connector-c/include)
	list(APPEND PureAppIncPath ${ThirdPartyBinPath}/mariadb-connector-c/include)
endif()
if (PURE_MONGODB)
	list(APPEND PureAppIncPath ${ThirdPartyRootPath}/mongo-c-driver/src/libmongoc/src/mongoc)
	list(APPEND PureAppIncPath ${ThirdPartyRootPath}/mongo-c-driver/src/libbson/src/bson)
endif()
if (PURE_LEVELDB)
	list(APPEND PureAppIncPath ${ThirdPartyRootPath}/leveldb/include)
endif()
if (PURE_SQLITE)
	list(APPEND PureAppIncPath ${ThirdPartyRootPath}/sqlite)
endif()


include_directories( ${PureAppIncPath} )
add_dependencies(${PROJECT_NAME} ${PureAppDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureAppDepLib})
include(../Cmake/PureOutput.cmake)

source_group_by_dir(src ${CMAKE_CURRENT_SOURCE_DIR}/tools ${CMAKE_CURRENT_SOURCE_DIR}/tools/PureLuaArchive.cpp )
add_executable( PureLuaArchive ${CMAKE_CURRENT_SOURCE_DIR}/tools/PureLuaArchive.cpp )
add_dependencies(PureLuaArchive PureApp)
target_link_libraries(PureLuaArchive ${PURE_SYSTEM_DEP} PureApp)

unset(PureAppFullFiles)
unset(PureAppIncRoot)
unset(PureAppSrcRoot)






