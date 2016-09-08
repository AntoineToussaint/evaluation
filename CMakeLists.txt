cmake_minimum_required (VERSION 2.6)
project (Evaluation)

set(CMAKE_CXX_FLAGS "-std=c++1y")
project (Evaluation)
add_subdirectory (src)

enable_testing ()
add_subdirectory (test)
