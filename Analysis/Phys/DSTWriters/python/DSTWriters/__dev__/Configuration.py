"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('SelDSTWriter',
           'MicroDSTWriter',
           'StripDSTWriter',
           'selDSTStreamConf',
           'microDSTStreamConf',
           'stripDSTStreamConf',
           'selDSTElements',
           'microDSTElements',
           'stripDSTElements')

from GaudiConf.Configuration import *
from Configurables import GaudiSequencer, InputCopyStream, OutputStream

from dstwriters     import DSTWriterSelectionSequence
from dstwriterutils import MicroDSTElementList
from seldst         import selDSTStreamConf, selDSTElements
from microdst       import microDSTStreamConf, microDSTElements
from stripdst       import stripDSTStreamConf, stripDSTElements

from copy import copy

class SelDSTWriter(ConfigurableUser) :
    """
    Write a DST for a set of selection sequences. Writes out the entire
    contents of the input DST file, plus optional extra items from the TES.
    """
    __slots__ = {
        "OutputFileSuffix"           : "Sel"
        , "SelectionSequences"       : []
        , "WriteFSR"                 : True
        , "StreamConf"               : { 'default' : selDSTStreamConf() }
        , "MicroDSTElements"         : { 'default' : selDSTElements()   } }

    _propertyDocDct = {  
        "OutputFileSuffix"             : """Add to name of output DST file. Default 'Sel'"""
        , "SelectionSequences" : """ List of SelectionSequences defining the selections"""
        , "WriteFSR"           : """ Flags whether to write out an FSR """
        , "StreamConf"         : """ Output stream configuration """
        , "MicroDSTElements"   : """ List of callables setting up each element to be added to each SelectionSequence and copied to MicroDST partition."""
        }

    def _buildClonerList(self, selSeq) :
        '''
        Build list of callables that will be used by the BaseDSTWriter.
        '''
        clonerDict = copy(self.getProp('MicroDSTElements'))
        seqName = selSeq.name()
        clonerList = []
        if seqName in clonerDict.keys() :
            clonerList = clonerDict[seqName]
        else :
            clonerList = clonerDict['default']
        
        return MicroDSTElementList(branch = seqName, callables = clonerList)

    def _buildStreamConf(self, selSeq) :
        scDict = copy(self.getProp('StreamConf'))
        seqName = selSeq.name()
        sc = None
        if seqName in scDict.keys() :
            sc = scDict[seqName]
        else :
            sc = scDict['default']
        prefix = self.getProp('OutputFileSuffix')
        if prefix != '' :
            sc.filePrefix = self.getProp('OutputFileSuffix') + '.'
        return sc
    
    def sequence(self) :
        return GaudiSequencer(self.name() + "MainSeq",
                              ModeOR = True, 
                              ShortCircuit = False)

    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration. Creates a DSTWriterSelectionSequence for
        each input SelectionSequence and adds it to the main sequence.
        """
        log.info("Configuring BaseDSTWriter")
        for sel in self.getProp('SelectionSequences') :
            seq = DSTWriterSelectionSequence(selSequence = sel,
                                             outputStreamConfiguration = self._buildStreamConf(sel),
                                             extras = self._buildClonerList(sel),
                                             writeFSR = self.getProp('WriteFSR'))
            self.sequence().Members += [ seq.sequence() ]

class MicroDSTWriter(SelDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """
    __slots__ = { "StreamConf"       : { 'default' : microDSTStreamConf() },
                  "MicroDSTElements" : { 'default' : microDSTElements() }   }

class StripDSTWriter(SelDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """
    __slots__ = { "StreamConf"       : { 'default' : stripDSTStreamConf() },
                  "MicroDSTElements" : { 'default' : stripDSTElements() }   }
