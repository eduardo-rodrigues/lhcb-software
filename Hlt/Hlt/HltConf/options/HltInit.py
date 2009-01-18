from Configurables import ApplicationMgr, HltANNSvc 

ApplicationMgr().ExtSvc.append( "HltANNSvc"  )

# generated by parsing the logfile, of a job _without_ explicitly specified 
# Hlt1SelectionIDs, with:
# awk '/autogenerated: Hlt1SelectionID/{c=substr($6,0,7); if (c!=n) {n=c; i =(1+int(i/100))*100} ; print sprintf( "    %-40s: %4d,","\""$6"\"",++i)}' logfile

#TODO: clean up: decision are smallest numbers (written most often, may want to reduce bits eventually)
#      selections are in another range
HltANNSvc().Hlt1SelectionID = {
    # Global
    "Hlt1Global"                            :    1,
    "Hlt2Global"                            :    2,
    # L0 (prescaled)
    "Hlt1L0MuonNoGlobDecision"              :   10,
    "Hlt1L0MuonDecision"                    :   11,
    "Hlt1L0DiMuonDecision"                  :   12,
    "Hlt1L0ElectronDecision"                :   13,
    "Hlt1L0PhotonDecision"                  :   14,
    "Hlt1L0HadronDecision"                  :   15,
    "Hlt1L0GlobalPi0Decision"               :   16,
    "Hlt1L0LocalPi0Decision"                :   17,
    # Misc...
    "Hlt1IncidentDecision"                  :   30,
    # LU
    "Hlt1LumiDecision"                      :   40, 
    "Hlt1IgnoringLumiDecision"              :   41,
    # VE
    "Hlt1VeloClosingDecision"               :   60,
    "Hlt1VeloASideDecision"                 :   61,
    "Hlt1VeloCSideDecision"                 :   62,
    # PA
    "Hlt1RandomDecision"                    :   70,
    "Hlt1PhysicsDecision"                   :   71,
    "Hlt1L0ForcedDecision"                  :   72,
    # XP
    "Hlt1XPressDecision"                    :   80,
    # MU
    "Hlt1SingleMuonIPandPTDecision"         :  100,
    "Hlt1SingleMuonNoIPDecision"            :  101,
    "Hlt1DiMuonFrom2L0HighIPDecision"       :  110,
    "Hlt1DiMuonFrom2L0IPDecision"           :  111,
    "Hlt1DiMuonFrom2L0NoIPDecision"         :  112,
    "Hlt1DiMuonFromL0DiMuonHighIPDecision"  :  113,
    "Hlt1DiMuonFromL0DiMuonIPDecision"      :  114,
    "Hlt1DiMuonFromL0DiMuonNoIPDecision"    :  115,
    "Hlt1DiMuonFromL0DiMuonNoPVDecision"    :  116,
    "Hlt1DiMuonFromMuonSegHighIPDecision"   :  117,
    "Hlt1DiMuonFromMuonSegIPDecision"       :  118,
    "Hlt1DiMuonFromMuonSegNoIPDecision"     :  119,
    "Hlt1DiMuonFromMuonSegNoPVDecision"     :  120,
    "Hlt1MuonTrackDecision"                 :  150,
    # HA
    "Hlt1SingleHadronDecision"              :  200,
    "Hlt1DiHadronDecision"                  :  201,
    # PH
    "Hlt1PhotonDecision"                    :  300,
    "Hlt1PhoFromEleDecision"                :  301,
    # EL
    "Hlt1SingleElectronDecision"            :  400,
    "Hlt1ElectronTrackDecision"             :  401,
    #
    #
    # intermediate selections: 10000 -- 20000
    # 'All' L0 candidates
    'Hlt1L0AllMuonCandidates'               :10000,
    'Hlt1L0ElectronCandidates'              :10010,
    'Hlt1L0PhotonCandidates'                :10011,
    'Hlt1L0HadronCandidates'                :10012,
    'Hlt1L0LocalPi0Candidates'              :10013,
    'Hlt1L0GlobalPi0Candidates'             :10014,
    #
    "Forward1"                              :10101,
    "Hlt1EleCompanionTFForward"             :10201,
    "Hlt1EleCompanionTFVelo"                :10202,
    "Hlt1EleCompanionTUForward"             :10203,
    "Hlt1EleDiElectronVM"                   :10204,
    "Hlt1EleSingleTFRZVelo"                 :10205,
    "Hlt1EleSingleTFVelo"                   :10206,
    "Hlt1EleSingleTUForward"                :10207,
    "Hlt1EleSingleTUVelo"                   :10208,
    "Hlt1EleTFForward"                      :10209,
    "Hlt1HadronDi"                          :10301,
    "Hlt1HadronDiTFForward"                 :10303,
    "Hlt1HadronDiTFForward1"                :10304,
    "Hlt1HadronDiTFVelo"                    :10305,
    "Hlt1HadronDiVFVelo"                    :10306,
    "Hlt1HadronDiVMForward1"                :10307,
    "Hlt1HadronDiVMVelo"                    :10308,
    "Hlt1HadronDiVUForward"                 :10309,
    "Hlt1HadronSingle"                      :10310,
    "Hlt1HadronSingleTFRZVelo"              :10312,
    "Hlt1HadronSingleTFVelo"                :10313,
    "Hlt1HadronSingleTMVeloCalo"            :10314,
    "Hlt1HadronSingleTUGuidedForward"       :10315,
    "Hlt1HadronSingleTUVelo"                :10316,
    "Hlt1HadronAlleysSF"                    :10317,
    "Hlt1HadronDiVFForward"                 :10318,
    "Hlt1HadronSF"                          :10319,
    "Hlt1HadronSingleTFGFwrd"               :10320,
    "Hlt1HadronSingleTUGFwd"                :10321,
    "Hlt1MuTrackCompanionVUForward"         :10401,
    "Hlt1MuTrackMuonTF"                     :10403,
    "Hlt1MuTrackVeloCompanionTF"            :10404,
    "Hlt1MuTrackVeloVF"                     :10405,
    "Hlt1MuTrackVeloVM"                     :10406,
    "Hlt1MuonAlleysSF"                      :10501,
    "Hlt1MuonDiMuon2L0VMVeloT"              :10503,
    "Hlt1MuonDiMuonAlleysSF"                :10505,
    "Hlt1MuonDiMuonAlleysSFNoIP"            :10506,
    "Hlt1MuonDiMuonAlleysSFWithIP"          :10507,
    "Hlt1MuonDiMuonMuonSegTFRZVelo"         :10509,
    "Hlt1MuonDiMuonMuonSegTFTConf"          :10510,
    "Hlt1MuonDiMuonMuonSegTMVeloT"          :10511,
    "Hlt1MuonDiMuonMuonSegTUTConf"          :10512,
    "Hlt1MuonDiMuonMuonSegTUVelo"           :10513,
    "Hlt1MuonDiMuonMuonSegVMVeloT"          :10514,
    "Hlt1MuonDiMuonTFRZVelo"                :10517,
    "Hlt1MuonDiMuonTFTConf"                 :10518,
    "Hlt1MuonDiMuonTMVeloT"                 :10519,
    "Hlt1MuonDiMuonTUTConf"                 :10520,
    "Hlt1MuonDiMuonTUVelo"                  :10521,
    "Hlt1MuonDiMuonV2TL0"                   :10522,
    "Hlt1MuonDiMuonVFVeloT"                 :10523,
    "Hlt1MuonDiMuonVMVeloT"                 :10524,
    "Hlt1MuonSingleAlleysSF"                :10526,
    "Hlt1MuonSingleTFL0"                    :10529,
    "Hlt1MuonSingleTFRZVelo"                :10530,
    "Hlt1MuonSingleTFTConf"                 :10531,
    "Hlt1MuonSingleTMVeloT"                 :10532,
    "Hlt1MuonSingleTUTConf"                 :10533,
    "Hlt1MuonSingleTUVelo"                  :10534,
    "Hlt1MuTrackSFEntry"                    :10535,
    "Hlt1MuonSFSingleTMVeloT"               :10536,
    "Hlt1PhoTFForward"                      :10601,
    "Hlt1PhoTFRZVelo"                       :10602,
    "Hlt1PhoTFVelo"                         :10603,
    "Hlt1PhoTUForward"                      :10604,
    "Hlt1PhoTUVelo"                         :10605,
    "Hlt1PhoTightL0Calos"                   :10606,
    "Hlt1PhoTFPhoton"                       :10609,
    "Hlt1PhoTFSecondVelo"                   :10610,
    "Hlt1PhoVFDiTrack"                      :10611,
    "Hlt1PhoVFSecondDiTrack"                :10612,
    "Hlt1PhoVMDiTrack"                      :10613,
    "Hlt1PhoVUDiTrack"                      :10614,
    "Hlt1PrepareMuonSeg"                    :10701,
    "PV2D"                                  :11801,
    "RZVelo"                                :11901, 
    "Velo"                                  :12001,
    "Velo1"                                 :12002,
    "Hlt1DiHadronL0Filter"                  :13001,
    "Hlt1DiHadronTFForward"                 :13002,
    "Hlt1DiHadronTFVelo"                    :13003,
    "Hlt1DiHadronVFVelo"                    :13005,
    "Hlt1DiHadronVM2Velo"                   :13006,
    "Hlt1DiHadronVUForward"                 :13007,
    "Hlt1DiMuonFrom2L0IPL0Filter"           :13008,
    "Hlt1DiMuonFrom2L0IPVM1VeloT"           :13010,
    "Hlt1DiMuonFrom2L0NoIPL0Filter"         :13011,
    "Hlt1DiMuonFrom2L0NoIPVM1VeloT"         :13013,
    "Hlt1DiMuonFromL0DiMuoNoIPL0Filter"     :13014,
    "Hlt1DiMuonFromL0DiMuonIPL0Filter"      :13016,
    "Hlt1DiMuonFromL0DiMuonPrepareTFRZVelo" :13018,
    "Hlt1DiMuonFromL0DiMuonPrepareTFTConf"  :13019,
    "Hlt1DiMuonFromL0DiMuonPrepareTMVeloT"  :13020,
    "Hlt1DiMuonFromL0DiMuonPrepareTUTConf"  :13021,
    "Hlt1DiMuonFromL0DiMuonPrepareTUVelo"   :13022,
    "Hlt1DiMuonFromL0DiMuonPrepareVFVeloT"  :13023,
    "Hlt1DiMuonFromL0DiMuonPrepareVM1L0DiMuon" :13024,
    "Hlt1DiMuonFromL0DiMuonPrepareVM1VeloT" :13025,
    "Hlt1DiMuonFromL0DiMuonPrepareVTL0"     :13026,
    "Hlt1DiMuonFromMuonSegIPL0Filter"       :13027,
    "Hlt1DiMuonFromMuonSegIPVM2VeloT"       :13029,
    "Hlt1DiMuonFromMuonSegNoIPL0Filter"     :13030,
    "Hlt1DiMuonFromMuonSegNoIPVM2VeloT"     :13032,
    "Hlt1ElectronTrackL0Filter"             :13033,
    "Hlt1ElectronTrackTFCompanionForward"   :13034,
    "Hlt1ElectronTrackTFCompanionVelo"      :13035,
    "Hlt1ElectronTrackTUCompanionForward"   :13036,
    "Hlt1ElectronTrackVM2DiElectron"        :13038,
    "Hlt1MainHadronPrepTFRZVelo"            :13039,
    "Hlt1MainHadronPrepTFVelo"              :13040,
    "Hlt1MainHadronPrepTMVeloCalo"          :13041,
    "Hlt1MainHadronPrepTUForward"           :13042,
    "Hlt1MainHadronPrepTUVelo"              :13043,
    "Hlt1MuonHadronL0Filter"                :13044,
    "Hlt1MuonHadronTFCompanionForward"      :13045,
    "Hlt1MuonHadronTFCompanionVelo"         :13046,
    "Hlt1MuonHadronTFMuon"                  :13047,
    "Hlt1MuonHadronTUCompanionForward"      :13048,
    "Hlt1MuonHadronVM2Vertex"               :13050,
    "Hlt1MuonSegPrepareTFPrepareMuonSeg"    :13051,
    "Hlt1MuonSegPrepareTFRZVelo"            :13052,
    "Hlt1MuonSegPrepareTFTConf"             :13053,
    "Hlt1MuonSegPrepareTMVeloT"             :13054,
    "Hlt1MuonSegPrepareTUTConf"             :13055,
    "Hlt1MuonSegPrepareTUVelo"              :13056,
    "Hlt1PhotonL0Filter"                    :13058,
    "Hlt1PhotonTFForward"                   :13059,
    "Hlt1PhotonTFPhoton"                    :13060,
    "Hlt1PhotonTFRZVelo"                    :13061,
    "Hlt1PhotonTFVelo"                      :13062,
    "Hlt1PhotonTUForward"                   :13063,
    "Hlt1PhotonTUVelo"                      :13064,
    "Hlt1PhotonVFDiTrack"                   :13065,
    "Hlt1PhotonVM1DiTrack"                  :13066,
    "Hlt1PrepElectronTFForward"             :13067,
    "Hlt1PrepElectronTFRZVelo"              :13068,
    "Hlt1PrepElectronTFVelo"                :13069,
    "Hlt1PrepElectronTUForward"             :13070,
    "Hlt1PrepElectronTUVelo"                :13071,
    "Hlt1SingleElectronL0Filter"            :13072,
    "Hlt1SingleHadronL0Filter"              :13074,
    "Hlt1SingleMuonIPandPTL0Filter"         :13076,
    "Hlt1SingleMuonNoIPL0Filter"            :13078,
    "Hlt1SingleMuonPrepL0MuonPrepareMuonORMuonNoGlob" :13080,
    "Hlt1SingleMuonPrepTFL0"                :13081,
    "Hlt1SingleMuonPrepTFRZVelo"            :13082,
    "Hlt1SingleMuonPrepTFTConf"             :13083,
    "Hlt1SingleMuonPrepTMVeloT"             :13084,
    "Hlt1SingleMuonPrepTUTConf"             :13085,
    "Hlt1SingleMuonPrepTUVelo"              :13086,
}

