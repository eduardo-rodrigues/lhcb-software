#include "Gaucho/services/DimInfoServers.h"
#include "Gaucho/services/ProcessMgr.h"
#include "Gaucho/services/BaseServiceMap.h"
#include "CPP/IocSensor.h"


DimInfoServers::DimInfoServers(ProcessMgr *processMgr):
  DimInfo("DIS_DNS/SERVER_LIST", -1),
  m_name("DimInfoServers"),
  m_processMgr(processMgr)
{
  m_serverChoosed = "";
}

DimInfoServers::~DimInfoServers() {
  
}

void DimInfoServers::infoHandler() {
  
  int tmpStringSize = -1;
  while ( (tmpStringSize = getSize()) <=0 ){usleep(10000);}
  
  std::string value = getString();
  
  m_processMgr->updateServerMap(value, m_serverMap);
  
  //if ("" == m_serverChoosed) m_serverChoosed = chooseServer();
  
  IocSensor::instance().send(m_processMgr->service(), s_updateServiceMap, (void *) &m_serverMap);
  
  //m_processMgr->serviceMap()->updateMap(m_serverMap);
}

void DimInfoServers::chooseServer() {
  
  //std::cout << "choosing server from " << m_serverMap.size() << std::endl;
  
  for (m_it = m_serverMap.begin(); m_it != m_serverMap.end(); ++m_it){
    
    //std::cout << "server " << m_it->second << " is ";
  
    if (m_it->second) {
      //std::cout <<  " active" << std::endl;
      m_serverChoosed = m_it->first;
      return ;
    }
     //std::cout <<  " inactive" << std::endl;
  }
}

bool DimInfoServers::isActive(std::string serverName) {
  return m_serverMap.find(serverName)->second;
}


void DimInfoServers::print() {
 MsgStream msg(m_processMgr->msgSvc(), name());
 for (m_it = m_serverMap.begin(); m_it != m_serverMap.end(); ++m_it){
   std::string status = "inactive" ;
   if (m_it->second) status = "active" ;
   msg << MSG::DEBUG << "The server " << m_it->first << " is " << status << endreq;    
  }
}

