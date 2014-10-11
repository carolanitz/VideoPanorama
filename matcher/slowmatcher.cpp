#include "slowmatcher.hpp"
#include "utils.hpp"

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
  
  //cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("ORB");
  cv::Ptr<cv::DescriptorExtractor> descriptor = cv::DescriptorExtractor::create("ORB" );
  //detector->detect(imgSrc, featuresSrc);
  //detector->detect(imgDst, featuresDst);
  
  // features
  cv::FAST(imgSrc, featuresSrc, 40, cv::FastFeatureDetector::TYPE_9_16);
  cv::FAST(imgDst, featuresDst, 40, cv::FastFeatureDetector::TYPE_9_16);
  
  printf("input %d vs %d\n", (int)featuresSrc.size(), (int)featuresDst.size());
  descriptor->compute(imgSrc, featuresSrc, descriptorsSrc);
  descriptor->compute(imgDst, featuresDst, descriptorsDst);
  
  // descriptors
  //cv::BriefDescriptorExtractor brief;
  //brief.compute(imgSrc, featuresSrc, descriptorsSrc);  
  //brief.compute(imgDst, featuresDst, descriptorsDst);
  
  if (featuresDst.size() < 10 || featuresSrc.size() < 10
      || descriptorsSrc.rows != featuresSrc.size()
      || descriptorsDst.rows != featuresDst.size())
  {
    cb(false, priorH);
    return;
  }
  
  // matching (simple nearest neighbours)
  cv::BFMatcher matcher(cv::NORM_HAMMING);
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
    if( matches[i].distance <= 15)//std::max(4. * min_dist, 0.02) )
    {
      goodMatches.push_back(matches[i]);
    }
  }
  printf("min = %f - %d\n", min_dist, goodMatches.size());
  
  for( int i = 0; i < goodMatches.size(); i++ )
  {
    ptsSrc.push_back( featuresSrc[ goodMatches[i].queryIdx ].pt );
    ptsDst.push_back( featuresDst[ goodMatches[i].trainIdx ].pt );
  }
  
  if (goodMatches.size() < 10)
  {
    printf("MATCH FAILED\n");
    cb(false, priorH);
    return;
  }
  
  /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::Mat img;
  cv::drawMatches(imgSrc, featuresSrc, imgDst, featuresDst, goodMatches, img);
  cv::imshow("imgae1", img);
  
  //cv::imshow("imgae1", imgSrc);
  //cv::imshow("imgae2", imgDst);
  cv::waitKey(0); */
  
  
  cv::Mat H = cv::findHomography(ptsSrc, ptsDst, CV_RANSAC);
  H.convertTo(H, CV_32FC1);
  cb(true, H);
  
  // DEBUG  
  /*printf("H:\n");
  for (int i=0; i < 3; i++)
    printf("%f %f %f\n", H.at<float>(i,0), H.at<float>(i,1), H.at<float>(i,2));
  */
  
  printf("matched %d features\n", (int)featuresSrc.size());
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
