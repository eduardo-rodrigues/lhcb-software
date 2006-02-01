
/** @file DeRich.h
 *
 *  Header file for detector description class : DeRich
 *
 *  CVS Log :-
 *  $Id: DeRich.h,v 1.17 2006-02-01 16:20:49 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH_H
#define RICHDET_DERICH_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"

// Mathcore
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Plane3DTypes.h"

// LHCbKernel
#include "Kernel/RichSide.h"

#include "RichDet/RichMirrorSegPosition.h"
#include "RichDet/DeRichSphMirror.h"

/** @namespace DeRichLocation
 *
 *  Namespace for the xml location of the detector elements Rich1 & Rich2
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichLocation {

  /// Rich1 location in transient detector store
  static const std::string& Rich1_old = "/dd/Structure/LHCb/Rich1";
  static const std::string& Rich1 = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1";

  /// Rich2 location in transient detector store
  static const std::string& Rich2_old = "/dd/Structure/LHCb/Rich2";
  static const std::string& Rich2 = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2";

}

/** @class DeRich DeRich.h
 *
 * Class for generic info about the Rich(1/2) system.
 * DeRich is an abstract class contaning the common parts of the DeRich1
 * and DeRich2 classes.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
class DeRich: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRich();

  /**
   * Default destructor
   */
  virtual ~DeRich();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Returns the nominal centre of curvature of the spherical mirror for
   * this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal centre of curvature
   */
  virtual const Gaudi::XYZPoint& nominalCentreOfCurvature(const Rich::Side side) const = 0;

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal normal vector
   */
  virtual const Gaudi::XYZVector& nominalNormal(const Rich::Side side) const = 0;

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal flat mirror plane
   */
  virtual const Gaudi::Plane3D& nominalPlane(const Rich::Side side) const = 0;

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  virtual Rich::Side side( const Gaudi::XYZPoint& point ) const = 0;


  /**
   * Returns the nominal spherical mirror radius for this Rich
   *
   * @return The nominal spherical mirror radius
   */
  inline double sphMirrorRadius() const
  {
    return m_sphMirrorRadius;
  }

  /**
   * Returns a pointer to the tabulated property that holds the refractive
   * index for the gas window for this Rich
   *
   * @return Pointer to gas window refIndex
   */
  inline const TabulatedProperty* gasWinRefIndex() const
  {
    return m_gasWinRefIndex;
  }

  /**
   * Returns a pointer to the tabulated property that holds the absorption
   * length of the gas window for this Rich
   *
   * @return Pointer gas window absorption length
   */
  inline const TabulatedProperty* gasWinAbsLength() const
  {
    return m_gasWinAbsLength;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal quantum
   * efficiency of an HPD.
   *
   * @return Pointer to quantum efficiency (can be null)
   */
  inline const TabulatedProperty* nominalHPDQuantumEff() const
  {
    return m_HPDQuantumEff;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal reflectivity
   * of the spherical mirror.
   *
   * @return Pointer to nominal spherical mirror reflectivity
   */
  inline const TabulatedProperty* nominalSphMirrorRefl() const
  {
    return m_nominalSphMirrorRefl;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal reflectivity
   * of the flat mirror.
   *
   * @return Pointer to nominal flat mirror reflectivity
   */
  inline const TabulatedProperty* nominalSecMirrorRefl() const
  {
    return m_nominalSecMirrorRefl;
  }

  /**
   * Method to find the row/column of a spherical mirror segment.  It can
   * be used to test if the mirror segment is at the edge or not
   * @return Position (row/column) for this spherical mirror segment
   */
  virtual RichMirrorSegPosition sphMirrorSegPos( const int mirrorNumber ) const;

  /**
   * Method to find the row/column of a flat mirror segment. It can be used to
   * test if the mirror segment is at the edge or not
   *
   * @return Position (row/column) for this flat mirror segment
   */
  virtual RichMirrorSegPosition secMirrorSegPos( const int mirrorNumber ) const;

  /** Returns the name of this particular detector
   *  @return detector name
   */
  inline const std::string& myName() const { return m_name; }

  virtual StatusCode alignSphMirrors() = 0;
  virtual StatusCode alignSecMirrors() = 0;
  

protected:
  
  StatusCode alignMirrors( std::vector<const ILVolume*> mirrorContainer,
                           const std::string& mirrorID,
                           SmartRef<Condition> cond,
                           const std::string& Rvector ) const;

  double m_sphMirrorRadius; ///< The nominal radius of the spherical mirror

  /// The nominal centre of curvature of the spherical mirror (positive side)
  Gaudi::XYZPoint  m_nominalCentreOfCurvature;

  /// The nominal normal vector of the flat mirror plane (positive side)
  Gaudi::XYZVector m_nominalNormal;

  int m_sphMirrorSegRows;  ///< number of spherical mirror rows
  int m_sphMirrorSegCols;  ///< number of spherical mirror columns
  int m_secMirrorSegRows; ///< number of secondary mirror rows
  int m_secMirrorSegCols; ///< number of secondary mirror columns

  /// flag to test if the xml supports mirror position info
  bool m_positionInfo;

  /// refractive index of the quartz gas window
  const TabulatedProperty* m_gasWinRefIndex;
  /// absorption length of the quartz gas window
  const TabulatedProperty* m_gasWinAbsLength;
  /// HPD quantum efficiency
  const TabulatedProperty* m_HPDQuantumEff;
  /// spherical mirror reflectivity
  const TabulatedProperty* m_nominalSphMirrorRefl;
  /// flat mirror reflectivity
  const TabulatedProperty* m_nominalSecMirrorRefl;

  SmartRef<Condition> m_sphMirAlignCond;
  SmartRef<Condition> m_secMirAlignCond;

  std::string m_name; ///< The name of this detector

  
};

#endif    // RICHDET_DERICH_H
