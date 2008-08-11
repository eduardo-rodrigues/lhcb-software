// $Id: AnalysisTask.h,v 1.4 2008-08-11 08:05:15 ggiacomo Exp $
#ifndef OMALIB_ANALYSISTASK_H 
#define OMALIB_ANALYSISTASK_H 1

/** @class AnalysisTask AnalysisTask.h OMAlib/AnalysisTask.h
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-18
 */
#include "OMAlib/OMAlib.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class SavesetFinder;

class AnalysisTask : public OMAlib, public GaudiAlgorithm {
public:
  AnalysisTask( const std::string& name, 
                ISvcLocator* pSvcLocator );

  virtual ~AnalysisTask( );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task) = 0;

protected:
  bool m_useDB;
  std::string m_DB;
  std::string m_DBuser;
  std::string m_DBpw;
  std::vector<std::string> m_inputFiles;
  std::vector<std::string> m_inputTasks;
  unsigned int m_MessageMode;
  std::vector<SavesetFinder*> m_saveset;
};
#endif // OMALIB_ANALYSISTASK_H
