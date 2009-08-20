// $Id: HltLumiWriter.cpp,v 1.2 2009-08-20 07:27:31 panmanj Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"
#include "HltLumiWriter.h"


//-----------------------------------------------------------------------------
// Implementation file for class : HltLumiWriter
//
// 2008-07-22 : Jaap Panman (using the calo as template)
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HltLumiWriter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltLumiWriter::HltLumiWriter( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputBank", m_inputBank = LHCb::HltLumiSummaryLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
HltLumiWriter::~HltLumiWriter() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltLumiWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_bank.reserve(20);
  m_bankType  = LHCb::RawBank::HltLumiSummary;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Restart
//=============================================================================
StatusCode HltLumiWriter::restart() {
  debug() << "==> Restart" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltLumiWriter::execute() {

  debug() << "==> Execute" << endmsg;
  
  m_bank.clear( );

  //== Build the data banks
  fillDataBankShort( );
  
  int totDataSize = 0;

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  // set source, type, version 
  rawEvent->addBank( 0, m_bankType, 0, m_bank );   
  totDataSize += m_bank.size();

  m_totDataSize += totDataSize;
  m_nbEvents++;

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Bank size: ";
    debug() << format( "%4d ", m_bank.size() ) 
		       << "Total Data bank size " << totDataSize << endreq;
  }

  if ( MSG::VERBOSE >= msgLevel() ) {
    verbose() << "DATA bank : " << endreq;
    int kl = 0;
    std::vector<unsigned int>::const_iterator itW;
    
    for ( itW = m_bank.begin(); m_bank.end() != itW; itW++ ){
      verbose() << format ( " %8x %11d   ", (*itW), (*itW) );
      kl++;
      if ( 0 == kl%4 ) verbose() << endreq;
    }
      verbose() << endreq ;
  }
 
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltLumiWriter::finalize() {
  
  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    info() << "Average event size : " << format( "%7.1f words", m_totDataSize ) 
	   << endreq;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
};

//=========================================================================
//  Fill the data bank, structure: Key (upper 16 bits) + value
//=========================================================================
void HltLumiWriter::fillDataBankShort ( ) {

  if (!exist<LHCb::HltLumiSummary>(m_inputBank) ){
    debug() << m_inputBank << " not found" << endmsg ;
    return;
  }

  LHCb::HltLumiSummary* HltLumiSummary = get<LHCb::HltLumiSummary>(m_inputBank);
  debug() << m_inputBank << " found" << endmsg ;

  LHCb::HltLumiSummary::ExtraInfo::iterator summaryIter;
  LHCb::HltLumiSummary::ExtraInfo  summaryInfo = HltLumiSummary->extraInfo();
  for (summaryIter = summaryInfo.begin(); summaryIter != summaryInfo.end(); summaryIter++) {
    // get the key and value of the input info
    int key = summaryIter->first;
    int s_value = summaryIter->second;
    // handle overflow
    int i_value = (s_value < 0xFFFF) ? (int) s_value : (int)0xFFFF;
    unsigned int word = ( key << 16 ) | ( i_value & 0xFFFF );
    m_bank.push_back( word );
    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << format ( " %8x %11d %11d %11d ", word, word, key, i_value ) << endreq;
    }
  }
  
  
};
//=============================================================================
