// $Id: ITsaSeedAddHits.h,v 1.3 2007-10-10 18:48:10 smenzeme Exp $
#ifndef _ITsaSeedAddHits_H
#define _ITsaSeedAddHits_H

#include "GaudiKernel/IAlgTool.h"

#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedHit.h"

static const InterfaceID IID_ITsaSeedAddHits( "ITsaSeedAddHits", 0, 0 );

#include <vector>

namespace Tf
{
  namespace Tsa
  {

    /** @class ITsaSeedAddHits ITsaSeedAddHits.h
     *
     *  Interface to Tsa Seed Hit Adder (!!)
     *
     *  @author M.Needham
     *  @date   12/11/2006
     */

    class ITsaSeedAddHits: virtual public IAlgTool {
    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ITsaSeedAddHits ; }

      virtual StatusCode execute(SeedTracks* seeds, SeedHits* hits) = 0;

    };

  }
}


#endif
