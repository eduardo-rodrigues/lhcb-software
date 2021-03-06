"""
Line config dictionaries for EW WG

Stripping15 definitions.
Dictionaries have the name of the line builder instance.
"""

from GaudiKernel.SystemOfUnits import *

#WMu
WMu = {    
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMuLinePrescale_ps'    : .1
    ,  'WMuLinePrescale_hps': .05
    ,  'WMuLinePrescale'    : 1.0 
    ,  'WMuLinePostscale'   : 1.0
    ,  'mucuttight' : 20.
    ,  'mucut' : 15.
    ,  'mucut_hps' : 10. 
    ,  'mucutlow' : 5.
    ,  'trkpchi2' : 0.001
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }


#Z02ee
Z02ee = {
    'BUILDERTYPE'  : 'Z02eeConf',
    'CONFIG'       : {
       'Z02eeLinePrescale'    : 1.0 
    ,  'Z02eeLinePostscale'   : 1.0
    ,  'Z0MinMass' : 40.
    ,  'ptcut' : 10.
    ,  'trkpchi2' : 0.001
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'mipdvchi2ratio' : 5.
       },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

#We
We = {
    'BUILDERTYPE'  : 'WeConf',
    'CONFIG'       : {
       'WeLinePrescale'    : 1.0 
    ,  'WeLinePostscale'   : 1.0
    ,  'WeLinePrescale_ps'    : .1 
    ,  'ptcut' : 20.
    ,  'ptcut_ps' : 15.
    ,  'trkpchi2' : 0.001
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'trkghostprob' : 0.6
       },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

#DY2ee
DY2ee = {
    'BUILDERTYPE'  : 'DY2eeConf',
    'CONFIG'       : {
       'DY2eeLine1Prescale'   : .05 
    ,  'DY2eeLine2Prescale'   : .5 
    ,  'DY2eeLine3Prescale'   : 1.0
    ,  'DY2eeLine4Prescale'   : 1.0
    ,  'DY2eeLinePostscale'   : 1.0
    ,  'DY1MinMass' : 2.5
    ,  'DY1MaxMass' : 5.
    ,  'DY2MinMass' : 5.
    ,  'DY2MaxMass' : 10.
    ,  'DY3MinMass' : 10.
    ,  'DY3MaxMass' : 20.
    ,  'DY4MinMass' : 20.
    ,  'DY4MaxMass' : 40.
    ,  'ptcut1' : 1.
    ,  'ptcut2' : 2.
    ,  'ptcut3' : 5.
    ,  'ePID'   : 1.
    ,  'trkpchi2' : 0.001
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'mipdvchi2ratio' : 5.
       },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

#SingleTrackTIS
SingleTrackTIS = {
    'BUILDERTYPE'  : 'SingleTrackTISConf',
    'CONFIG'       : {
    'LowPtPrescale'     : .01
    , 'LowPtPostscale'  : 1.0
    , 'HighPtPrescale'  : .1
    , 'HighPtPostscale' : 1.0
    , 'LowPtCut'  : 5.
    , 'HighPtCut' : 15.
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

DisplVertices = {
    'BUILDERTYPE'       : 'DisplVerticeLinesConf',
    'CONFIG'    : {
    'NCands':{
       'SinglePS': 1 ,
       'SingleLowMass': 1 ,
       'SingleHighMass': 1 ,
       'SingleDown': 1 ,
       'Double':2
    },
    'RCutMethod' : 'FromBeamSpot',
    'MinR':{
       'PreselVelo':     0.3*mm , 
       'SinglePS': 0.3*mm ,
       'SingleLowMass': 0.3*mm ,
       'SingleHighMass': 0.3*mm ,
       'SingleDown': 0.3*mm ,
       'Double': 0.3*mm
    },
    'MinMass':{
       'PreselVelo':     3.*GeV , 
       'SinglePS': 5.*GeV  ,
       'SingleLowMass': 6.8*GeV ,
       'SingleHighMass':  12.*GeV,
       'SingleDown':  3.*GeV,
       'Double': 3.*GeV 
    },
    'MinSumPt':{
       'PreselVelo':     3.*GeV ,
       'SinglePS': 5.*GeV  ,
       'SingleLowMass': 6.*GeV ,
       'SingleHighMass':  6.*GeV,
       'SingleDown':  3.5*GeV,
       'Double': 3.*GeV
    },
    'MaxMass':{
       'SinglePS': 14*TeV ,
       'SingleLowMass': 12.*GeV,
       'SingleHighMass':  14*TeV ,
       'SingleDown':  14*TeV,
       'Double': 14*TeV
    },
    'MaxSumPt':{
       'SinglePS': 14*TeV ,
       'SingleLowMass': 14*TeV,
       'SingleHighMass':  14*TeV ,
       'SingleDown':  14*TeV ,
       'Double': 14*TeV
    },
    'NTracks':{
       'PreselVelo':     4,
       'SinglePS': 5,
       'SingleLowMass': 6,
       'SingleHighMass':  5 ,
       'SingleDown':  4 ,
       'Double': 4
    },
    'MinZ':{
       'SinglePS': -10*m,
       'SingleLowMass':  -10*m,
       'SingleHighMass':   -10*m ,
       'SingleDown': 100*mm  ,
       'Double': -10*m
    },
    'MaterialVeto':{
       'PreselVelo':     5 ,
       'SinglePS': 0,
       'SingleLowMass': 5,
       'SingleHighMass':  0 ,
       'SingleDown':  0 ,
       'Double': 0
    },
    'prescale':{
       'SinglePS': 0.05,
       'SingleLowMass': 1.,
       'SingleHighMass':  1. ,
       'SingleDown':  1. ,
       'Double': 1.
    }
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['Exotics']
    }
    
ExclusiveDiMuon = {
    'BUILDERTYPE'  : 'ExclusiveDiMuonConf',
    'CONFIG'       : {
    'ExclusiveDiMuonPrescale'    : 1.0 
    ,  'ExclusiveDiMuonPostscale'   : 1.0
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }    
 
