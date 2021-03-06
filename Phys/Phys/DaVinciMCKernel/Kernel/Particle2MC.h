// $Id$
// ============================================================================
#ifndef KERNEL_PARTICLE2MC_H 
#define KERNEL_PARTICLE2MC_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/Particle.h"
// ============================================================================
// Forward Declarations 
// ============================================================================
template <class FROM,class TO>              class IRelation           ;
template <class FROM,class TO>              class IRelation2D         ;
template <class FROM,class TO,class WEIGHT> class IRelationWeighted   ;
template <class FROM,class TO,class WEIGHT> class IRelationWeighted2D ;
// ============================================================================
/** @file
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
 *  @date 2006-08-17 
 */
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  namespace Relations_
  {
    // ========================================================================
    /// the actual type of Particle   -> MC relations 
    typedef IRelationWeighted<LHCb::Particle,LHCb::MCParticle,double> 
    P2MCW   ;
    /// the actual type of MC   -> Particle relations 
    typedef IRelationWeighted<LHCb::MCParticle,LHCb::Particle,double> 
    MC2PW   ;
    /// the actual type of Particle <--> MC relations 
    typedef IRelationWeighted2D<LHCb::Particle,LHCb::MCParticle,double> 
    P2MC2DW ;
    /// the actual type of Particle   -> MC relations 
    typedef IRelation<LHCb::Particle,LHCb::MCParticle>            P2MC    ;
    /// the actual type of MC   -> Particle relations 
    typedef IRelation<LHCb::MCParticle,LHCb::Particle>            MC2P    ;
    /// the actual type of Particle <--> MC relations 
    typedef IRelation2D<LHCb::Particle,LHCb::MCParticle>          P2MC2D  ;
    // ========================================================================
  } //                                        end of namespace LHCb::Relations_ 
  // ==========================================================================
  /// the actual type of Particle   -> MC relations 
  typedef LHCb::Relations_::P2MCW                                 P2MCW   ;  
  /// the actual type of MC   -> Particle relations 
  typedef LHCb::Relations_::MC2PW                                 MC2PW   ;  
  /// the actual type of Particle<--> MC relations 
  typedef LHCb::Relations_::P2MC2DW                               P2MC2DW ;
  /// the actual type of Particle   -> MC relations 
  typedef LHCb::Relations_::P2MC                                  P2MC    ;  
  /// the actual type of MC   -> Particle relations 
  typedef LHCb::Relations_::MC2P                                  MC2P    ;  
  /// the actual type of Particle<--> MC relations 
  typedef LHCb::Relations_::P2MC2D                                P2MC2D  ;  
  // ==========================================================================
  namespace Particle2MCLocation
  {
    // ========================================================================
  } // end of namespace LHCb::Particle2MCLocation
  // ==========================================================================
} //                                                      end of namespace LHCb
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KERNEL_PARTICLE2MC_H
// ============================================================================
