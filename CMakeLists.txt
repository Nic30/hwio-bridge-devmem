cmake_minimum_required(VERSION 3.0)

project(hwio-devmem)


# C++14 standard should be safely usable
set(CMAKE_CXX_STANDARD 14)

#Warning flags for all builds
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# collect sources
set(hwio-devmem_SOURCES
  ./hwio_devmem.cpp
)

# find required libraries
find_package(hwio REQUIRED)

# define executable and dependencies
add_executable(hwio-devmem ${hwio-devmem_SOURCES})
target_link_libraries(hwio-devmem hwio)

# define installation
install(TARGETS hwio-devmem DESTINATION bin)
