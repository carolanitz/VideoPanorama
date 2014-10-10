#include "matcher.hpp"

#include "opencv2/highgui/highgui.hpp"

int main(int argc, char** argv)
{
  if (argc < 3) {
    return 2;
  }

  //initialize and allocate memory to load the video stream from camera
  cv::VideoCapture video1(argv[1]);
  cv::VideoCapture video0(argv[2]);

  if(!video0.isOpened()) {
    return 1;
  }
  if(!video1.isOpened()) {
    return 1;
  }

  Matcher matcher;

  while(true) {
    //grab and retrieve each frames of the video sequentially
    cv::Mat3b frame0;
    video0 >> frame0;
    cv::Mat3b frame1;
    video1 >> frame1;

    matcher.updateImage1(frame0, {0, 0, 0, 0});
    matcher.updateImage2(frame1, {0, 0, 0, 0});

    cv::imshow("frame0", frame0);
    cv::imshow("frame1", frame1);

    // wait for 100 milliseconds
    int c = cvWaitKey(100);

    // exit the loop if user press "Esc" key  (ASCII value of "Esc" is 27)
    if(27 == char(c)) break;
  }

  return 0;
}
