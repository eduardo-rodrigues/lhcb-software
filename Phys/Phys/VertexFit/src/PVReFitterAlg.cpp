// $Id: PVReFitterAlg.cpp,v 1.12 2008-11-20 15:26:25 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
//#include "GaudiKernel/ObjectVector.h"
// LHCb
#include <Event/RecVertex.h>
#include <Event/Particle.h>
#include <Event/Track.h>
#include "Kernel/IPVReFitter.h"
#include "Kernel/ILifetimeFitter.h"
#include "TrackInterfaces/IPVOfflineTool.h"
// local
#include "PVReFitterAlg.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PVReFitterAlg
//
// 2008-06-25 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PVReFitterAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVReFitterAlg::PVReFitterAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_pvOfflineTool(0),
  m_pvReFitter(0),
  m_pvOfflinetoolType("PVOfflineTool"),
  m_pvReFitterType("AdaptivePVReFitter"),
  m_particleInputLocation(""),
  m_PVInputLocation(LHCb::RecVertexLocation::Primary),
  m_particle2VertexRelationsOutputLocation(""),
  m_vertexOutputLocation("")

{

  declareProperty("IPVOfflineTool", m_pvOfflinetoolType);
  declareProperty("IPVReFitter",    m_pvReFitterType);
  declareProperty("ParticleInputLocation",  m_particleInputLocation);
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
  declareProperty("P2VRelationsOutputLocation",  
                  m_particle2VertexRelationsOutputLocation);
  declareProperty("VertexOutputLocation", m_vertexOutputLocation);

}
//=============================================================================
// Destructor
//=============================================================================
PVReFitterAlg::~PVReFitterAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVReFitterAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by DVAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_pvOfflineTool = tool<IPVOfflineTool> (m_pvOfflinetoolType, this);

  m_pvReFitter = tool<IPVReFitter>(m_pvReFitterType, this);

  return ( m_pvOfflineTool && 
           m_pvReFitter       )
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVReFitterAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (!exist<LHCb::Particle::Container>(m_particleInputLocation)) {
    return Error("No LHCb::Particle::Container found at " + 
                 m_particleInputLocation,
                 StatusCode::SUCCESS);
  }
  if (!exist<LHCb::RecVertex::Container>(m_PVInputLocation)) {
    return Error("No LHCb::RecVertex::Container found at " +
                 m_PVInputLocation,
                 StatusCode::SUCCESS);
  }

  LHCb::Particle::Container* particles = 
    get<LHCb::Particle::Container>(m_particleInputLocation);

  if (0==particles) {
    return Error("No LHCb::Particles in LHCb::Particle::Container " +
                 m_particleInputLocation,
                 StatusCode::SUCCESS);
  }

  LHCb::RecVertex::Container* vertices = 
    get<LHCb::RecVertex::Container>(m_PVInputLocation);

  if (0==vertices) {
    return Error("No LHCb::RecVertices in LHCb::Particle::Container "+
                 m_PVInputLocation,
                 StatusCode::SUCCESS);
  }
  
  LHCb::RecVertex::Container* vertexContainer = 
    new  LHCb::RecVertex::Container();

  verbose() << "Storing re-fitted vertices in " 
            << m_vertexOutputLocation << endmsg;

  put(vertexContainer, m_vertexOutputLocation);  

  Particle2Vertex::Table* newTable = new Particle2Vertex::Table();

  verbose() << "Storing Particle->Refitted Vtx relations table in "
            << m_particle2VertexRelationsOutputLocation << endmsg;

  put( newTable, m_particle2VertexRelationsOutputLocation);

  verbose() << "Loop over " << particles->size() << " particles" << endmsg;

  for ( LHCb::Particle::Container::const_iterator itP = particles->begin();
        itP != particles->end(); 
        ++itP) {
    // need to add an incremental weight to keep the ordering as
    // in the standard PV TES
    double weight(1.);
    
    for (LHCb::RecVertex::Container::const_iterator itPV = vertices->begin();
         itPV != vertices->end();
         ++itPV) {
      
      verbose() << "Re-fitting vertex for particle\n" 
                << *(*itP) << endmsg;
      LHCb::RecVertex* refittedVertex = refitVertex(*itPV, *itP);
      if (0!=refittedVertex) {
        verbose() << "Storing vertex with key " << refittedVertex->key()
                  << " into container slot with key " << (*itPV)->key() 
                  << endmsg;
        vertexContainer->insert(refittedVertex);
        newTable->relate(*itP, refittedVertex, weight);
        weight += 0.1;
        verbose() << "Re-fitted vertex " << *(*itPV) << "\n as\n"
                  << *refittedVertex << endmsg;
      }
    }
  }
  
  setFilterPassed(true);

  if (exist<LHCb::RecVertex::Container>(m_vertexOutputLocation) )
  {  
    const LHCb::RecVertex::Container* vertices = 
      get<LHCb::RecVertex::Container>(m_vertexOutputLocation);
    verbose() << "CHECK: stored " 
              << vertices->size()
              << " re-fitted vertices in " 
              << m_vertexOutputLocation << endmsg;
  } else {
    return Error("No re-fitted vertices at "+
                 m_vertexOutputLocation,
                 StatusCode::SUCCESS);
  }


  if (exist<Particle2Vertex::Table>(m_particle2VertexRelationsOutputLocation) )
  {  
    verbose() << "CHECK: table is at " 
              << m_particle2VertexRelationsOutputLocation << endmsg;
  } else {
    return Error("No LHCb::Particle->LHCb::RecVertex table found at "+
                 m_particle2VertexRelationsOutputLocation,
                 StatusCode::SUCCESS);
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::RecVertex* PVReFitterAlg::refitVertex(const LHCb::RecVertex* v,
                                            const LHCb::Particle* p  ) const
{

  LHCb::RecVertex* reFittedVertex = new LHCb::RecVertex();  

  LHCb::Track::ConstVector tracks;
  
  getTracks(p, tracks);

  StatusCode sc = m_pvOfflineTool->reDoSinglePV(v->position(), 
                                                tracks, 
                                                *reFittedVertex );

  if (sc==StatusCode::SUCCESS) {
    sc = m_pvReFitter->remove(p, reFittedVertex);
  }
  
  return (sc==StatusCode::SUCCESS) ? reFittedVertex : 0;
}
//=============================================================================
void PVReFitterAlg::getTracks(const LHCb::Particle* p,
                              LHCb::Track::ConstVector& tracks) const
{
  const LHCb::ProtoParticle*   proto  = p->proto() ;

  if( proto ) {

    const LHCb::Track* track = proto->track();
    if(track) tracks.push_back(track);

  }
 
  const SmartRefVector< LHCb::Particle >& Prods = p->daughters();
  SmartRefVector< LHCb::Particle >::const_iterator iProd;

  for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
    const LHCb::Particle* daughter = *iProd;
    getTracks(daughter, tracks);      
  }

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PVReFitterAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
