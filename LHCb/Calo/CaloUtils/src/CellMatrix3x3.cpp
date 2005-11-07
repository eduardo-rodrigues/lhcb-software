// $Id: CellMatrix3x3.cpp,v 1.4 2005-11-07 11:57:13 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/10/22 09:29:08  ibelyaev
//  bug fix in the evaluation of energy sharing fractions
//
// ============================================================================
// Include files
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// local 
#include "CaloUtils/CellMatrix3x3.h"

// ============================================================================
/** @file CellMatrix3x3.cpp  
 *  
 *  Implementation file for class : CellMatrix3x3
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 07/11/2001 
 */
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
CellMatrix3x3::~CellMatrix3x3() {}

// ============================================================================
/** if cells belong to different areas
 *  one need to treat them in a different way
 *  @param cell  the cell to be tested
 *  @return fraction of area of the second cell which fits inside 
 *          3x3 matrix aroud the seed cell
 */
// ============================================================================
double CellMatrix3x3::treatDifferentAreas ( const CaloCellID& seed ,
                                            const CaloCellID& cell ) const 
{
  /// valid detector
  if ( 0 == det() ) 
  { Exception ( "CellMatrix3x3::treatDifferentAreas: DeCalo=0!" ) ; }
  /// both cells are valid? 
  if ( !det()->valid( seed ) || !det()->valid( cell )  ) { return 0 ; }
  /// cell sizes 
  const double      sizeMtrx   =  0.5  * 3 * det()->cellSize   ( seed ) ;
  const double      sizeCell   =  0.5  *     det()->cellSize   ( cell ) ;
  /// cell area 
  const double      cellArea   = sizeCell * sizeCell * 4.0 ;
  /// cell centers 
  const HepPoint3D& centerMtrx =             det()->cellCenter ( seed ) ;
  const HepPoint3D& centerCell =             det()->cellCenter ( cell ) ;
  /// effective halfsize 
  const double      sizeMin    =  0.01 * mini ( sizeMtrx , sizeCell ) ;
  /// 
  const double      Area       = area ( centerMtrx , sizeMtrx ,
                                        centerCell , sizeCell ) ;
  ///
  if ( Area <= sizeMin )       { return 0; }  ///< RETURN 
  ///
  return Area / cellArea ;
};

// ============================================================================
// The End 
// ============================================================================
