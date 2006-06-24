// $Id: Welcome.cpp,v 1.4 2006-06-24 17:18:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <iostream>
// ============================================================================
// local
// ============================================================================
#include "LoKi/Welcome.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================


// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Welcome 
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
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/// Meyers's singleton
// ============================================================================
LoKi::Welcome& LoKi::Welcome::instance ( const bool Short ) 
{
  static LoKi::Welcome s_welcome = Welcome( std::cout , Short ) ;
  if ( s_welcome.m_short && !Short ) 
  { 
    s_welcome.setShort ( Short ) ; 
    s_welcome.welcome  () ; 
  }
  return s_welcome ;
};
// ============================================================================
/// standard constructor 
// ============================================================================
LoKi::Welcome::Welcome ( std::ostream& stream , 
                         const bool    Short  ) 
  : m_stream ( stream   ) 
  , m_short  ( Short    )
  , m_len1   ( 103      ) 
  , m_str1   ( "LoKi"   ) 
  , m_fmt1   ( "%|-5|"  ) 
  , m_fmt2   ( "%|-94|" ) 
  , m_fmt3   ( "%|=94|" ) 
{ 
  welcome  () ;
};
// ============================================================================
LoKi::Welcome::~Welcome() { goodbye () ; };
// ============================================================================
void LoKi::Welcome::setShort ( const bool Short ) { m_short = Short ; }
// ============================================================================

// ============================================================================
void LoKi::Welcome::welcome() const
{
  std::string m_str2 = "" ;
  while ( m_str2.size() < m_len1 ) { m_str2 += "Welcome " ; }
  m_str2 = std::string( m_str2.begin() , m_str2.begin() + m_len1 ) ;
  
  if ( m_short ) 
  {
    m_stream << std::endl << m_str2    << std::endl ;
    m_stream << boost::format ( m_fmt1 ) % m_str1 ;
    m_stream << boost::format ( m_fmt3 ) % "Welcome to LoKi!" ;
    m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
    m_stream << m_str2    << std::endl << std::endl ;
    //
    return ;                                                   // RETURN 
  }
  
  m_stream << std::endl << std::endl ; 
  m_stream << m_str2    << std::endl ; 
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "Welcome to LoKi!" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "(LOops & KInematics)" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % 
    "Smart & Friendly C++ Physics Analysis Tool Kit" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % 
    "Author:  Vanya BELYAEV (ITEP/Moscow) Ivan.Belyaev@itep.ru " ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % 
    "With the kind help of Galina Pakhlova & Sergey Barsuk " ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt2 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "Have fun and enjoy!" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt2 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  m_stream << m_str2    << std::endl ;
  
  m_stream << m_str2    << std::endl ;  
  m_stream << std::endl << std::endl ;
  
};
// ============================================================================

// ============================================================================
void LoKi::Welcome::goodbye () const 
{
  
  std::string m_str2 = "" ;
  while ( m_str2.size() < m_len1 ) { m_str2 += "Good Bye " ; }
  m_str2 = std::string ( m_str2.begin() ,  m_str2.begin() + m_len1 ) ;

  if ( m_short ) 
  {
    m_stream << std::endl << m_str2    << std::endl ;
    m_stream << boost::format ( m_fmt1 ) % m_str1 ;
    m_stream << boost::format ( m_fmt3 ) % "Good Bye from LoKi!" ;
    m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
    m_stream << m_str2    << std::endl << std::endl ;
    // 
    return ;                                                          // RETURN
  }

  m_stream << std::endl << std::endl ;
  
  m_stream << m_str2    << std::endl ;
  m_stream << m_str2    << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "Good Bye from LoKi!" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "(LOops & KInematics)" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % 
    "Smart & Friendly C++ Physics Analysis Tool Kit" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;

  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;

  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt2 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % 
    "LoKi mailing list ('LoKi-club') : lhcb-loki@cern.ch ";
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  std::string _s("Comments, suggestions, criticism, ideas, ") ;
  _s += "questions and requests are *ALWAYS* welcome";
  m_stream << boost::format ( m_fmt3 ) % _s ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << boost::format ( m_fmt1 ) % m_str1 ;
  m_stream << boost::format ( m_fmt3 ) % "" ;
  m_stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  m_stream << m_str2    << std::endl ;  
  m_stream << m_str2    << std::endl ;  
  m_stream << std::endl << std::endl ;

}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
