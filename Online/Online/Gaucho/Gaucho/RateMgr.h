#ifndef GAUCHO_RATEMGR_H
#define GAUCHO_RATEMGR_H 1

#include "GaudiKernel/StatEntity.h"
#include "Gaucho/MonRate.h"
#include "Gaucho/MonSubSys.h"

#include <map>
class IMessageSvc;

class RateMgr
{
public:
  unsigned long long m_deltaT;

protected:
  std::map<const std::string, MonRateBase*>  m_rateMap;
  typedef std::map<const std::string, MonRateBase*>::iterator Rateit;

public:

  RateMgr(IMessageSvc* msgSvc, const std::string& source, int version=0);
  virtual ~RateMgr();

//  void addRate(const std::string& countName, const std::string& , MonRateBase& count)
//  {
//    m_rateMap[countName] = &count;
//  }
  void addRate(const std::string& countName,  MonRateBase& count)
  {
    m_rateMap[countName] = &count;
  }

  MonRateBase* removeRate (std::string &name)
  {
    Rateit rateMapIt = m_rateMap.find(name);
    if (rateMapIt != m_rateMap.end())
    {
      MonRateBase *p = rateMapIt->second;
      m_rateMap.erase(rateMapIt);
      return p;
    }
    return 0;
  }

  void removeRateAll (MonSubSys *s,bool del=true)
  {
    if (del)
    {
      Rateit rateMapIt;
      rateMapIt = m_rateMap.begin();
      while (rateMapIt != m_rateMap.end())
      {
        MonRateBase *p = rateMapIt->second;
        m_rateMap.erase(rateMapIt);
        s->removeObj(p);
        delete p;
        rateMapIt = m_rateMap.begin();
      }
    }
    else
    {
      m_rateMap.clear();
    }
  }

  void makeRates(unsigned long long dt);
  void print();
};

#endif //GAUCHO_RATEMGR_H
