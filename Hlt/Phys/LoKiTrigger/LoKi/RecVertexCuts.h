// $Id: RecVertexCuts.h,v 1.2 2008-01-22 14:26:37 hernando Exp $
// ============================================================================
#ifndef LOKI_RECVERTEXCUTS_H 
#define LOKI_RECVERTEXCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/RecVertices.h"
// ============================================================================
namespace LoKi 
{
  namespace Cuts 
  {
    // ========================================================================
    /** @var RVMAXPT
     *  The evaluator of "maxPt" for the recontructed vertex
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::VertexMaxPT
     *  @see LoKi::RecVertices::MaxPt
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::RecVertices::VertexMaxPt        RVMAXPT ;
    // ========================================================================
    /** @var RVMINPT
     *  The evaluator of "minPt" for the recontructed vertex
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::VertexMinPT
     *  @see LoKi::RecVertices::MinPt
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::RecVertices::VertexMinPt            RVMINPT ;
    // ========================================================================
    /** @var RVVDZ
     *  Evaluator of "delta-Z" distance between two vertices 
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::RecVertices::DeltaZ
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::RecVertices::DeltaZ                                     RVVDZ ;
    // ========================================================================
    /** @var RVVFC 
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::FC
     *  @see LoKi::RecVertices::FC
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */        
    const LoKi::RecVertices::FC                                         RVVFC ;
    // ========================================================================
    /** @var RVVFC2
     *  
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::FC2
     *  @see LoKi::RecVertices::FC2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */        
    const LoKi::RecVertices::FC2                                       RVVFC2 ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
} // end of namespace LoKi 
// ============================================================================
// the END 
// ============================================================================
#endif // LOKI_RECVERTEXCUTS_
// ============================================================================
