// $Id: TaggerPionSameTool.h,v 1.7 2007-03-01 20:59:23 musy Exp $
#ifndef USER_TAGGERPIONSAMETOOL_H 
#define USER_TAGGERPIONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/ProtoParticle.h"
#include "Event/FlavourTag.h"

#include "Kernel/ITagger.h"

// from local
#include "ITaggingUtils.h"
#include "INNetTool.h"

/** @class TaggerPionSameTool TaggerPionSameTool.h 
 *
 *  Tool to tag the B flavour with a PionSame Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerPionSameTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerPionSameTool( const std::string& type,
		      const std::string& name,
		      const IInterface* parent );
  virtual ~TaggerPionSameTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
			    std::vector<const LHCb::Vertex*>&, LHCb::
			    Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  INNetTool* m_nnet;
  ITaggingUtils* m_util;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties 
  double m_Pt_cut_pionS;
  double m_P_cut_pionS;
  double m_IPs_cut_pionS;
  double m_dQcut_pionS;
  double m_dQcut_extra_pionS;
  double m_AverageOmega;
  double m_ProbMin, m_lcs_cut;
};
//===============================================================//
#endif // USER_TAGGERPIONSAMETOOL_H
