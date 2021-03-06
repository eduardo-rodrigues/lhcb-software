// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/GetTESLocations.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/MCFinder.h"
#include "LoKi/MCFinderObj.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"
// ============================================================================
// LoKiAlgoMC
// ============================================================================
#include "LoKi/AlgoMC.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::AlgoMC
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-31 
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// standars suffix for constainer of particles 
  const std::string s_PARTICLES      = "/Particles"      ;
  /// standars suffix P->MC relations 
  const std::string s_P2MCPRELATIONS = "/P2MCPRelations" ;
  // ==========================================================================
  /// has non-trivial RootInTES ? 
  inline bool hasRootInTES ( const std::string& rit ) 
  { return 7 < rit.length() ; }
  // ==========================================================================  
}
// ============================================================================
/*  standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::AlgoMC::AlgoMC
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : LoKi::Algo ( name , pSvc ) 
//
  , m_mcselected   ()
  , m_mcvselected  ()
//
  , m_genselected  ()
  , m_genvselected ()
//
  , m_mcfinders    () 
  , m_imcfinders   () 
  , m_mcmatchers   ()  
//
// Relation tables (TES addresses) 
//
// Particle      -> MC
  , m_P2MC         ()
// Particle      -> MC
  , m_P2MCW        () 
// ProtoParticle -> MC
  , m_PP2MC        () 
// Track         -> MC
  , m_T2MC         ()
// Track         -> MC
  , m_T2MCW        ()
//
  , m_mc2collisionName ( "LoKi_MC2Collision/MC2Collision:PUBLIC" )
  , m_mc2collision     ( 0 )
//
  , m_hepmc2mcName     ( "LoKi_HepMC2MC/HepMC2MC:PUBLIC"         )
  , m_hepmc2mc         ( 0 )
//
  , m_pv2mcName        ( "LoKi_PV2MC/PV2MC:PUBLIC"               )
  , m_pv2mc            ( 0 )
//
  , m_disableMCMatch   ( false ) 
//
  , m_collectP2MCLinks ( false ) 
  , m_mcdecay  ( 0 ) 
  , m_gendecay ( 0 ) 
{
  //
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
  
  declareProperty   
    ( "P2MCs"  , 
      m_P2MC   , 
      "List of Particle -> MCParticle relation tables      (LoKi::Types::TableP2MC)" ) ;
  declareProperty  
    ( "WP2MCs" , 
      m_P2MCW  , 
      "List of Particle -> MCParticle relation tables      (LoKi::Types::TableP2MCW)" ) ;
  declareProperty   
    ( "PP2MCs" , 
      m_PP2MC  , 
      "List of ProtoParticle -> MCParticle relation tables (LoKi::Types::TablePP2MC)" ) ;
  declareProperty   
    ( "T2MCs"  ,
      m_T2MC   , 
      "List of Track -> MCParticle relation tables         (LoKi::Types::TableT2MC)"  ) ;
  declareProperty   
    ( "WT2MCs" , 
      m_T2MCW  , 
      "List of Track -> MCParticle relation tables         (LoKi::Types::TableT2MCW)"  ) ;
  //
  declareProperty   
    ( "MC2CollisionTool" , 
      m_mc2collisionName , 
      "Type/Name of tool for MC <--> Collision association (IMC2Collision interface)") ;  
  declareProperty   
    ( "HepMC2MCTool"     , 
      m_hepmc2mcName  , 
      "Type/Name of tool for MC <--> HepMC association     (IHepMC2MC interface)") ;  
  declareProperty  
    ( "PV2MCTool" , 
      m_pv2mcName ,
      "Type/Name of tool for MC association of PVs         (IPV2MC interface)"   ) ;  
  //
  declareProperty   
    ( "DisableMCMatch"    , 
      m_disableMCMatch    , 
      "Disable MC-truth matching" ) ;
  declareProperty 
    ( "CollectP2MCPLinks" , 
      m_collectP2MCLinks  , 
      "(Auto)collect Particle -> MCParticle links (useful for micro-DST") ;
} 
// ============================================================================
// virtual and protected destructor 
// ============================================================================
LoKi::AlgoMC::~AlgoMC(){} 
// ============================================================================
/** 'Select' the MC particles to be used in local storage
 *  
 *  - The MC Particles are selected from the location
 *
 *  @code
 *
 *  MCRange kaons = mcselect( "Kaons" , 
 *                  "K+" == MCABSID  && MCP > 5 * GeV );
 *
 *  @endcode
 *
 *  - The example illustrate the 'selection'/'filtering of MC particles, 
 *    which are @c K+ or @c K- and have a
 *  momentum in excess of 5GeV/c
 *  - The selected MC particles
 *  are stored inside local LoKi storage under the tag @c "Kaons"
 *  and returned as a sequence of MC particles @c kaons .
 *
 *  @see LHCb::MCParticle 
 *  @see LoKi::Types::MCRange
 *  @see LoKi::Cuts::MCABSID 
 *  @see LoKi::Cuts::MCP
 *  @param name name/tag assigned to the selected particles
 *  @param cut  cut to be applied
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&         tag      , 
  const LoKi::Types::MCCuts& cut      , 
  const std::string&         location ) 
{
  const LHCb::MCParticles* mcps = get<LHCb::MCParticles>( location );
  return mcselect ( tag , mcps , cut ) ;
}
// ============================================================================
/* 'Select' the MC vertices to be used in local storage
 *  
 *  - The MC vertices are selected from the TES location
 *
 *  @code
 *
 *  MCVRange primary = mcvselect( "MCPVs" , MCISPRIMARY );
 *
 *  @endcode
 *
 *  @see LHCb::MCVertex
 *  @see LoKi::Types::MCVRange
 *  @see LoKi::Cuts::MCISPRIMARY 
 *  @param name name/tag assigned to the selected particles
 *  @param cut  cut to be applied
 *  @param location TES location of MC-vertices
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCVRange 
LoKi::AlgoMC::mcvselect 
( const std::string&          tag      , 
  const LoKi::Types::MCVCuts& cut      ,
  const std::string&          location )
{
  const LHCb::MCVertices* mcps = get<LHCb::MCVertices>( location );
  return mcvselect ( tag , mcps , cut ) ;
}
// ============================================================================
// get LoKi::MCFinder objects
// ============================================================================
LoKi::MCFinder 
LoKi::AlgoMC::mcFinder( const std::string& name ) const 
{
  { // check exisiting tool 
    MCfinders::iterator ifind = m_mcfinders.find( name ) ;
    if ( m_mcfinders.end() != ifind ) { return LoKi::MCFinder( ifind->second ) ; }
  }
  // create new finder
  const LoKi::IReporter* rep = reporter ("MCFinder " + name  ) ;
  // get IMCDecayFinder tool 
  IMCDecayFinder* finder = 0 ;
  { // check located tool 
    IMCfinders::iterator ifind = m_imcfinders.find( name ) ;
    if ( m_imcfinders.end() == ifind )
    {
      // locate new tool
      finder = tool<IMCDecayFinder>( "MCDecayFinder" , name , this ) ;
      // add the finder into the container 
      m_imcfinders[name] = finder ;
    }
    else { finder = ifind->second ; }
  }
  
  Assert ( 0 != finder , "IMCDecayFinder* points to NULL!" );
  
  LoKi::MCFinderObj* object = 
    new LoKi::MCFinderObj ( name , finder , this , rep ) ;
  // increment the reference counter 
  object->addRef() ;
  // add the finder into the container 
  m_mcfinders[name] = object ;
  
  return LoKi::MCFinder ( object ) ;
} 
// ============================================================================
// get LoKi::MCMatch obejct
// ============================================================================
LoKi::MCMatch 
LoKi::AlgoMC::mcTruth  ( const std::string& name ) const 
{  
  //
  LoKi::MCMatchObj* object = 0 ;
  // check exisiting tool 
  MCmatchers::iterator ifind = m_mcmatchers.find( name ) ;
  if ( m_mcmatchers.end() != ifind ) { object = ifind->second ; }
  if ( 0 == object ) 
  {
    // get the reporter 
    const LoKi::IReporter* rep = reporter( "MCMatch " + name ) ;  
    // create the new matcher 
    object = new LoKi::MCMatchObj( name , rep ) ;
    // increment the reference counter 
    object->addRef() ;
    // add the matcher into container 
    m_mcmatchers[name] = object  ;
  } 
  //
  object->clear() ;
  /// feed the matcher with the information
  if ( !m_disableMCMatch ) 
  {
    // Particle      -> MC
    _feedIt<LoKi::Types::TableP2MC>   ( object , m_P2MC   ) . ignore () ;
    // Particle      -> MC with weight 
    _feedIt<LoKi::Types::TableP2MCW>  ( object , m_P2MCW  ) . ignore ()  ;
    // ProtoParticle -> MC with weight 
    _feedIt<LoKi::Types::TablePP2MC>  ( object , m_PP2MC  ) . ignore () ;
    // Track         -> MC 
    _feedIt<LoKi::Types::TableT2MC>   ( object , m_T2MC   ) . ignore () ;
    // Track         -> MC with double  weight 
    _feedIt<LoKi::Types::TableT2MCW>  ( object , m_T2MCW  ) . ignore () ;
    //
    //
    // for uDST MC a bit more work is needed:
    if ( m_collectP2MCLinks && hasRootInTES ( rootInTES() ) ) 
    {
      const std::string::size_type rit_len = rootInTES().length() ;
      // get all particles
      const LHCb::Particle::Range parts = this->particles();
      // collect all TES locations from them 
      const DaVinci::Utils::GetTESLocations locator ( parts.begin() , parts.end  () ) ;
      const std::vector<std::string>& inputs =  locator.locations() ;
      // transform it into proper locations of relation tables 
      std::vector<std::string>  tables ; tables.reserve( inputs.size() ) ;
      for ( std::vector<std::string>::const_iterator i = inputs.begin() ;
            inputs.end() != i ; ++i )
      {
        const std::string::size_type p1 = i-> find ( rootInTES () ) ;
        const std::string::size_type p2 = i->rfind ( s_PARTICLES  ) ;
        if ( 0 == p1 && std::string::npos != p2 && rit_len < p2 ) 
        { tables.push_back ( std::string ( *i , rit_len , p2 - rit_len  ) + s_P2MCPRELATIONS ) ; }
        else 
        { Warning("Can't construct P2MCP location from '" + (*i)+ "', skip it") ; }     
      }
      // finally use these tables to load MC-truth information 
      // Particle      -> MC
      _feedIt<LoKi::Types::TableP2MC> ( object , tables ,  false ) . ignore () ;
    }
  }
  //
  return LoKi::MCMatch( object ) ;
} 
// ============================================================================
// clear the internal LoKi storages 
// ============================================================================
StatusCode LoKi::AlgoMC::clear() 
{
  StatusCode sc = LoKi::Algo::clear() ;
  //
  for ( LoKi::MCTypes::MCSelected::map_iterator imc = 
          m_mcselected.begin() ; m_mcselected.end() != imc ; ++imc ) 
  { counter ( "#" + imc->first )   += imc->second.size() ; }
  m_mcselected   .clear () ;
  //
  for ( LoKi::MCTypes::MCVSelected::map_iterator imcv = 
          m_mcvselected.begin() ; m_mcvselected.end() != imcv ; ++imcv ) 
  { counter ( "#" + imcv->first )  += imcv->second.size() ; }
  m_mcvselected  .clear () ;
  //
  for ( LoKi::GenTypes::GenSelected::map_iterator igen = 
          m_genselected.begin() ; m_genselected.end() != igen ; ++igen ) 
  { counter ( "#" + igen->first )  += igen->second.size() ; }
  m_genselected  .clear () ;
  //
  for ( LoKi::GenTypes::GenVSelected::map_iterator igenv = 
          m_genvselected.begin() ; m_genvselected.end() != igenv ; ++igenv ) 
  { counter ( "#" + igenv->first ) += igenv->second.size() ; }
  m_genvselected .clear () ;
  //
  // clear the matchers  
  for ( MCmatchers::iterator imc = m_mcmatchers.begin() ; 
        m_mcmatchers.end() != imc ; ++imc )
  {
    LoKi::MCMatchObj* obj = imc->second  ;
    if ( 0 != obj ) { obj->clear() ; }
  }
  return sc ;
} 
// ============================================================================
// initialize the algorithm 
// ============================================================================
StatusCode LoKi::AlgoMC::initialize () 
{ 
  StatusCode sc = LoKi::Algo::initialize () ; 
  if ( sc.isFailure() ) { return sc ; }
  //
  if      ( m_disableMCMatch   )
  { Warning ( "MCMatch is explicitely DISABLED" ) ; }
  else if (  m_P2MC  .empty() && 
             m_P2MCW .empty() && 
             m_PP2MC .empty() && 
             m_T2MC  .empty() && 
             m_T2MCW .empty() && !hasRootInTES( rootInTES() ) ) 
  { Warning ( "MCMatch is implicitely DISABLED: no input data specified" ) ; }
  else if (  m_P2MC  .empty() && 
             m_P2MCW .empty() && 
             m_PP2MC .empty() && 
             m_T2MC  .empty() && 
             m_T2MCW .empty() &&  
             hasRootInTES( rootInTES() ) && !m_collectP2MCLinks ) 
  { Warning ( "MCMatch is implicitely DISABLED: no input data specified" ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// make the execution of the algorithm 
// ============================================================================
StatusCode LoKi::AlgoMC::execute    () { return LoKi::Algo::execute () ; } 
// ============================================================================
// perform the real analysis 
// ============================================================================
StatusCode LoKi::AlgoMC::analyse    () { return LoKi::Algo::analyse    () ; } 
// ============================================================================
// finalize the algorithm 
// ============================================================================
StatusCode LoKi::AlgoMC::finalize   () 
{
  // clear all the storages 
  clear().ignore() ;  
  { // finders
    for ( MCfinders::iterator entry = m_mcfinders.begin() ; 
          m_mcfinders.end() != entry ; ++entry ) 
    {
      LoKi::MCFinderObj* obj = entry->second ;
      if ( 0 != obj ) { obj->release() ; }
    } ;
    m_mcfinders  .clear() ;
    m_imcfinders .clear() ;
  }  
  { // matchers  
    for ( MCmatchers::iterator entry = m_mcmatchers.begin() ; 
          m_mcmatchers.end() != entry ; ++entry ) 
    {
      LoKi::MCMatchObj* obj = entry->second ;
      if ( 0 != obj ) { obj->release() ; }
    } 
    m_mcmatchers .clear() ;
  }
  //
  m_mcdecay  . release () ;
  m_gendecay . release () ;
  // finalize the base class 
  return LoKi::Algo::finalize   () ; 
} 
// ============================================================================
// get the pointer to IMC2Collision tool 
// ============================================================================
const IMC2Collision* LoKi::AlgoMC::mc2collision    () const 
{
  if ( 0 == m_mc2collision ) 
  { m_mc2collision = tool<IMC2Collision>( m_mc2collisionName , this ) ; }
  return m_mc2collision ;
} 
// ============================================================================
// get the pointer to IHepMC2MC tool 
// ============================================================================
const IHepMC2MC* LoKi::AlgoMC::hepMC2MC  () const 
{
  if ( 0 == m_hepmc2mc ) { m_hepmc2mc = tool<IHepMC2MC>( m_hepmc2mcName, this ) ; }
  return m_hepmc2mc ;
} 
// ============================================================================
// get the pointer to IPV2MC tool 
// ============================================================================
const IPV2MC* LoKi::AlgoMC::pv2MC        () const 
{
  if ( 0 == m_pv2mc ) { m_pv2mc = tool<IPV2MC> ( m_pv2mcName , this ) ; }
  return m_pv2mc ;
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const HepMC::GenEvent*              event  , 
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == event ) 
  {
    Error ( "HepMC::GenEvent* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  return gselect
    ( tag , event->particles_begin() , event->particles_end() , cut ) ; 
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const LHCb::HepMCEvent*             event  , 
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == event ) 
  {
    Error ( "LHCb::HepMCEvent* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  return gselect ( tag , event->pGenEvt() , cut ) ;
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const LHCb::HepMCEvent::Container*  events , 
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == events ) 
  {
    Error ( "LHCb::HepMCEvents* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  // explicit loop over the container of events
  for ( LHCb::HepMCEvent::Container::const_iterator ie = 
          events->begin() ; events->end() != ie ; ++ie ) 
  {
    gselect ( tag , *ie , cut ) ;  
  }
  return gselected ( tag ) ;
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&        tag      , 
  const LoKi::Types::GCuts& cuts     ,
  const std::string&        location ) 
{
  const LHCb::HepMCEvent::Container* events = 
    get<LHCb::HepMCEvent::Container> ( location ) ;
  return gselect ( tag , events , cuts ) ; 
}
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&                  tag    ,
  const HepMC::GenVertex*             vertex ,
  HepMC::IteratorRange                range  ,
  const LoKi::Types::GCuts&           cut    ) 
{
  if ( 0 == vertex ) 
  {
    Error ( "HepMC::GenVertex* points to null, return empty range!" ) ;
    return LoKi::Types::GRange() ;
  }
  //
  HepMC::GenVertex* v = const_cast<HepMC::GenVertex*> ( vertex ) ;
  //
  return gselect 
    ( tag , v->particles_begin ( range ) , v->particles_end   ( range ) , cut ) ;
} 
// ============================================================================
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const Decays:::IMCDecay::Finder& finder = ... ; 
 *  MCRange good = mcselect( "Good" , finder ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param finder   the decay finder to be used 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&              tag      , 
  const Decays::IMCDecay::Finder& finder   ,
  const std::string&              location ) 
{
  /// get MC-particles form TES 
  const LHCb::MCParticle::Container* mcps = 
    get<LHCb::MCParticle::Container> ( location ) ;
  //
  return mcselect ( tag                  , 
                    mcps->begin ()       , 
                    mcps->end   ()       ,
                    finder               , 
                    LoKi::Objects::_ALL_ ) ;
}
// ============================================================================
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const Decays:::IMCDecay::iTree& tree = ... ; 
 *  MCRange good = mcselect( "Good" , tree ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::iTree
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param tree     the decay tree to be used 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&              tag      , 
  const Decays::IMCDecay::iTree&  tree     ,
  const std::string&              location ) 
{
  //
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "mcselect: Unable to validate tree '" + tree.toString() + "'" ) ;
      return LoKi::Types::MCRange() ;
    }  
  }
  //
  Decays::IMCDecay::Finder finder ( tree ) ;
  return mcselect ( tag , finder , location ) ;
}
// ========================================================================    
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const std::string& descriptor  = ... ; 
 *  MCRange good = mcselect( "Good" , descriptor ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *
 *  @param tag         name/tag assigned to the selected particles
 *  @param descriptor  the decay descriptor 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ========================================================================    
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string& tag         , 
  const std::string& descriptor  ,
  const std::string& location    ) 
{
  if ( !m_mcdecay ) 
  { m_mcdecay = tool<Decays::IMCDecay>  ( "LoKi::MCDecay/MCDecay" ,this ) ; }
  //
  Assert ( !(!m_mcdecay) , "Decays::IMCDecay* poitns to NULL!" ) ;
  //
  Decays::IMCDecay::Tree tree = m_mcdecay->tree ( descriptor ) ;
  if ( !tree ) 
  {
    Error ( "mcselect: Unable to create decay tree from descriptor '" + 
            descriptor + "'" ) ;
    return LoKi::Types::MCRange () ;
  }
  //
  return mcselect ( tag, Decays::IMCDecay::Finder ( tree ) , location ) ;
}
// ============================================================================
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const Decays:::IMCDecay::Finder& finder = ... ; 
 *  const MCRange input = ...; 
 *
 *  MCRange good = mcselect( "Good" , input , finder ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param input    the inptu range 
 *  @param finder   the decay finder to be used 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&              tag    , 
  const LoKi::Types::MCRange&     input  , 
  const Decays::IMCDecay::Finder& finder ) 
{
  return mcselect ( tag            , 
                    input.begin () , 
                    input.end   () , 
                    finder         , 
                    LoKi::Objects::_ALL_) ;
}
// ============================================================================
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const Decays:::IMCDecay::Finder& finder = ... ; 
 *  const LHCb::MCParticle::ConstVector input = ...; 
 *
 *  MCRange good = mcselect( "Good" , input , finder ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param input    the inptu range 
 *  @param finder   the decay finder to be used 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&                   tag    , 
  const LHCb::MCParticle::ConstVector& input  , 
  const Decays::IMCDecay::Finder&      finder ) 
{
  return mcselect ( tag            , 
                    input.begin () , 
                    input.end   () , 
                    finder         , 
                    LoKi::Objects::_ALL_) ;
}
// ========================================================================    
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const Decays:::IMCDecay::iTree& tree = ... ; 
 *  const MCRange input = ...; 
 *
 *  MCRange good = mcselect( "Good" , input , tree ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param input    the input range 
 *  @param yree    the decay tree 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ========================================================================    
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&              tag    , 
  const LoKi::Types::MCRange&     input  , 
  const Decays::IMCDecay::iTree&  tree   ) 
{
  //
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "mcselect: Unable to validate tree '" + tree.toString() + "'" ) ;
      return LoKi::Types::MCRange() ;
    }  
  }
  // 
  return mcselect ( tag , input , Decays::IMCDecay::Finder ( tree ) ) ;
}
// ========================================================================    
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const Decays:::IMCDecay::iTree& tree = ... ; 
 *  const LHCb::MCParticle::ConstVector input = ...; 
 *
 *  MCRange good = mcselect( "Good" , input , tree ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param input    the input range 
 *  @param tree    the decay tree 
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ========================================================================    
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&                   tag    , 
  const LHCb::MCParticle::ConstVector& input  , 
  const Decays::IMCDecay::iTree&       tree   ) 
{
  //
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "mcselect: Unable to validate tree '" + tree.toString() + "'" ) ;
      return LoKi::Types::MCRange() ;
    }  
  }
  // 
  return mcselect ( tag , input , Decays::IMCDecay::Finder ( tree ) ) ;
}
// ========================================================================    
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const std::string& descriptor = ... ; 
 *  const MCRange input = ...; 
 *
 *  MCRange good = mcselect( "Good" , input , descriptor ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param input    the input range 
 *  @param descriptor the decay descriptor
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ========================================================================    
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&          tag        , 
  const LoKi::Types::MCRange& input      , 
  const std::string&          descriptor ) 
{
  if ( !m_mcdecay ) 
  { m_mcdecay = tool<Decays::IMCDecay>  ( "LoKi::MCDecay/MCDecay" ,this ) ; }
  //
  Assert ( !(!m_mcdecay) , "Decays::IMCDecay* poitns to NULL!" ) ;
  //
  Decays::IMCDecay::Tree tree = m_mcdecay->tree ( descriptor ) ;
  if ( !tree ) 
  {
    Error ( "mcselect: Unable to create decay tree from descriptor '" + 
            descriptor + "'" ) ;
    return LoKi::Types::MCRange () ;
  }
  //
  return mcselect ( tag                               , 
                    input.begin()                     , 
                    input.end  ()                     , 
                    Decays::IMCDecay::Finder ( tree ) , 
                    LoKi::Objects::_ALL_              ) ;
  
}
// ========================================================================    
/*  'Select' the MC particles to be used in local storage 
 *  
 *  - The MC Particles are selected from the TES location
 *
 *  @code
 *
 *  const std::string& descriptor = ... ; 
 *  const LHCb::MCParticle::ConstVector input = ...; 
 *
 *  MCRange good = mcselect( "Good" , input , descriptor ) ;
 *
 *  @endcode
 *
 *  @see LHCb::MCParticle 
 *  @see Decays::IMCDecay
 *  @see Decays::IMCDecay::Finder 
 *
 *  @param tag      name/tag assigned to the selected particles
 *  @param input    the input range 
 *  @param descriptor the decay descriptor
 *  @param location TES location of MC-particles
 *  @return selected range of particles
 */
