// $Id: RelationBase.h,v 1.2 2002-04-03 15:35:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_RELATIONBASE_H 
#define RELATIONS_RELATIONBASE_H 1
// Include files
// STD & STL 
#include <algorithm>
#include <functional>
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StatusCode.h"
// Relation
#include "Relations/RelationTypeTraits.h"
#include "Relations/Reserve.h"

namespace Relations
{
  /** @class RelationBase RelationBase.h Relations/RelationBase.h
   *  
   *  Implementation of ordinary unidirectional relation 
   *
   *  @warning for the current implementation the actual type of 
   *  "FROM" should differ from the actual type of "TO"
   *  
   *  @warning users should not deal with this class directly!
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   27/01/2002
   */  
  template<class FROM,class TO>
  class RelationBase
  {  
  public:
    
    /// short cut to own type 
    typedef RelationBase<FROM,TO>                  OwnType     ;
    /// short cut to type of inverse relations 
    typedef RelationBase<TO,FROM>                  InvType     ;
    /// shortcut to type traits structure 
    typedef Relations::RelationTypeTraits<FROM,TO> TypeTraits  ;
    /// actual "FROM" type 
    typedef TypeTraits::From                       From        ;
    /// actual "TO"   type 
    typedef TypeTraits::To                         To          ;  
    /// Entry type  
    typedef TypeTraits::Entry                      Entry       ;
    /// container  type  
    typedef TypeTraits::Entries                    Entries     ;
    /// iterator type (internal)
    typedef Entries::iterator                      iterator    ;
    /// iterator type (internal)   
    typedef TypeTraits::IT                         IT          ;
    /// iterator pair type type (internal)   
    typedef TypeTraits::IP                         IP          ;
    /// const iterator type (internal)   
    typedef TypeTraits::CIT                        CIT         ;
    /// size_type 
    typedef Entries::size_type                     size_type   ;
    
  protected: 
    
    /// comparison criteria for sorting 
    typedef TypeTraits::Less                       Less        ;
    /// comparion criteria ( "less" by "From" field )
    typedef TypeTraits::LessByFrom                 Less1       ;
    /// equality criteria  ( "equal" by "To" field ) 
    typedef TypeTraits::EqualByTo                  Equal       ;
    
  public:
    
    /** retrive all relations 
     *
     *  @see IRelation 
     *  @param  object  smart reference to the object
     *  @return the pair of iterators for output relations   
     */
    inline  IP    i_relations () const 
    { return IP( m_entries.begin() , m_entries.end() ) ;};
    
    /** retrive all relations from the object
     *
     *   - the CPU performance is proportional to log(N), 
     *     where N is the total number of relations
     *
     *  @see IRelation 
     *  @param  object  smart reference to the object
     *  @return the pair of iterators for output relations   
     */
    inline  IP    i_relations
    ( const From& object      ) const
    { return std::equal_range( m_entries.begin() , 
                               m_entries.end  () , 
                               Entry ( object  ) , 
                               Less1          () ) ;};
    
