#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "quantities/dimensionless.hpp"
#include "testing_utilities/quantity_matchers.hpp"

namespace principia {
namespace test_utilities {

using quantities::Dimensionless;
using testing::Not;
using testing::Eq;

class QuantityMatchersTest : public testing::Test {
 protected:
};

TEST_F(QuantityMatchersTest, AlmostButNotQuiteEquals) {
  EXPECT_THAT(Dimensionless(1), AlmostEquals(1));
  EXPECT_THAT(Dimensionless(1.01), Not(AlmostEquals(1)));
  Dimensionless not_quite_one = 0;
  for (int i = 1; i <= 100; ++i) {
    not_quite_one += 0.01;
  }
  EXPECT_THAT(not_quite_one, Not(Eq(1)));
  EXPECT_THAT(not_quite_one, AlmostEquals(1));
  EXPECT_THAT(not_quite_one - 1, Not(AlmostEquals(0)));
  EXPECT_THAT(not_quite_one - 1, AlmostVanishesBefore(1));
  EXPECT_THAT(not_quite_one - 1, Not(AlmostVanishesBefore(.5)));
}

TEST_F(QuantityMatchersTest, ApproximationMatcher) {
  EXPECT_THAT(Dimensionless(2.19), Approximates(2, 0.1));
  EXPECT_THAT(Dimensionless(2.21), Not(Approximates(2, 0.1)));
}

}  // namespace test_utilities
}  // namespace principia
