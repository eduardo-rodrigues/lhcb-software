from Configurables import (GaudiSequencer, TrackMonitor, TrackVertexMonitor,
                           TrackAlignMonitor,
                           TrackFitMatchMonitor,TrackV0Monitor,TrackDiMuonMonitor,
                           OTTrackMonitor, OTHitEfficiencyMonitor,OTTimeMonitor,
                           TrackCaloMatchMonitor,TrackMuonMatchMonitor)
from Configurables import (RecSysConf, TrackSys)

def ConfiguredTrackMonitorSequence(Name = "TrackMonitorSequence",
                                   HistoPrint = False ):
    seq = GaudiSequencer(Name)
    seq.Members.append( TrackMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackVertexMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackFitMatchMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackV0Monitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackDiMuonMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackAlignMonitor(HistoPrint=HistoPrint) )

    if "CALO" in RecSysConf().RecoSequence:
        seq.Members.append( TrackCaloMatchMonitor("TrackEcalMatchMonitor", CaloSystem='Ecal', HistoPrint=HistoPrint) )
        #seq.Members.append(TrackCaloMatchMonitor("TrackHcalMatchMonitor", CaloSystem='Hcal', HistoPrint=HistoPrint))
        seq.Members.append(TrackCaloMatchMonitor("TrackSpdMatchMonitor", CaloSystem='Spd',HistoPrint=HistoPrint))
        seq.Members.append(TrackCaloMatchMonitor("TrackPrsMatchMonitor", CaloSystem='Prs',HistoPrint=HistoPrint))

    if "MUON" in RecSysConf().RecoSequence:
        seq.Members.append(TrackMuonMatchMonitor("TrackMuonMatchMonitor", HistoPrint=HistoPrint))

    return seq
    
def ConfiguredOTMonitorSequence(Name = "MoniOTSeq",
                                HistoPrint = False ):
    seq = GaudiSequencer(Name)
    seq.Members.append( OTTrackMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( OTHitEfficiencyMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( OTTimeMonitor(HistoPrint=HistoPrint) )

    from Configurables import TrackSys
    if TrackSys().cosmics():
        OTTrackMonitor().RawBankDecoder = 'OTMultiBXRawBankDecoder'
        OTHitEfficiencyMonitor().RawBankDecoder = 'OTMultiBXRawBankDecoder'
        OTTimeMonitor().RawBankDecoder = 'OTMultiBXRawBankDecoder'

    return seq

