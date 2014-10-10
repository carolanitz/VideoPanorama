cmake_minimum_required(VERSION 3.0)
project(Matcher)

find_package(OpenCV REQUIRED)

add_definitions(-std=c++11)

set(SOURCES
  main.cpp
  matcher.hpp
  matcher.cpp)

add_executable(matcher ${SOURCES})

target_link_libraries(matcher ${OpenCV_LIBS})
