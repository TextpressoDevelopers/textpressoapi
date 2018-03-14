cmake_minimum_required(VERSION 3.5)
project(textpressoapi)

execute_process(COMMAND git submodule update --init -- thirdparty/SQLiteCpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# set FOO_INCLUDE_DIR properly
set(SQLiteCpp_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SQLiteCpp/include
        CACHE PATH "SQLite include directory")

# for convenience setup a target
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SQLiteCpp)

include_directories(${SQLiteCpp_INCLUDE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")
set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories("/home/valerio/workspace/caltech/textpresso/libtpc")

find_package(APR REQUIRED)
include_directories(${APR_INCLUDE_DIR})

find_package(Lucene++ REQUIRED)
include_directories(${Lucene++_INCLUDE_DIRS})

find_package(Textpresso REQUIRED)
include_directories(${Textpresso_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)
add_executable(textpressoapi ${SOURCE_FILES})
target_link_libraries(textpressoapi boost_system pthread textpresso uima xerces-c lucene++ SQLiteCpp sqlite3 pthread dl
        boost_filesystem boost_system boost_program_options crypto ssl)