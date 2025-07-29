
cmake_minimum_required(VERSION 3.2)

project(wjcrypt)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
endif()


set( IncRootPath ${wjcrypt_SOURCE_DIR}/include )
set( SrcRootPath ${wjcrypt_SOURCE_DIR}/src )

include_directories( ${IncRootPath} )

file(GLOB_RECURSE wjcryptInc ${IncRootPath}/*.h)
file(GLOB_RECURSE wjcryptSrc ${SrcRootPath}/*.c)

list(APPEND FullSourcePath ${wjcryptInc}
							${wjcryptSrc})


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

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_compile_options(/wd4595)
endif()

if(NOT WIN32)
	add_compile_options(-fPIC)
endif()


add_library( ${PROJECT_NAME} STATIC ${FullSourcePath} )

include(../../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${IncRootPath} ${wjcryptInc})
source_group_by_dir(src ${SrcRootPath} ${wjcryptSrc})
unset(wjcryptInc)
unset(wjcryptSrc)
if (WIN32)
    target_link_libraries(${PROJECT_NAME})
else ()
    target_link_libraries(${PROJECT_NAME} m)
endif ()
include(../../Cmake/PureOutput.cmake)

unset(FullSourcePath)
unset(IncRootPath)
unset(SrcRootPath)






