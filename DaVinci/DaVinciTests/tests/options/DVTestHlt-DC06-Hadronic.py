########################################################################
#
# $Id: DVTestHlt-DC06-Hadronic.py,v 1.3 2009/10/16 08:39:00 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
importOptions( "$DAVINCITESTSROOT/tests/options/DVTestHlt-DC06-None.py")
#
# now overwrite threshold setting
#
from Configurables import DaVinci
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_HadronicHlt2_Jul09'
