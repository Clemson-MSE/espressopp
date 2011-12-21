#include "Version.hpp"
#include "python.hpp"
#include <sstream>

#ifdef VTRACE
#include "vampirtrace/vt_user.h"
#else
# define VT_ON()
# define VT_OFF()
#endif

namespace espresso {

  Version::Version() {
	  name          = "ESPResSo++";
          major         = MAJORVERSION;
	  minor         = MINORVERSION;
	  patchlevel    = PATCHLEVEL;
	  hgrevision    = hgversion;
	  date          = __DATE__;
	  time          = __TIME__;
  }

  std::string Version::info() {
	  std::stringstream ss;
	  ss << name << " v"  << major << "." << minor;
	  ss << " patchlevel " << patchlevel;
	  ss << ", Mercurial(hg) revision: " << hgrevision;
	  ss << ", compiled on " << date << ", " << time;
#ifdef VTRACE
	  ss << ", VampirTrace mode";
#endif
	  return ss.str();
  }

  //////////////////////////////////////////////////
  // REGISTRATION WITH PYTHON
  //////////////////////////////////////////////////
  void
  Version::registerPython() {
    using namespace espresso::python;

    class_< Version >
      ("Version", init<>())
      .def_readonly("major", &Version::major)
      .def_readonly("minor", &Version::minor)
      .def_readonly("hgrevision", &Version::hgrevision)
      .def_readonly("patchlevel", &Version::patchlevel)
      .def_readonly("date", &Version::date)
      .def_readonly("time", &Version::time)
      .def_readonly("name", &Version::name)
      .def("info", &Version::info);
  }
}
