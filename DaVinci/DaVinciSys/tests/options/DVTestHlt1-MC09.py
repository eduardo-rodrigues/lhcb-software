########################################################################
#
# $Id: DVTestHlt1-MC09.py,v 1.6 2010-02-09 15:25:42 pkoppenb Exp $
#
# Options for Hlt1 tests. Identical to Hlt1&2, except for type.
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
importOptions("$DAVINCISYSROOT/tests/options/DVTestHlt12-MC09.py")
########################################################################
#
# HLT
#
from Configurables import DaVinci
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Jan10'  ## only Hlt1
