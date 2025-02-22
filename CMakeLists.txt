cmake_minimum_required(VERSION 3.17)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the project name
project(Kvstore)

# Add the executable
add_executable(Kvstore Kvstore.cpp)
