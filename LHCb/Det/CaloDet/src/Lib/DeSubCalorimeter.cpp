// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2001/12/09 14:16:18  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.9  2001/08/21 13:05:09  ibelyaev
// fix problems of user parameters on Win2K
//
// Revision 1.8  2001/08/21 11:03:57  ibelyaev
// parameter fix on Win2K
//
// Revision 1.7  2001/07/17 17:26:53  ibelyaev
// modifications to improve Doxygen documentation
//
// Revision 1.6  2001/07/12 21:53:39  ibelyaev
// adaptation for Win2K
//
// Revision 1.5  2001/06/30 10:29:03  ibelyaev
// modification in printout methods
//
// Revision 1.4  2001/06/29 10:26:18  ibelyaev
// update to use new features of DetDesc v7 package
//
// ============================================================================
#define CALODET_DESUBCALORIMETER_CPP 1
// ============================================================================

// CaloDet
#include "CaloDet/DeSubCalorimeter.h"

/** @file DeSubCalorimeter.cpp
 *
 *  Implementation of class :  DeSubCalorimeter
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Standard Constructors
// ============================================================================
DeSubCalorimeter::DeSubCalorimeter( const std::string& name )
  : DetectorElement     ( name )
  , m_size( 0.0 )
{};

// ============================================================================
// Destructor
// ============================================================================
DeSubCalorimeter::~DeSubCalorimeter(){};


// ============================================================================
/// object identification
// ============================================================================
const CLID& DeSubCalorimeter::clID() const
{ return DeSubCalorimeter::classID(); }

// ============================================================================
// standard initialization
// ============================================================================
StatusCode DeSubCalorimeter::initialize()
{
  /// initialize the base class
  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { return sc ; }
  ///
  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  ///
  Parameters pars( paramNames() );
  ///
  { /// cell size
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("CellSize") );
    if( pars.end() != it )
      {
		const double value = param<double>( *it );
        setSize( value ) ;
        pars.erase( it );
      }
    else
      { return StatusCode::FAILURE ; }
  }
  if( !pars.empty() ) {
    // some "extra" parameters.
    // should be an error??
  }
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
// standard printout to STL standard stream
// ============================================================================
std::ostream& DeSubCalorimeter::printOut( std::ostream& os ) const
{
  os << "\tDeSubCalorimeter"
     << " fullname ='"    << name ()
     << std::endl;
  os << "\t\t\tCellSize=" << std::setw(12) << m_size
     << std::endl;
  return os;
};

// ============================================================================
// standard printout to Gaudi standard stream
// ============================================================================
MsgStream&    DeSubCalorimeter::printOut( MsgStream&    os ) const
{
  os << "\tDeSubCalorimeter"
     << " fullname ='"    << name ()
     << endreq   ;
  os << "\t\t\tCellSize=" << std::setw(12) << m_size
     << endreq   ;
  return os;
};

// ============================================================================
