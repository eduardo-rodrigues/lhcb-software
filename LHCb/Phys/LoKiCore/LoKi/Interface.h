// $Id: Interface.h,v 1.1 2006-03-14 18:56:59 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_INTERFACE_H 
#define LOKI_INTERFACE_H 1
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/AddRef.h"
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
  /** @class Interface Interface.h LoKi/Interface.h
   *
   *  Helper class for manipulations with interfaces 
   *  It looks like reduced smart INTRUSIVE pointer 
   *
   *  @author Vanya BELYAEV ibelyaev@physics,syr.edu
   *  @date   2006-03-10
   */
  template <class TYPE>
  class Interface 
  {
  public: 
    /** Standard constructor from the interface 
     *  @param obj object to be used 
     */
    Interface ( const TYPE* obj ) 
      : m_object ( const_cast<TYPE*>( obj ) ) { LoKi::addRef ( m_object ) ; } ;
    /// templated copy constructor 
    template <class OTHER>
    Interface ( const Interface<OTHER>& right ) 
      : m_object( right.m_object )            { LoKi::addRef ( m_object ) ; } ;
    /// templated assignement
    template <class OTHER>
    Interface& operator= ( const Interface<OTHER>& right ) 
    {
      if ( &right == *this ) { return *this ; }       // RETURN 
      TYPE* tmp = m_object ;
      m_object  = right.m_object ;
      //                           the order *DOES* matter! 
      LoKi::addRef  ( m_object ) ; // 1) increment the counter 
      LoKi::release ( tmp      ) ; // 2) decrement the counter 
      //
      return *this ;                                  // RETURN 
    } ;
    /// virtual destructor 
    virtual ~Interface()  ///< Destructor
    { LoKi::release ( m_object ) ; };
    /// valid pointer? 
    bool validPointer         () const { return 0 != m_object ; }
    /// conversion to underlying type 
    inline TYPE* operator->   () const { return getObject()   ; }
    /// access to the object 
    inline TYPE* getObject    () const { return m_object      ; }
  protected:
    /// the default constructor is protected
    Interface() : m_object( 0 ) {} ;
  private:
    /// the object to be stored 
    TYPE* m_object ;
  } ; 
  
} ; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_INTERFACE_H
// ============================================================================
