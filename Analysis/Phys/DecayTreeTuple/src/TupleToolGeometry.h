// $Id: TupleToolGeometry.h,v 1.10 2010-02-09 09:40:49 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLGEOMETRY_H
#define JBOREL_TUPLETOOLGEOMETRY_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IDistanceCalculator;
class DVAlgorithm;

namespace LHCb {
  class Particle;
  class VertexBase;
};


/** @class TupleToolGeometry TupleToolGeometry.h jborel/TupleToolGeometry.h
 *
 * \brief Fill geometry related information for DecayTreeTuple
 *
 * - head_MINIP : minimum impact parameter on any PV
 * - head_MINIPCHI2 : minimum chi2 IP on all PVs
 * - head_ENDVERTEX_[X|Y|Z] : decay vertex position for composite particles
 * - head_ENDVERTEX_[X|Y|Z]ERR : decay vertex position error estimate for composite particles
 * - head_ENDVERTEX_CHI2 : decay vertex chi2
 * - head_ENDVERTEX_NDOF : decay vertex nDoF
 * - head_OWNPV_[X|Y|Z] : related primary vertex position
 * - head_OWNPV_[X|Y|Z]ERR : related primary vertex position error estimate for composite particles
 * - head_OWNPV_CHI2 : related primary vertex chi2
 * - head_OWNPV_NDOF : related primary vertex nDoF
 * - head_IP_OWNPV : impact parameter with respect to the PhysDesktop::relatedVertex() considered particle 
 * - head_IPCHI2_OWNPV : impact parameter chi2 with respect to the PhysDesktop::relatedVertex() considered particle 
 * - head_FD_OWNPV : flight distance of composite particle wrt. the PhysDesktop::relatedVertex() considered particle 
 * - head_FDCHI2_OWNPV : flight distance significance in units of chi2 wrt. the PhysDesktop::relatedVertex() considered particle 
 * - head_DIRA_OWNPV : direction angle wrt. the PhysDesktop::relatedVertex() considered particle 
 *
 *  If Verbose is true:
 *
 *
 * - head_TOPPV_[X|Y|Z] : PhysDesktop::relatedVertex() of the top of decay chain position
 * - head_TOPPV_[X|Y|Z]ERR : PhysDesktop::relatedVertex() of the top of decay chain position error estimate
 * - head_TOPPV_CHI2 : PhysDesktop::relatedVertex() of the top of decay chain chi2
 * - head_TOPPV_NDOF : PhysDesktop::relatedVertex() of the top of decay chain nDoF
 * - head_IP_TOPPV : impact parameter with respect to the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_IPCHI2_TOPPV : impact parameter chi2 with respect to the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_FD_TOPPV : flight distance of composite particle wrt. the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_FDCHI2_TOPPV : flight distance significance in units of chi2 wrt. 
 *      the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_DIRA_TOPPV : direction angle wrt. the PhysDesktop::relatedVertex() of the top of decay chain
 *
 * - head_ORIVX_[X|Y|Z] : ancestor's related primary vertex position (when applicable)
 * - head_ORIVX_[X|Y|Z]ERR : ancestor's related primary vertex position error estimate (when applicable)
 * - head_ORIVX_CHI2 : ancestor's related primary vertex chi2 (when applicable)
 * - head_ORIVX_NDOF : ancestor's related primary vertex nDoF (when applicable)
 * - head_IP_ORIVX : impact parameter with respect to the ancestor's vertex (when applicable)
 * - head_IPCHI2_ORIVX : impact parameter chi2 with respect to the ancestor's vertex (when applicable)
 * - head_FD_ORIVX : flight distance of composite particle wrt. the ancestor's vertex (when applicable)
 * - head_FDCHI2_ORIVX : flight distance significance in units of chi2 wrt. ancestor's vertex (when applicable)
 * - head_DIRA_ORIVX : direction angle wrt. ancestor's vertex (when applicable)
 *
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolGeometry : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolGeometry( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  
  virtual ~TupleToolGeometry( ){}; ///< Destructor
  
  virtual StatusCode initialize();
  
  StatusCode fill( const LHCb::Particle*
		   , const LHCb::Particle*
		   , const std::string&
		   , Tuples::Tuple& );

private:

  /// fill end vertex stuff
  StatusCode fillVertexFull(const LHCb::VertexBase* vtx, 
		     const LHCb::Particle* P, 
		     std::string head, std::string vtx_name, Tuples::Tuple&) const ;

  /// origin vertex
  const LHCb::VertexBase* originVertex( const  LHCb::Particle*
                                        , const LHCb::Particle* ) const;
  /// fill related pV stuff
  StatusCode fillBPV(const LHCb::VertexBase*, const LHCb::Particle*, std::string, Tuples::Tuple&, std::string trail = "") const ;

  /// fill min IP
  StatusCode fillMinIP(const LHCb::Particle*, std::string, Tuples::Tuple&) const ;

  /// fill end vertex stuff
  StatusCode fillVertex(const LHCb::VertexBase* vtx, std::string vtx_name, Tuples::Tuple&) const ;

  /// fill flight
  StatusCode fillFlight(const LHCb::VertexBase* oriVtx, const LHCb::Particle*, 
                        std::string, Tuples::Tuple&, std::string trail = "") const ;

  const IDistanceCalculator* m_dist;

  int m_photonID; 
  int m_pi0ID; 
  int m_etaID;  
//bool m_fillMother;
  
  DVAlgorithm* m_dva;

};
#endif // JBOREL_TUPLETOOLGEOMETRY_H
