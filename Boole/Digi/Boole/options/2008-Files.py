# Example 2008 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py 2008-Files.py
#

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Boole.Configuration import *

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Main ('signal') event input
datasetName = '11144103-500ev-20080228' # Events from Gauss v30r1
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2008/v31r0/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Spillover events
EventSelector("SpilloverSelector").Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2008/v31r0/30000000-1000ev-20080223.sim' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

# Set the property used to build other file names
Boole().setProp( "datasetName", datasetName )

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Boole().histosName()

#-- Possible output streams. Enabled by setting the corresponding Boole() property
outputName = Boole().outputName()
# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
OutputStream("RawWriter").Output = "DATAFILE='PFN:" + outputName + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
# writeDigi = true: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + outputName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
# write L0ETC = true: ETC with L0 result
TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + datasetName + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
