cmake_minimum_required(VERSION 3.22)
project(del_poc_subscriber CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find IceOryx2 - adjust path if not installed system-wide
find_package(iceoryx2-cxx QUIET)
if(NOT iceoryx2-cxx_FOUND)
    # Fall back to local build in the submodule
    set(IOX2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../iceoryx2_src")
    if(EXISTS "${IOX2_DIR}/CMakeLists.txt")
        message(STATUS "Using local iceoryx2 from ${IOX2_DIR}")
        add_subdirectory(${IOX2_DIR} ${CMAKE_BINARY_DIR}/iceoryx2)
    else()
        message(FATAL_ERROR
            "IceOryx2 not found. Install it or ensure the submodule "
            "at utils/iceoryx2_src/ is checked out.")
    endif()
endif()

add_executable(poc_subscriber poc_subscriber.cpp)
target_link_libraries(poc_subscriber PRIVATE iceoryx2-cxx::iceoryx2-cxx)

# Build instructions:
#   cd utils/iceoryx2_poc_subscriber
#   cmake -S . -B build
#   cmake --build build
#   ./build/poc_subscriber
