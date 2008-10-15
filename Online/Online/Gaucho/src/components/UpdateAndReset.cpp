//      ====================================================================
//      UpdateAndReset.cpp
//      --------------------------------------------------------------------
//
//      Package   : Gaucho
//
//      Author    : Eric van Herwijnen
//      Should be run as first algorithm in TopAlg
//      Checks:
//      1) If the run number has changed. If so, updateAll(true) and resetHistos
//      2) If timerelapsed flag is true. If so, updateAll(false)   
//
//      ====================================================================


// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram.h"
#include "AIDA/IProfile1D.h"
#include "TFile.h"
#include "TH1.h"
#include "TProfile.h"
#include "TDirectory.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "Gaucho/Misc.h"

#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "RTL/rtl.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"
#include "Event/RecHeader.h"

#include "UpdateAndReset.h"
#include "MonitorSvc.h"
#include "RTL/rtl.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(UpdateAndReset)

// Constructor
//------------------------------------------------------------------------------
UpdateAndReset::UpdateAndReset(const std::string& name, ISvcLocator* ploc)
  : GaudiAlgorithm(name, ploc)
{
  declareProperty("desiredDeltaTCycle", m_desiredDeltaTCycle = 10);
  declareProperty("disableReadOdin", m_disableReadOdin = 0);
  declareProperty("disableChekInTimer", m_disableChekInTimer = 0);
  declareProperty("disableChekInExecute", m_disableChekInExecute = 0);
  declareProperty("disableUpdateData", m_disableUpdateData = 0);
  declareProperty("disableResetHistos", m_disableResetHistos = 0);
  
  declareProperty("saveHistograms", m_saveHistograms = 0);
  declareProperty("saveSetDir", m_saveSetDir = "~/");
  declareProperty("saverCycle", m_saverCycle = 100);
  declareProperty("resetHistosAfterSave", m_resetHistosAfterSave = 0);
  
  declareProperty("teste", m_teste = 100000);
  
  m_timerCycle = 9;
  m_firstExecute = true;
  
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::initialize() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isSuccess() ) msg << MSG::DEBUG << "GaudiAlgorithm Initialized" << endreq;
  else {
    msg << MSG::FATAL << "GaudiAlgorithm not Initialized" << endreq;
    return StatusCode::FAILURE;
  }

  //const std::string& utgid = RTL::processName();
  m_utgid = RTL::processName();
  msg << MSG::DEBUG << "Setting up DIM for UTGID " << m_utgid << endreq;
    
  sc = serviceLocator()->service("MonitorSvc", m_pGauchoMonitorSvc, false);
  if( sc.isSuccess() ) msg << MSG::DEBUG << "Found the IGauchoMonitorSvc interface" << endreq;
  else {
    msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
    return StatusCode::FAILURE;
  }

  m_pGauchoMonitorSvc->enableMonObjectsForString();
  
  m_infoFileStatus = "SAVESETLOCATION/......................................................";
  std::string infoName = "SAVESETLOCATION";
  declareInfo(infoName, m_infoFileStatus, "Filename of latest saveset");
  
  sc = serviceLocator()->service("HistogramDataSvc", m_histogramSvc, true); 
  
  if (sc.isFailure()) msg << MSG::FATAL << "Unable to locate the HistogramDataService" << endreq;
  else msg << MSG::DEBUG << "Found the HistogramDataService" << endreq;
  
  

  if (0 == m_desiredDeltaTCycle){
    msg << MSG::WARNING << "Your algorithm is using the UpdateAndReset algrithm which update and reset data every desiredDeltaTCycle seconds. You didn't fill the desiredDeltaTCycle option in your options file, then we will consider 10 seconds as default." << endreq;
    m_desiredDeltaTCycle = 10;
  }
    
  // In the begining the delayed values are the same as the current values.
  msg << MSG::INFO << "This program will update data every " << m_desiredDeltaTCycle << " seconds !!!" << endreq;

  if (1 == m_saveHistograms){
    msg << MSG::INFO << "As you demand in your options file, this program will save data " << endreq;
    div_t divresult = div (m_saverCycle, m_desiredDeltaTCycle);
    m_numCyclesToSave = divresult.quot;
    if (0 == divresult.rem ) {
      msg << MSG::INFO << " every " << m_saverCycle << " seconds" << endreq;
    } 
    else {
      msg << MSG::INFO << " every " << divresult.quot * m_desiredDeltaTCycle << " seconds. Sorry !!!!, you ask for a Saver cycle of " << m_saverCycle << " but for implementation reasons it's not possible because the Saver cycle must be a multiple of the desiredDeltaTCycle, which is" <<  m_desiredDeltaTCycle << endreq;
      m_saverCycle = divresult.quot * m_desiredDeltaTCycle;
    } 
  }
  
  // The below part is for test
  m_countExecutes = 0; 
  m_deltaTRunTest = 8 * m_desiredDeltaTCycle;

  m_timeStart = GauchoTimer::currentTime();

  m_runNumber = currentRunNumber().first.first;
  m_cycleNumber = currentCycleNumber(m_timeStart).first;
  m_timeFirstEvInRun = m_timeStart;
  m_offsetTimeFirstEvInRun = offsetToBoundary(m_cycleNumber, m_timeFirstEvInRun, true);
  m_timeLastEvInCycle = m_timeStart;
  m_gpsTimeLastEvInCycle = gpsTime();
  m_runStatus = s_statusNoUpdated;
  m_cycleStatus = s_statusNoUpdated;

  if (0==m_disableMonRate) m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumber, m_cycleNumber, m_deltaTCycle, m_offsetTimeFirstEvInRun, m_offsetTimeLastEvInCycle, m_offsetGpsTimeLastEvInCycle);

  return StatusCode::SUCCESS;
}

