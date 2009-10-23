### @file DVTest-B2HH.py
 #
 #  Test file for HLT on selected B2HH events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="B2HH"  
               , Decay = "{[[B0]nos -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}]cc, [[B0]os -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}]cc}"# we know it's Kpi
               # Angelo Carbone, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/a/acarbone/MC09_bd2kpi.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

