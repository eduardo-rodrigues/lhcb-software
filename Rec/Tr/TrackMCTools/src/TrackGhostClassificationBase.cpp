// $Id: TrackGhostClassificationBase.cpp,v 1.1 2007-05-15 13:04:17 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"


#include "TrackGhostClassificationBase.h"
#include "TsaKernel/ILHCbIDsToMCParticles.h"

#include "Event/Track.h"


DECLARE_TOOL_FACTORY(TrackGhostClassificationBase)

using namespace LHCb;

TrackGhostClassificationBase::TrackGhostClassificationBase(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){

  declareProperty("purityCut", m_purityCut = 0.7);

  // constructer
  declareInterface<ITrackGhostClassification>(this);
};

TrackGhostClassificationBase::~TrackGhostClassificationBase(){
  // destructer
}

StatusCode TrackGhostClassificationBase::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  m_linkTool = tool<ILHCbIDsToMCParticles>("LHCbIDsToMCParticles");

  return StatusCode::SUCCESS;
}


void TrackGhostClassificationBase::info(const LHCb::Track& aTrack, 
                                        LHCb::GhostTrackInfo& tinfo) const{
  generic(aTrack,tinfo);
  specific(aTrack,tinfo);
  return;
}

void TrackGhostClassificationBase::generic(const LHCb::Track& aTrack, 
                                           LHCb::GhostTrackInfo& tinfo) const{

  LHCbIDs::const_iterator start = aTrack.lhcbIDs().begin();
  LHCbIDs::const_iterator stop = aTrack.lhcbIDs().end();
  generic(start,stop,tinfo);

  if (tinfo.classification() == LHCb::GhostTrackInfo::None){

    // we still have some work to do

    // parent was a ghost
    if (ghostParent(aTrack) == true){
     tinfo.setClassification(GhostTrackInfo::GhostParent); 
     return;
   }
  }
  return;

};

void TrackGhostClassificationBase::specific(const LHCb::Track& aTrack, 
                                            LHCb::GhostTrackInfo& tinfo) const{
 
  // default behaviour call the id version 
  if (tinfo.classification() == LHCb::GhostTrackInfo::None){
    // we still have some work to do !
    LHCbIDs::const_iterator start = aTrack.lhcbIDs().begin();
    LHCbIDs::const_iterator stop = aTrack.lhcbIDs().end();
    specific(start,stop,tinfo);
  }   // if
  return;
}


void TrackGhostClassificationBase::info(LHCbIDs::const_iterator& start, 
                                        LHCbIDs::const_iterator& stop, 
                                        LHCb::GhostTrackInfo& tinfo) const{

  generic(start,stop,tinfo);
  specific(start,stop,tinfo);
  return;
}

void TrackGhostClassificationBase::generic(LHCbIDs::const_iterator& start, 
                                           LHCbIDs::const_iterator& stop, 
                                           LHCb::GhostTrackInfo& tinfo) const{

    // get the information on the track
  ILHCbIDsToMCParticles::LinkMap testMap;
  linkTool()->link(start,stop,testMap);  

  // set the link map
  tinfo.setLinkMap(testMap);

  // get the bestmatch
  LHCb::GhostTrackInfo::LinkPair bestPair = tinfo.bestLink();

  // test the generic cases...

  // spillover
  if (spillover(bestPair) == true){
    tinfo.setClassification(GhostTrackInfo::SpilloverAndNoise); 
    return;
  }

  // second best match
  LHCb::GhostTrackInfo::LinkPair secondBest = this->secondBest(tinfo,bestPair.first);

  // check for decay in flight
  if (decayInFlight(bestPair,secondBest) == true){
    tinfo.setClassification(GhostTrackInfo::DecayInFlight); 
    return;
  }

  // conversion
  if (conversion(bestPair,secondBest) == true){
    tinfo.setClassification(GhostTrackInfo::Conversion); 
    return;
  }

  // electromagnetic shower
  if (emShower(tinfo) == true){
    tinfo.setClassification(GhostTrackInfo::EM); 
    return;
  }

  // interaction
  if (interaction(tinfo) == true){
    tinfo.setClassification(GhostTrackInfo::HadronicInteraction); 
    return;
  }

  // phi...
  if (fromPhi(bestPair,secondBest) == true){
    tinfo.setClassification(GhostTrackInfo::FromPhi);
    return;
  }
 
  return;
}

void TrackGhostClassificationBase::specific(LHCbIDs::const_iterator& , 
                                            LHCbIDs::const_iterator& , 
                                            LHCb::GhostTrackInfo& ) const{

  //nothing specific for the base class
  return;
}


bool TrackGhostClassificationBase::spillover(const LHCb::GhostTrackInfo::LinkPair& aPair) const{
  // is it spillover
  return (aPair.first  == 0 ? true: false);
}


bool TrackGhostClassificationBase::ghostParent(const LHCb::Track& aTrack) const{

  // is it a ghost parent ?
  const SmartRefVector<LHCb::Track>& parents = aTrack.ancestors();
 
  if (parents.empty() == true) return false;

  // take the first track in the list as the parent
  ILHCbIDsToMCParticles::LinkMap testMap;  
  const Track& pTrack = *(parents.front());
  m_linkTool->link(pTrack,testMap);

  // see if it is a ghost
  return (isGhost(testMap));
}

