
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracks.cpp
 *
 *  Implementation file for tool : Rich::Rec::TrackCreatorFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromRecoTracks.cpp,v 1.17 2009-08-06 18:10:57 smenzeme Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromRecoTracks.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-------------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackCreatorFromRecoTracks );

// Standard constructor
TrackCreatorFromRecoTracks::
TrackCreatorFromRecoTracks( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : TrackCreatorBase       ( type, name, parent ),
    m_trTracks             ( NULL ),
    m_massHypoRings        ( NULL ),
    m_segMaker             ( NULL ),
    m_signal               ( NULL ),
    m_trSegToolNickName    ( "RichTrSegMakerFromRecoTracks" ),
    m_allDone              ( false ),
    m_buildHypoRings       ( false )
{

  // declare interface for this tool
  declareInterface<ITrackCreator>(this);

  // job options
  declareProperty( "TracksLocation",           m_trTracksLocation   );
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings     );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName  );

}

StatusCode TrackCreatorFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TrackCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( m_trTracksLocation.empty() )
  { return Error( "Input Track location is not set" ); }

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_signal         );
  acquireTool( m_trSegToolNickName,       m_segMaker, this );
  if ( m_buildHypoRings )
  {
    acquireTool( "RichMassHypoRings", m_massHypoRings );
    info() << "Will create Mass hypothesis rings for each track" << endmsg;
  }

  return sc;
}

StatusCode TrackCreatorFromRecoTracks::newTracks() const
{

  if ( !m_allDone )
  {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    const LHCb::Tracks * tracks = trTracks();
    if ( tracks )
    {

      // check the number of input tracks
      if ( tracks->size() > m_maxInputTracks )
      {
        procStatus()->addAlgorithmStatus( name(), Rich::Rec::ReachedTrTrackLimit );
        std::ostringstream mess;
        mess << "Number of input tracks exceeds maximum of "
             << m_maxInputTracks << " -> Abort";
        return Warning( mess.str(), StatusCode::SUCCESS, 1 );
      }
      
      // make rich tracks
      richTracks()->reserve( nInputTracks() );
      for ( LHCb::Tracks::const_iterator track = tracks->begin();
            track != tracks->end(); ++track )
      {
        newTrack( *track );
      }
      
      // Too many selected tracks ?
      if ( richTracks()->size() > m_maxSelTracks )
      {
        procStatus()->addAlgorithmStatus( name(), Rich::Rec::ReachedRichTrackLimit );
        richTracks()->clear();
        std::ostringstream mess;
        mess << "Number of RICH tracks exceeds maximum of "
             << m_maxSelTracks << " -> Abort";
        return Warning( mess.str(), StatusCode::SUCCESS, 0 );
      }
      
    }
    
  }
  
  return StatusCode::SUCCESS;
}

long TrackCreatorFromRecoTracks::nInputTracks() const
{
  const LHCb::Tracks * tracks = trTracks();
  return ( tracks ? tracks->size() : 0 );
}

const LHCb::Tracks *
TrackCreatorFromRecoTracks::trTracks() const
{
  if ( !m_trTracks )
  {
    // Obtain smart data pointer to Tracks
    if ( exist<LHCb::Tracks>(m_trTracksLocation) )
    {
      m_trTracks = get<LHCb::Tracks>( m_trTracksLocation );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "located " << m_trTracks->size() << " Tracks at "
                << m_trTracksLocation << endmsg;
      }
    }
    else
    {
      Warning( "No input Tracks at '"+m_trTracksLocation+"'" ).ignore();
    }
  }

  return m_trTracks;
}

