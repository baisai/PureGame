
cmake_minimum_required(VERSION 3.2...3.31)

project(PureDb)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PUREDB_SHARED_LIB "export shared" OFF)

if (PUREDB_SHARED_LIB)
	add_definitions(-DPUREDB_SHARED_LIB)
	add_definitions(-DPUREDB_SHARED_EXPORT)
endif()

if (PUREDB_ERRORCODE_BEGIN)
	add_definitions(-DPUREDB_ERRORCODE_BEGIN=${PUREDB_ERRORCODE_BEGIN})
endif ()

set( PureDbIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureDbSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB PureDbInc ${PureDbIncRoot}/PureDb/*.h)
file(GLOB PureDbSrc ${PureDbSrcRoot}/*.h ${PureDbSrcRoot}/*.cpp)
if (PURE_REDIS)
	file(GLOB PureTemp ${PureDbIncRoot}/PureDb/Redis/*.h)
	list(APPEND PureDbInc ${PureTemp})
	file(GLOB PureTemp ${PureDbSrcRoot}/Redis/*.h ${PureDbSrcRoot}/Redis/*.cpp)
	list(APPEND PureDbSrc ${PureTemp})
	unset(PureTemp)
endif ()
if (PURE_LEVELDB)
	file(GLOB PureTemp ${PureDbIncRoot}/PureDb/LevelDb/*.h)
	list(APPEND PureDbInc ${PureTemp})
	file(GLOB PureTemp ${PureDbSrcRoot}/LevelDb/*.h ${PureDbSrcRoot}/LevelDb/*.cpp)
	list(APPEND PureDbSrc ${PureTemp})
	unset(PureTemp)
endif ()
list(APPEND PureDbFullFiles ${PureDbInc}
							${PureDbSrc})

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

if (PUREDB_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureDbFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureDbFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureDbIncRoot}/PureDb ${PureDbInc})
source_group_by_dir(src ${PureDbSrcRoot} ${PureDbSrc})
unset(PureDbInc)
unset(PureDbSrc)

list(APPEND PureDbDepLib PureCore PureEncrypt PureMsg)
if (PURE_REDIS)
	list(APPEND PureDbDepLib hiredis)
endif()
if (PURE_LEVELDB)
	list(APPEND PureDbDepLib leveldb)
endif()

list(APPEND PureDbIncPath ${PureDbIncRoot} 
            ${PURE_MIMALLOC_INC}
            ${PureCoreRootPath}/include 
            ${PureEncryptRootPath}/include 
            ${PureMsgRootPath}/include 
            ${ThirdPartyRootPath}/lz4/lib
		    ${ThirdPartyRootPath}/zlib-ng
		    ${CMAKE_BINARY_DIR}/ThirdParty/zlib-ng
            ${ThirdPartyRootPath}/minizip-ng
            )
if (PURE_REDIS)
	list(APPEND PureDbIncPath ${ThirdPartyRootPath}/hiredis)
endif()
if (PURE_LEVELDB)
	list(APPEND PureDbIncPath ${ThirdPartyRootPath}/leveldb/include)
endif()


include_directories( ${PureDbIncPath} )
add_dependencies(${PROJECT_NAME} ${PureDbDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureDbDepLib})
include(../Cmake/PureOutput.cmake)


if (PURE_LEVELDB)
	file(COPY ${ThirdPartyRootPath}/leveldb/db/leveldbutil.cc DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/tools)
	source_group_by_dir(src ${CMAKE_CURRENT_SOURCE_DIR}/tools ${CMAKE_CURRENT_SOURCE_DIR}/tools/leveldbutil.cc )
	add_executable( LeveldbDump ${CMAKE_CURRENT_SOURCE_DIR}/tools/leveldbutil.cc )
	add_dependencies(LeveldbDump leveldb)
	target_link_libraries(LeveldbDump ${PURE_SYSTEM_DEP} leveldb)

	source_group_by_dir(src ${CMAKE_CURRENT_SOURCE_DIR}/tools ${CMAKE_CURRENT_SOURCE_DIR}/tools/PureLevelReverter.cpp )
	add_executable( PureLevelReverter ${CMAKE_CURRENT_SOURCE_DIR}/tools/PureLevelReverter.cpp )
	add_dependencies(PureLevelReverter PureDb)
	target_link_libraries(PureLevelReverter ${PURE_SYSTEM_DEP} PureDb)
endif ()

unset(PureDbFullFiles)
unset(PureDbIncRoot)
unset(PureDbSrcRoot)
unset(PureDbIncRoot)






