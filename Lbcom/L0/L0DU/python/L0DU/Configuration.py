"""
Configuration tools for the L0 trigger
"""

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import LHCbConfigurableUser
 
## @class L0Conf
#  Configurable for the L0 trigger (simulation, emulation, decoding, monitoring and filtering)
#  @author Julien Cogan <cogan@cppm.in2p3.fr>
#  @date   18/02/2009
class L0Conf(LHCbConfigurableUser) :

    __slots__ = {
        # Properties
         "ReplaceL0BanksWithEmulated" : False
        ,"SimulateL0"     : False
        ,"EmulateL0"      : False
        ,"DecodeL0"       : False
        ,"FilterL0"       : False
        ,"DecodeL0DU"     : False
        ,"FilterL0FromRaw": False
        ,"MonitorL0"      : False
        # Sequencers 
        ,"L0Sequencer"    : None 
        ,"LinkSequencer"  : None 
        ,"MoniSequencer"  : None 
        ,"FilterSequencer": None 
        ,"ETC"            : False
        }

    _propertyDocDct = {
        # Properties
         "ReplaceL0BanksWithEmulated" : """ If True, run the emulators and replace the existing L0 banks."""
        ,"SimulateL0"     : """ If True, run the L0 simulation and write L0Banks."""
        ,"EmulateL0"      : """ If True, run the L0 emulators and write on TES at a non default location."""
        ,"DecodeL0"       : """ If True, run the L0 decoding (decode all L0 banks)."""
        ,"DecodeL0DU"     : """ If True, run the L0DU decoding."""
        ,"MonitorL0"      : """ If True, run the L0 monitoring algorithms."""
        ,"FilterL0FromRaw": """ If True, run the L0DU decoding and filter according to L0 decision."""
        ,"FilterL0"       : """ If True, filter according to L0 decision."""
        # Sequencers 
        ,"L0Sequencer"    : """ Sequencer filled according to the L0Conf properties."""
        ,"LinkSequencer"  : """ Sequencer filled with the MC associator algorithms (not configurable)."""
        ,"MoniSequencer"  : """ Sequencer filled with the L0 monitoring sequence (not configurable)."""
        ,"FilterSequencer": """ Sequencer filled with the L0Filter algorithm (not configurable)."""
        ,"ETC"            : """ If True, import the ETC option file to write out a L0-ETC."""
        }

    def checkOptions(self):
        """
        Sanity checks.
        """
        if self.getProp("SimulateL0") and self.getProp("EmulateL0"):
            raise L0ConfError("SimulateL0","EmulateL0","Simulation and Emulation can not run in the same job.")
        if self.getProp("ReplaceL0BanksWithEmulated") and self.getProp("SimulateL0"):
            raise L0ConfError("ReplaceL0BanksWithEmulated","SimulateL0")
        if self.getProp("ReplaceL0BanksWithEmulated") and self.getProp("EmulateL0"):
            raise L0ConfError("ReplaceL0BanksWithEmulated","EmulateL0")
        if self.getProp("DecodeL0") and self.getProp("DecodeL0DU"):
            raise L0ConfError("DecodeL0","DecodeL0DU")
        if self.getProp("FilterL0FromRaw") and self.getProp("FilterL0"):
            raise L0ConfError("FilterL0FromRaw","FilterL0")
        if self.getProp("FilterL0FromRaw") and self.getProp("DecodeL0"):
            raise L0ConfError("FilterL0FromRaw","DecodeL0")
        if self.getProp("FilterL0FromRaw") and self.getProp("DecodeL0DU"):
            raise L0ConfError("FilterL0FromRaw","DecodeL0DU")

    def _defineL0Sequencer(self):
        """
        Fill the sequencer given to the L0Sequencer attribute.
        The filling of the sequencer is done according to the L0Conf properties. 
        """
        if self.isPropertySet("L0Sequencer"):

            seq=self.getProp("L0Sequencer")

            if self.getProp("ReplaceL0BanksWithEmulated"):
                importOptions("$L0DUROOT/options/ReplaceL0BanksWithEmulated.opts")
                seq.Members+= [ GaudiSequencer("ReplaceL0BanksSeq") ]
                log.warning("\n  \tEXISTING L0 BANKS WILL BE REMOVED AND REPLACED BY EMULATED BANKS\n\n")

            if self.getProp("SimulateL0"):
                importOptions("$L0DUROOT/options/Boole.opts")
                seq.Members+= [GaudiSequencer("L0SimulationSeq") ]

            if self.getProp("DecodeL0"):
                importOptions("$L0DUROOT/options/L0Sequence.opts")
                seq.Members+= [ GaudiSequencer("L0FromRawSeq") ]

            if self.getProp("EmulateL0"):
                importOptions("$L0DUROOT/options/L0EmulatorSeq.opts")
                seq.Members+= [ GaudiSequencer("L0EmulationSeq") ]

            if self.getProp("MonitorL0"):
                importOptions("$L0DUROOT/options/L0Sequence.opts")
                seq.Members+= [ GaudiSequencer("L0MoniSeq") ]
                
            if self.getProp("FilterL0FromRaw"):
                importOptions("$L0DUROOT/options/L0Sequence.opts")
                seq.Members+= [ GaudiSequencer("L0FilterFromRawSeq") ]
                
            if self.getProp("FilterL0"):
                from Configurables import L0Filter
                seq.Members+= [ L0Filter() ]
                
    def _defineL0LinkSequencer(self):                                                        
        """
        Fill the sequencer given to the LinkSequencer attribute (not configurable).
        This sequencer is always filled with 2 L0CaloToMCParticleAsct algorithms.
        """
        if self.isPropertySet("LinkSequencer"):
            seq=self.getProp("LinkSequencer")
            from Configurables import L0CaloToMCParticleAsct
            seq.Members += [ L0CaloToMCParticleAsct() ]
            seq.Members += [ L0CaloToMCParticleAsct("L0CaloFullTruth", InputContainer = "Trig/L0/FullCalo") ]

    def _defineL0MoniSequence(self):
        """
        Fill the sequencer given to the MoniSequencer attribute (not configurable).
        This sequencer is always filled with the L0MoniSeq sequencer.
        """
        if self.isPropertySet("MoniSequencer"):
            seq=self.getProp("MoniSequencer")
            importOptions("$L0DUROOT/options/L0Sequence.opts")
            seq.Members+= [ GaudiSequencer("L0MoniSeq") ]

    def _defineL0FilterSequence(self):
        """
        Fill the sequencer given to the FilterSequencer attribute (not configurable).
        This sequencer is always filled with the L0Filter algorithm.
        """
        if self.isPropertySet("FilterSequencer"):
            seq=self.getProp("FilterSequencer")
            from Configurables import L0Filter
            seq.Members+= [ L0Filter() ]

    def _defineETC(self):
        """
        Import the option file for writing a L0-ETC.
        """
        if self.getProp("ETC"):
            importOptions( "$L0DUROOT/options/ETC.opts" )
        
    def __apply_configuration__(self):
        """
        L0Conf configuration.
        """
        self.checkOptions()
        self._defineL0Sequencer()
        self._defineL0LinkSequencer()
        self._defineL0MoniSequence()
        self._defineL0FilterSequence()
        self._defineETC()


class L0ConfError(Exception):
    """ Raised when conflicting options have been selected in L0Conf."""
    def __init__(self,prop1,prop2,message=''):
        self._prop1=prop1
        self._prop2=prop2
        self._message=message
    def __str__(self):
        s="Conflict in L0Conf options : %s and %s are not compatible. %s\n" % (self._prop1,self._prop2,self._message)
        return s