// Forms a new RichRecTrack object from a Track
LHCb::RichRecTrack *
TrackCreatorFromRecoTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a Track ?
  const LHCb::Track * trTrack = dynamic_cast<const LHCb::Track*>(obj);
  if ( !trTrack )
  {
    Warning( "Input data object is not of type 'Track'" ).ignore();
    return NULL;
  }

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // track type
  Rich::Rec::Track::Type trType = Rich::Rec::Track::Unknown;
  try { trType = Rich::Rec::Track::type(trTrack); }
  // Catch exceptions ( track type unknown )
  catch ( const GaudiException & expt )
  {
    Error( expt.message(), expt.code() ).ignore();
    return NULL;
  }

  // unique ?
  const bool trUnique = !trTrack->checkFlag(LHCb::Track::Clone);

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << trTrack->key()
              << " type " << trType << ", unique=" << trUnique
              << ", charge=" << trTrack->charge()
              << endmsg;
  }

  // Is track a usable type
  if ( !Rich::Rec::Track::isUsable(trType) ) return NULL;

  // Get reference to track stats object
  TrackCount & tkCount = trackStats().trackStats(trType,trUnique);

  // See if this RichRecTrack already exists
  if ( bookKeep() && m_trackDone[trTrack->key()] )
  {
    return richTracks()->object(trTrack->key());
  }
  else
  {

    // Add to reference map
    if ( bookKeep() ) m_trackDone[trTrack->key()] = true;

    // count tried tracks
    ++tkCount.triedTracks;

    // New track object pointer
    LHCb::RichRecTrack * newTrack = NULL;

    // Track selection
    if ( trackSelector().trackSelected(trTrack) )
    {

      // Form the RichRecSegments for this track
      std::vector<LHCb::RichTrackSegment*> segments;
      const int Nsegs = m_segMaker->constructSegments( trTrack, segments );

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " Found " << Nsegs << " radiator segment(s)" << endmsg;

      if ( 0 < Nsegs )
      {

        // Form a new RichRecTrack
        newTrack = new LHCb::RichRecTrack();

        // Configure TrackID for this Track
        newTrack->trackID().initialiseFor( trTrack );

        bool keepTrack = false;
        for ( std::vector<LHCb::RichTrackSegment*>::iterator iSeg = segments.begin();
              iSeg != segments.end(); ++iSeg )
        {
          if ( !(*iSeg) ) continue;

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "  -> Testing " << (*iSeg)->radiator() << " segment" << endmsg;

          // make a new RichRecSegment from this RichTrackSegment
          // takes ownership of RichTrackSegment* *iSeg - responsible for deletion
          LHCb::RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

          // Get PD panel impact point
          if ( rayTraceHPDPanelPoints(**iSeg,newSegment) )
          {
            // test if this segment has valid information
            if ( m_signal->hasRichInfo(newSegment) )
            {

              if ( msgLevel(MSG::VERBOSE) )
                verbose() << "   -> TrackSegment in " << (*iSeg)->radiator() << " selected" << endmsg;

              // keep track
              keepTrack = true;

              // Save this segment
              segmentCreator()->saveSegment( newSegment );

              // Add to the SmartRefVector of RichSegments for this RichRecTrack
              newTrack->addToRichRecSegments( newSegment );

              // set radiator info
              setDetInfo( newTrack, (*iSeg)->radiator() );

              // make RichRecRings for the mass hypotheses if requested
              if ( m_buildHypoRings ) m_massHypoRings->massHypoRings( newSegment );

              // Count radiator segments
              tkCount.countRadiator( (*iSeg)->radiator() );

            }
            else
            {
              if ( msgLevel(MSG::VERBOSE) )
              {
                verbose() << "   -> TrackSegment has no RICH info -> rejected" << endmsg;
              }
              delete newSegment;
              newSegment = NULL;
            }

          }
          else
          {
            if ( msgLevel(MSG::VERBOSE) )
            {
              verbose() << "   -> TrackSegment does not trace to an HPD panel -> rejected" << endmsg;
            }
            delete newSegment;
            newSegment = NULL;
          }

        } // end loop over RichTrackSegments

        if ( keepTrack )
        {

          // give to container
          richTracks()->insert( newTrack, trTrack->key() );

          // reserve size in vectors
          newTrack->richRecPhotons().reserve(30);
          newTrack->richRecPixels().reserve(30);

          // Set vertex momentum
          newTrack->setVertexMomentum ( static_cast<LHCb::RichRecTrack::FloatType>(trTrack->p())  );
          newTrack->setVertexPt       ( static_cast<LHCb::RichRecTrack::FloatType>(trTrack->pt()) );

          // chi2
          newTrack->setChi2PerDoF( static_cast<LHCb::RichRecTrack::FloatType>(trTrack->chi2PerDoF()) );
          newTrack->setNDoF      ( trTrack->nDoF()       );

          // track charge
          newTrack->setCharge( trTrack->charge() );

          // clone variable
          newTrack->setCloneDist( static_cast<LHCb::RichRecTrack::FloatType>(trTrack->info( LHCb::Track::CloneDist,
                                                                                            newTrack->cloneDist() )) );

          // likelihood
          newTrack->setLikelihood( static_cast<LHCb::RichRecTrack::FloatType>(trTrack->likelihood()));

          // ghost prob
          newTrack->setGhostProbability( static_cast<LHCb::RichRecTrack::FloatType>(trTrack->ghostProbability()));

          // Set parent information
          newTrack->setParentTrack( trTrack );

          // Count selected tracks
          ++(tkCount.selectedTracks);

        }
        else
        {
          delete newTrack;
          newTrack = NULL;
        }

      } // end segments if

    } // end track selection if

    return newTrack;
  }

}

void TrackCreatorFromRecoTracks::InitNewEvent()
{
  RichTrackCreatorBase::InitNewEvent();
  m_allDone  = false;
  m_trTracks = NULL;
}
