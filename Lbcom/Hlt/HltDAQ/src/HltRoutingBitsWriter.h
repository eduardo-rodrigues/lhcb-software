// $Id: HltRoutingBitsWriter.h,v 1.4 2010-05-19 09:47:54 graven Exp $
#ifndef HLTCOMMON_HLTROUTINGBITSWRITER_H 
#define HLTCOMMON_HLTROUTINGBITSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "boost/array.hpp"

#include "LoKi/OdinTypes.h"
#include "LoKi/L0Types.h"
#include "LoKi/HLTTypes.h"

/** @class HltRoutingBitsWriter HltRoutingBitsWriter.h
 *  
 *
 *  @author Gerhard Raven
 *  @date   2008-07-29
 */
class HltRoutingBitsWriter : public GaudiHistoAlg {
public: 
  /// Standard constructor
  HltRoutingBitsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRoutingBitsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
private:
  StatusCode decode();

  enum { nBits = 3*sizeof(unsigned int)*8 };
  struct hlt_eval_t {
    LoKi::Types::HLT_Cut *predicate;
    StatEntity *counter;
    AIDA::IHistogram1D* hist;
  } ;
  struct l0_eval_t {
    LoKi::Types::L0_Cut *predicate;
    StatEntity *counter;
    AIDA::IHistogram1D* hist;
  } ;
  struct odin_eval_t {
    LoKi::Types::ODIN_Cut *predicate;
    StatEntity *counter;
    AIDA::IHistogram1D* hist;
  } ;
  // 8 ODIN, 24 L0DU, 32 Hlt1, 32 Hlt2
  boost::array<odin_eval_t,8> m_odin_evaluators;
  boost::array<l0_eval_t, 24> m_l0_evaluators;
  boost::array<hlt_eval_t,64> m_hlt_evaluators;
  void zeroEvaluators(bool skipDelete=false);
 
  std::string m_odin_location;
  std::string m_l0_location;
  std::string m_hlt_location;

  unsigned long long m_startOfRun;
  unsigned m_runNumber;
  double m_binWidth; // in _minutes_!
  double m_timeSpan; // in _minutes_!

  bool updateRequired() const { return m_bits_updated || m_preambulo_updated; }

  std::map<unsigned int,std::string> m_bits;
  bool m_bits_updated;
  void updateBits ( Property& );

  std::vector<std::string> m_preambulo_ ;             // the preambulo itself
  std::string m_preambulo ;                           // the preambulo itself
  bool m_preambulo_updated;
  void updatePreambulo ( Property& );
};
#endif // HLTROUTINGBITSWRITER_H
