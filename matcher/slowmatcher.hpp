#ifndef VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>

class SlowMatcher {

public:
  SlowMatcher();
  ~SlowMatcher();

  // Set new images
  void updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion);
  void updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion);

  cv::Mat image1();
  cv::Mat image2();

  cv::Mat homography1();
  cv::Mat homography2();

private:
  cv::Mat m_image1;
  cv::Mat m_image2;
};


#endif // VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
