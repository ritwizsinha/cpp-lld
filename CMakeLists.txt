cmake_minimum_required(VERSION 3.17)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)  # Default to Debug if not specified
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Set the project name
project(TicTacToe)

# Add the executable
add_executable(TicTacToe tic_tac_toe.cpp)
