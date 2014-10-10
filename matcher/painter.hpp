#ifndef VIDEO_PANORAMA_MATCHER_PAINTER_HPP
#define VIDEO_PANORAMA_MATCHER_PAINTER_HPP

#include <array>

#include <opencv2/core/core.hpp>

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
};


#endif // VIDEO_PANORAMA_MATCHER_PAINTER_HPP
