#include <acconfig.hpp>
#define BOOST_TEST_MODULE PropertyVector

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/foreach.hpp>

#include "../TupleVector.hpp"

using namespace espresso::esutil;

/*
  compile like this:
  libtool --mode=link g++  -Wall -O3 -I ../../../contrib/boost -I ../../../obj-cayley/src \
  ../TupleVector.cpp TestTupleVector.cpp ../../../obj-cayley/contrib/boost/libboost_unit_test_framework.la -o test \
  && ./test
*/

struct Fixture {
    TupleVector mv;
    const TupleVector &constMv;

    TupleVector::PropertyId intProp, floatProp;

    Fixture(): constMv(mv) {
	intProp = mv.addProperty<int>();
	floatProp = mv.addProperty<float>(2);
        mv.setGranularity(8);
        mv.setShrinkThreshold(32);
	mv.resize(42);
    }

    ~Fixture() {
    }
};

//____________________________________________________________________________//


BOOST_FIXTURE_TEST_CASE(properties_resize_test, Fixture)
{
    // check that empty vector is empty
    TupleVector mve;
    mve.setGranularity(8);
    mve.setShrinkThreshold(32);
    BOOST_CHECK(mve.size() == 0);
    BOOST_CHECK(mve.getNumProperties() == 0);

    // but not, if we add a Property
    mve.addProperty<int>(3);
    BOOST_REQUIRE(mve.getNumProperties() == 1);

    // add a second property
    TupleVector::PropertyId prop = mve.addProperty<float>();
    BOOST_REQUIRE(mve.getNumProperties() == 2);

    // now, delete the property again
    mve.eraseProperty(prop);
    BOOST_REQUIRE(mve.getNumProperties() == 1);

    // check resizing copy
    TupleVector mvc(mve, 42);
    BOOST_CHECK_EQUAL(mvc.size(), size_t(42));
    BOOST_CHECK_EQUAL(mvc.capacity(), size_t(48));
    BOOST_CHECK_EQUAL(mvc.getNumProperties(), size_t(1));
}

BOOST_FIXTURE_TEST_CASE(particles_resize_test, Fixture)
{
    // this test needs the initial size to be 42
    BOOST_REQUIRE(mv.size() == 42);

    // resizing above threshold, should shrink
    mv.clear();
    BOOST_CHECK(mv.size() == 0);
    BOOST_CHECK(mv.capacity() == 0);

    mv.resize(12);
    BOOST_CHECK(mv.size() == 12);
    BOOST_CHECK(mv.capacity() == 16);

    // resizing below the threshold should not shrink
    mv.clear();
    BOOST_CHECK(mv.size() == 0);
    BOOST_CHECK(mv.capacity() == 16);
}

BOOST_FIXTURE_TEST_CASE(pointers_test, Fixture)
{
    TupleVector::PropertyPointer<int> pRef = mv.getProperty<int>(intProp);

    ////// element references
    TupleVector::reference ref = mv[17];
    TupleVector::const_reference constRef = constMv[17];
    TupleVector::pointer ptr = &ref;
    TupleVector::const_pointer constPtr = &constRef;
    // check comparisons
    BOOST_CHECK(ptr == constPtr);
    BOOST_CHECK(!(ptr != constPtr));

    // check changing the pointer
    constPtr = &mv[3];
    BOOST_CHECK(ptr != constPtr);

    // this does not compile, tries const -> non-const conversion
    //TupleVector::pointer ptr2 = &constRef;

    // check that addresses seem to match
    pRef[ref] = 42;
    pRef[*ptr] = 45;    
    BOOST_CHECK(pRef[ref] == 45);
}

