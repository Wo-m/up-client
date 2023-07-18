cmake_minimum_required(VERSION 3.25)
project(up_client)

# SETUP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
set(CMAKE_CXX_STANDARD 17)  # C++ Standard

set(SOURCES                 # All .cpp files in src/
        src/UpDao.cpp)

set(LIBRARY_NAME engine)    # Default name for the library built from src/*.cpp (change if you wish)

add_library(${LIBRARY_NAME} OBJECT ${SOURCES})  # Create library from sources


# LINK >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# - LOCAL SOURCES ---------------------------------------------------------------
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# - DEPENDENCIES ----------------------------------------------------------------
# Rest
find_package(cpr CONFIG REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC cpr::cpr)
# Json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC nlohmann_json::nlohmann_json)


# EXECUTABLE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
add_executable(up-client app/main.cpp)                      # Create executable
target_link_libraries(up-client PRIVATE ${LIBRARY_NAME})    # Link