#include "painter.hpp"

#include <GL/gl.h>

Painter::Painter()
{

}

Painter::~Painter()
{

}

void Painter::updateImage1(cv::Mat image)
{

}

void Painter::updateImage2(cv::Mat image)
{

}

void Painter::updateHomography1(cv::Mat H)
{

}

void Painter::updateHomography2(cv::Mat H)
{

}

void Painter::setupOpenGL(int width, int height)
{
  std::lock_guard<std::mutex> lock(m_mutex);
}

void Painter::draw()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)

  // Draw a Red 1x1 Square centered at origin
  glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
     glColor3f(1.0f, 0.0f, 0.0f); // Red
     glVertex2f(-0.5f, -0.5f);    // x, y
     glVertex2f( 0.5f, -0.5f);
     glVertex2f( 0.5f,  0.5f);
     glVertex2f(-0.5f,  0.5f);
  glEnd();

  glFlush();  // Render now
}

void Painter::cleanupOpenGL()
{
  std::lock_guard<std::mutex> lock(m_mutex);
}
