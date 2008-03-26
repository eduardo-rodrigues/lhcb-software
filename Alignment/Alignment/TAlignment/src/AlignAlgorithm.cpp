// $Id: AlignAlgorithm.cpp,v 1.39 2008-03-26 10:51:41 wouter Exp $
// Include files
// from std
// #include <utility>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiUtils/Aida2ROOT.h"

// from root
#include "TH1D.h"

// from DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/GeometryInfoPlus.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from BOOST
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"

// local
#include "AlignAlgorithm.h"
#include "AlParameters.h"
#include "AlignSolvTools/AlEquations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {

 Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
   Gaudi::Vector6 v;
   for (int i = 0; i < 6; ++i) v(i) = m(0,i);
   return v;
 }

 class Data {
 public:
   Data(unsigned nodeindex, unsigned index, double r, const Gaudi::Matrix1x6& d)
     : m_nodeindex(nodeindex),
       m_index(index),
       m_r(r),
       m_d(d) {}
   unsigned index() const { return m_index; }
   double r() const { return m_r; }
   const Gaudi::Matrix1x6& d() const { return m_d; }
   size_t nodeindex() const { return m_nodeindex ; }
 private:
   size_t            m_nodeindex;
   unsigned          m_index;
   double            m_r;
   Gaudi::Matrix1x6  m_d;
 };
 
 //FIXME: the next two stand-alone functions should move into AlVec & AlSymMat...
 template <typename T>
 void ass(AlVec& lhs, unsigned row, const T& rhs) {
   for (unsigned i = 0; i < T::kSize; ++i) lhs[row+i] = rhs(i);
 }
 
 template <typename T>
 void ass(AlSymMat& lhs, unsigned col, unsigned row, const T& rhs) {
   for (unsigned i = 0; i < T::kCols; ++i)
     for (unsigned j = 0; j < T::kRows; ++j)
       lhs[col+i][row+j] = rhs(i,j);
 }
 
 void fillHisto1D(IHistogram1D* histo1D, const size_t& bin, const double& value, const double& error) {
   /// Get pointer to root histo
   TH1D* rHisto1D = Gaudi::Utils::Aida2ROOT::aida2root(histo1D);
   rHisto1D->SetBinContent(bin, value); rHisto1D->SetBinError(bin, error);
 }
 
   
};

using namespace LHCb;
using namespace boost;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm );

