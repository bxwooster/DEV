#ifndef __Matrix_h__
#define __Matrix_h__

#include "Ok.h"

#define EIGEN_DEFAULT_TO_ROW_MAJOR
#define ei_assert OK_P
#include <Eigen/Core>
#include <Eigen/LU> 

namespace common {

using Eigen::Matrix;
EIGEN_USING_MATRIX_TYPEDEFS

} //namespace common

#endif