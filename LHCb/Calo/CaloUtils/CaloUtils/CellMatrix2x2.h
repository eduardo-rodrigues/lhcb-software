// $Id: CellMatrix2x2.h,v 1.4 2005-11-07 11:57:13 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/05/27 13:40:03  cattanem
// v4r7
//
// Revision 1.2  2001/11/08 20:04:22  ibelyaev
//  update and bug fix
//
// Revision 1.1  2001/11/07 17:24:48  ibelyaev
//  new utilities for determination of relative positions of cells
// 
// ============================================================================
#ifndef CALOUTILS_CELLMATRIX2X2_H 
#define CALOUTILS_CELLMATRIX2X2_H 1
// Include files
#include "CaloUtils/CellMatrix.h"

/** @class CellMatrix2x2 CellMatrix2x2.h CaloUtils/CellMatrix2x2.h
 *  
 *  
 *  The simple utility class to determine if 2 cells belongs 
 *  to the same 2x2 sub-matrix  
 *
 *  @author Ivan Belyaev
 *  @date   07/11/2001
 */
class CellMatrix2x2 : public CellMatrix
{
  
public:
  
  enum SubMatrixType
  { 
    UpperLeft        = 0 ,
    UpperRight       = 1 ,
    LowerLeft        = 2 , 
    LowerRight       = 3 ,
    Max              = 4
  };
  
public:
  
  /** Standard constructor
   *  @param Det  pointer to calorimeter 
   *  @param Type sub-matrix type 
   */
  CellMatrix2x2( const DeCalorimeter* Det  = 0         ,
                 const SubMatrixType  Type = UpperLeft )
    : CellMatrix( Det  ) 
    , m_type    ( Type )
  {};
  
  virtual ~CellMatrix2x2(); ///< Destructor
  
  /** accessor to submatrix type 
   *  @return sub-matrix type 
   */
  inline SubMatrixType type() const { return m_type ; }
  
  /** set new  submatrix type 
   *  @param newT new value for sub-matrix type 
   */
  inline void setType ( const SubMatrixType & newT ) const 
  { m_type = newT ; }
  
  /** the major method. It returns "true" if the cell 
   *  belongs to 2x2 sub-matrix of given type around the seed 
   *  @param seed  the seed   cell
   *  @param cell  the cell to be tested
   *  @return fraction of area of the second cell which fits inside 
   *          2x2 matrix aroud the seed cell
   */
  inline double operator() ( const CaloCellID& seed , 
                             const CaloCellID& cell ) const 
  {
    /// the same cells
    if( seed             == cell             ) { return 1 ; } ///< RETURN
    /// the same calorimeter?
    if( seed.calo ()     != cell.calo ()     ) { return 0 ; } ///< RETURN 
    /// the same area ?
    if( seed.area ()     != cell.area ()     )  
      { return treatDifferentAreas( seed , cell ); }          ///< RETURN
    ///
    switch( type() ) 
      {
      case LowerLeft  :
        if( cell.col ()      >  seed.col ()     ) { return 0 ; }
        if( cell.col () + 1  <  seed.col ()     ) { return 0 ; }
        if( cell.row ()      >  seed.row ()     ) { return 0 ; }
        if( cell.row () + 1  <  seed.row ()     ) { return 0 ; }
        /// ok 
        return 1 ;
        break ;
      case LowerRight :
        if( cell.col ()      >  seed.col () + 1 ) { return 0 ; }
        if( cell.col ()      <  seed.col ()     ) { return 0 ; }
        if( cell.row ()      >  seed.row ()     ) { return 0 ; }
        if( cell.row () + 1  <  seed.row ()     ) { return 0 ; }
        /// ok 
        return 1 ;
        break ;
      case UpperLeft  :
        if( cell.col ()      >  seed.col ()     ) { return 0 ; }
        if( cell.col () + 1  <  seed.col ()     ) { return 0 ; }
        if( cell.row ()      >  seed.row () + 1 ) { return 0 ; }
        if( cell.row ()      <  seed.row ()     ) { return 0 ; }
        /// ok 
        return 1 ;
        break ;
      case UpperRight :
        if( cell.col ()      >  seed.col () + 1 ) { return 0 ; }
        if( cell.col ()      <  seed.col ()     ) { return 0 ; }
        if( cell.row ()      >  seed.row () + 1 ) { return 0 ; }
        if( cell.row ()      <  seed.row ()     ) { return 0 ; }
        /// ok 
        return 1 ;
        break ;
      default         :
        return 0 ;
        break;
      }
    /// return ???
    return 0 ;
  };
  
protected:
  
  /** if cells belong to diffeent areas
   *  one need to treat them in a different way
   *  @exception CaloException if detector information is not available
   *  @param seed  the seed cell
   *  @param cell  the cell to be tested
   *  @return fraction of area of the second cell which fits inside 
   *          2x2 sub-matrix of given type aroud the seed cell
   */
  double treatDifferentAreas( const CaloCellID& seed ,
                              const CaloCellID& cell ) const ;
  
private:
  
  mutable SubMatrixType   m_type ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOUTILS_CELLMATRIX2X2_H
// ============================================================================
