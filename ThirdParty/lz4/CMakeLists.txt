
cmake_minimum_required(VERSION 3.2...3.31)

project(lz4)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
endif()

option(LZ4_SHARED_LIB "export shared" OFF)

if (LZ4_SHARED_LIB)
	add_definitions(-DLZ4_SHARED_LIB)
	if (WIN32)
		set(LZ4_DLL_EXPORT 1)
		add_definitions(-DLZ4_DLL_EXPORT=1)
	endif()
endif()


set( IncRootPath ${lz4_SOURCE_DIR}/lib )
set( SrcRootPath ${lz4_SOURCE_DIR}/lib )

include_directories( ${IncRootPath} )

file(GLOB_RECURSE lz4Inc ${IncRootPath}/*.h)
file(GLOB_RECURSE lz4Src ${SrcRootPath}/*.c)

list(APPEND FullSourcePath ${lz4Inc}
							${lz4Src})


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

if (LZ4_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${FullSourcePath} )
else()
	add_library( ${PROJECT_NAME} STATIC ${FullSourcePath} )
endif()

include(../../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${IncRootPath} ${lz4Inc})
source_group_by_dir(src ${SrcRootPath} ${lz4Src})
unset(lz4Inc)
unset(lz4Src)
target_link_libraries(${PROJECT_NAME})
include(../../Cmake/PureOutput.cmake)

unset(FullSourcePath)
unset(IncRootPath)
unset(SrcRootPath)
unset(LZ4_DLL_EXPORT)






