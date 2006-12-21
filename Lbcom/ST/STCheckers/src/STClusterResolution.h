// $Id: STClusterResolution.h,v 1.4 2006-12-21 17:54:48 jvantilb Exp $
#ifndef STClusterResolution_H
#define STClusterResolution_H 1

// base class
#include "GaudiAlg/GaudiHistoAlg.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
  class MCHit;
  class STCluster;
};

class DeSTDetector;
class DeSTSector;
class AIDA::IHistogram1D;
class STDetectionLayer;
class IMCParticleSelector;
class ISTClusterPosition;
class ITrajPoca;

/** @class STClusterResolution STClusterResolution.h
 *
 *  Class for plotting the resolution of STClusters. It makes the following
 *  histograms for 1, 2, 3 and 4 strip clusters:
 *  - Offline resolution (rec. minus true position)
 *  - Offline pull
 *  - Online resolution
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STClusterResolution : public GaudiHistoAlg {

public:
 
  /// constructer
  STClusterResolution(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STClusterResolution();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  virtual StatusCode fillHistograms(const LHCb::STCluster* aCluster,
                                    const LHCb::MCHit* aHit) const;
  int histoId(const int clusterSize) const;

  double calculateUTrue( const LHCb::MCHit* aHit, 
                         const DeSTSector* aSector) const;

  std::string m_detType;
  std::string m_clusterLocation;
  std::string m_asctLocation;
  DeSTDetector* m_tracker;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  ISTClusterPosition* m_positionTool;
  std::string m_positionToolName;

  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

};

#endif // STClusterResolution_H

