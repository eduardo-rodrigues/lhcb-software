//-----------------------------------------------------------------------------
/** @file TStation.cpp
 *
 *  Implementation file for RICH reconstruction tool : TStation
 *
 *  CVS Log :-
 *  $Id: TStation.cpp,v 1.7 2010-03-17 16:37:54 jblouw Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "TStation.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TStation );

//-----------------------------------------------------------------------------

TStation::TStation( const std::string& name,
                              ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name , pSvcLocator )
{



  // define track containers
  declareProperty("InputContainer", m_inputcontainer = TrackLocation::Default );
  declareProperty("OTTrackContainer", m_OTTrackLocation = "Event/Alignment/OTTracks" );
  declareProperty("ITTrackContainer", m_ITTrackLocation = "Event/Alignment/ITTracks" );
  declareProperty("CrossedTrackContainer", m_XTrackLocation = "Event/Alignment/XTracks");
  declareProperty( "MinITHitCut",  m_minITHitCut   = 0.0 );
  declareProperty( "MinOTHitCut",  m_minOTHitCut   = 0.0 );
  declareProperty( "MinTTHitCut",  m_minTTHitCut   = 0.0 );
  declareProperty( "MinVeloHitCut",  m_minVeloHitCut   = 0.0 );

  
}

TStation::~TStation() { 
        info() << "TStation destructor" << endreq;
}

StatusCode TStation::initialize() {
  info() << "Initialising TStation alignment..." << endreq;
  const StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire the track selection tool
  info() << "create track selection tool" << endreq;
  info() << "Using " << m_OTTrackLocation << " to select OTTracks from " << m_inputcontainer << endreq;

  m_trackselection = tool<IATrackSelectorTool>( "ATrackSelector");
  m_trackselection->initialize();

  m_trackenergy = tool<ITrackCaloMatch>( "TrackCaloMatch" );
  setHistoTopDir("OT/");
  CaloEnergy = book( "CaloEnergy", "Calorimeter energy on track", 0.0, 12000.0 );

  return sc;
}

StatusCode TStation::execute() {
  setHistoTopDir("OT/");
  Tracks* inCont = get<Tracks>(m_inputcontainer);
  ITTracks = new Tracks();
  OTTracks = new Tracks();
  XTracks = new Tracks();
  if ( ITTracks->size() > 0 ) ITTracks->clear();
  if ( OTTracks->size() > 0 ) OTTracks->clear();
  if ( XTracks->size() > 0 ) XTracks->clear();
  
//   debug() << "Number of tracks in container " << inCont->end() - inCont->begin() << endreq;
   for ( Tracks::iterator iterT = inCont->begin();
 	iterT != inCont->end(); iterT++) {
     Track* aTrack = *iterT;
//     int OThits = 0, IThits = 0;
     if ( m_trackselection->accept( *aTrack ) ) {
         double energy = m_trackenergy->energy( *aTrack );
         if ( energy > 0.0 ) 
	    fill(CaloEnergy, energy, 1.0);
         debug() << "Energy = " << energy << " on track type " << aTrack->TypesToString(aTrack->type()) << endreq;
//       int hot = 0;
//       hot = m_trackselection->traversesIT( *aTrack, OThits, IThits );
// //      if ( hot == 10 ) debug() << "Error: found " << OThits << " OThits on track and " << IThits << " IThits." << endreq;
//       if ( hot > 0 && IThits >= m_minITHitCut ) {
//         ITTracks->insert( aTrack->clone() );
//       } else if ( hot < 0 && OThits >= m_minOTHitCut ) {
         OTTracks->insert( aTrack->clone() );
//       } else if ( OThits+IThits > m_minOTHitCut + m_minITHitCut ) {
//         XTracks->insert( aTrack->clone() );
//       }
     } else {
     debug() << "Track not selected!" << " "
           << *iterT << endreq; 
    }
  }
//   debug() << "Into TStation execute(): storing tracks..." <<endreq;
//   debug() << "Size of ITTrack container: " << (*ITTracks).size() << endreq;
//   put ( ITTracks, m_ITTrackLocation  );
//   debug() << "Size of OTTrack container: " << (*OTTracks).size() << endreq;
   put ( OTTracks, m_OTTrackLocation  );
//   debug() << "Size of XTrack container: " << (*XTracks).size() << endreq;
//   put ( XTracks, m_XTrackLocation  );
  return StatusCode::SUCCESS;
}

StatusCode TStation::finalize(){
  debug() << "==> Finalize" << endmsg;
  return GaudiTupleAlg::finalize();
}
