
//-----------------------------------------------------------------------------
/** @file RichRecMoniToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniToolBase.cpp,v 1.2 2005-02-02 10:01:20 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecMoniToolBase.h"

// Standard constructor, initializes variables
RichRecMoniToolBase::RichRecMoniToolBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichMoniToolBase ( type, name, parent ),
    m_pixTool  (0),
    m_tkTool   (0),
    m_segTool  (0),
    m_photTool (0),
    m_statTool (0) { }

StatusCode RichRecMoniToolBase::initialize()
{
  // base class initilize
  return RichMoniToolBase::initialize();
}

StatusCode RichRecMoniToolBase::finalize()
{
  // base class finalize
  return RichMoniToolBase::finalize();
}
