
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSel.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::TrackSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTrTrackSel.cpp,v 1.33 2007-12-14 14:21:18 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDTrTrackSel.h"

// From GaudiKernel
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackSel );

// Standard constructor, initializes variables
TrackSel::TrackSel( const std::string& name,
                    ISvcLocator* pSvcLocator )
  : AlgBase ( name, pSvcLocator ),
    m_tkSignal       ( 0 ),
    m_trSelector     ( 0 ),
    m_frozenTrSel    ( 0 ),
    m_minPhysPtot    ( 0 ),
    m_minLLPtot      ( 0 ),
    m_resetToPion    ( false ),
    m_maxUsedTracks  ( 0 ),
    m_maxInputTracks ( 0 )
{

  // Selection cuts
  declareProperty( "MinimumPhysicsMomentum",  m_minPhysPtot = 0.0*Gaudi::Units::GeV );
  declareProperty( "MinimumLikelihoodMomentum", m_minLLPtot = 0.0*Gaudi::Units::GeV );
  declareProperty( "ResetTracksToPion", m_resetToPion = false );
  declareProperty( "MaxUsedTracks", m_maxUsedTracks = 250 );
  declareProperty( "MaxInputTracks", m_maxInputTracks = 350 );
  declareProperty( "FreezeSelectedTracks", m_freezeTracks = false );
  declareProperty( "FrozenTrackMassHypo",  m_frozenType = (int)Rich::Pion );

}

// Destructor
TrackSel::~TrackSel() {}

//  Initialize
StatusCode TrackSel::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );
  acquireTool( "TrackSelector", m_trSelector, this );

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biasing any timing numbers
  trackCreator();
  segmentCreator();

  if ( m_freezeTracks )
  {
    info() << "Will freeze selected tracks to mass hypothesis "
           << (Rich::ParticleIDType)m_frozenType << endreq;
    acquireTool( "TrackSelector", "FrozenTrackSelector", m_frozenTrSel, this );
  }
  
  return sc;
}

StatusCode TrackSel::eventInit()
{ 
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check if track processing was aborted.
  if ( procStatus()->aborted() ) 
  {
    procStatus()->addAlgorithmStatus( gpidName(), Rich::Rec::ProcStatAbort );
    deleteGPIDEvent();
    return Warning("Processing aborted -> Abort",StatusCode::SUCCESS);
  }

  // Check the number of input raw tracks
  if ( trackCreator()->nInputTracks() > m_maxInputTracks ) 
  {
    procStatus()->addAlgorithmStatus( gpidName(), Rich::Rec::ReachedTrTrackLimit );
    deleteGPIDEvent();
    return Warning("Max. number of input tracks exceeded -> Abort",StatusCode::SUCCESS);
  }

  // Make sure RichRecTracks are available
  if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  if ( richTracks()->empty() ) 
  {
    procStatus()->addAlgorithmStatus( gpidName(), Rich::Rec::NoRichTracks );
    deleteGPIDEvent();
    return Warning("No tracks selected -> Abort",StatusCode::SUCCESS);
  } 
  else if ( richTracks()->size() > m_maxUsedTracks )
  {
    procStatus()->addAlgorithmStatus( gpidName(), Rich::Rec::ReachedRichTrackLimit );
    deleteGPIDEvent();
    return Warning("Max. number of RICH tracks exceeded -> Abort",StatusCode::SUCCESS);
  }

  return StatusCode::SUCCESS;
}

// Select tracks for analysis
StatusCode TrackSel::execute()
{
  // event initisation
  const StatusCode sc = eventInit();
  if ( sc.isFailure() ) return sc;

  // Iterate over all RichRecTracks and choose those to use
  for ( LHCb::RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track ) 
  {

    // select tracks for use
    Rich::Rec::GlobalPID::TkQuality quality = trackStatus( *track );
    if ( Rich::Rec::GlobalPID::Unusable == quality ) 
    {
      (*track)->setInUse( false );
      continue;
    } 
    else 
    {
      (*track)->setInUse( true );
    }

    // Set to pion hypothesis if configured to do so
    if ( m_resetToPion ) (*track)->setCurrentHypothesis( Rich::Pion );
    if ( msgLevel(MSG::VERBOSE) )
    { verbose() << "Track " << (*track)->key() << " has initial hypothesis "
                << (*track)->currentHypothesis() << endreq; }
    
    // Make a new RichGlobalPIDTrack
    LHCb::RichGlobalPIDTrack * pidTrack = new LHCb::RichGlobalPIDTrack();
    gpidTracks()->insert( pidTrack, (*track)->key() );

    // Make new PID result and give to track
    LHCb::RichGlobalPID * newPID = new LHCb::RichGlobalPID();
    gpidPIDs()->insert( newPID, (*track)->key() );

    // Frozen track ?
    if ( m_freezeTracks )
    {
      // is this track selected as one to freeze ?
      if ( m_frozenTrSel->trackSelected(*track) )
      {
        // freeze the track
        pidTrack->setFrozenHypo(true);
        // Set mass hypo to frozen type
        (*track)->setCurrentHypothesis( (Rich::ParticleIDType)m_frozenType );
        // label track as one not to have a final PID object 
        quality = Rich::Rec::GlobalPID::LikelihoodOnly;
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " -> Track is selected to be frozen to type " 
                    << (Rich::ParticleIDType)m_frozenType << endreq;
        }
      }
    }

    // set track reference
    pidTrack->setGlobalPID( newPID );

    // Set Track reference
    const LHCb::Track * trtrack = dynamic_cast<const LHCb::Track *>((*track)->parentTrack());
    if ( !trtrack ) Warning( "Input track type is not Track -> RichPID has null track reference" );
    newPID->setTrack( trtrack );

    // Store threshold information
    m_tkSignal->setThresholdInfo( (*track), newPID );

    // Set quality
    pidTrack->setTrQuality( quality );

    // Set its SmartRef to RichRecTrack
    pidTrack->setRichRecTrack( *track );

  } // track loop

  if ( gpidTracks()->empty() ) 
  {
    procStatus()->addAlgorithmStatus( gpidName(), Rich::Rec::NoRichTracks );
    deleteGPIDEvent();
    return Warning("No tracks selected -> Abort",StatusCode::SUCCESS);
  }

  return sc;
}

Rich::Rec::GlobalPID::TkQuality
TrackSel::trackStatus( LHCb::RichRecTrack * track ) 
{
  // Set default quality to be good
  Rich::Rec::GlobalPID::TkQuality quality = Rich::Rec::GlobalPID::Physics;

  // Momentum
  const double pTot = track->vertexMomentum();

  // If below minimum momentum for use in LL, return false
  if ( pTot < m_minLLPtot ) 
  {
    quality = Rich::Rec::GlobalPID::Unusable;
  }
  // Likelihood use only ?
  else if ( pTot >= m_minLLPtot && pTot < m_minPhysPtot ) 
  {
    quality = Rich::Rec::GlobalPID::LikelihoodOnly;
  }

  // Only use requested track types
  if ( !trackSelection(track) ) quality = Rich::Rec::GlobalPID::Unusable;

  return quality;
}

bool TrackSel::trackSelection( LHCb::RichRecTrack * track ) const
{
  return m_trSelector->trackSelected(track);
}
