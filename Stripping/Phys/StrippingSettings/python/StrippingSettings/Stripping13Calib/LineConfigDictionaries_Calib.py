"""
Line config dictionaries for Calibration stream

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""

BeamGas = {
   'BUILDERTYPE' : 'BeamGasConf',
   'CONFIG' : {'Prescale' : 0.05, 'Postscale' : 1.0},
   'STREAMS' : ['Calibration'],
   'WGs' : ['ALL']
}

D02KPiPi0 = {
   'BUILDERTYPE' : 'StrippingD02KPiPi0Conf',
   'CONFIG' : { 'TrackMinPT'          : 300       # MeV
                   ,'TrackMinPT_R'       : 600       # MeV  (>= TrackMinPT else no effect)
                   ,'TrackMinTrackProb'  : 0.000001  # unitless
                   ,'TrackMinIPChi2'     : 16        # unitless
                   ,'Pi0MinPT_M'         : 2000      # MeV
                   ,'Pi0MinPT_R'         : 1000      # MeV
                   ,'ResPi0MinGamCL'     : 0.2       # unitless
                   ,'D0MinM'             : 1600      # MeV
                   ,'D0MaxM'             : 2100      # MeV
                   ,'D0MinVtxProb'       : 0.001     # unitless
                   ,'D0MaxIPChi2'        : 9         # unitless
                   ,'D0MinDIRA'          : 0.9999    # unitless
                   ,'D0MinVVDChi2'       : 64        # unitless
                   ,'MergedLinePrescale'   : 1.        # unitless
                   ,'MergedLinePostscale'  : 1.        # unitless
                   ,'ResolvedLinePrescale' : 1.        # unitless
                   ,'ResolvedLinePostscale': 1.        # unitless
                   },
   'STREAMS' : ['Calibration'],
   'WGs' : ['ALL']
}

NoPIDDstarWithD02RSKPi = {
    'BUILDERTYPE' : 'NoPIDDstarWithD02RSKPiConf',
    'CONFIG' : {'DaugPt'             : 250       ## MeV
                , 'DaugP'            : 2000      ## MeV
                , 'DaugIPChi2'       : 16        ## unitless
                , 'DaugTrkChi2'      : 5         ## unitless
                , 'D0MassWin'        : 75        ## MeV
                , 'D0Pt'             : 1500      ## MeV
                , 'D0VtxChi2Ndof'    : 13        ## unitless
                , 'D0FDChi2'         : 49        ## unitless
                , 'D0BPVDira'        : 0.9999    ## unitless
                , 'D0IPChi2'         : 30        ## unit
                , 'SlowPiPt'         : 150       ## MeV
                , 'SlowPiTrkChi2'    : 5         ## unitless
                , 'DstarPt'          : 2200      ## MeV
                , 'DstarVtxChi2Ndof' : 13        ## unitless
                , 'DeltaM_Min'       : 130       ## MeV
                , 'DeltaM_Max'       : 155       ## MeV
                , 'DCS_WrongMass'    : 25        ## MeV (3 sigma veto)
                , 'KK_WrongMass'     : 25        ## MeV (3 sigma veto)
                , 'PiPi_WrongMass'   : 25        ## MeV (3 sigma veto)
                , 'Prescale'         : 0.45      ## unitless
                , 'Postscale'        : 1.0       ## unitless
                , 'Monitor'          : None      ## Monitoring?
                },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }

# NoPID Inclusive Phi Lines
# A. Powell & S. Schleich
InclPhi={
    'BUILDERTYPE' : 'InclPhiConf',
    'CONFIG' : {'KaonPT'                : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.0015   # adimensional
                , 'LowPtPrescale'       : 0.0020   # adimensional
                , 'LDPrescale'          : 0.00002  # adimensional
                },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }

# NoPID V0 Lines
# A. Powell
V0ForPID = {
    'BUILDERTYPE' : 'StrippingV0ForPIDConf',
    'CONFIG' : {'KS0LL_Prescale'           : 0.006,
                'KS0DD_Prescale'           : 0.006,
                'LamLL_Prescale_LoP'       : 0.036,
                'LamLL_Prescale_HiP'       : 0.250,
                'LamDD_Prescale'           : 0.027,
                'LamLLIsMUON_Prescale_LoP' : 0.869,
                'LamLLIsMUON_Prescale_HiP' : 1.000,
                'LamDDIsMUON_Prescale'     : 0.460
                },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }


Jpsi2eeForElectronID = {
    'BUILDERTYPE'	: 'ElectronIDConf',
    'CONFIG'	: {'Both_MIPCHI2': 9.0,
                   'Both_P': 3000.0,
                   'Both_PT': 500.0,
                   'Both_TRCHI2DOF': 5.0,
                   'LinePostscale': 1.0,
                   'LinePrescale': 0.40000000000000002,
                   'Probe_MIPCHI2': 9.0,
                   'Probe_P': 3000.0,
                   'Probe_PT': 500.0,
                   'Tag_MIPCHI2': 9.0,
                   'Tag_P': 6000.0,
                   'Tag_PIDe': 5.0,
                   'Tag_PT': 1500.0,
                   'eeCombMaxMass': 4300.0,
                   'eeCombMinMass': 2100.0,
                   'eeFDCHI2': 225.0,
                   'eeMaxMass': 4200.0,
                   'eeMinMass': 2200.0,
                   'eeVCHI2PDOF': 9.0},
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }


MuIDCalib = {
    'BUILDERTYPE' : 'MuIDCalibConf',
    'WGs'         : [ 'ALL' ],
    'STREAMS'     : [ 'Calibration' ],
    'CONFIG'      : { 'PromptPrescale'           : 0.08,
                  'DetachedPrescale'         : 1.,
                  'DetachedNoMIPPrescale'    : 0.3,
                  'DetachedNoMIPHiPPrescale' : 1.,
                  'DetachedNoMIPKPrescale'   : 1.
                 }
    }

TrackEffDownMuon = { 
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {
    'MuMom':                1000.   
    ,       'MuTMom':               100.
    ,       'TrChi2':               10. 
    ,       'MassPreComb':          2000.
    ,       'MassPostComb':         200. 
    ,       'Doca':                 5.   
    ,       'VertChi2':             25.  
    ,       'DataType':             '2010'
    ,       'NominalLinePrescale':  1.
    ,       'NominalLinePostscale': 1.
    ,       'ValidationLinePrescale': 0.5
    ,       'ValidationLinePostscale': 1.,
    'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
    ,	'HLT1PassOnAll': True
    ,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
    ,	'HLT2PassOnAll': False
    } 
    }

TrackEffVeloMuon = { 
    'BUILDERTYPE' : 'StrippingTrackEffVeloMuonConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {
    "TrChi2Mu":             10.     # adimensional
    ,       "JpsiPt":               0.5     # GeV
    ,       "TrPt":                 100.    # MeV
    ,       "VertChi2":             25.     # adimensional
    ,       "MassPreComb":          1000.   # MeV
    ,       "MassPostComb":         400.    # MeV
    ,       "Prescale":             1.      # MeV
    ,       "Postscale":            1.      # MeV
    ,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
    ,	'HLT1PassOnAll': True
    ,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
    ,	'HLT2PassOnAll': False
    }
    }

TrackEffMuonTT = {
    'BUILDERTYPE' : 'StrippingTrackEffMuonTTConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {'JpsiMassWin'                 : 500,
                       'UpsilonMassWin'              : 1500,
                       'ZMassWin'                    : 40000,
                       'BMassWin'                    : 500,
                       'JpsiMuonTTPT'                : 0,
                       'UpsilonMuonTTPT'             : 500,
                       'ZMuonTTPT'                   : 500,
                       'JpsiLongPT'                  : 1000,
                       'UpsilonLongPT'               : 1000,
                       'ZLongPT'                     : 10000,
                       'JpsiPT'                      : 500,
                       'UpsilonPT'                   : 0,
                       'ZPT'                         : 0,
                       'JpsiLongMuonMinIP'           : 0.1,
                       'UpsilonLongMuonMinIP'        : 0,
                       'ZLongMuonMinIP'              : 0,
                       'JpsiLongMuonTrackCHI2'       : 5,
                       'UpsilonLongMuonTrackCHI2'    : 5,
                       'ZLongMuonTrackCHI2'          : 5,
                       'VertexChi2'                  : 10,
                       'LongMuonPID'                 : 0,
                       'JpsiHlt1Triggers'            :  { "Hlt1TrackMuonDecision%TOS" : 0},
                       'UpsilonHlt1Triggers'         :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
                       'ZHlt1Triggers'               :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
                       'JpsiHlt2Triggers'            :  { "Hlt2SingleMuon.*Decision%TOS" : 0}, 
                       'UpsilonHlt2Triggers'         :  { "Hlt2SingleMuonLowPTDecision%TOS" : 0}, 
                       'ZHlt2Triggers'               :  { "Hlt2SingleMuonHighPTDecision%TOS" : 0},
                       'BJpsiKHlt2TriggersTUS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TUS" : 0}, 
                       'BJpsiKHlt2TriggersTOS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TOS" : 0}, 
                       'JpsiPrescale'                : 1,
                       'UpsilonPrescale'             : 1,
                       'ZPrescale'                   : 1,
                       'BJpsiKPrescale'              : 1,
                       'Postscale'                   : 1
                       }
    }
