
//---------------------------------------------------------------------------------
/** @file IRichRayTraceCherenkovCone.h
 *
 *  Header file for RICH reconstruction tool interface : IRichRayTraceCherenkovCone
 *
 *  CVS Log :-
 *  $Id: IRichRayTraceCherenkovCone.h,v 1.6 2004-08-19 14:07:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/07/26 18:00:57  jonrob
 *  Various improvements to the doxygen comments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H
#define RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H 1

// Event model
#include "Event/RichRecRing.h"
class RichRecSegment;

// RichKernel
#include "RichKernel/RichTraceMode.h"

/// Static Interface Identification
static const InterfaceID IID_IRichRayTraceCherenkovCone( "IRichRayTraceCherenkovCone", 1, 0 );

/** @class IRichRayTraceCherenkovCone IRichRayTraceCherenkovCone.h
 *
 *  Interface for tool to ray trace cherenkov photons in a cone around a given
 *  RichRecSegment direction, at the given angle or the angle correct for a given mass
 *  hypothesis.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichRayTraceCherenkovCone : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRayTraceCherenkovCone; }

  /** Ray trace the Cherenkov cone for the given segment and mass hypothesis
   *  to the detector plane. Assummed emission point is the best average for the
   *  given RichRecSegment and the direction around which the cone is projected
   *  is the best average direction for the segment.
   *
   *  @param segment Pointer to the RichRecSegment to raytrace from
   *  @param id      The mass hypothesis to use to calculate the Cherenkov cone angle
   *  @param points  On return the vector of points (in global coordinates)
   *                 on the HPD detector plane that lie on the Cherenkov ring.
   *  @param mode    The ray-tracing mode configuration object
   *
   *  @return StatusCode for the ray tracing
   *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
   *  @retval StatusCode::FAILURE Ray tracing failed
   */
  virtual StatusCode rayTrace ( RichRecSegment * segment,
                                const Rich::ParticleIDType id,
                                std::vector<HepPoint3D> & points,
                                const RichTraceMode mode = RichTraceMode() ) const = 0;

  /** Ray trace the Cherenkov cone for the given segment and Cherenkoc angle
   *  to the detector plane. Assummed emission point is the best average for the
   *  given RichRecSegment and the direction around which the cone is projected
   *  is the best average direction for the segment.
   *
   *  @param segment Pointer to the RichRecSegment to raytrace from
   *  @param ckTheta The Cherenkov angle to use
   *  @param points  On return the vector of points (in global coordinates)
   *                 on the HPD detector plane that lie on the Cherenkov ring.
   *  @param mode    The ray-tracing mode configuration object
   *
   *  @return StatusCode for the ray tracing
   *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
   *  @retval StatusCode::FAILURE Ray tracing failed
   */
  virtual StatusCode rayTrace ( RichRecSegment * segment,
                                const double ckTheta,
                                std::vector<HepPoint3D> & points,
                                const RichTraceMode mode = RichTraceMode() ) const = 0;

  /** Ray trace the Cherenkov cone using the given emission point, direction
   *  and Cherenkov angle.
   *
   *  @param rich          The RICH detector in which to ray trace
   *  @param emissionPoint The emission point for the ray tracing
   *  @param direction     The direction around which to project the Cherenkov cone
   *  @param ckTheta       The Cherenkov angle to use
   *  @param points        On return the vector of points (in global coordinates)
   *                       on the HPD detector plane that lie on the Cherenkov ring.
   *  @param mode          The ray-tracing mode configuration object
   *
   *  @return StatusCode for the ray tracing
   *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
   *  @retval StatusCode::FAILURE Ray tracing failed
   */
  virtual StatusCode rayTrace ( const Rich::DetectorType rich,
                                const HepPoint3D & emissionPoint,
                                const HepVector3D & direction,
                                const double ckTheta,
                                std::vector<HepPoint3D> & points,
                                const RichTraceMode mode = RichTraceMode() ) const = 0;

  /** Ray trace the Cherenkov cone for the given ring to the detector plane,
   *  in Global coordinates.
   *
   *  @param ring Point to the Ring object to use as the basis of the ray tracing
   *
   *  @return Reference to a vector of points (in global coordinates)
   *          on the detector plane that lie on the Cherenkov ring.
   */
  virtual const std::vector<HepPoint3D> & rayTrace ( RichRecRing * ring,
                                                     const RichTraceMode mode = RichTraceMode() ) const = 0;

  /** Ray trace the Cherenkov cone for the given ring to the detector plane,
   *  in Local HPD panel coordinates
   *
   *  @param ring Point to the Ring object to use as the basis of the ray tracing
   *
   *  @return Reference to a vector of points (in local HPD panel coordinates)
   *          on the detector plane that lie on the Cherenkov ring.
   */
  virtual const std::vector<HepPoint3D> & rayTraceLocal ( RichRecRing * ring,
                                                          const RichTraceMode mode = RichTraceMode() ) const = 0;

};

#endif // RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H
