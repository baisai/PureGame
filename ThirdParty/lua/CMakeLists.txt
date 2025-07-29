
cmake_minimum_required(VERSION 3.2)

project(Lua)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
endif()

option(LUA_SHARED_LIB "export shared" OFF)

if (LUA_SHARED_LIB)
	add_definitions(-DLUA_SHARED_LIB)
	if (WIN32)
		add_definitions(-DLUA_BUILD_AS_DLL)
		add_definitions(-DLUA_LIB)
	endif ()
endif()


set( IncRootPath ${Lua_SOURCE_DIR}/src )
set( SrcRootPath ${Lua_SOURCE_DIR}/src )

include_directories( ${IncRootPath} )

file(GLOB_RECURSE LuaInc ${IncRootPath}/*.h)
file(GLOB_RECURSE LuaSrc ${SrcRootPath}/*.c)
list(REMOVE_ITEM LuaSrc ${SrcRootPath}/lua.c ${SrcRootPath}/luac.c)

list(APPEND FullSourcePath ${LuaInc}
							${LuaSrc})


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

if (LUA_SHARED_LIB)
	add_definitions(-DLUA_EXPORTS)
	add_library( ${PROJECT_NAME} SHARED ${FullSourcePath} )
else()
	add_library( ${PROJECT_NAME} STATIC ${FullSourcePath} )
endif()

include(../../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${IncRootPath} ${LuaInc})
source_group_by_dir(src ${SrcRootPath} ${LuaSrc})
unset(LuaInc)
unset(LuaSrc)
if (WIN32)
    target_link_libraries(${PROJECT_NAME})
else ()
    target_link_libraries(${PROJECT_NAME} m)
endif ()
include(../../Cmake/PureOutput.cmake)

unset(FullSourcePath)
unset(IncRootPath)
unset(SrcRootPath)






