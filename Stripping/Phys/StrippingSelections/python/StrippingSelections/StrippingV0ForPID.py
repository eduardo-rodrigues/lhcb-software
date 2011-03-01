#!/usr/bin/env python
# =============================================================================
# @file StripingSelections/StrippingV0ForPID.py
#
# Configuration file for PID V0 ( K0S and  Lambda0) stripping.
#
# This series of stripping lines are designed for the purpose of PID
# calibration/monitoring of the RICH and MUON systems. Tracks from these
# samples will be used to determine ID and mis-ID in each of these
# sub-detectors. In order to enhance the number of tracks reconstructed in
# the periphery of phase-space, the selections are binned in terms of the
# V0's rapidity and pT. Each bin is then assigned its own
# pre-scale.
# 
#
# Acknowledgemens:
#
# Vanya Belyaev
# The selection cuts used in this series of lines are essentially a tightened
# version of those developed by Vanya Belyaev for the original V0 stripping
# performed in 2009 at /sqrt{s} = 0.9 TeV. Furthermore, the selection code
# borrows heavily on his original stripping lines (StrippingV0Conf).
#
# For more information see:
#
#  http://indico.cern.ch/conferenceDisplay.py?confId=74536
#  http://indico.cern.ch/materialDisplay.py?contribId=13&sessionId=2&materialId=0&confId=78767
#
# @author Andrew Powell a.powell1@physics.ox.ac.uk
# @date 2010-09-05
# =============================================================================
"""

Usage:

from StrippingSelections import StrippingV0ForPID
config_V0ForPID = {
    'KS0LL_Prescale'   : 0.006,
    'KS0DD_Prescale'   : 0.006,
    'LamLL_Prescale'   : 0.025,
    'LamDD_Prescale'   : 0.022,
    'LamLLIsMUON_Prescale' : 0.72,
    'LamDDIsMUON_Prescale' : 0.47
    }
confV0ForPID = StrippingV0ForPID.StrippingV0ForPIDConf ('noPIDV0',config_V0ForPID)
stream.appendLines( confV0ForPID.lines() )


Results from running over:
$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py

with prescales above prescales:

StrippingReport                                                INFO Event 50000, Good event 46049
 |                                    *Decision name*|  *Rate*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |0.005234|       241|       |  22.768|       |       |       |
 |_StrippingSequenceStreamTest_                      |0.005234|       241|       |  22.763|       |       |       |
 |!StrippingK0SLLLinenoPIDV0                         |0.001064|        49|  0.000|   0.110|      0|      0|      2|
 |!StrippingK0SDDLinenoPIDV0                         |0.000956|        44|  0.000|   0.099|      0|      0|      0|
 |!StrippingLam0LLLinenoPIDV0                        |0.000847|        39|  0.000|   0.274|      0|      0|      3|
 |!StrippingLam0DDLinenoPIDV0                        |0.000977|        45|  0.000|   0.412|      0|      0|      2|
 |!StrippingLam0LLIsMUONLinenoPIDV0                  |0.000586|        27|  0.000|   9.225|      0|      0|     81|
 |!StrippingLam0DDIsMUONLinenoPIDV0                  |0.000869|        40|  0.000|  12.188|      0|      0|      0|

========================================================================================================================
                                     AlgorithmCorrelationsAlg.AlgorithmCorrelations
========================================================================================================================
    Algorithm                           Eff.       1        2        3        4        5        6        7        8
------------------------------------------------------------------------------------------------------------------------
  1 StrippingGlobal                     0.523% |  ####### 100.000% 100.000% 100.000% 100.000% 100.000% 100.000% 100.000%
  2 StrippingSequenceStreamTest         0.523% | 100.000%  ####### 100.000% 100.000% 100.000% 100.000% 100.000% 100.000%
  3 StrippingK0SLLLinenoPIDV0           0.106% |  20.332%  20.332%  #######   0.000%   0.000%   0.000%   0.000%   0.000%
  4 StrippingK0SDDLinenoPIDV0           0.096% |  18.257%  18.257%   0.000%  #######   0.000%   0.000%   0.000%   0.000%
  5 StrippingLam0LLLinenoPIDV0          0.085% |  16.183%  16.183%   0.000%   0.000%  #######   0.000%   7.407%   0.000%
  6 StrippingLam0DDLinenoPIDV0          0.098% |  18.672%  18.672%   0.000%   0.000%   0.000%  #######   0.000%   2.500%
  7 StrippingLam0LLIsMUONLinenoPIDV0    0.059% |  11.203%  11.203%   0.000%   0.000%   5.128%   0.000%  #######   0.000%
  8 StrippingLam0DDIsMUONLinenoPIDV0    0.087% |  16.598%  16.598%   0.000%   0.000%   0.000%   2.222%   0.000%  #######
========================================================================================================================    
"""
# =============================================================================
__author__  = 'Andrew Powell a.powell1@physics.ox.ac.uk'
__date__    = '2011-02-25'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $'
__all__ = ('StrippingV0ForPIDConf')
# =============================================================================

