// $Id: RichPhotonCreator.cpp,v 1.3 2003-07-03 13:09:01 jonesc Exp $

// local
#include "RichPhotonCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonCreator
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreator>          s_factory ;
const        IToolFactory& RichPhotonCreatorFactory = s_factory ;

// Standard constructor
RichPhotonCreator::RichPhotonCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichPhotonCreator>(this);

  declareProperty( "RichRecPhotonLocation",
                   m_richRecPhotonLocation = RichRecPhotonLocation::Default );

  m_minCKtheta.push_back( 0.09 );   // aerogel
  m_minCKtheta.push_back( 0 );      // c4f10
  m_minCKtheta.push_back( 0 );      // cf4
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

  m_maxCKtheta.push_back( 0.300 ); // aerogel
  m_maxCKtheta.push_back( 0.080 ); // c4f10
  m_maxCKtheta.push_back( 0.050 ); // cf4
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  m_minPhotonProb.push_back( 1e-15 ); // aerogel
  m_minPhotonProb.push_back( 1e-15 ); // c4f10
  m_minPhotonProb.push_back( 1e-15 ); // cf4
  declareProperty( "MinPhotonProbability", m_minPhotonProb );

}

StatusCode RichPhotonCreator::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);
  acquireTool("RichTrackCreator", m_trackCreator);
  acquireTool("RichPhotonSignal", m_photonSignal);
  acquireTool("RichPixelCreator", m_pixelCreator);
  acquireTool("RichPhotonPredictor", m_photonPredictor);

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    return StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->release();
  }

  // Informational Printout
  msg << MSG::DEBUG
      << " Min Cherenkov Theta          = " << m_minCKtheta << endreq
      << " Max Cherenkov Theta          = " << m_maxCKtheta << endreq
      << " Min Photon Probability       = " << m_minPhotonProb << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonCreator::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  if ( m_evtDataSvc ) { m_evtDataSvc->release(); m_evtDataSvc = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPhotonCreator::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_photonDone.clear();

    SmartDataPtr<RichRecPhotons> tdsPhotons( m_evtDataSvc,
                                             m_richRecPhotonLocation );
    if ( !tdsPhotons ) {

      // Reinitialise the Photon Container
      m_photons = new RichRecPhotons();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !m_evtDataSvc->registerObject(m_richRecPhotonLocation, m_photons) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecPhotons at "
            << m_richRecPhotonLocation << endreq;
      }

    } else {

      // Set smartref to TES photon container
      m_photons = tdsPhotons;

      // Remake local photon reference map
      for ( RichRecPhotons::const_iterator iPhoton = tdsPhotons->begin();
            iPhoton != tdsPhotons->end();
            ++iPhoton ) {
        m_photonDone[(int)(*iPhoton)->key()] = true;
      }

    }

  }

}

RichRecPhoton*
RichPhotonCreator::reconstructPhoton( RichRecSegment * segment,
                                      RichRecPixel * pixel ) {

  // check photon is possible before proceeding
  if ( !m_photonPredictor->photonPossible(segment, pixel) ) return NULL;

  // Form the key for this photon
  RichRecPhotonKey photonKey;
  photonKey.setSegmentNumber( segment->key() );
  photonKey.setPixelNumber( pixel->key() );

  // See if this photon already exists
  if ( m_photonDone[(int)photonKey] ) {
    return (RichRecPhoton*)(m_photons->object(photonKey));
  } else {
    return buildPhoton( segment, pixel, photonKey );
  }

}

