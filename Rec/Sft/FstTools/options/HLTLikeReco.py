from Configurables import Brunel
from Configurables import MessageSvc
from Configurables import RecMoniConf
from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker

from GaudiConf import IOHelper
import Gaudi.Configuration as GC
from FstTools.Configuration import StagedRecoConf


RecMoniConf().MoniSequence = []

Brunel().RecoSequence = ["L0", "HLT"]
Brunel().MCLinksSequence = ["Unpack", "Tr"]
Brunel().MCCheckSequence = ["Pat"]
Brunel().OutputType = "NONE"
Brunel().DataType = "2012"
Brunel().InputType = "DIGI"
Brunel().WithMC = True
Brunel().PrintFreq = 100
Brunel().Simulation = True
Brunel().EvtMax = 5000
Brunel().CondDBtag = 'sim-20131023-vc-md100' # use the mu100 for MagUp data
Brunel().DDDBtag = 'dddb-20130929-1'

MessageSvc().Format = '% F%50W%S%7W%R%T %0W%M'

staged = StagedRecoConf()
#staged.AddToBest = ["Fwd1", "Fwd2", "Match"]

# sim://MC/Dev/Beam6500GeV-RunII-MagDown-Nu1.5-25ns-Pythia8/Sim08e/30000000 ( minbias )/XDIGI
#
# dirac-dms-get-file /lhcb/MC/Dev/XDIGI/00034696/0000/00034696_00000024_1.xdigi
inputFiles = ['/tmp/thead/minbias/00034696_00000024_1.xdigi']
IOHelper('ROOT').inputFiles(inputFiles)

def setup_mc_truth_matching():
    GaudiSequencer("CaloBanksHandler").Members = []
    GaudiSequencer("DecodeTriggerSeq").Members = []
    GaudiSequencer("MCLinksTrSeq").Members = ["PrLHCbID2MCParticle", "PrTrackAssociator"]
    GaudiSequencer("CheckPatSeq" ).Members = ["PrChecker"]
    
    PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"

    PrChecker().TriggerNumbers = True
    PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
    PrChecker().UpTracks = "/Event/Fst/Track/VeloTT"
    PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"
    PrChecker().SeedTracks = "/Event/Fst/Track/Seed"
    PrChecker().MatchTracks = "/Event/Fst/Track/Match"
    PrChecker().DownTracks = "/Event/Fst/Track/Downstream"
    PrChecker().BestTracks = "/Event/Fst/Track/Best"
GC.appendPostConfigAction(setup_mc_truth_matching)
