// $Id: VeloHit.cpp,v 1.1.1.1 2002-12-12 14:42:12 witoldp Exp $
// Include files

// local
#include "VeloHit.h"

G4Allocator<VeloHit> VeloHitAllocator;


void* VeloHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) VeloHitAllocator.MallocSingle();
  return aHit;
}

void VeloHit::operator delete(void *aHit)
{
  VeloHitAllocator.FreeSingle((VeloHit*) aHit);
}


//-----------------------------------------------------------------------------
// Implementation file for class : VeloHit
//
// 2002-08-16 : Witold Pokorski
//-----------------------------------------------------------------------------

VeloHit::VeloHit( ) : m_sensor(0) {;}
VeloHit::~VeloHit( ){;}

//=============================================================================