RichRecPhoton * RichPhotonCreator::buildPhoton( RichRecSegment * segment,
                                                RichRecPixel * pixel,
                                                RichRecPhotonKey & key ) {

  RichRecPhoton * newPhoton = NULL;

  // Reconstruct the Cherenkov angles
  RichGeomPhoton geomPhoton;
  if ( m_richDetInt->reconstructPhoton( segment->trackSegment(),
                                        pixel->globalPosition(),
                                        geomPhoton ) != 0 ) {

    Rich::RadiatorType rad = segment->trackSegment().radiator();
    if ( ( geomPhoton.CherenkovTheta() > 0. ||
           geomPhoton.CherenkovPhi() > 0. ) &&
         geomPhoton.CherenkovTheta() < m_maxCKtheta[rad] &&
         geomPhoton.CherenkovTheta() > m_minCKtheta[rad] ) {

      newPhoton = new RichRecPhoton();

      // give photon same smart ID as pixel
      geomPhoton.setSmartID( pixel->smartID() );

      // set geometrical photon information
      newPhoton->setGeomPhoton( geomPhoton );

      // Set various navigation info
      newPhoton->setRichRecSegment( segment );
      newPhoton->setRichRecTrack( segment->richRecTrack() );
      newPhoton->setRichRecPixel( pixel );

      // check photon has significant probability to be signal for any
      // hypothesis. If not then reject and delete
      bool keepPhoton = false;
      for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
        if ( m_photonSignal->predictedPixelSignal(newPhoton,
                       (Rich::ParticleIDType)iHypo) > m_minPhotonProb[rad] ) {
          keepPhoton = true;
          break;
        }
      }

      if ( keepPhoton ) {

        m_photons->insert( newPhoton, key );

        // add this photon to pixel/segment/track references
        segment->addToRichRecPixels( pixel );
        segment->addToRichRecPhotons( newPhoton );
        segment->richRecTrack()->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecTracks( segment->richRecTrack() );
        SmartRefVector<RichRecPixel> & tkPixs =
          segment->richRecTrack()->richRecPixels();
        bool notThere = true;
        for ( SmartRefVector<RichRecPixel>::iterator pix = tkPixs.begin();
              pix != tkPixs.end(); ++pix ) {
          if ( (RichRecPixel*)(*pix) == pixel ) { notThere = false; break; }
        }
        if ( notThere ) segment->richRecTrack()->addToRichRecPixels( pixel );

      } else {
        delete newPhoton;
        newPhoton = NULL;
      }

    }

  }

  // Add to reference map
  m_photonDone[(int)key] = true;

  // Return pointer to this photon
  return newPhoton;

}

void RichPhotonCreator::reconstructPhotons() {

  bool noPhots = m_photons->empty();

  // Iterate over all tracks
  for ( RichRecTracks::iterator iTrack =
          m_trackCreator->richTracks()->begin();
        iTrack != m_trackCreator->richTracks()->end();
        ++iTrack ) {
    RichRecTrack * track = *iTrack;
    if ( !track->inUse() ) continue; // skip tracks not "on"

    // Iterate over pixels
    for ( RichRecPixels::iterator iPixel =
            m_pixelCreator->richPixels()->begin();
          iPixel != m_pixelCreator->richPixels()->end();
          ++iPixel ) {
      RichRecPixel * pixel = *iPixel;

      // Iterate over segments
      for ( SmartRefVector<RichRecSegment>::iterator iSegment =
              track->richRecSegments().begin();
            iSegment != track->richRecSegments().end();
            ++iSegment) {
        RichRecSegment * segment = *iSegment;

        // If container was empty, skip checks for whether photon already exists
        if ( noPhots ) {
          if ( m_photonPredictor->photonPossible( segment, pixel ) ) {
            RichRecPhotonKey photonKey;
            photonKey.setSegmentNumber( segment->key() );
            photonKey.setPixelNumber( pixel->key() );
            buildPhoton( segment, pixel, photonKey );
          }
        } else {
          reconstructPhoton( segment, pixel );
        }

      } // segment loop
    } // pixel loop
  } // track loop


}

SmartRefVector<RichRecPhoton>&
RichPhotonCreator::reconstructPhotons( RichRecTrack * track ) {

  // Iterate over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment) { reconstructPhotons( *segment ); }

  return track->richRecPhotons();
}

SmartRefVector<RichRecPhoton>&
RichPhotonCreator::reconstructPhotons( RichRecSegment * segment ) {

  // Iterate over pixels
  for ( RichRecPixels::iterator pixel =
          m_pixelCreator->richPixels()->begin();
        pixel != m_pixelCreator->richPixels()->end();
        ++pixel ) { reconstructPhoton( segment, *pixel ); }

  return segment->richRecPhotons();
}

SmartRefVector<RichRecPhoton>&
RichPhotonCreator::reconstructPhotons( RichRecPixel * pixel ) {

  // Iterate over tracks
  for ( RichRecTracks::iterator track =
          m_trackCreator->richTracks()->begin();
        track != m_trackCreator->richTracks()->end();
        ++track ) {
    if ( !(*track)->inUse() ) continue;
    reconstructPhotons( *track, pixel );
  }

  return pixel->richRecPhotons();
}

// Note to self. Need to review what this method passes back
SmartRefVector<RichRecPhoton>
RichPhotonCreator::reconstructPhotons( RichRecTrack * track,
                                       RichRecPixel * pixel ) {

  SmartRefVector<RichRecPhoton> photons;

  // Iterate over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {

    RichRecPhoton * photon = reconstructPhoton( *segment, pixel );
    if ( photon ) photons.push_back( photon );

  }

  return photons;
}

RichRecPhotons *& RichPhotonCreator::richPhotons() {
  return m_photons;
}