void UpdateAndReset::timerHandler()
{
  MsgStream msg(msgSvc(), name());
  if (0 != m_disableChekInTimer) {
    msg << MSG::DEBUG << "===============> Checking changes inside timer handler is disable." << endreq;
    return;
  }
//  msg << MSG::DEBUG << "**********************************************************************" << endreq;
  msg << MSG::DEBUG << "********************Inside timerHandler*******************************" << endreq;

  //verifyAndProcessRunChange("timer");
  verifyAndProcessCycleChange(true);

  DimTimer::start(m_timerCycle); // we verify the cycle status every 9 seconds
//  msg << MSG::DEBUG << "***********************End timerHandler*******************************" << endreq;
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
}

StatusCode UpdateAndReset::testeExecute() {
  MsgStream msg( msgSvc(), name() );
  
/*  div_t divresult = div (m_countExecutes,1000);
  if (divresult.rem == 0) msg << MSG::INFO << "Execute method # " << m_countExecutes << endreq;*/
  msg << MSG::INFO << "Execute method # " << m_countExecutes << endreq;
  if (m_countExecutes == m_teste) {
    msg << MSG::INFO << "Execute method # " << m_countExecutes << endreq;
    m_pGauchoMonitorSvc->resetHistos(true);
    m_countExecutes = 0;
  }
  m_countExecutes++;
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

  m_countExecutes++;
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  //msg << MSG::DEBUG << "Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "********************************************************************" << endreq;

  if (0 == m_disableChekInExecute){
    verifyAndProcessRunChange();
    verifyAndProcessCycleChange(false);
  }
  else msg << MSG::DEBUG << "===============> Checking changes inside execute method is disable." << endreq;

  // Because the plot method we start the timer after the first execute...
  // This is because of plot method declareinfo in the execute method...
  if (m_firstExecute){
    m_firstExecute = false;
    DimTimer::start(m_timerCycle);
    m_firstCycleNumber = currentCycleNumber(GauchoTimer::currentTime()).first;
  }
  //msg << MSG::DEBUG << "End of Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  return StatusCode::SUCCESS;
}

void UpdateAndReset::verifyAndProcessCycleChange(bool isFromTimerHandler) {
  ulonglong currentTime = GauchoTimer::currentTime();
  std::pair<int, bool> cycleNumber = currentCycleNumber(currentTime);
  if (!cycleNumber.second) return; // false means that the cycleNumber wasn't change
  if (s_statusNoUpdated != m_cycleStatus) return; // We update if the cycleStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  std::string method = "execute";
  if (isFromTimerHandler) method = "timerHandler";
  msg << MSG::DEBUG << " Process Triggered by the "<< method <<"() method." << endreq;
  m_cycleStatus = s_statusProcessingUpdate;
  updateData(false, isFromTimerHandler); //1er param false means that the update wasn't called when runNumber changed
  m_cycleStatus = s_statusUpdated;
  retrieveCycleNumber(cycleNumber.first);
}

