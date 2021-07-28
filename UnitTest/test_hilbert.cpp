#include "functions.h"
#include "gtest/gtest.h"

TEST(HilbertIndex2D, HandlesZeroInput) {
  VectorD<2> quadrant0 = {0, 0};
  EXPECT_EQ(hilbert::index<2>(quadrant0, 1), 0);
  EXPECT_EQ(hilbert::index<2>(quadrant0, 2), 0);
  EXPECT_EQ(hilbert::index<2>(quadrant0, 3), 0);
  EXPECT_EQ(hilbert::index<2>(quadrant0, 4), 0);
  VectorD<2> quadrant1 = {0, 1};
  EXPECT_EQ(hilbert::index<2>(quadrant1, 1), 1);
  // level1: 1 * 4 level 2: 1
  EXPECT_EQ(hilbert::index<2>(quadrant1, 2), 5);
  EXPECT_EQ(hilbert::index<2>(quadrant1, 3), 21);
  EXPECT_EQ(hilbert::index<2>(quadrant1, 4), 85);
  VectorD<2> quadrant2 = {1, 1};
  EXPECT_EQ(hilbert::index<2>(quadrant2, 1), 2);
  // level1: 2 * 4 level 2: 2 -> 10
  EXPECT_EQ(hilbert::index<2>(quadrant2, 2), 10);
  EXPECT_EQ(hilbert::index<2>(quadrant2, 3), 42);
  EXPECT_EQ(hilbert::index<2>(quadrant2, 4), 170);
  VectorD<2> quadrant3 = {1, 0};
  EXPECT_EQ(hilbert::index<2>(quadrant3, 1), 3);
  EXPECT_EQ(hilbert::index<2>(quadrant3, 2), 15);
  EXPECT_EQ(hilbert::index<2>(quadrant3, 3), 63);
  EXPECT_EQ(hilbert::index<2>(quadrant3, 4), 255);

  // level1: 1 * 4 level 2: 2
  VectorD<2> quadrant6 = {0.3, 1};
  EXPECT_EQ(hilbert::index<2>(quadrant6, 2), 6);

  VectorD<2> quadrant5 = {0.2, 1};
  EXPECT_EQ(hilbert::index<2>(quadrant5, 2), 5);

  // level1: 2 * 4 level 2: 1 -> 9
  VectorD<2> quadrant9 = {0.5, 1};
  EXPECT_EQ(hilbert::index<2>(quadrant9, 2), 9);

  // level1: 1 * 4 level 2: 3 -> 7
  VectorD<2> quadrant7 = {0.4, 0.6};
  EXPECT_EQ(hilbert::index<2>(quadrant7, 2), 7);

  // level1: 2 * 4 level 2: 0 -> 8
  VectorD<2> quadrant8 = {0.5, 0.5};
  EXPECT_EQ(hilbert::index<2>(quadrant8, 2), 8);
}

TEST(HilbertIndex3D, HandlesZeroInput) {
  VectorD<3> quadrant0 = {0, 0, 0};
  EXPECT_EQ(hilbert::index<3>(quadrant0, 1), 0);
  EXPECT_EQ(hilbert::index<3>(quadrant0, 2), 0);
  EXPECT_EQ(hilbert::index<3>(quadrant0, 3), 0);
  EXPECT_EQ(hilbert::index<3>(quadrant0, 4), 0);
  VectorD<3> quadrant1 = {0, 1, 0};
  EXPECT_EQ(hilbert::index<3>(quadrant1, 1), 1);
  EXPECT_EQ(hilbert::index<3>(quadrant1, 2), 9);
  EXPECT_EQ(hilbert::index<3>(quadrant1, 3), 73);
  EXPECT_EQ(hilbert::index<3>(quadrant1, 4), 585);
  VectorD<3> quadrant2 = {1, 1, 0};
  EXPECT_EQ(hilbert::index<3>(quadrant2, 1), 2);
  EXPECT_EQ(hilbert::index<3>(quadrant2, 2), 18);
  EXPECT_EQ(hilbert::index<3>(quadrant2, 3), 146);
  EXPECT_EQ(hilbert::index<3>(quadrant2, 4), 1170);
  VectorD<3> quadrant3 = {1, 0, 0};
  EXPECT_EQ(hilbert::index<3>(quadrant3, 1), 3);
  EXPECT_EQ(hilbert::index<3>(quadrant3, 2), 27);
  EXPECT_EQ(hilbert::index<3>(quadrant3, 3), 219);
  EXPECT_EQ(hilbert::index<3>(quadrant3, 4), 1755);

  VectorD<3> quadrant4 = {1, 0, 1};
  EXPECT_EQ(hilbert::index<3>(quadrant4, 1), 4);
  EXPECT_EQ(hilbert::index<3>(quadrant4, 2), 36);
  EXPECT_EQ(hilbert::index<3>(quadrant4, 3), 292);
  EXPECT_EQ(hilbert::index<3>(quadrant4, 4), 2340);

  VectorD<3> quadrant5 = {0, 0, 1};
  EXPECT_EQ(hilbert::index<3>(quadrant5, 1), 5);
  EXPECT_EQ(hilbert::index<3>(quadrant5, 2), 45);
  EXPECT_EQ(hilbert::index<3>(quadrant5, 3), 365);
  EXPECT_EQ(hilbert::index<3>(quadrant5, 4), 2925);

  VectorD<3> quadrant6 = {0, 1, 1};
  EXPECT_EQ(hilbert::index<3>(quadrant6, 1), 6);
  EXPECT_EQ(hilbert::index<3>(quadrant6, 2), 54);
  EXPECT_EQ(hilbert::index<3>(quadrant6, 3), 438);
  EXPECT_EQ(hilbert::index<3>(quadrant6, 4), 3510);

  VectorD<3> quadrant7 = {1, 1, 1};
  EXPECT_EQ(hilbert::index<3>(quadrant7, 1), 7);
  EXPECT_EQ(hilbert::index<3>(quadrant7, 2), 63);
  EXPECT_EQ(hilbert::index<3>(quadrant7, 3), 511);
  EXPECT_EQ(hilbert::index<3>(quadrant7, 4), 4095);
}

