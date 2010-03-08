#ifndef CALOPHOTONESTIMATORTOOL_H
#define CALOPHOTONESTIMATORTOOL_H 1
// Include files
// Gaudi
#include "GaudiAlg/GaudiTool.h"
// Math
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// Event
#include "Event/Track.h"
#include "Event/CaloCluster.h"
// Associator
#include "Relations/IRelationWeighted.h"

/** @class CaloPhotonEstimatorTool CaloPhotonEstimatorTool.h
 *
 *  Photon Selection Tool
 *  (LHCb 2004-03)
 *
 *  @author Frederic Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */

// Forward declarations
class DeCalorimeter   ;
namespace LHCb {
  class CaloHypo;
}

class CaloPhotonEstimatorTool :  public         GaudiTool,
                                 virtual public ICaloHypoLikelihood{
public:
  CaloPhotonEstimatorTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );

  virtual ~CaloPhotonEstimatorTool();

  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual const LHCb::CaloHypo::Hypothesis& hypothesis () const ;
  
  virtual double operator() (const LHCb::CaloHypo* hypo ) const ;
  virtual double likelihood (const LHCb::CaloHypo* hypo ) const ;

  virtual double operator() (const LHCb::ProtoParticle* hypo ) const ;
  virtual double likelihood (const LHCb::ProtoParticle* hypo ) const ;


protected:
  inline unsigned int bin(const double value,
                          const std::vector<double> vect) const {
    unsigned int index=0;
    if (value>=vect[ vect.size()-1 ]) {return vect.size();}
    while ((index<vect.size()) && (value>=vect[ index ])) {index++;}
    return index;
  }

  double evalChi2(const LHCb::CaloCluster*) const ;
  double evalChi2(const LHCb::ProtoParticle*) const ;
  
  void evalParam(const LHCb::CaloHypo* ,
                   double &,
		   double &,
		   double &, 
		   double &,
		   double &,
		   unsigned int &) const ;
					       
  double evalLikelihood(double ,
			double ,
			double ,
			double ,
			double ,
			double ,
			unsigned int ) const ;				       
 private:

// Detector Information
  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detPrs;
  DeCalorimeter *m_detSpd;

  std::string m_nameOfECAL;
  std::string m_nameOfPRS;
  std::string m_nameOfSPD;

  Gaudi::Plane3D m_planeEcal     ;
  Gaudi::Plane3D m_planePrs      ;
  Gaudi::Plane3D m_planeSpd      ;

  // Hypothesis
  LHCb::CaloHypo::Hypothesis       m_hypothesis;

  // use tracking
  bool m_tracking;
  
  // Relations 
  mutable LHCb::Calo2Track::IClusTrTable*  m_table  ;
  std::string      m_tableLocation ;

//Vertex
  Gaudi::XYZPoint m_vertex    ;

// Photon Pdf

  typedef std::vector<double>  Data    ;
  
  Data                m_binEPrs;
  Data                m_binChi2;
  Data                m_binSeed;

  Data                m_ePrsXBin;
  Data                m_chi2XBin;
  Data                m_seedXBin;

  Data                m_ePrsYBin;
  Data                m_chi2YBin;
  Data                m_seedYBin;

  Data                m_signalEPrsData_0     ;
  Data                m_signalEPrsSpdData_0  ;
  Data                m_backgrEPrsData_0     ;
  Data                m_backgrEPrsSpdData_0  ;

  Data                m_signalChi2Data_0     ;
  Data                m_signalChi2SpdData_0  ;
  Data                m_backgrChi2Data_0     ;
  Data                m_backgrChi2SpdData_0  ;

  Data                m_signalSeedData_0     ;
  Data                m_signalSeedSpdData_0  ;
  Data                m_backgrSeedData_0     ;
  Data                m_backgrSeedSpdData_0  ;

  Data                m_signalEPrsData_1     ;
  Data                m_signalEPrsSpdData_1  ;
  Data                m_backgrEPrsData_1     ;
  Data                m_backgrEPrsSpdData_1  ;

  Data                m_signalChi2Data_1     ;
  Data                m_signalChi2SpdData_1  ;
  Data                m_backgrChi2Data_1     ;
  Data                m_backgrChi2SpdData_1  ;

  Data                m_signalSeedData_1     ;
  Data                m_signalSeedSpdData_1  ;
  Data                m_backgrSeedData_1     ;
  Data                m_backgrSeedSpdData_1  ;

  Data                m_signalEPrsData_2     ;
  Data                m_signalEPrsSpdData_2  ;
  Data                m_backgrEPrsData_2     ;
  Data                m_backgrEPrsSpdData_2  ;

  Data                m_signalChi2Data_2     ;
  Data                m_signalChi2SpdData_2  ;
  Data                m_backgrChi2Data_2     ;
  Data                m_backgrChi2SpdData_2  ;

  Data                m_signalSeedData_2     ;
  Data                m_signalSeedSpdData_2  ;
  Data                m_backgrSeedData_2     ;
  Data                m_backgrSeedSpdData_2  ;

  bool                m_extrapol;
 };

#endif // CALOPHOTONESTIMATORTOOL_H




