#include "fastmatcher.hpp"

FastMatcher::FastMatcher()
{

}

FastMatcher::~FastMatcher()
{
}


void FastMatcher::updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp)
{
  m_image1 = image;
}


void FastMatcher::updateImage2(cv::Mat image, cv::Vec4f oreintationQuaternion, int64_t timestamp)
{
  m_image2 = image;
}

cv::Mat FastMatcher::image1()
{
  return m_image1;
}

cv::Mat FastMatcher::image2()
{
  return m_image2;
}

cv::Vec4f FastMatcher::quaternion1()
{
  return cv::Vec4f();

}

cv::Vec4f FastMatcher::quaternion2()
{
  return cv::Vec4f();
}