// ========================================================================    
LoKi::Types::MCRange 
LoKi::AlgoMC::mcselect 
( const std::string&                   tag        , 
  const LHCb::MCParticle::ConstVector& input      , 
  const std::string&                   descriptor ) 
{
  if ( !m_mcdecay ) 
  { m_mcdecay = tool<Decays::IMCDecay>  ( "LoKi::MCDecay/MCDecay" ,this ) ; }
  //
  Assert ( !(!m_mcdecay) , "Decays::IMCDecay* points to NULL!" ) ;
  //
  Decays::IMCDecay::Tree tree = m_mcdecay->tree ( descriptor ) ;
  if ( !tree ) 
  {
    Error ( "mcselect: Unable to create decay tree from descriptor '" + 
            descriptor + "'" ) ;
    return LoKi::Types::MCRange () ;
  }
  //
  return mcselect ( tag                               , 
                    input.begin()                     , 
                    input.end  ()                     , 
                    Decays::IMCDecay::Finder ( tree ) , 
                    LoKi::Objects::_ALL_              ) ;
  
}
// ============================================================================
/*  'Select' generator(HepMC) particles to be used in local storage.
 *  
 *   @code
 * 
 *     const Decays::IGenDecay::Finder& finder = ... ;
 *     GRange pions = gselect("gpi" , finder ) ;
 *
 *   @endcode 
 * 
 *   @param tag the unique tag to be associated with selecte dparticles 
 *   @param finder the decay finder 
 *   @param location TES location of LHCb::HepMCEvent::Container
 *   @return selected particles, saved into the local storage 
 */
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&               tag      , 
  const Decays::IGenDecay::Finder& finder   ,
  const std::string&               location ) 
{
  // 
  // get HEpMC-Event form TES 
  const LHCb::HepMCEvent::Container* events = 
    get<LHCb::HepMCEvent::Container> ( location ) ;
  //
  if ( 0 == events ) { return LoKi::Types::GRange() ; }  // RETURN ;
  // loop ocver HEpMC-events 
  for ( LHCb::HepMCEvent::Container::const_iterator ievent = 
          events->begin() ; events->end() != ievent ; ++ievent ) 
  {
    const LHCb::HepMCEvent* event  = *ievent ;
    if ( 0 == event  ) { continue ; }                     // RETURN 
    const HepMC::GenEvent*  gevent = event->pGenEvt() ;
    if ( 0 == gevent ) { continue ; }                     // RETURN 
    //
    gselect 
      ( tag                        , 
        gevent->particles_begin () , 
        gevent->particles_end   () , 
        finder                     , 
        LoKi::Objects::_ALL_       ) ;
  }
  //
  return gselected ( tag ) ;
}
// ============================================================================
/*  'Select' generator(HepMC) particles to be used in local storage.
 *  
 *   @code
 * 
 *     const Decays::IGenDecay::iTree& tree = ... ;
 *     GRange pions = gselect("gpi" , tree ) ;
 *
 *   @endcode 
 * 
 *   @param tag the unique tag to be associated with selecte dparticles 
 *   @param tree  the decay tree
 *   @param location TES location of LHCb::HepMCEvent::Container
 *   @return selected particles, saved into the local storage 
 */
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string&               tag      , 
  const Decays::IGenDecay::iTree&  tree     ,
  const std::string&               location )
{
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "gselect: Unable to validate tree '" + tree.toString() + "'" ) ;
      return LoKi::Types::GRange() ;
    }  
  }
  //
  return gselect ( tag                                , 
                   Decays::IGenDecay::Finder ( tree ) , 
                   location                           ) ;
  //
}
// ============================================================================
/*  'Select' generator(HepMC) particles to be used in local storage.
 *  
 *   @code
 * 
 *     GRange psis = gselect("gpsi" , "Xb --> ^( J/psi(1S) => l+ l- ) ... ") ;
 *
 *   @endcode 
 * 
 *   @param tag the unique tag to be associated with selecte dparticles 
 *   @param descriptor the decay descriptor 
 *   @param location TES location of LHCb::HepMCEvent::Container
 *   @return selected particles, saved into the local storage 
 */
// ============================================================================
LoKi::Types::GRange 
LoKi::AlgoMC::gselect 
( const std::string& tag        , 
  const std::string& descriptor ,
  const std::string& location   ) 
{
  //
  if ( !m_gendecay ) 
  { m_gendecay = tool<Decays::IGenDecay>  ( "LoKi::GenDecay/GenDecay" ,this ) ; }
  //
  Assert ( !(!m_gendecay) , "Decays::IGenDecay* points to NULL!" ) ;
  //
  Decays::IGenDecay::Tree tree = m_gendecay->tree ( descriptor ) ;
  if ( !tree ) 
  {
    Error ( "gselect: Unable to create decay tree from descriptor '" + 
            descriptor + "'" ) ;
    return LoKi::Types::GRange () ;
  }
  //
  return gselect ( tag      , 
                   tree     , 
                   location ) ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

