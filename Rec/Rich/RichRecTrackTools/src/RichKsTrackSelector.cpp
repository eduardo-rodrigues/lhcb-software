
//-----------------------------------------------------------------------------
/** @file RichKsTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::KsTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichKsTrackSelector.cpp,v 1.1.1.1 2007-11-26 17:28:18 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichKsTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( KsTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
KsTrackSelector::KsTrackSelector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : BaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<IBaseTrackSelector>(this);
  // redefine default cut values for Ks tracks
  m_minPCut     = 1.0;
}

//=============================================================================
// Destructor
//=============================================================================
KsTrackSelector::~KsTrackSelector() {}

//=============================================================================


