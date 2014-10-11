#ifndef VIDEO_PANORAMA_FAST_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_FAST_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>

class FastMatcher {

public:
  FastMatcher();
  ~FastMatcher();

  // Set new images
  void updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp);
  void updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp);
};


#endif // VIDEO_PANORAMA_FAST_MATCHER_MATCHER_HPP
