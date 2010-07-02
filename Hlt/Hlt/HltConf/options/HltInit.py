from Configurables import HltANNSvc 

# quick hack to make sure Hlt2Global is OK...
Hlt2ID = HltANNSvc().Hlt2SelectionID
if "Hlt2Global" not in Hlt2ID : Hlt2ID.update( {  "Hlt2Global" : 2 } )
Hlt2ID.update(  { 'Hlt2PileUpDecision'                    : 65534 } )

HltANNSvc().Hlt1SelectionID = {
    # Global
    "Hlt1Global"                            :    1,
    # L0 (prescaled) Hlt1L0.*Decision
    "Hlt1L0MuonDecision"                    :   10,
    "Hlt1L0MuonNoGlobDecision"              :   11,
    "Hlt1L0DiMuonDecision"                  :   12,
    "Hlt1L0Muon,lowMultDecision"            :   13,
    "Hlt1L0DiMuon,lowMultDecision"          :   14,
    "Hlt1L0ElectronDecision"                :   15,
    "Hlt1L0PhotonDecision"                  :   16,
    "Hlt1L0HadronDecision"                  :   17,
    "Hlt1L0GlobalPi0Decision"               :   18,
    "Hlt1L0LocalPi0Decision"                :   19,
    "Hlt1L0PUDecision"                      :   20,
    "Hlt1L0PU20Decision"                    :   21,
    "Hlt1L0SPDDecision"                     :   22,
    "Hlt1L0SPD40Decision"                   :   23,
    "Hlt1L0B1gasDecision"                   :   24,
    "Hlt1L0B2gasDecision"                   :   25,
    "Hlt1L0MUON,minbiasDecision"            :   26,
    # Misc..
    "Hlt1IncidentDecision"                  :   30,
    "Hlt1Tell1ErrorDecision"                :   31,
    # LU Hlt1Lumi.*Decision
    "Hlt1LumiDecision"                      :   40, 
    "Hlt1LumiLowNoBeamDecision"             :   41, 
    "Hlt1LumiLowBeamCrossingDecision"       :   42, 
    "Hlt1LumiLowBeam1Decision"              :   43, 
    "Hlt1LumiLowBeam2Decision"              :   44,
    "Hlt1LumiNoBeamDecision"                :   45, 
    "Hlt1LumiBeamCrossingDecision"          :   46, 
    "Hlt1LumiBeam1Decision"                 :   47, 
    "Hlt1LumiBeam2Decision"                 :   48,
    "Hlt1LumiLowDecision"                   :   49,
    # BG  Hlt1BeamGas.*Decision
    "Hlt1BeamGasDecision"                   :   50, 
    "Hlt1BeamGasBeam1Decision"              :   51,
    "Hlt1BeamGasBeam2Decision"              :   52,
    "Hlt1BeamGasCrossingDecision"           :   53,
    "Hlt1BeamGasCrossingForcedRZRecoDecision":  54,
    # VE Hlt1Velo.*Decision
    #"Hlt1VeloClosingDecision"               :   60,
    "Hlt1VeloASideDecision"                 :   61,
    "Hlt1VeloCSideDecision"                 :   62,
    # PA
    "Hlt1ODINTechnicalDecision"             :   70,
    "Hlt1ODINPhysicsDecision"               :   71,
    "Hlt1L0ForcedDecision"                  :   72,
    "Hlt1L0AnyDecision"                     :   73,
    "Hlt1L0AnyRateLimitedDecision"          :   74,
    # XP
    "Hlt1XPressDecision"                    :   80,
    "Hlt1AlignVeloDecision"                 :   81,
    #  more lumi:
    "Hlt1LumiLowNoBeamRateLimitedDecision"  :   90,
    "Hlt1LumiLowBeam2RateLimitedDecision"   :   91,
    "Hlt1LumiLowBeam1RateLimitedDecision"   :   92,
    "Hlt1LumiLowBeamCrossingRateLimitedDecision"  :   93,
    "Hlt1LumiMidBeamCrossingDecision"       :   94,
    # MU Hlt1(Single|Di)Muon.*Decision
    "Hlt1SingleMuonNoIPL0Decision"          :  100,
    "Hlt1SingleMuonNoIPGECDecision"         :  101,
    "Hlt1SingleMuonIPCL0Decision"           :  105,
    "Hlt1SingleMuonIPCGECDecision"          :  106,
    "Hlt1DiMuonNoIPL0DiDecision"            :  110,
    "Hlt1DiMuonNoIP2L0Decision"             :  111,
    "Hlt1DiMuonNoIPL0SegDecision"           :  112,
    "Hlt1DiMuonNoIPL0GECDecision"           :  113,
    "Hlt1DiMuonNoIPGECSegDecision"          :  114,
    "Hlt1DiMuonIPCL0DiDecision"             :  120,
    "Hlt1DiMuonIPC2L0Decision"              :  121,
    "Hlt1DiMuonIPCL0SegDecision"            :  122,
    "Hlt1DiMuonIPCL0GRCDecision"            :  123,
    "Hlt1DiMuonIPCGECSegDecision"           :  124,
    "Hlt1DiMuonNoPVL0DiDecision"            :  130,
    "Hlt1DiMuonNoPV2L0Decision"             :  131,
    "Hlt1DiMuonNoPVL0SegDecision"           :  132,
    "Hlt1DiMuonIPCL0GECDecision"            :  133,
    # Hlt1MuTrack.*Decision
    "Hlt1MuTrackDecision"                   :  150,
    "Hlt1MuTrack4JPsiDecision"              :  151,
    "Hlt1MuTrackFitMuDecision"              :  152,
    # HA Hl1(Single,Di)Hadron.*Decision
    "Hlt1SingleHadronDecision"              :  200,
    "Hlt1DiHadronDecision"                  :  201,
    "Hlt1DiHadronLTUnbiasedDecision"        :  202,
    # HAViaT
    "Hlt1SingleHadronViaTDecision"          :  230,
    "Hlt1DiHadronViaTDecision"              :  231,
    # HAVeloOpen
    "Hlt1SingleHadronOpenDecision"          :  260,
    # PH
    "Hlt1PhotonDecision"                    :  300,
    "Hlt1PhoFromEleDecision"                :  301,
    "Hlt1PhotonFromEleDecision"             :  302,
    "Hlt1PhotonTrackDecision"               :  303,
    "Hlt1PhotonFromEleTrackDecision"        :  304,
    "Hlt1PhotonDiTrackDecision"             :  305,
    "Hlt1PhotonFromEleDiTrackDecision"      :  306,
    "Hlt1PhotonDiTrackFromEleDecision"      :  307,
    "Hlt1PhotonTrackFromEleDecision"        :  308,
    # EL
    "Hlt1SingleElectronDecision"            :  400,
    "Hlt1ElectronFromPi0TrackNoIPDecision"  :  401,
    "Hlt1ElectronFromPi0TrackWithIPDecision":  402,
    "Hlt1ElectronTrackDecision"             :  403,
    "Hlt1ElectronTrackNoIPDecision"         :  404,
    "Hlt1ElectronTrackWithIPDecision"       :  405,
    "Hlt1SingleElectronFromPi0Decision"     :  406,
    "Hlt1SingleElectronNoIPDecision"        :  407,
    "Hlt1SingleElectronWithIPDecision"      :  408,

    # CO
    "Hlt1CosmicOTDecision"                  :  480,
    # MiniBias
    "Hlt1MBMiniBiasNoBeamDecision"          :  500, 
    "Hlt1MBMiniBiasBeam1Decision"           :  501, 
    "Hlt1MBMiniBiasBeam2Decision"           :  502,
    "Hlt1MBMiniBiasBeamCrossingDecision"    :  503, 
    # NoBias
    "Hlt1MBNoBiasNoBeamDecision"            :  504, 
    "Hlt1MBNoBiasBeam1Decision"             :  505, 
    "Hlt1MBNoBiasBeam2Decision"             :  506,
    "Hlt1MBNoBiasBeamCrossingDecision"      :  507, 
    #  MicroBias
    'Hlt1MBMicroBiasRZVeloDecision'         :  508,
    'Hlt1MBMicroBiasTStationDecision'       :  509,
    'Hlt1MBMiniBiasDecision'                :  510,
    'Hlt1MBNoBiasDecision'                  :  511,


    # Autogenerated Decisions start at 1K
    'Hlt1L0CALODecision'                    : 1000,
    'Hlt1L0MUONDecision'                    : 1001,
    #
    # intermediate selections: 10000 -- 20000
    # 'All' L0 candidates ; 
    'Hlt1L0AllMuonCandidates'               :10000,
    'Hlt1L0AllElectronCandidates'           :10010,
    'Hlt1L0AllPhotonCandidates'             :10011,
    'Hlt1L0AllHadronCandidates'             :10012,
    'Hlt1L0AllLocalPi0Candidates'           :10013,
    'Hlt1L0AllGlobalPi0Candidates'          :10014,
    #
    'RZVelo'                                :10100,
    'Forward1'                              :10102,
    'PV3D'                                  :10103,
    'VeloOpen'                              :10104,
    'PV3DOpen'                              :10105,
    'RZVeloBW'                              :10106,
    #
    # 11K - 20K : added automatically by configuration...
    # 50K+ : used by Hlt2
}

