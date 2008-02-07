// $Id: HltConfigSvc.h,v 1.1 2008-02-07 16:36:58 graven Exp $
#ifndef HLTCONFIGSVC_H 
#define HLTCONFIGSVC_H 1

// Include files
#include <string>
#include <map>
#include <vector>
// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "PropertyConfigSvc.h"


/** @class HltConfig HltConfig.h
 *  
 *  functionality:
 *         configure the HLT algorithms...
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class HltConfigSvc : public PropertyConfigSvc, virtual public IIncidentListener {
public:

  HltConfigSvc(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltConfigSvc( );           ///< Destructor
  virtual StatusCode initialize();    ///< Service initialization
  virtual StatusCode finalize();      ///< Service finalization
  virtual void handle(const Incident&);

private:

  typedef unsigned int TCK_t ;
  typedef std::map<std::string,std::string> TCKMap_t;

  TCKMap_t                     m_tck2config;      ///< from TCK to configuration ID
  TCK_t                        m_initialTCK;      ///< which TCK to start with...
  TCK_t                        m_configuredTCK;   ///< which TCK is currently in use?

  // resolve TCK -> toplevel config ID, then call matching method with ID
  StatusCode configure(const TCK_t& tck) const;
  StatusCode reconfigure(const TCK_t& tck) const;
  bool loadConfig(const TCK_t& tck);

};
#endif // HLTCONFIGSVC_H
