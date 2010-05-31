// $Id: MCParticles1.cpp,v 1.1 2010-05-31 20:33:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/MCTypes.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Services.h"
#include "LoKi/MCParticles1.h"
// ============================================================================


// ============================================================================
// constructor from the actual node
// ============================================================================
LoKi::MCParticles::DecNode::DecNode
( const Decays::iNode& node )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_node ( node )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::DecNode::result_type
LoKi::MCParticles::DecNode::operator()
  ( LoKi::MCParticles::DecNode::argument p ) const
{
  //
  if ( 0 == p )
  {
    Error ( "LHCb::MCParticle* point to NULL, return false") ;
    return false ;
  }
  //
  if ( !valid () ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::ParticlePropertySvc* poinst to NULL!") ;
    StatusCode sc = m_node.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Node" , sc ) ;
  }
  // use the node for evaluation
  return m_node.node ( p->particleID() ) ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::MCParticles::DecNode::fillStream( std::ostream& s ) const
{
  if ( !valid() ) { return s << " MCDECNODE(<invalid>) " ; }
  return s << " MCDECNODE( " << m_node << " ) ";
}
// ============================================================================


// ============================================================================
// constructor from the actual tree
// ============================================================================
LoKi::MCParticles::DecTree::DecTree
( const LoKi::MCParticles::DecTree::iTree& tree )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_tree ( tree )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::DecTree::result_type
LoKi::MCParticles::DecTree::operator()
  ( LoKi::MCParticles::DecTree::argument p ) const
{
  if ( 0 == p )
  {
    Error ( "LHCb::MCParticle* point to NULL, return false") ;
    return false ;
  }
  //
  if ( !valid () ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::ParticlePropertySvc* poinst to NULL!") ;
    StatusCode sc = m_tree.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Node" , sc ) ;
  }
  //
  // evaluate the tree 
  const bool OK = m_tree.tree ( p ) ;
  if ( OK               ) { return true  ; }                          // RETURN

  // for no-marked trees, the search is completed 
  if ( !m_tree.marked() ) { return false ; }                          // RETURN  
  
  // check for "marked" elements 
  iTree::Collection marked ;
  
  // loop over all "mother" particles 
  const LHCb::MCParticle* mother = p->mother() ;
  while ( 0 != mother ) 
  {
    reset () ;
    // check if mother matches the tree 
    if  ( m_tree.tree ( mother ) ) 
    {
      // collect the marked elements 
      marked.clear() ;
      m_tree.collect ( marked ) ;
      // look for particle in the list of marked particles 
      if ( marked.end() != std::find 
           ( marked.begin() , marked.end() , p ) ) { return true ; } // RETURN
    }
    // go to the next level 
    mother = mother->mother() ;                                // NEXT LEVEL 
  }
  //
  return false ;                                                      // RETURN 
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::MCParticles::DecTree::fillStream( std::ostream& s ) const
{
  if ( !valid() ) { return s << " MCDECTREE(<invalid>) " ; }
  return s << " MCDECTREE( " << m_tree << " ) ";
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
