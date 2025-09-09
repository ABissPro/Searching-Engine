cmake_minimum_required(VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Копируем необходимые файлы в директорию тестов
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_CURRENT_BINARY_DIR}/config.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/request.json ${CMAKE_CURRENT_BINARY_DIR}/request.json COPYONLY)
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


set(SrcFiles ${MY_SRC_DIR}/converterJson.cpp
             ${MY_SRC_DIR}/invertedIndex.cpp
             ${MY_SRC_DIR}/searchServer.cpp)

set(TestsFiles converterJson_test.cpp
               invertedIndex_test.cpp
               searchServer_test.cpp)

add_executable(GtestTests ${TestsFiles} ${SrcFiles})
target_link_libraries(GtestTests nlohmann_json::nlohmann_json GTest::gtest_main)
include(GoogleTest)
target_include_directories(GtestTests PUBLIC ${MY_INCLUDE_DIR})
gtest_discover_tests(GtestTests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})