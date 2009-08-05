// $Id: PhotonMatchAlg.cpp,v 1.5 2009-08-05 17:35:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2008/06/30 15:37:34  odescham
// prepare for HLT processing
//
// Revision 1.3  2007/08/24 21:25:19  odescham
// fix uncheck. StatusCodes
//
// Revision 1.2  2006/06/20 18:17:49  odescham
// minor update to please ChargedProtoPAlg
//
// Revision 1.1  2006/06/18 18:35:29  ibelyaev
//  the firstcommmit for DC06 branch
//
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloCluster.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackMatchAlg.h"
// ============================================================================
/** @class PhotonMatchAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-16 
 */
// ============================================================================
class PhotonMatchAlg : public CaloTrackMatchAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<PhotonMatchAlg> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// execution of the algorithm
  virtual StatusCode execute () ;  
  // ==========================================================================
protected:
  // ==========================================================================
  /// standard (protected) constructor 
  PhotonMatchAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrackMatchAlg ( name , pSvc ) 
  {

    if( "HLT" == context() ){
      Inputs inputs = Inputs ( 1 , LHCb::CaloClusterLocation::EcalHlt    ) ;
      _setProperty ( "Calos"     , Gaudi::Utils::toString ( inputs )     ) ;
      _setProperty ( "Output"    , LHCb::CaloIdLocation::ClusterMatchHlt ) ;
      _setProperty ( "Filter"    , LHCb::CaloIdLocation::InEcalHlt       ) ;      
    }else{      
      Inputs inputs = Inputs ( 1 , LHCb::CaloClusterLocation::Ecal  ) ;
      _setProperty ( "Calos"     , Gaudi::Utils::toString ( inputs )  ) ;
      _setProperty ( "Output"    , LHCb::CaloIdLocation::ClusterMatch ) ;
      _setProperty ( "Filter"    , LHCb::CaloIdLocation::InEcal       ) ;
    }
    
    _setProperty ( "Tool"      , "CaloPhotonMatch/PhotonMatch" ) ;
    _setProperty ( "Threshold" , "1000"                        ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Long,
                     LHCb::Track::Downstream ,
                     LHCb::Track::Ttrack     ) ) ;
  }
  /// virtual and protected destrcutror
  virtual ~PhotonMatchAlg() {}
  // ==========================================================================
private:
  // ==========================================================================
  PhotonMatchAlg() ;
  PhotonMatchAlg           ( const PhotonMatchAlg& ) ;
  PhotonMatchAlg& operator=( const PhotonMatchAlg& ) ;
  // ==========================================================================
};
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( PhotonMatchAlg );
// ============================================================================
// Standard execution of the algorithm
// ============================================================================
StatusCode PhotonMatchAlg::execute () 
{
  Assert ( !m_tracks .empty() , "No Input tracks" ) ;
  Assert ( !m_calos  .empty() , "No Input Calos"  ) ;
  
  typedef LHCb::RelationWeighted2D<LHCb::CaloCluster,LHCb::Track,float> Table ;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::IClusTrTable2D));
  
  // create the relation table and register it in TES 
  Table* table = new Table ( 50 * 100  ) ; 
  put ( table , m_output ) ;
  
  // perform the actual jobs 
  return doTheJob<LHCb::CaloCluster,Table>( table ) ;
} 
// ============================================================================
// The END 
// ============================================================================