void UpdateAndReset::verifyAndProcessRunChange() { // this method can not be called from TimerHandler
  std::pair<std::pair<int, ulonglong>, bool> runNumber = currentRunNumber(); // if this is too late, we can not avoid two process calling it.
  if (!runNumber.second) return;// false means that the runNumber wasn't change
  if (s_statusNoUpdated != m_runStatus) return; // We update if the runStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " The Run Number has changed then we UpdateAll and reset Histograms" << endreq;
  m_runStatus = s_statusProcessingUpdate;
  updateData(true, false); //1er param true means that the update was called because the runNumber changed and 2d false means no TimerHandler
  m_runStatus = s_statusUpdated;
  retrieveRunNumber(runNumber.first.first, runNumber.first.second);
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "finalizing...." << endreq;
  manageTESHistos (false, false, true, true);
  DimTimer::stop();
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveRunNumber(int runNumber, ulonglong gpsTime) {
//------------------------------------------------------------------------------
  m_runNumber = runNumber;
  m_runStatus = s_statusNoUpdated;
  m_timeFirstEvInRun = gpsTime;
  m_offsetTimeFirstEvInRun = offsetToBoundary(currentCycleNumber(m_timeFirstEvInRun).first, m_timeFirstEvInRun, true);
}

//------------------------------------------------------------------------------
// std::pair<int, bool> UpdateAndReset::currentRunNumber() {
std::pair<std::pair<int, ulonglong>, bool> UpdateAndReset::currentRunNumber() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  int runNumber=0;
  ulonglong gpsTime = GauchoTimer::currentTime();
  bool changed = false;

  //msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
      msg << MSG::DEBUG<< "ODIN Bank exist. " <<endreq;
      LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
      msg << MSG::DEBUG<< "Getting RunNumber. " <<endreq;
      runNumber = odin->runNumber();
      gpsTime = odin->gpsTime();
      msg << MSG::DEBUG<< "runNumber from ODIN is. " <<endreq;
    }
    else
    {
      msg << MSG::DEBUG<< "ODIN Bank doesn't exist. " <<endreq;	
    }
  }
  else {
//    msg << MSG::DEBUG<< "===============> Reading Odin bank is disabled. Then runNumber = 0 and gpsTime = currentTime" <<endreq;
  }

  if (m_runNumber != runNumber) changed = true;

  std::pair<int, ulonglong> runNumberGpsTime = std::pair<int, ulonglong>(runNumber, gpsTime);
  
  // return std::pair<int, bool>(runNumber,changed);
  return std::pair<std::pair<int, ulonglong>, bool>(runNumberGpsTime, changed);
}

//------------------------------------------------------------------------------
ulonglong UpdateAndReset::gpsTime() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

//  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
      msg << MSG::DEBUG<< " Congratulations, you can read the ODIN Bank. " << endreq;
      LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
      return odin->gpsTime();
    }
  }
  else msg << MSG::DEBUG<< "===============> Reading Odin bank is disabled. " <<endreq;

  // this is only for test when Odin doesn't work
  ulonglong currentTime = GauchoTimer::currentTime();

  int cycleNumber = currentTime/(m_desiredDeltaTCycle*1000000);

  ulonglong gpsTime = ((ulonglong)cycleNumber)*((ulonglong) m_desiredDeltaTCycle*1000000);

  return gpsTime;
}

std::pair<int, bool> UpdateAndReset::currentCycleNumber(ulonglong currentTime) {
  bool changed = false;
  int cycleNumber = currentTime/(m_desiredDeltaTCycle*1000000);
  if (m_cycleNumber != cycleNumber ) changed = true;
  return std::pair<int, bool>(cycleNumber,changed);
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveCycleNumber(int cycleNumber) {
//------------------------------------------------------------------------------
  // In order to avoid conflict, the cycle number is updated here and only here
  m_cycleNumber = cycleNumber;
  m_cycleStatus = s_statusNoUpdated;
}

double UpdateAndReset::offsetToBoundary(int cycleNumber, ulonglong time, bool inferior){
  if (inferior) {
    ulonglong timeIniCycle = ((ulonglong) cycleNumber)*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeIniCycle));
  }
  else {
    ulonglong timeEndCycle = ((ulonglong) (cycleNumber+1))*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeEndCycle));
  }
}

