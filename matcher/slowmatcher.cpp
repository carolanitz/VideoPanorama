#include "slowmatcher.hpp"
#include "utils.hpp"

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching/stitcher.hpp>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"


using namespace cv;
#include <iostream>

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
  cv::Stitcher stitcher = cv::Stitcher::createDefault(false);

  //-- Step 1: Detect the keypoints using SURF Detector
   int minHessian = 400;

   SurfFeatureDetector detector( minHessian );

   std::vector<KeyPoint> keypoints_object, keypoints_scene;

   detector.detect( imageSrc, keypoints_object );
   detector.detect( imageDst, keypoints_scene );

   //-- Step 2: Calculate descriptors (feature vectors)
   SurfDescriptorExtractor extractor;

   Mat descriptors_object, descriptors_scene;

   extractor.compute( imageSrc, keypoints_object, descriptors_object );
   extractor.compute( imageDst, keypoints_scene, descriptors_scene );

   //-- Step 3: Matching descriptor vectors using FLANN matcher
   FlannBasedMatcher matcher;
   std::vector< DMatch > matches;
   matcher.match( descriptors_object, descriptors_scene, matches );

   double max_dist = 0; double min_dist = 100;

   //-- Quick calculation of max and min distances between keypoints
   for( int i = 0; i < descriptors_object.rows; i++ )
   { double dist = matches[i].distance;
     if( dist < min_dist ) min_dist = dist;
     if( dist > max_dist ) max_dist = dist;
   }

   printf("-- Max dist : %f \n", max_dist );
   printf("-- Min dist : %f \n", min_dist );

   //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
   std::vector< DMatch > good_matches;

   for( int i = 0; i < descriptors_object.rows; i++ )
   { if( matches[i].distance < 3*min_dist )
      { good_matches.push_back( matches[i]); }
   }


   //-- Localize the object
   std::vector<Point2f> obj;
   std::vector<Point2f> scene;

   for( int i = 0; i < good_matches.size(); i++ )
   {
     //-- Get the keypoints from the good matches
     obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
     scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
   }

   Mat H = findHomography( obj, scene, CV_RANSAC );


  H.convertTo(H, CV_32FC1);
  cb(true, H);
  
  // DEBUG  
  /*printf("H:\n");
  for (int i=0; i < 3; i++)
    printf("%f %f %f\n", H.at<float>(i,0), H.at<float>(i,1), H.at<float>(i,2));
  */
  }

// ----------------------------------------------------------------------------------
void QualityMatcher::matchImagesAsync(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb)
{
  if (m_matchingThread)
  {
    m_matchingThread->join();
  }
  
  // extract one channel for matching -> better have YUV, but green channel is god enough
  //cv::Mat rgbSrc[4];
  //cv::Mat rgbDst[4];
  //cv::split(imageSrc, rgbSrc);
  //cv::split(imageDst, rgbDst);
    
  m_matchingThread.reset(new std::thread(std::bind(&QualityMatcher::doTheMagic, this, imageSrc, imageDst, priorH, cb)));
}
