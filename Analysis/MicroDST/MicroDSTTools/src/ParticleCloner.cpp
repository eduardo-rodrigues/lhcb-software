// $Id: ParticleCloner.cpp,v 1.10 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/Particle.h"
#include "Event/Vertex.h"

// From MicroDST
#include <MicroDST/ICloneVertex.h>
#include <MicroDST/ICloneProtoParticle.h>

// local
#include "ParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleCloner::ParticleCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  :
  //  base_class ( type, name , parent ),
  base_class( type, name, parent),
  m_vertexCloner(NULL),
  m_vertexClonerName("VertexCloner"),
  m_ppCloner(NULL),
  m_ppClonerName("ProtoParticleCloner")
{
  declareProperty("ICloneVertex", m_vertexClonerName);
  declareProperty("ICloneProtoParticle", m_ppClonerName);
}

//=============================================================================
// Destructor
//=============================================================================
ParticleCloner::~ParticleCloner() {}

//=============================================================================

StatusCode ParticleCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "Going to initialise ICloneProtoParticle and ICloneVertex" << endmsg;

  m_vertexCloner = ( m_vertexClonerName != "NONE" ?
                     tool<ICloneVertex>(m_vertexClonerName, this->parent() ) : NULL );

  m_ppCloner = ( m_ppClonerName != "NONE"  ?
                 tool<ICloneProtoParticle>(m_ppClonerName, this->parent() ) : NULL );

  if (m_ppCloner) 
  {
    verbose() << "Found ICloneProtoParticle " << m_ppClonerName << endmsg;
  }
  else
  {
    verbose() << "Did not find ICloneProtoParticle " << m_ppClonerName
              << ". ProtoParticle cloning de-activated."<< endmsg;
  }

  return sc;
}

//=============================================================================

LHCb::Particle* ParticleCloner::operator() (const LHCb::Particle* particle)
{
  return this->clone(particle);
}

//=============================================================================

LHCb::Particle* ParticleCloner::clone(const LHCb::Particle* particle)
{
  if ( !particle ) return NULL;

  LHCb::Particle* particleClone =
    cloneKeyedContainerItem<BasicParticleCloner>(particle);

  if ( !particleClone ) return NULL;

  if ( m_vertexCloner )
  {
    particleClone->setEndVertex( (*m_vertexCloner)(particle->endVertex()) );
  }
  else
  {
    particleClone->setEndVertex(SmartRef<LHCb::Vertex>(particle->endVertex()));
  }

  storeDaughters( particleClone, particle->daughters() );

  if ( m_ppCloner )
  {
    particleClone->setProto( (*m_ppCloner)( particle->proto() ) );
  }

  return particleClone;
}

//=============================================================================

void ParticleCloner::storeDaughters(LHCb::Particle* particleClone,
                                    const SmartRefVector<LHCb::Particle>& daughters)
{
  particleClone->clearDaughters();

  for ( SmartRefVector<LHCb::Particle>::const_iterator dau = daughters.begin();
        dau != daughters.end(); ++dau )
  {
    LHCb::Particle* daughterClone = this->clone( *dau );
    if (daughterClone) particleClone->addToDaughters(daughterClone);
  }
}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleCloner )
