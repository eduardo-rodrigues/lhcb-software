
//-----------------------------------------------------------------------------
/** @file RichRecPixelParentType.cpp
 *
 *  Implementation ile for RichRecPixel enumerations
 *
 *  CVS Log :-
 *  $Id: RichRecPixelParentType.cpp,v 1.7 2005-04-20 16:21:20 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecPixelParentType.h"

// Text conversion for Rich::RecPixel::ParentType enumeration
std::string Rich::text( const Rich::PixelParent::Type & parent )
{
  switch( parent )
    {
    case Rich::PixelParent::RawBuffer:   return "RawBuffer";
    case Rich::PixelParent::Digit:       return "RichDigit";
    case Rich::PixelParent::MCHit:       return "MCRichHit";
    case Rich::PixelParent::NoParent:    return "No Parent";
    default:                             return "SHOULD NEVER SEE THIS";
    }
}
