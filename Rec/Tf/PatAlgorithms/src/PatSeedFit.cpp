// $Id: PatSeedFit.cpp,v 1.12 2009-12-07 09:45:09 mschille Exp $

#include <boost/foreach.hpp>
#include <boost/array.hpp>

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "Event/STLiteCluster.h"
#include "Event/Track.h"
#include "Event/OTLiteTime.h"
#include "Event/StateParameters.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackMomentumEstimate.h"
#include "STDet/DeSTDetector.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/RecoFuncs.h"

#include "PatSeedFit.h"
#include "PatSeedTool.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedFit
//
// 2009-01-28 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PatSeedFit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeedFit::PatSeedFit(const std::string& type,
                       const std::string& name,
                       const IInterface* parent)
  : GaudiTool(type, name, parent)
{
  declareInterface<IPatSeedFit>(this);
  declareProperty( "StateErrorX2",              m_stateErrorX2          =   4. );
  declareProperty( "StateErrorY2",              m_stateErrorY2          = 400. );
  declareProperty( "StateErrorTX2",             m_stateErrorTX2         = 6e-5 );
  declareProperty( "StateErrorTY2",             m_stateErrorTY2         = 1e-4 );
  declareProperty( "PatSeedDRatio",             m_dRatio          = -3.2265e-4 );
  declareProperty( "PatSeedInitialArrow",       m_initialArrow    = 4.25307e-9 );
  declareProperty( "TrackMaxChi2",              m_maxChi2               = -1.  );
  declareProperty( "zReference",		m_zReference = StateParameters::ZMidT );
  declareProperty( "MomentumScale",		m_momentumScale		= 44.1416);
}
//============================================================================
// Destructor
//=============================================================================
PatSeedFit::~PatSeedFit() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeedFit::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_patSeedTool = tool<PatSeedTool>("PatSeedTool", this);
  m_itDet = getDet<DeSTDetector>(DeSTDetLocation::IT);
  // Retrieve the raw bank decoder (for recreating the OTLiteTime)
  m_othitcreator = tool<Tf::IOTHitCreator>("Tf::OTHitCreator/OTHitCreator") ;
  m_momentumTool = tool<ITrackMomentumEstimate>( "FastMomentumEstimate" );
  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode PatSeedFit::finalize()
{
  debug() << "==> Finalize" << endmsg;


  return GaudiTool::finalize();  // must be called after all other actions
}

namespace PatSeedFitUtils {
    struct sortadapter {
	bool operator()(const PatFwdHit& h1, const PatFwdHit& h2) const
	{ return Tf::increasingByZ<PatForwardHit>()(&h1, &h2); }
    };
}

