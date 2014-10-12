#include "slowmatcher.hpp"
#include "utils.hpp"

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

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
bool niceHomography(cv::Mat H)
{
  const float det = H.at<float>(0, 0) * H.at<float>(1, 1) - H.at<float>(1, 0) * H.at<float>(0, 1);
  if (det < 0)
    return false;

  const float N1 = sqrt(H.at<float>(0, 0) * H.at<float>(0, 0) + H.at<float>(1, 0) * H.at<float>(1, 0));
  if (N1 > 4 || N1 < 0.1)
    return false;

  const float N2 = sqrt(H.at<float>(0, 1) * H.at<float>(0, 1) + H.at<float>(1, 1) * H.at<float>(1, 1));
  if (N2 > 4 || N2 < 0.1)
    return false;

  const float N3 = sqrt(H.at<float>(2, 0) * H.at<float>(2, 0) + H.at<float>(2, 1) * H.at<float>(2, 1));
  if (N3 > 0.002)
    return false;

  return true;
}

// ----------------------------------------------------------------------------------
void QualityMatcher::doTheMagic(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb)
{
  // keypoints 
  std::vector<cv::KeyPoint> featuresSrc;
  std::vector<cv::KeyPoint> featuresDst;
  
  // TODO - use the provided prior
  
  // prefilter slightly
  cv::Mat imgSrc = imageSrc, imgDst = imageDst;
  //cv::GaussianBlur(imageSrc, imgSrc, cv::Size(3,3), 5.0);
  //cv::GaussianBlur(imageDst, imgDst, cv::Size(3,3), 5.0);

  cv::medianBlur(imageSrc, imgSrc, 3);
  cv::medianBlur(imageDst, imgDst, 3);
  
  cv::Mat descriptorsSrc, descriptorsDst;
  
  // detect
  
  //cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SURF");
  //detector->detect(imgSrc, featuresSrc);
  //detector->detect(imgDst, featuresDst);
  
  // features
  cv::FAST(imgSrc, featuresSrc, 50, cv::FastFeatureDetector::TYPE_9_16);
  cv::FAST(imgDst, featuresDst, 50, cv::FastFeatureDetector::TYPE_9_16);
    
  printf("input %d vs %d\n", (int)featuresSrc.size(), (int)featuresDst.size());
  cv::Ptr<cv::DescriptorExtractor> descriptor = cv::DescriptorExtractor::create("ORB" );
  descriptor->compute(imgSrc, featuresSrc, descriptorsSrc);
  descriptor->compute(imgDst, featuresDst, descriptorsDst);
  
  // descriptors
  //cv::BriefDescriptorExtractor descriptor;
  //descriptor.compute(imgSrc, featuresSrc, descriptorsSrc);  
  //descriptor.compute(imgDst, featuresDst, descriptorsDst);
  
  if (featuresDst.size() < 10 || featuresSrc.size() < 10
      || descriptorsSrc.rows != featuresSrc.size()
      || descriptorsDst.rows != featuresDst.size())
  {
    cb(false, priorH);
    return;
  }
  
  // matching (simple nearest neighbours)
  cv::BFMatcher matcher(cv::NORM_HAMMING);
  std::vector<cv::DMatch> matches;
  matcher.match( descriptorsSrc, descriptorsDst, matches );
  
  
  std::vector<cv::DMatch> goodMatches;
  std::vector<cv::Point2f> ptsSrc, ptsDst;
  for( int i = 0; i < matches.size(); i++ )
  {
    if( matches[i].distance <= 20)//std::max(4. * min_dist, 0.02) )
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
  
  // ----------------------------
  // KLT tracker to further improve the result
  // ----------------------------
  cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.001);
  cv::cornerSubPix(imgSrc, ptsSrc, cv::Size(3,3), cv::Size(-1,-1), termcrit);
  cv::cornerSubPix(imgDst, ptsDst, cv::Size(3,3), cv::Size(-1,-1), termcrit);
  if(1)
  {
    std::vector<uchar> status;
    std::vector<float> err;
    cv::Size winSize(5,5);
    
    std::vector<cv::Point2f> ptsDstKlt = ptsDst;
    std::vector<cv::Point2f> ptsSrcOld = ptsSrc;
    
    std::vector<cv::Mat> pyrSrc, pyrDst;
    cv::buildOpticalFlowPyramid(imgSrc, pyrSrc, winSize, 5);
    cv::buildOpticalFlowPyramid(imgDst, pyrDst, winSize, 5);
    
    
    /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::Mat img;
    cv::drawMatches(imgSrc, featuresSrc, imgDst, featuresDst, goodMatches, img);
    cv::imshow("imgae1", img);
    cv::waitKey(0);*/
    
    cv::calcOpticalFlowPyrLK(pyrSrc, pyrDst, ptsSrc, ptsDstKlt, status, err, winSize, 5, termcrit, cv::OPTFLOW_USE_INITIAL_FLOW);
    
    // remove bad points
    ptsSrc.clear();
    ptsDst.clear();
    for (size_t i=0; i < status.size(); i++)
    {
      if (!status[i]) continue;
      
      ptsSrc.push_back(ptsSrcOld[i]);
      ptsDst.push_back(ptsDstKlt[i]);
    }
  }
  printf("klt tracked %d\n", (int)ptsDst.size());
  if (ptsDst.size() < 10)
  {
    printf("MATCH FAILED\n");
    cb(false, priorH);
    return;
  }
  
  cv::Mat H = cv::findHomography(ptsSrc, ptsDst, CV_RANSAC, 2.0);
  H.convertTo(H, CV_32FC1);
  
  if (!niceHomography(H))
  {
      printf("MATCH FAILED\n");
      cb(false, priorH);
      return;    
  }
  
  
  // DEBUG  
  printf("H:\n");
  for (int i=0; i < 3; i++)
    printf("%f %f %f\n", H.at<float>(i,0), H.at<float>(i,1), H.at<float>(i,2));
  printf("prior H:\n");
  for (int i=0; i < 3; i++)
    printf("%f %f %f\n", priorH.at<float>(i,0), priorH.at<float>(i,1), priorH.at<float>(i,2));
  
  float nrm = cv::norm(priorH);
  if (nrm > 2)
  {
    nrm = cv::norm(priorH, H);
    printf("(H-prior).norm() = %f\n", nrm);
    if (nrm > 10.0)
    {
      printf("MATCH FAILED - bad H\n");
      cb(false, priorH);
      return;    
    }
  }
  
  cb(true, H);
  
  
  
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
    
  m_matchingThread.reset(new std::thread(std::bind(&QualityMatcher::doTheMagic, this, rgbSrc[1], rgbDst[1], priorH.clone(), cb)));
}
