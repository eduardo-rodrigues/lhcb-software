#
#   Dimuon stripping selections 
# 
#   all physics channels with at least 2 electrons inside
#   (with minimal electron ID required)
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingBd2eeKstar import Bd2eeKstarConf
from StrippingSelections.StrippingBd2JpsieeKstar import Bd2JpsieeKstarConf

#from StrippingSelections.StrippingBu2LLK       import StrippingBu2LLKConf
from StrippingSelections.StrippingDiElectron   import StrippingDiElectronConf
from StrippingSelections.StrippingBs2JpsieePhi import StrippingBs2JpsieePhiConf


stream = StrippingStream("Dielectron")
stream.appendLines( [
    Bd2eeKstarConf().line,
    Bd2JpsieeKstarConf().line,    
#    StrippingBu2LLKConf().eeK(),
    StrippingDiElectronConf().LooseJpsi2ee_line(),
    StrippingDiElectronConf().Jpsi2ee_line(),
    StrippingDiElectronConf().IncDiElectron_line(),
    StrippingDiElectronConf().IncDiElectronLowMass_line(),
    StrippingDiElectronConf().BiasedIncDiElectron_line(),
    StrippingDiElectronConf().BiasedIncDiElectronLowMass_line(),
    StrippingBs2JpsieePhiConf().UnbiasedPT_line(),
    StrippingBs2JpsieePhiConf().Signalbox_line()
    ] )
