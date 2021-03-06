from Configurables import HltANNSvc

# 11K - 20K : added automatically by configuration...
# 50K+ : used by Hlt2

# Hlt1Global is hardcoded in the C++ to 1
# PV3D needs to be added explicitly
HltANNSvc().Hlt1SelectionID.update(
    {"Hlt1Global"                            :    1,
     'PV3D'                                  :10103})

# Hlt2 selection IDs. The one for Hlt2Global is hardcoded in the SelReportsMaker.
# The others are selections that are not created by trigger lines.
HltANNSvc().Hlt2SelectionID.update(
    {'Hlt2Global'     : 2,
     'PV3D'           : 50103,
     '_RefitPVs'      : 50104,
     'Hlt2RecSummary' : 65000})

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
    "AntiEleConf"                           : 1101,
    "Calo3DChi2"                            : 1301,
    "Chi2Mu"                                : 1311,
    "Chi2OverN"                             : 1312,
    "DOCA"                                  : 1401,
    "DeltaP"                                : 1501,
    "DoShareM3"                             : 1601,
    "Ecal3DChi2"                            : 1801,
    "Ecal3DChi2_Hlt1L0ElectronDecision"     : 1802,
    "FitChi2OverNdf"                        : 1901,
    "FitMuChi2"                             : 1902,
    "HltTrackFit"                           : 2001,
    "HltTrackFit/VeloTTFit"                 : 2002,
    "HltRadCorTool"                         : 2091,
    "HltRadCorTool/HltRadCorTool"           : 2092,
    "IP"                                    : 2101,
    "IP_PV3D"                               : 2103,
    "IsMuon"                                : 2201,
    "IsPhoton"                              : 2301,
    "L0ET"                                  : 2501,
    "MatchIDsFraction"                      : 2601,
    "PT"                                    : 2701,
    "PT0"                                   : 2702,
    "PatForwardTool"                        : 2801,
    "PatForwardTool/PatForwardTool"         : 2802,
    "SumPT"                                 : 3001,
    "Tf::PatVeloSpaceTool"                  : 3101,
    "VeloCalo3DChi2"                        : 3201,
    "VertexAngle"                           : 3301,
    "VertexDimuonMass"                      : 3401,
    "VertexDiElectronMass"                  : 3402,
    "VertexDz"                              : 3501,
    "VertexDz_PV3D"                         : 3503,
    "VertexMatchIDsFraction"                : 3601,
    "VertexMinIP"                           : 3701,
    "VertexMinIP_PV3D"                      : 3703,
    "VertexMinPT"                           : 3801,
    "VertexNumberOfASideTracks"             : 3901,
    "VertexNumberOfCSideTracks"             : 4001,
    "VertexTrack1PT"                        : 4150,
    "VertexTrack2PT"                        : 4151,
    "chi2_PatMatch"                         : 4201,
    "FitVertexAngle"                        : 5101,
    "FitVertexDOCA"                         : 5201,
    "FitVertexDimuonMass"                   : 5301,
    "FitVertexMaxChi2OverNdf"               : 5501,
    "FitVertexMaxMuChi2"                    : 5502,
    "FitVertexMinIP"                        : 5601,
    "FitVertexMinIP_PV3D"                   : 5603,
    "FitVertexMinIP_PV3DOpen"               : 5604,
    "FitVertexMinPT"                        : 5701,
    "FitVertexTrack1Chi2OverNdf"            : 5811,
    "FitVertexTrack1IP"                     : 5821,
    "FitVertexTrack2IP"                     : 5822,
    "FitVertexTrack1IP_PV3D"                : 5825,
    "FitVertexTrack2IP_PV3D"                : 5826,
    "FitVertexTrack1PT"                     : 5831,
    "FitVertexTrack2PT"                     : 5832,
    "FitVertexTrack2Chi2OverNdf"            : 5841,
    "HltGuidedForward"                      : 5901,
    "HltGuidedForward/HltGuidedForward"     : 5902,
    "Velo"                                       : 6298,
    "PatForwardTool/LooseForward"                : 6299,
    "FastTTValidationTool/ValidateWithTT"        : 6350,
    "MatchVeloMuon"                              : 6400,
    "IsMuonTool"                                 : 6401,
    'Hlt2Topo2BodyBBDTResponse'                  : 6502,
    'Hlt2Topo3BodyBBDTResponse'                  : 6303,
    'Hlt2Topo4BodyBBDTResponse'                  : 6304,
    'Hlt::MatchVeloL0Calo/VeloL0Calo'            : 6600,
    'HltUnit/Hlt1VertexDisplVertexStreamer'      : 6700,
    'HltUnit/Hlt1VertexDisplVertexPSStreamer'    : 6701
}
