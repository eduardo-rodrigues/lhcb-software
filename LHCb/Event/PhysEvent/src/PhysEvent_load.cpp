// $Id: PhysEvent_load.cpp,v 1.10 2003-12-18 15:33:29 cattanem Exp $

// Include files
#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Implementation file for classes in PhysEvent
//
// 28/05/2001 : Gloria Corti
//-----------------------------------------------------------------------------

/// ====================================================================
#include "Event/Particle.h"
_ImplementContainedObjectFactory( Particle )
_ImplementDataObjectFactory( Particles )

/// ===================================================================
#include "Event/Vertex.h"
_ImplementContainedObjectFactory( Vertex )
_ImplementDataObjectFactory( Vertices )

/// ===================================================================
#include "Event/PrimVertex.h"
_ImplementContainedObjectFactory( PrimVertex )
_ImplementDataObjectFactory( PrimVertices )

/// ====================================================================
#include "Event/FlavourTag.h"
_ImplementContainedObjectFactory( FlavourTag )
_ImplementDataObjectFactory( FlavourTags )

/// ====================================================================
#include "Event/PhysSel.h"
_ImplementDataObjectFactory( PhysSel );

// ===================================================================
//#include "Event/AxPartCandidate.h"
//_ImplementContainedObjectFactory( AxPartCandidate )
//_ImplementDataObjectFactory( AxPartCandidates )

// ===================================================================
#include "Event/ProtoParticle.h"
_ImplementContainedObjectFactory( ProtoParticle )
_ImplementDataObjectFactory( ProtoParticles )

// ===================================================================
#include "Event/V0.h"
_ImplementContainedObjectFactory( V0 )
_ImplementDataObjectFactory( V0s )

// ====================================================================
void PhysEvent_load()  {

  // Declaration of data object factories
  DLL_DECL_OBJECTFACTORY  ( PhysSel );

  // Declaration of contained object factories
  DLL_DECL_OBJECTFACTORY( Particle );
  DLL_DECL_OBJECTFACTORY( Particles );

  DLL_DECL_OBJECTFACTORY( Vertex );
  DLL_DECL_OBJECTFACTORY( Vertices );
  
  DLL_DECL_OBJECTFACTORY( PrimVertex );
  DLL_DECL_OBJECTFACTORY( PrimVertices );
  
  DLL_DECL_OBJECTFACTORY( FlavourTag );
  DLL_DECL_OBJECTFACTORY( FlavourTags );
  
  //DLL_DECL_OBJECTFACTORY( AxPartCandidate );
  //DLL_DECL_OBJECTFACTORY( AxPartCandidates );

  DLL_DECL_OBJECTFACTORY( ProtoParticle );
  DLL_DECL_OBJECTFACTORY( ProtoParticles );

  DLL_DECL_OBJECTFACTORY( V0 );
  DLL_DECL_OBJECTFACTORY( V0s );
}

extern "C" void PhysEvent_loadRef()  {
  PhysEvent_load();
}

