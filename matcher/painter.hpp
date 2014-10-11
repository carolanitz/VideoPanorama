#ifndef VIDEO_PANORAMA_MATCHER_PAINTER_HPP
#define VIDEO_PANORAMA_MATCHER_PAINTER_HPP

#include <array>
#include <mutex>

#include <opencv2/core/core.hpp>


#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

class Painter {

public:
  Painter();
  ~Painter();

  // Set new images
  void updateImage1(cv::Mat image);
  void updateImage2(cv::Mat image);
  void updateHomography1(cv::Mat H);
  void updateHomography2(cv::Mat H);

  // Set OpenGL images
  void setupOpenGL(int width, int height);
  void draw();
  void cleanupOpenGL();

private:
  std::mutex m_mutex;

  cv::Mat m_image1, m_image2;

  bool m_texture1Created = false;
  bool m_texture2Created = false;

  GLuint m_shaderProgram;
  GLuint m_textures[2];

};


#endif // VIDEO_PANORAMA_MATCHER_PAINTER_HPP
