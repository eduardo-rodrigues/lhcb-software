// $Id: TrackStateProvider.cpp,v 1.16 2010-04-07 21:08:22 wouter Exp $

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackInterpolator.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"

// from TrackKernel
#include "TrackKernel/TrackTraj.h"

/** @class TrackStateProvider TrackStateProvider.h
 *  
 *  A TrackStateProvider is a base class implementing methods
 *  from the ITrackStateProvider interface.
 *
 *  @author Wouter Hulsbergen
 *  @date   14/08/2010
 **/


// forward declarations
namespace {
  class TrackCache ;
}

class TrackStateProvider : public GaudiTool, 
			   virtual public ITrackStateProvider,
			   virtual public IIncidentListener {
public:

  /// Standard constructor
  TrackStateProvider( const std::string& type, 
		     const std::string& name,
		     const IInterface* parent ) ;
  
  /// Destructor
  virtual ~TrackStateProvider( );
  
  /// initialize
  StatusCode initialize() ;

  /// initialize
  StatusCode finalize() ;

  /// Compute the state of the track at position z.  The third
  /// argument is the tolerance: if an existing state is found within
  /// a z-distance 'tolerance', that state is returned.
  /// If there are 'fitnodes' on the track (e.g. in Brunel), this
  /// method will use interpolation. If there are no fitnodes (e.g. in
  /// DaVinci) the method will use extrapolation. In that case the
  /// answer is only correct outside the measurement range.
  virtual StatusCode state( LHCb::State& astate,
			    const LHCb::Track& track,
			    double z,
			    double ztolerance ) const ;

  ///
  virtual StatusCode stateFromTrajectory( LHCb::State& state,
					  const LHCb::Track& track, 
					  double z ) const {
    const LHCb::TrackTraj* traj = trajectory( track ) ;
    if( traj ) state = traj->state(z) ;
    return traj ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  }
  
  ///
  virtual const LHCb::TrackTraj* trajectory( const LHCb::Track& track ) const ;

  /// incident service handle
  virtual void handle( const Incident& incident ) ;

protected:
  const LHCb::State* addState( TrackCache& tc, double z) const ;
  TrackCache* cache( const LHCb::Track& track ) const ;

private :
  typedef std::map< size_t, ::TrackCache*> TrackCacheMap ;
  mutable TrackCacheMap m_trackcache ;
  ToolHandle< ITrackExtrapolator > m_extrapolator ;
  ToolHandle< ITrackInterpolator > m_interpolator ;
  bool m_applyMaterialCorrections ;
};



/**********************************************************************************************/



namespace {
  
  inline bool compareStateZ(const LHCb::State* lhs, const LHCb::State* rhs)
  {
    return lhs->z() < rhs->z() ;
  }
  
  inline bool equalStateZ(const LHCb::State* lhs, const LHCb::State* rhs)
  {
    return std::abs(lhs->z() - rhs->z()) < TrackParameters::propagationTolerance ;
  }
  
  // The TrackCache is basically just an 'extendable' TrackTraj
  class TrackCache : public LHCb::TrackTraj {
  private:
    const LHCb::Track* m_track ;
    std::vector<LHCb::State*> m_ownedstates ;
    double m_zFirstMeasurement ;
  public:
    TrackCache( const LHCb::Track& track ) 
      :  LHCb::TrackTraj(track), m_track(&track)
    {
      const LHCb::State* state = track.stateAt( LHCb::State::FirstMeasurement ) ;
      m_zFirstMeasurement = state ? state->z() : -9999 ;
    }

    ~TrackCache() {
      for( std::vector<LHCb::State*>::const_iterator it = m_ownedstates.begin() ;
	   it != m_ownedstates.end() ; ++it) delete *it ;
    }

    /// get the track
    const LHCb::Track& track() const { return *m_track; }

    /// because the pointer to the track is not good enough, for paranoia we compute a sort of uniqueID
    static size_t trackID(const LHCb::Track& track) { 
      return size_t(&track) + track.lhcbIDs().front().lhcbID() + track.lhcbIDs().back().lhcbID() ; }
    
    /// return z position of state at first measurement
    double zFirstMeasurement() const { return m_zFirstMeasurement ; }

