cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0077 NEW)

project(pugixml VERSION 1.14 LANGUAGES CXX)

# Expose options from the pugiconfig.hpp
option(PUGIXML_WCHAR_MODE "Enable wchar_t mode" OFF)
option(PUGIXML_COMPACT "Enable compact mode" OFF)

# Advanced options from pugiconfig.hpp
option(PUGIXML_NO_XPATH "Disable XPath" OFF)
option(PUGIXML_NO_STL "Disable STL" OFF)
option(PUGIXML_NO_EXCEPTIONS "Disable Exceptions" OFF)

add_definitions(-DPUGIXML_HAS_LONG_LONG)

if (PUGIXML_WCHAR_MODE)
    add_definitions(-DPUGIXML_WCHAR_MODE)
endif()
if (PUGIXML_COMPACT)
    add_definitions(-DPUGIXML_COMPACT)
endif()
if (PUGIXML_NO_XPATH)
    add_definitions(-DPUGIXML_NO_XPATH)
endif()
if (PUGIXML_NO_STL)
    add_definitions(-DPUGIXML_NO_STL)
endif()
if (PUGIXML_NO_EXCEPTIONS)
    add_definitions(-DPUGIXML_NO_EXCEPTIONS)
endif()

set( IncRootPath ${pugixml_SOURCE_DIR}/src )
set( SrcRootPath ${pugixml_SOURCE_DIR}/src )

include_directories( ${IncRootPath} )

file(GLOB_RECURSE LuaInc ${IncRootPath}/*.hpp)
file(GLOB_RECURSE LuaSrc ${SrcRootPath}/*.cpp)

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

add_library( ${PROJECT_NAME} STATIC ${FullSourcePath} )
target_link_libraries(${PROJECT_NAME})

include(../../Cmake/PureOutput.cmake)

unset(FullSourcePath)
unset(IncRootPath)
unset(SrcRootPath)

