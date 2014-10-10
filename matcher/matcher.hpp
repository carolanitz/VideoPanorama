#ifndef VIDEO_PANORAMA_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_MATCHER_MATCHER_HPP

#include <array>

#include <opencv2/core/core.hpp>

class Matcher {

public:
  Matcher();
  ~Matcher();

  // Set new images
  void updateImage1(cv::Mat image, std::array<float, 4> gyro);
  void updateImage2(cv::Mat image, std::array<float, 4> gyro);

  // Set OpenGL images
  void setupOpenGL(int width, int height);
  void draw();
  void cleanupOpenGL();
};


#endif // VIDEO_PANORAMA_MATCHER_MATCHER_HPP
