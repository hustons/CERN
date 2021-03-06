// $Id: MC.h, 14/04/2011 rhuston Exp $
#ifndef MC_H 
#define MC_H 1 

//#####################//
//### INCLUDE FILES ###//
//#####################//

//### from DaVinci, this is a specialized GaudiAlgorithm ###//
#include "Kernel/DaVinciTupleAlgorithm.h"
//### MC data retrieval ###//
#include "Event/MCParticle.h"
//### Is MC Reconstructible? ###//
#include "MCInterfaces/IMCReconstructible.h"
//### Extract L0 Decision ###//
#include "Event/L0DUReport.h"
//### Association between MC and Reconstructed###//
#include "Kernel/Particle2MCLinker.h"
// Convert MCParticle to Particle??
#include "Kernel/MCParticleMakerBase.h"
#include "Kernel/IParticleMaker.h"
#include "GaudiAlg/GaudiTool.h"

/*
 * @class MC MC.h
 *  
 *  Master algorithm for handling Monte Carlo data
 *  Will be used to call invMassMC and plotMC
 *  
 *  @author Shane Huston
 *  @date   14-04-2011
 */

class MC : public DaVinciTupleAlgorithm 
{
public: 
  MC( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MC( );                    ///< Destructor
  virtual StatusCode initialize();   ///< Algorithm initialization
  virtual StatusCode execute   ();   ///< Algorithm execution
  virtual StatusCode finalize  ();   ///< Algorithm finalization

  // Get Invariant Mass of two MC Daughters
  StatusCode invMassMC(LHCb::MCParticle::Vector&, LHCb::MCParticle::Vector&, LHCb::MCParticle::Vector, Tuple mcTuple,
                       const std::string&);
  // Plot MC data
  StatusCode plotMC(const LHCb::MCParticle*, const LHCb::MCParticle*, LHCb::MCParticle::Vector, Tuple, const std::string&);

protected:

private:
  int m_errorCode;                       //< Error code for nTuples
  ISvcLocator *m_local;                  //< Test to use pSvcLocator to call external local algorithms 
  int m_coneMax;                         //< Defines cone around muon for isolation criteria
  int m_numDivs;                         //< Defines the number of differen cones to analyse
  double m_pi;                           //< Defines pi for calcultion of azimuthal angle difference
};
#endif // MC_H
