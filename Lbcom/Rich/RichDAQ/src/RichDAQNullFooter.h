
//=============================================================================================
/** @file RichDAQNullFooter.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::NullFooter
 *
 *  CVS Log :-
 *  $Id: RichDAQNullFooter.h,v 1.3 2007-02-01 17:42:29 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQNULLFOOTER_H
#define RICHDAQ_RICHDAQNULLFOOTER_H 1

#include "RichDAQFooterPDBase.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    //=============================================================================================
    /** @class NullFooter RichDAQNullFooter.h
     *
     *  Implements a null footer for the LHCB HPD data blocks
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   05/09/2006
     */
    //=============================================================================================

    class NullFooter : public FooterPDBase
    {

    public:

      /// Standard constructor
      NullFooter( ) : FooterPDBase(0) {}

      /// Destructor
      ~NullFooter( ) { }

      /// Does this foot have a parity word ?
      inline bool hasParityWord() const { return false; }

      /// Access the parity word
      inline Rich::DAQ::LongType parityWord() const { return 0; }

      /// Set the parity word
      inline void setParityWord( const Rich::DAQ::LongType /* word */ ) {;}

      /// Test the parity word
      inline bool testParityWord( const ShortType ) const { return true; }

    };

  }
}

#endif // RICHDAQ_RICHDAQNULLFOOTER_H
