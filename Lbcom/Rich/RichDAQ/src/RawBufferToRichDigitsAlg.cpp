
//-----------------------------------------------------------------------------
/** @file RawBufferToRichDigitsAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RawBufferToRichDigitsAlg
 *
 *  CVS Log :-
 *  $Id: RawBufferToRichDigitsAlg.cpp,v 1.17 2006-10-04 10:46:47 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBufferToRichDigitsAlg.h"

// namespaces
using namespace LHCb; ///< General LHCb namespace

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RawBufferToRichDigitsAlg>          s_factory ;
const        IAlgFactory& RawBufferToRichDigitsAlgFactory = s_factory ;

// Standard constructor, initializes variables
RawBufferToRichDigitsAlg::RawBufferToRichDigitsAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichAlgBase ( name , pSvcLocator )
{
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLoc = RichDigitLocation::Default );
  declareProperty( "DecodeBufferOnly", m_decodeOnly = false );
}

// Destructor
RawBufferToRichDigitsAlg::~RawBufferToRichDigitsAlg() {};

// Initialisation.
StatusCode RawBufferToRichDigitsAlg::initialize()
{
  // intialise base
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichSmartIDDecoder", m_decoder );

  if ( m_decodeOnly )
  { info() << "Will only decode Raw Buffer -> No RichDigits produced" << endreq; }

  return sc;
}

// Main execution
StatusCode RawBufferToRichDigitsAlg::execute()
{

  // Get RichSmartIDs decoded from RawEvent
  const RichDAQ::PDMap & smartIDs = m_decoder->allRichSmartIDs();

  if ( !m_decodeOnly )
  {
    // Make new container for RichDigits and give to Gaudi
    RichDigits * digits = new RichDigits();
    put( digits, m_richDigitsLoc );
    
    // Create a RichDigit for each SmartID
    for ( RichDAQ::PDMap::const_iterator iPD = smartIDs.begin();
          iPD != smartIDs.end(); ++iPD )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iPD).second.begin();
            iID != (*iPD).second.end(); ++iID )
      {
        digits->insert( new RichDigit(), *iID );
      }
    }
    
    // Final printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Successfully registered " << digits->size()
              << " RichDigits at " << m_richDigitsLoc << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
