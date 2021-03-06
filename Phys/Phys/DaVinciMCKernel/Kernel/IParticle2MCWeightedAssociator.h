// $Id$
#ifndef KERNEL_IPARTICLE2MCWEIGHTEDASSOCIATOR_H 
#define KERNEL_IPARTICLE2MCWEIGHTEDASSOCIATOR_H 1

// from STL
#include <string>

#include "Kernel/Particle2MCParticle.h"
#include "Kernel/IParticle2MCAssociator.h"

/** @class IParticle2MCWeightedAssociator Kernel/IParticle2MCWeightedAssociator.h
 *  
 *  Tool interface to generate weighted associations between an 
 *  LHCb::Particle and return them to the user. The
 *  user is in control of the resulting associations
 *  Interface implementations should not populate the TES with their results,
 *  and should not have any runtime configuration
 *  that alters the result, except for the assumed TES location of the 
 *  LHCb::MCParticles for the relevant methods.
 *
 *  @author Juan PALACIOS
 *  @date   2009-03-25
 */
class GAUDI_API IParticle2MCWeightedAssociator : virtual public IParticle2MCAssociator
{

public: 

  /// Interface ID
  DeclareInterfaceID(IParticle2MCWeightedAssociator, 1, 0);

  /// Destructor
  virtual ~IParticle2MCWeightedAssociator() { }

public:

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles from the TES. The location handling is defined
   * by the implementation.
   * @param particle LHCb::Particle* to be associated
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @author Vladimir Gligorov
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle) const = 0;

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticle::Container
   *         from which to associate
   * @return Particle2MCParticle::ToVector containing sorted weighted 
   *         associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const std::string& mcParticleLocation) const = 0;

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of LHCb::MCParticle from which to associate
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::Container& mcParticles) const = 0;
  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of LHCb::MCParticle from which to associate
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

};

#endif // KERNEL_IPARTICLE2MCWEIGHTEDASSOCIATOR_H
