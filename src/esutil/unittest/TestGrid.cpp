#define BOOST_TEST_MODULE Grid

#include "ut.hpp"
#include <boost/test/floating_point_comparison.hpp>

#include "../Grid.hpp"
using namespace espresso;
using namespace esutil;

BOOST_AUTO_TEST_CASE(testGrid) {
  Grid testGrid(1, 2, 3);
  BOOST_REQUIRE_EQUAL(testGrid.getNumberOfCells(), int(6));

  BOOST_CHECK_EQUAL(testGrid.getGridSize(0), int(1));
  BOOST_CHECK_EQUAL(testGrid.getGridSize(1), int(2));
  BOOST_CHECK_EQUAL(testGrid.getGridSize(2), int(3));

  for (int i = 0; i < 6; ++i) {
    int x, y, z;
    testGrid.mapIndexToPosition(x, y, z, i);
    BOOST_CHECK_EQUAL(testGrid.mapPositionToIndex(x, y, z), i);
  }
}
