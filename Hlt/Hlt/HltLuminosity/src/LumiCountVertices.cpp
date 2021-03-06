// $Id: LumiCountVertices.cpp,v 1.8 2009-11-26 10:53:14 panmanj Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"

#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

// local
#include "LumiCountVertices.h"

#include "boost/format.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountVertices
//
// 2008-07-21 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCountVertices )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCountVertices::LumiCountVertices( const std::string& name,
                                      ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator )
{
    declareProperty( "InputSelection", m_InputSelectionName );
    declareProperty( "CounterName", m_CounterName );
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiCountVertices::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    debug() << "InputSelection: " << boost::format( "%20s" ) % m_InputSelectionName
            << " "
            << "CounterName: " << boost::format( "%20s" ) % m_CounterName << " "
            << "OutputContainer: " << boost::format( "%20s" ) % m_OutputContainerName
            << endmsg;

    // ------------------------------------------
    m_Counter = LHCb::LumiCounters::counterKeyToType( m_CounterName );
    if ( m_Counter == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounter not found with name: " << m_CounterName << endmsg;
    } else {
        debug() << m_CounterName << " key value: " << m_Counter << endmsg;
    }
    // ------------------------------------------

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCountVertices::execute()
{

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

    const LHCb::RecVertices* container =
        getIfExists<LHCb::RecVertices>( m_InputSelectionName );
    int nCounter = container ? container->size() : -1;

    if ( msgLevel( MSG::DEBUG ) ) {
        if ( nCounter < 0 )
            debug() << m_InputSelectionName << " not found" << endmsg;
        else
            debug() << "There are " << nCounter << " vertices in "
                    << m_InputSelectionName << endmsg;
    }

    // get container
    LHCb::HltLumiSummary* sums =
        getOrCreate<HltLumiSummary, HltLumiSummary>( m_OutputContainerName );
    // add counter
    sums->addInfo( m_Counter, nCounter );

    setFilterPassed( true );

    return StatusCode::SUCCESS;
}
