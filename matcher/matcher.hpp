#ifndef VIDEO_PANORAMA_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

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
  QualityMatcher m_qualityMatcher;
  std::mutex m_mutex;

  Eigen::Quaternionf m_lastOrientation[2];
  Eigen::Quaternionf m_sumOrientation[2];
  
  
  cv::Mat m_lastImage[2];
  cv::Mat m_H_1to2;
  Eigen::Matrix3f m_K;
  Eigen::Matrix3f m_iK;
  
  // called from the slow matcher when matching is finished
  void matched1to2(bool, cv::Mat H);
  void matched2to1(bool, cv::Mat H);
  void prepareMatch();
  
  //! update homographies based on intermediate orientation data while matcher is running  
  void updateIntermediate();
  
  bool m_tracking;
  bool m_matcherAvalable;
};


#endif // VIDEO_PANORAMA_MATCHER_MATCHER_HPP
