cmake_minimum_required(VERSION 3.20)
project(LaunchpadFYPArbitrage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(arbitrage_lib
  src/Token.cpp
  src/Pool.cpp
  src/Graph.cpp
  src/CycleDetector.cpp
  src/JSONParser.cpp
  src/ArbitrageAnalyzer.cpp
  src/HTMLExporter.cpp
)

target_include_directories(arbitrage_lib PUBLIC src)
target_link_libraries(arbitrage_lib PUBLIC nlohmann_json::nlohmann_json)

add_executable(arbitrage_app src/main.cpp)
target_link_libraries(arbitrage_app PRIVATE arbitrage_lib)

include(CTest)
if(BUILD_TESTING)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(arbitrage_tests
    tests/test_main.cpp
    tests/test_Token.cpp
    tests/test_Pool.cpp
    tests/test_Graph.cpp
    tests/test_CycleDetector.cpp
  )

  target_include_directories(arbitrage_tests PRIVATE src)
  target_link_libraries(arbitrage_tests PRIVATE arbitrage_lib GTest::gtest GTest::gtest_main)

  add_test(NAME arbitrage_tests COMMAND arbitrage_tests)
endif()
