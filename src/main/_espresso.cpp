/*
 This file contains the main routine for the embedded python
 interpreter version of ESPResSo. 
*/
#include "acconfig.hpp"
#include "espresso_common.hpp"
#include "python.hpp"

static void finalize();

BOOST_PYTHON_MODULE(_espresso)
{
  initMPIEnv();

  // register all classes with python 
  espresso::registerPython();

  Py_AtExit(&finalize);
}

void finalize() {
  finalizeMPIEnv();
}
