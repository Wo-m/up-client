cmake_minimum_required(VERSION 3.25)
project(up_client)

# Generate compile_commands.json for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CMake needs explicit decleration of triplet
# and build script included
set(VCPKG_TARGET_TRIPLET arm64-osx)
include(/Users/dominicthompson/vcpkg/scripts/buildsystems/vcpkg.cmake)

# SETUP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
set(CMAKE_CXX_STANDARD 17)  # C++ Standard

set(SOURCES                 # All .cpp files in src/
        src/config/Config.cpp 
        src/service/UpService.cpp)

set(LIBRARY_NAME engine)    # Default name for the library built from src/*.cpp (change if you wish)

add_library(${LIBRARY_NAME} OBJECT ${SOURCES})  # Create library from sources


# LINK >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# - LOCAL SOURCES ---------------------------------------------------------------
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

# - DEPENDENCIES ----------------------------------------------------------------
# Rest
find_package(cpr CONFIG REQUIRED)
target_link_libraries(${LIBRARY_NAME} PRIVATE cpr::cpr)
# Json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC nlohmann_json::nlohmann_json)
# Toml
find_package(toml11 CONFIG REQUIRED)
target_link_libraries(${LIBRARY_NAME} PRIVATE toml11::toml11)
# String fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC fmt::fmt)

# EXECUTABLE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
add_executable(up-client app/main.cpp)                      # Create executable
target_link_libraries(up-client PRIVATE ${LIBRARY_NAME})    # Link