AlignAlgorithm::AlignAlgorithm( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_iteration(0u),
    m_nIterations(0u),
    m_nTracks(0u),
    m_covFailure(0u),
    m_rangeElements(),
    m_initAlignConstants(),
    m_align(0),
    m_projSelector(0),
    m_matrixSolverTool(0),
    m_trackresidualtool("Al::TrackResidualTool"),
    m_vertexresidualtool("Al::VertexResidualTool"),
    m_equations(0)
{
  declareProperty("NumberOfIterations"          , m_nIterations                  = 10u                     );
  declareProperty("TracksLocation"              , m_tracksLocation               = TrackLocation::Default  );
  declareProperty("VertexLocation"              , m_vertexLocation               = ""  );
  declareProperty("ProjectorSelector"           , m_projSelectorName             = "TrackProjectorSelector");
  declareProperty("MatrixSolverTool"            , m_matrixSolverToolName         = "SpmInvTool"            );
  declareProperty("UseCorrelations"             , m_correlation                  = true                    );
  declareProperty("UpdateInFinalize"            , m_updateInFinalize             = false                   );
  declareProperty("CanonicalConstraintStrategy" , m_canonicalConstraintStrategy  = CanonicalConstraintAuto ); 
  declareProperty("ConstrainZShearings"         , m_constrainZShearings          = false                   );
  declareProperty("UseWeightedAverageConstraint", m_useWeightedAverageConstraint = false                   );
  declareProperty("MinNumberOfHits"             , m_minNumberOfHits              = 1u                      ); 
  declareProperty("FillCorrelationsHistos"      , m_fillCorrelationHistos        = false                   );
  declareProperty("UsePreconditioning"          , m_usePreconditioning           = false                   );
}

AlignAlgorithm::~AlignAlgorithm() {}


StatusCode AlignAlgorithm::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set histogram path
  if ("" == histoTopDir()) setHistoTopDir("Alignment/");

  /// Get pointer to incident service and add listener
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  incSvc->addListener(this, "UpdateConstants");

  /// Get tool to align detector
  m_align = tool<IGetElementsToBeAligned>("GetElementsToBeAligned", this);
  if (!m_align) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);

  sc = m_trackresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_vertexresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  /// Get range  detector elements
  m_rangeElements = m_align->rangeElements();
  
  if (printDebug()) {
    debug() << "==> Got " << m_rangeElements.size() << " elements to align!" << endmsg;
    for(ElementRange::const_iterator i = m_rangeElements.begin(); i!= m_rangeElements.end(); ++i) {
      const AlDofMask& ownDoFMask = i->dofMask();
      debug() <<  "        " << (*i) << endmsg;
      const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");
      debug() << "DOFs: ";
      for (AlDofMask::const_iterator j = ownDoFMask.begin(), jEnd = ownDoFMask.end(); j != jEnd; ++j) {
        if ((*j)) info() << dofs.at(std::distance(ownDoFMask.begin(), j));
      }
      debug() << endmsg;
    }
  }
  
  m_equations = new Al::Equations(m_rangeElements.size());
  m_equations->clear() ;

  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);

  ///Get matrix solver tool
  m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
  if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

  /// Monitoring
  /// Book residual histograms
  /// Residuals
  /// @todo: this should go into a monitoring tool
  info() << "booking histograms assuming " << m_nIterations << " iterations " << endmsg;
  m_trackChi2Histo           = book2D(10, "Track chi2 distribution vs iteration",
				      -0.5, m_nIterations-0.5, m_nIterations, -1.00, 
                                      +100.00, 100);
  m_trackNorChi2Histo        = book2D(11, "Normalised track chi2 distribution vs iteration", 
                                      -0.5, m_nIterations-0.5, m_nIterations, -1.00, 
                                      +100.00, 100);
  m_totNusedTracksvsIterHisto= bookProfile1D(20, "Total number of used tracks for alignment vs iteration" , -0.5, m_nIterations-0.5, m_nIterations);
  m_totChi2vsIterHisto       = bookProfile1D(30, "Total sum of track chi2 vs iteration"            , -0.5, m_nIterations-0.5, m_nIterations);
  m_avgChi2vsIterHisto       = bookProfile1D(31, "Average sum of track chi2 vs iteration"          , -0.5, m_nIterations-0.5, m_nIterations);
  m_norTotChi2vsIterHisto    = bookProfile1D(32, "Normalised total sum of track chi2 vs iteration" , -0.5, m_nIterations-0.5, m_nIterations);
  m_dAlignChi2vsIterHisto    = bookProfile1D(40, "Delta Alignment chi2 vs iteration"               , -0.5, m_nIterations-0.5, m_nIterations);
  m_nordAlignChi2vsIterHisto = bookProfile1D(41, "Delta Alignment normalised chi2 vs iteration"    , -0.5, m_nIterations-0.5, m_nIterations);

  for(ElementRange::const_iterator i = m_rangeElements.begin(); i!= m_rangeElements.end(); ++i) {
    const unsigned    index_i = i->index();
    const std::string name   = i->name();
    if (printDebug()) debug() << "Booking histograms for element " << name << " with index " << index_i <<endmsg;
    m_resHistos[index_i]     = book2D(1000u+index_i, "Residual vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations,
                                      -1.00 , +1.00, 100);
    m_pullHistos[index_i]    = book2D(2000u+index_i, "Pull vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations,
                                      -5.00 , +5.00, 100);
    m_nHitsHistos[index_i]   = book1D(3000u+index_i, "Number of hits vs iteration for " + name, 
                                      -0.5, m_nIterations-0.5, m_nIterations);

    m_deltaTxHisto[index_i]  = book1D(10000u+index_i, "Delta Tx vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    m_deltaTyHisto[index_i]  = book1D(20000u+index_i, "Delta Ty vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    m_deltaTzHisto[index_i]  = book1D(30000u+index_i, "Delta Tz vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    m_deltaRxHisto[index_i]  = book1D(40000u+index_i, "Delta Rx vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    m_deltaRyHisto[index_i]  = book1D(51000u+index_i, "Delta Ry vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    m_deltaRzHisto[index_i]  = book1D(60000u+index_i, "Delta Rz vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations);
    
    if ( m_fillCorrelationHistos ) {
      m_autoCorrHistos[index_i] = book2D(4000u+index_i, "hit autocorrelation in " + name + "  vs iteration ",
                                         -0.5, m_nIterations-0.5, m_nIterations,
                                         -0.5, +0.5, 250);
      unsigned(index_j) = index_i; 
      for (ElementRange::const_iterator j = i; j != m_rangeElements.end(); ++j, ++index_j) {
	m_corrHistos[std::make_pair(index_i, index_j)] = book2D((10000 + index_i) * (100 + index_j),
                                                                "hit correlation of " + name + " with " + j->name()
                                                                + " vs iteration "
                                                                + ( i == j ? "(excluding autocorrelations!)" : "" ),
                                                                -0.5, m_nIterations-0.5, m_nIterations,
                                                                -1.0, +1.0, 250);
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::finalize() {
  if (m_updateInFinalize) update() ;
  return  
    m_trackresidualtool.release() && 
    m_vertexresidualtool.release() && 
    GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Get tracks. Copy them into a vector, because that's what we need for dealing with vertices.
  const Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printVerbose()) verbose() << "Number of tracks in container " + m_tracksLocation + ": " << tracks->size() << endmsg;
  std::vector<const LHCb::Track*> selectedtracks ;
  for( Tracks::const_iterator iTrack = tracks->begin(), iTrackEnd = tracks->end(); iTrack != iTrackEnd; ++iTrack) 
    // just a sanity check
    if( (*iTrack)->fitStatus()==LHCb::Track::Fitted &&
	(*iTrack)->nDoF() > 0 &&
	!(*iTrack)->nodes().empty() ) {
      selectedtracks.push_back( *iTrack ) ;
    } else {
      warning() << "Skipping bad track:"
		<< " fitstatus = " << (*iTrack)->fitStatus()
		<< " nDoF = " << (*iTrack)->nDoF()
		<< " #nodes = " << (*iTrack)->nodes().size() << endreq ;
    }
  m_nTracks += selectedtracks.size();

  // Now deal with vertices, if there are any.
  size_t numusedvertices(0) ;
  if( !m_vertexLocation.empty() ) {
    const LHCb::RecVertices* vertices = get<LHCb::RecVertices>(m_vertexLocation);
    if(vertices ) {
      for( LHCb::RecVertices::const_iterator ivertex = vertices->begin() ;
	   ivertex != vertices->end(); ++ivertex ) {
	// used tracks are automatically removed from the output list
	const Al::MultiTrackResiduals* res = m_vertexresidualtool->get(**ivertex,selectedtracks) ;
	if (res ) {
	  accumulate( *res ) ;
	  // need some histogramming here
	  ++numusedvertices ;
	} else {
	  warning() << "Error computing residual cov matrix for vertex." << endmsg;
	  ++m_covFailure;
	}
      }
    }
  }	
  
  // Loop over the remaining tracks
  info() << "Number of tracks left after processing vertices: " << selectedtracks.size() << endreq ;
  for( std::vector<const LHCb::Track*>::const_iterator iTrack = selectedtracks.begin() ;
       iTrack != selectedtracks.end(); ++iTrack ) {
    
    const Al::Residuals* res = m_trackresidualtool->get(**iTrack) ;
    if( res ) {
      accumulate( *res ) ;
      m_trackChi2Histo->fill(m_iteration, (*iTrack)->chi2());
      m_trackNorChi2Histo->fill(m_iteration, (*iTrack)->chi2PerDoF());
    } else {
      warning() << "Error computing residual cov matrix. Skipping track of type "
		<< (*iTrack)->type() << " with key: " << (*iTrack)->key() << " and chi2 / dof: " << (*iTrack)->chi2() << "/" << (*iTrack)->nDoF() << endmsg ;
      ++m_covFailure;
    }
  } 

  m_equations->addEventSummary( selectedtracks.size(), numusedvertices ) ;

  



  return StatusCode::SUCCESS;
}

void AlignAlgorithm::accumulate( const Al::Residuals& residuals )
{
  if (printVerbose()) verbose() << "==> Found " << residuals.nodes().size() << " nodes"<< endmsg;
  std::vector<Data> data;
  // Loop over nodes and get measurements, residuals and errors
  typedef Al::Residuals::NodeContainer::const_iterator NodeIter;
  size_t numexternalhits(0) ;
  size_t nodeindex(0) ;
  for (Al::Residuals::NodeContainer::const_iterator node = residuals.nodes().begin(); 
       node != residuals.nodes().end();++node, ++nodeindex) {
    assert( (*node)->hasMeasurement() ) ;

    // Get measurement
    const Measurement& meas = (*node)->measurement();
    // Get element that belongs to this measurment
    const AlignmentElement* elem = m_align->findElement(meas);
    if (!elem) {
      if (printVerbose()) verbose() << "==> Measurement not on a to-be-aligned DetElem " 
				    << meas.lhcbID() << endmsg;
      ++numexternalhits ;
      continue;
    }
      
    const unsigned index = elem->index();
    if (printVerbose()) verbose() << "==> measure = " << meas.measure() << " id = " << meas.lhcbID() << " -> index = " 
				  << index << " -> " <<  elem->name() << endmsg;
    
    // Project measurement
    const ITrackProjector* proj = m_projSelector->projector(meas);
    if (!proj) {
      error() << "==> Could not get projector for selected measurement!" << endmsg;
      continue;
    }
    double res  = residuals.r(nodeindex) ;
    double err  = std::sqrt(residuals.V(nodeindex)) ;
    
    m_nHitsHistos[index]->fill(m_iteration);
    m_resHistos[index]->fill(m_iteration, res);
    m_pullHistos[index]->fill(m_iteration, res/(*node)->errResidual());
    // Get alignment derivatives
    LHCb::StateVector state((*node)->state().stateVector(),(*node)->state().z());
    // the projector calculates the derivatives in the global
    // frame. The jacobian corrects for the transformation to the
    // alignment frame.
    Derivatives der =  
      proj->alignmentDerivatives(state,meas,Gaudi::XYZPoint(0,0,0)) *
      elem->jacobian() ;
    
    // push back normalized residuals & derivatives;
    res /= err;
    der /= err;
    data.push_back(Data(nodeindex, index, res, der));
    m_equations->addHitSummary( index, err ) ;
  }
  
  if (!data.empty()) {
    for (std::vector<Data>::const_iterator id = data.begin(), idEnd = data.end(); id != idEnd; ++id) {
      m_equations->V(id->index())               -= convert(id->r()*id->d()) ;
      m_equations->M(id->index(), id->index())  += (Transpose(id->d())*id->d());
      
      for (std::vector<Data>::const_iterator jd = data.begin(); jd != idEnd; ++jd) {
	if ( id == jd || ( m_correlation && id->index() <= jd->index() )) {
	  double c = residuals.HCH_norm( id->nodeindex(), jd->nodeindex() ) ;
	  m_equations->M(id->index(), jd->index()) -= c * (Transpose(id->d())*jd->d());
	  
	  if ( m_fillCorrelationHistos ) {
	    if (!( id->nodeindex() == jd->nodeindex())) {
	      m_corrHistos[std::make_pair(id->index(), jd->index())]->
		fill(m_iteration, c/std::sqrt(residuals.HCH_norm(id->nodeindex(),id->nodeindex()) *
					      residuals.HCH_norm(jd->nodeindex(),jd->nodeindex())) ) ;
	    } else {
	      m_autoCorrHistos[id->index()]->fill(m_iteration, c);
	    }
	  }
	}
      }
    }
    // keep some information about the tracks that we have seen
    m_equations->addChi2Summary( residuals.chi2(), residuals.nDoF(), numexternalhits ) ;
  }
}



void AlignAlgorithm::preCondition(AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
				  AlVec& scale, const std::vector<int>& offsets) const
{  
  // This is just not sufficiently fool proof!
  size_t size = halfDChi2DAlpha.size() ;
  scale.reSize(size) ;
  for (size_t i = 0u, iEnd = size; i< iEnd; ++i) scale[i] = 1 ;
  int iElem(0u) ;
  for (ElementRange::const_iterator it = m_rangeElements.begin(), itEnd = m_rangeElements.end(); it != itEnd; ++it, ++iElem) {
    if ( offsets[iElem]>=0 ) {
      size_t ndof = (*it).dofMask().nActive() ;
      size_t N = m_equations->numHits(iElem) ;
      for (size_t i = offsets[iElem]; i< offsets[iElem]+ndof; ++i) {
        assert( i < size ) ;
        if ( halfD2Chi2DAlpha2[i][i] > 0 ) scale[i] = std::sqrt( N / halfD2Chi2DAlpha2[i][i] ) ;
      }
    }
  }
  
  for (size_t i = 0u, iEnd = size; i < iEnd; ++i) {
    halfDChi2DAlpha[i] *= scale[i] ;
    for (size_t j = 0u; j <= i; ++j) halfD2Chi2DAlpha2[i][j] *= scale[i] * scale[j] ;
  }
}

void AlignAlgorithm::postCondition(AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
				   const AlVec& scale) const
{ 
  size_t size = halfDChi2DAlpha.size() ;
  for (size_t i = 0u, iEnd = size; i < iEnd; ++i) {
    halfDChi2DAlpha[i] *= scale[i] ;
    for (size_t j = 0u; j<=i; ++j) halfD2Chi2DAlpha2[i][j] *= scale[i] * scale[j] ;
  }
}

size_t AlignAlgorithm::addCanonicalConstraints(AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
					       std::vector<bool>& dofmask,
					       std::ostream& logmessage) const
{
  // This adds lagrange multipliers to constrain the average rotation
  // and translation. Ideally, we could calculate this in any
  // frame. In practise, the average depends on the reference frame in
  // which is is calculated. We will calculate a single 'pivot' point
  // to define the transform to the frame in which we apply the
  // constraint.
  //
  info() << "Adding canonical constraints." << endmsg ;
  double weight(0) ;
  Gaudi::XYZVector pivot ;
  double zmin(9999999), zmax(-999999) ;
  size_t iElem(0u) ;
  for (ElementRange::const_iterator it = m_rangeElements.begin(), itEnd = m_rangeElements.end(); it != itEnd; ++it, ++iElem) {
    double thisweight = m_equations->weight(iElem) ;
    weight += thisweight ;
    pivot += thisweight * Gaudi::XYZVector( it->centerOfGravity() ) ;
    zmin = std::min(it->centerOfGravity().z(),zmin) ;
    zmax = std::min(it->centerOfGravity().z(),zmax) ;
  }
  if (weight>0) pivot *= 1/weight ;
  Gaudi::Transform3D canonicalframe( pivot ) ;
  Gaudi::Transform3D canonicalframeInv = canonicalframe.Inverse() ;
  info() << "Pivot for canonical constraints: " << pivot << endmsg ;

  // add extra rows/columns
  size_t size = halfDChi2DAlpha.size() ;
  size_t numConstraints = 6u;
  if (m_constrainZShearings) numConstraints += 3u;
  
  halfDChi2DAlpha.reSize(size + numConstraints) ;
  halfD2Chi2DAlpha2.reSize(size + numConstraints) ;
  // Set all new elements to 0
  for (size_t i = size; i < size+numConstraints; ++i) {
    halfDChi2DAlpha[i] = 0 ;
    for (size_t j = 0u; j <= i; ++j) halfD2Chi2DAlpha2[i][j] = 0 ;
  }
  
  // Now fill the only part that is nonzero: halfD2Chi2/DAlphadLambda. 
  iElem = 0u ;
  for (ElementRange::const_iterator it = m_rangeElements.begin(), itEnd = m_rangeElements.end(); it != itEnd; ++it, ++iElem) {
    // calculate the Jacobian for going from the 'alignment' frame to
    // the 'canonical' frame. This is the first place where we could
    // turn things around. It certainly works if the transforms are
    // just translations.
    Gaudi::Transform3D trans = canonicalframeInv * it->alignmentFrame() ;
    Gaudi::Matrix6x6 jacobian = AlParameters::jacobian( trans ) ;
    double thisweight = m_useWeightedAverageConstraint ? m_equations->weight(iElem)/weight : 1./double( m_equations->nElem() ) ;
    thisweight = 1.0/double( m_equations->nElem() ) ;
    for (size_t i = 0u; i < 6; ++i) {
      for (size_t j = 0; j < Derivatives::kCols; ++j) {
	// and here comes the 2nd place we could do things entirely
	// wrong, but I think that this is right.
	halfD2Chi2DAlpha2[size+i][j+iElem*Derivatives::kCols] = thisweight * jacobian(i,j) ;
      }
    }
    if (m_constrainZShearings) {
      double deltaZ = it->centerOfGravity().z() - pivot.z() ;
      // the 3 constraints are in this order: zx-shearing, zy-shearing and z-scale ('zz-shearing')
      for (size_t i = 0u; i < 3u; ++i) {
        for (size_t j = 0u; j < Derivatives::kCols; ++j) {
          halfD2Chi2DAlpha2[size+i+6][j+iElem*Derivatives::kCols] = thisweight * deltaZ/(zmax-zmin) * jacobian(i,j) ;
        }
      }
    }
  }
  
  if (printDebug()) debug() << "Full matrix after adding constraints: " << std::endl
                            << halfD2Chi2DAlpha2 << endmsg ;
  
  // we have now calculated everything for all constraints. However,
  // we may not want all of them:
  // * remove constraints that have no active non-zero derivative 
  // * remove constraints if the dof is effectively constrained by inactive parameters
  // this is pretty tricky. need to replace the following with something more sensible:
  const std::vector<std::string> constraintnames = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz")("Szx")("Szy")("Szz");
  const double threshold = FLT_MIN ;
  size_t numRemovedConstraints(0) ;
  for (size_t i = 0u; i < numConstraints; ++i) {
    bool hasNonZeroDerivativeToActive(false) ;
    bool hasNonZeroDerivativeToInactive(false) ;
    for (size_t j = 0u; j < size && !(hasNonZeroDerivativeToActive && hasNonZeroDerivativeToInactive); ++j) {
      // check that derivative is non-zero
      if ( std::abs(halfD2Chi2DAlpha2[size+i][j]) > threshold ) {
	if ( dofmask[j] ) hasNonZeroDerivativeToActive = true ;
	// the logic fails because rotations cannot 'constrain' translations. need to think of something else.
	// if( !dofmask[j] ) hasNonZeroDerivativeToInactive = true ;
      }
    }
    bool useConstrained = hasNonZeroDerivativeToActive && !hasNonZeroDerivativeToInactive ;
    dofmask.push_back( useConstrained ) ;
    numRemovedConstraints += useConstrained ? 0 : 1 ;
  }

  // Finally, add some info to the log message
  assert( dofmask.size() == size + numConstraints) ;
  logmessage << "Added canonical constraints for (global) : " ;
  for (size_t i = 0u; i < numConstraints; ++i) if ( dofmask[size+i] ) logmessage << constraintnames[i] << (i != numConstraints-1u ? ", " : "" );
  logmessage << std::endl ;

  return numConstraints - numRemovedConstraints ;
}


void AlignAlgorithm::printCanonicalConstraints(const AlVec& parameters, const AlSymMat& covariance,
					       size_t numConstraints, std::ostream& logmessage) const
{
  // first extract the part concerning the lagrange multipliers
  size_t N = parameters.size() ;
  AlVec lambda(numConstraints);
  AlSymMat covlambda(numConstraints) ;
  for (size_t i = 0u; i < numConstraints; ++i) {
    lambda[i] = parameters[N - numConstraints + i] ;
    for (size_t j = 0u; j <= i; ++j)
      covlambda[i][j] = covariance[N - numConstraints + i][N - numConstraints + j] ;
  }
  // For a lagrage constraint defined as delta-chisqyare = 2 * lambda * constraint, the solution for lambda is
  //  lambda     =   W * constraint
  //  cov-lambda =   W
  // where W is minus the inverse of the covariance of the constraint. From this we extract
  //   constraint       = W^{-1} * lambda
  //   constraint error = sqrt( W^{-1} )
  //   chisquare = - lambda * constraint
  // Note that this all needs to work for vectors.

  AlSymMat V = -1 * covlambda ; // Al$%$%^&$%&&*&^
  int ierr ;
  V.invert(ierr) ;
  AlVec x = (V * lambda) ; // This might need a minus sign ...
  double chisquare = (lambda * V * lambda) ;
  logmessage << "Canonical constraint values: " << std::endl ;
  for (size_t i = 0u; i < numConstraints; ++i) logmessage << std::setw(5) << i << std::setw(12) << x[i] << " +/- " << std::sqrt( V[i][i] ) << std::endl ;
  logmessage << "Canonical constraint chisquare: " << chisquare << std::endl ;
}

static inline double signedroot(double root)
{
  return root > 0 ? std::sqrt(root) : ( root < 0  ? - std::sqrt(-root) : (std::isfinite(root) ? 0 : root ) ) ;
}


//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() {

  if ( m_equations->nElem() == 0 ) {
    warning() << "==> No elements to align." << endmsg ;
    return ;
  }
  
  info() << "\n";
  info() << "==> iteration " << m_iteration << " : Initial alignment conditions  : [";
  std::vector<double> deltas;
  deltas.reserve(m_rangeElements.size()*6u);  
  getAlignmentConstants(m_rangeElements, deltas);
  for (std::vector<double>::const_iterator i = deltas.begin(), iEnd = deltas.end(); i != iEnd; ++i) {
    info() << (*i) << (i != iEnd-1u ? ", " : "]");
  }
  info() << endmsg;

  info() << "==> Updating constants" << endmsg ;
  std::ostringstream logmessage ;
  logmessage << "********************* ALIGNMENT LOG ************************" << std::endl
	     << "Iteration: " << m_iteration << std::endl
	     << "Total number of tracks: " << m_nTracks << std::endl
	     << "Number of covariance calculation failures: " << m_covFailure << std::endl
	     << "Used " << m_equations->numVertices() << " vertices for alignment" << std::endl
             << "Used " << m_equations->numTracks() << " tracks for alignment" << std::endl
	     << "Total track chisquare/dofs: " << m_equations->totalChiSquare() << " / " << m_equations->totalNumDofs() << std::endl
	     << "Average track chisquare: " << m_equations->totalChiSquare() / m_equations->numTracks() << std::endl
             << "Normalised total track chisquare: " << m_equations->totalChiSquare() / m_equations->totalNumDofs() << std::endl
	     << "Used " << m_equations->numHits() << " hits for alignment" << std::endl
             << "Total number of hits in external detectors: " << m_equations->numExternalHits() << std::endl;

  m_totNusedTracksvsIterHisto->fill(m_iteration, m_equations->numTracks());
  m_totChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare());
  m_avgChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare() / m_equations->numTracks());
  m_norTotChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare() / m_equations->totalNumDofs());

  if (printDebug()) {
    for (size_t i = 0; i < m_equations->nElem(); ++i) {
      for (size_t j = i; j < m_equations->nElem(); ++j) {
	debug() << "==> M["<<i<<","<<j<<"] = "      << m_equations->M(i,j) << endmsg;
      }
      debug() << "\n==> V["<<i<<"] = "    << m_equations->V(i) << endmsg;
    }
  }
    
  /// Take the gaudi vector and matix and fill AlVec and AlSym for ALL elements and ALL parameters
  size_t numParameters = Derivatives::kCols * m_equations->nElem() ;
  AlVec    tmpDerivatives(numParameters);
  AlSymMat tmpMatrix(numParameters);

  /// Loop over map of index to 2nd derivative matrix and 1st derivative vector
  for (unsigned i = 0u, iEnd = m_equations->nElem(); i < iEnd ; ++i) {
    ass(tmpDerivatives, i*Derivatives::kCols, m_equations->V(i));
    /// (assume upper triangular input!)
    for (unsigned j = i ; j < iEnd; ++j ) ass(tmpMatrix, i*Derivatives::kCols, j*Derivatives::kCols, m_equations->M(i,j));
  }

  // Now reduce the large system to the active parameters

  // Create the dof mask and a map from AlignableElements to an
  // offset. The offsets are initialized with '-1', which signals 'not
  // enough hits'.
  size_t dim(0) ;
  size_t iElem(0) ;
  std::vector<int>  offsets(m_rangeElements.size(), -1) ;
  std::vector<bool> dofmask(tmpDerivatives.size(), true) ;
  for (ElementRange::const_iterator it = m_rangeElements.begin(), itEnd = m_rangeElements.end(); it != itEnd; ++it, ++iElem) {
    if (m_equations->numHits(iElem) >= m_minNumberOfHits) {
      offsets[iElem] = dim ;
      for (size_t ipar = 0u; ipar < Derivatives::kCols; ++ipar) dofmask[iElem*Derivatives::kCols+ipar] = it->dofMask()[ipar] ;
      dim += it->dofMask().nActive() ;
    } else {
      for (size_t ipar = 0u; ipar < Derivatives::kCols; ++ipar) dofmask[iElem*Derivatives::kCols+ipar] = false ;
    }
  }
  
  // Add the canonical constraints, if needed
  size_t numConstraints(0) ;
  if ( m_canonicalConstraintStrategy == CanonicalConstraintOn ||
       (m_canonicalConstraintStrategy == CanonicalConstraintAuto && m_equations->numExternalHits() == 0u) ) {
    numConstraints = addCanonicalConstraints(tmpDerivatives, tmpMatrix, dofmask, logmessage) ;
    assert(dofmask.size() == tmpDerivatives.size() ) ;
  }
  
  size_t nDOFs = std::count(dofmask.begin(), dofmask.end(), true) ;
  
  logmessage << "Number of alignables with insufficient statistics: " << std::count(offsets.begin(),offsets.end(),-1) << std::endl
	     << "Number of constraints: "                             << numConstraints << std::endl
	     << "Number of active parameters: "                       << nDOFs-numConstraints << std::endl ;
  
  if ( nDOFs > 0 ) {
    AlVec    halfDChi2dX(nDOFs);
    AlSymMat halfD2Chi2dX2(nDOFs);
    
    // Remove all parameters that are masked out.
    unsigned insert_i = 0u;
    unsigned insert_j = 0u;
    typedef std::vector<bool>::const_iterator boolIter; 
    for (std::vector<bool>::const_iterator i = dofmask.begin(), iEnd = dofmask.end(); i != iEnd; ++i) {
      if (*i) {
	const unsigned index_i = std::distance(boolIter(dofmask.begin()), i);
	halfDChi2dX[insert_i] = tmpDerivatives[index_i];
	for (std::vector<bool>::const_iterator j = i, jEnd = dofmask.end(); j != jEnd; ++j) {
	  if (*j) {
	    if (printVerbose()) {
	      verbose() << "insert_i = " << insert_i << " insert_j = " << insert_j 
			<< " halfD2Chi2dX2[" << index_i << "][" << std::distance(boolIter(dofmask.begin()), j) << "]" 
			<< tmpMatrix[index_i][std::distance(boolIter(dofmask.begin()), j)] << endmsg;
	    }
	    halfD2Chi2dX2[insert_i][insert_j++] = tmpMatrix[index_i][std::distance(boolIter(dofmask.begin()), j)];
	  }
	}
	insert_j = ++insert_i;
      }
    }
    
    if (printDebug()) {
      debug() << "Tmp AlVec Vector    = " << tmpDerivatives << endmsg;
      debug() << "Tmp AlSymMat Matrix = " << tmpMatrix      << endmsg;
    }
    
    if (nDOFs < 50 ) {
      info() << "AlVec Vector    = " << halfDChi2dX << endmsg;
      info() << "AlSymMat Matrix = " << halfD2Chi2dX2      << endmsg;
    } else {
      info() << "Matrices too big to dump to std" << endmsg ;
    }
    
    // Tool returns H^-1 and alignment constants. Need to copy the 2nd derivative because we still need it.
    AlVec    scale(halfD2Chi2dX2.size()) ;
    AlSymMat covmatrix = halfD2Chi2dX2 ;
    AlVec    solution  = halfDChi2dX ;
    if (m_usePreconditioning) preCondition(solution,covmatrix, scale,offsets) ;
    bool solved = m_matrixSolverTool->compute(covmatrix, solution);
    if (m_usePreconditioning) postCondition(solution,covmatrix, scale) ;
    
    if (solved) {
      StatusCode sc = StatusCode::SUCCESS;
      double deltaChi2 = solution * halfDChi2dX ; //somewhere we have been a bit sloppy with two minus signs!
      if (printDebug()) {
	info() << "Solution = " << solution << endmsg ;
	info() << "Covariance = " << covmatrix << endmsg ;
      }
      logmessage << "Alignment change chisquare/dof: " 
		 << deltaChi2 << " / " << nDOFs-numConstraints << std::endl
                 << "Normalised alignment change chisquare: " << deltaChi2 / (nDOFs-numConstraints) << std::endl;
      
      m_dAlignChi2vsIterHisto->fill(m_iteration, deltaChi2) ;
      m_nordAlignChi2vsIterHisto->fill(m_iteration, deltaChi2 / (nDOFs-numConstraints));
      
      if (numConstraints > 0) printCanonicalConstraints(solution, covmatrix, numConstraints, logmessage) ;
      
      if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
      size_t iElem(0u) ;
      for (ElementRange::iterator it = m_rangeElements.begin(); it != m_rangeElements.end(); ++it, ++iElem) {
	logmessage << "Alignable: " << it->name() << std::endl
		   << "Number of hits seen: " << m_equations->numHits(iElem) << std::endl ;
	if( offsets[iElem] < 0 ) {
	  logmessage << "Not enough hits for alignment. Skipping update." << std::endl ;
	} else {
	  AlParameters delta( solution, covmatrix, halfD2Chi2dX2, it->dofMask(), offsets[iElem] ) ;
	  AlParameters refdelta = it->currentActiveDelta() ;
	  //logmessage << delta ;
	  for(unsigned int iactive = 0u; iactive < delta.dim(); ++iactive) 
	    logmessage << std::setw(6)  << delta.activeParName(iactive) << ":" 
		       << " cur= " << std::setw(12) << refdelta.parameters()[iactive]
		       << " delta= " << std::setw(12) << delta.parameters()[iactive] << " +/- "
		       << std::setw(12) << signedroot(delta.covariance()[iactive][iactive]) 
		       << " gcc= " << delta.globalCorrelationCoefficient(iactive) << std::endl ;
	  
	  // need const_cast because loki range givess access only to const values 
	  StatusCode sc = (const_cast<AlignmentElement&>(*it)).updateGeometry(delta) ;
	  if (!sc.isSuccess()) error() << "Failed to set alignment condition for " << it->name() << endmsg ; 
          fillHisto1D(m_deltaTxHisto[it->index()], m_iteration+1u, delta.translation()[0], delta.errTranslation()[0]);
          fillHisto1D(m_deltaTyHisto[it->index()], m_iteration+1u, delta.translation()[1], delta.errTranslation()[1]);
          fillHisto1D(m_deltaTzHisto[it->index()], m_iteration+1u, delta.translation()[2], delta.errTranslation()[2]);
          fillHisto1D(m_deltaRxHisto[it->index()], m_iteration+1u, delta.rotation()[0]   , delta.errRotation()[0]);
          fillHisto1D(m_deltaRyHisto[it->index()], m_iteration+1u, delta.rotation()[1]   , delta.errRotation()[1]);
          fillHisto1D(m_deltaRzHisto[it->index()], m_iteration+1u, delta.rotation()[2]   , delta.errRotation()[2]);
        }
      }
    } else {
      error() << "Failed to solve system" << endmsg ;
    }
  } else {
    logmessage << "No alignable degrees of freedom ... skipping solver tools and update." << std::endl ;
  }
  
  logmessage << "********************* END OF ALIGNMENT LOG ************************" ;
  info() << logmessage.str() << endmsg ;
  info() << "\n";
  info() << "==> iteration " << m_iteration << " : Updated alignment conditions  : [";
  deltas.clear();
  getAlignmentConstants(m_rangeElements, deltas);
  for (std::vector<double>::const_iterator i = deltas.begin(), iEnd = deltas.end(); i != iEnd; ++i) {
    info() << (*i) << (i != iEnd-1u ? ", " : "]");
  }
  info() << endmsg;
}

