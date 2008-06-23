#include "HltBase/HltBase.h" 
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiHistoTool.h"

HltBase<GaudiHistoAlg>::HltBase( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{hltBaseConstructor();}

HltBase<GaudiHistoAlg>::HltBase ( const std::string& type   ,
                                  const std::string& name   ,
                                  const IInterface*  parent )
  : GaudiHistoAlg("ERROR",0)
{
  fatal() << " invalid tool constructor " << type << " name " << name
          << " interface" << parent << endreq;
}

HltBase<GaudiHistoTool>::HltBase( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiHistoTool ( "ERROR", "ERROR", 0 )
{
  fatal() << " invalid algoritm constructor : name " << name
          << " svclocator " << pSvcLocator << endreq;
}

HltBase<GaudiHistoTool>::HltBase ( const std::string& type   ,
                                   const std::string& name   ,
                                   const IInterface*  parent )
  : GaudiHistoTool(type,name,parent)
{
  hltBaseConstructor();
}

