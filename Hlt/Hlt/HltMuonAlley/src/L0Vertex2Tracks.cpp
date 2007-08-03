// $Id: L0Vertex2Tracks.cpp,v 1.1 2007-08-03 09:29:30 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "L0Vertex2Tracks.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : L0Vertex2Tracks
//
// 2007-08-03 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Vertex2Tracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Vertex2Tracks::L0Vertex2Tracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{




}
//=============================================================================
// Destructor
//=============================================================================
L0Vertex2Tracks::~L0Vertex2Tracks() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Vertex2Tracks::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Vertex2Tracks::execute() {

  debug() << "==> Execute" << endmsg;


  for(  Hlt::VertexContainer::const_iterator L0dimuon=
          m_inputVertices->begin();
        L0dimuon<m_inputVertices->end();L0dimuon++){
    SmartRefVector< LHCb::Track >  vertex_tracks=       (*L0dimuon)->tracks();
    //info()<<" vertex size "<<vertex_tracks.size()<<endreq;
    
    if(vertex_tracks.size()!=2)return StatusCode::FAILURE;
    if(m_debug){
      info()<<"both L0 muons exist in input vertex "<<endreq;
       
    }
    Track* firstMuonInL0=vertex_tracks[0];
    Track* secondMuonInL0=vertex_tracks[1];
    bool already=isIn(firstMuonInL0);
    if(!already) m_outputTracks->push_back(firstMuonInL0);
    already=false;
    already=isIn(secondMuonInL0);
    if(!already) m_outputTracks->push_back(secondMuonInL0);
  }
  
  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Vertex2Tracks::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
bool L0Vertex2Tracks::isIn(Track* track)
{
  Hlt::TrackContainer::const_iterator it;
  for(it=m_outputTracks->begin();it<m_outputTracks->end();it++){
    if(track==*it)return true;
  
  }
  return false;
  
}