BOOST_FIXTURE_TEST_CASE(dereference_scalar_test, Fixture)
{
    ////// property references
    const TupleVector &constMv = mv;
    TupleVector::PropertyPointer<int> pRef = mv.getProperty<int>(intProp);
    TupleVector::ConstPropertyPointer<int> constPRef = constMv.getProperty<int>(intProp);

    // convert non-const -> const
    { TupleVector::ConstPropertyPointer<int> constPRef2 = pRef; }

    // this does not compile, tries const -> non-const conversion
    //TupleVector::PropertyPointer<int> pRef2 = constMv.getProperty<int>(intProp);    

    ////// element references
    TupleVector::reference ref = mv[0];
    TupleVector::const_reference constRef = constMv[0];

    // this does not compile, reassigning a non-trivial reference
    // ref = ref;

    // convert non-const -> const
    { TupleVector::const_reference constRef = mv[0]; }

    // this does not compile, tries const -> non-const conversion
    //TupleVector::reference ref2 = constMv[0];

    BOOST_CHECK_THROW(TupleVector::reference ref2 = mv.at(42),
		      std::out_of_range);

    BOOST_CHECK_THROW(TupleVector::const_reference ref2 = constMv.at(43),
		      std::out_of_range);

    pRef[ref] = 176;
    BOOST_CHECK_EQUAL(pRef[ref], 176);
    pRef[ref] = 42;
    BOOST_CHECK_EQUAL(constPRef[constRef], 42);

    // this does not compile, overriding const in various ways
    //pRef[constRef] = 42;
    //constPRef[ref] = 42;
    //constPRef[constRef] = 42;
    //TupleVector::ReferenceIndexAccess::getIndex(constRef);
}

BOOST_FIXTURE_TEST_CASE(dereference_array_test, Fixture)
{
    ////// property references
    const TupleVector &constMv = mv;
    TupleVector::ArrayPropertyPointer<float>           pRef = mv.getArrayProperty<float>(floatProp);
    TupleVector::ConstArrayPropertyPointer<float> constPRef = constMv.getArrayProperty<float>(floatProp);

    // convert non-const -> const
    { TupleVector::ConstArrayPropertyPointer<float> constPRef2 = pRef; }

    // this does not compile, tries const -> non-const conversion
    //TupleVector::ArrayPropertyPointer<float, 3> pRef2 = constMv.getArrayProperty<float, 3>(floatProp);

    ////// element references
    TupleVector::reference ref = mv[0];
    TupleVector::const_reference constRef = constMv[0];

    // this does not compile, tries const -> non-const conversion
    //TupleVector::reference ref2 = constMv[0];

    BOOST_CHECK_THROW(TupleVector::reference ref2 = mv.at(42),
		      std::out_of_range);

    BOOST_CHECK_THROW(TupleVector::const_reference ref2 = constMv.at(43),
		      std::out_of_range);

    // to check that array elements do not overlap, create two adjacent elements
    TupleVector::reference ref2 = mv[1];

    pRef[ref][0] = 0.01;
    pRef[ref][1] = 0.2;

    pRef[ref2][0] = 0.04;
    pRef[ref2][1] = 0.5;

    BOOST_CHECK_CLOSE(constPRef[constRef][0], 0.01f, 1e-10f);
    BOOST_CHECK_CLOSE(          pRef[ref][1], 0.20f, 1e-10f);
    BOOST_CHECK_CLOSE(     pRef[constRef][0], 0.01f, 1e-10f);

    BOOST_CHECK_CLOSE(constPRef[ref2][0], 0.04f, 1e-10f);
    BOOST_CHECK_CLOSE(constPRef[ref2][1], 0.50f, 1e-10f);

    // this does not compile, overriding const in various ways
    //pRef[constRef][0] = 42;
    //constPRef[ref][1] = 42;
    //constPRef[constRef][2] = 42;
}

