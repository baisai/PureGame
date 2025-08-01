cmake_minimum_required(VERSION 3.2...3.31)
cmake_policy(SET CMP0077 NEW)

project(PureGame)

if (NOT CMAKE_BUILD_TYPE)
    if (Debug)
        set(CMAKE_BUILD_TYPE Debug)
    elseif (Release)
        set(CMAKE_BUILD_TYPE Release)
    elseif (RelWithDebInfo)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    elseif (MinSizeRel)
        set(CMAKE_BUILD_TYPE MinSizeRel)
    else()
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif()
endif()

include(CMakeDependentOption)
option(PURE_REDIS "redis" ON)
option(PURE_LEVELDB "leveldb" ON)

set(ThirdPartyRootPath ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)
set(ThirdPartyBinPath ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty)
set(PureCoreRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureCore)
set(PureEncryptRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureEncrypt)
set(PureLuaRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureLua)
set(PureMsgRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureMsg)
set(PureNetRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureNet)
set(PureDbRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureDb)
set(PureAppRootPath ${CMAKE_CURRENT_SOURCE_DIR}/PureApp)

include(Cmake/PureOutput.cmake)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_compile_options(/wd4595)
    add_compile_options(/wd4200)
    add_compile_options(/utf-8)
    add_compile_options(/MP)
endif()

set(PURE_OPENSSL OFF)

if (WIN32)
    list(APPEND PURE_SYSTEM_DEP DbgHelp)
    if (PURE_OPENSSL)
        list(APPEND PURE_SYSTEM_DEP Crypt32)
    endif()
else()
	list(APPEND PURE_SYSTEM_DEP pthread m dl)
endif ()

add_subdirectory(ThirdParty/yyjson)

set(PUGIXML_NO_XPATH ON)
set(PUGIXML_NO_STL ON)
add_subdirectory(ThirdParty/pugixml)

add_subdirectory(ThirdParty/lua)

set(BUILD_SHARED_LIBS OFF)
set(ZLIB_COMPAT OFF)
set(ZLIB_ENABLE_TESTS OFF)
set(ZLIBNG_ENABLE_TESTS OFF)
set(WITH_GTEST OFF)
set(WITH_FUZZERS OFF)
set(WITH_BENCHMARKS OFF)
set(WITH_BENCHMARK_APPS OFF)
add_subdirectory(ThirdParty/zlib-ng)

set(MZ_COMPAT OFF)
set(MZ_ZLIB ON)
set(ZLIBNG_FOUND ON)
set(ZLIBNG_INCLUDE_DIRS "${ThirdPartyRootPath}/zlib-ng" "${CMAKE_BINARY_DIR}/ThirdParty/zlib-ng")
set(ZLIBNG_DEP "zlib-ng")
set(MZ_BZIP2 OFF)
set(MZ_LZMA OFF)
set(MZ_ZSTD OFF)
set(MZ_LIBCOMP OFF)
set(MZ_FETCH_LIBS OFF)
set(MZ_FORCE_FETCH_LIBS OFF)
if (PURE_OPENSSL)
    set(MZ_PKCRYPT ON)
    set(MZ_WZAES ON)
    set(MZ_OPENSSL ON)
else()
    set(MZ_PKCRYPT OFF)
    set(MZ_WZAES OFF)
    set(MZ_OPENSSL OFF)
endif()
set(MZ_LIBBSD OFF)
set(MZ_ICONV OFF)
set(MZ_COMPRESS_ONLY OFF)
set(MZ_DECOMPRESS_ONLY OFF)
set(MZ_FILE32_API OFF)
set(MZ_BUILD_TESTS OFF)
set(MZ_BUILD_UNIT_TESTS OFF)
set(MZ_BUILD_FUZZ_TESTS OFF)
set(MZ_CODE_COVERAGE OFF)
add_definitions(-DMZ_LIB_SUFFIX="-ng")
add_subdirectory(ThirdParty/minizip-ng)

if (WIN32)
    if (LZ4_SHARED_LIB AND NOT LZ4_DLL_EXPORT)
        add_definitions(-DLZ4_DLL_IMPORT=1)
    endif()
endif()
add_subdirectory(ThirdParty/lz4)

set(BUILD_SHARED_LIBS OFF)
set(SNAPPY_BUILD_TESTS OFF)
set(SNAPPY_BUILD_BENCHMARKS OFF)
set(SNAPPY_FUZZING_BUILD OFF)
set(SNAPPY_REQUIRE_AVX OFF)
set(SNAPPY_REQUIRE_AVX2 OFF)
set(SNAPPY_INSTALL OFF)
add_subdirectory(ThirdParty/snappy)

add_subdirectory(ThirdParty/libuv)

add_subdirectory(ThirdParty/wjcrypt)

if (PURE_REDIS)
    add_definitions(-DPURE_REDIS=1)
    set(BUILD_SHARED_LIBS OFF)
    set(DISABLE_TESTS ON)
    add_subdirectory(ThirdParty/hiredis)
    list(APPEND PURE_DB_THIRDPARTY hiredis )
endif ()

if (PURE_LEVELDB)
    add_definitions(-DPURE_LEVELDB=1)
    set(LEVELDB_BUILD_TESTS OFF)
    set(LEVELDB_BUILD_BENCHMARKS OFF)
    set(LEVELDB_INSTALL OFF)
    set(BUILD_SHARED_LIBS OFF)
    set(HAVE_SNAPPY ON)
    add_subdirectory(ThirdParty/leveldb)
    list(APPEND PURE_DB_THIRDPARTY leveldb )
    list(APPEND ProjectTools LeveldbDump PureLevelReverter)
endif ()

add_subdirectory(PureCore)
add_subdirectory(PureEncrypt)
add_subdirectory(PureLua)
add_subdirectory(PureMsg)
add_subdirectory(PureNet)
add_subdirectory(PureDb)
add_subdirectory(PureApp)
add_subdirectory(PureGame)

list(APPEND ProjectTools PureLuaArchive)

if (WIN32)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
    set_property(TARGET yyjson
                        pugixml
                        Lua 
                        zlib-ng 
                        minizip-ng 
                        lz4 
                        snappy
                        uv 
                        wjcrypt
                        ${PURE_DB_THIRDPARTY}
                        PROPERTY FOLDER "ThirdParty")
    if (ProjectTools)
        set_property(TARGET ${ProjectTools} PROPERTY FOLDER "Tools")
    endif()
endif()
