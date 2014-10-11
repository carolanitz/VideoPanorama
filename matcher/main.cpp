#include "matcher.hpp"

#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include "opencv2/highgui/highgui.hpp"

#include <vector>
#include <string>
#include <boost/filesystem.hpp>

struct Data
{
   cv::Mat img;
   cv::Vec3f gyro;
   int64_t timestamp;
   cv::Vec4f orientation;
};

std::vector<Data> input[2];
size_t inputIndex[2] = {0,0};

cv::VideoCapture video0, video1;
Matcher matcher;

int refreshMillis = 80;      // Refresh period in milliseconds

bool setup = false;

int w = 800;
int h = 600;
/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display()
{
  if (!setup)
  {
    matcher.setupOpenGL(w, h);
    setup = true;
  }
  matcher.draw();
  glutSwapBuffers();
}

// -------------------------------------------------------------------------------------------------
void timer(int)
{
  cv::Mat frame0, frame1;
  cv::Vec4f orientation0(0,0,0,0);
  cv::Vec4f orientation1(0,0,0,0);
  int64_t time0 = 0, time1 = 0;
  
  //grab and retrieve each frames of the video sequentially
  if (!input[0].empty() && !input[1].empty())
  {
    frame0 = input[0][inputIndex[0]].img;
    frame1 = input[1][inputIndex[0]].img;
    
    time0 = input[0][inputIndex[0]].timestamp;
    time1 = input[1][inputIndex[0]].timestamp;
    
    orientation0 = input[0][inputIndex[0]].orientation;
    orientation1 = input[1][inputIndex[0]].orientation;
    
    inputIndex[0] = (inputIndex[0] + 1) % input[0].size();
    inputIndex[1] = (inputIndex[1] + 1) % input[1].size();
  }else
  {
    video0 >> frame0;
    video1 >> frame1;
  }
    
  matcher.updateImage1(frame0, orientation0, time0);
  matcher.updateImage2(frame1, orientation1, time1);

  glutTimerFunc(refreshMillis, timer, 0); // subsequent timer call at milliseconds
  glutPostRedisplay();
}

// -------------------------------------------------------------------------------------------------
bool readInput(const std::string& in1, const std::string& in2)
{
    namespace fs = boost::filesystem;
    std::string dir[2];
    dir[0] = in1;
    dir[1] = in2;

    for (int i=0; i < 2; i++)
    {
        fs::path path = dir[i];
        std::vector<fs::path> in_png;

        if (fs::is_directory(path))
        {
           fs::recursive_directory_iterator it(path);
           fs::recursive_directory_iterator endit;
           while(it != endit)
           {
              if (fs::is_regular_file(*it) && (it->path().extension() == ".png"))
              {
                 in_png.push_back(it->path());
              }
              ++it;
           }
        }

        std::sort(in_png.begin(), in_png.end());

        // import
        for (fs::path& filepath : in_png)
        {
           std::string imgfile = filepath.string();
           std::string datfile = filepath.replace_extension(".dat").string();

           Data d;
           d.img = cv::imread(imgfile);
           
           FILE* dat = fopen(datfile.c_str(), "r");
                
           fscanf(dat, "%ld\n", &d.timestamp);
           fscanf(dat, "%f %f %f %f\n", &d.orientation[0], &d.orientation[1], &d.orientation[2], &d.orientation[3]);
           fscanf(dat, "%f %f %f\n", &d.gyro[0], &d.gyro[1], &d.gyro[2]);
           fclose(dat);
           
           input[i].push_back(d);
        }
    }
    
    return !input[0].empty() && !input[1].empty();
}

// -------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  if (argc < 3) {
    return 2;
  }

  //initialize and allocate memory to load the video stream from camera
  video0 = cv::VideoCapture(argv[1]);
  video1 = cv::VideoCapture(argv[2]);

  if(!video0.isOpened() || !video1.isOpened())
  {
    if (!readInput(argv[1], argv[2]))
      return 1;
  }

  // Drawing
  glutInit(&argc, argv);                 // Initialize GLUT
  glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
  glutInitWindowSize(w, h);   // Set the window's initial width & height
  glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
  glutDisplayFunc(display); // Register display callback handler for window re-paint
  glutTimerFunc(0, timer, 0);   // First timer call immediately
  //glutIdleFunc(timer);
  glutMainLoop();           // Enter the event-processing loop
}
