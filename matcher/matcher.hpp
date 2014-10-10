#ifndef VIDEO_PANORAMA_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_MATCHER_MATCHER_HPP

#include <array>

#include <opencv2/core/core.hpp>

#include "painter.hpp"

class Matcher {

public:
  Matcher();
  ~Matcher();

  // Set new images
  void updateImage1(cv::Mat image, cv::Vec4f gyro);
  void updateImage2(cv::Mat image, cv::Vec4f gyro);

  // Set OpenGL images
  void setupOpenGL(int width, int height);
  void draw();
  void cleanupOpenGL();

private:
  Painter painter;
};


#endif // VIDEO_PANORAMA_MATCHER_MATCHER_HPP