TEST(HilbertIndex4D, HandlesZeroInput) {
  VectorD<4> quadrant0 = {0, 0, 0, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant0, 1), 0);
  EXPECT_EQ(hilbert::index<4>(quadrant0, 2), 0);
  EXPECT_EQ(hilbert::index<4>(quadrant0, 3), 0);
  EXPECT_EQ(hilbert::index<4>(quadrant0, 4), 0);

  VectorD<4> quadrant1 = {0, 1, 0, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant1, 1), 1);
  EXPECT_EQ(hilbert::index<4>(quadrant1, 2), 17);
  EXPECT_EQ(hilbert::index<4>(quadrant1, 3), 273);
  EXPECT_EQ(hilbert::index<4>(quadrant1, 4), 4369);

  VectorD<4> quadrant2 = {1, 1, 0, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant2, 1), 2);
  EXPECT_EQ(hilbert::index<4>(quadrant2, 2), 34);
  EXPECT_EQ(hilbert::index<4>(quadrant2, 3), 546);
  EXPECT_EQ(hilbert::index<4>(quadrant2, 4), 8738);

  VectorD<4> quadrant3 = {1, 0, 0, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant3, 1), 3);
  EXPECT_EQ(hilbert::index<4>(quadrant3, 2), 51);
  EXPECT_EQ(hilbert::index<4>(quadrant3, 3), 819);
  EXPECT_EQ(hilbert::index<4>(quadrant3, 4), 13107);

  VectorD<4> quadrant4 = {1, 0, 1, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant4, 1), 4);
  EXPECT_EQ(hilbert::index<4>(quadrant4, 2), 68);
  EXPECT_EQ(hilbert::index<4>(quadrant4, 3), 1092);
  EXPECT_EQ(hilbert::index<4>(quadrant4, 4), 17476);

  VectorD<4> quadrant5 = {0, 0, 1, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant5, 1), 5);
  EXPECT_EQ(hilbert::index<4>(quadrant5, 2), 85);
  EXPECT_EQ(hilbert::index<4>(quadrant5, 3), 1365);
  EXPECT_EQ(hilbert::index<4>(quadrant5, 4), 21845);

  VectorD<4> quadrant6 = {0, 1, 1, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant6, 1), 6);
  EXPECT_EQ(hilbert::index<4>(quadrant6, 2), 102);
  EXPECT_EQ(hilbert::index<4>(quadrant6, 3), 1638);
  EXPECT_EQ(hilbert::index<4>(quadrant6, 4), 26214);

  VectorD<4> quadrant7 = {1, 1, 1, 0};
  EXPECT_EQ(hilbert::index<4>(quadrant7, 1), 7);
  EXPECT_EQ(hilbert::index<4>(quadrant7, 2), 119);
  EXPECT_EQ(hilbert::index<4>(quadrant7, 3), 1911);
  EXPECT_EQ(hilbert::index<4>(quadrant7, 4), 30583);
}
