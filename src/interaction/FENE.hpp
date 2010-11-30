// ESPP_CLASS
#ifndef _INTERACTION_FENE_HPP
#define _INTERACTION_FENE_HPP

#include "Potential.hpp"
#include "FixedPairListInteractionTemplate.hpp"
#include <cmath>

namespace espresso {
  namespace interaction {
    /** This class provides methods to compute forces and energies of
        the FENE potential.

        \f[ V(r) = -\frac{1}{2} \Delta r_{max}^2 K \log \left[ 1 -
        \left(\frac{r-r_0}{\Delta r_{max}} \right)^2 \right]
        \f]
    */
    class FENE : public PotentialTemplate< FENE > {
    private:
      real K;
      real r0;
      real rMax;

    public:
      static void registerPython();

      FENE()
	: K(0.0), r0(0.0), rMax(0.0) {
	setShift(0.0);
	setCutoff(infinity);
      }

      FENE(real _K, real _r0, real _rMax, 
		   real _cutoff, real _shift) 
	: K(_K), r0(_r0), rMax(_rMax) {
	setShift(_shift);
	setCutoff(_cutoff);
      }

      FENE(real _K, real _r0, real _rMax, 
		   real _cutoff)
	: K(_K), r0(_r0), rMax(_rMax) 
      {	
	autoShift = false;
	setCutoff(_cutoff);
	setAutoShift(); 
      }

      // Setter and getter
      void setK(real _K) {
	K = _K;
	updateAutoShift();
      }
      real getK() const { return K; }

      void setR0(real _r0) { 
	r0 = _r0; 
	updateAutoShift();
      }
      real getR0() const { return r0; }

      void setRMax(real _rMax) { 
	rMax = _rMax; 
	updateAutoShift();
      }
      real getRMax() const { return rMax; }

      real _computeEnergySqrRaw(real distSqr) const {
        real energy = -0.5 * pow(rMax, 2) * K *
                      log(1 - pow((sqrt(distSqr) - r0) / rMax, 2));
	return energy;
      }

      bool _computeForceRaw(Real3D& force,
			    const Real3D& dist,
			    real distSqr) const {

        // real r = sqrt(distSqr);
        // real ffactor = -K * (r - r0) / (1 - pow((r - r0) / rMax, 2)) / r;

        real r0sq = rMax * rMax;
        real rlogarg = 1.0 - distSqr / r0sq;
        real ffactor = -K / rlogarg;
        force = dist * ffactor;
        return true;

      }
    };
  }
}

#endif
