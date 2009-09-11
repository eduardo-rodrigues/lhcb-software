
#ifndef GAUDISVC_XMLSUMMARYSVC_H
#define GAUDISVC_XMLSUMMARYSVC_H

#include "Python.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/ICounterSummarySvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiUtils/IFileCatalog.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <utility>

namespace Gaudi
{  
  class IIODataManager;
}

template <class TYPE> class SvcFactory;


/** @class XMLSummarySvc XMLSummarySvc.h
 *  
 *  Service controlling the python writing of
 *  an XML summary.
 *  Inherits from the Incident handler->gets EOF events
 *  To collect all counters it must be named "CounterSummarySvc"
 *
 *  @code
 *   sum=XMLSummarySvc("CounterSummarySvc")
 *   sum.xmlfile="summary.xml"
 *   ApplicationMgr().ExtSvc+=sum
 *   
 *   #OR
 *   LHCbApp().XMLSummary="summary.xml"
 *  @endcode
 *
 *  @author Robert Lambert
 *  @date   2009-09-11
 */
class XMLSummarySvc
  : public extends1<Service, ICounterSummarySvc>
  , public virtual IIncidentListener {

public:

  //a list of pairs is used, as referring to the entry by key is not required
  //sorting is also not required, and would waste time
  //these internal typedefs are used as a shortcut only
  typedef std::pair< std::string, const StatEntity > NameStatPair;
  //first=name, second=counter
  typedef std::pair< std::pair< std::string, const StatEntity >, const Gaudi::CounterSummary::SaveType > 
          NameStatTypePair;
  //first.first=name, first.second=counter, second=SaveType
  typedef std::list< std::pair< std::pair< std::string, const StatEntity >, const Gaudi::CounterSummary::SaveType > > 
          NameStatList;


  virtual StatusCode initialize();
  virtual StatusCode finalize();

  XMLSummarySvc( const std::string& name, ISvcLocator* svc );
  
  // ==========================================================================
  // ICounterSummarySvc interface
  // ==========================================================================

  virtual void addCounter(
			  const std::string alg,
			  const std::string name,
			  const StatEntity & count,
			  const Gaudi::CounterSummary::SaveType saveType
			    =Gaudi::CounterSummary::SaveSimpleCounter);


  virtual void addCounter(
			  const std::string alg,
			  const std::string name,
			  const Stat & count,
			  const Gaudi::CounterSummary::SaveType saveType
			    =Gaudi::CounterSummary::SaveSimpleCounter);
  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) ;
  // ==========================================================================

  // Destructor.
  virtual ~XMLSummarySvc();

private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<XMLSummarySvc>;
  IIncidentSvc* m_incSvc; ///the incident service
  Gaudi::IIODataManager * m_ioDataManager; ///the pointer to the data manager service
  ///RegEx list of integer counters to write. Set by property "CounterList".
  std::vector<std::string> m_counterList;
  ///RegEx list of StatEntity counters to write. Set by property "StatEntityList".
  std::vector<std::string> m_statEntityList; 
  ///name of the xmlfile to write. Set by property xmlfile.
  std::string m_xmlfile;
  ///name of the schema to use. Set by property xsdfile.
  std::string m_xsdfile;

  PyObject * m_summary; ///the pointer to the python object
  StatEntity m_handled; ///simple counter of #handled events

  ///the list of added counters
  NameStatList m_addedCounters; 

  std::string m_filename; ///the current open filename

  ///counters are filled in the finalise method, using these internal functions
  StatusCode fillcounters();
  StatusCode fillcounter(const NameStatTypePair & count);
  
  ///fill in the virtual memory usage
  StatusCode fillUsage();

  ///shorcut to prepare the incident service
  StatusCode prepareIncSvc();
  
  ///shorcut to define the summary object
  StatusCode prepareXML();
  ///shorcut to write the summary
  StatusCode writeXML(MSG::Level lev=MSG::VERBOSE);
  StatusCode printXML(MSG::Level lev=MSG::VERBOSE);
  
  std::string file2GUID(const std::string & filename);
  std::string AFN2name(const std::string & filename);
  
  /// Map of FID to PFN
  typedef std::map<std::string, std::string> FidMap;
  /// Map of FID to PFN
  std::map<std::string, std::string>   m_fidMap;
};

#endif
