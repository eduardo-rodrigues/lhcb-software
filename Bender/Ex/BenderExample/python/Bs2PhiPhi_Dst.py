#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiPhi_Dst.py,v 1.3 2005-01-24 17:33:00 ibelyaev Exp $
# =============================================================================
# CVS version $Revisison:  $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Bs->phi phi stripping using Event Tag Collection
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule  import *

import benderPreLoad as     preload
from   Bs2PhiPhi     import Bs2PhiPhi

# =============================================================================
# job configuration
# =============================================================================
def configure () :
    # Generic job configuration & input data 
    
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$DAVINCIROOT/options/DstContent.opts'  ,   # dst content
                    '$LOKIEXAMPLEOPTS/Bs_phiphi_DC04.opts'  ] , # input data 
                  options =
                  [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                    'HcalPIDmu.OutputLevel     =   5  ' ,
                    'EcalPIDe.OutputLevel      =   5  ' ,
                    'HcalPIDe.OutputLevel      =   5  ' ,
                    'BremPIDe.OutputLevel      =   5  ' ,
                    'PrsPIDe.OutputLevel       =   5  ' ,
                    'NeutralPP2MC.OutputLevel  =   5  ' ,
                    'Hadrons.OutputLevel       =   5  ' ,
                    'EventSelector.PrintFreq   = 100  ' ] )
    
    # specific job configuration 
    # preload algorithm(s)
    gaudi.addAlgorithm( 'LoKiPreLoad/Hadrons' ) 
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    bs = Bs2PhiPhi('Bs2PhiPhi')
    
    #g.topAlg += [ 'Bs2PhiPhi' ]
    
    bs = gaudi.algorithm('Bs2PhiPhi')
    bs.OutputLevel = 5
    bs.NTupleLUN  = "PHIPHI"
    desktop = gaudi.tool('Bs2PhiPhi.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'phi_writedst.hbook'
    # ntuples 
    nsvc = gaudimodule.iProperty( 'NTupleSvc' )
    nsvc.Output = [ "PHIPHI  DATAFILE='bs2phiphi_writedst_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

    # output stream configuration 
    gaudi.OutStream = [ 'Sequencer/DstSeq' ]
    seq = gaudi.algorithm('DstSeq')
    seq.Members     = [ 'Bs2PhiPhi' , 'OutputStream/DstWriter' ]
    dst = gaudi.algorithm( 'DstWriter' )
    dst.Preload     = FALSE
    dst.Output      = "DATAFILE='PFN:Bs2PhiPhi.dst' TYP='POOL_ROOTTREE' OPT='CREATE'" ;

    return SUCCESS 
    
# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 10 )
    # terminate Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
