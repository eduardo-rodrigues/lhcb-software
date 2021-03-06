#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsDownElectrons.py,v 1.1 2010-08-11 12:43:05 ukerzel Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsElectrons.py
#  configuration file for 'Standard NoPIDs Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard DiElectrons from gamma conversion with PT reversed for background studies'
"""
__author__  = "Olivier Deschamps : Olivier.Deschamps @cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdAllTightSymGammaLL' ,
    'StdAllTightSymGammaDD' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from CommonParticles.Utils import *
from GaudiKernel.SystemOfUnits import *


###--- Long pair
from Configurables import DiElectronMaker,ProtoParticleCALOFilter
dieSymLL = DiElectronMaker('StdAllTightSymGammaLL')
dieSymLL.DecayDescriptor = "gamma -> e+ e-"
selector = trackSelector ( dieSymLL , trackTypes = [ "Long"]) 
dieSymLL.addTool( ProtoParticleCALOFilter('Electron'))
dieSymLL.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'0.0'"]
dieSymLL.DeltaY = 3.
dieSymLL.DeltaYmax = 200 * mm
dieSymLL.DiElectronMassMax = 100.*MeV
dieSymLL.DiElectronPtMin = 400.*MeV
dieSymLL.SymetricPair = True

locations = updateDoD ( dieSymLL )
StdAllTightSymGammaLL=dieSymLL

###--- Downstream pair
dieSymDD = DiElectronMaker('StdAllTightSymGammaDD')
dieSymDD.DecayDescriptor = "gamma -> e+ e-"
selector = trackSelector ( dieSymDD , trackTypes = [ "Downstream"]) 
dieSymDD.addTool( ProtoParticleCALOFilter('Electron'))
dieSymDD.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'0.0'"]
dieSymDD.DeltaY = 3.
dieSymDD.DeltaYmax = 200 * mm
dieSymDD.DiElectronMassMax = 100.*MeV
dieSymDD.DiElectronPtMin = 400.*MeV
dieSymDD.SymetricPair = True

locations.update( updateDoD ( dieSymDD ) )
StdAllTightSymGammaDD=dieSymDD



## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