#/ generated by parsing the logfile, of a job _without_ explicitly specified 
#/ InfoID, with:
#/ awk 'BEGIN{n="foo"}/autogenerated: InfoID/{if (substr($6,0,length(n))!=n) {n=$6; i =(1+int(i/100))*100} ; print sprintf( "    %-40s: %4d,","\""$6"\"",++i)}' logfile

##
## InfoID below 1000 is reserved for the tracking reconstruction group (JAH 020409)
## 
HltANNSvc().InfoID = {
    "FitIP"                                 : 1001,
    "FitTrack"                              : 1003,
    "FitIP_PV3D"                            : 1004,
    "FitIP_PV3DOpen"                        : 1005,
    "AntiEleConf"                           : 1101,
    "Calo2DChi2"                            : 1201,
    "Calo2DChi2_Hlt1DiHadronTFL0Hadrons"    : 1202,
    "Calo2DChi2_Hlt1SingleHadronTFL0Hadrons": 1203,
    "Calo2DChi2_Hlt1L0HadronDecision"       : 1204,
    "Calo3DChi2"                            : 1301,
    "Calo3DChi2_Hlt1DiHadronTFL0Hadrons"    : 1302,
    "Calo3DChi2_Hlt1SingleHadronTFL0Hadrons": 1303,
    "Calo2DChi2_Hlt1DiHadronSoftTFL0Hadron" : 1304,
    "Calo3DChi2_Hlt1L0HadronDecision"       : 1305,
    "Calo3DChi2_Hlt1DiHadronSoftTFL0Hadron" : 1306,
    "Chi2Mu"                                : 1311,
    "Chi2OverN"                             : 1312,
    "DOCA"                                  : 1401,
    "DOCA_Hlt1MuTrackTFMuon"                : 1402,
    "DOCA_Hlt1PrepElectronTFForward"        : 1403,
    "DOCA_Hlt1ElectronFromPi0TrackNoIPTFVeloT" : 1404,
    "DOCA_Hlt1ElectronFromPi0TrackWithIPTFVeloT" : 1405,
    "DOCA_Hlt1ElectronTrackNoIPTFVeloT" :     1406,
    "DOCA_Hlt1ElectronTrackWithIPTFVeloT"   : 1407,
    "DOCA_Hlt1MuTrack4JPsiTFMuon"           : 1408,
    "DOCA_Hlt1MuTrackFitMuTFMuonFitIPpT"    : 1409,
    "DOCA_Hlt1MuTrackTFMuonIP"              : 1410,
    "DeltaP"                                : 1501,
    "DoShareM3"                             : 1601,
    "DoShareM3_Hlt1SingleMuonNoPVPrepTMVeloT": 1602,
    "DoShareM3_Hlt1SingleMuonPrepTMVeloT"   : 1603,
    "DoShareM3_Hlt1MuonNoPVPrepTMVeloT"     : 1604,
    "DoShareM3_Hlt1MuonPrepTMVeloT"         : 1605,
    "Ecal2DChi2"                            : 1701,
    "Ecal2DChi2_Hlt1L0ElectronDecision"     : 1702,
    "Ecal3DChi2"                            : 1801,
    "Ecal3DChi2_Hlt1L0ElectronDecision"     : 1802,
    "FitChi2OverNdf"                        : 1901,
    "FitMuChi2"                             : 1902,
    "HltTrackFit"                           : 2001,
    "HltRadCorTool"                         : 2091,
    "IP"                                    : 2101,
    "IP_PV3D"                               : 2103,
    "IP_PV3DOpen"                           : 2104,
    "IsMuon"                                : 2201,
    "IsPhoton"                              : 2301,
    "L0ConfirmWithT"                        : 2401,
    "L0ET"                                  : 2501,
    "L0ConfirmWithT/TEleConf"               : 2550,
    "L0ConfirmWithT/TMuonConf"              : 2551,
    "L0ConfirmWithT/THadronConf"            : 2552,
    "MatchIDsFraction"                      : 2601,
    "MatchIDsFraction_Hlt1DiHadronTFGuidedForward": 2602,
    "MatchIDsFraction_Hlt1DiHadronTFConfirmed" : 2603,
    "MatchIDsFraction_Hlt1DiHadronSoftTFConfirmed" : 2604,
    "PT"                                    : 2701,
    "PT0"                                   : 2702,
    "PatForwardTool"                        : 2801,
    "RZVeloTMatch"                          : 2901,
    "RZVeloTMatch_Hlt1DiMuonIPL0DiDiMuonPrepTFTConf": 2902,
    "RZVeloTMatch_Hlt1DiMuonNoPVL0DiMuonPrepTFTConf": 2903,
    "RZVeloTMatch_Hlt1DiMuonNoIPL0DiMuonPrepTFTConf": 2904,
    "RZVeloTMatch_Hlt1SingleMuonNoPVSegPrepTFTConf": 2905,
    "RZVeloTMatch_Hlt1SingleMuonSegPrepTFTConf"   : 2906,
    "RZVeloTMatch_Hlt1SingleMuonNoPVPrepNoTFTConf": 2907,
    "RZVeloTMatch_Hlt1SingleMuonPrepTFTConf"      : 2908,
    "RZVeloTMatch_Hlt1AllL0MuPrepTFDeltaP"  : 2909,
    "RZVeloTMatch_Hlt1DiMuonNoPVPrepTFDeltaP" :2910,
    "RZVeloTMatch_Hlt1DiMuonPrepTFDeltaP"   : 2911,
    "RZVeloTMatch_Hlt1ElectronFromPi0TrackNoIPTUTConf" : 2912,
    "RZVeloTMatch_Hlt1ElectronFromPi0TrackWithIPTUTConf" : 2913,
    "RZVeloTMatch_Hlt1ElectronTrackNoIPTUTConf" : 2914,
    "RZVeloTMatch_Hlt1ElectronTrackWithIPTUTConf" : 2915,
    "RZVeloTMatch_Hlt1MuonNoPVPrepTFDeltaP" : 2916,
    "RZVeloTMatch_Hlt1MuonNoPVSegPrepTFDeltaP" : 2917,
    "RZVeloTMatch_Hlt1MuonPrepTFDeltaP"     : 2918,
    "RZVeloTMatch_Hlt1MuonSegPrepTFDeltaP"  : 2919,
    "RZVeloTMatch_Hlt1SingleElectronFromPi0TUTConf" : 2920,
    "RZVeloTMatch_Hlt1SingleElectronTUTConf": 2921,
    "SumPT"                                 : 3001,
    "Tf::PatVeloSpaceTool"                  : 3101,
    "VeloCalo3DChi2"                        : 3201,
    "VertexAngle"                           : 3301,
    "VertexDimuonMass"                      : 3401,
    "VertexDiElectronMass"                  : 3402,
    "VertexDz"                              : 3501,
    "VertexDz_PV3D"                         : 3503,
    "VertexDz_PV3DOpen"                     : 3504,
    "VertexMatchIDsFraction"                : 3601,
    "VertexMatchIDsFraction_Hlt1DiMuonIPL0DiDiMuonPrepPVM1L0DiMuon": 3602,
    "VertexMatchIDsFraction_Hlt1DiMuonNoPVL0DiDiMuonPrepVM1L0DiMuon": 3603,
    "VertexMatchIDsFraction_Hlt1DiMuonNoIPL0DiDiMuonPrepVM1L0DiMuon": 3604,
    "VertexMatchIDsFraction_Hlt1DiMuonNoPVPrepVM1L0DiMuon" : 3605,
    "VertexMatchIDsFraction_Hlt1DiMuonPrepVM1L0DiMuon" : 3606,
    "VertexMinIP"                           : 3701,
    "VertexMinIP_PV3D"                      : 3703,
    "VertexMinIP_PV3DOpen"                  : 3704, 
    "VertexMinPT"                           : 3801,
    "VertexNumberOfASideTracks"             : 3901,
    "VertexNumberOfCSideTracks"             : 4001,
    "VertexPointing"                        : 4101,
    "VertexPointing_PV3D"                   : 4103,
    "VertexPointing_PV3DOpen"               : 4104,
    "VertexTrack1PT"                        : 4150,
    "VertexTrack2PT"                        : 4151,
    "chi2_PatMatch"                         : 4201,
    "rIP"                                   : 4301,
    "rIP_PV3D"                              : 4303,
    "rIP_PV3DOpen"                          : 4304,
    "FitVertexAngle"                        : 5101,
    "FitVertexDOCA"                         : 5201,
    "FitVertexDimuonMass"                   : 5301,
    "FitVertexDz"                           : 5401,
    "FitVertexDz_PV3D"                      : 5403,
    "FitVertexDz_PV3DOpen"                  : 5404,
    "FitVertexMaxChi2OverNdf"               : 5501,
    "FitVertexMaxMuChi2"                    : 5502,
    "FitVertexMinIP"                        : 5601,
    "FitVertexMinIP_PV3D"                   : 5603,
    "FitVertexMinIP_PV3DOpen"               : 5604,
    "FitVertexMinPT"                        : 5701,
    "FitVertexPointing"                     : 5801,
    "FitVertexPointing_PV3D"                : 5803,
    "FitVertexPointing_PV3DOpen"            : 5804,
    "FitVertexTrack1Chi2OverNdf"            : 5811,
    "FitVertexTrack1IP"                     : 5821,
    "FitVertexTrack2IP"                     : 5822,
    "FitVertexTrack1IP_PV3D"                : 5825,
    "FitVertexTrack2IP_PV3D"                : 5826,
    "FitVertexTrack1IP_PV3DOpen"            : 5827,
    "FitVertexTrack2IP_PV3DOpen"            : 5828,
    "FitVertexTrack1PT"                     : 5831,
    "FitVertexTrack2PT"                     : 5832,
    "FitVertexTrack2Chi2OverNdf"            : 5841,
    "HltGuidedForward"                      : 5901,
    "Hlt1AllL0MuPrepTFDeltaP"               : 6000, 
    "Hlt1AllL0MuPrepTFL0"                   : 6001, 
    "Hlt1AllL0MuPrepTFRZVelo"               : 6002, 
    "Hlt1AllL0MuPrepTMVeloT"                : 6003, 
    "Hlt1AllL0MuPrepTUTConf"                : 6004, 
    "Hlt1AllL0MuPrepTUVelo"                 : 6005, 
    "Hlt1DiHadronOpenTF1UVelo"              : 6006, 
    "Hlt1DiHadronOpenTFCompanion"           : 6007, 
    "Hlt1DiHadronOpenTFDiHadronIP"          : 6008, 
    "Hlt1DiHadronOpenVF1Forward"            : 6009, 
    "Hlt1DiHadronOpenVFUVelo"               : 6010, 
    "Hlt1DiHadronOpenVM2UVelo"              : 6011, 
    "Hlt1DiHadronOpenVUForward"             : 6012, 
    "Hlt1DiHadronTF1UVelo"                  : 6013, 
    "Hlt1DiHadronTFCompanion"               : 6014, 
    "Hlt1DiHadronTFDiHadronIP"              : 6015, 
    "Hlt1DiHadronTUUVelo"                   : 6016, 
    "Hlt1DiHadronVF1FitTrack"               : 6017, 
    "Hlt1DiHadronVF1Forward"                : 6018, 
    "Hlt1DiHadronVFDiHadron"                : 6019, 
    "Hlt1DiHadronVFUVelo"                   : 6020, 
    "Hlt1DiHadronVM2UVelo"                  : 6021, 
    "Hlt1DiHadronVUFitTrack"                : 6022, 
    "Hlt1DiHadronVUForward"                 : 6023, 
    "Hlt1DiHadronViaTTF1UVelo"              : 6024, 
    "Hlt1DiHadronViaTTFCompanion"           : 6025,
    "Hlt1DiHadronViaTTFTConf"               : 6026, 
    "Hlt1DiHadronViaTTFVelo3DIP"            : 6027, 
    "Hlt1DiHadronViaTTFVeloRZTMatch"        : 6028, 
    "Hlt1DiHadronViaTTMVeloT"               : 6029, 
    "Hlt1DiHadronViaTTUTConf"               : 6030, 
    "Hlt1DiHadronViaTTFDiHadronViaTIP"      : 6031, 
    "Hlt1DiHadronViaTTUVelo"                : 6032, 
    "Hlt1DiHadronViaTVF1Forward"            : 6033, 
    "Hlt1DiHadronViaTVFUVelo"               : 6034, 
    "Hlt1DiHadronViaTVM2UVelo"              : 6035, 
    "Hlt1DiHadronViaTVUForward"             : 6036, 
    "Hlt1DiMuonIPC2L0VFChi2Mu"              : 6037, 
    "Hlt1DiMuonIPC2L0VFIP"                  : 6038, 
    "Hlt1DiMuonIPC2L0VFMass"                : 6039, 
    "Hlt1DiMuonIPC2L0VM2VeloT"              : 6040, 
    "Hlt1DiMuonIPC2L0VUFitTrack"            : 6041, 
    "Hlt1DiMuonIPCGECSegVFChi2Mu"           : 6042, 
    "Hlt1DiMuonIPCGECSegVFIP"               : 6043, 
    "Hlt1DiMuonIPCGECSegVFMass"             : 6044, 
    "Hlt1DiMuonIPCGECSegVM2VeloT"           : 6045, 
    "Hlt1DiMuonIPCGECSegVUFitTrack"         : 6046, 
    "Hlt1DiMuonIPCL0DiVFChi2Mu"             : 6047, 
    "Hlt1DiMuonIPCL0DiVFIP"                 : 6048, 
    "Hlt1DiMuonIPCL0DiVFMass"               : 6049, 
    "Hlt1DiMuonIPCL0DiVUFitTrack"           : 6050, 
    "Hlt1DiMuonIPCL0GECVFChi2Mu"            : 6051, 
    "Hlt1DiMuonIPCL0GECVFIP"                : 6052, 
    "Hlt1DiMuonIPCL0GECVFMass"              : 6053, 
    "Hlt1DiMuonIPCL0GECVM2VeloT"            : 6054, 
    "Hlt1DiMuonIPCL0GECVUFitTrack"          : 6055, 
    "Hlt1DiMuonIPCL0SegVFChi2Mu"            : 6056, 
    "Hlt1DiMuonIPCL0SegVFIP"                : 6057, 
    "Hlt1DiMuonIPCL0SegVFMass"              : 6058, 
    "Hlt1DiMuonIPCL0SegVM2VeloT"            : 6059, 
    "Hlt1DiMuonIPCL0SegVUFitTrack"          : 6060, 
    "Hlt1DiMuonNoIP2L0VFChi2Mu"             : 6061, 
    "Hlt1DiMuonNoIP2L0VFMass"               : 6062, 
    "Hlt1DiMuonNoIP2L0VM2VeloT"             : 6063, 
    "Hlt1DiMuonNoIP2L0VUFitTrack"           : 6064, 
    "Hlt1DiMuonNoIPGECSegVFChi2Mu"          : 6065, 
    "Hlt1DiMuonNoIPGECSegVFMass"            : 6066, 
    "Hlt1DiMuonNoIPGECSegVM2VeloT"          : 6067, 
    "Hlt1DiMuonNoIPGECSegVUFitTrack"        : 6068, 
    "Hlt1DiMuonNoIPL0DiVFChi2Mu"            : 6069, 
    "Hlt1DiMuonNoIPL0DiVFMass"              : 6070, 
    "Hlt1DiMuonNoIPL0DiVUFitTrack"          : 6071, 
    "Hlt1DiMuonNoIPL0GECVFChi2Mu"           : 6072, 
    "Hlt1DiMuonNoIPL0GECVFMass"             : 6073, 
    "Hlt1DiMuonNoIPL0GECVM2VeloT"           : 6074, 
    "Hlt1DiMuonNoIPL0GECVUFitTrack"         : 6075, 
    "Hlt1DiMuonNoIPL0SegVFChi2Mu"           : 6076, 
    "Hlt1DiMuonNoIPL0SegVFMass"             : 6077, 
    "Hlt1DiMuonNoIPL0SegVM2VeloT"           : 6078, 
    "Hlt1DiMuonNoIPL0SegVUFitTrack"         : 6079, 
    "Hlt1DiMuonNoPV2L0VFChi2Mu"             : 6080, 
    "Hlt1DiMuonNoPV2L0VFMass"               : 6081, 
    "Hlt1DiMuonNoPV2L0VM1VeloT"             : 6082, 
    "Hlt1DiMuonNoPV2L0VUFitTrack"           : 6083, 
    "Hlt1DiMuonNoPVL0DiVFChi2Mu"            : 6084, 
    "Hlt1DiMuonNoPVL0DiVFMass"              : 6085, 
    "Hlt1DiMuonNoPVL0DiVUFitTrack"          : 6086, 
    "Hlt1DiMuonNoPVL0SegVFChi2Mu"           : 6087, 
    "Hlt1DiMuonNoPVL0SegVFMass"             : 6088, 
    "Hlt1DiMuonNoPVL0SegVM2VeloT"           : 6089, 
    "Hlt1DiMuonNoPVL0SegVUFitTrack"         : 6090, 
    "Hlt1DiMuonNoPVPrepTFDeltaP"            : 6091, 
    "Hlt1DiMuonNoPVPrepTFRZVelo"            : 6092, 
    "Hlt1DiMuonNoPVPrepTMVeloT"             : 6093, 
    "Hlt1DiMuonNoPVPrepTUTConf"             : 6094, 
    "Hlt1DiMuonNoPVPrepTUVelo"              : 6095, 
    "Hlt1DiMuonNoPVPrepVFVeloT"             : 6096, 
    "Hlt1DiMuonNoPVPrepVM1DOCA"             : 6097, 
    "Hlt1DiMuonNoPVPrepVM1L0DiMuon"         : 6098, 
    "Hlt1DiMuonNoPVPrepVTL0"                : 6099, 
    "Hlt1DiMuonPrepTFDeltaP"                : 6100, 
    "Hlt1DiMuonPrepTFRZVelo"                : 6101, 
    "Hlt1DiMuonPrepTMVeloT"                 : 6102, 
    "Hlt1DiMuonPrepTUTConf"                 : 6103, 
    "Hlt1DiMuonPrepTUVelo"                  : 6104, 
    "Hlt1DiMuonPrepVFVeloT"                 : 6105, 
    "Hlt1DiMuonPrepVM1DOCA"                 : 6106, 
    "Hlt1DiMuonPrepVM1L0DiMuon"             : 6107, 
    "Hlt1DiMuonPrepVTL0"                    : 6108, 
    "Hlt1ElectronTrackNoIPTFCompanionForward"  : 6109, 
    "Hlt1ElectronTrackNoIPTFCompanionVelo"  : 6110, 
    "Hlt1ElectronTrackNoIPTFL0Electrons"    : 6111, 
    "Hlt1ElectronTrackNoIPTFRZVelo"         : 6112, 
    "Hlt1ElectronTrackNoIPTMVeloT"          : 6113, 
    "Hlt1ElectronTrackNoIPTUCompanionForward"  : 6114, 
    "Hlt1ElectronTrackNoIPTUTConf"          : 6115, 
    "Hlt1ElectronTrackNoIPTUVelo"           : 6116, 
    "Hlt1ElectronTrackNoIPVFVertexCut"      : 6117, 
    "Hlt1ElectronTrackNoIPVM2DiElectron"    : 6118, 
    "Hlt1ElectronTrackNoIPVURadCor"         : 6119, 
    "Hlt1ElectronTrackWithIPTFCompanionForward"  : 6120, 
    "Hlt1ElectronTrackWithIPTFCompanionVelo"     : 6121, 
    "Hlt1ElectronTrackWithIPTFL0Electrons"       : 6122, 
    "Hlt1ElectronTrackWithIPTFRZVelo"            : 6123, 
    "Hlt1ElectronTrackWithIPTFVeloT"             : 6124, 
    "Hlt1ElectronTrackWithIPTMVeloT"             : 6125, 
    "Hlt1ElectronTrackWithIPTUCompanionForward"  : 6126, 
    "Hlt1ElectronTrackWithIPTUTConf"             : 6127, 
    "Hlt1ElectronTrackWithIPTUVelo"              : 6128, 
    "Hlt1ElectronTrackWithIPVFVertexCut"         : 6129, 
    "Hlt1ElectronTrackWithIPVM2DiElectron"       : 6130, 
    "Hlt1ElectronTrackWithIPVUFitTrack"          : 6131, 
    "Hlt1HadronConfirmationSoftTF1Velo"          : 6132, 
    "Hlt1HadronConfirmationSoftTF2Velo"          : 6133, 
    "Hlt1HadronConfirmationSoftTFConfirmed"      : 6134, 
    "Hlt1HadronConfirmationSoftTFL0Hadron"       : 6135, 
    "Hlt1HadronConfirmationSoftTFRZVelo"         : 6136, 
    "Hlt1HadronConfirmationSoftTMVeloCalo"       : 6137, 
    "Hlt1HadronConfirmationSoftTUGuidedForward"  : 6138, 
    "Hlt1HadronConfirmationSoftTUVelo"           : 6139, 
    "Hlt1HadronConfirmationTF1Velo"              : 6140, 
    "Hlt1HadronConfirmationTF2Velo"              : 6141, 
    "Hlt1HadronConfirmationTFConfirmed"          : 6142, 
    "Hlt1HadronConfirmationTFRZVelo"             : 6143, 
    "Hlt1HadronConfirmationTMVeloCalo"           : 6144, 
    "Hlt1HadronConfirmationTUGuidedForward"      : 6145, 
    "Hlt1HadronConfirmationTUVelo"               : 6146, 
    "Hlt1HadronVeloOpenConfirmationTF1Velo"      : 6147, 
    "Hlt1HadronVeloOpenConfirmationTF2Velo"      : 6148, 
    "Hlt1HadronVeloOpenConfirmationTFConfirmed"  : 6149, 
    "Hlt1HadronVeloOpenConfirmationTMVeloCalo"   : 6150, 
    "Hlt1HadronVeloOpenConfirmationTUGuidedForward"  : 6151, 
    "Hlt1MuTrack4JPsiTFCompanionVelo"            : 6152, 
    "Hlt1MuTrack4JPsiTFMuonPt"                   : 6153, 
    "Hlt1MuTrack4JPsiVFTrackChi2"                : 6154, 
    "Hlt1MuTrack4JPsiVFVeloVertex"               : 6155, 
    "Hlt1MuTrack4JPsiVFVertexMass"               : 6156, 
    "Hlt1MuTrack4JPsiVFVertexPt"                 : 6157, 
    "Hlt1MuTrack4JPsiVM2VeloVertex"              : 6158, 
    "Hlt1MuTrack4JPsiVUVTrackFit"                : 6159, 
    "Hlt1MuTrack4JPsiVUVertex"                   : 6160, 
    "Hlt1MuTrackTFMuonIP"                        : 6161, 
    "Hlt1MuTrackTFMuonPt"                        : 6162, 
    "Hlt1MuTrackTFVeloCompDOCA"                  : 6163, 
    "Hlt1MuTrackTFVeloCompIP"                    : 6164, 
    "Hlt1MuTrackVFTrackChi2"                     : 6165, 
    "Hlt1MuTrackVFVeloVertex"                    : 6166, 
    "Hlt1MuTrackVFVertexMass"                    : 6167, 
    "Hlt1MuTrackVFVertexPoint"                   : 6168, 
    "Hlt1MuTrackVFVertexPt"                      : 6169, 
    "Hlt1MuTrackVM2VeloVertex"                   : 6170, 
    "Hlt1MuTrackVUVForward"                      : 6171, 
    "Hlt1MuTrackVUVTrackFit"                     : 6172, 
    "Hlt1MuonGECPrepTFDeltaP"                    : 6173, 
    "Hlt1MuonGECPrepTFRZVelo"                    : 6174, 
    "Hlt1MuonGECPrepTMVeloT"                     : 6175, 
    "Hlt1MuonGECPrepTUTConf"                     : 6176, 
    "Hlt1MuonGECPrepTUVelo"                      : 6177, 
    "Hlt1MuonGECSegPrepTFDeltaP"                 : 6178, 
    "Hlt1MuonGECSegPrepTFPrepMuSeg"              : 6179, 
    "Hlt1MuonGECSegPrepTFRZVelo"                 : 6180, 
    "Hlt1MuonGECSegPrepTMVeloT"                  : 6181, 
    "Hlt1MuonGECSegPrepTUTConf"                  : 6182, 
    "Hlt1MuonGECSegPrepTUVelo"                   : 6183, 
    "Hlt1MuonNoPVPrepTFDeltaP"                   : 6184, 
    "Hlt1MuonNoPVPrepTFL0"                       : 6185, 
    "Hlt1MuonNoPVPrepTFRZVelo"                   : 6186, 
    "Hlt1MuonNoPVPrepTMVeloT"                    : 6187, 
    "Hlt1MuonNoPVPrepTUTConf"                    : 6188, 
    "Hlt1MuonNoPVPrepTUVelo"                     : 6189, 
    "Hlt1MuonNoPVSegPrepTFDeltaP"                : 6190, 
    "Hlt1MuonNoPVSegPrepTFPrepMuSeg"             : 6191, 
    "Hlt1MuonNoPVSegPrepTFRZVelo"                : 6192, 
    "Hlt1MuonNoPVSegPrepTMVeloT"                 : 6193, 
    "Hlt1MuonNoPVSegPrepTUTConf"                 : 6194, 
    "Hlt1MuonNoPVSegPrepTUVelo"                  : 6195, 
    "Hlt1MuonPrepTFDeltaP"                       : 6196, 
    "Hlt1MuonPrepTFRZVelo"                       : 6197, 
    "Hlt1MuonPrepTMVeloT"                        : 6198, 
    "Hlt1MuonPrepTUTConf"                        : 6199, 
    "Hlt1MuonPrepTUVelo"                         : 6200, 
    "Hlt1MuonSegPrepTFDeltaP"                    : 6201, 
    "Hlt1MuonSegPrepTFPrepMuSeg"                 : 6202, 
    "Hlt1MuonSegPrepTFRZVelo"                    : 6203, 
    "Hlt1MuonSegPrepTMVeloT"                     : 6204, 
    "Hlt1MuonSegPrepTUTConf"                     : 6205, 
    "Hlt1MuonSegPrepTUVelo"                      : 6206, 
    "Hlt1PhotonDiTrackFromEleTFAntiEle"          : 6207, 
    "Hlt1PhotonDiTrackFromEleTFFirstForward"     : 6208, 
    "Hlt1PhotonDiTrackFromEleTFL0ET"             : 6209, 
    "Hlt1PhotonDiTrackFromEleTFPhoton"           : 6210, 
    "Hlt1PhotonDiTrackFromEleTFSecondForward"    : 6211, 
    "Hlt1PhotonDiTrackFromEleTFSecondVelo"       : 6212, 
    "Hlt1PhotonDiTrackFromEleTFVelo"             : 6213, 
    "Hlt1PhotonDiTrackFromEleTUFirstForward"     : 6214, 
    "Hlt1PhotonDiTrackFromEleTUSecondForward"    : 6215, 
    "Hlt1PhotonDiTrackFromEleVFDiTrack"          : 6216, 
    "Hlt1PhotonDiTrackFromEleVFFitTrack"         : 6217, 
    "Hlt1PhotonDiTrackFromEleVM2DiTrack"         : 6218, 
    "Hlt1PhotonDiTrackFromEleVUFitTrack"         : 6219, 
    "Hlt1PhotonDiTrackTFAntiEle"                 : 6220, 
    "Hlt1PhotonDiTrackTFFirstForward"            : 6221, 
    "Hlt1PhotonDiTrackTFL0ET"                    : 6222, 
    "Hlt1PhotonDiTrackTFPhoton"                  : 6223, 
    "Hlt1PhotonDiTrackTFSecondForward"           : 6224, 
    "Hlt1PhotonDiTrackTFSecondVelo"              : 6225, 
    "Hlt1PhotonDiTrackTFVelo"                    : 6226, 
    "Hlt1PhotonDiTrackTUFirstForward"            : 6227, 
    "Hlt1PhotonDiTrackTUSecondForward"           : 6228, 
    "Hlt1PhotonDiTrackVFDiTrack"                 : 6229, 
    "Hlt1PhotonDiTrackVFFitTrack"                : 6230, 
    "Hlt1PhotonDiTrackVM2DiTrack"                : 6231, 
    "Hlt1PhotonDiTrackVUFitTrack"                : 6232, 
    "Hlt1PhotonTrackFromEleTFAntiEle"            : 6233, 
    "Hlt1PhotonTrackFromEleTFFitTrack"           : 6234, 
    "Hlt1PhotonTrackFromEleTFForward"            : 6235, 
    "Hlt1PhotonTrackFromEleTFL0ET"               : 6236, 
    "Hlt1PhotonTrackFromEleTFPhoton"             : 6237, 
    "Hlt1PhotonTrackFromEleTFVelo"               : 6238, 
    "Hlt1PhotonTrackFromEleTUFirstForward"       : 6239, 
    "Hlt1PhotonTrackFromEleTUFitTrack"           : 6240, 
    "Hlt1PhotonTrackTFAntiEle"                   : 6241, 
    "Hlt1PhotonTrackTFFitTrack"                  : 6242, 
    "Hlt1PhotonTrackTFForward"                   : 6243, 
    "Hlt1PhotonTrackTFL0ET"                      : 6244, 
    "Hlt1PhotonTrackTFPhoton"                    : 6245, 
    "Hlt1PhotonTrackTFVelo"                      : 6246, 
    "Hlt1PhotonTrackTUFirstForward"              : 6247, 
    "Hlt1PhotonTrackTUFitTrack"                  : 6248, 
    "Hlt1SingleElectronNoIPTFL0Electrons"        : 6249, 
    "Hlt1SingleElectronNoIPTFRZVelo"             : 6250, 
    "Hlt1SingleElectronNoIPTMVeloT"              : 6251, 
    "Hlt1SingleElectronNoIPTUTConf"              : 6252, 
    "Hlt1SingleElectronNoIPTUVelo"               : 6253, 
    "Hlt1SingleElectronWithIPTFL0Electrons"      : 6254, 
    "Hlt1SingleElectronWithIPTFPTCut"            : 6255, 
    "Hlt1SingleElectronWithIPTFRZVelo"           : 6256, 
    "Hlt1SingleElectronWithIPTFVeloT"            : 6257, 
    "Hlt1SingleElectronWithIPTMVeloT"            : 6258, 
    "Hlt1SingleElectronWithIPTUFitTrack"         : 6259, 
    "Hlt1SingleElectronWithIPTUTConf"            : 6260, 
    "Hlt1SingleElectronWithIPTUVelo"             : 6261, 
    "Hlt1SingleHadronOpenTFSingleHadronPT"       : 6262, 
    "Hlt1SingleHadronTF1FitTrack"                : 6263, 
    "Hlt1SingleHadronTFSingleHadronIP"           : 6264, 
    "Hlt1SingleHadronTFSingleHadronPT"           : 6265, 
    "Hlt1SingleHadronTUFitTrack"                 : 6266, 
    "Hlt1SingleHadronViaTTFTConf"                : 6267, 
    "Hlt1SingleHadronViaTTFVelo3DIP"             : 6268, 
    "Hlt1SingleHadronViaTTUTConf"                 : 6269, 
    "Hlt1SingleHadronViaTPT"                     : 6270, 
    "Hlt1SingleHadronViaTIP"                     : 6271, 
    "Hlt1SingleHadronViaTTUVelo"                 : 6272, 
    "Hlt1SingleMuonIPCGECTFChi2Mu"               : 6273, 
    "Hlt1SingleMuonIPCGECTFIP"                   : 6274, 
    "Hlt1SingleMuonIPCGECTFPT"                   : 6275, 
    "Hlt1SingleMuonIPCGECTUFitTrack"             : 6276, 
    "Hlt1SingleMuonIPCL0TFChi2Mu"                : 6277, 
    "Hlt1SingleMuonIPCL0TFIP"                    : 6278, 
    "Hlt1SingleMuonIPCL0TFPT"                    : 6279, 
    "Hlt1SingleMuonIPCL0TUFitTrack"              : 6280, 
    "Hlt1SingleMuonNoIPGECTFChi2Mu"              : 6281, 
    "Hlt1SingleMuonNoIPGECTFPT"                  : 6282, 
    "Hlt1SingleMuonNoIPGECTUFitTrack"            : 6283, 
    "Hlt1SingleMuonNoIPL0TFChi2Mu"               : 6284, 
    "Hlt1SingleMuonNoIPL0TFPT"                   : 6285, 
    "Hlt1SingleMuonNoIPL0TUFitTrack"             : 6286, 
    "Velo"                                       : 6298,
}
