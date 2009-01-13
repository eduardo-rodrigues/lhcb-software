#include "IAlignConstraintTool.h"
#include "IGetElementsToBeAligned.h"
#include "GaudiAlg/GaudiTool.h"
#include "SolvKernel/AlMat.h"
#include "GaudiKernel/ToolFactory.h"
#include "LHCbMath/LHCbMath.h"
#include "boost/tokenizer.hpp"
#include "GaudiKernel/SystemOfUnits.h"

/* helper class */

namespace Al
{
  class ConstraintDerivatives
  {
  public:
    enum EConstraintDerivatives { Tx=0, Ty, Tz, Rx, Ry, Rz, Szx, Szy, Szz, Sxx, SRz, Trx, Try, Trtx, Trty, Cur, Sz2x, Sz2y, NumConstraints } ;
    ConstraintDerivatives(size_t dim, const std::vector<std::string>& activeconstraints, const std::string& name="", int offset=0) ;
    AlMat& derivatives() { return m_derivatives ; }
    const AlMat& derivatives() const { return m_derivatives ; }

    static std::vector<std::string> all()  { return m_names ; }
    std::string name(size_t i) const { return m_nameprefix + m_names[i] ; }
    static size_t index(const std::string& aname) {
      std::vector<std::string>::const_iterator it = std::find( m_names.begin(), m_names.end(), aname) ;
      return size_t( it - m_names.begin() ) ;
      //return std::distance( names.begin(),it) ; // doesn't exist for random access iterator ?!
    }
    static bool check( const std::vector<std::string>& names ) ;

    bool isActive(size_t ipar) const {  return m_dofmask.isActive(ipar) ;}
    void setActive(size_t ipar, bool b=true) {  return m_dofmask.setActive(ipar,b) ;}
    size_t nActive() const { return m_dofmask.nActive() ;}
    int activeParIndex(size_t ipar) const { return m_dofmask.activeParIndex(ipar) ; }
    std::string activeParName(size_t iactive) const { return name(m_dofmask.parIndex(iactive)) ; }
    int activeParOffset() const { return m_activeParOffset ; }
    void setActiveParOffset(int offset) { m_activeParOffset = offset ; }
    const AlDofMask& dofMask() const { return m_dofmask ; }
  private:
    AlDofMask m_dofmask ;
    AlMat m_derivatives ;
    AlVec m_residuals ;
    std::string m_nameprefix ;
    static const std::vector<std::string> m_names ;
    int m_activeParOffset;  
  } ;

  bool ConstraintDerivatives::check( const std::vector<std::string>& names ) 
  {
    bool success(true) ;
    for( std::vector<std::string>::const_iterator iname = names.begin();
	 iname != names.end() && success; ++iname) 
      if ( !(success = std::find( m_names.begin(), m_names.end(), *iname) != m_names.end()) ) 
	std::cout << "Warning: unknown constraint " << *iname << std::endl ;
    return success ;
  }
  
  ConstraintDerivatives::ConstraintDerivatives(size_t dim, const std::vector<std::string>& activeconstraints, 
			   const std::string& nameprefix, int offset) 
    : m_dofmask(size_t(NumConstraints)),m_derivatives(NumConstraints,dim), m_residuals(NumConstraints), 
      m_nameprefix(nameprefix),m_activeParOffset(offset)
  {
    std::vector<bool> active(NumConstraints,false) ;
    if( check(activeconstraints) ) 
      for( std::vector<std::string>::const_iterator iname = activeconstraints.begin();
	   iname != activeconstraints.end(); ++iname) 
	active[index(*iname)] = true ;
    m_dofmask = AlDofMask(active) ;
  }
  
  const std::vector<std::string> ConstraintDerivatives::m_names 
  = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz")("Szx")("Szy")("Szz")("Sxx")("SRz")
    ("Trx")("Try")("Trtx")("Trty")("Trcur")("Sz2x")("Sz2y");
  

