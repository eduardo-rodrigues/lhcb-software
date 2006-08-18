// $Id: NeutralPP2MC.cpp,v 1.11 2006-08-18 11:59:57 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.11 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2006/06/23 14:54:39  phicharp
// Adapt to DC06 and new event model, remove tools
//
// Revision 1.8  2005/05/09 11:37:26  pkoppenb
// Fix compilation warning
//
// Revision 1.7  2005/05/09 11:20:47  ibelyaev
//  modification for NeutralMMC2MC
//
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"
// Event
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
// Associators
#include "Relations/RelationWeighted1D.h"

// ============================================================================
// CaloEvent/Event
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/PP2MCLocation.h"
// ============================================================================
// DaVinci associators 
// ============================================================================
#include "DaVinciAssociators/Particle2MCLinker.h"
// ============================================================================
// local 
// ============================================================================
#include "AsctAlgorithm.h"
// ============================================================================

/** @class NeutralPP2MC NeutralPP2MC.cpp
 *  
 *  The algorithm for building the relation table for 
 *  MC truth information for neutrals protoparticles.
 *
 *  The major properties of the algorithm :
 *  
 *   - "InputData"
 *      The defualt value is ProtoParticleLocation::Neutrals
 *      The list of (input) location of ProtoParticles 
 *
 *   - "OutputTable" 
 *      The defualt valeu is  NeutralPP2MCAsctLocation
 *      The locatioin of (output) relation table for
 *      ProtoParticle -> MCRelation 
 *   
 *   - "MCCaloTable"
 *      The default value is "Rec/Relations/EcalClusters2MCParticles"
 *      The locatiin in TES the major source of MC information : 
 *      the relation table for CaloCluster -> MCParticle relations 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */

static const std::string& 
NeutralPP2MCAsctLocation = LHCb::ProtoParticle2MCLocation::Neutrals;

class NeutralPP2MC
  : public AsctAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<NeutralPP2MC>;
public:
  /** standard execution of the algorithm
   *  @return StatusCode
   */
  virtual StatusCode execute    () ;
protected:  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  NeutralPP2MC
  ( const std::string& name , 
    ISvcLocator*       svc  )
    : AsctAlgorithm ( name , svc )
    //
    , m_mcTable ( "Rec/Relations/EcalClusters2MCParticles" ) 
  { 
    // define proper default for input data (location of Protoparticles )
    m_inputData.push_back( LHCb::ProtoParticleLocation::Neutrals );
    // define proper default for output data (location of relation table )
    m_outputTable = NeutralPP2MCAsctLocation;
    // define proper default for CaloCLuster->MCPArticle data 
    declareProperty ( "MCCaloTable" , m_mcTable ) ;
  };
  /// destructor (virtual and protected)
  virtual ~NeutralPP2MC(){};
private:
  /// default  constructor is private
  NeutralPP2MC();
  /// copy     constructor is private
  NeutralPP2MC
  ( const NeutralPP2MC& );
  /// assignement operator is private
  NeutralPP2MC& operator=
  ( const NeutralPP2MC& );
private:
  /// the location oif Calo Cluster -> MCParticle table 
  std::string m_mcTable ;
};

using namespace LHCb;


// ============================================================================
// Algorithm Factory
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  local concrete factory for algorithm instantiation
   *  @see NeutralPP2MC
   */
  // ==========================================================================
  const  AlgFactory<NeutralPP2MC>         s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var NeutralPP2MCFactory
 *  abstract (importable) factory for algorithm instantiation
 *  @see NeutralPP2MC
 */
// ============================================================================
const   IAlgFactory&NeutralPP2MCFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** standard execution of the algorithm
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode NeutralPP2MC::execute() 
{
  // avoid the long name and always use "const" qualifier
  typedef const SmartRefVector<CaloHypo>                        Hypos    ;
  typedef const SmartRefVector<CaloCluster>                     Clusters ;
  
  // ProtoParticle -> MCParticle relations   (MC output) 
  typedef RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle, double>  Table;
  
  // CaloCluster -> MCParticle relations     (MC input)
  typedef const IRelationWeighted<CaloCluster,MCParticle,float> MCTable  ;
  typedef const MCTable::Range                                  Range    ;
  typedef       MCTable::iterator                               relation ;

  // input locations 
  typedef std::vector<std::string>                              Inputs   ;
  
  // get the CaloCluster->MCParticle table from TES 
  const MCTable* mcTable = get<MCTable> ( m_mcTable ) ;
  if ( 0 == mcTable ) { return StatusCode::FAILURE ; }       // RETURN 
  
  // create relation table  
  Table* table = 0 ;
  if ( !outputTable().empty() ) 
  {
    // create new table 
    table = new Table() ;
    // register it in TES 
    put( table, outputTable() );
  };
  
  // Loop on all input containers of ProtoParticles
  for ( Inputs::const_iterator inp = m_inputData.begin(); 
        m_inputData.end()!= inp; inp++) 
  {
    // get the protoparticles from TES 
    const ProtoParticles* protos = get<ProtoParticles> ( *inp ) ;
    if ( 0 == protos ) { return StatusCode::FAILURE ; }       // RETURN 
    
    // Create a linker table
    const std::string linkContainer = 
      (*inp) + Particle2MCMethod::extension[Particle2MCMethod::NeutralPP];
    
    // Just a fake helper class
    Object2MCLinker p2MCLink(this);
    Object2MCLinker::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    //
    if ( NULL == table && NULL == linkerTable ) { continue; } // CONTINUE
    
    for ( ProtoParticles::const_iterator ipp = protos->begin() ; 
          protos->end() != ipp ; ++ipp ) 
    {
      const ProtoParticle* pp = *ipp ;
      // skip nulls
      if ( 0 == pp ) { continue ; }
      
      //      bool matched = false;
      // get all calo hypos 
      const Hypos& hypos = pp->calo() ;
      // loop over all hypotheses for givel protoparticle
      for ( Hypos::const_iterator ihypo = hypos.begin() ; 
            hypos.end() != ihypo ; ++ihypo) 
      {
        const CaloHypo* hypo = *ihypo ;
        // skip nulls
        if ( 0 == hypo  ) { continue ; }                         // CONTINUE 
        // get all clusters from the hypo 
        const Clusters& clusters = hypo->clusters();
        // loop over all clusters for given hypo 
        for ( Clusters::const_iterator icluster = clusters.begin() ; 
              clusters.end() != icluster ; ++icluster ) 
        {
          const CaloCluster* cluster = *icluster ;
          // skip nulls 
          if ( 0 == cluster ) { continue ; }                  // CONTINUE 
          // get all relations form the cluster 
          Range range = mcTable -> relations ( cluster ) ;
          
          for ( relation rel = range.begin() ; range.end() != rel ; ++rel ) 
          {
            const MCParticle* mcPart = rel->to();
            // skip nulls 
            if ( 0 == mcPart ) { continue ; }              // CONTINUE 
            // propagate the relation to protoparticle 
            const double weight = rel->weight() / cluster->e() ;
            if ( NULL != table )
            { table       -> relate (  pp , mcPart , weight ) ; }
            if ( NULL != linkerTable ) 
            { linkerTable -> link   (  pp , mcPart , weight ) ; }
            
          } // end of loop over MC relations
        } // end of loop over clusters 
      } // end of loop over hypothesis
    } // end of loop over protoparticles  
  } // end of loop over input containters 
  
  if ( msgLevel( MSG::DEBUG ) && 0 != table )  
  {
    debug() << "Number of MC links are " 
            << table->relations().size() << endreq ;
  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
