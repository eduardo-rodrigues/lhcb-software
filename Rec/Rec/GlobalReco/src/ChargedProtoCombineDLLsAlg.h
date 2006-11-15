
//-----------------------------------------------------------------------------
/** @file ChargedProtoCombineDLLsAlg.h
 *
 * Header file for algorithm ChargedProtoCombineDLLsAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoCombineDLLsAlg.h,v 1.2 2006-11-15 16:42:46 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/11/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoCombineDLLsAlg_H
#define GLOBALRECO_ChargedProtoCombineDLLsAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoCombineDLLsAlg ChargedProtoCombineDLLsAlg.h
 *
 *  Algorithm to calculate Combined DLL values for ProtoParticles, and store
 *  this information in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 15/11/2006
 */
//-----------------------------------------------------------------------------

class ChargedProtoCombineDLLsAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoCombineDLLsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoCombineDLLsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // utility classes

  //-----------------------------------------------------------------------------
  /** @class CombinedLL ChargedProtoPAlg.h
   *
   *  Utility class holding the combined LL values for a ProtoParticle
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date 29/03/2006
   */
  //-----------------------------------------------------------------------------
  class CombinedLL
  {
  public:
    /// Standard constructor with initialisation value
    CombinedLL( const double init = 0 )
      : elDLL(init), muDLL(init), piDLL(init), kaDLL(init), prDLL(init) {};
    double elDLL; ///< Electron Log Likelihood
    double muDLL; ///< Muon Log Likelihood
    double piDLL; ///< Pion Log Likelihood
    double kaDLL; ///< Kaon Log Likelihood
    double prDLL; ///< Proton Log Likelihood
  };

private: // methods

  /// Add the Rich DLL information to the combined DLL
  void addRich( LHCb::ProtoParticle * proto, CombinedLL & combDLL ) const;

  /// Add the Muon DLL information to the combined DLL
  void addMuon( LHCb::ProtoParticle * proto, CombinedLL & combDLL ) const;

  /// Add the Calo DLL information to the combined DLL
  void addCalo( LHCb::ProtoParticle * proto, CombinedLL & combDLL ) const;

private: // data

  std::string m_protoPath;    ///< Location in TES of ProtoParticles

};

#endif // GLOBALRECO_ChargedProtoCombineDLLsAlg_H
