### @file
#
# Standard Ks
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.GoodParticles import GoodPions
from Configurables import CombineParticles
from HltConf.HltLine import bindMembers

__all__ = ( 'KsLL' )

Hlt2SharedKsLL = CombineParticles("Hlt2SharedKsLL")
Hlt2SharedKsLL.DecayDescriptor = "KS0 -> pi+ pi-" 
Hlt2SharedKsLL.DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" } 
Hlt2SharedKsLL.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
Hlt2SharedKsLL.MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2/VDOF)<30)"
Hlt2SharedKsLL.InputLocations = [ "Hlt2GoodPions" ]

KsLL = bindMembers( None, [ GoodPions, Hlt2SharedKsLL ] )
