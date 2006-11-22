// $Id: CaloTriggerBitsFromRawAlg.cpp,v 1.3 2006-11-22 12:29:33 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/L0PrsSpdHit.h"

// local
#include "CaloTriggerBitsFromRawAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerBitsFromRawAlg
//
// 2006-04-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloTriggerBitsFromRawAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerBitsFromRawAlg::CaloTriggerBitsFromRawAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputData"  , m_outputData  );  
  declareProperty("ToolName"    , m_toolName    );

  m_toolType  = "CaloTriggerBitsFromRaw";
  m_toolName = name + "Tool";
  if ( "Prs" == name.substr( 0 , 3 ) ) {
    m_outputData = rootOnTES() + LHCb::L0PrsSpdHitLocation::Prs;
    m_isPrs = true;
  } else if ( "Spd" == name.substr( 0 , 3 ) ) {
    m_outputData = rootOnTES() + LHCb::L0PrsSpdHitLocation::Spd;
    m_isPrs = false;
  }

}
//=============================================================================
// Destructor
//==============h===============================================================
CaloTriggerBitsFromRawAlg::~CaloTriggerBitsFromRawAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloTriggerBitsFromRawAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" <<  endreq;  
  m_l0BitTool = tool<ICaloTriggerBitsFromRaw>( m_toolType, m_toolName , this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloTriggerBitsFromRawAlg::execute() {

  debug() << "==> Execute"  <<endmsg;

  //*** create the output container
  LHCb::L0PrsSpdHits* newL0Bits = new LHCb::L0PrsSpdHits();
  put( newL0Bits , m_outputData );

  //*** get the input data from Raw and fill the output container
  std::vector<LHCb::CaloCellID> l0Cells = m_l0BitTool->firedCells(m_isPrs );
  std::vector<LHCb::CaloCellID>::const_iterator iCell;
  
  for( iCell = l0Cells.begin(); l0Cells.end() != iCell ; ++iCell ) {
    LHCb::L0PrsSpdHit* l0Bit = new LHCb::L0PrsSpdHit( *iCell );
    newL0Bits->insert( l0Bit ) ;
  };
  
  debug() << " L0PrsSpdHits container size " << newL0Bits->size() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloTriggerBitsFromRawAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
