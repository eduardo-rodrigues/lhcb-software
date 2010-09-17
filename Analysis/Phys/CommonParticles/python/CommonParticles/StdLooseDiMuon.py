"""
Configuration file for Standard Loose DiMuon, very loose for penguins
"""

__author__ = 'Rob Lambert'
__date__ = '07/07/2009'


from Gaudi.Configuration import *
from Configurables import CombineParticles 
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdLooseDiMuon #
###########################

StdLooseDiMuon = CombineParticles("StdLooseDiMuon")
StdLooseDiMuon.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
StdLooseDiMuon.InputLocations = ["StdLooseMuons"]
StdLooseDiMuon.CombinationCut = "(ADAMASS('J/psi(1S)') < 3000*MeV) & (ADOCACHI2('') < 30.)"
StdLooseDiMuon.MotherCut = "(VFASPF(VCHI2) < 25)"
locations = updateDoD(StdLooseDiMuon)

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )
    