void UpdateAndReset::updateData(bool isRunNumberChanged, bool isFromTimerHandler) {
  MsgStream msg( msgSvc(), name() );
  ulonglong currentTime = GauchoTimer::currentTime();
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
  msg << MSG::DEBUG << "************Updating data " << (currentTime - m_timeStart) << " microseconds after start **********" << endreq;
  msg << MSG::DEBUG << "m_runNumber        = " << m_runNumber << endreq;
  msg << MSG::DEBUG << "m_cycleNumber      = " << m_cycleNumber << endreq;
  msg << MSG::DEBUG << "m_timeFirstEvInRun      = " << m_timeFirstEvInRun << endreq;
  msg << MSG::DEBUG << "m_offsetTimeFirstEvInRun      = " << m_offsetTimeFirstEvInRun << endreq;
  m_deltaTCycle = currentTime - m_timeLastEvInCycle;
  msg << MSG::DEBUG << "m_deltaTCycle = " << m_deltaTCycle << " microseconds" << endreq;
  m_timeLastEvInCycle = currentTime;
  m_offsetTimeLastEvInCycle = offsetToBoundary(m_cycleNumber, m_timeLastEvInCycle, false);
  msg << MSG::DEBUG << "m_timeLastEvInCycle     = " << m_timeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "m_offsetTimeLastEvInCycle     = " << m_offsetTimeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "deltaT error = " << m_deltaTCycle - m_desiredDeltaTCycle*1000000 << " microseconds" << endreq;
  
  if (!isFromTimerHandler) m_gpsTimeLastEvInCycle = currentTime; // we can not read ODIN from timerHandler
  else  m_gpsTimeLastEvInCycle = gpsTime();
    
  m_offsetGpsTimeLastEvInCycle = offsetToBoundary(m_cycleNumber, m_gpsTimeLastEvInCycle, false);
  
  msg << MSG::DEBUG << "m_gpsTimeLastEvInCycle  = " << m_gpsTimeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "m_offsetGpsTimeLastEvInCycle  = " << m_offsetGpsTimeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "TimeLastEvent error = " << (m_timeLastEvInCycle - m_gpsTimeLastEvInCycle) << " microseconds" << endreq;

  if (isRunNumberChanged) {
    if (0 == m_disableUpdateData) m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
    else msg << MSG::DEBUG << "===============> Data was not updated because the UpdateData process is disable." << endreq;
    if (0 == m_disableResetHistos) {
      if ( 1 == m_saveHistograms ) {
         msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE FAST RUN CHANGE<=======================" << endreq;
         msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE FAST RUN CHANGE<=======================" << endreq;
         msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE FAST RUN CHANGE<=======================" << endreq;
         manageTESHistos(false, true, true, false);
      }
      else manageTESHistos(false, true, false, false);
    }
    else msg << MSG::DEBUG << "===============> resetHistos disabled." << endreq;
  }
  else{
    if (0 == m_disableUpdateData) m_pGauchoMonitorSvc->updateAll(false);
    if ( 1 == m_saveHistograms ) {
      bool resetHistos = false;
      if(1 == m_resetHistosAfterSave) resetHistos = true;
      if (isSaveCycle(m_cycleNumber)) {
        msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE CYCLE SAVER <=======================" << endreq;
        msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE CYCLE SAVER <=======================" << endreq;
        msg << MSG::DEBUG << "==============> SAVING HISTOS BECAUSE CYCLE SAVER <=======================" << endreq;
        manageTESHistos(false, resetHistos, true, false);
      }
    }
  }
  msg << MSG::DEBUG << "***************************  End updateData  *************************" << endreq;
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
}

bool UpdateAndReset::isSaveCycle(int m_cycleNumber) {
  if (m_cycleNumber == m_firstCycleNumber) return false;
  div_t divresult = div (m_cycleNumber - m_firstCycleNumber, m_numCyclesToSave);
  if (0 != divresult.rem) return false;
  return true;
}

void UpdateAndReset::manageTESHistos (bool list, bool reset, bool save, bool isFromEndOfRun) {
  MsgStream msg( msgSvc(), name() );
  IRegistry* object = rootObject();
  int level = 0;
  std::vector<std::string> idList;
  
  TFile *f=0;
  m_infoFileStatus = "......this is the file name were we will save histograms...........";
  char timestr[64];
  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);
  
  std::vector<std::string> serviceParts = Misc::splitString(m_utgid, "_");
  
  std::string groupName = "GroupName";
  if (3 == serviceParts.size()) groupName = serviceParts[1];
  else if (4 == serviceParts.size()) groupName = serviceParts[0] + "-" +  serviceParts[2];
  
  std::string tmpfile = m_saveSetDir + groupName + "-" + timestr + ".root";  
  if (isFromEndOfRun) tmpfile = m_saveSetDir + "EOR_"+ groupName + "-" + timestr + ".root";  
  
  m_infoFileStatus.replace(0, m_infoFileStatus.length(), tmpfile);

  if (save)  {
    msg << MSG::INFO << "SaverSvc will save histograms in file " << m_infoFileStatus << endreq;
    f = new TFile(m_infoFileStatus.c_str(),"create");
  }
  
  histogramIdentifier(object, idList, reset, save, level, (TDirectory*) f);
  
  if (0 == idList.size()) msg << MSG::INFO << "No histogram found" << endreq;
  
  if (save) {
    f->Close();
    delete f;f=0;
  }
  
  if ((list)&&(0 != idList.size())) {
    msg << MSG::INFO << "Printing identified histograms/profiles " << endreq;
    for (std::vector<std::string>::iterator it = idList.begin(); it != idList.end(); it++){
      msg << MSG::INFO << "    " << (*it) << endreq;
    }
  }
}

