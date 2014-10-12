#include "matcher.hpp"
#include "utils.hpp"

#include <opencv2/core/eigen.hpp>

#include <Eigen/LU>

#include <functional>
#include <iostream>

//#define DEBUG_SENSORS

// ----------------------------------------------------------------------------------
Matcher::Matcher()
    : m_tracking(false) 
    , m_matcherAvalable(true)
{
  m_H_1to2 = cv::Mat::eye(3,3,CV_32FC1);
  m_H_prior = cv::Mat::eye(3,3,CV_32FC1);
  
  m_painter.updateHomography1(m_H_1to2);
  m_painter.updateHomography2(m_H_1to2);
  
  m_lastOrientation[0] = Eigen::Quaternionf::Identity();
  m_lastOrientation[1] = Eigen::Quaternionf::Identity();
  m_sumOrientation[0] = Eigen::Quaternionf::Identity();
  m_sumOrientation[1] = Eigen::Quaternionf::Identity();
  
  
  m_K <<
      1081.6440045, 0, 632.85466506,
      0, 1081.6440045, 362.19709930,
      0, 0, 1;
  
   //m_K.block<3,1>(0,2) *= -1;
   
  m_iK = m_K.inverse();
}

// ----------------------------------------------------------------------------------
Matcher::~Matcher()
{
}

// ----------------------------------------------------------------------------------
void Matcher::setupOpenGL(int width, int height)
{
  m_painter.setupOpenGL(width, height);
}

// ----------------------------------------------------------------------------------
void Matcher::draw()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  
  m_painter.draw();
  std::cout.flush();
}

// ----------------------------------------------------------------------------------
void Matcher::cleanupOpenGL()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  
  m_painter.cleanupOpenGL ();
}

// ----------------------------------------------------------------------------------
void Matcher::updateIntermediate()
{
  // H matrix as came from the matcher so far
  Eigen::Matrix3f H;
  cv::cv2eigen<float,3,3>(m_H_1to2, H);
  
  // we have collected sensor data so far -> 
   Eigen::Quaternionf R = m_sumOrientation[0] * m_sumOrientation[1].inverse();
    
  // move matched poitns even further (based on the sensors)
  H = m_K * utils::makeRotX3((float)M_PI_2) * R.toRotationMatrix() * utils::makeRotX3(-(float)M_PI_2) * m_iK * H;
  H /= H(2,2);
  
  float a = 0.5; // totally incorrect (linearly interpolating H matrix, ha ha ha)
  for (int i=0; i < 3; i++)
    for (int j=0; j < 3; j++)
      H(i,j) = H(i,j) * a + m_lastH(i,j) * (1.0 - a);
  
  m_lastH = H;
    
  cv::Mat cvH;
  cv::eigen2cv<float,3,3>(H, cvH);
  m_H_prior = cvH.inv();
  
  m_painter.updateHomography2(cvH.inv());  
  //m_painter.updateHomography1(cvH.inv());  // one is fixed for now
}

// ----------------------------------------------------------------------------------
/*void Matcher::prepareMatch()
{
  // start collection of gyros, which will be used for intermediate position
  // update while the matcher is running
  m_sumOrientation[0] = Eigen::Quaternionf::Identity();
  m_sumOrientation[1] = Eigen::Quaternionf::Identity();
}
*/

// ----------------------------------------------------------------------------------
void Matcher::updateImage1(cv::Mat image, cv::Vec4f rq, cv::Vec3f g, int64_t timestamp)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  
   std::cout << rq << std::endl;
  // accumulate orientation
  //Eigen::Quaternionf q(rq[3],rq[1],rq[2],rq[0]); // iOS sensors
  Eigen::Quaternionf q(rq[3],rq[0],rq[1],rq[2]);
  Eigen::Quaternionf dq = q * m_lastOrientation[0].inverse();
  m_lastOrientation[0] = q;
  if (!m_lastImage[0].empty())
  {
    m_sumOrientation[0] = dq * m_sumOrientation[0];
  }
  m_lastImage[0] = image;

  using namespace std::placeholders;  // for _1, _2, _3...

  // if quality matcher is avalable, then send new image to it
  if (m_matcherAvalable && !m_lastImage[1].empty())
  {
#ifndef DEBUG_SENSORS
    //prepareMatch();
    m_qualityMatcher.matchImagesAsync(m_lastImage[0], m_lastImage[1], m_H_prior, 
        std::bind(&Matcher::matched1to2, this, _1, _2));
    m_matcherAvalable = false;
#endif
  }
  
  if (m_tracking)
  {
    updateIntermediate();
  }
   m_painter.updateImage1(image);

}

