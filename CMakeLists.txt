cmake_minimum_required(VERSION 3.20)
project(CarbonGrid CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Find all cpp files
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

# Core library
add_library(CarbonGridLib STATIC ${ALL_SOURCES})

# Create the main executable (CarbonGrid)
add_executable(CarbonGrid src/main.cpp)
target_link_libraries(CarbonGrid CarbonGridLib)

# Windows specific networking library
if(WIN32)
    target_link_libraries(CarbonGrid ws2_32)
    target_link_libraries(CarbonGridLib ws2_32)
endif()

# Tests target
if(TEST_SOURCES)
    add_executable(CarbonGridTests ${TEST_SOURCES})
    target_link_libraries(CarbonGridTests CarbonGridLib)
    if(WIN32)
        target_link_libraries(CarbonGridTests ws2_32)
    endif()
endif()
