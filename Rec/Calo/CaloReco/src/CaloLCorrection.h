// $Id: CaloLCorrection.h,v 1.3 2006-05-30 09:42:03 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALORECO_CALOLCORRECTION_H 
#define CALORECO_CALOLCORRECTION_H 1
// Include files
// from STL
#include <string>
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// from Event 
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"
// Kernel
#include "Kernel/Point3DTypes.h"

/** @namespace CaloLCorrection_Local
 */
namespace CaloLCorrection_Local
{
  /** @class DigitFromCalo 
   *  simple utility to count digits from certain calorimeter 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 31/03/2002 
   */
  class DigitFromCalo : public std::unary_function<const LHCb::CaloDigit*,bool>
  { 
  public:
    /** constructor
     *  @param calo  calorimeter name 
     */
    explicit DigitFromCalo( const std::string& calo )
      : m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
    /** constructor
     *  @param calo  calorimeter index 
     */
    explicit DigitFromCalo( const int  calo )
      : m_calo(                                calo   ) {} ;
    /** the only essential method
     *  @param digit pointer to CaloDigit object 
     *  @return true if digit belongs to the predefined calorimeter 
     */
    inline bool operator() ( const LHCb::CaloDigit* digit ) const 
    {
      if( 0 == digit ) { return false ; }
      return ( ( (int) digit->cellID().calo() ) == m_calo ) ;
    };
  private:
    /// default constructor is private 
    DigitFromCalo();
  private:
    int m_calo ;
  };
};
// ============================================================================


/** @class CaloLCorrection CaloLCorrection.h
 *  
 *
 *  @author XXX-XXX xxx@xxx.com 
 *  @date   2003-03-10
 */
class CaloLCorrection : 
  public virtual ICaloHypoTool ,
  public              GaudiTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloLCorrection>;
  
public:
  
  /** The main processing method
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
public:

  /** initialization of the tool 
   *  @see  GaudiTool 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** finalization of the tool 
   *  @see  GaudiTool 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see GaudiTool 
   *  @see  AlgTool 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent  tool parent 
   */
  CaloLCorrection ( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent ) ;
  
  /// destructor
  virtual ~CaloLCorrection () ;
  
private:
  
  /// default constructor is private 
  CaloLCorrection () ;
  /// copy constructor is private 
  CaloLCorrection           ( const CaloLCorrection& ) ;
  /// assignement operator is private 
  CaloLCorrection& operator=( const CaloLCorrection& ) ;
  
private:
  
  // internal types 
  typedef std::vector<LHCb::CaloHypotheses::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  typedef std::vector<double>                       Parameters  ;
  
  /// list of allowed hypos
  Hypotheses                                        m_hypos     ;
  Hypotheses_                                       m_hypos_    ;

  Parameters                                        Par_Al1 ;
  Parameters                                        Par_Al2 ;
  Parameters                                        Par_Al3 ;
  Parameters                                        Par_Be1 ;
  Parameters                                        Par_Be2 ;
  Parameters                                        Par_Be3 ;
  Parameters                                        Par_z0  ;

  /// evaluator of "cluster area"
  LHCb::ClusterFunctors::ClusterArea                      m_area      ;
  /// evaluator of "cluster calorimeter 
  LHCb::ClusterFunctors::ClusterFromCalo                  m_calo      ;
  /// evaluator of calorimeter for digit 
  CaloLCorrection_Local::DigitFromCalo              m_spd       ;
  /// evaluator of calorimeter for digit 
  CaloLCorrection_Local::DigitFromCalo              m_prs       ;
  
  std::string m_detData;
  const DeCalorimeter* m_det;
  

};
// ============================================================================
#endif // CALORECO_CALOLCORRECTION_H
