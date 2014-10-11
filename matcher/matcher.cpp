#include "matcher.hpp"

#include <functional>

Matcher::Matcher()
    : m_matcherAvalable(true)
{
  m_H_1to2 = cv::Mat::eye(3,3,CV_32FC1);
  
  m_painter.updateHomography1(m_H_1to2);
  m_painter.updateHomography2(m_H_1to2);
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
  m_painter.cleanupOpenGL ();
}

// ----------------------------------------------------------------------------------
void Matcher::updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  // TODO update position of both images
  m_lastImage[0] = image;

    // ----
    // ----

  //m_fastMatcher.updateImage1(image, orientationQuaternion, timestamp);

  using namespace std::placeholders;  // for _1, _2, _3...

  // if quality matcher is avalable, then send new image to it
  if (m_matcherAvalable && !m_lastImage[1].empty())
  {
    m_qualityMatcher.matchImagesAsync(m_lastImage[0], m_lastImage[1], m_H_1to2, 
        std::bind(&Matcher::matched1to2, this, _1, _2));
    m_matcherAvalable = false;
  }
  m_painter.updateImage1(image);
}

// ----------------------------------------------------------------------------------
void Matcher::updateImage2(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  m_lastImage[1] = image;
  
  using namespace std::placeholders;  // for _1, _2, _3...

  if (m_matcherAvalable && !m_lastImage[0].empty())
  {
    m_qualityMatcher.matchImagesAsync(m_lastImage[1], m_lastImage[0], m_H_1to2.inv(), 
        std::bind(&Matcher::matched2to1, this, _1, _2));
    m_matcherAvalable = false;
  }
  m_painter.updateImage2(image);
}

// ----------------------------------------------------------------------------------
void Matcher::matched1to2(bool valid, cv::Mat H)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!valid) return;
  
  m_H_1to2 = H;
  m_painter.updateHomography2(H.inv());  
  m_matcherAvalable = true;  
}

// ----------------------------------------------------------------------------------
void Matcher::matched2to1(bool valid, cv::Mat H)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!valid) return;

  m_H_1to2 = H.inv();
  m_painter.updateHomography2(H);  
  m_matcherAvalable = true;  
}

