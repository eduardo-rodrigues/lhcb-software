#!/usr/bin/env python
# =============================================================================
# $Id: Phi.py,v 1.8 2004-08-06 12:12:03 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Python based phi -> K+ K- "analysis"
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule  import *
import benderconfig  as bender 
import benderPreLoad as preload

global h1 
# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Phi(Algo):
    " My own analysis algorithm for selection of phi -> K+ K- "
    def analyse ( self ) :
        # get kaons 
        kplus  = self.select( tag="K+" , cuts = 'K+' == ID )
        kminus = self.select( tag="K-" , cuts = 'K-' == ID )
        # create loop
        phis   = self.loop( formula = "K+ K-" , pid = "phi(1020)" )
        # loop over KK combinations 
        for phi in phis:
            if phi.mass(1,2) > 1050 * MeV : continue
            global h1
            h1 = self.plot ( title = "K+ K- mass"      ,
                             value = M ( phi ) / MeV   ,
                             low   = 1000              ,
                             high  = 1050              )
            phi.save('phi')
            
        return SUCCESS 
        

# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    bender.config( files   =
                   [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                     '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
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
    g.topAlg += ['LoKiPreLoad/Hadrons']
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    phi = Phi('Phi')
    
    g.topAlg += [ 'Phi' ]
    
    phi = gaudi.iProperty('Phi')
    phi.OutputLevel = 5
    
    desktop                 = g.property('Phi.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    # output histogram file 
    hsvc = g.property( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'phi.hbook'

    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 1000
    # configure the job
    configure() 
    # run job 
    g.run  ( nEvents )
    # terminate the Application Manager 
    g.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