  class AlignConstraintTool : public GaudiTool,  
			      virtual public IAlignConstraintTool
  {
  public:
    typedef std::vector<const AlignmentElement*> ElementPointers ;
    typedef std::vector<Al::ConstraintDerivatives*> DerivativeContainer ;
    AlignConstraintTool(const std::string& type,
			const std::string& name,
			const IInterface* parent) ;
    ~AlignConstraintTool() ;
    StatusCode initialize() ;

    size_t addConstraints(const Elements& elements,
			  const Al::Equations& equations,
			  AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const ;
    
    void printConstraints( const AlVec& parameters, const AlSymMat& covariance,
			   std::ostream& logmessage) const ;
  private:
    void clearConstraintDerivatives() const ;
    Al::ConstraintDerivatives* createConstraintDerivatives(const std::string& name,
						 const std::vector<std::string>& activeconstraints,
						 const ElementPointers& elements,
						 const Al::Equations& equations, size_t numAlignPars) const ;

    void addConstraintToNominal(const Elements& elements,
				AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const ;
  private:
    class ConstraintDefinition
    {
    public:
      ConstraintDefinition(const std::string& name) : m_name(name) {}
      const std::string& name() const { return m_name ; }
      const std::vector<std::string>& dofs() const { return m_dofs ; }
      const ElementPointers& elements() const { return m_elements ; }
      void addDof(const std::string& dof) { m_dofs.push_back(dof) ; }
      void addElements(const ElementPointers& elements) { m_elements.insert( m_elements.end(), elements.begin(), elements.end() ) ; }
    private:
      std::string m_name ;
      std::vector<std::string> m_dofs ;
      ElementPointers m_elements ;
    } ;
    
  private:
    std::vector<std::string> m_constraintNames ;
    std::vector<ConstraintDefinition> m_definitions ;
    mutable DerivativeContainer m_derivatives ;
    bool m_useWeightedAverage ;
    bool m_constrainToNominal ;
    double m_sigmaNominalTx ;
    double m_sigmaNominalTy ;
    double m_sigmaNominalTz ;
    double m_sigmaNominalRx ;
    double m_sigmaNominalRy ;
    double m_sigmaNominalRz ;
    const IGetElementsToBeAligned* m_elementtool ;
  } ;
  
  DECLARE_TOOL_FACTORY( AlignConstraintTool );
  
  AlignConstraintTool::AlignConstraintTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
    : GaudiTool(type,name,parent),
      m_derivatives(0)
  {
    // interfaces
    declareInterface<IAlignConstraintTool>(this);
    declareProperty("Constraints", m_constraintNames ) ;
    declareProperty("UseWeightedAverage", m_useWeightedAverage = false ) ;
    declareProperty("ConstrainToNominal", m_constrainToNominal = false ) ;
    declareProperty("SigmaNominalTx",m_sigmaNominalTx = 1*Gaudi::Units::mm) ;
    declareProperty("SigmaNominalTy",m_sigmaNominalTy = 1*Gaudi::Units::mm) ;
    declareProperty("SigmaNominalTz",m_sigmaNominalTz = 1*Gaudi::Units::mm) ;
    declareProperty("SigmaNominalRx",m_sigmaNominalRx = 0.01 ) ;
    declareProperty("SigmaNominalRy",m_sigmaNominalRy = 0.01 ) ;
    declareProperty("SigmaNominalRz",m_sigmaNominalRz = 0.01 ) ;
  }

  AlignConstraintTool::~AlignConstraintTool()
  {
    clearConstraintDerivatives() ;
  }

#include <string>

/* If 'from' matches 'to' or 'from' is empty, */
/* does not parse 's', returns std::string::npos */
/* */
/* Otherwise returns number of replacements done */
/* */

  static std::string removechars(const std::string& s,
				 const std::string& chars)
  {
    std::string rc ;
    for( std::string::const_iterator it = s.begin() ;
	 it != s.end(); ++it)
      if( std::find(chars.begin(), chars.end(), *it)==chars.end() )
	rc.push_back(*it);
    return rc ;
  }

  static std::vector<std::string> tokenize(const std::string& s,
					   const char* separator)
  {
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    Tokenizer split(s, Separator(separator));
    std::vector<std::string> rc ;
    rc.insert(rc.end(), split.begin(), split.end() ) ;
    return rc ;
  }
  
  StatusCode AlignConstraintTool::initialize()
  {
    // check that constraints exist
    StatusCode sc = GaudiTool::initialize() ;
    
    m_elementtool = tool<IGetElementsToBeAligned>("GetElementsToBeAligned");

    if( sc.isSuccess() ) {
      // now we need to decode the string with constraints
      ConstraintDefinition common("") ;
      ElementPointers elements ;
      for( Elements::const_iterator ielem=m_elementtool->rangeElements().begin() ;
	   ielem != m_elementtool->rangeElements().end(); ++ielem)
	elements.push_back(&(*ielem)) ;
      common.addElements( elements ) ;
      m_definitions.push_back( common ) ; 
      for( std::vector<std::string>::const_iterator ic = m_constraintNames.begin() ;
	   ic != m_constraintNames.end(); ++ic ) {
	// tokenize
	std::vector<std::string> tokens = tokenize(*ic,":") ;
	// 	std::cout << "string: " << *ic << std::endl
	// 		  << "number of tokens: " << tokens.size() << std::endl ;
	if( tokens.size() == 1 ) {
	  // this is a single or a set of common constraints
	  std::vector<std::string> dofs = tokenize(*ic," ,") ;
	  for( std::vector<std::string>::const_iterator idof = dofs.begin(); idof != dofs.end(); ++idof)
	    m_definitions.front().addDof( removechars(*idof," ,") ) ;
	} else if( tokens.size() == 3 ) {
	  ConstraintDefinition newconstraint( tokens.at(0) ) ;
	  ElementPointers elements ;
	  sc = m_elementtool->findElements( removechars(tokens.at(1)," ,"), elements ) ;
	  newconstraint.addElements( elements ) ;
	  std::vector<std::string> dofs = tokenize(tokens.at(2)," ,") ;
	  for( std::vector<std::string>::const_iterator idof = dofs.begin(); idof != dofs.end(); ++idof)
	    newconstraint.addDof( removechars(*idof," ,") ) ;
	  m_definitions.push_back( newconstraint ) ;
	}
      }
      
      int nactive(0) ;
      for( std::vector<ConstraintDefinition>::const_iterator ic =  m_definitions.begin() ;
	   ic != m_definitions.end(); ++ic ) {
	if( !ConstraintDerivatives::check(ic->dofs()) ) {
	  error() << "Constraint list contains unknown constraints! "  << endreq ;
	  sc = StatusCode::FAILURE ;
	} else {
	  nactive += ic->dofs().size() ;
	  info() << "Constraint definition: " << ic->name() 
		 << " " << "dofs = " ;
	  for( std::vector<std::string>::const_iterator it = ic->dofs().begin() ;
	       it != ic->dofs().end(); ++it )
	    info() << *it << " , " ;
	  info() << "num elements = " << ic->elements().size() << endmsg ;
	}
      }
      info() << "Number of constraint definitions= " 
	     << m_definitions.size() 
	     << ",  number of active constraints = " << nactive << endreq ;
    }
    return sc ;
  }
  
  Al::ConstraintDerivatives* AlignConstraintTool::createConstraintDerivatives(const std::string& name,
									      const std::vector<std::string>& activeconstraints,
									      const ElementPointers& elements,
									      const Al::Equations& equations,
									      size_t numAlignPars) const
  {
    // This adds lagrange multipliers to constrain the average rotation
    // and translation. Ideally, we could calculate this in any
    // frame. In practise, the average depends on the reference frame in
    // which is is calculated. We will calculate a single 'pivot' point
    // to define the transform to the frame in which we apply the
    // constraint.
    
    // To make the bookkeeping easy, we add all possible constraints and
    // then 'disable' those we don't need with the 'dofmask'.
    // 
    // This is the numbering:
    // 0 -> Tx
    // 1 -> Ty
    // 2 -> Tz
    // 3 -> Rx
    // 4 -> Ry
    // 5 -> Rz
    // 6 -> Sx  (zx shearing)
    // 7 -> Sy  (zy shearing)
    // 8 -> Sz  (zz shearing == z-scale)
    // 9 -> SRz (twist around z-axis)
    // 10 -> Average track X
    // 11 -> Average track Y
    // 12 -> Average track Tx
    // 13 -> Average track Ty
    // 14 -> Curvature (Curvature constraint)
    
    // it should be possible to do this better
    double weight(0) ;
    size_t numhits(0) ;
    Gaudi::XYZVector pivot ;
    double zmin(9999999), zmax(-999999), xmin(9999999), xmax(-999999) ;
    for (ElementPointers::const_iterator it = elements.begin(); it !=elements.end() ; ++it) 
      if((*it)->activeParOffset() >= 0) {
	size_t elemindex = (*it)->index() ;
	double thisweight = m_useWeightedAverage ? equations.element(elemindex).weightV() : 1 ;
	weight += thisweight ;
	Gaudi::XYZPoint cog = (*it)->centerOfGravity() ;
	pivot += thisweight * Gaudi::XYZVector( cog ) ;
	zmin = std::min(cog.z(),zmin) ;
	zmax = std::max(cog.z(),zmax) ;
	xmin = std::min(cog.x(),xmin) ;
	xmax = std::max(cog.x(),xmax) ;
	numhits += equations.element(elemindex).numHits() ;
      }
      
    if (weight>0) pivot *= 1/weight ;
    Gaudi::Transform3D canonicalframe( pivot ) ;
    Gaudi::Transform3D canonicalframeInv = canonicalframe.Inverse() ;
    info() << "Pivot, z/x-range for canonical constraints: " 
	   << pivot << ", [" << zmin << "," << zmax << "]" 
	   << ", [" << xmin << "," << xmax << "]" << endmsg ;

    // create the object that we will return
    Al::ConstraintDerivatives* constraints = new Al::ConstraintDerivatives(numAlignPars,activeconstraints,name) ;
 
    for (ElementPointers::const_iterator it = elements.begin(); it !=elements.end() ; ++it) 
      if((*it)->activeParOffset() >= 0) {
	// calculate the Jacobian for going from the 'alignment' frame to
	// the 'canonical' frame. This is the first place where we could
	// turn things around. It certainly works if the transforms are
	// just translations.
	size_t elemindex = (*it)->index() ;
	Gaudi::Transform3D trans = canonicalframeInv * (*it)->alignmentFrame() ;
	Gaudi::Matrix6x6 jacobian = AlParameters::jacobian( trans ) ;
	double thisweight = (m_useWeightedAverage ? equations.element(elemindex).weightV() : 1.0 )/weight ;
	double deltaZ = (*it)->centerOfGravity().z() - 0.5*(zmax+zmin) ;
	double deltaX = (*it)->centerOfGravity().x() - 0.5*(xmax+xmin) ;
	
	// loop over all parameters in this element. skip inactive parameters.
	for (size_t j=0 ; j<6; ++j) {
	  int jpar = (*it)->activeParIndex(j) ;
	  if(jpar>=0) {
	    // Derivatives for global rotations and translations
	    for (size_t i = 0u; i < 6; ++i) 
	      // and here comes the 2nd place we could do things entirely
	      // wrong, but I think that this is right. if( it->activeParOffset() >= 0 ) { // only take selected elements
	      constraints->derivatives()(i,jpar) = thisweight * jacobian(i,j) ;
	    
	    if(zmax > zmin) {
	      double zweight = deltaZ/(zmax-zmin) ;
	      // Derivatives for shearings
	      for (size_t i = 0u; i < 3u; ++i) 
		constraints->derivatives()(i+6,jpar) = thisweight * zweight * jacobian(i,j) ;
	      
	      // Derivatives for twist around z-axis
	      constraints->derivatives()(ConstraintDerivatives::SRz,jpar) = thisweight * zweight * jacobian(5,j) ;

	      // Derivatives for bowing
	      for (size_t i = 0u; i < 2u; ++i) 
		constraints->derivatives()(ConstraintDerivatives::Sz2x+i,jpar) = thisweight * zweight*zweight * jacobian(i,j) ;
	      
	    }

	    if( xmax > xmin ) {
	      // Derivative for scaling in x
	      double xweight = deltaX/(xmax-xmin) ;
	      constraints->derivatives()(ConstraintDerivatives::Sxx,jpar) = thisweight * xweight * jacobian(0,j) ;
	    }
	    
	    // Curvature constraint. The constraint is on the average per track.
	    for(size_t trkpar=0; trkpar<5; ++trkpar) 
	      constraints->derivatives()(ConstraintDerivatives::Trx+trkpar,jpar) 
		= equations.element(elemindex).dStateDAlpha()(trkpar,j)/equations.numTracks() ;
	  }
	} 
      }
    
    // turn off constraints with only zero derivatives
    for(size_t ipar = 0; ipar<ConstraintDerivatives::NumConstraints; ++ipar ) 
      if( constraints->isActive(ipar) ) {
	bool hasNonZero(false) ;
	for (size_t jpar = 0; jpar <numAlignPars && !hasNonZero; ++jpar)
	  hasNonZero = std::abs(constraints->derivatives()(ipar,jpar)) > LHCb::Math::lowTolerance ;
	if( !hasNonZero ) {
	  warning() << "removing constraint \'" <<  constraints->name(ipar) << "\' because it has no non-zero derivatives." << endreq ;
	  constraints->setActive(ipar,false) ;
	}
      }
    return constraints ;
  }
  
  void AlignConstraintTool::clearConstraintDerivatives() const
  {
    for( std::vector<Al::ConstraintDerivatives*>::iterator ic = m_derivatives.begin() ;
	 ic != m_derivatives.end() ; ++ic) delete *ic ;
    m_derivatives.clear() ;
  }

  size_t AlignConstraintTool::addConstraints(const Elements& elements,
					     const Al::Equations& equations,
					     AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const
  {
    // get the total number of alignment parameters
    size_t numpars = halfDChi2DAlpha.size() ;

    // fill the constraint derivatives
    clearConstraintDerivatives() ;
    size_t numactive(0) ;
    for( std::vector<ConstraintDefinition>::const_iterator idef = m_definitions.begin() ;
	 idef != m_definitions.end(); ++idef) {
      ConstraintDerivatives* derivative = 
	createConstraintDerivatives(idef->name(),idef->dofs(),idef->elements(),
				    equations,numpars) ;
      derivative->setActiveParOffset(numpars + numactive) ;
      m_derivatives.push_back(derivative) ;
      numactive += derivative->nActive() ;
    }
    info() << "Total number of active constraints: " << numactive << endreq ;
    
    // now add them, if any constraints are active
    if( numactive > 0 ) {
      
      // create new matrices
      size_t dim = numpars + numactive ;
      AlVec halfDChi2DAlphaNew( dim ) ;
      AlSymMat halfD2Chi2DAlpha2New( dim ) ;
      
      // copy the old matrices in there
      for(size_t irow=0; irow<numpars; ++irow) {
	halfDChi2DAlphaNew(irow) = halfDChi2DAlpha(irow) ;
	for(size_t icol=0; icol<=irow; ++icol) 
	  halfD2Chi2DAlpha2New.fast(irow,icol) = halfD2Chi2DAlpha2.fast(irow,icol) ;
      }
      
      // now add the constraints, only to the 2nd derivative (residuals are zero for now)
      for( DerivativeContainer::const_iterator ic = m_derivatives.begin() ;
	   ic != m_derivatives.end(); ++ic ) 
	if( (*ic)->nActive()>0 ) {
	  for(size_t irow=0; irow<Al::ConstraintDerivatives::NumConstraints; ++irow) {
	    int iactive = (*ic)->activeParIndex(irow) ;
	    if( 0 <= iactive ) {
	      for(size_t jrow=0; jrow<numpars; ++jrow)
		halfD2Chi2DAlpha2New.fast( (*ic)->activeParOffset() + iactive , jrow ) = 
		  (*ic)->derivatives()(irow,jrow) ;
	    }
	  }
	}
      
      // copy the result back
      halfDChi2DAlpha = halfDChi2DAlphaNew ;
      halfD2Chi2DAlpha2 = halfD2Chi2DAlpha2New ;
    }
    
    // eventually, add also the chisquare constraints
    if( m_constrainToNominal ) 
      addConstraintToNominal(elements,halfDChi2DAlpha,halfD2Chi2DAlpha2) ;

    return numactive ;
  }

  void AlignConstraintTool::printConstraints(const AlVec& parameters, const AlSymMat& covariance,
					     std::ostream& logmessage) const
  {
    if( !m_derivatives.empty() ) {
      // the total number of alignment parameters
      size_t numPars = m_derivatives.front()->activeParOffset() ;
      size_t numConstraintDerivatives = parameters.size() - numPars ;
    
      // ugly: create a map from an index to an alignment parameter in a constraints object
      AlDofMask totalmask ;
      for( DerivativeContainer::const_iterator ic = m_derivatives.begin() ;
	   ic != m_derivatives.end(); ++ic ) 
	totalmask.push_back( (*ic)->dofMask() ) ;

      // first extract the part concerning the lagrange multipliers
      AlVec lambda(numConstraintDerivatives);
      AlSymMat covlambda(numConstraintDerivatives) ;
      for (size_t i = 0u; i < numConstraintDerivatives; ++i) {
	lambda[i] = parameters[numPars + i] ;
	for (size_t j = 0u; j <= i; ++j)
	  covlambda[i][j] = covariance[numPars + i][numPars + j] ;
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
      for (size_t iactive = 0u; iactive < numConstraintDerivatives; ++iactive) {
	size_t parindex = totalmask.parIndex(iactive) ;
	logmessage << std::setw(5) 
		   << m_derivatives[parindex/Al::ConstraintDerivatives::NumConstraints]->name(parindex%Al::ConstraintDerivatives::NumConstraints)
		   << std::setw(12) << x(iactive)
		   << " +/- " << AlParameters::signedSqrt(V.fast(iactive,iactive)) << std::endl ;
      }
      logmessage << "Canonical constraint chisquare: " << chisquare << std::endl ;
        
      // Now the inactive constraints. Shrink the parameter and covariance matrix to the parameters only part
      AlVec subparameters(numPars) ;
      AlSymMat subcovariance(numPars) ;
      for(size_t irow=0; irow<numPars; ++irow) {
	subparameters[irow] = parameters[irow] ;
	for(size_t icol=0; icol<=irow; ++icol) 
	  subcovariance.fast(irow,icol) = covariance.fast(irow,icol) ;
      }
      // Calculate the delta
      AlVec constraintdelta  = m_derivatives.front()->derivatives() * subparameters ;
      AlMat constraintcov = m_derivatives.front()->derivatives() * subcovariance * m_derivatives.front()->derivatives().T() ;
      
      logmessage << "Values of constraint equations after solution (X=active,O=inactive): "
		 << std::endl ;
      for(size_t i = 0; i<Al::ConstraintDerivatives::NumConstraints; ++i) {
	logmessage << std::setw(4) << m_derivatives.front()->name(i) << " "
		   << std::setw(2) << (m_derivatives.front()->isActive(i)?'X':'O') << " "
		   << std::setw(12) << constraintdelta(i) 
		   << " +/- " << AlParameters::signedSqrt( constraintcov(i,i) ) << std::endl ;
      }
    }
  }

  void AlignConstraintTool::addConstraintToNominal(const Elements& elements,
						   AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const
  {
    info() << "Adding constraint to nominal geometry" << endreq ;
    // this adds chisquare constraints for the survey. since we don't
    // really have the survey we constrain to the 'nomina; position.
    double sigma[6] ;
    sigma[0] = m_sigmaNominalTx ;
    sigma[1] = m_sigmaNominalTy ;
    sigma[2] = m_sigmaNominalTz ;
    sigma[3] = m_sigmaNominalRx ;
    sigma[4] = m_sigmaNominalRy ;
    sigma[5] = m_sigmaNominalRz ;

    for(Elements::const_iterator ielem = elements.begin() ;
	ielem != elements.end(); ++ielem) 
      if( ielem->activeParOffset()>=0) {
	// get the current difference with nominal
	AlParameters currentdelta = (*ielem).currentActiveDelta() ;
	// now assign errors. for the moment we choose errors that are
	// constant: 1mm for all positions and 1mrad for all rotations.
	// update the first and second derivative
	for(size_t iactive = 0; iactive<(*ielem).dofMask().nActive(); ++iactive) {
	  double thissigma = sigma[(*ielem).dofMask().parIndex(iactive)] ;
	  double weight = 1/(thissigma*thissigma) ;
	  // this is tricky: need to get the sign right!
	  double residual = -currentdelta.parameters()(iactive) ;
	  size_t ipar = iactive + (*ielem).activeParOffset() ;
	  halfDChi2DAlpha(ipar) += weight * residual; 
	  halfD2Chi2DAlpha2.fast(ipar,ipar) += weight ;
	}
      }
  }
}
