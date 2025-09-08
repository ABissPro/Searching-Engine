cmake_minimum_required(VERSION 3.16)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическое подключение модулей Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Находим Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Test)

# Включаем систему тестирования
enable_testing()

# Добавляем поддиректорию src
add_subdirectory(src)

# Копируем ресурсы и конфигурационные файлы в корень билд-директории
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.json)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION ${CMAKE_BINARY_DIR})
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/requests.json)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/requests.json DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Добавляем заголовочные файлы и JSON файлы в проект для отображения в IDE
file(GLOB_RECURSE HEADER_FILES "include/*.h")
file(GLOB JSON_FILES "*.json")
add_custom_target(project_files SOURCES ${HEADER_FILES} ${JSON_FILES})