PatSeedTrack PatSeedFit::getTrackITOT(std::vector<PatFwdHit>& hits, int staIT) const
{
  boost::array<PatFwdHit*, 4> seedhits = { { 0, 0, 0, 0 } };
  if (staIT >= 0) {
    // we *know* that we have enough hits in staIT
    BOOST_FOREACH( PatFwdHit& ihit, hits ) {
      if (ihit.hit()->type() == Tf::RegionID::OT) continue;
      if (staIT != ihit.planeCode() / 4) continue;
      unsigned lay = ihit.planeCode() & 3;
      if (!seedhits[lay]) seedhits[lay] = &ihit;
    }
  } else {
    // fallback solution: two X, two stereo hits; prefer IT over OT
    BOOST_FOREACH( PatFwdHit& ihit, hits ) {
      if (ihit.hit()->type() == Tf::RegionID::OT) continue;
      unsigned lay = ihit.planeCode() & 3;
      if (!seedhits[lay]) seedhits[lay] = &ihit;
    }
    BOOST_FOREACH( PatFwdHit& ihit, hits ) {
      if (ihit.hit()->type() == Tf::RegionID::IT) continue;
      unsigned lay = ihit.planeCode() & 3;
      if (!seedhits[lay]) seedhits[lay] = &ihit;
    }
  }
  // need a fallback for really broken tracks with only u or only v hits
  // as well...
  if (!seedhits[0] || !seedhits[1] || !seedhits[2] || !seedhits[3]) {
    seedhits[0] = seedhits[1] = seedhits[2] = seedhits[3] = 0;
    BOOST_FOREACH( PatFwdHit& ihit, hits ) {
      bool isX = ihit.hit()->isX();
      if (isX) {
	if (!seedhits[0]) {
	  seedhits[0] = &ihit;
	} else {
	  if (!seedhits[3]) seedhits[3] = &ihit;
	}
      } else {
	if (!seedhits[1]) {
	  seedhits[1] = &ihit;
	} else {
	  if (!seedhits[2]) seedhits[2] = &ihit;
	}
      }
    }
  }

  // construct track
  PatSeedTrack tr(seedhits[0], seedhits[1], seedhits[2], seedhits[3],
      StateParameters::ZMidT, m_dRatio, m_initialArrow);
  tr.setValid(true);

  // save initial track parameters in case, internal fit fails later
  double z0, bx, ax, cx, dx, ay, by;
  tr.getParameters( z0, bx, ax, cx, dx, ay, by);

  const bool fitOK = !m_patSeedTool->refitStub(tr, m_dRatio, m_initialArrow);
  BOOST_FOREACH( PatFwdHit& ihit, hits ) {
      // if the hit is not on the track, add it
      if (seedhits.end() == 
	  std::find(seedhits.begin(), seedhits.end(), &ihit))
	  tr.addCoord( &ihit ) ;
  }
  if (!fitOK) {
    Warning("Stub fit failed, using best guess parameters", StatusCode::FAILURE, 0).ignore();
    // restore best guess of track parameters in case of fit failure
    tr.setParameters(z0, bx, ax, cx, dx, ay, by);
    tr.updateHits();
    tr.setValid(false);
    return tr;
  }
  tr.updateHits();
  // new best guess is result from stub fit
  tr.getParameters( z0, bx, ax, cx, dx, ay, by);

  // full refit
  if (!m_patSeedTool->fitTrack( tr, m_maxChi2, 5, false, false )) {
    Warning("First full fit failed, using best guess parameters", StatusCode::FAILURE, 0).ignore();
    // restore best guess of track parameters in case of fit failure
    tr.setParameters(z0, bx, ax, cx, dx, ay, by);
    tr.setValid(false);
  }
  tr.updateHits();
  return tr;
}

void PatSeedFit::makeCluster(std::vector<PatFwdHit>& hits, PatFwdHit& ihit,
		const PatFwdHit*& in, double& x, double& z, bool& isCluster) const
{
  BOOST_FOREACH( PatFwdHit& jhit, hits ) {
    if (!jhit.hit()->isX()) continue;
    if (ihit.planeCode() != jhit.planeCode()) continue;
    if (&ihit == &jhit) continue;
    if (std::abs(ihit.hit()->xAtYEq0() - jhit.hit()->xAtYEq0()) > 3. ||
	std::abs(ihit.hit()->xAtYEq0() - jhit.hit()->xAtYEq0()) < 2.)
      continue;
    // ok, have neighbouring hit
    const Tf::OTHit* otHit1 = ihit.hit()->othit();
    const Tf::OTHit* otHit2 = jhit.hit()->othit();
    // form a weighted sum of wire positions to determine the cluster
    // position (this degenerates to the arithmetic mean if we run without
    // drift time information)
    // to make sure that the weighting procedure does not diverge by accident,
    // we add the measurement variance to the squared drift radius, and take
    // the square of this expression to obtain the weights
    const double rr1 = otHit1->untruncatedDriftDistance(0.);
    const double r1 = std::sqrt(otHit1->variance() + rr1 * rr1);
    const double rr2 = otHit2->untruncatedDriftDistance(0.);
    const double r2 = std::sqrt(otHit2->variance() + rr2 * rr2);
    // the cluster position should be closer to the hit with the smaller
    // drift radius
    in = &jhit;
    x = (ihit.hit()->xAtYEq0() * r2 + jhit.hit()->xAtYEq0() * r1) / (r1 + r2);
    z = (ihit.hit()->zAtYEq0() * r2 + jhit.hit()->zAtYEq0() * r1) / (r1 + r2);
    isCluster = true;
    return;
  }
}

