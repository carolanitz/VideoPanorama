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

  cv::Mat image1();
  cv::Mat image2();

  cv::Vec4f quaternion1();
  cv::Vec4f quaternion2();

private:
  cv::Mat m_image1;
  cv::Mat m_image2;
};


#endif // VIDEO_PANORAMA_FAST_MATCHER_MATCHER_HPP
