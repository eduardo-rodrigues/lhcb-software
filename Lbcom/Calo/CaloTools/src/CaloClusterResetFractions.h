// $Id: CaloClusterResetFractions.h,v 1.3 2001-12-09 14:33:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/25 15:50:40  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.1  2001/11/08 20:07:04  ibelyaev
//  new tools are added into  the package
// 
// ============================================================================
#ifndef CALOTOOLS_CALOCLUSTERRESETFRACTIONS_H 
#define CALOTOOLS_CALOCLUSTERRESETFRACTIONS_H 1
// Include files
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"

/** @class CaloClusterResetFractions CaloClusterResetFractions.h
 *  
 *  This concrete Tool reset energy fractions and weigths,
 *  which are set by any energy shareing procedure, thus
 *  it is "undo" for energy-shareing procedure 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/11/2001
 */

class CaloClusterResetFractions : 
  public virtual ICaloClusterTool ,
  public                 CaloTool
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloClusterResetFractions>;
  
public:
  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode    initialize ();
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
  /** query interafce method  
   *  @param  iiD  unique interface identifier 
   *  @param  pI   placeholder for interface 
   *  @return status code 
   */
  virtual StatusCode queryInterface 
  ( const InterfaceID& iiD ,
    void**             pI  );

  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloCluster* cluster ) const ;
  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const ;
  
  /** The main processing method with hypothesis 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @param hypo    processing hypothesis 
   *  @return status code 
   */  
  virtual StatusCode process    
  ( CaloCluster* cluster                   , 
    const CaloHypotheses::Hypothesis& hypo ) const;
  
protected:

  /** Standard constructor
   *  @param type   tool type 
   *  @param name   tool name 
   *  @param parent tool parent 
   */
  CaloClusterResetFractions( const std::string& type   ,
                             const std::string& name   ,
                             const IInterface*  parent );

  /** destructor, protected and virtual
   */
  virtual ~CaloClusterResetFractions( );

protected:

private:

};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOCLUSTERRESETFRACTIONS_H
// ============================================================================
