########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2009-11-30
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DaVinci

########################################################################
# CheatedSelection. 

from Configurables import CheatedSelection, PhysDesktop

cheat = CheatedSelection("CheatedSelection")
cheat.InputLocations = [ "Phys/TaggingPions" ]
cheat.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheat.OutputLevel = 2

########################################################################
# Flavour tagging. 

from Configurables import BTagging, BTaggingTool, BTaggingAnalysis, BTaggingChecker

location = "/Event/Sel/Phys/CheatedSelection"

tag = BTagging("BTagging")
tag.InputLocations = [ location ]
tag.OutputLevel = 2
tag.addTool( PhysDesktop )
tag.PhysDesktop.OutputLevel = 3

########################################################################
# Flavour tagging Checker:

tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"
tagcheck.OutputLevel = 2

########################################################################
# BTaggingAnalysis ntuple creation

tagana = BTaggingAnalysis("BTaggingAnalysis")
tagana.InputLocations = [ location , "Phys/TaggingPions" ]
tagana.TagOutputLocation =  location+"/FlavourTags"
tagana.OutputLevel = 2

########################################################################
# Standard configuration

MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

DaVinci().EvtMax     = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().HistogramFile = "DVHistos.root"     # Histogram file
DaVinci().TupleFile     = "analysis.root"     # Ntuple

DaVinci().Simulation = True
DaVinci().DataType   = "MC09" 
#DaVinci().DataType   = "DC06" 
#importOptions("$FLAVOURTAGGINGOPTS/BTaggingTool_DC06.py") #to switch to DC06 tuning

from Configurables import PrintDecayTree
PrintDecayTree().InputLocations = [ location ] 

DaVinci().MoniSequence = [ PrintDecayTree(),
                           cheat,
                           tag,
                           tagcheck,
                           tagana
                         ]  # The algorithms

########################################################################
# example data file

#EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/p/pkoppenb/MC09-Bu2eeK/Bu2LLK-1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

#bsdspi_nu3_2.txt.gz
EventSelector().Input   = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/DST/00005272/0000/00005272_00000002_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

