HltGlobalEventPrefix            = ""
HltGlobalTrackLocation          = "Track"

def _baseTrackLocation(prefix,tracks) :
    return prefix + "/" + HltGlobalTrackLocation + "/" + tracks 

Hlt1TracksPrefix            = HltGlobalEventPrefix + "Hlt1"
Hlt1TrackRoot               = Hlt1TracksPrefix + "/" + HltGlobalTrackLocation + "/"

TrackName = { "Velo"              : "Velo"           # full Velo recoonstruction
            , "VeloL0Calo"        : "VeloL0Calo"
            , "FittedVelo"        : "FittedVelo"
            , "MatchVeloMuon"     : "MatchVeloMuon"
            , "MatchVeloTTMuon"   : "MatchVeloTTMuon"
            , "VeloSelection"     : "VeloSelection"  # filtered velo tracks
            , "VeloTTHPT"         : "VeloTTHPT"      # VeloTT for high pt and p thresholds (HLT1-like)
            , "Forward"           : "Forward"
            , "ForwardHPT"        : "ForwardHPT"     # Forward for high pt and p thresholds (HLT1-like)
            , "ComplementForward" : "ComplementForward"    # Forward complement
            , "LooseComplementForward" : "LooseComplementForward"    # Loose forward complement
            , "LooseForward"      : "LooseForward"   # Loose Forward
            , "Seeding"           : "Seeding"        # Seeding
            , "ForwardSecondLoop" : "ForwardSecondLoop"
            , "Match"             : "Match"
            , "Long"              : "Long"
            , "Downstream"        : "Downstream"
            , "Ttrack"            : "Ttrack"
            , "Best"              : "Best"
            , "FitTrack"          : "FitTrack"   
            , "FilterGhostProb"   : "FilterGhostProb"
            , "VeloOnlyFitTrack"  : "VeloOnlyFitTrack"
            , "IsMuon"            : "IsMuon"
            , "IsMuonTight"       : "IsMuonTight"
            , "TrackL0Calo"       : "TrackL0Calo"
            }    
Hlt1TrackLoc = {name : _baseTrackLocation(Hlt1TracksPrefix, loc) for name, loc in TrackName.iteritems()}
Hlt1CacheLoc = {name : loc + 'Cache' for name, loc in Hlt1TrackLoc.iteritems()}

Hlt1Tools = { "ComplementForward" : "PatForwardTool/CompForward"
            , "LooseComplementForward" : "PatForwardTool/LooseCompForward"
            , "LooseForward"      : "PatForwardTool/LooseForward"
            , "FitTrack"          : "HltTrackFit"
            , "VeloOnlyFitTrack"  : "HltTrackFit/VeloOnlyFit"
            , "VeloL0Calo"        : "Hlt::MatchVeloL0Calo/VeloL0Calo"
            , "IsMuon"            : "IsMuonTool"
            , "IsMuonTight"       : "IsMuonTightTool"
            , "FilterGhostProb"   : "HltTrackFilterGhostProb"
            , "MatchVeloMuon"     : "MatchVeloMuon"
            , "MatchVeloTTMuon"   : "MatchVeloTTMuon"
            , "TrackL0Calo"       : "Hlt::Track2L0CaloMatch/TrackL0Calo"
            }
