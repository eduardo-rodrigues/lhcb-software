// $Id: RichPhotonSignal.cpp,v 1.1 2003-06-30 15:47:05 jonrob Exp $

// local
#include "RichPhotonSignal.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonSignal
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonSignal>          s_factory ;
const        IToolFactory& RichPhotonSignalFactory = s_factory ;

// Standard constructor
RichPhotonSignal::RichPhotonSignal( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichPhotonSignal>(this);

}

StatusCode RichPhotonSignal::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;
  
  // Acquire instances of tools
  acquireTool("RichSegmentProperties", m_segProps);

  // Temporary variables - should be got from XML eventually
  m_radiusCurv[ Rich::Rich1 ] = 240.0;
  m_radiusCurv[ Rich::Rich2 ] = 860.0;
  m_pixelArea = 0.24*0.24;

  // Informational Printout
  //msg << MSG::DEBUG

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonSignal::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichPhotonSignal::predictedPixelSignal( RichRecPhoton * photon,
                                               const Rich::ParticleIDType id ) {

  double prob = photon->expPixelSignalPhots( id );
  if ( prob < -0.5 ) {

    prob = photon->geomPhoton().activeSegmentFraction() *
      ( ( signalProb(photon, id) *
          m_segProps->nSignalPhotons(photon->richRecSegment(),id) ) +
        ( scatterProb(photon, id) *
          m_segProps->nScatteredPhotons(photon->richRecSegment(),id) ) );

    photon->setExpPixelSignalPhots( id, prob );

  }

  return prob;
}

double RichPhotonSignal::signalProb( RichRecPhoton * photon,
                                     const Rich::ParticleIDType& id ) {

  // Expected Cherenkov theta angle
  double thetaExp =
    m_segProps->avgCherenkovTheta( photon->richRecSegment(), id );
  if ( thetaExp < 0.000001 ) return 0.0;

  // Reconstructed Cherenkov theta angle
  double thetaReco = photon->geomPhoton().CherenkovTheta();

  // Expected Cherenkov theta angle resolution
  double thetaExpRes =
    m_segProps->ckThetaResolution( photon->richRecSegment(), id );

  double thetaDiff = thetaReco-thetaExp;
  if ( fabs(thetaDiff) > 30.0*thetaExpRes ) return 0.;

  // Which detector
  Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();

  double expArg = 0.5*thetaDiff*thetaDiff/(thetaExpRes*thetaExpRes);
  expArg = ( expArg>650 ? 650 : expArg );
  return ( exp(-expArg) / ( sqrt(2.*M_PI)*M_PI*thetaExpRes ) ) *
    (2.*m_pixelArea / ( m_radiusCurv[det]*m_radiusCurv[det] *
                        (thetaReco>0.0001 ? thetaReco : 0.0001) ) );
}

double RichPhotonSignal::scatterProb( RichRecPhoton * photon,
                                      const Rich::ParticleIDType& id ) {

  Rich::RadiatorType rad = photon->richRecSegment()->trackSegment().radiator();

  if ( rad == Rich::Aerogel ) {

    // Expected Cherenkov theta angle
    double thetaExp =
      m_segProps->avgCherenkovTheta( photon->richRecSegment(), id );
    if ( thetaExp < 0.000001 ) return 0.0;

    double thetaRec = photon->geomPhoton().CherenkovTheta();

    double fbkg = 0.0;
    if ( thetaRec < thetaExp ) {
      fbkg = ( exp(17.0*thetaRec) - 1.0 ) / ( exp(17.0*thetaExp) - 1.0 );
    } else if ( thetaRec < 0.5*M_PI + thetaExp - 0.04 ) {
      fbkg = cos( thetaRec - thetaExp + 0.04 );
      fbkg = fbkg*fbkg/0.9984;
    } else {
      return 0.0;
    }

    Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();
    return 8.*m_pixelArea*fbkg /
      (M_PI*M_PI*m_radiusCurv[det]*m_radiusCurv[det]*thetaRec);

  }

  return 0.;
}
