cmake_minimum_required(VERSION 3.16)
project(CusaSearcher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Multimedia)
find_package(nlohmann_json REQUIRED)

add_executable(CusaSearcher
    src/main.cpp
    src/MainWindow.cpp src/MainWindow.h
    src/AddGameDialog.cpp src/AddGameDialog.h
    src/FileHandler.cpp src/FileHandler.h
    src/SynonymIndex.cpp src/SynonymIndex.h
    src/utils.cpp src/utils.h
    include/Game.h
)

target_link_libraries(CusaSearcher PRIVATE
    Qt5::Core Qt5::Widgets Qt5::Multimedia
    nlohmann_json::nlohmann_json
)

target_include_directories(CusaSearcher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_custom_command(TARGET CusaSearcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:CusaSearcher>/data
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_utils
    tests/test_utils.cpp
    src/utils.cpp
)
target_link_libraries(test_utils PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Multimedia
    GTest::gtest_main
)
target_include_directories(test_utils PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Тест SynonymIndex
add_executable(test_synonym_index
    tests/test_synonym_index.cpp
    src/SynonymIndex.cpp
    src/utils.cpp
)
target_link_libraries(test_synonym_index PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Multimedia
    GTest::gtest_main
    nlohmann_json::nlohmann_json
)
target_include_directories(test_synonym_index PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Тест FileHandler
add_executable(test_filehandler
    tests/test_filehandler.cpp
    src/FileHandler.cpp
    src/utils.cpp
)
target_link_libraries(test_filehandler PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Multimedia
    GTest::gtest_main
    nlohmann_json::nlohmann_json
)
target_include_directories(test_filehandler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(test_utils)
gtest_discover_tests(test_synonym_index)
gtest_discover_tests(test_filehandler)