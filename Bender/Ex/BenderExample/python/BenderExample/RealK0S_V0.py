#!/usr/bin/env python
# ===========================================================================# ==========================================================================================
## @file BenderExample/RealK0S_V0.py
#
#  The script to analyse the K0S from the V0-stripping
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-02-10
# ===========================================================================================
"""

The script to analyse the K0S from the V0-stripping

"""
# ===========================================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__     = "2010-02-10"
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

from   BenderExample.RealLam0_V0     import Lam0 

# =============================================================================
## @class Ks
#  Simple algorithm to study K0s -> pi+ pi-
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-01-11
class Ks(Lam0) :
    """
    Simple algorithm to study K0s -> pi+ pi-
    """    
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Lam0.initialize ( self )
        if sc.isFailure() : return sc

        self.nks1 = self.counter ( '#nK0S_1' )
        self.nks2 = self.counter ( '#nK0S_2' )

        return SUCCESS
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """
        
        ## get K0S from TES:
        
        kss = self.select ( 'k0s' , 'KS0' == ID )        
        self.nks1 += kss.size()
        
        if kss.empty() :
            return self.Warning ( 'No K0S are found' , SUCCESS )
        
        self.nks1 += kss.size()


        odin = self.get('DAQ/ODIN')
        runNum = odin.runNumber   ()
        evtNum = odin.eventNumber () 
        evt1   = int ( evtNum/2**32  ) 
        evt2   = int ( evtNum - 2**32*evt1 ) 
        
        tup  = self.nTuple ( 'Ks' )        
        
        for ks in kss :
            
            m = M ( ks ) / GeV 
            self.plot ( m , 'Ks - mass 1', 0.4 , 0.6 ) 
            
            pi1  = ks.child(1) 
            pi2  = ks.child(2)
            
            long1 = True if ISLONG ( pi1  ) else False  
            long2 = True if ISLONG ( pi2  ) else False  
            
            down1 = True if ISDOWN ( pi1  ) else False  
            down2 = True if ISDOWN ( pi2  ) else False  
            
            if   long1 and long2 :
                self.plot ( m , 'Ks - mass, LL-1', 0.4 , 0.4 ) 
            elif down1 and down2 :
                self.plot ( m , 'Ks - mass, DD-1', 0.4 , 0.6 )
            else :        
                self.plot ( m , 'Ks - mass, LD-1', 0.4 , 0.6 )
                
            ksVertex = ks.endVertex()
            
            chi2 = VCHI2 ( ksVertex )
            if not 0 <= chi2 < 25 : continue
            
            self.plot ( m , 'Ks  mass, chi2(vx)<25', 0.4 , 0.6 ) 
            
            z = VZ ( ksVertex ) 
            
            if z > 220 * cm : continue

            tup.column ( 'run'     , runNum         )
            tup.column ( 'evt1'    , evt1           )
            tup.column ( 'evt2'    , evt2           )
            tup.column ( 'bxid'    , odin.bunchId() )
            tup.column ( 'bxtype'  , odin.bunchCrossingType() )
            tup.column ( 'trgtype' , odin.triggerType() )
            
            ## get the related primary vertex 
            bpv = self.bestPV ( ks )
            
            if not not bpv :
                _ltime  = LTIME        ( self.ltfitter , bpv )
                _ltchi2 = LTIMEFITCHI2 ( self.ltfitter , bpv )
                
                ltime  = _ltime  ( ks ) * c_light
                ltchi2 = _ltchi2 ( ks )
                pvz    = VZ ( bpv )
                
            else :
                self.Warning( 'No best-PV is found!', SUCCESS )
                continue 

            mips   = IPCHI2 ( bpv , self.geo() )
            
            mips1 = mips ( pi1 ) 
            mips2 = mips ( pi2 )
            
            tup.column ( "mips0" , mips ( ks ) )
            tup.column ( "mips1" , mips1 )
            tup.column ( "mips2" , mips2 ) 

            self.tupPV ( tup , bpv )
            
            if   down1 and down2 and ltime < 10 * mm : continue
            elif                     ltime <  1 * mm : continue 
            
            tup.column ( "p0"    , P  ( ks  ) / GeV ) 
            tup.column ( "p1"    , P  ( pi1 ) / GeV ) 
            tup.column ( "p2"    , P  ( pi2 ) / GeV ) 
            tup.column ( "pt0"   , PT ( ks  ) / GeV ) 
            tup.column ( "pt1"   , PT ( pi1 ) / GeV ) 
            tup.column ( "pt2"   , PT ( pi2 ) / GeV )
            
            tup.column ( "pKs"   , ks  . momentum ( ) / GeV ) 
            tup.column ( "pPi1"  , pi1 . momentum ( ) / GeV ) 
            tup.column ( "pPi2"  , pi2 . momentum ( ) / GeV ) 
            
            tup.column ( "lv01"  , LV01( ks )  )
            tup.column ( "z"     , z     )
            tup.column ( "chi2"  , chi2  )
            
            tup.column ( 'yKs'   , Y   ( ks   ) )
            tup.column ( 'yPi1'  , Y   ( pi1  ) )
            tup.column ( 'yPi2'  , Y   ( pi2  ) )
            
            tup.column ( 'y0Ks'  , Y0  ( ks   ) )
            tup.column ( 'y0Pi1' , Y0  ( pi1  ) )
            tup.column ( 'y0Pi2' , Y0  ( pi2  ) )
            
            tup.column ( 'etaKs'  , ETA ( ks  ) )
            tup.column ( 'etaPi1' , ETA ( pi1 ) )
            tup.column ( 'etaPi2' , ETA ( pi2 ) )
            
            tup.column ( 'long1'  , long1 )
            tup.column ( 'long2'  , long2 )
            
            tup.column ( 'down1'  , down1 )
            tup.column ( 'down2'  , down2 )
            
            tup.column ( 'trgh1'  , TRGHP( pi1 ) )
            tup.column ( 'trgh2'  , TRGHP( pi2 ) )
            
            tup.column ( 'trchi2dof1' , TRCHI2DOF ( pi1 ) )
            tup.column ( 'trchi2dof2' , TRCHI2DOF ( pi2 ) )
            
            tup.column ( 'ctau'  , ltime  )
            tup.column ( 'tchi2' , ltchi2 )
            tup.column ( 'pvz'   , pvz    )
            
            tup.column ( "m"  , m  )
            
            tup.write  ( )
            
            if   long1 and long2 :
                self.plot ( m , 'Ks  mass, LL' , 0.4  , 0.6 , 200 )    
            elif down1 and down2 :
                self.plot ( m , 'Ks  mass, DD' , 0.4  , 0.6 , 200 )
            else                 :
                self.plot ( m , 'Ks  mass, LD' , 0.4  , 0.6 , 200 )
                    
        return SUCCESS 

# =============================================================================
## configure the job 
def configure ( datafiles , catalogs = [] ) :
    """
    Job configuration 
    """
        
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    from GaudiConf.Configuration import FileCatalog   ## needed for job configuration 
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 


    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = False  ,
        PrintFreq     = 1000   ,
        EvtMax        = -1     , 
        HistogramFile = 'RealK0S_V0_Histos.root' 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )

    NTupleSvc (
        Output = [ "K0S DATAFILE='RealK0S_V0.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    gaudi = appMgr()
    
    alg = Ks(
        'Ks'              ,   ## Algorithm name
        NTupleLUN = 'K0S' ,   ## Logical unit for output file with N-tuples
        ## 
        InputPrimaryVertices = '/Event/Strip/Rec/Vertex/Primary', 
        InputLocations = [ '/Event/Strip/Phys/StrippingK0S' ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    ## gaudi.setAlgorithms ( [] ) 
    
    return SUCCESS 

# =============================================================================
# The actual job steering
if '__main__' == __name__ :

    from Configurables  import EventSelector
    
    EventSelector( PrintFreq = 1000 )

    
    import BenderExample.V0Stripping
    
    configure ( [] )

    gaudi = appMgr()
    
    run ( -1 )
    
    myalg = gaudi.algorithm ( 'Ks' )

    import GaudiPython.HistoUtils
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 25 , False )
        

# =============================================================================
# The END 
# =============================================================================
