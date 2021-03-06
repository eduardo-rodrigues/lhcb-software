# Special options for 2011 25ns fill, without Hlt2

# Syntax is:
#   gaudirun.py COLLISION11_25.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/2011-25ns.py")

# Events from run 103053, fill 2186 on 2011-10-07, 25ns bunch spacing
from PRConfig import TestFileDB
TestFileDB.test_file_db['2011_25ns_raw_default'].run(withDB=False)