    /** make the relation between 2 objects
     * 
     *   - StatusCode::FAILURE is returned if the relation
     *     between the given objects  is already set
     *
     *   - the CPU performance is proportional to log(N)
     *     for location of the object plus some overhead for 
     *     list operation, which is more or less constant for 
     *     std::deque implementation of the underlying relation 
     *     store and proportional to N for std::vector implementation, 
     *     where N is the total number of relations 
     *
     *  @see IRelation 
     *  @see RelationTypeTraits
     *  @param object1  the first object
     *  @param object2  the second object 
     *  @return status code 
     */
    inline  StatusCode i_relate 
    ( const From&      object1 , 
      const To&        object2 ) 
    {
      // look for existing relations 
      const Entry ent ( object1 , object2 ) ;
      Less  cmp ;
      iterator it = 
        std::lower_bound( m_entries.begin () , m_entries.end () , ent , cmp ) ;
      // the relation does exist ! 
      if( m_entries.end () != it && !cmp( ent , *it ) ) 
        { return StatusCode::FAILURE; }
      // insert new relation !
      m_entries.insert( it , ent ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** remove the concrete relation between objects
     *
     *   - StatusCode::FAILURE is returned if the relation
     *     between the given objects  is already set
     *
     *   - the CPU performance is proportional to log(N)
     *     for location of the object plus some overhead for 
     *     list operations, which is more or less constant for 
     *     std::deque (or std::list) implementation of the 
     *     underlying relation store and proportional to N
     *     for std::vector implementation, where N is the 
     *     total number of relations 
     *
     *  @see IRelation 
     *  @see RelationTypeTraits
     *  @param object1  the first object
     *  @param object2  to the second object 
     *  @return status code 
     */
    inline  StatusCode i_remove 
    ( const From&      object1 , 
      const To&        object2 ) 
    {
      // look for existing relations 
      const Entry ent ( object1 , object2 );
      Less  cmp ;
      iterator it  = 
        std::lower_bound( m_entries.begin() , m_entries.end() , ent , cmp ) ;
      // the relation does not exist ! 
      if( m_entries.end() == it || cmp( ent , *it ) ) 
        { return StatusCode::FAILURE ; }
      // remove existing relation     
      m_entries.erase( it );
      return StatusCode::SUCCESS ; 
    };
    
    /** remove all relations FROM the defined object
     *
     *   - StatusCode::FAILURE is returned if there are no relations
     *     from the given object
     *
     *   - the CPU performance is proportional to log(N)
     *     for location of the object plus some overhead for 
     *     list operations, which is more or less constant for 
     *     std::deque (or std::list) implementation of the 
     *     underlying relation store and proportional to N
     *     for std::vector implementation, where N is the 
     *     total number of relations 
     *
     *  @see    IRelation 
     *  @param  object  to the object
     *  @return status code 
     */
    inline  StatusCode i_removeFrom 
    ( const From&      object  )
    {
      // look for all existing relations from the given object  
      IP ip  = i_relations( object ) ;
      // there are no relations 
      if( ip.second == ip.first ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase relations 
      m_entries.erase( ip.first , ip.second ) ;
      return StatusCode::SUCCESS ;
    };
    
    /** remove all relations TO the defined object
     *  @attention the method is relatively "slow"
     *
     *   - StatusCode::FAILURE is returned if there are no relations
     *     from the given object
     *
     *   - the CPU performance is proportional to 
     *     the total number of relations 
     *
     *  @see    IRelation 
     *  @param  object the object
     *  @return status code 
     */
    inline  StatusCode i_removeTo 
    ( const To&        object  ) 
    {
      // use the predicate "Equal"
      iterator it = 
        std::remove_if( m_entries.begin () , 
                        m_entries.end   () , 
                        std::bind2nd( Equal() , Entry( From() , object ) ) ) ;
      // no relations are found!
      if( m_entries.end() == it ) { return StatusCode::FAILURE ; }// RETURN !!!
      // erase the relations 
      m_entries.erase( it , m_entries.end() ) ;
      return StatusCode::SUCCESS ;
    };

    /** remove ALL relations from ALL  object to ALL objects
     *
     *  @see    IRelation 
     *  @param  object the object
     *  @return status code 
     */
    inline  StatusCode i_clear() 
    { m_entries.clear() ; return StatusCode::SUCCESS ; }
    
    /// standard/default constructor 
    RelationBase( const size_type reserve = 0 ) 
      : m_entries() 
    { if( reserve ) { Relations::reserve( m_entries , reserve ) ; } ; };
    
    /// destructor (virtual)
    virtual ~RelationBase() { m_entries.clear(); }
    
    /** copy constructor
     *  @param copy object to be copied  
     */
    RelationBase( const OwnType& copy ) : m_entries( copy.m_entries ) {} ;
    
    /** constructor from the inverse type! 
     *  @attention it is indeed the effective way to get the inverse relations!
     *  @param inv object to be inversed!
     *  @param flag artificial agument to make teh difference 
     *  for Win#2 platform
     */
    RelationBase(  const InvType& inv , int /* flag */ ) : m_entries()
    {
      /// get all relations from "inv"
      InvType::IP ip = inv.i_relations() ;
      // reserve the space for relations
      m_entries.reserve( ip.second - ip.first  );
      /// invert the relations    
      for( InvType::CIT it = ip.first ; ip.second != it ; ++it ) 
        { i_relate( it->to() , it->from() ) ;  }
    };
    
  private:
    
    mutable Entries m_entries ;
    
  };
  
}; // End of name space Relations 


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATIONBASE_H
// ============================================================================
