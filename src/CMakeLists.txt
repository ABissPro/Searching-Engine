# Явно перечисляем файлы для основного приложения
set(MAIN_SOURCE_FILES
    converterjson.cpp
    invertedindex.cpp
    main.cpp
    searchserver.cpp
)

# Явно перечисляем файлы для тестового приложения
set(TEST_SOURCE_FILES
    converterjson.cpp
    invertedindex.cpp
    searchserver.cpp
    test_main.cpp
)

# Основное приложение
add_executable(SearchEngine ${MAIN_SOURCE_FILES})
target_link_libraries(SearchEngine PRIVATE Qt6::Core)
set_target_properties(SearchEngine PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_include_directories(SearchEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/nlohmann_json/json-develop/include
)

# Тестовое приложение
add_executable(SearchEngineTests ${TEST_SOURCE_FILES})
target_link_libraries(SearchEngineTests PRIVATE Qt6::Test Qt6::Core)
set_target_properties(SearchEngineTests PROPERTIES
    AUTOMOC ON
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_include_directories(SearchEngineTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/nlohmann_json/json-develop/include
)

# Добавляем тест
add_test(NAME QtTests COMMAND SearchEngineTests)
add_test(NAME SearchServerTests COMMAND SearchEngineTests)