BOOST_FIXTURE_TEST_CASE(iterator_test, Fixture)
{
  {
    TupleVector::PropertyPointer<int> intRef = mv.getProperty<int>(intProp);
    TupleVector::ArrayPropertyPointer<float> floatRef = mv.getArrayProperty<float>(floatProp);

    // fill int property
    size_t i = 0;
    BOOST_FOREACH(TupleVector::reference ref, mv) {
	intRef[ref] = i;
	floatRef[ref][0] = 0.42*i;
	floatRef[ref][1] = 0.24/(i+1);
	i++;
    }
    BOOST_CHECK_EQUAL(i, mv.size());

    // and check the data is there, with constant array this time
    i = 0;
    BOOST_FOREACH(TupleVector::const_reference ref, constMv) {
	BOOST_CHECK_EQUAL(intRef[ref], int(i));
	BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*i));
	BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(i+1)));
	i++;
    }
    // this code above should not compile without const_
  }

  // insert two elements by insert()
  TupleVector::iterator it = mv.begin() + 10;
  // iterator it actually never changes, because it points to the
  // inserted element
  it = mv.insert(it);
  BOOST_CHECK_EQUAL(size_t(43), mv.size());
  it = mv.insert(it, mv[6]);
  BOOST_CHECK_EQUAL(size_t(44), mv.size());
  it = mv.insert(it, mv[3]);
  BOOST_CHECK_EQUAL(size_t(45), mv.size());
  mv.insert(it,3);
  BOOST_CHECK_EQUAL(size_t(48), mv.size());

  /* check the data is there, with constant array this time.
     After above magic, we have:
     mv[0:9]=0:9
     mv[10:12] = x
     mv[13] = mv[3] = 3
     mv[14] = mv[6] = 6
     mv[15] = x
     mv[16:] = 10:
  */
  {
    TupleVector::PropertyPointer<int> intRef = mv.getProperty<int>(intProp);
    TupleVector::ArrayPropertyPointer<float> floatRef = mv.getArrayProperty<float>(floatProp);
    size_t i = 0;
    BOOST_FOREACH(TupleVector::const_reference ref,
		  static_cast<const TupleVector &>(mv)) {
	if (i < 10) {
	    BOOST_CHECK_EQUAL(intRef[ref], int(i));
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*i));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(i+1)));
	} else if (i == 13) {
	    BOOST_CHECK_EQUAL(intRef[ref], 3);
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*3));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(3+1)));
	} else if (i == 14) {
	    BOOST_CHECK_EQUAL(intRef[ref], 6);
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*6));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(6+1)));
	} else if (i >= 16) {
	    BOOST_CHECK_EQUAL(intRef[ref], int(i - 6));
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*(i-6)));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(i-6+1)));
	}
        i++;
    }
  }

  // and now, delete some elements
  it = mv.begin() + 10;
  it = mv.erase(it, it + 3);
  BOOST_CHECK_EQUAL(size_t(45), mv.size());
  it = mv.erase(it + 2);
  BOOST_CHECK_EQUAL(size_t(44), mv.size());

  /* check the data is there, with constant array this time.
     Now, we have:
     mv[0:9]=0:9
     mv[10] = mv[3] = 3
     mv[11] = mv[6] = 6
     mv[12:] = 10:
  */
  {
    TupleVector::PropertyPointer<int> intRef = mv.getProperty<int>(intProp);
    TupleVector::ArrayPropertyPointer<float> floatRef = mv.getArrayProperty<float>(floatProp);

    size_t i = 0;
    BOOST_FOREACH(TupleVector::const_reference ref,
		  static_cast<const TupleVector &>(mv)) {
	if (i < 10) {
	    BOOST_CHECK_EQUAL(intRef[ref], int(i));
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*i));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(i+1)));
	} else if (i == 10) {
	    BOOST_CHECK_EQUAL(intRef[ref], 3);
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*3));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(3+1)));
	} else if (i == 11) {
	    BOOST_CHECK_EQUAL(intRef[ref], 6);
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*6));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(6+1)));
	} else if (i >= 12) {
	    BOOST_CHECK_EQUAL(intRef[ref], int(i - 2));
	    BOOST_CHECK_EQUAL(floatRef[ref][0], float(0.42*(i-2)));
	    BOOST_CHECK_EQUAL(floatRef[ref][1], float(0.24/(i-2+1)));
	}
        i++;
    }
  }

  // and now, move some elements
  mv[10] = mv[9];
  BOOST_CHECK_EQUAL(size_t(44), mv.size());
  std::copy(mv.begin() + 3, mv.begin() + 7, mv.begin() + 11);
  BOOST_CHECK_EQUAL(size_t(44), mv.size());

  /* check the data is there, with constant array this time.
     Now, we have:
     mv[0:9]=0:9
     mv[10] = mv[9] = 9
     mv[11:14] = mv[3:6] = 3:6
     mv[15:] = 13:
  */
  {
    TupleVector::PropertyPointer<int> intRef = mv.getProperty<int>(intProp);

    size_t i = 0;
    BOOST_FOREACH(TupleVector::const_reference ref,
		  static_cast<const TupleVector &>(mv)) {
      if (i < 10) {
	BOOST_CHECK_EQUAL(intRef[ref], int(i));
      } else if (i == 10) {
	BOOST_CHECK_EQUAL(intRef[ref], 9);
      } else if (i < 15) {
	BOOST_CHECK_EQUAL(intRef[ref], int(i - 8));
      } else {
	BOOST_CHECK_EQUAL(intRef[ref], int(i - 2));
      }
      i++;
    }
  }
}

