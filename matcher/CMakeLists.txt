cmake_minimum_required(VERSION 3.0)
project(Matcher)


add_definitions(-std=c++11)

set(SOURCES
  main.cpp
  matcher.hpp
  matcher.cpp
  painter.hpp
  painter.cpp)

add_executable(matcher ${SOURCES})

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(matcher ${Boost_LIBRARIES})
target_link_libraries(matcher ${OpenCV_LIBS})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
target_link_libraries(matcher ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
