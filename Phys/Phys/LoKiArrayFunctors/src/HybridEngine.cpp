// $Id: HybridEngine.cpp,v 1.4 2010-05-27 19:29:55 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/HybridEngine.h"
#include "LoKi/HybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::Engine
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
 *  @date 2004-06-30 
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::Engine::Engine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::Engine::~Engine() {}
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::EngineActor& actor = LoKi::Hybrid::EngineActor::instance() ;
    return actor.process ( name , cut ) ;
  } 
  // ==========================================================================
}
// ============================================================================
// predicates:
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process 
( const std::string&          name , 
  const LoKi::Types::Cuts&    cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::VCuts&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::ACuts&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::PPCuts&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// functions 
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::Func&    cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::VFunc&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::AFunc&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::PPFunc&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// Maps 
// ============================================================================
// add the map
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::Maps&    cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the map
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::VMaps&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the map
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::PPMaps&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// Pipes
// ============================================================================
// add the pipe
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::Pipes&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the pipe 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::VPipes&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the pipe 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::PPPipes& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// Fun-Vals
// ============================================================================
// add the fun-val
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::FunVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the fun-val
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&           name , 
  const LoKi::Types::VFunVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the fun-val
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&            name , 
  const LoKi::Types::PPFunVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// cut-Vals
// ============================================================================
// add the cut-val
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::CutVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut-val
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&           name , 
  const LoKi::Types::VCutVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut-val
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&            name , 
  const LoKi::Types::PPCutVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// Elements
// ============================================================================
// add the element
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&           name , 
  const LoKi::Types::Elements& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the element
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&            name , 
  const LoKi::Types::VElements& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the element
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&             name , 
  const LoKi::Types::PPElements& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// Sources
// ============================================================================
// add the source
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::Sources& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the source
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&           name , 
  const LoKi::Types::VSources& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the source
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&            name , 
  const LoKi::Types::PPSources& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
