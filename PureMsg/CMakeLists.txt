
cmake_minimum_required(VERSION 3.2)

project(PureMsg)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PUREMSG_SHARED_LIB "export shared" OFF)

if (PUREMSG_SHARED_LIB)
	add_definitions(-DPUREMSG_SHARED_LIB)
	add_definitions(-DPUREMSG_SHARED_EXPORT)
endif()

if (PUREMSG_ERRORCODE_BEGIN)
	add_definitions(-DPUREMSG_ERRORCODE_BEGIN=${PUREMSG_ERRORCODE_BEGIN})
endif ()

set( PureMsgIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureMsgSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB_RECURSE PureMsgInc ${PureMsgIncRoot}/PureMsg/*.h)
file(GLOB_RECURSE PureMsgSrc ${PureMsgSrcRoot}/*.cpp)

list(APPEND PureMsgFullFiles ${PureMsgInc}
							${PureMsgSrc}
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

if (PUREMSG_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureMsgFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureMsgFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureMsgIncRoot}/PureMsg ${PureMsgInc})
source_group_by_dir(src ${PureMsgSrcRoot} ${PureMsgSrc})
unset(PureMsgInc)
unset(PureMsgSrc)

list(APPEND PureMsgDepLib PureCore)
list(APPEND PureMsgIncPath  ${PureMsgIncRoot}
							${PURE_MIMALLOC_INC}
                            ${PureCoreRootPath}/include 
                            )

include_directories( ${PureMsgIncPath} )
add_dependencies(${PROJECT_NAME} ${PureMsgDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureMsgDepLib})
include(../Cmake/PureOutput.cmake)

unset(PureMsgFullFiles)
unset(PureMsgIncRoot)
unset(PureMsgSrcRoot)
unset(PureMsgIncPath)