// ----------------------------------------------------------------------------------
void Matcher::updateImage2(cv::Mat image, cv::Vec4f rq, cv::Vec3f g, int64_t timestamp)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  
  // accumulate orientation
  //Eigen::Quaternionf q(rq[3],rq[1],rq[2],rq[0]); // iOS sensors
  Eigen::Quaternionf q(rq[3],rq[0],rq[1],rq[2]);
  Eigen::Quaternionf dq = q * m_lastOrientation[1].inverse();
  m_lastOrientation[1] = q;
  if (!m_lastImage[1].empty())
  {
    m_sumOrientation[1] = dq * m_sumOrientation[1];
  }
  
  m_lastImage[1] = image;
  using namespace std::placeholders;  // for _1, _2, _3...

  if (m_matcherAvalable && !m_lastImage[0].empty()
    #ifdef DEBUG_SENSORS
      && !m_tracking
    #endif
      )
  {
    //prepareMatch();
    m_qualityMatcher.matchImagesAsync(m_lastImage[1], m_lastImage[0], m_H_prior.inv(), 
        std::bind(&Matcher::matched2to1, this, _1, _2));
    m_matcherAvalable = false;
  }
  
  if (m_tracking)
  {
    updateIntermediate();
  }
   m_painter.updateImage2(image);
}

// ----------------------------------------------------------------------------------
void Matcher::updateAndFixH(cv::Mat cvH)
{
  // H matrix as came from the matcher so far
  Eigen::Matrix3f H;
  cv::cv2eigen<float,3,3>(cvH, H);
  
  // we have collected sensor data so far -> 
   Eigen::Quaternionf R = m_sumOrientation[0] * m_sumOrientation[1].inverse();
    
  // move matched poitns even further (based on the sensors)
  H = m_K * utils::makeRotX3((float)M_PI_2) * R.toRotationMatrix() * utils::makeRotX3(-(float)M_PI_2) * m_iK * H;
  H /= H(2,2);
  
  float a = 0.5; // totally incorrect (linearly interpolating H matrix, ha ha ha)
  for (int i=0; i < 3; i++)
    for (int j=0; j < 3; j++)
      H(i,j) = H(i,j) * a + m_lastH(i,j) * (1.0 - a);
  
  m_lastH = H;
  
  cv::eigen2cv<float,3,3>(H, m_H_1to2);

  m_sumOrientation[0] = Eigen::Quaternionf::Identity();
  m_sumOrientation[1] = Eigen::Quaternionf::Identity();
}

// ----------------------------------------------------------------------------------
void Matcher::matched1to2(bool valid, cv::Mat H)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  
  m_matcherAvalable = true;  
  m_tracking = true;
  
  if (!valid)
  {
     updateAndFixH(m_H_1to2); // update with old
     return;
  }
  
  H.convertTo(m_H_1to2, CV_32FC1);
  updateAndFixH(m_H_1to2); // update with new
}

// ----------------------------------------------------------------------------------
void Matcher::matched2to1(bool valid, cv::Mat H)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  
  m_matcherAvalable = true;  
  m_tracking = true;
  
  if (!valid)
  {
    updateAndFixH(m_H_1to2); // update with old
    return;
  }

  H = H.inv();
  H.convertTo(m_H_1to2, CV_32FC1);
  updateAndFixH(m_H_1to2); // update with new  
}

