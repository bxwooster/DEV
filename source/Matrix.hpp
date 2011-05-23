#pragma once

#include "OK.hpp"

#define NOMINMAX
#define EIGEN_DEFAULT_TO_ROW_MAJOR
#define eigen_assert OK

#include <Eigen/Core>
#include <Eigen/LU> 
#include <Eigen/StdVector>
#include <Eigen/Geometry>

using Eigen::Matrix;
EIGEN_USING_MATRIX_TYPEDEFS
