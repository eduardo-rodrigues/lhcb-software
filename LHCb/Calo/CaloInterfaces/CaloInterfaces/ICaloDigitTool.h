// $Id: ICaloDigitTool.h,v 1.2 2002-04-02 11:08:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.2  2001/11/02 16:53:13  ibelyaev
// New Package: the first release
//
// ============================================================================
#ifndef CALOINTERFACES_ICALODIGITTOOL_H
#define CALOINTERFACES_ICALODIGITTOOL_H 1
// Include files
// STD & STL
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// local
#include "CaloInterfaces/IIDICaloDigitTool.h"

class    CaloDigit                ;  ///< from CaloEvent

/** @class ICaloDigitTool ICaloDigitTool.h CaloInterfaces/ICaloDigitTool.h
 *
 *  The generic interface for "Calorimeter tools" , which deals with
 *  CaloDigit objects, the potential candidates are:
 *
 *    - digit calibration
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/10/2001
 */

class ICaloDigitTool:
  public virtual IAlgTool ,
  public std::unary_function<CaloDigit*,StatusCode>
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloDigitTool ; }

  /** The main processing method
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code
   */
  virtual StatusCode process    ( CaloDigit* digit ) const = 0 ;

  /** The main processing method (functor interface)
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code
   */
  virtual StatusCode operator() ( CaloDigit* digit ) const = 0 ;
  
  
  /** destructor, virtual
   */
  virtual ~ICaloDigitTool(){};

};

// ============================================================================
// The End
// ============================================================================
#endif // CALOINTERFACES_ICALODIGITTOOL_H
// ============================================================================
