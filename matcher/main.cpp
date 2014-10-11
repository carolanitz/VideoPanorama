#include "matcher.hpp"

#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include "opencv2/highgui/highgui.hpp"

cv::VideoCapture video0, video1;
Matcher matcher;

int refreshMillis = 80;      // Refresh period in milliseconds

bool setup = false;

int w = 800;
int h = 600;
/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
  if (!setup)
  {
    matcher.setupOpenGL(w, h);
  }
  matcher.draw();
}

void timer(int) {
  //grab and retrieve each frames of the video sequentially
  cv::Mat3b frame0;
  video0 >> frame0;
  cv::Mat3b frame1;
  video1 >> frame1;

  matcher.updateImage1(frame0, {0, 0, 0, 0});
  matcher.updateImage2(frame1, {0, 0, 0, 0});

  glutTimerFunc(refreshMillis, timer, 0); // subsequent timer call at milliseconds
          glutPostRedisplay();
}

int main(int argc, char* argv[])
{
  if (argc < 3) {
    return 2;
  }

  //initialize and allocate memory to load the video stream from camera
  video0 = cv::VideoCapture(argv[1]);
  video1 = cv::VideoCapture(argv[2]);

  if(!video0.isOpened()) {
    return 1;
  }
  if(!video1.isOpened()) {
    return 1;
  }

  // Drawing
  glutInit(&argc, argv);                 // Initialize GLUT
  glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
  glutInitWindowSize(w, h);   // Set the window's initial width & height
  glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
  glutDisplayFunc(display); // Register display callback handler for window re-paint
  glutTimerFunc(0, timer, 0);   // First timer call immediately
  glutMainLoop();           // Enter the event-processing loop
}
