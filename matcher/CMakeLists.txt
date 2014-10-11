cmake_minimum_required(VERSION 3.0)
project(Matcher)


add_definitions(-std=c++11)

set(SOURCES
  main.cpp
  matcher.hpp
  matcher.cpp
  fastmatcher.cpp
  fastmatcher.hpp
  slowmatcher.cpp
  slowmatcher.hpp
  painter.hpp
  painter.cpp
  utils.hpp
  utils.cpp)

#add_definitions(-DTARGET_IPHONE_SIMULATOR)
add_definitions(-DTARGET_DACUDAS_LINUX_BUILD_SERVER_PLEASE_RENAME)

add_executable(matcher ${SOURCES})

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system thread)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(3rdparty/eigen3)
target_link_libraries(matcher ${Boost_LIBRARIES})
target_link_libraries(matcher ${OpenCV_LIBS})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
target_link_libraries(matcher ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
