// $Id: PatVeloPhiHitManager.h,v 1.4 2009-07-20 11:35:32 dhcroft Exp $
#ifndef INCLUDE_TF_PATVELOPHIHITMANAGER_H
#define INCLUDE_TF_PATVELOPHIHITMANAGER_H 1

#include "PatVeloHit.h"
#include "TfKernel/ExtendedVeloPhiHitManager.h"

namespace Tf {
  static const InterfaceID IID_PatVeloPhiHitManager( "Tf::PatVeloPhiHitManager", 1, 0 );

  /** @class PatVeloPhiHitManager PatVeloPhiHitManager.h
   * Mananges specialised phi hits for VELO space tracking. 
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-08
   */
  class PatVeloPhiHitManager : public Tf::ExtendedVeloPhiHitManager<PatVeloPhiHit> {

    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_PatVeloPhiHitManager; }

      /// Standard Constructor
      PatVeloPhiHitManager(const std::string& type,
          const std::string& name,
          const IInterface* parent);

      virtual ~PatVeloPhiHitManager() {;} ///< Destructor

      StatusCode initialize(); ///< Tool initialization
      StatusCode   finalize(); ///< Tool finalize

      virtual void prepareHits();

      void prepareHits(StationIterator it) { prepareHits(*it); }          ///< Prepare hits for one station only
      void prepareHits(StationReverseIterator rit) { prepareHits(*rit); } ///< Prepare hits for one station only
      void prepareHits(Station* station);        ///< Prepare hits for one station only, implementation

      void resetUsedFlagOfHits(); ///< Reset all used flags to unused

  private:
      /// Max number of clusters in phi inner region before region killed as too hot
      unsigned int m_maxPhiInner; 
      /// Max number of clusters in phi outer region before region killed as too hot
      unsigned int m_maxPhiOuter; 
      /// Use CCE scan pattern recognition
      bool m_CCEscan;
      /// CCE scan sensor list to kill
      std::vector<unsigned int> m_killSensorList;
      /// CCE step for voltage determination
      int m_cceStep;      
  };
}
#endif // INCLUDE_TF_PATVELOPHIHITMANAGER_H

