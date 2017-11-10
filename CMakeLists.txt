cmake_minimum_required(VERSION 3.8)
project(textpressoapi)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")
set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(/usr/local/include/)

find_package(Textpresso REQUIRED)
include_directories(${Textpresso_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)
add_executable(textpressoapi ${SOURCE_FILES})
target_link_libraries(textpressoapi boost_system pthread textpresso uima xerces-c lucene++)