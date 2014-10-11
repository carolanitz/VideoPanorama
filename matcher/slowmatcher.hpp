#ifndef VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>

#include <thread>
#include <utility>

class QualityMatcher {

public:
  QualityMatcher();
  virtual ~QualityMatcher();

  typedef std::function<void(bool, cv::Mat)> MatchingResultCallback;

  void matchImagesAsync(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb);

private:

  cv::Mat cameraPoseFromHomography(const cv::Mat& H);
  
  MatchingResultCallback m_matchResultCallback;

  std::shared_ptr<std::thread> m_matchingThread;
  void doTheMagic(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb);
};


#endif // VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