from Gaudi.Configuration       import *
from LHCbKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV

# =============================================================================
# @class StrippingV0ForPIDConf
#
# Configurable for PID V0 ( K0S and  Lambda0 ) stripping.
##
# @author Andrew Powell a.powell1@physics.ox.ac.uk
# @date 2010-09-05
# =============================================================================

from Configurables import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand        
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions, StdNoPIDsDownPions, StdNoPIDsProtons, StdNoPIDsDownProtons

import logging
log = logging.getLogger('StrippingPromptCharm')

# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    'TrackChi2'            :  5       ,          ## Track Chi2/ndof quality 
    'VertexChi2'           :  25      ,          ## Cut on Vertex chi2-quality
    'DeltaMassK0S'         :  50 * MeV,          ## Mass-window (half)-width for K0S 
    'DeltaMassLambda'      :  25 * MeV,          ## Mass-window (half)-width for Lambda 
    'MaxZ'                 : 220 * cm ,          ## Maximal allowed vertex Z-position
    'DaughtersIPChi2'      :  25      ,          ## minimal IP-chi2 for daughter particles  
    'LTimeFitChi2'         :  36      ,          ## Chi2 from LifeTime Fitter
    ##
    'WrongMassK0S'         :   9 * MeV,          ## Mass-window (half)-width for ppi hypthesis
    'WrongMassK0S_DD'      :  18 * MeV,          ## Mass-window (half)-width for ppi hypthesis (DD-case)
    ##
    'WrongMassLambda'      :  20 * MeV,          ## Mass-window (half)-width for pipi hypthesis
    'WrongMassLambda_DD'   :  40 * MeV,          ## Mass-window (half)-width for pipi hypthesis (DD-case)
    ##
    'CTauK0S'              :   1 * mm ,          ## Cut on c*tau for K0S 
    'CTauK0S_DD'           :  10 * mm ,          ## Cut on c*tau for K0S (DD-case)
    ##
    'CTauLambda0'          :   5 * mm ,          ## Cut on c*tau for Lambda0
    'CTauLambda0_DD'       :  20 * mm ,          ## Cut on c*tau for Lambda0 (DD-case)
    ##
    'Proton_IsMUONCut'     :  "(INTREE( (ABSID=='p+') & ISMUON ) )" , 
    ## 
    # Technicalities:
    #
    'Monitor'              : None    ,          ## Activate the monitoring ?
    ##
    "HLT"                  : "HLT_PASS_RE('Hlt1MB.*Decision')" ,  ## HLT-cut
    ##
    'K0S_Monitor'          : [
    ## define historam type (shortcut)
    "Histo  = Gaudi.Histo1DDef"  ,
    ## monitor LL-case
    "massLL     = monitor ( M / GeV , Histo ( 'K0S, LL-case' , 0.45 , 0.55 , 100 ) , 'M_LL'     ) " ,
    "pTLL       = monitor ( PT / GeV , Histo ( 'K0S, LL-case' , 0.00 , 1.20 , 100 ) , 'pT_LL'   ) " ,
    "yLL        = monitor ( Y , Histo ( 'K0S, LL-case' , 1.60 , 5.40 , 100 ) , 'y_LL'           ) " ,
    ## monitor DD-case
    "massDD     = monitor ( M / GeV , Histo ( 'K0S, DD-case' , 0.45 , 0.55 , 100 ) , 'M_DD'       ) " ,
    "pTDD       = monitor ( PT / GeV , Histo ( 'K0S, DD-case' , 0.00 , 2.00 , 100 ) , 'pT_DD'   ) " ,
    "yDD        = monitor ( Y , Histo ( 'K0S, DD-case' , 1.60 , 5.40 , 100 ) , 'y_DD'           ) " 
    ],
    ##
    'Lam0_Monitor'         : [
    ## define historam type (shortcut)
    "Histo  = Gaudi.Histo1DDef"  ,
    ## monitor LL-case
    "massLL     = monitor ( M / GeV , Histo ( 'Lambda0, LL-case' , 1.090 , 1.140 , 100 ) , 'M_LL'     ) " ,
    "pTLL       = monitor ( PT / GeV , Histo ( 'Lambda0, LL-case' , 0.00 , 2.00 , 100 ) , 'pT_LL'   ) " ,
    "yLL        = monitor ( Y , Histo ( 'Lambda0, LL-case' , 1.60 , 5.40 , 100 ) , 'y_LL'           ) " ,
    ## monitor DD-case
    "massDD     = monitor ( M / GeV , Histo ( 'Lambda0, DD-case' , 1.080 , 1.140 , 100 ) , 'M_DD'     ) " ,
    "pTDD       = monitor ( PT / GeV , Histo ( 'Lambda0, DD-case' , 0.00 , 2.00 , 100 ) , 'pT_DD'   ) " ,
    "yDD        = monitor ( Y , Histo ( 'Lambda0, DD-case' , 1.60 , 5.40 , 100 ) , 'y_DD'           ) " 
    ] ,
    'Preambulo'       : [
    ## import c_light
    "from GaudiKernel.PhysicalConstants import c_light" ,
    ## define DD-category of K0S 
    "DD =    CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) " ,
    ## define LL-category of K0S 
    "LL =    CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) "
    ]
    }

