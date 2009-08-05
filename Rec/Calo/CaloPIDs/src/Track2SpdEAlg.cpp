// $Id: Track2SpdEAlg.cpp,v 1.6 2009-08-05 17:35:34 ibelyaev Exp $
// ============================================================================
// Local
// ============================================================================
#include "CaloTrack2IDAlg.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
/** @class Track2SpdEAlg Track2SpdEAlg.cpp
 *  preconfigured instance of class  CaloTrack2IDAlg
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-15
 */
class Track2SpdEAlg : public CaloTrack2IDAlg
{
  // ==========================================================================
  friend class AlgFactory<Track2SpdEAlg> ;
  // ==========================================================================
protected:  
  // ==========================================================================
  /// Standard protected constructor
  Track2SpdEAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrack2IDAlg( name , pSvc ) 
  {
    if( "HLT"==context() ){
      _setProperty ( "Output" , LHCb::CaloIdLocation::SpdEHlt  ) ;
      _setProperty ( "Filter" , LHCb::CaloIdLocation::InSpdHlt  ) ;
    }else{
      _setProperty ( "Output" , LHCb::CaloIdLocation::SpdE  ) ;
      _setProperty ( "Filter" , LHCb::CaloIdLocation::InSpd  ) ;     
    }
    _setProperty ( "Tool"   , "SpdEnergyForTrack/SpdE"   ) ;
  } 
  /// virtual protected destructor 
  virtual ~Track2SpdEAlg(){} 
  // ==========================================================================
private:
  // ==========================================================================
  Track2SpdEAlg() ;
  Track2SpdEAlg           ( const Track2SpdEAlg& ) ;
  Track2SpdEAlg& operator=( const Track2SpdEAlg& ) ;
  // ==========================================================================
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( Track2SpdEAlg );
// ============================================================================
// The END 
// ============================================================================
