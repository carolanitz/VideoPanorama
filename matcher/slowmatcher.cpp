#include "slowmatcher.hpp"

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>


// ----------------------------------------------------------------------------------
QualityMatcher::QualityMatcher()
{

}

// ----------------------------------------------------------------------------------
QualityMatcher::~QualityMatcher()
{

}

// ----------------------------------------------------------------------------------
void QualityMatcher::doTheMagic(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb)
{
  // keypoints 
  std::vector<cv::KeyPoint> featuresSrc;
  std::vector<cv::KeyPoint> featuresDst;
  
  // TODO - use the provided prior
  
  // prefilter slightly
  cv::Mat imgSrc, imgDst;
  cv::medianBlur(imageSrc, imgSrc, 3);
  cv::medianBlur(imageDst, imgDst, 3);
  
  
  // detect
  cv::FAST(imgSrc, featuresSrc, 20, cv::FastFeatureDetector::TYPE_9_16);
  cv::FAST(imgDst, featuresDst, 20, cv::FastFeatureDetector::TYPE_9_16);
      
  if (featuresDst.size() == 0 || featuresSrc.size() == 0)
  {
    cb(false, priorH);
    return;
  }
  
  cv::Mat H = priorH;
  
  std::vector<cv::Point2f> ptsSrc, ptsDst;
  
  cv::KeyPoint::convert(featuresSrc, ptsSrc);
  cv::KeyPoint::convert(featuresDst, ptsDst);
  
  for (cv::Point2f& pt : ptsSrc)
    cv::circle(imgSrc, pt, 5, cv::Scalar(255,0,255));
  
  for (cv::Point2f& pt : ptsDst)
    cv::circle(imgDst, pt, 5, cv::Scalar(255,0,255));
  
  /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::imshow("imgae1", imgSrc);
  cv::imshow("imgae2", imgDst);
  cv::waitKey(0);
  
  cv::Mat H = cv::findHomography(ptsSrc, ptsDst, CV_RANSAC, 5.0);
  */
  cb(true, H);
  
}

// ----------------------------------------------------------------------------------
void QualityMatcher::matchImagesAsync(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb)
{
  if (m_matchingThread)
  {
    m_matchingThread->join();
  }
  
  cv::Mat a = imageSrc.clone();
  cv::Mat b = imageDst.clone();
  
  m_matchingThread.reset(new boost::thread(boost::bind(&QualityMatcher::doTheMagic, this, a, b, priorH, cb)));
}
