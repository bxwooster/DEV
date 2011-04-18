#ifndef __Matrix_h__
#define __Matrix_h__

#include "Ok.h"

#define EIGEN_DEFAULT_TO_ROW_MAJOR
#define eigen_assert OK_P

#include <Eigen/Core>
#include <Eigen/LU> 
#include <Eigen/StdVector>

namespace common {

using Eigen::Matrix;
EIGEN_USING_MATRIX_TYPEDEFS

} //namespace common

#endif