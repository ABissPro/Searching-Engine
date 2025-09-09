cmake_minimum_required(VERSION 3.8)

set(Src main.cpp converterJson.cpp searchServer.cpp invertedIndex.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(SearchEng ${Src})

target_link_libraries(SearchEng nlohmann_json::nlohmann_json)
target_include_directories(SearchEng PUBLIC ${MY_INCLUDE_DIR})