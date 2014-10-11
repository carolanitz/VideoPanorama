#ifndef VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
#define VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP

#include <opencv2/core/core.hpp>
#include <boost/thread.hpp>

class QualityMatcher {

public:
  QualityMatcher();
  virtual ~QualityMatcher();

  typedef boost::function<void(bool, cv::Mat)> MatchingResultCallback;

  void matchImagesAsync(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb);

private:

  MatchingResultCallback m_matchResultCallback;

  boost::shared_ptr<boost::thread> m_matchingThread;
  void doTheMagic(cv::Mat imageSrc, cv::Mat imageDst, cv::Mat priorH, MatchingResultCallback cb);
};


#endif // VIDEO_PANORAMA_SLOW_MATCHER_MATCHER_HPP
