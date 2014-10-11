#include "matcher.hpp"

Matcher::Matcher()
{
}

Matcher::~Matcher()
{
}

void Matcher::setupOpenGL(int width, int height)
{
  // Thread safe
  m_painter.setupOpenGL(width, height);
}

void Matcher::draw()
{
  // Thread safe
  m_painter.draw();
}

void Matcher::cleanupOpenGL()
{
  // Thread safe
  m_painter.cleanupOpenGL();
}

void Matcher::updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  m_fastMatcher.updateImage1(image, orientationQuaternion, timestamp);
  m_slowMatcher.updateImage1(m_fastMatcher.image1(), m_fastMatcher.quaternion1());
  m_painter.updateImage1(m_slowMatcher.image1());
  m_painter.updateHomography1(m_slowMatcher.homography1());
}

void Matcher::updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  m_fastMatcher.updateImage2(image, orientationQuaternion, timestamp);
  m_slowMatcher.updateImage2(m_fastMatcher.image2(), m_fastMatcher.quaternion2());
  m_painter.updateImage2(m_slowMatcher.image2());
  m_painter.updateHomography2(m_slowMatcher.homography2());
}