bool TrackGhostClassificationBase::isGhost(const ILHCbIDsToMCParticles::LinkMap& lMap) const{

  // is it a ghost ?
  ILHCbIDsToMCParticles::LinkMap::const_iterator iterMap = lMap.begin();
  double tot = 0.0;
  ILHCbIDsToMCParticles::LinkPair best(0,0);
  for (; iterMap != lMap.end(); ++iterMap){
    if ( iterMap->second > best.second ){
      best = *iterMap;
    }
    tot+= iterMap->second;
  } // iterMap
  
  double purity = double(best.second)/tot;

  return (purity < m_purityCut ? true : false);
}


LHCb::GhostTrackInfo::LinkPair TrackGhostClassificationBase::secondBest(const LHCb::GhostTrackInfo& tinfo,
                                                                        const MCParticle* bestPart) const{

  GhostTrackInfo::LinkMap::const_iterator iterMap = tinfo.linkMap().begin();
  double tot = 0.0;
  GhostTrackInfo::LinkPair best(0,0);
  for (; iterMap != tinfo.linkMap().end(); ++iterMap){
    if ( iterMap->second > best.second && iterMap->first != bestPart ){
      best = *iterMap;
    }
    tot+= iterMap->second;
  } // iterMap
  
  double purity = double(best.second)/tot;
  return std::make_pair(best.first,purity);
} 


bool TrackGhostClassificationBase::decayInFlight(LHCb::GhostTrackInfo::LinkPair& p1,
                                                  LHCb::GhostTrackInfo::LinkPair& p2 ) const{

  bool decay = false;  
  if (p1.second + p2.second > purityCut() && p1.first !=0 && p2.first !=0){
    MCParticle* daughter = 0;  MCParticle* mother = 0;
    if (p1.first == p2.first->mother()){
      daughter = p2.first;
      mother = p1.first;
    }
    else if (p2.first == p1.first->mother()) {
      daughter = p1.first;
      mother = p2.first;
    }
    if (daughter != 0 && daughter->particleID().abspid() == 13 && 
	(mother->particleID().abspid() == 211  ||mother->particleID().abspid() == 321 )) decay = true;    
  }
  return decay;
}

bool TrackGhostClassificationBase::conversion(LHCb::GhostTrackInfo::LinkPair& p1,
                                              LHCb::GhostTrackInfo::LinkPair& p2 ) const{

  bool con = false;  
  if (p1.first != 0 && p2.first != 0 && (p1.second + p2.second > purityCut()) &&
      p1.first->mother() ==  p2.first->mother() && p1.first->mother() !=0
      && p1.first->mother()->particleID().abspid() == 22) con = true; 

  return con;  
}

bool TrackGhostClassificationBase::fromPhi(LHCb::GhostTrackInfo::LinkPair& p1,
                                           LHCb::GhostTrackInfo::LinkPair& p2) const{

  const MCParticle* m1 = 0; const MCParticle* m2 = 0;
  if (p1.first != 0) m1 = p1.first->mother();
  if (p2.first != 0 )m2 = p2.first->mother();
  bool phi = false;
  if ((m1 !=0 && m1->particleID().abspid() == 333 ) || (m1 !=0 && m1->particleID().abspid() == 333)) phi = true;
  return phi; 
}

bool TrackGhostClassificationBase::interaction(const LHCb::GhostTrackInfo& tinfo) const{

  unsigned int nHadronic = 0;
  LHCb::GhostTrackInfo::LinkMap::const_iterator iter = tinfo.linkMap().begin();
  for (;iter != tinfo.linkMap().end() ; ++iter){

    if (iter->first == 0) continue; // skip noise

    const MCVertex* origin = iter->first->originVertex();
    if (origin != 0 && origin->type() == MCVertex::HadronicInteraction ){
      ++nHadronic;
    }    
  }

  return (nHadronic > 1 ? true : false);

}

bool TrackGhostClassificationBase::emShower(const LHCb::GhostTrackInfo& tinfo) const{

  double emComponent = 0.0;  double tot = 0.0;
  LHCb::GhostTrackInfo::LinkMap::const_iterator iter = tinfo.linkMap().begin();
  for (;iter != tinfo.linkMap().end() ; ++iter){
    if (iter->first != 0 && iter->first->particleID().abspid() == 11){
      emComponent += iter->second; 
      
    }
    tot += iter->second;
  } // iter

  return (emComponent/tot > purityCut() ? true : false) ;  
}


LHCb::GhostTrackInfo::LinkPair TrackGhostClassificationBase::bestPair(const LHCb::GhostTrackInfo& tinfo) const{

  return bestPair(tinfo.linkMap()); 
}

LHCb::GhostTrackInfo::LinkPair TrackGhostClassificationBase::bestPair(const LHCb::GhostTrackInfo::LinkMap& lMap) const{

  LHCb::GhostTrackInfo::LinkMap::const_iterator iterMap = lMap.begin();
  double tot = 0.0;
  std::pair<LHCb::MCParticle*,unsigned int> best(0,0);
  for (; iterMap != lMap.end(); ++iterMap){
    if ( iterMap->second > best.second ){
      best = *iterMap;
    }
    tot+= iterMap->second;
  } // iterMap
  
  const double purity = double(best.second)/tot;
  return std::make_pair(best.first,purity); 
}


LHCb::GhostTrackInfo::LinkPair TrackGhostClassificationBase::bestPair(const LHCbIDs& ids) const{

  ILHCbIDsToMCParticles::LinkMap testMap; 
  LHCbIDs::const_iterator start = ids.begin();
  LHCbIDs::const_iterator stop = ids.end();
  linkTool()->link(start,stop, testMap);
  return bestPair(testMap);

}

bool TrackGhostClassificationBase::isMatched(const LHCb::GhostTrackInfo::LinkPair& aPair) const{
  return aPair.second > purityCut();
}
