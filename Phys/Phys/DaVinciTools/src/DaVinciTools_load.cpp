// $Id: DaVinciTools_load.cpp,v 1.59 2006-03-15 13:40:12 pkoppenb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciTools ) {

  // Miscellaneous
  DECLARE_TOOL( Algorithm2ID ) ;
  DECLARE_TOOL( CheckOverlap );
  /// @todo: Restore CheckVeloOverlap
  //  DECLARE_TOOL( CheckVeloOverlap );
  /// @todo: Get DecayFinder to compile (need stl expert), use tag v13r0df to restore it 
  //  DECLARE_TOOL( DecayFinder ); /     
  DECLARE_TOOL( DecodeSimpleDecayString );
  DECLARE_TOOL( OnOfflineTool ) ;
  DECLARE_TOOL( ParticleDescendants ) ;
  DECLARE_TOOL( RecursivePlotTool ) ;
  DECLARE_TOOL( SimplePlotTool ) ;

  // Algorithms
  DECLARE_ALGORITHM( CheckPV );  
  DECLARE_ALGORITHM( MakeResonances ) ;
  DECLARE_ALGORITHM( SimplePlots );  
  
}

