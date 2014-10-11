#include "utils.hpp"

#include <Eigen/LU>

namespace utils
{
  
// ----------------------------------------------------------------------------------
cv::Mat cameraPoseFromHomography(const cv::Mat& H)
{
  cv::Mat pose = cv::Mat::eye(3, 4, CV_32FC1); //3x4 matrix
  float norm1 = (float)cv::norm(H.col(0)); 
  float norm2 = (float)cv::norm(H.col(1));
  float tnorm = (norm1 + norm2) / 2.0f;
  
  cv::Mat v1 = H.col(0);
  cv::Mat v2 = pose.col(0);
  
  cv::normalize(v1, v2); // Normalize the rotation
  
  v1 = H.col(1);
  v2 = pose.col(1);
  
  cv::normalize(v1, v2);
  
  v1 = pose.col(0);
  v2 = pose.col(1);
    
  cv::Mat v3 = v1.cross(v2);  //Computes the cross-product of v1 and v2
  cv::Mat c2 = pose.col(2);
  v3.copyTo(c2);      
  
  pose.col(3) = H.col(2) / tnorm; //vector t [R|t]
  
  return pose;
}

}
