cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(GameOFLife_proj VERSION 0.4 LANGUAGES CXX)
set(CMAKE_CXX STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
 googletest
 URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
 # SOURCE_DIR "<path_to>/googletest-main"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

option(ENABLE_TEST "Enable tests generation" ON)
if (ENABLE_TEST)
 enable_testing()
endif()

add_library(life_lib STATIC Life.h Life.cpp)

add_executable(GameOfLife Main.cpp)
if (ENABLE_TEST)
 add_executable(life_test LifeTest.cpp)
endif()

target_link_libraries(life_test GTest::gtest_main life_lib)
target_link_libraries(GameOfLife life_lib)

include(GoogleTest)
gtest_discover_tests(life_test)
