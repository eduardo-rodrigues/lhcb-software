// $Id: RichPhotonSignal.h,v 1.1 2003-06-30 15:47:05 jonrob Exp $
#ifndef RICHRECTOOLS_RICHPHOTONSIGNAL_H
#define RICHRECTOOLS_RICHPHOTONSIGNAL_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPhoton.h"

// Interfaces
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichSegmentProperties.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"

/** @class RichPhotonSignal RichPhotonSignal.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPhotonSignal : public RichRecToolBase,
                         virtual public IRichPhotonSignal {
  
public:

  /// Standard constructor
  RichPhotonSignal( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonSignal(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Predicted pixel signal for a given reconstructed photon under a given mass hypothesis
  double predictedPixelSignal( RichRecPhoton * photon,
                               const Rich::ParticleIDType id );

private: // private methods

  /// Signal Probability
  double signalProb( RichRecPhoton * photon,
                     const Rich::ParticleIDType& id );

  /// Scatter Probability
  double scatterProb( RichRecPhoton * photon,
                      const Rich::ParticleIDType& id );

private: // private data

  /// Pointer to RichSegmentProperties interface
  IRichSegmentProperties * m_segProps;

  /// Temporary local value for Radii of curvature
  double m_radiusCurv[Rich::NRiches];

  /// Temporary local value pixel area
  double m_pixelArea;

};

#endif // RICHRECTOOLS_RICHPHOTONSIGNAL_H
