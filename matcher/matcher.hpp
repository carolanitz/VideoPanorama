#ifndef VIDEO_PANORAMA_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>

#include "painter.hpp"
#include "fastmatcher.hpp"
#include "slowmatcher.hpp"

class Matcher {

public:
  Matcher();
  ~Matcher();

  // Set new images
  void updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp);
  void updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp);

  // Set OpenGL images
  void setupOpenGL(int width, int height);
  void draw();
  void cleanupOpenGL();

private:
  Painter m_painter;
  FastMatcher m_fastMatcher;
  SlowMatcher m_slowMatcher;
  std::mutex m_mutex;
};


#endif // VIDEO_PANORAMA_MATCHER_MATCHER_HPP
