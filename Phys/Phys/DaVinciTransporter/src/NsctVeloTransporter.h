// $Id: NsctVeloTransporter.h,v 1.3 2005-01-06 10:58:17 pkoppenb Exp $
#ifndef NSCTVELOTRANSPORTER_H 
#define NSCTVELOTRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "Kernel/IParticleTransporter.h"

/** @class NsctVeloTransporter NsctVeloTransporter.h 
 *  Transport the particle in the Velo region considering
 *  multiple scattering. It uses the prescription in 
 *  LHCB-98-47 (F. Teubert, et al. equation (2) and L1Track.cpp).
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class NsctVeloTransporter : public GaudiTool, 
                            virtual public IParticleTransporter {
public:
  /// Standard constructor
  NsctVeloTransporter( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
  virtual ~NsctVeloTransporter( ){}; ///< Destructor
  
  /// Initialize.
  StatusCode initialize();
  
  /// Transport in VELO region according to linear extrapolation
  /// accounting for Multiple Scattering in material.
  StatusCode transport(ParticleVector::const_iterator &, 
                       const double znew,
                       Particle &transParticle);
  
  /// Transport in VELO region according to linear extrapolation
  /// accounting for Multiple Scattering in material.
  StatusCode transport(const Particle &, 
                       const double znew,
                       Particle &transParticle);
  
  
  /// Finalize.
  StatusCode finalize();
  
protected:
  
private:

  /// Method to extrapolate in VELO region for common part of public
  /// methods.   
  StatusCode veloTransport(const Particle *&, 
                           const double znew,
                           Particle &transParticle,
                           int ipz);

  double m_tolerance;                ///< Tolerance for z estimation
  double m_k;                        ///< 
  std::vector<double> m_zVelo;       ///< Vector with Velo stations position
  
};
#endif // NSCTVELOTRANSPORTER_H
