// $Id: Tokens.h,v 1.1.1.1 2006-01-24 09:39:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_TOKENS_H 
#define LOKI_TOKENS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <vector>
#include <algorithm>
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  /// useful typedef 
  typedef std::vector<std::string> Tokens;
  
  /** @class Tokens Tokens.h LoKi/Tokens.h
   *   
   *  Very simple tokenizer for LoKi 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-14
   */
  
  inline size_t tokenize
  ( const std::string& value            , 
    Tokens&            tokens           , 
    const std::string& separators = " " ) 
  {
    // reset the existing tokens 
    tokens.clear();
    if ( value       .empty () ) { return tokens.size () ; }        
    std::string::const_iterator it1 = value.begin() ;
    std::string::const_iterator it2 = value.begin() ;
    while ( value.end() != it1 && value.end() != it2 ) 
    {
      it2 = std::find_first_of( it1                  , 
                                value.end         () ,
                                separators.begin  () ,
                                separators.end    () ) ;
      if ( it2 != it1 ) 
      {
        std::string aux( value , it1 - value.begin() , it2 - it1 ) ;
        tokens.push_back( aux ) ;
        it1  = it2 ;
      }
      else { ++it1 ; }
      
    }
    return tokens.size();
  };
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TOKENS_H
// ============================================================================
