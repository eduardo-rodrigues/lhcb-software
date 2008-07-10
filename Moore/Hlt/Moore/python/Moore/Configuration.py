"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-07-10 14:15:57 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *

import GaudiKernel.ProcessJobOptions

class Moore(ConfigurableUser):
    __slots__ = {
        "EvtMax":          -1 # Maximum number of events to process
        }   
                
    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions('$STDOPTS/LHCbApplication.opts')
        importOptions('$STDOPTS/DstDicts.opts')
        importOptions('$STDOPTS/DC06Conditions.opts')
        LHCbApp().applyConf()

                                                                                                                                                                                                                                                                                                 