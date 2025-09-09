cmake_minimum_required(VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# копируем ресурсы в каталог сборки тестов (работает надёжно)
# В отличие от configure_file, делаем копирование непосредственно в папку, где окажетс exe
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/copied_test_resources.stamp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/config.json ${CMAKE_CURRENT_BINARY_DIR}/config.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/request.json ${CMAKE_CURRENT_BINARY_DIR}/request.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/copied_test_resources.stamp
    COMMENT "Copying test resources to test binary directory"
)

set(SrcFiles ${MY_SRC_DIR}/converterJson.cpp
             ${MY_SRC_DIR}/invertedIndex.cpp
             ${MY_SRC_DIR}/searchServer.cpp)

set(TestsFiles converterJson_test.cpp
               invertedIndex_test.cpp
               searchServer_test.cpp)

add_executable(GtestTests ${TestsFiles} ${SrcFiles})

# Ensure the resource copy runs before the test target is linked/build
#add_dependencies(GtestTests ${CMAKE_CURRENT_BINARY_DIR}/copied_test_resources.stamp)

target_link_libraries(GtestTests nlohmann_json::nlohmann_json GTest::gtest_main)
include(GoogleTest)
target_include_directories(GtestTests PUBLIC ${MY_INCLUDE_DIR})
gtest_discover_tests(GtestTests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Also ensure debugger working directory is set to the target binary dir
set_target_properties(GtestTests PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    UNITY_BUILD OFF
)
