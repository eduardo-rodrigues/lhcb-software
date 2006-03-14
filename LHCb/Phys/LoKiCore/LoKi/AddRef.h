// $Id: AddRef.h,v 1.1 2006-03-14 18:56:59 ibelyaev Exp $
// ============================================================================
// CVS tag $NAme:$, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_ADDREF_H 
#define LOKI_ADDREF_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Convertible.h"
#include "LoKi/Release.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23 
 */
// ============================================================================

namespace LoKi 
{  
  /** @struct AddRef AddRef.h LoKi/AddRef.h
   * 
   *  Helper utility to resolve the templated 
   *  ambiguity for GCC compiler 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  template <class TYPE,bool>
  struct AddRef
    : public std::unary_function<TYPE*,void>
  {
    bool operator() ( TYPE*       o ) 
    { 
      if ( 0 != o ) { o->addRef() ; } 
      return 0 != o ;
    }
  } ;
  template <class TYPE>
  struct AddRef<TYPE,true>
    : public std::unary_function<IInterface*,bool>
  {
    bool operator() ( IInterface* o ) 
    { 
      if ( 0 != o ) { o->addRef() ; } 
      return 0 != o ;
    }
  } ;
  
  /** @fn addRef
   *  helper function to invoke properly TYPE::addRef method 
   *  avoiding the template ambiguity for GCC compiler 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */   
  template <class TYPE>
  inline bool  addRef( TYPE* obj ) 
  {
    // create the proper actor
    AddRef<TYPE,LoKi::Convertible<TYPE,IInterface>::value> actor ;
    // and perform the proper action!
    return actor ( obj ) ;
  } ;
  
} ; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ADDREF_H
// ============================================================================
