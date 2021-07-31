#include "math/mathfunctions.h"
#include "gtest/gtest.h"

TEST(MathApprox, HandlesZeroInput) {
  const double eps = std::numeric_limits<double>::epsilon();
  ASSERT_TRUE(approx(1.0, 1.0, eps));
  ASSERT_FALSE(approx(1.0, 1.0+eps, eps));

  const float one = 1.0;
  const float epsf = std::numeric_limits<float>::epsilon();
  ASSERT_TRUE(approx(one, one, epsf));
  ASSERT_FALSE(approx(one, one+epsf, epsf));
}