PatSeedTrack PatSeedFit::getTrackXY(std::vector<PatFwdHit>& hits) const
{
  boost::array<double, 3> x = { { 0., 0., 0. } }, z = { { 0., 0., 0. } };
  boost::array<const PatFwdHit*, 3> in = { { 0, 0, 0 } };
  boost::array<bool, 3> isCluster = { { false, false, false } };
  BOOST_FOREACH( PatFwdHit& ihit, hits ) {
    if (!ihit.hit()->isX()) continue;
    unsigned sta = ihit.planeCode() / 4;
    if (in[sta] && &ihit != in[sta]) {
      // if in[sta] is OT but ihit is IT, ihit takes precedence
      if (in[sta]->hit()->type() == Tf::RegionID::OT &&
	  ihit.hit()->type() == Tf::RegionID::IT) {
	in[sta] = &ihit;
	x[sta] = ihit.hit()->xAtYEq0();
	z[sta] = ihit.hit()->zAtYEq0();
	isCluster[sta] = false;
	continue;
      }
      // if both in[sta] and ihit are OT, but isCluster[sta] is false,
      // try to find a cluster starting with ihit
      if (in[sta]->hit()->type() == Tf::RegionID::OT &&
	  ihit.hit()->type() == Tf::RegionID::OT && !isCluster[sta]) {
	makeCluster(hits, ihit, in[sta], x[sta], z[sta], isCluster[sta]);
	continue;
      }
      // in all other cases, we stick to what we have
      continue;
    } else {
      // nothing there, just add hit
      in[sta] = &ihit;
      x[sta] = ihit.hit()->xAtYEq0();
      z[sta] = ihit.hit()->zAtYEq0();
      if (ihit.hit()->type() == Tf::RegionID::OT) {
	// if OT, look for cluster
	makeCluster(hits, ihit, in[sta], x[sta], z[sta], isCluster[sta]);
      }
    }
  }
  // this should never happen, but we make sure we have a fallback solution...
  if (!in[0] || !in[1] || !in[2])
    return getTrackITOT(hits, -1);
  PatSeedTrack tr(x[0], x[1], x[2], z[0], z[1], z[2],
      StateParameters::ZMidT, m_dRatio);
  tr.setValid(true);
  // add all the X hits
  BOOST_FOREACH( PatFwdHit& ihit, hits ) {
    if (!ihit.hit()->isX()) continue;
    tr.addCoord(&ihit);
  }
  tr.sort();
  // refit X only
  if (!m_patSeedTool->fitTrack( tr, m_maxChi2, 3, true, false )) {
    Warning("First x fit failed, falling back on emergency fit", StatusCode::FAILURE, 0).ignore();
    return getTrackITOT(hits, -1);
  }
  // add all the stereo hits and get rough estimate of y parameters
  double ty = 0.;
  unsigned nStereo = 0;
  BOOST_FOREACH( PatFwdHit& ihit, hits ) {
    if (ihit.hit()->isX()) continue;
    tr.addCoord(&ihit);
    const double x = ihit.hit()->xAtYEq0();
    const double z = ihit.hit()->zAtYEq0();
    ty += (tr.xAtZ(z)- x) / ihit.hit()->dxDy() / z;
    ++nStereo;
  }
  tr.setYParams(0., ty / double(nStereo));
  tr.sort();
  // refit stereo
  if (!m_patSeedTool->fitTrack( tr, m_maxChi2, 5, false, false )) {
    Warning("First full fit failed, falling back on emergency fit", StatusCode::FAILURE, 0).ignore();
    return getTrackITOT(hits, -1);
  }
  // all ok
  tr.updateHits();
  return tr;
}

