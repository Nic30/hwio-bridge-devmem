# libhwio-devmem

Devmem tool for libhwio the library for unified access to hardware resources

This tool has same functionality as known linux tool devmem,
but instead of address it uses compatibility string and offset from base address of device.
Use -h option to find out more.


## Installation

This project has CMake managed build without anything special.
You need to have [libhwio](https://github.com/Nic30/libhwio) installed (and there is a description how to build and install CMake libs. as well).
