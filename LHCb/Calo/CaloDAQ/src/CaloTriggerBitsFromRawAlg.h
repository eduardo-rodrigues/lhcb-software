#ifndef CALOTRIGGERBITSFROMRAWALG_H 
#define CALOTRIGGERBITSFROMRAWALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// CaloDAQ
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"

/** @class CaloTriggerBitsFromRawAlg CaloTriggerBitsFromRawAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-04-07
 */
class CaloTriggerBitsFromRawAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloTriggerBitsFromRawAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloTriggerBitsFromRawAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_outputData;         ///< Output container
  std::string m_toolName;
  std::string m_toolType;
  bool m_isPrs;
  ICaloTriggerBitsFromRaw*    m_l0BitTool;
  std::string m_extension;  
  bool m_statusOnTES;
};
#endif // CALOTRIGGERBITSFROMRAWALG_H
