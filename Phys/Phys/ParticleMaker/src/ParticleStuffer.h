// $Id: ParticleStuffer.h,v 1.6 2006-05-09 09:43:04 jpalac Exp $
#ifndef PARTICLESTUFFER_H 
#define PARTICLESTUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IParticleStuffer.h"
#include "Kernel/IParticleTransporter.h"

// Forward declarations
class IParticlePropertySvc;

/** @class ParticleStuffer ParticleStuffer.h
 *  Fill a particle given a vertex and a particle ID
 *  @author Paul Colrain
 *  @date   14/03/2002
 *  Modified by S. Amato to transport parameters to the vertex position 11/10/2002 
 */
class ParticleStuffer : public GaudiTool, 
                        virtual public IParticleStuffer {

public:

  /// Standard constructor
  ParticleStuffer( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);
  
  /// Desctructor
  virtual ~ParticleStuffer( ){ }; 

  /// Retrieve  the ParticlePropertyService.
  StatusCode initialize();    

  
  /// Fill Composite Particle from Vertex
  StatusCode fillParticle( const LHCb::Particle::ConstVector& daughters,
                           const LHCb::Vertex&,  
                           const LHCb::ParticleID&,
                           LHCb::Particle&);

  /// Fill Composite Particle from Vertex
  StatusCode fillParticle( const LHCb::Particle::ConstVector& daughters,
                           const LHCb::Vertex&,  
                           LHCb::Particle&);

  /// Fill Composite Particle from a state
  StatusCode fillParticle( const LHCb::State& state,
                           LHCb::Particle& par);

  /// Sum 4-vectors for a vector of daughters
  Gaudi::XYZTVector sumMomenta( const LHCb::Particle::ConstVector& );

  /// Sum 4-vectors for a vector of daughters
  Gaudi::XYZTVector sumMomenta( const SmartRefVector<LHCb::Particle>& );

private:
  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc
  IToolSvc* m_pToolSvc;
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use
  
};

#endif // PARTICLESTUFFER_H
