// $Id: LoKiJetParticleMaker.cpp,v 1.1 2007-10-15 22:06:35 ibelyaev Exp $
// ============================================================================
// include files  
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IParticleMaker.h"
#include "Kernel/IJetMaker.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class JetParticleMaker
   *  The simple implementation of interface IParticleMaker for building the 
   *  jets using IJetMaker tool
   *  @see IParticleMaker 
   *  @see IJetMaker 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-10-15
   */
  class JetParticleMaker 
    : public virtual IParticleMaker 
    , public              GaudiTool 
  {
    /// friend class for instantiation
    friend class ToolFactory<LoKi::JetParticleMaker> ;
  public:
    /// make the particles 
    virtual StatusCode makeParticles ( LHCb::Particle::ConstVector& particles ) ;
    /// initialize the tool 
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiTool::initialize () ;
      if ( sc.isFailure() ) { return sc ; }
      // locate the jet-finder 
      m_jetMaker = tool<IJetMaker>( m_jetMakerName , this ) ;
      // check the inputs 
      if ( m_inputs.empty() ) { Warning ( "No input locations are specified!" ) ; }
      return StatusCode::SUCCESS ;
    }
  protected:
    // standard constructor
    JetParticleMaker ( const std::string& type   , 
                       const std::string& name   , 
                       const IInterface*  parent ) 
      : GaudiTool      (  type , name , parent ) 
      , m_jetMakerName ( "LoKi::KtKetMaker/JetMaker" )
      , m_jetMaker     ( 0 )
    {
      declareInterface<IParticleMaker> ( this ) ;
      
      declareProperty ( "JetMaker" , m_jetMakerName , "The type/name of Jet-Maker tool" ) ;
      declareProperty ( "Inputs"   , m_inputs       , "The list of input locations "    ) ; 
    }
    /// virtual and protected destructor 
    virtual ~JetParticleMaker() {}  
  private:
    // the default contructor is disabled 
    JetParticleMaker() ; ///< no default contstructor 
    // the copy contructor is disabled 
    JetParticleMaker( const JetParticleMaker&) ; ///< no copy contstructor 
    // the assignement operator is disabled 
    JetParticleMaker& operator=( const JetParticleMaker&) ; ///< no assignement
  private:
    // the jet maker name 
    std::string      m_jetMakerName ; ///< the jet-maker name 
    // the jet maker itself 
    const IJetMaker* m_jetMaker     ; ///< the jet-maker itself  
    typedef std::vector<std::string>  Inputs ;
    // input locations 
    Inputs         m_inputs ; ///< the input locations 
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,JetParticleMaker) ;
// ============================================================================
// make the particles 
// ============================================================================
StatusCode LoKi::JetParticleMaker::makeParticles 
( LHCb::Particle::ConstVector& particles ) 
{
  const size_t n0 = particles.size() ;
  
  IJetMaker::Input inputs ;
  // loop over all input locations 
  for ( Inputs::const_iterator  i = m_inputs.begin() ; m_inputs.end() != i ; ++i ) 
  {
    if ( exist<LHCb::Particle::Container> ( *i ) ) 
    {
      const LHCb::Particle::Container* parts = 
        get<LHCb::Particle::Container> ( *i ) ;
      inputs.insert( inputs.end() , parts -> begin() , parts -> end() ) ;
    }
    else if ( exist<LHCb::Particle::Container> ( (*i)  + "/Particles" ) ) 
    {
      const LHCb::Particle::Container* parts = 
        get<LHCb::Particle::Container> ( (*i) + "/Particles") ;
      inputs.insert( inputs.end() , parts -> begin() , parts -> end() ) ;      
    }
    else { return Error ( "No valid location: " + (*i) ) ; }
  }        
  
  // empty container of conmstituents? 
  if ( inputs.empty() ) { Warning ( "Empty container of input particles ") ; }

  
  // prepare the container for output jets 
  IJetMaker::Jets jets ;
  jets.reserve ( 20 ) ;
  
  // use Jet Maker tool:
  StatusCode sc = m_jetMaker -> makeJets ( inputs ,  jets ) ;
  if ( sc.isFailure() ) { return Error ( "The error form JetMaker" , sc ) ; }
  
  // add the jets to the output container: 
  particles.insert( particles.end() , jets.begin() , jets.end() ) ;
  
  // some statistics 
  counter("#jets") += jets.size() ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================]
// The END 
// ============================================================================