/// fit a PatSeedTrack
StatusCode PatSeedFit::fitSeed( const std::vector<LHCb::LHCbID> lhcbIDs, 
				std::vector<LHCb::State> *states) const
{

  // these are the z-positions for the output states
  std::vector<double> m_zOutputs  =
    boost::assign::list_of(StateParameters::ZBegT)(StateParameters::ZMidT)(StateParameters::ZEndT);

  //
  LHCb::STLiteCluster::FastContainer*  m_stLiteContainer =
    get<LHCb::STLiteCluster::FastContainer>(LHCb::STLiteClusterLocation::ITClusters);

  // loop over LHCbIDs, extract T-station measurements
  std::vector< PatFwdHit > hits ;
  std::vector< Tf::STHit > sthits;
  std::vector< Tf::OTHit > othits;
  hits.reserve(lhcbIDs.size());
  sthits.reserve(lhcbIDs.size());
  othits.reserve(lhcbIDs.size());

  BOOST_FOREACH(LHCb::LHCbID hit, lhcbIDs) {
    if(  hit.detectorType()==LHCb::LHCbID::IT ) {
      LHCb::STChannelID stChan = hit.stID() ;
      const DeSTSector* stSector = m_itDet->findSector( stChan );
      if(stSector==0) {
          return Error( "No sector found for IT hit!" );
      }

      LHCb::STLiteCluster::FastContainer::const_iterator iclus =
          m_stLiteContainer->find< LHCb::STLiteCluster::findPolicy >( stChan ) ;

      if( iclus != m_stLiteContainer->end() ) {
          sthits.push_back( Tf::STHit(*stSector, *iclus ) ) ;
          hits.push_back( sthits.back() ) ;
      } else {
          return Error("Cannot find lite cluster!");
      }
    } else if( hit.detectorType()==LHCb::LHCbID::OT ) {
      othits.push_back( m_othitcreator->hit( hit.otID() ) ) ;
      hits.push_back( PatFwdHit(othits.back()) ) ;
    }
  }
  // create the PatSeedTrack
  if( hits.size() < 4 ) {
      info() << "Not enough SEED hits! "<< hits.size()  << endmsg ;
      return StatusCode::FAILURE;
  }
  std::sort(hits.begin(), hits.end(), PatSeedFitUtils::sortadapter());

  boost::array<unsigned, 12> planectr = { { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 } };
  boost::array<unsigned, 12> planectrIT = { { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 } };
  boost::array<unsigned, 12> planectrOT = { { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 } };
  unsigned numStereo = 0, n = 0;

  BOOST_FOREACH( PatFwdHit& ihit, hits ) {
      ihit.setSelected(true);
      ++planectr[ihit.planeCode()];
      if (ihit.hit()->type() == Tf::RegionID::OT)
	++planectrOT[ihit.planeCode()];
      else
	++planectrIT[ihit.planeCode()];

      if (ihit.hit()->isX()) ++n;
      else ++numStereo;
      // paranoia:
      // reset the coordinates of a hit to their values at y = 0
      // (once the true y is known, hit coordinates are shifted to their
      // correct position)
      ihit.setX(ihit.hit()->xAtYEq0());
      ihit.setZ(ihit.hit()->zAtYEq0());
      const Tf::OTHit* otHit = ihit.hit()->othit();
      if (otHit)
	ihit.setDriftDistance(otHit->untruncatedDriftDistance(0));
  }
  if ( n < 2 || numStereo < 2 ) {
      info() << "Not enough SEED hits! "<< hits.size() << ", " <<
	  n << " X hits, " << numStereo << " stereo hits" << endmsg ;
      return StatusCode::FAILURE;
  }
  // work out if we can do
  // a) normal 3 station x fit followed by adding stereo hits and a full fit
  // b) IT only stub inside a single station, adding remaining hits
  //    afterwards, followed by a full fit
  // c) quick and dirty recovery in case above does not work: like b) but using hits
  //    anywhere in T stations for stub formation
  bool has1XperSta = ((planectr[0] + planectr[3]) > 0) &&
    ((planectr[4] + planectr[7]) > 0) && ((planectr[8] + planectr[11]) > 0);
  int hasITOTSta = -1;
  if (planectrIT[0] && planectrIT[1] && planectrIT[2] && planectr[3])
    hasITOTSta = 0;
  if (planectrIT[4] && planectrIT[5] && planectrIT[6] && planectr[7])
    hasITOTSta = 1;
  if (planectrIT[8] && planectrIT[9] && planectrIT[10] && planectr[11])
    hasITOTSta = 2;
  // case c) above is identified by hasITOTSta == -1
  PatSeedTrack pattrack((has1XperSta ? getTrackXY(hits) :
	getTrackITOT(hits, hasITOTSta)));

  // save initial track parameters in case internal refit fails
  double z0, bx, ax, cx, dx, ay, by;
  pattrack.getParameters(z0, bx, ax, cx, dx, ay, by);

  // refit (unless we have a troublesome track)
  if (pattrack.valid() &&
      !m_patSeedTool->fitTrack(pattrack, m_maxChi2, 5, false, false)) {
    Warning("Second full fit failed, falling back on best guess parameters", StatusCode::FAILURE, 0).ignore();
    // fall back on old track parameters
    pattrack.setParameters(z0, bx, ax, cx, dx, ay, by);
  }

  LHCb::State temp(Gaudi::TrackVector(pattrack.xAtZ(m_zReference), 
	      pattrack.yAtZ(m_zReference),
	      pattrack.xSlope(m_zReference), 
	      pattrack.ySlope(m_zReference), 0.),
	  m_zReference, LHCb::State::AtT);

  double qOverP, sigmaQOverP;
  StatusCode sc = m_momentumTool->calculate(&temp, qOverP, sigmaQOverP, true) ;

  if (sc.isFailure()) {
    // if our momentum tool doesn't succeed, we have to try ourselves
    // to get q/p from curvature
    const double scaleFactor = m_magFieldSvc->signedRelativeCurrent();
    if (std::abs(scaleFactor) < 1e-4) {
      // return a track of 1 GeV, with a momentum uncertainty which makes it
      // compatible with a 1 MeV track
      qOverP = ((pattrack.curvature() < 0) ? -1. : 1.) *
	((scaleFactor < 0.) ? -1. : 1.) / Gaudi::Units::GeV;
      sigmaQOverP = 1. / Gaudi::Units::MeV;
    } else {
      // get qOverP from curvature
      qOverP = pattrack.curvature() * m_momentumScale / (-scaleFactor);
      sigmaQOverP = 0.5 * qOverP; // be conservative
    }
  }

  temp.setQOverP(qOverP);
  //== overestimated covariance matrix, as input to the Kalman fit
  Gaudi::TrackSymMatrix& cov = temp.covariance();
  cov(0,0) = m_stateErrorX2;
  cov(1,1) = m_stateErrorY2;
  cov(2,2) = m_stateErrorTX2;
  cov(3,3) = m_stateErrorTY2;
  cov(4,4) = sigmaQOverP * sigmaQOverP;

  BOOST_FOREACH( const double z, m_zOutputs ) {
      temp.setX(pattrack.xAtZ(z));
      temp.setY(pattrack.yAtZ(z));
      temp.setZ(z);
      temp.setTx(pattrack.xSlope(z));
      temp.setTy(pattrack.ySlope(z));
      states->push_back( temp );
  }

  return StatusCode::SUCCESS ;
}

/// fit a PatSeedTrack
StatusCode PatSeedFit::fitTrack( PatSeedTrack& track, double maxChi2,
		unsigned minPlanes, bool xOnly, bool forceDebug ) const
{
  if (m_patSeedTool->fitTrack(track, maxChi2, minPlanes, xOnly, forceDebug))
    return StatusCode::SUCCESS;
  else
    return StatusCode::FAILURE;
}

