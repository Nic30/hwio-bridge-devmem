cmake_minimum_required(VERSION 3.0)

project(hwio_devmem)


# C++14 standard should be safely usable
set(CMAKE_CXX_STANDARD 14)

#Warning flags for all builds
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# collect sources
set(hwio_devmem_SOURCES
  ./hwio_devmem.cpp
)

# find required libraries
find_package(hwio REQUIRED)

# define executable and dependencies
add_executable(hwio_devmem ${hwio_devmem_SOURCES})
target_link_libraries(hwio_devmem hwio)

# define installation
install(TARGETS hwio_devmem DESTINATION bin)
