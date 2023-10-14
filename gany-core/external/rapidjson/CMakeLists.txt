cmake_minimum_required(VERSION 3.20)

project(RapidJson)

set(TARGET_NAME rj)

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
