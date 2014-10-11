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
  if (m_matchingThread)
  {
    m_matchingThread->join();
  }
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
  cv::medianBlur(imageSrc, imgSrc, 5);
  cv::medianBlur(imageDst, imgDst, 5);
  
  cv::Mat descriptorsSrc, descriptorsDst;
  
  // detect
  /*cv::ORB detector(200,1.5f,6);
  detector.compute(imgSrc, cv::Mat(), featuresSrc, descriptorsSrc);
  detector.compute(imgDst, cv::Mat(), featuresDst, descriptorsDst);
  */
  
  // features
  cv::FAST(imgSrc, featuresSrc, 40, cv::FastFeatureDetector::TYPE_9_16);
  cv::FAST(imgDst, featuresDst, 40, cv::FastFeatureDetector::TYPE_9_16);
  
  printf("input %d vs %d\n", featuresSrc.size(), featuresDst.size());
  
  // descriptors
  cv::BriefDescriptorExtractor brief;
  brief.compute(imgSrc, featuresSrc, descriptorsSrc);  
  brief.compute(imgDst, featuresDst, descriptorsDst);
  
  if (featuresDst.size() < 10 || featuresSrc.size() < 10
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
  float min_dist = FLT_MAX;
  for( size_t i = 0; i < featuresSrc.size(); i++ )
  { 
    min_dist = std::min(min_dist, matches[i].distance);
  }
  
  std::vector< cv::DMatch > goodMatches;
  std::vector<cv::Point2f> ptsSrc, ptsDst;
  for( int i = 0; i < matches.size(); i++ )
  {
    //if( matches[i].distance <= std::max(2. * min_dist, 0.02) )
    {
      goodMatches.push_back(matches[i]);
    }
  }
  for( int i = 0; i < goodMatches.size(); i++ )
  {
    ptsSrc.push_back( featuresSrc[ goodMatches[i].queryIdx ].pt );
    ptsDst.push_back( featuresDst[ goodMatches[i].trainIdx ].pt );
  }
  
  if (goodMatches.size() < 10)
  {
    cb(false, priorH);
    return;
  }
  
  
  /*for (cv::Point2f& pt : ptsSrc)
    cv::circle(imgSrc, pt, 5, cv::Scalar(255,0,255));
  
  for (cv::Point2f& pt : ptsDst)
    cv::circle(imgDst, pt, 5, cv::Scalar(255,0,255));
  */
  
  /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::Mat img;
  cv::drawMatches(imgSrc, featuresSrc, imgDst, featuresDst, goodMatches, img);
  cv::imshow("imgae1", img);
  
  //cv::imshow("imgae1", imgSrc);
  //cv::imshow("imgae2", imgDst);
  cv::waitKey(0); */
  
  
  cv::Mat H = priorH;
  H = cv::findHomography(ptsSrc, ptsDst, CV_RANSAC, 5.0);
   
  cb(true, H);
  
  printf("matched %d features\n", featuresSrc.size());
}

// ----------------------------------------------------------------------------------
void QualityMatcher::matchImagesAsync(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb)
{
  if (m_matchingThread)
  {
    m_matchingThread->join();
  }
  
  // extract one channel for matching -> better have YUV, but green channel is god enough
  cv::Mat rgbSrc[4];
  cv::Mat rgbDst[4];
  cv::split(imageSrc, rgbSrc);
  cv::split(imageDst, rgbDst);
    
  m_matchingThread.reset(new std::thread(std::bind(&QualityMatcher::doTheMagic, this, rgbSrc[1], rgbDst[1], priorH, cb)));
}
