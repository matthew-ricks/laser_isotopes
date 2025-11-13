cmake_minimum_required(VERSION 3.10)

project(Ac225Production)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Geant4 installation path
set(Geant4_DIR /home/mttricks/G4/geant4-v11.3.1-build)
find_package(Geant4 REQUIRED)

# Add the Geant4 include and library directories
include_directories(${Geant4_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)  # Include the 'include' directory
link_directories(${Geant4_LIBRARY_DIRS})

# Add source files
file(GLOB SOURCES src/*.cc)

# Create the executable
add_executable(Ac225Production ${SOURCES})

# Link Geant4 libraries
target_link_libraries(Ac225Production ${Geant4_LIBRARIES})


