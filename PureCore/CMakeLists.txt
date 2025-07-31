
cmake_minimum_required(VERSION 3.2...3.31)

project(PureCore)

if (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

option(PURECORE_SHARED_LIB "export shared" OFF)

if (PURECORE_SHARED_LIB)
	add_definitions(-DPURECORE_SHARED_LIB)
	add_definitions(-DPURECORE_SHARED_EXPORT)
endif()

if (PURECORE_ERRORCODE_BEGIN)
	add_definitions(-DPURECORE_ERRORCODE_BEGIN=${PURECORE_ERRORCODE_BEGIN})
endif ()

set( PureCoreIncRoot ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( PureCoreSrcRoot ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB_RECURSE PureCoreInc ${PureCoreIncRoot}/PureCore/*.h)
file(GLOB_RECURSE SpdlogSrc ${PureCoreIncRoot}/spdlog/*.h)
file(GLOB_RECURSE PureCoreSrc ${PureCoreSrcRoot}/*.cpp)

list(APPEND PureCoreFullFiles ${PureCoreInc}
                            ${SpdlogSrc}
							${PureCoreSrc})


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

if (PURECORE_SHARED_LIB)
	add_library( ${PROJECT_NAME} SHARED ${PURE_MIMALLOC_OUT} ${PureCoreFullFiles} )
else()
	add_library( ${PROJECT_NAME} STATIC ${PURE_MIMALLOC_OUT} ${PureCoreFullFiles} )
endif()

include(../Cmake/SourceGroup.cmake)
source_group_by_dir(include ${PureCoreIncRoot}/PureCore ${PureCoreInc})
source_group_by_dir(spdlog ${PureCoreIncRoot}/spdlog ${SpdlogSrc})
source_group_by_dir(src ${PureCoreSrcRoot} ${PureCoreSrc})
unset(PureCoreInc)
unset(SpdlogSrc)
unset(PureCoreSrc)

list(APPEND PureCoreDepLib yyjson pugixml)
list(APPEND PureCoreIncPath ${PureCoreIncRoot} 
                            ${ThirdPartyRootPath}/yyjson/src
                            ${ThirdPartyRootPath}/pugixml/src
                            ${PURE_MIMALLOC_INC} 
                            )

include_directories(${PureCoreIncPath})
add_dependencies(${PROJECT_NAME} ${PureCoreDepLib})
target_link_libraries(${PROJECT_NAME} ${PURE_SYSTEM_DEP} ${PureCoreDepLib})
include(../Cmake/PureOutput.cmake)

unset(PureCoreFullFiles)
unset(PureCoreIncRoot)
unset(PureCoreSrcRoot)
unset(PureCoreIncPath)






