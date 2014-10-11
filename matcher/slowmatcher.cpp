#include "slowmatcher.hpp"

SlowMatcher::SlowMatcher()
{

}

SlowMatcher::~SlowMatcher()
{

}

void SlowMatcher::updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion)
{
  m_image1 = image;
}


void SlowMatcher::updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion)
{
  m_image2 = image;
}

cv::Mat SlowMatcher::image1()
{
  return m_image1;
}

cv::Mat SlowMatcher::image2()
{
  return m_image2;
}

cv::Mat SlowMatcher::homography1()
{
  return cv::Mat();
}

cv::Mat SlowMatcher::homography2()
{
  return cv::Mat();
}