    /// return the closest state to a certain z-position
    const LHCb::State* closestState(double z) const ;

    // insert a new state in the container with states
    void insertState( StateContainer::iterator pos, LHCb::State* state ) ;

    // return the states (work around keyword protected)
    StateContainer& states() { return refStates() ; }
    
  } ;

  /// return the closest ref state to a certain z-position
  const LHCb::State* TrackCache::closestState( double z ) const 
  {
    // we use 'min_element' even though this does not exploit that
    // states are sorted.  the nicest would be to use
    // lowerbound. 
    const StateContainer& refstates = refStates() ;
    static LHCb::State tmpstate ;
    tmpstate.setZ( z ) ;
    StateContainer::const_iterator it = 
      std::lower_bound( refstates.begin(),refstates.end(),&tmpstate,compareStateZ ) ;
    if( it == refstates.end() ) --it ;
    else if( it != refstates.begin() ) {
      StateContainer::const_iterator prev = it ; --prev ;
      if( z - (*prev)->z() < (*it)->z() - z ) it = prev ;
      assert( z - (*prev)->z()>=0) ;
      assert( (*it)->z() - z>=0) ;
    }
    
    //StateContainer::const_iterator it =
    //std::min_element( refStates().begin(),refStates().end(),
    //                  TrackFunctor::distanceAlongZ<LHCb::State>(z) );
    return *it;
  }

  /// Add a state
  void TrackCache::insertState( StateContainer::iterator pos, LHCb::State* state ) {

    // take ownership of state
    m_ownedstates.push_back(state) ;

    // get the vector with states
    StateContainer refstates = refStates() ;

    // temporary logic test
    if( refstates.size()>0) {
      if(pos == refstates.end() ) {
	assert( refstates.back()->z() < state->z() ) ;
      } else if ( pos == refstates.begin() ) {
	assert( refstates.front()->z() > state->z() ) ;
      } else {
	StateContainer::iterator prev = pos ; --prev ;
	assert( (*prev)->z() < state->z() && state->z() < (*pos)->z() ) ;
      }
    }
    
    // insert the state
    refstates.insert(pos, state) ;
    
    // update the range of the trajectory
    Trajectory::setRange( refstates.front()->z(), refstates.back()->z() ) ;
    
    // invalidate the cache
    TrackTraj::invalidateCache() ;
  }

  double closestToBeamLine( const LHCb::State& state )
  {
    const Gaudi::TrackVector& vec = state.stateVector();
    double z = state.z();
    // check on division by zero (track parallel to beam line!)
    if ( vec[2] != 0 || vec[3] != 0 ) {
      z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
    } 
    // don't go outside the sensible volume
    return std::min(std::max(z,-100*Gaudi::Units::cm), StateParameters::ZBegRich2 ) ;
  }
}

DECLARE_TOOL_FACTORY( TrackStateProvider );

//=============================================================================
// TrackStateProvider constructor.
//=============================================================================
TrackStateProvider::TrackStateProvider( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : GaudiTool(type, name, parent),
    m_extrapolator("TrackMasterExtrapolator",this),
    m_interpolator("TrackInterpolator",this)
{ 
  declareInterface<ITrackStateProvider>(this);
  declareProperty("Extrapolator",m_extrapolator) ;
  declareProperty("Interpolator",m_interpolator) ;
  declareProperty("ApplyMaterialCorrections",m_applyMaterialCorrections) ;
}

//=============================================================================
// TrackStateProvider destructor.
//=============================================================================
TrackStateProvider::~TrackStateProvider() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackStateProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  // retrieve extrapolator
  if(sc.isSuccess()) sc = m_extrapolator.retrieve() ;
  if(sc.isSuccess()) sc = m_interpolator.retrieve() ;
  // reset pointer to list of clusters at beginevent
  incSvc()->addListener(this, IncidentType::BeginEvent);
  
  return sc ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackStateProvider::finalize()
{
  m_extrapolator.release().ignore() ;
  m_interpolator.release().ignore() ;
  return GaudiTool::finalize();
}


//=============================================================================
// Incident handle
//=============================================================================
void TrackStateProvider::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_trackcache.clear() ;
}

