
//-----------------------------------------------------------------------------
/** @file RichExpectedTrackSignal.cpp
 *
 *  Implementation file for tool : Rich::Rec::ExpectedTrackSignal
 *
 *  CVS Log :-
 *  $Id: RichExpectedTrackSignal.cpp,v 1.23 2007-06-22 14:35:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichExpectedTrackSignal.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( ExpectedTrackSignal );

// Standard constructor
ExpectedTrackSignal::ExpectedTrackSignal ( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_geomEff      ( 0 ),
    m_sellmeir     ( 0 ),
    m_sigDetEff    ( 0 ),
    m_richPartProp ( 0 ),
    m_rayScat      ( 0 )
{
  declareInterface<IExpectedTrackSignal>(this);
}

StatusCode ExpectedTrackSignal::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichGeomEff" ,           m_geomEff      );
  acquireTool( "RichSellmeirFunc",       m_sellmeir     );
  acquireTool( "RichSignalDetectionEff", m_sigDetEff    );
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichRayleighScatter",    m_rayScat      );
  acquireTool( "RichGasQuartzWindow",    m_gasQuartzWin );

  return sc;
}

StatusCode ExpectedTrackSignal::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double ExpectedTrackSignal::nEmittedPhotons ( LHCb::RichRecSegment * segment,
                                              const Rich::ParticleIDType id ) const
{

  if ( !segment->nEmittedPhotons().dataIsValid(id) )
  {

    // loop over energy bins
    double signal = 0;
    Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra = segment->emittedPhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin )
    {

      double phots =
        m_sellmeir->photonsInEnergyRange( segment,
                                          id,
                                          spectra.binEnergyLowerEdge(iEnBin),
                                          spectra.binEnergyUpperEdge(iEnBin) );
      if ( phots<0 ) phots = 0;
      (spectra.energyDist(id))[iEnBin] = phots;
      signal += phots;

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " nEmittedPhotons = " << signal << endreq;
    }

    segment->setNEmittedPhotons( id, signal );
  }

  return segment->nEmittedPhotons( id );
}

double ExpectedTrackSignal::nDetectablePhotons (  LHCb::RichRecSegment * segment,
                                                  const Rich::ParticleIDType id ) const
{

  if ( !segment->nDetectablePhotons().dataIsValid(id) )
  {

    // Make sure emitted Photons are calculated
    nEmittedPhotons( segment, id );

    // loop over energy bins
    double signal = 0;
    Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & emitSpectra 
      = segment->emittedPhotonSpectra();
    Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & detSpectra  
      = segment->detectablePhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < emitSpectra.energyBins(); ++iEnBin )
    {
      const double sig = (emitSpectra.energyDist(id))[iEnBin] *
        m_sigDetEff->photonDetEfficiency( segment, emitSpectra.binEnergy(iEnBin) );
      const double gasQuartzWinTrans =
        m_gasQuartzWin->photonTransProb(segment,emitSpectra.binEnergy(iEnBin));
      signal += ( (detSpectra.energyDist(id))[iEnBin] = sig*gasQuartzWinTrans );
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " nDetectablePhotons = " << signal << endreq;
    }

    segment->setNDetectablePhotons( id, signal );
  }

  return segment->nDetectablePhotons( id );
}

double
ExpectedTrackSignal::nSignalPhotons (  LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id ) const {

  if ( !segment->nSignalPhotons().dataIsValid( id ) )
  {
    double signal  = 0;
    double scatter = 0;

    // compute detectable emitted photons
    double detectablePhots = nDetectablePhotons( segment, id );
    if ( detectablePhots > 0 )
    {

      // which radiator
      const Rich::RadiatorType rad = segment->trackSegment().radiator();

      // loop over energy bins
      Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & sigSpectra 
        = segment->signalPhotonSpectra();
      const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & detSpectra 
        = segment->detectablePhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < detSpectra.energyBins(); ++iEnBin )
      {

        const double scattProb =
          ( rad != Rich::Aerogel ? 0 :
            m_rayScat->photonScatteredProb( segment,
                                            detSpectra.binEnergy(iEnBin) ) );

        // observable photons * signal/scatter prob
        (sigSpectra.energyDist(id))[iEnBin] =
          (detSpectra.energyDist(id))[iEnBin] * (1.-scattProb);
        signal  += (sigSpectra.energyDist(id))[iEnBin];
        scatter += (detSpectra.energyDist(id))[iEnBin] * scattProb;

      } // energy bin loop

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " nSignalPhotons = " << signal
              << " nScatteredPhotons = " << scatter << endreq;
    }

    segment->setNSignalPhotons( id, signal );
    segment->setNScatteredPhotons( id, scatter );
  }

  return segment->nSignalPhotons( id );
}

double
ExpectedTrackSignal::avgSignalPhotEnergy(  LHCb::RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) const
{

  // NB : If used often this method should cache information in segment

  // make sure signal photons are calculated
  double nSig = nSignalPhotons ( segment, id );

  double avgEnergy = 0;
  if ( nSig> 0 )
  {

    // loop over energy bins
    const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra = segment->signalPhotonSpectra();
    double totalEnergy = 0;
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) {
      avgEnergy   += (spectra.energyDist(id))[iEnBin] * spectra.binEnergy(iEnBin);
      totalEnergy += (spectra.energyDist(id))[iEnBin];
    } // energy bin loop
    avgEnergy = ( totalEnergy>0 ? avgEnergy/totalEnergy : 0 );

  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment " << segment->key() << " " << id
            << " avgSignalPhotEnergy = " << avgEnergy << endreq;
  }

  return avgEnergy;
}

double
ExpectedTrackSignal::avgEmitPhotEnergy( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) const
{
  // NB : If used often this method should cache information in segment

  // make sure signal photons are calculated
  const double nSig = nEmittedPhotons ( segment, id );

  double avgEnergy = 0;
  if ( nSig> 0 )
  {

    // loop over energy bins
    Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra = segment->emittedPhotonSpectra();
    double totalEnergy = 0;
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) {
      avgEnergy   += (spectra.energyDist(id))[iEnBin] * spectra.binEnergy(iEnBin);
      totalEnergy += (spectra.energyDist(id))[iEnBin];
    } // energy bin loop
    avgEnergy = ( totalEnergy>0 ? avgEnergy/totalEnergy : 0 );

  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment " << segment->key() << " " << id
            << " avgEmitPhotEnergy = " << avgEnergy << endreq;
  }

  return avgEnergy;
}

double
ExpectedTrackSignal::nScatteredPhotons ( LHCb::RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const
{
  if ( !segment->nScatteredPhotons().dataIsValid(id) ) {
    // Scattered componented is calculated with the signal
    nSignalPhotons( segment, id );
  }

  return segment->nScatteredPhotons( id );
}

double
ExpectedTrackSignal::nTotalObservablePhotons ( LHCb::RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const
{
  return ( m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id) ) +
    ( m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id) );
}

double
ExpectedTrackSignal::nObservableSignalPhotons ( LHCb::RichRecSegment * segment,
                                                const Rich::ParticleIDType id ) const
{
  return m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id);
}

double
ExpectedTrackSignal::nObservableScatteredPhotons ( LHCb::RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const
{
  return m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id);
}

double
ExpectedTrackSignal::nSignalPhotons ( LHCb::RichRecTrack * track,
                                      const Rich::ParticleIDType id ) const
{

  if ( !track->nSignalPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nSignalPhotons( *segment, id );
    }
    track->setNSignalPhotons( id, signal );
  }

  return track->nSignalPhotons( id );
}

double
ExpectedTrackSignal::nObservableSignalPhotons ( LHCb::RichRecTrack * track,
                                                const Rich::ParticleIDType id ) const
{
  if ( !track->nObservableSignalPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nObservableSignalPhotons( *segment, id );
    }
    track->setNObservableSignalPhotons( id, signal );
  }

  return track->nObservableSignalPhotons( id );
}

double
ExpectedTrackSignal::nScatteredPhotons ( LHCb::RichRecTrack * track,
                                         const Rich::ParticleIDType id ) const
{
  if ( !track->nScatteredPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nScatteredPhotons( *segment, id );
    }
    track->setNScatteredPhotons( id, signal );
  }

  return track->nScatteredPhotons( id );
}

double
ExpectedTrackSignal::nObservableScatteredPhotons ( LHCb::RichRecTrack * track,
                                                   const Rich::ParticleIDType id ) const
{
  if ( !track->nObservableScatteredPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nObservableScatteredPhotons( *segment, id );
    }
    track->setNObservableScatteredPhotons( id, signal );
  }

  return track->nObservableScatteredPhotons( id );
}

double
ExpectedTrackSignal::nTotalObservablePhotons ( LHCb::RichRecTrack * track,
                                               const Rich::ParticleIDType id ) const
{
  return nObservableSignalPhotons( track, id ) + nObservableScatteredPhotons( track, id );
}

double
ExpectedTrackSignal::nEmittedPhotons ( LHCb::RichRecTrack * track,
                                       const Rich::ParticleIDType id ) const
{
  if ( !track->nEmittedPhotons().dataIsValid(id) ) {
    double signal = 0.0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nEmittedPhotons( *segment, id );
    }
    track->setNEmittedPhotons( id, signal );
  }

  return track->nEmittedPhotons( id );
}

double
ExpectedTrackSignal::nDetectablePhotons ( LHCb::RichRecTrack * track,
                                          const Rich::ParticleIDType id ) const
{
  if ( !track->nDetectablePhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nDetectablePhotons( *segment, id );
    }
    track->setNDetectablePhotons( id, signal );
  }

  return track->nDetectablePhotons( id );
}

bool
ExpectedTrackSignal::activeInRadiator( LHCb::RichRecTrack * track,
                                       const Rich::RadiatorType rad,
                                       const Rich::ParticleIDType id ) const
{
  for ( LHCb::RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( rad == (*segment)->trackSegment().radiator() )
    {
      if ( nEmittedPhotons(*segment,id) > 0 ) return true;
    }
  }

  return false;
}

bool
ExpectedTrackSignal::hasRichInfo( LHCb::RichRecSegment * segment ) const
{
  // default to no info
  bool hasInfo = false;
  // above electron hypothesis ?
  if ( aboveThreshold( segment, Rich::Electron ) )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment is above electron threshold -> hasRichInfo" << endreq;
    }

    // see if any mass hypothesis is detectable
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      if ( m_geomEff->geomEfficiency(segment,(Rich::ParticleIDType)iHypo) > 0 )
      {
        hasInfo = true; break;
      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      const std::string & A = ( hasInfo ? "non-zero" : "zero" );
      debug() << "RichRecSegment has " << A << " geometrical efficiency" << endreq;
    }
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment is below electron threshold -> noRichInfo" << endreq;
  }

  return hasInfo;
}

bool
ExpectedTrackSignal::hasRichInfo( LHCb::RichRecTrack * track ) const
{
  for ( LHCb::RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( hasRichInfo(*segment) ) return true;
  }
  return false;
}

bool
ExpectedTrackSignal::aboveThreshold( LHCb::RichRecSegment * segment,
                                     const Rich::ParticleIDType type ) const
{
  // Geometrical track segment
  const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();

  // momentum for this track segment
  const double P = sqrt(tkSeg.bestMomentum().mag2());
  // Adjust momentum to account for a 1 sigma fluctuation,
  // so segment is really above threshold but measured below
  const double Perr = tkSeg.middleErrors().errP();

  // is this momentum above the cherenkov threshold momentum
  const double pthres = m_richPartProp->thresholdMomentum(type,tkSeg.radiator());
  //const bool above = ( P > pthres );
  const bool above = ( P+Perr > pthres );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Threshold check : " << tkSeg.radiator() << " " << type
            << " : P=" << P << " Perr=" << Perr << " Pthres=" << pthres
            << " : above=" << above << endreq;
  }

  // return status
  return above;
}

bool
ExpectedTrackSignal::aboveThreshold( LHCb::RichRecTrack * track,
                                     const Rich::ParticleIDType type ) const
{
  // loop over segments
  for ( LHCb::RichRecTrack::Segments::iterator segment = track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( aboveThreshold(*segment,type) ) return true;
  }
  return false;
}

bool
ExpectedTrackSignal::aboveThreshold( LHCb::RichRecTrack * track,
                                     const Rich::ParticleIDType type,
                                     const Rich::RadiatorType radiator ) const
{
  // loop over segments
  for ( LHCb::RichRecTrack::Segments::iterator segment = track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( radiator == (*segment)->trackSegment().radiator() )
    {
      if ( aboveThreshold( *segment, type ) ) return true;
    }
  }
  return false;
}

// Set the threshold information in a RichPID object for given track
void
ExpectedTrackSignal::setThresholdInfo( LHCb::RichRecTrack * track,
                                       LHCb::RichPID * pid ) const
{

  // Assume all hypos are above threshold
  pid->setElectronHypoAboveThres(true);
  pid->setMuonHypoAboveThres(true);
  pid->setPionHypoAboveThres(true);
  pid->setKaonHypoAboveThres(true);
  pid->setProtonHypoAboveThres(true);

  // Now find those which aren't

  if ( aboveThreshold(track,Rich::Proton) &&
       nTotalObservablePhotons(track,Rich::Proton)>0 ) { return; }
  pid->setProtonHypoAboveThres(false);

  if ( aboveThreshold(track,Rich::Kaon) &&
       nTotalObservablePhotons(track,Rich::Kaon)>0 ) { return; }
  pid->setKaonHypoAboveThres(false);

  if ( aboveThreshold(track,Rich::Pion) &&
       nTotalObservablePhotons(track,Rich::Pion)>0 ) { return; }
  pid->setPionHypoAboveThres(false);

  if ( aboveThreshold(track,Rich::Muon) &&
       nTotalObservablePhotons(track,Rich::Muon)>0 ) { return; }
  pid->setMuonHypoAboveThres(false);

  if ( aboveThreshold(track,Rich::Electron) &&
       nTotalObservablePhotons(track,Rich::Electron)>0 ) { return; }
  pid->setElectronHypoAboveThres(false);

}

// Set the threshold information in a RichPID object for given segment
void
ExpectedTrackSignal::setThresholdInfo( LHCb::RichRecSegment * segment,
                                       LHCb::RichPID * pid ) const
{

  // Assume all hypos are above threshold
  pid->setElectronHypoAboveThres(true);
  pid->setMuonHypoAboveThres(true);
  pid->setPionHypoAboveThres(true);
  pid->setKaonHypoAboveThres(true);
  pid->setProtonHypoAboveThres(true);

  // Now find those which aren't

  if ( aboveThreshold(segment,Rich::Proton) &&
       nTotalObservablePhotons(segment,Rich::Proton)>0 ) { return; }
  pid->setProtonHypoAboveThres(false);

  if ( aboveThreshold(segment,Rich::Kaon) &&
       nTotalObservablePhotons(segment,Rich::Kaon)>0 ) { return; }
  pid->setKaonHypoAboveThres(false);

  if ( aboveThreshold(segment,Rich::Pion) &&
       nTotalObservablePhotons(segment,Rich::Pion)>0 ) { return; }
  pid->setPionHypoAboveThres(false);

  if ( aboveThreshold(segment,Rich::Muon) &&
       nTotalObservablePhotons(segment,Rich::Muon)>0 ) { return; }
  pid->setMuonHypoAboveThres(false);

  if ( aboveThreshold(segment,Rich::Electron) &&
       nTotalObservablePhotons(segment,Rich::Electron)>0 ) { return; }
  pid->setElectronHypoAboveThres(false);

}