BOOST_FIXTURE_TEST_CASE(allocation_test, Fixture)
{
  mv.clear();

  // stress-test by continous growing
  // checks realloc code
  for (size_t i = 0; i < 10000; ++i)
    mv.insert(mv.end());

  BOOST_CHECK_EQUAL(size_t(10000), mv.size());

  // add property to new field
  // checks malloc code
  TupleVector::PropertyId newProp = mv.addProperty<float>(3);

  // try wether writing to present and new fields raises memory problems
  {
    TupleVector::PropertyPointer<int> intRef = mv.getProperty<int>(intProp);
    TupleVector::ArrayPropertyPointer<float> fltRef
      = mv.getArrayProperty<float>(floatProp);
    TupleVector::ArrayPropertyPointer<float> fltNRef
      = mv.getArrayProperty<float>(newProp);

    
    BOOST_FOREACH(TupleVector::reference ref, mv) {
      intRef[ref] = 42;
      fltRef[ref][0] =   4.2;
      fltRef[ref][1] =  42.0;
      fltNRef[ref][0] =   1.2;
      fltNRef[ref][1] =  12.0;
      fltNRef[ref][2] = 120.0;
    }

    BOOST_FOREACH(TupleVector::reference ref, mv) {
      BOOST_CHECK_EQUAL(intRef[ref], 42);
      BOOST_CHECK_CLOSE(fltRef[ref][0],   4.2f, 1e-4f);
      BOOST_CHECK_CLOSE(fltRef[ref][1],  42.0f, 1e-4f);
      BOOST_CHECK_CLOSE(fltNRef[ref][0],   1.2f, 1e-4f);
      BOOST_CHECK_CLOSE(fltNRef[ref][1],  12.0f, 1e-4f);
      BOOST_CHECK_CLOSE(fltNRef[ref][2], 120.0f, 1e-4f);
    }
  }

  mv.eraseProperty(newProp);

  for (size_t i = 0; i < 10000; ++i)
    mv.erase(mv.begin());

  BOOST_CHECK_EQUAL(size_t(0), mv.size());
}

/* Quality assurance, for checking that implicit conversion
   has no unwanted overhead in the generated assembly code,
   which would be desastrous for the various references/iterators */

const int qa_intValue = 42;

TupleVector::reference qa_reference_(TupleVector &tv)
{ return tv[qa_intValue]; }
TupleVector::const_reference qa_const_reference_(TupleVector &tv)
{ return tv[qa_intValue]; }
TupleVector::const_reference qa_const_reference_indirect_(TupleVector &tv)
{ return TupleVector::reference(tv[qa_intValue]); }
TupleVector::fat_reference qa_fat_reference_(TupleVector &tv)
{ return tv[qa_intValue]; }

TupleVector::iterator qa_iterator_(TupleVector &tv)
{ return tv.begin(); }
TupleVector::const_iterator qa_const_iterator_(TupleVector &tv)
{ return tv.begin(); }
TupleVector::const_iterator qa_const_iterator_indirect_(TupleVector &tv)
{ return TupleVector::iterator(tv.begin()); }
TupleVector::fat_iterator qa_fat_iterator_(TupleVector &tv)
{ return tv.begin(); }

/* test that using fat_iterators in a loop is also not desastrous.
   Note that this loop really would crash if executed. */
size_t qa_manual_loop_(TupleVector &tv)
{
  TupleVector::fat_iterator it = tv.begin();
  const TupleVector::fat_iterator end = tv.end();
  size_t cnt = 0;
  while (it != end) {
    *it++ = *it;
    cnt++;
  }
  return cnt;
}

