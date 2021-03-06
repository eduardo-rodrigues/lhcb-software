// $Id: RichG4HistoFillSet4.h,v 1.2 2004-02-10 14:24:12 jonesc Exp $
#ifndef RICHANALYSIS_RICHG4HISTOFILLSET4_H
#define RICHANALYSIS_RICHG4HISTOFILLSET4_H 1

// Include files
// Include files
#include "G4Event.hh"
#include <vector>

#include "GaussRICH/RichG4Hit.h"
#include "GaussRICH/RichG4ReconResult.h"

/** @class RichG4HistoFillSet4 RichG4HistoFillSet4.h RichAnalysis/RichG4HistoFillSet4.h
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-22
 */
class RichG4HistoFillSet4 {
public:
  /// Standard constructor
  RichG4HistoFillSet4( );

  virtual ~RichG4HistoFillSet4( ); ///< Destructor

  void FillRichG4HistoSet4(RichG4Hit* acHit,
                           RichG4ReconResult* aRichG4ReconResult );

protected:

private:

};
#endif // RICHANALYSIS_RICHG4HISTOFILLSET4_H
