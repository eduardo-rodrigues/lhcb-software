// $Id: IParticle2MCAssociator.h,v 1.8 2009-03-26 11:06:33 jpalac Exp $
#ifndef KERNEL_IPARTICLE2MCASSOCIATOR_H 
#define KERNEL_IPARTICLE2MCASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb
{
  class Particle;
  class MCParticle;
}


static const InterfaceID IID_IParticle2MCAssociator ( "IParticle2MCAssociator", 1, 0 );

/** @class IParticle2MCAssociator IParticle2MCAssociator.h Kernel/IParticle2MCAssociator.h
 *  
 *  Tool interface to generate a single association between an 
 *  LHCb::Particle and return them to the user. The
 *  user is in control of the resulting association and invoking the interface
 *  methods should have no side-effects. TES access should be read-only.
 *  Interface implementations should not populate the TES with their results
 *  directly or indirectly, and should not have any runtime configuration
 *  that alters the result, except for the assumed TES location of the 
 *  LHCb::MCParticles for the relevant methods.
 * 
 *  @author Juan PALACIOS
 *  @date   2009-01-19
 */
class IParticle2MCAssociator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2MCAssociator; }
  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles from a TES location.
   * @param particle LHCb::Particle* to be associated
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle* 
  relatedMCP(const LHCb::Particle*) const = 0;

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles from a TES location.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of MCParticles
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle*,
             const std::string& mcParticleLocation) const = 0;

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of MCParticles
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle* particles,
             const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of MCParticles
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle* particles,
             const LHCb::MCParticle::Container& mcParticles) const = 0;


};
#endif // KERNEL_IPARTICLE2MCASSOCIATOR_H
