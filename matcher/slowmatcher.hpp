#ifndef VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>

class SlowMatcher {

public:
  SlowMatcher();
  ~SlowMatcher();

  // Set new images
  void updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp);
  void updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp);
};


#endif // VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
