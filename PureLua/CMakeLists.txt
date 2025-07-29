
cmake_minimum_required(VERSION 3.2)

project(PureLua)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PURELUA_SHARED_LIB "export shared" OFF)
option(PURELUA_CACHE_OBJECT "cache cpp object" ON)

if (PURELUA_SHARED_LIB)
	add_definitions(-DPURELUA_SHARED_LIB)
	add_definitions(-DPURELUA_SHARED_EXPORT)
endif()

if (PURELUA_CACHE_OBJECT)
	add_definitions(-DPURELUA_CACHE_OBJECT)
endif()

if (PURELUA_ERRORCODE_BEGIN)
	add_definitions(-DPURELUA_ERRORCODE_BEGIN=${PURELUA_ERRORCODE_BEGIN})
endif ()

set( PureLuaIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureLuaSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )
set( PureLuaToolsRoot ${CMAKE_CURRENT_SOURCE_DIR}/tools )

file(GLOB_RECURSE PureLuaInc ${PureLuaIncRoot}/PureLua/*.h)
file(GLOB_RECURSE PureLuaSrc ${PureLuaSrcRoot}/*.cpp)

list(APPEND PureLuaFullFiles ${PureLuaInc}
							${PureLuaSrc})


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

if (PURELUA_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureLuaFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureLuaFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureLuaIncRoot}/PureLua ${PureLuaInc})
source_group_by_dir(src ${PureLuaSrcRoot} ${PureLuaSrc})
unset(PureLuaInc)
unset(PureLuaSrc)

list(APPEND PureLuaDepLib PureCore Lua)
list(APPEND PureLuaIncPath ${PureCoreRootPath}/include
                            ${PureLuaIncRoot}
							${PURE_MIMALLOC_INC}
							${ThirdPartyRootPath}/lua/src 
							${ThirdPartyRootPath}/yyjson/src
                            ${ThirdPartyRootPath}/pugixml/src
							${PureCoreRootPath}/include 
                            )

include_directories( ${PureLuaIncPath} )
add_dependencies(${PROJECT_NAME} ${PureLuaDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureLuaDepLib})
include(../Cmake/PureOutput.cmake)

unset(PureLuaFullFiles)
unset(PureLuaIncRoot)
unset(PureLuaSrcRoot)
unset(PureLuaToolsRoot)
unset(PureLuaIncPath)