#/ generated by parsing the logfile, of a job _without_ explicitly specified 
#/ InfoID, with:
#/ awk 'BEGIN{n="foo"}/autogenerated: InfoID/{if (substr($6,0,length(n))!=n) {n=$6; i =(1+int(i/100))*100} ; print sprintf( "    %-40s: %4d,","\""$6"\"",++i)}' logfile

HltANNSvc().InfoID = {
  "L0ET"           : 1100,
  "Calo2DChi2"                            : 1101,
  "Calo2DChi2_L0ElectronDecision"         : 1102,
  "Calo2DChi2_L0HadronDecision"           : 1103,
  "Calo3DChi2"                            : 1201,
  "Calo3DChi2_L0ElectronDecision"         : 1202,
  "Calo3DChi2_L0HadronDecision"           : 1203,
  "DOCA"                                  : 1301,
  "DOCA_Hlt1EleTFForward"                 : 1302,
  "DOCA_Hlt1MuTrackMuonTF"                : 1303,
  "DoShareM3"                             : 1401,
  "DoShareM3_Hlt1MuonSingleTUTConf"       : 1402,
  "HltGuidedForward"                      : 1501,
  "IP"                                    : 1601,
  "IP_PV2D"                               : 1602,
  "IsMuon"                                : 1701,
  "L0ConfirmWithT"                        : 1801,
  "MatchIDsFraction"                      : 1901,
  "MatchIDsFraction_Hlt1HadronDiTFForward": 1902,
  "MatchIDsFraction_Hlt1HadronSingle"     : 1903,
  "Muon2DxDist"                           : 1950,
  "Muon3DxDist"                           : 1951,
  "Muon3DyDist"                           : 1952,
  "MuonTDist"                             : 1953,
  "PT"                                    : 2001,
  "PT0"                                   : 2002,
  "PatForwardTool"                        : 2101,
  "RZVeloTMatch"                          : 2201,
  "RZVeloTMatch_Hlt1MuonDiMuonMuonSegTFTConf": 2202,
  "RZVeloTMatch_Hlt1MuonDiMuonTFTConf"    : 2203,
  "RZVeloTMatch_Hlt1MuonSingleTFTConf"    : 2204,
  "SumPT"                                 : 2301,
  "Tf::PatVeloSpaceTool"                  : 2401,
  "VeloCalo3DChi2"                        : 2501,
  "VertexDimuonMass"                      : 2601,
  "VertexDz"                              : 2701,
  "VertexDz_PV2D"                         : 2702,
  "VertexMatchIDsFraction"                : 2801,
  "VertexMatchIDsFraction_Hlt1HadronDi"   : 2802,
  "VertexMatchIDsFraction_L0DiMuonDecision": 2803,
  "VertexMaxPT"                           : 2901,
  "VertexMinIP"                           : 3001,
  "VertexMinIP_PV2D"                      : 3002,
  "VertexMinPT"                           : 3101,
  "VertexPointing"                        : 3201,
  "VertexPointing_PV2D"                   : 3202,
  "chi2_PatMatch"                         : 3301,
  "rIP"                                   : 3401,
  "rIP_PV2D"                              : 3402,
  "MatchIDsFraction_Hlt1HadronSingleTFGFwrd"     : 4001,
  "VertexMatchIDsFraction_Hlt1HadronDiVFForward" : 4002,
  "IsPhoton"                              : 4003,
  "DOCA_Hlt1MuonHadronTFMuon"             : 4011,
  "DOCA_Hlt1PrepElectronTFForward"        : 4012,
  "DoShareM3_Hlt1SingleMuonPrepTMVeloT"   : 4013,
  "MatchIDsFraction_Hlt1DiHadronTFForward" : 4014,
  "RZVeloTMatch_Hlt1DiMuonFromL0DiMuonPrepareTFTConf" : 4015,
  "RZVeloTMatch_Hlt1MuonSegPrepareTFTConf" : 4016,
  "RZVeloTMatch_Hlt1SingleMuonPrepTFTConf" : 4017,
  "VertexMatchIDsFraction_Hlt1DiMuonFromL0DiMuonPrepareVM1L0DiMuon" : 4018,
  "VertexNumberOfASideTracks" : 4019, 
  "VertexNumberOfCSideTracks" : 4020,
  "DeltaP" : 4021
}
