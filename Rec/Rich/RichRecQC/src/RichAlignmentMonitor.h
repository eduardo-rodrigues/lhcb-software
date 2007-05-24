
/** @file RichAlignmentMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::AlignmentMonitor
 *
 *  CVS Log :-
 *  $Id: RichAlignmentMonitor.h,v 1.1 2007-05-24 07:08:07 papanest Exp $
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-02-19
 */

#ifndef RICHRECMONITOR_RICHALIGNMENTMONITOR_H
#define RICHRECMONITOR_RICHALIGNMENTMONITOR_H 1

// Include files
// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"

// Kernel
#include "RichKernel/BoostArray.h"

// boost
#include "boost/lexical_cast.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /** @class AlignmentMonitor RichAlignmentMonitor.h
       *
       *  Provides monitoring histograms for the alignment of the RICH mirrors.
       *
       *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
       *  @date   2004-02-19
       */

      class AlignmentMonitor : public RichRecHistoAlgBase
      {

      public:

        /// type definition
        typedef LHCb::RichGeomPhotonCode::ShortType MirrorNumber;

        /// Standard constructor
        AlignmentMonitor( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~AlignmentMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // methods


        /**Check whether to produce a histogram for this HPD
           @return The HPD identifier
         */
        int makePlotForHPD ( LHCb::RichSmartID smartID ) const;
        

      private: // data


        int m_richTemp;    ///< which rich detector to monitor
        Rich::DetectorType m_rich;

        int m_maxUsedTracks;

        // set to know to stop all MC Truth
        bool m_useMCTruth;

        // to avoid bias towards small angles use only a photons in the expected range
        double m_deltaThetaRange;
        double m_deltaThetaHistoRange;

        // particle type when fixed
        int m_particleType;
        Rich::ParticleIDType m_pType;

        // keep totals of particle types
        std::vector<int> m_pTypes;

        // tools
        const ITrackSelector* m_trSelector;        ///< Track selector
        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth; 
        const IParticleProperties* m_richPartProp; ///< Rich Particle properties
        const ICherenkovAngle* m_ckAngle;  ///< Pointer to RichCherenkovAngle tool

        // Histograms
        IHistogram1D* m_sphMirrorNumberHist[2];
        IHistogram1D* m_flatMirrorNumberHist[2];
        IHistogram2D* m_sphMirReflPoint[2];
        IHistogram2D* m_flatMirReflPoint[2];

        // vector of histogram numbers to prebook (helps with merging files)
        std::vector<int> m_preBookHistos;

        // list of HPDs for histograms
        std::vector<int> m_HPDList;
        /// make plots for all HPDs ignoring the list 
        bool m_plotAllHPDs;
        
      };

    }
  }
}

#endif // RICHRECMONITOR_RICHALIGNMENTMONITOR_H
