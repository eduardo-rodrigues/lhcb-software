// $Id: $

// local
#include "RecProcessingTimeMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecProcessingTimeMoni
//
// 2010-07-15 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecProcessingTimeMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecProcessingTimeMoni::RecProcessingTimeMoni( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_hist        ( NULL )
{
  declareProperty( "Algorithms",      m_algNames );
  declareProperty( "LogMinEventTime", m_logMinTime = -2.0 );
  declareProperty( "LogMaxEventTime", m_logMaxTime =  8.0 );
  setProperty( "HistoTopDir", "Timing/" );
}

//=============================================================================
// Destructor
//=============================================================================
RecProcessingTimeMoni::~RecProcessingTimeMoni() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecProcessingTimeMoni::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  

  // are we properly configured
  if ( m_algNames.empty() ) { sc = Warning( "No algorithms to time !"); }

  // book the histogram at initialization time
  m_hist = book( "overallTime", "log10(Event Processing Time / ms)", 
                 m_logMinTime, m_logMaxTime, 100 );

  // return
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RecProcessingTimeMoni::execute() 
{

  // Loop over algorithms to include in the timing and add them up
  double time = 0;
  for ( AlgorithmNames::const_iterator name = m_algNames.begin();
        name != m_algNames.end(); ++name )
  {
    const double alg_time = 
      chronoSvc()->chronoDelta((*name)+":Execute",IChronoStatSvc::ELAPSED)/1000;
    time += alg_time;
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << *name << " " << alg_time << endmsg;
  }

  // only fill if algorithm(s) ran (time>0)
  if ( time > 0 )
  {
    // Take the base 10 log of the time (helps show the large tails)
    fill( m_hist, std::log10(time), 1.0 );
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
