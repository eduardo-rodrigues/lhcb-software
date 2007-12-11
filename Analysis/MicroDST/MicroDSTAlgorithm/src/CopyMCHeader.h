// $Id: CopyMCHeader.h,v 1.2 2007-12-11 17:37:12 jpalac Exp $
#ifndef COPYMCHEADER_H 
#define COPYMCHEADER_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyRecHeader CopyRecHeader.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-02
 */
class CopyMCHeader : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyMCHeader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyMCHeader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  struct MCHeaderCopy 
  {
    LHCb::MCHeader* operator() (const LHCb::MCHeader* mcHeader) 
    { 
      return copy(mcHeader); 
    }
    
    static LHCb::MCHeader* copy(const LHCb::MCHeader* mcHeader)
    {

      LHCb::MCHeader* newHeader = new LHCb::MCHeader(*mcHeader);

      newHeader->clearPrimaryVertices();

      const SmartRefVector<LHCb::MCVertex>& PVs = 
        mcHeader->primaryVertices();

      for (SmartRefVector<LHCb::MCVertex>::const_iterator iPV = PVs.begin();
           iPV != PVs.end();
           ++iPV) newHeader->addToPrimaryVertices(*iPV);

      return newHeader;
    
    }

  };
  

};
#endif // COPYMCHEADER_H
