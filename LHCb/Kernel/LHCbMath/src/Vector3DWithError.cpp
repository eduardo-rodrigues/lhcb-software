// $Id: Vector3DWithError.cpp,v 1.1 2009-06-12 14:57:18 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD& STL 
// ============================================================================
#include <sstream>
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/Vector3DWithError.h"
#include "LHCbMath/Point3DWithError.h"
#include "LHCbMath/MatrixTransforms.h"
#include "LHCbMath/SymPosDefMatrixInverter.h"
#include "LHCbMath/MatrixInversion.h"
// ============================================================================
/** @file 
 *  Implementation file for class Gaudi::Math::PointWithError
 *  @author Vanya BELYAEV Ivane.BElyaev@nikhef.nl
 *  @date 20090603
 */
// ============================================================================
// constructor form the point and covariance matrix
// ============================================================================
Gaudi::Math::Vector3DWithError::Vector3DWithError 
( const Gaudi::Math::Vector3DWithError::Vector3D&   vct    ,
  const Gaudi::Math::Vector3DWithError::Covariance& matrix ) 
  : Gaudi::XYZVector ( vct )  
  , m_cov2  ( matrix ) 
{}
// ============================================================================
Gaudi::Math::Vector3DWithError& 
Gaudi::Math::Vector3DWithError::operator+= 
( const Gaudi::Math::Vector3DWithError& right ) 
{
  vector3d () += right.value() ;
  m_cov2      += right.cov2 () ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError& 
Gaudi::Math::Vector3DWithError::operator-= 
( const Gaudi::Math::Vector3DWithError& right ) 
{
  vector3d () -= right.value() ;
  m_cov2      += right.cov2 () ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError& 
Gaudi::Math::Vector3DWithError::operator+= 
( const Gaudi::Math::Vector3DWithError::Vector3D& right ) 
{
  vector3d () += right ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError& 
Gaudi::Math::Vector3DWithError::operator-= 
( const Gaudi::Math::Vector3DWithError::Vector3D& right ) 
{
  vector3d () -= right;
  return *this ;
}
// ============================================================================
// scaling
// ============================================================================
Gaudi::Math::Vector3DWithError& 
Gaudi::Math::Vector3DWithError::operator*= ( const double v ) 
{
  vector3d () *=  v    ;
  m_cov2      *= (v*v) ;
  return *this ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError& 
Gaudi::Math::Vector3DWithError::operator/= ( const double v ) 
{
  vector3d () /=  v    ;
  m_cov2      /= (v*v) ;
  return *this ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::Vector3DWithError::chi2 
( const Gaudi::Math::Vector3DWithError& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  s_cov2 += right.cov2() ;
  // use Manuel's inverter:
  const bool fail = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
  if  ( fail ) { return -1 ; }                                 // RETURN  
  /// calculate chi2 
  return Gaudi::Math::Similarity ( vector3d() - right.vector3d() , s_cov2 ) ;
}
// ============================================================================
// chi2 distance 
// ============================================================================
double Gaudi::Math::Vector3DWithError::chi2
( const Gaudi::XYZVector& right ) const 
{
  Covariance s_cov2 ( cov2() ) ;
  // use Manuel's inverter:
  const bool fail = Gaudi::Math::invertPosDefSymMatrix ( s_cov2 ) ;
  if  ( fail ) { return -1 ; }                                 // RETURN  
  /// calculate chi2 
  return Gaudi::Math::Similarity ( vector3d() - right , s_cov2 ) ;
}
// ============================================================================
// printout 
// ============================================================================
namespace 
{
  inline double err ( double cov ) 
  { return 0 <= cov ? std::sqrt ( cov ) : -std::sqrt(-cov) ; } 
}
// ============================================================================
std::ostream& 
Gaudi::Math::Vector3DWithError::fillStream ( std::ostream& s ) const // printout 
{
  return s << "(" 
           << X () << "+-" << err ( m_cov2(0,0) ) << ","
           << Y () << "+-" << err ( m_cov2(1,1) ) << ","
           << Z () << "+-" << err ( m_cov2(2,2) ) << ")";
}  
// ============================================================================
// conversion to the string 
// ============================================================================
std::string
Gaudi::Math::Vector3DWithError::toString   () const // conversion to the string 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// unary- 
// ============================================================================
Gaudi::Math::Vector3DWithError 
Gaudi::Math::Vector3DWithError::operator-() const 
{ return Gaudi::Math::Vector3DWithError ( -value() , cov2() ) ; }



// ============================================================================
// operators 
// ============================================================================
Gaudi::Math::Vector3DWithError 
Gaudi::Math::Vector3DWithError::__add__ 
( const Gaudi::Math::Vector3DWithError& right ) const 
{
  Gaudi::Math::Vector3DWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError 
Gaudi::Math::Vector3DWithError::__add__ 
( const Gaudi::XYZVector& right ) const 
{
  Gaudi::Math::Vector3DWithError tmp ( *this ) ;
  return tmp += right ;
}
// ============================================================================
Gaudi::Math::Point3DWithError 
Gaudi::Math::Vector3DWithError::__add__ 
( const Gaudi::Math::Point3DWithError& right ) const 
{
  Gaudi::Math::Point3DWithError tmp ( right ) ;
  return tmp += (*this)  ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError 
Gaudi::Math::Vector3DWithError::__sub__ 
( const Gaudi::Math::Vector3DWithError& right ) const 
{
  Gaudi::Math::Vector3DWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError 
Gaudi::Math::Vector3DWithError::__sub__ 
( const Gaudi::XYZVector& right ) const 
{
  Gaudi::Math::Vector3DWithError tmp ( *this ) ;
  return tmp -= right ;
}
// ============================================================================
Gaudi::Math::Vector3DWithError 
Gaudi::Math::Vector3DWithError::__rsub__ 
( const Gaudi::XYZVector& right ) const 
{ return Gaudi::Math::Vector3DWithError ( right - value() , cov2() ) ; }
// ============================================================================






// ========================================================================
Gaudi::Math::Vector3DWithError operator- 
( const Gaudi::XYZPoint&               b ,
  const Gaudi::Math::Point3DWithError& a ) { return a.__rsub__ ( b ) ; }
Gaudi::Math::Vector3DWithError operator- 
( const Gaudi::Math::Point3DWithError& a , 
  const Gaudi::Math::Point3DWithError& b ) { return a.__sub__ ( b ) ; }
Gaudi::Math::Vector3DWithError operator- 
( const Gaudi::Math::Point3DWithError& a , 
  const Gaudi::XYZPoint&               b ) { return a.__sub__ ( b ) ; }
// ========================================================================


// ============================================================================
// The END 
// ============================================================================