# =============================================================================
class StrippingV0ForPIDConf(LineBuilder) :    
    """
    Helper class to configure 'V0ForPID'-lines
    """
    __configuration_keys__ = ('KS0LL_Prescale',
                              'KS0DD_Prescale',
                              'LamLL_Prescale',
                              'LamDD_Prescale',
                              'LamLLIsMUON_Prescale',
                              'LamDDIsMUON_Prescale'
                              )

    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        if key : return _config[ key ]
        return _config
    
    ## constructor
    def __init__(self, name, config) :
        """
        Constructor
        """
        LineBuilder.__init__(self, name, config)
    
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        
        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                log.warning ('StrippingV0ForPID: new configuration: %-16s : %s ' %( key , _config[key] ) )

        self._name            = name

        self._trackChi2       = _config.pop ( 'TrackChi2'    ,    _default_configuration_ [ 'TrackChi2'      ] )
        self._vertexChi2      = _config.pop ( 'VertexChi2'   ,    _default_configuration_ [ 'VertexChi2'     ] )
        self._deltaMassK0S    = _config.pop ( 'DeltaMassK0S' ,    _default_configuration_ [ 'DeltaMassK0S'   ] )
        self._deltaMassLambda = _config.pop ( 'DeltaMassLambda' , _default_configuration_ [ 'DeltaMassLambda'] )
        self._maxZ            = _config.pop ( 'MaxZ' ,            _default_configuration_ [ 'MaxZ'           ] )
        self._daughtersIPChi2 = _config.pop ( 'DaughtersIPChi2' , _default_configuration_ [ 'DaughtersIPChi2'] )
        self._lTimeFitChi2    = _config.pop ( 'LTimeFitChi2' ,    _default_configuration_ [ 'LTimeFitChi2'   ] )
        
        self._wrongMassK0S       = _config.pop ( 'WrongMassK0S' ,     _default_configuration_ [ 'WrongMassK0S'      ] )
        self._wrongMassK0S_DD    = _config.pop ( 'WrongMassK0S_DD' ,  _default_configuration_ [ 'WrongMassK0S_DD'   ] )
        self._wrongMassLambda    = _config.pop ( 'WrongMassLambda' ,  _default_configuration_ [ 'WrongMassLambda'   ] )
        self._wrongMassLambda_DD = _config.pop ( 'WrongMassLambda_DD',_default_configuration_ [ 'WrongMassLambda_DD'] )
        
        self._cTauK0S        = _config.pop ( 'CTauK0S',         _default_configuration_ [ 'CTauK0S'        ] )
        self._cTauK0S_DD     = _config.pop ( 'CTauK0S_DD' ,     _default_configuration_ [ 'CTauK0S_DD'     ] )
        self._cTauLambda0    = _config.pop ( 'CTauLambda0' ,    _default_configuration_ [ 'CTauLambda0'    ] )
        self._cTauLambda0_DD = _config.pop ( 'CTauLambda0_DD' , _default_configuration_ [ 'CTauLambda0_DD' ] )

        self._protonIsmuon   = _config.pop ( 'Proton_IsMUONCut',_default_configuration_ [ 'Proton_IsMUONCut'] )
        
        self._KS0LLPrescale   = config['KS0LL_Prescale']
        self._KS0DDPrescale   = config['KS0DD_Prescale']
        self._LamLLPrescale   = config['LamLL_Prescale']
        self._LamDDPrescale   = config['LamDD_Prescale']
        self._LamLLIsMUONPrescale   = config['LamLLIsMUON_Prescale']
        self._LamDDIsMUONPrescale   = config['LamDDIsMUON_Prescale']

        self._monitor      = _config.pop ( 'Monitor',      _default_configuration_ [ 'Monitor'     ] )
        self._hlt          = _config.pop ( 'HLT',          _default_configuration_ [ 'HLT'         ] )
        self._k0smonitor   = _config.pop ( 'K0S_Monitor',  _default_configuration_ [ 'K0S_Monitor' ] )
        self._lam0monitor  = _config.pop ( 'Lam0_Monitor', _default_configuration_ [ 'Lam0_Monitor'] )
        self._Preambulo    = _config.pop ( 'Preambulo'   , _default_configuration_ [ 'Preambulo'   ] )
        
        if _config :
            raise KeyError('Invalid keys are specified for configuration: %s ' % _config.keys() )
        
        for line in self._lines_inner() :
            self.registerLine(line)

    def _lines_inner ( self ) :
        if hasattr ( self , '_Lines' ) : return self._Lines

        self._Lines = [
            ##
            StrippingLine (
            "K0SLLLine" + self._name ,
            prescale   = self._KS0LLPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.K0S_LL_Bin1() ]
            ) ,
            ## 
            StrippingLine (
            "K0SDDLine" + self._name ,
            prescale   = self._KS0DDPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.K0S_DD_Bin1() ]
            ) ,
            ## 
            StrippingLine (
            "Lam0LLLine" + self._name ,
            prescale   = self._LamLLPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin1() ]
            ) ,
            ## 
            StrippingLine (
            "Lam0DDLine" + self._name ,
            prescale   = self._LamDDPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_DD_Bin1() ]
            ),
            ## 
            StrippingLine (
            "Lam0LLIsMUONLine" + self._name ,
            prescale   = self._LamLLIsMUONPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin1_IsMUON() ]
            )
            ,
            ## 
            StrippingLine (
            "Lam0DDIsMUONLine" + self._name ,
            prescale   = self._LamDDIsMUONPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_DD_Bin1_IsMUON() ]
            )
            ]

        return self._Lines
    
    selK0S = None    
    def K0S ( self ) :
        """
        Selection for K0S 
        """
        if StrippingV0ForPIDConf.selK0S == None :
            _K0S = CombineParticles("StripK0S")
            _K0S.DecayDescriptor = "KS0 -> pi+ pi-"
            _K0S.DaughtersCuts   = {
                '' : self._k0s_daughtersCuts ()
                }
            _K0S.CombinationCut  = "AM < 1.0 * GeV " 
            _K0S.MotherCut       = self._k0s_motherCut()        
            _K0S.Preambulo       = self._Preambulo
            
            if self._monitor != None :    
                _K0S.Preambulo    += self._k0smonitor
                _K0S.Monitor       = True
                _K0S.HistoProduce  = True
                _K0S.HistoPrint    = True
                _K0S.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            K0S = Selection("SelK0S2PiPi_LL",
                            Algorithm = _K0S,
                            RequiredSelections = [ StdNoPIDsPions ]) 

            StrippingV0ForPIDConf.selK0S = K0S    
        return StrippingV0ForPIDConf.selK0S

    selK0SDD = None    
    def K0SDD ( self ) :
        """
        Selection for K0S DD 
        """
        if StrippingV0ForPIDConf.selK0SDD == None :
            _K0S = CombineParticles("StripK0S_DD")
            _K0S.DecayDescriptor = "KS0 -> pi+ pi-"
            _K0S.DaughtersCuts   = {
                '' : self._k0s_daughtersCuts ()
                }
            _K0S.CombinationCut  = "AM < 1.0 * GeV " 
            _K0S.MotherCut       = self._k0s_motherCut()      
            _K0S.Preambulo       = self._Preambulo
            
            if self._monitor != None :    
                _K0S.Preambulo    += self._k0smonitor
                _K0S.Monitor       = True
                _K0S.HistoProduce  = True
                _K0S.HistoPrint    = True
                _K0S.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            K0SDD = Selection("SelK0S2PiPi_DD",
                            Algorithm = _K0S,
                            RequiredSelections = [ StdNoPIDsDownPions ]) 

            StrippingV0ForPIDConf.selK0SDD = K0SDD    
        return StrippingV0ForPIDConf.selK0SDD

    selLam0 = None    
    def Lam0 ( self ) :
        """
        Selection for Lam0 
        """
        if StrippingV0ForPIDConf.selLam0 == None :
            _Lam0 = CombineParticles("StripLam0")
            _Lam0.DecayDescriptor = "[ Lambda0 -> p+ pi-]cc"
            _Lam0.DaughtersCuts   = {
                '' : self._lam0_daughtersCuts ()
                }
            _Lam0.CombinationCut  = "AM < 1.5 * GeV " 
            _Lam0.MotherCut       = self._lam0_motherCut()        
            _Lam0.Preambulo       = self._Preambulo
            
            if self._monitor != None :    
                _Lam0.Preambulo    += self._lam0monitor
                _Lam0.Monitor       = True
                _Lam0.HistoProduce  = True
                _Lam0.HistoPrint    = True
                _Lam0.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            Lam0 = Selection("SelLam02PPi_LL",
                             Algorithm = _Lam0,
                             RequiredSelections = [ StdNoPIDsPions, StdNoPIDsProtons ]) 

            StrippingV0ForPIDConf.selLam0 = Lam0    
        return StrippingV0ForPIDConf.selLam0

    selLam0DD = None    
    def Lam0DD ( self ) :
        """
        Selection for Lam0 DD 
        """
        if StrippingV0ForPIDConf.selLam0DD == None :
            _Lam0 = CombineParticles("StripLam0_DD")
            _Lam0.DecayDescriptor = "[ Lambda0 -> p+ pi-]cc"
            _Lam0.DaughtersCuts   = {
                '' : self._lam0_daughtersCuts (),
                }
            _Lam0.CombinationCut  = "AM < 1.5 * GeV " 
            _Lam0.MotherCut       = self._lam0_motherCut()        
            _Lam0.Preambulo       = self._Preambulo
            
            if self._monitor != None:    
                _Lam0.Preambulo    += self._lam0monitor
                _Lam0.Monitor       = True
                _Lam0.HistoProduce  = True
                _Lam0.HistoPrint    = True
                _Lam0.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            Lam0DD = Selection("SelLam02PiPi_DD",
                               Algorithm = _Lam0,
                               RequiredSelections = [ StdNoPIDsDownPions , StdNoPIDsDownProtons]) 

            StrippingV0ForPIDConf.selLam0DD = Lam0DD    
        return StrippingV0ForPIDConf.selLam0DD

    def K0S_LL_Bin1 ( self ) :
        """
        Selection for K0S LL Bin1
        """
        if hasattr(self, 'K0S_LL_Bin1_V0ForPID') :
            return self.K0S_LL_Bin1_V0ForPID

        K0S = self.K0S()
        _K0S_LL_Bin1_Filter = FilterDesktop (
            ##
#            'K0S_LL_Bin1_Filter' ,
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            Preambulo    = self._k0smonitor,
            Code         = self._k0s_bin1Cut_LL(),
            PostMonitor  = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            )

        self.K0S_LL_Bin1_V0ForPID = Selection(
            "SelK0S_LL_Bin1" + self._name,
            Algorithm          = _K0S_LL_Bin1_Filter,
            RequiredSelections = [ K0S ]
            )
        return self.K0S_LL_Bin1_V0ForPID

    def K0S_DD_Bin1 ( self ) :
        """
        Selection for K0S DD Bin1
        """
        if hasattr(self, 'K0S_DD_Bin1_V0ForPID') :
            return self.K0S_DD_Bin1_V0ForPID

        K0SDD = self.K0SDD()
        _K0S_DD_Bin1_Filter = FilterDesktop (
            ##
#            'K0S_DD_Bin1_Filter' ,
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            Preambulo    = self._k0smonitor,
            Code         = self._k0s_bin1Cut_DD(),
            PostMonitor  = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            )

        self.K0S_DD_Bin1_V0ForPID = Selection(
            "SelK0S_DD_Bin1" + self._name,
            Algorithm          = _K0S_DD_Bin1_Filter,
            RequiredSelections = [ K0SDD ]
            )
        return self.K0S_DD_Bin1_V0ForPID


    def Lam0_LL_Bin1 ( self ) :
        """
        Selection for Lam0 LL Bin1
        """
        if hasattr(self, 'Lam0_LL_Bin1_V0ForPID') :
            return self.Lam0_LL_Bin1_V0ForPID

        Lam0 = self.Lam0()
        _Lam0_LL_Bin1_Filter = FilterDesktop (
            ##
#            'Lam0_LL_Bin1_Filter' ,
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            Preambulo    = self._lam0monitor,
            Code         = self._lam0_bin1Cut_LL(),
            PostMonitor  = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            )

        self.Lam0_LL_Bin1_V0ForPID = Selection(
            "SelLam0_LL_Bin1" + self._name,
            Algorithm          = _Lam0_LL_Bin1_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return self.Lam0_LL_Bin1_V0ForPID

    def Lam0_LL_Bin1_IsMUON ( self ) :
        """
        Selection for Lam0 LL Bin1
        """
        if hasattr(self, 'Lam0_LL_Bin1_IsMUON_V0ForPID') :
            return self.Lam0_LL_Bin1_IsMUON_V0ForPID
        
        Lam0 = self.Lam0()
        _Lam0_LL_Bin1_IsMUON_Filter = FilterDesktop (
            ##
#            'Lam0_LL_Bin1_Filter' ,
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            Preambulo    = self._lam0monitor,
            Code         = self._lam0_bin1Cut_LL_IsMUON(),
            PostMonitor  = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            )
        
        self.Lam0_LL_Bin1_IsMUON_V0ForPID = Selection(
            "SelLam0_LL_Bin1_ISMUON" + self._name,
            Algorithm          = _Lam0_LL_Bin1_IsMUON_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return self.Lam0_LL_Bin1_IsMUON_V0ForPID

    def Lam0_DD_Bin1 ( self ) :
        """
        Selection for Lam0 DD Bin1
        """
        if hasattr(self, 'Lam0_DD_Bin1_V0ForPID') :
            return self.Lam0_DD_Bin1_V0ForPID
        
        Lam0DD = self.Lam0DD()
        _Lam0_DD_Bin1_Filter = FilterDesktop (
            ##
#            'Lam0_DD_Bin1_Filter' ,
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            Preambulo    = self._lam0monitor,
            Code         = self._lam0_bin1Cut_DD(),
            PostMonitor  = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            )
        
        self.Lam0_DD_Bin1_V0ForPID = Selection(
            "SelLam0_DD_Bin1" + self._name,
            Algorithm          = _Lam0_DD_Bin1_Filter,
            RequiredSelections = [ Lam0DD ]
            )
        return self.Lam0_DD_Bin1_V0ForPID

    def Lam0_DD_Bin1_IsMUON ( self ) :
        """
        Selection for Lam0 DD Bin1
        """
        if hasattr(self, 'Lam0_DD_Bin1_IsMUON_V0ForPID') :
            return self.Lam0_DD_Bin1_IsMUON_V0ForPID
        
        Lam0DD = self.Lam0DD()
        _Lam0_DD_Bin1_IsMUON_Filter = FilterDesktop (
            ##
#            'Lam0_DD_Bin1_Filter' ,
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            Preambulo    = self._lam0monitor,
            Code         = self._lam0_bin1Cut_LL_IsMUON(),
            PostMonitor  = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            )
        
        self.Lam0_DD_Bin1_IsMUON_V0ForPID = Selection(
            "SelLam0_DD_Bin1_ISMUON" + self._name,
            Algorithm          = _Lam0_DD_Bin1_IsMUON_Filter,
            RequiredSelections = [ Lam0DD ]
            )
        return self.Lam0_DD_Bin1_IsMUON_V0ForPID

    def trackChi2       ( self ) : return self._trackChi2
    def daughtersIPChi2 ( self ) : return self._daughtersIPChi2
    
    ## define daughter cuts for K0S 
    def _k0s_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for K0S
        """
        cuts = """
        ( 2.0 * GeV < P )
        & ( ISLONG | ISDOWN )
        & ( TRCHI2DOF < %g )
        & ( MIPCHI2DV(PRIMARY) > %g )
        """
        cuts = cuts %( self._trackChi2, self._daughtersIPChi2 )
        return cuts.replace ( '\n' , ' ' ) 
    
    ## define daughter cuts for Lambda0
    def _lam0_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for Lambda0
        """
        return self._k0s_daughtersCuts () 

    ## define "MotherCut" for K0s 
    def _k0s_motherCut      ( self ) :
        """
        Define 'MotherCut' for K0S
        """
        cut       = """
        ( ADMASS ( 'KS0') < %g  )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %g   )
        & ( VFASPF ( VZ   ) < %g          )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %g )
        & ( BPVLTIME()*c_light > switch ( DD , %g , %g ) )
        & ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > switch( DD, %g , %g ) )
        & ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > switch( DD, %g , %g ) )
        """
        cut = cut %(self._deltaMassK0S,
                    self._vertexChi2,
                    self._maxZ,
                    self._lTimeFitChi2,
                    self._cTauK0S_DD,
                    self._cTauK0S,
                    self._wrongMassK0S_DD,
                    self._wrongMassK0S,
                    self._wrongMassK0S_DD,
                    self._wrongMassK0S
                    )
        
        return cut.replace ( '\n' , ' ' )  
    
    ## define "MotherCut" for Lambda0
    def _lam0_motherCut      ( self ) :
        """
        Define 'MotherCut' for Lambda0
        """
        cut       = """
        ( ADMASS ( 'Lambda0')   < %g )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %g   )
        & ( VFASPF ( VZ       ) < %g            )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %g )
        & ( BPVLTIME()*c_light > switch ( DD , %g , %g ) )
        & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > switch( DD, %g  , %g ) ) 
        """
        cut = cut %(self._deltaMassLambda,
                    self._vertexChi2,
                    self._maxZ,
                    self._lTimeFitChi2,
                    self._cTauLambda0_DD,
                    self._cTauLambda0,
                    self._wrongMassLambda_DD,
                    self._wrongMassLambda
                    )
        
        return cut.replace ( '\n' , ' ' ) 
        

    ## define "Bin1 Cut" for K0S LL
    def _k0s_bin1Cut_LL ( self ) :
        """
        Define 'Bin1 Cut' for K0S LL
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin1 Cut" for Lambda0 LL
    def _lam0_bin1Cut_LL ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 LL
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 


    ## define "Bin1 IsMUON Cut" for Lambda0 LL 
    def _lam0_bin1Cut_LL_IsMUON ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 LL
        """
        cut       = """
        (INTREE( (ABSID=='p+') & ISMUON ) )
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 


    ## define "Bin1 Cut" for K0S DD
    def _k0s_bin1Cut_DD ( self ) :
        """
        Define 'Bin1 Cut' for K0S DD
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 

 

    ## define "Bin1 Cut" for Lambda0 DD
    def _lam0_bin1Cut_DD ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 DD
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 


if '__main__' == __name__ :

    print __doc__
    print 'Author : %s ' % __author__
    print 'Date   : %s ' % __date__
    print __version__

    conf = StrippingV0ForPIDConf()
    
    print conf

    props = [ 'DaughtersCuts'   ,
              'CombinationCut'  ,
              'MotherCut'       ,
              'DecayDescriptor' ,
              'DecayDescriptors'] 
                           
    for l in conf.K0S     () : print __enroll__ ( l , lst = props ) 
    for l in conf.Lambda0 () : print __enroll__ ( l , lst = props )
    
# =============================================================================
# The END 
# =============================================================================

