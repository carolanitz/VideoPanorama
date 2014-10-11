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
  
  cv::Mat descriptorsSrc, descriptorsDst;
  
  // detect
  /*cv::ORB detector(200,1.5f,6);
  detector.compute(imgSrc, cv::Mat(), featuresSrc, descriptorsSrc);
  detector.compute(imgDst, cv::Mat(), featuresDst, descriptorsDst);
  */
  
  // features
  cv::FAST(imgSrc, featuresSrc, 50, cv::FastFeatureDetector::TYPE_9_16);
  cv::FAST(imgDst, featuresDst, 50, cv::FastFeatureDetector::TYPE_9_16);
  
  // descriptors
  cv::BriefDescriptorExtractor brief;
  brief.compute(imgSrc, featuresSrc, descriptorsSrc);  
  brief.compute(imgDst, featuresDst, descriptorsDst);
  
  if (featuresDst.size() == 0 || featuresSrc.size() == 0
      || descriptorsSrc.rows != featuresSrc.size()
      || descriptorsDst.rows != featuresDst.size())
  {
    cb(false, priorH);
    return;
  }
  
  // matching (simple nearest neighbours)
  cv::FlannBasedMatcher matcher(new cv::flann::LshIndexParams(20,10,4));
  std::vector< cv::DMatch > matches;
  matcher.match( descriptorsSrc, descriptorsDst, matches );
  
  // extract good matches  
  std::vector<cv::Point2f> ptsSrc, ptsDst;
  for( int i = 0; i < matches.size(); i++ )
  {
    ptsSrc.push_back( featuresSrc[ matches[i].queryIdx ].pt );
    ptsDst.push_back( featuresDst[ matches[i].trainIdx ].pt );
  }
  
  
  cv::Mat H = priorH;
  
  for (cv::Point2f& pt : ptsSrc)
    cv::circle(imgSrc, pt, 5, cv::Scalar(255,0,255));
  
  for (cv::Point2f& pt : ptsDst)
    cv::circle(imgDst, pt, 5, cv::Scalar(255,0,255));
  
  /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::imshow("imgae1", imgSrc);
  cv::imshow("imgae2", imgDst);
  cv::waitKey(0); */
  
  H = cv::findHomography(ptsSrc, ptsDst, CV_RANSAC, 5.0);
  
  cb(true, H);
  
}

// ----------------------------------------------------------------------------------
void QualityMatcher::matchImagesAsync(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb)
{
  if (m_matchingThread)
  {
    m_matchingThread->join();
  }
  
  // extract one channel for matching -> better have YUV, but green channel is god enough
  cv::Mat rgbSrc[3];
  cv::Mat rgbDst[3];
  cv::split(imageSrc, rgbSrc);
  cv::split(imageDst, rgbDst);
    
  m_matchingThread.reset(new boost::thread(boost::bind(&QualityMatcher::doTheMagic, this, rgbSrc[1], rgbDst[1], priorH, cb)));
}
