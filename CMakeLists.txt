cmake_minimum_required(VERSION 3.8)
project(SearchEng VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(MY_SRC_DIR ${CMAKE_SOURCE_DIR}/src)

include(FetchContent)
#GoogleTest
FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.13.0)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${CMAKE_SOURCE_DIR}/nlohmann_json/json-develop
    ${CMAKE_CURRENT_BINARY_DIR}/nlohmann_json)

