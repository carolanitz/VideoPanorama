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
  painter.setupOpenGL(width, height);
}

void Matcher::draw()
{
  // Thread safe
  painter.draw();
}

void Matcher::cleanupOpenGL()
{
  // Thread safe
  painter.cleanupOpenGL();
}

void Matcher::updateImage2(cv::Mat image, cv::Vec4f oreintationQuaternion, int64_t timestamp)
{
  painter.updateImage1(image);
}

void Matcher::updateImage1(cv::Mat image, cv::Vec4f orientationQuaternion, int64_t timestamp)
{
  painter.updateImage2(image);
}
