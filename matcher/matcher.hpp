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
  QualityMatcher m_qualityMatcher;
  std::mutex m_mutex;

  
  cv::Mat m_lastImage[2];
  cv::Mat m_H_1to2;
  
  // called from the slow matcher when matching is finished
  void matched1to2(bool, cv::Mat H);
  void matched2to1(bool, cv::Mat H);

  bool m_matcherAvalable;
};


#endif // VIDEO_PANORAMA_MATCHER_MATCHER_HPP