void AlignAlgorithm::reset() {
  if (printDebug()) debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  // set counters to zero
  m_nTracks = 0u;
  m_covFailure = 0u;
  // clear derivatives and H maps
  m_equations->clear();
}

StatusCode AlignAlgorithm::queryInterface(const InterfaceID& id, void** ppI) {
  // check the placeholder:
  if (0 == ppI) return StatusCode::FAILURE;
  // check ID of the interface
  if (IIncidentListener::interfaceID() != id) return Algorithm::queryInterface(id, ppI);
  *ppI = static_cast<IIncidentListener*>(this);
  addRef();
  return StatusCode::SUCCESS;
}

void AlignAlgorithm::handle(const Incident& incident) {
  if ("UpdateConstants" == incident.type()) {
    update();
    reset();
  }
}

void AlignAlgorithm::getAlignmentConstants(const ElementRange& rangeElements, AlignConstants& alignConstants) const {
  alignConstants.reserve(6*rangeElements.size()); /// 6 constants * num elements
  for (ElementRange::const_iterator i = rangeElements.begin(); i != rangeElements.end(); ++i) {
    if (printDebug()) debug() << "Getting alignment constants for " << (*i) << endmsg;
    /// Get translations and rotations
    const std::vector<double>& translations = i->deltaTranslations();
    const std::vector<double>& rotations    = i->deltaRotations();
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin()   , rotations.end()   );
  }
}