void UpdateAndReset::histogramIdentifier(IRegistry* object, std::vector<std::string> &idList, bool reset, bool save, int &level,
                                         TDirectory* rootdir){
  MsgStream msg( msgSvc(), name() );
  std::vector<IRegistry*> leaves;
  std::vector<IRegistry*>::const_iterator  it;
  
  msg << MSG::DEBUG << "Looking for histos in object " << object->identifier() << ", level  " << level << endreq;
  SmartIF<IDataManagerSvc> dataManagerSvc(m_histogramSvc);
  if (!dataManagerSvc) {
    msg << MSG::WARNING << "    Unable to go to the transient store. " << endreq;
    return;
  }
  
  StatusCode sc = dataManagerSvc->objectLeaves(object, leaves);
  if (sc.isFailure()) msg << MSG::WARNING << "    No histograms found on the transient store." << endreq;
      
  for ( it=leaves.begin(); it != leaves.end(); it++ ) {
    const std::string& id = (*it)->identifier();
    rootdir->cd();
    //msg << MSG::DEBUG << "    Object found: " << id << endreq;
    
    DataObject* dataObject;
    sc = m_histogramSvc->retrieveObject(id, dataObject);
    if (sc.isFailure()) {
      msg << MSG::INFO << "Could not retrieve object from TES " << endreq;
      continue;
    }

    IHistogram* histogram = dynamic_cast<AIDA::IHistogram*> (dataObject);
    //TH1* hRoot = dynamic_cast<TH1*> (dataObject);
    if ( 0 != histogram) {
//    if ( 0 != hRoot) {
      if (save) {
        TH1* hRoot = (TH1*) Gaudi::Utils::Aida2ROOT::aida2root(histogram);
        std::vector<std::string> HistoFullName = Misc::splitString(hRoot->GetName(), "/");
        TH1* copy = (TH1*) hRoot->Clone( HistoFullName[HistoFullName.size()-1].c_str() );
        msg << MSG::DEBUG << ", saving name=" << copy->GetName() << " directory="
            << (copy->GetDirectory() ? copy->GetDirectory()->GetName() : "none") <<endreq;
        copy->Write();
        delete copy;
      }
      if (reset) histogram->reset();
//      if (reset) hRoot->Reset();
      idList.push_back(id);
      continue;
    }
    IProfile1D* profile = dynamic_cast<AIDA::IProfile1D*> (dataObject);
    //TProfile* hProf = dynamic_cast<TProfile*> (dataObject);
    if (0 != profile) {
//    if (0 != hProf) {
      //msg << MSG::DEBUG << ", title = " << profile->title() << endreq;
      if (save)  {
        TProfile* hRoot = (TProfile*) Gaudi::Utils::Aida2ROOT::aida2root(profile);
        std::vector<std::string> HistoFullName = Misc::splitString(hRoot->GetName(), "/");
        TH1* copy = (TH1*) hRoot->Clone( HistoFullName[HistoFullName.size()-1].c_str() );
        msg << MSG::DEBUG << ", saving name=" << copy->GetName() << " directory="
            << (copy->GetDirectory() ? copy->GetDirectory()->GetName() : "none") <<endreq;
        copy->Write();
        delete copy;
      }
      if (reset) profile->reset();
//      if (reset) hProf->Reset();
      idList.push_back(id);
      continue;
    }
    
    // not an histogram: must be a directory: create corresponding TDirectory
    std::vector<std::string> rootDirs = Misc::splitString(id, "/");
    TDirectory* newdir = rootdir;
    if(NULL != newdir) {
      newdir = rootdir->mkdir(rootDirs[rootDirs.size()-1].c_str());
      newdir->cd();
    }
    int newLevel = level + 1;
    if (newLevel >= 10) continue;
    histogramIdentifier(*it, idList, reset, save, newLevel, newdir);
  }
} 

IRegistry* UpdateAndReset::rootObject(){
  MsgStream msg( msgSvc(), name() );
  std::string  path;
  SmartDataPtr<DataObject> smartDataPtr(m_histogramSvc, path);
  msg << MSG::INFO << "root identifier : " << smartDataPtr->registry()->identifier() << endreq;
  return smartDataPtr->registry();
}



