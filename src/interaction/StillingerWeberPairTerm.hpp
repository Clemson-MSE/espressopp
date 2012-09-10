// ESPP_CLASS
#ifndef _INTERACTION_STILLINGERWEBERPAIRTERM_HPP
#define _INTERACTION_STILLINGERWEBERPAIRTERM_HPP

#include "Potential.hpp"

#include "VerletListInteractionTemplate.hpp"
#include "VerletListAdressInteractionTemplate.hpp"
#include "CellListAllPairsInteractionTemplate.hpp"
#include "FixedPairListInteractionTemplate.hpp"

#include <cmath>

namespace espresso {
  namespace interaction {
    /** This class provides methods to compute forces and energies of
	2 body term of Stillinger-Weber potential.
    */
    class StillingerWeberPairTerm : public PotentialTemplate< StillingerWeberPairTerm > {
    private:
      // 2 body
      real A, B, p, q;
      real epsilon;
      real sigma;
      
    public:
      static void registerPython();

      StillingerWeberPairTerm(): A(0.0), B(0.0), p(0.0), q(0.0), epsilon(0.0), sigma(0.0){
        setShift(0.0);
        setCutoff(infinity);
        preset();
      }

      StillingerWeberPairTerm(real _A, real _B, real _p, real _q, real _epsilon,
                real _sigma, real _cutoff):
                A(_A), B(_B), p(_p), q(_q), epsilon(_epsilon), sigma(_sigma){
        setShift(0.0);
        setCutoff(_cutoff);
        preset();
      }

      virtual ~StillingerWeberPairTerm() {};

      void setA(real _A) { A = _A; }
      real getA() const { return A; }

      void setB(real _B) { B = _B; }
      real getB() const { return B; }
      
      void setP(real _p) { p = _p; }
      real getP() const { return p; }
      
      void setQ(real _q) { q = _q; }
      real getQ() const { return q; }
      
      // Setter and getter
      void setEpsilon(real _epsilon) {
        epsilon = _epsilon;
        preset();
      }
      
      real getEpsilon() const { return epsilon; }

      void setSigma(real _sigma) { 
        sigma = _sigma; 
        preset();
      }
      real getSigma() const { return sigma; }

      
      void preset() {
      }

      real _computeEnergySqrRaw(real distSqr) const {
        real d12 = sqrt(distSqr);
        real inv_d12_a = 1.0 / (d12-getCutoff());
        //real energy = epsilon*A * ( B/pow(d12, p)-1.0/pow(d12, q) ) * exp ( inv_d12_a );
        real energy = A * ( B*pow(d12, -p) - pow(d12, -q) ) * exp ( inv_d12_a );
        return energy;
      }

      bool _computeForceRaw(Real3D& force, const Real3D& r12, real distSqr) const {

        real d12 = sqrt(distSqr);
        real inv_d12 = 1.0 / d12;
        real inv_d12_a = 1.0 / (d12-getCutoff());
        
        real term1 = B * pow(d12, -p);
        real term2 = pow(d12, -q);
        real term12 = term1 - term2;
        
        real energy = A * term12 * exp ( inv_d12_a );
        
        real inv_d12_a2 = inv_d12_a*inv_d12_a;
        
        real term3 = ( p*term1 - q*term2 ) * inv_d12 / term12;
        
        real ffactor = energy * ( term3 + inv_d12_a2);
        
        force = r12 * inv_d12 * ffactor;
        return true;
      }
    };
  }
}

#endif