#ifndef L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H
#define L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H     1

#include "L0Base/L0FromRawBase.h"
#include "L0MuonOutputs.h"

/** @class L0MuonCandidatesFromRaw L0MuonCandidatesFromRaw.h component/L0MuonCandidatesFromRaw.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-04-14
 */
class L0MuonCandidatesFromRaw : public L0FromRawBase {
    
public:

  L0MuonCandidatesFromRaw(const std::string& name, ISvcLocator* pSvcLocator);
  // ~L0MuonCandidatesFromRaw();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode dumpErrors();
  StatusCode tae_bunches(std::vector<std::string> &bunches);
  
private:

  // Algorithm's properties
  int m_procVersion;                  // Emulator version
  std::string  m_configfile;          // Config file name
  int m_mode;                         // Output mode (defin the bank to be decoded)
  bool m_disableTAE;                  // Flag to disable the decoding of TAE event

  // For output to RawEvent, TES or L0ProcessorDatas
  L0MuonOutputs* m_outputTool;

  // Counters for final report
  int m_totEvent ;  // Tot number of events processed
  int m_totBx ;     // Tot number of time slices processed
  int m_errorEvent ; // Number of events where there is a difference between the reprocessing and the raw data
  
  static inline std::string timeSlot(int bx)
  {
    std::string ts;
    switch (bx) {
    case -7 : return "Prev7/";
    case -6 : return "Prev6/";
    case -5 : return "Prev5/";
    case -4 : return "Prev4/";
    case -3 : return "Prev3/";
    case -2 : return "Prev2/";
    case -1 : return "Prev1/";
    case  0 : return "";
    case  1 : return "Next1/";
    case  2 : return "Next2/";
    case  3 : return "Next3/";
    case  4 : return "Next4/";
    case  5 : return "Next5/";
    case  6 : return "Next6/";
    case  7 : return "Next7/";
    default : return "Unknown";
    };
    
  };
    
};

#endif      // L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H 
