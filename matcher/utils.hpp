
#ifndef UTILS__HPP__
#define UTILS__HPP__

#include <opencv2/core/core.hpp>
#include <Eigen/Core>

namespace utils
{
  
  cv::Mat cameraPoseFromHomography(const cv::Mat& H);

  
  
  //-----------------------------------------------------------------------------
  template <typename Scalar, int Options>
  inline Eigen::Matrix<Scalar, 4, 4, Options> inverseTransformationMatrix(const Eigen::Matrix<Scalar, 4, 4, Options>& T)
  {
     Eigen::Matrix<Scalar, 4, 4, Options> result = Eigen::Matrix<Scalar, 4, 4, Options>::Identity();
  
     //! TODO/NOTE - since we are dealing with rotations and translations only
     //!             the rotation part can be just transposed. However, as soon as we incorporate
     //!             scaling and shearing, inverse operation is required.
     result.template block<3,3>(0,0) = T.template block<3,3>(0,0).transpose();   // rotation is inverted/transposed
     result.template block<3,1>(0,3) = result.template block<3,3>(0,0) * -T.template block<3,1>(0,3);     // translation is inverted
  
     return result;
  }
  
  //-----------------------------------------------------------------------------
  template<typename Radian, int D>
  Eigen::Matrix<Radian,D,D> makeRotX3(Radian angle)
  {
     Eigen::Matrix<Radian,D,D> M = Eigen::Matrix<Radian,D,D>::Identity();
  
     M(1,1) = cos(angle); M(1,2) = -sin(angle);
     M(2,1) = sin(angle); M(2,2) = cos(angle);
     return M;
  }
  
  //-----------------------------------------------------------------------------
  template<typename Radian>
  Eigen::Matrix<Radian,3,3> makeRotX3(Radian angle)
  {
     return makeRotX3<Radian,3>(angle);
  }
  
  //-----------------------------------------------------------------------------
  template<typename Radian, int D>
  Eigen::Matrix<Radian,D,D> makeRotY3(Radian angle)
  {
     Eigen::Matrix<Radian,D,D> M = Eigen::Matrix<Radian,D,D>::Identity();
  
     M(0,0) = cos(angle); M(0,2) = sin(angle);
     M(2,0) = -sin(angle); M(2,2) = cos(angle);
     return M;
  }
  
  //-----------------------------------------------------------------------------
  template<typename Radian>
  Eigen::Matrix<Radian,3,3> makeRotY3(Radian angle)
  {
     return makeRotY3<Radian, 3>(angle);
  }
  
  //-----------------------------------------------------------------------------
  template<typename Radian, int D>
  Eigen::Matrix<Radian,D,D> makeRotZ3(Radian angle)
  {
     Eigen::Matrix<Radian,D,D> M = Eigen::Matrix<Radian,D,D>::Identity();
  
     M(0,0) = cos(angle); M(0,1) = -sin(angle);
     M(1,0) = sin(angle); M(1,1) = cos(angle);
     return M;
  }
  
  
  //-----------------------------------------------------------------------------
  template<typename Radian>
  Eigen::Matrix<Radian,3,3> makeRotZ3(Radian angle)
  {
     return makeRotZ3<Radian, 3>(angle);
  }
  
}

#endif
