// $Id: IAssociator.h,v 1.5 2002-05-10 14:49:39 phicharp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/04/29 09:09:56  phicharp
//  Make retrieve methods safer (2 new methods)
//
// Revision 1.3  2002/04/26 13:21:42  phicharp
// Add typedefs and methods to IAssociator(s)
//
// Revision 1.2  2002/04/25 08:44:03  ibelyaev
//  bug fix for Win2K
//
// Revision 1.1  2002/04/08 14:26:01  ibelyaev
//  new version of 'Relations'-subpackage
//
// Revision 1.1  2002/04/03 15:35:17  ibelyaev
// essential update and redesing of all 'Relations' stuff
//
// ============================================================================
#ifndef RELATIONS_IAssociator_H 
#define RELATIONS_IAssociator_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Relations 
#include "Relations/IRelation.h"
// forward decalrations 
class    IAlgorithm ;   // from GaudiKernel 

/** @class IAssociator IAssociator.h Relations/IAssociator.h
 *  
 *  Generic interface (tool) to access the relations from Transient Store 
 *
 *  @see IRelation
 *  @see IRelation2D
 *  @see IAssociatorWeighted
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep
 *  @date   24/03/2002
 */

template<class FROM,class TO>
class IAssociator:
  public virtual IAlgTool
{
public:
  
  /// shorcut for "own"  type 
  typedef IAssociator<FROM,TO>    OwnType       ;
  /// shortcut for "direct relations
  typedef IRelation<FROM,TO>      DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelation<TO,FROM>      InverseType   ;

  /// another set of shortcuts which are may be more explicit (PhC)
  typedef OwnType                             IAsct;
  typedef DirectType                          Table;
  typedef Table::Range                        ToRange;
  typedef Table::iterator                     ToIterator;
  typedef InverseType                         InvTable;
  typedef InvTable::Range                     FromRange;
  typedef InvTable::iterator                  FromIterator;

public:

  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)  (non const version)
   *  @see IRelation
   *  @return pointer to "direct" relation table 
   */
  virtual       DirectType*   direct    ()       = 0 ;
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (const version)
   *  @see IRelation
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct    () const = 0 ;

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (non const version)
   *  @see IRelation
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse   ()       = 0 ;
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (const version)
   *  @see IRelation
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType*  inverse   () const = 0 ;
  
  /** accessor to relation builder algoritthm
   *  @see IAlgorithm
   *  @return pointer to builder algorithm
   */
  virtual IAlgorithm*         algorithm () const = 0 ;

  /** interface identification (static)
   *  @attention the unique interface identifier is constructed "on-fly"
   *  using hash-technique from the generic interface name and 
   *  unique identifiers of related objects 
   *  @see IInterface
   *  @return the uniqie interface identifier
   */
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID s_iid  =
      Relations::interfaceID( "IAssociator"                     ,
                              DirectType::FromTypeTraits::id () ,
                              DirectType::ToTypeTraits::id   () , 
                              0                                 ,
                              DirectType::TypeTraits::  version , 0 );
    return s_iid ;
  };

  /** Method to retrieve a range associated to a given FROM element

      @param FROM*& Pointer to a FROM object one want to retrieve associated 
      range
      @param ToRange& A range of associated objects. 
      It is empty if no table was found
      @return StatusCode Failure it no table was found

   */
  StatusCode range( const FROM* from, ToRange& range)
  {
    Table* table = direct();
    if( 0 == table ) {
      range = ToRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( from );
    return StatusCode::SUCCESS;
  }

  /** Method to retrieve a range associated to a given TO element

      @param TO*& Pointer to a TO object one want to retrieve associated range
      @param FromRange& A range of associated objects. 
      It is empty if no table was found
      @return StatusCode Failure it no table was found

   */
  StatusCode invRange( const TO* to, FromRange& range)
  {
    InvTable* table = inverse();
    if( 0 == table ) {
      range = InvTable::Range() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( to );
    return StatusCode::SUCCESS;
  }
  
  /** Method to retrieve a range associated to a given FROM element

      @param from Pointer to a FROM object one want to retrieve associated 
      range
      @return A range of associated objects. 
      It is empty if no table was found

   */
  ToRange range( const FROM* from )
  {
    Table* table = direct();
    if (0 == table) {
      return ToRange();
    }
    return table->relations( from );
  }

  /** Method to retrieve a range associated to a given TO element

      @param TO*& Pointer to a TO object one want to retrieve associated range
      @return FromRange& A range of associated objects. 
      It is empty if no table was found

   */
  FromRange invRange( const TO* to )
  {
    InvTable* table = inverse();
    if( 0 == table ) {
      return FromRange() ;
    }
    return table->relations( to );
  }
  
  /* Method to test if the table does exist or not
   */
  bool tableExists()
  {
    return 0 == direct() ? false : true ;
  }

protected:
  
  /// destructor (virtual and protected) 
  virtual ~IAssociator(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IAssociator_H
// ============================================================================