//=============================================================================
// get a state at a particular z position, within given tolerance
//=============================================================================

StatusCode TrackStateProvider::state( LHCb::State& thestate,
				      const LHCb::Track& track, 
				      double z,
				      double ztolerance ) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  TrackCache* tc = cache( track ) ;
  const LHCb::State* state = tc->closestState( z ) ;
  if( std::abs( z - state->z() ) > ztolerance ) {
    state = addState( *tc, z ) ;
    if( state == 0 ) {
      sc = StatusCode::FAILURE ; 
    } else {
      thestate = *state ;
    }
  } else {
    thestate = *state ;
  }
  return sc ;
}

//=============================================================================
// add a state to the cache of a given track
//=============================================================================

const LHCb::State* TrackStateProvider::addState( TrackCache& tc, double z) const
{
  LHCb::State* state = new LHCb::State( LHCb::State::Vertex ) ;
  state->setZ( z ) ;
  LHCb::TrackTraj::StateContainer& refstates = tc.states() ;
  LHCb::TrackTraj::StateContainer::iterator it = 
    std::lower_bound( refstates.begin(),refstates.end(),state, compareStateZ ) ;
  // in brunel, we simply use the interpolator. in davinci, we use the
  // extrapolator, and we take some control over material corrections.
  const LHCb::Track& track = tc.track() ;
    StatusCode sc ;
  if( track.fitResult() && !track.fitResult()->nodes().empty() && track.fitStatus()==LHCb::Track::Fitted) {
    sc = m_interpolator->interpolate( track, z, *state ) ;
  } else {
    bool applyMaterialCorrections = false ;
    if( it == refstates.end() ) {
      *state = *refstates.back() ;
    } else if( it == refstates.begin() || z < tc.zFirstMeasurement() ) {
      *state = **it ;
      applyMaterialCorrections = m_applyMaterialCorrections ;
    } else {
      // take the closest state. forget
      LHCb::TrackTraj::StateContainer::iterator prev = it ;
      --prev ;
      *state = std::abs( (**it).z() - z ) < std::abs( (**prev).z() - z ) ? **it : **prev ;
    }
    state->setLocation( LHCb::State::Vertex ) ;
    if( applyMaterialCorrections ) {
      sc = m_extrapolator->propagate( *state, z ) ;
    } else {
      LHCb::StateVector statevec( state->stateVector(), state->z() ) ;
      Gaudi::TrackMatrix transmat ;
      sc = m_extrapolator->propagate( statevec, z, &transmat ) ;
      state->setState( statevec ) ;
      state->setCovariance( ROOT::Math::Similarity<double,Gaudi::TrackMatrix::kRows,Gaudi::TrackMatrix::kCols>
			    ( transmat, state->covariance() ) );
    }
  }

  if( sc.isFailure() ) {
    delete state ;
    state = 0 ;
  } else {
    refstates.insert( it, state ) ;    
  }
  return state ;
}

//=============================================================================
// retrieve the cache for a given track
//=============================================================================

TrackCache* TrackStateProvider::cache( const LHCb::Track& track ) const
{
  TrackCache* tc(0) ;
  size_t key = TrackCache::trackID(track) ;
  TrackCacheMap::iterator it = m_trackcache.find( key ) ;
  if( it == m_trackcache.end() ) {
    tc = new TrackCache( track ) ;
    m_trackcache[key] = tc ;
    // make sure Downstream tracks get assigned a state at the beamline
    if( (track.hasVelo() || track.hasTT() ) &&
	!track.firstState().location() == LHCb::State::ClosestToBeam &&
	track.stateAt( LHCb::State::ClosestToBeam ) == 0 ) {
      // compute poca of first state with z-axis
      double z = closestToBeamLine( track.firstState() ) ;
      addState( *tc, z ) ;
    }
  } else {
    tc = it->second ;
  }
  
  return tc ;
}

//=============================================================================
// retrieve a trajectory for a given track (from the cache)
//=============================================================================
const LHCb::TrackTraj* TrackStateProvider::trajectory( const LHCb::Track& track ) const
{
  const TrackCache* tc = cache( track ) ;
  return tc ? static_cast<const LHCb::TrackTraj*>(tc) : 0 ;
}
