#$Id: Wrappers.py,v 1.50 2010-08-11 08:42:14 ibelyaev Exp $
"""
Wrapper classes for a DaVinci offline physics selection. The following classes
are available:

   - Selection          Wraps a selection configurable and the selections it
                        requires
   - DataOnDemand       Wraps a string TES location to make it look like a Seleciton
   - AutomaticData      Wraps a string TES location to make it look like a Seleciton
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
   - MergedSelection    Merges a set of selections in OR mode, writing all data to a single location.
   - EventSelection     Wraps an algorithm that selects an event and produces no output data.
   - VoidEventSelection Selects event based on TES container properties, applying a LoKi__VoidFilter compatible cut to the data from a required selection.
   - PassThroughSelection
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('DataOnDemand',
           'AutomaticData',
           'Selection',
           'MergedSelection',
           'EventSelection',
           'PassThroughSelection',
           'SelectionSequence',
           'MultiSelectionSequence',
           'SelSequence',
           'NameError',
           'NonEmptyInputLocations',
           'IncompatibleInputLocations'
           )

from copy import copy


from Configurables import GaudiSequencer

from SelPy.selection import ( flatAlgorithmList,
                              NamedObject,
                              UniquelyNamedObject,
                              SelectionBase,
                              AutomaticData,
                              NameError,
                              NonEmptyInputLocations,
                              IncompatibleInputLocations,
                              SelSequence                )

from SelPy.utils import update_dict_overlap

from SelPy.selection import Selection as Sel
from SelPy.selection import EventSelection as EvtSel
from SelPy.selection import PassThroughSelection as PassThroughSel
from SelPy.selection import AutomaticData as autodata

from Configurables import LoKi__VoidFilter as VoidFilter
from GaudiConfUtils import configurableExists, isConfigurable
from GaudiConfUtils import ConfigurableGenerators

def checkName(name) :
    if configurableExists(name) :
        raise NameError('Target Configurable '+ name + ' already exists. Pick a new one')

def selectionWrapper(selType, name, *args, **kwargs) :
    """
    Construct a selection of type selType with construction arguments.
    Checks if name already belongs to an existing Configurable.
    Checks whether input is Configurable and if so, extracts its non-default
    parameters and passes them as a configurableGenerator.
    """
    checkName(name)

    algorithm = kwargs.pop('Algorithm')
    if isConfigurable( algorithm )  :
        algGen = getattr(ConfigurableGenerators, algorithm.__class__.__name__)(**algorithm.getValuedProperties())
        kwargs['ConfGenerator'] = algGen
    else :
        kwargs['ConfGenerator'] = algorithm
    return selType(name, *args, **kwargs)

def Selection(name, *args, **kwargs) :
    """
    Wrapper around SelPy.Selection. Since SelPy.Selection takes a
    ConfigurableGenerator as input and constructs a Configurable 
    with it it's own name, check first whether a configurable
    with that name already exists and raise a NameError if that is the case.
    If not, construct and return a SelPy.Selection.
    """
    return selectionWrapper(Sel, name, *args, **kwargs)

def EventSelection(name, *args, **kwargs) :
    """
    Wrapper around SelPy.EventSelection. Since SelPy.EventSelection takes a
    ConfigurableGenerator as input and constructs a Configurable 
    with it it's own name, check first whether a configurable
    with that name already exists and raise a NameError if that is the case.
    If not, construct and return a SelPy.EventSelection.
    """
    return selectionWrapper(EvtSel, name, *args, **kwargs)

def PassThroughSelection(name, *args, **kwargs) :
    """
    Wrapper around SelPy.PassThroughSelection. Since
    SelPy.PassThroughSelection takes a
    ConfigurableGenerator as input and constructs a Configurable 
    with it it's own name, check first whether a configurable
    with that name already exists and raise a NameError if that is the case.
    If not, construct and return a SelPy.EventSelection.
    """
    return selectionWrapper(PassThroughSel, name, *args, **kwargs)    

class AutomaticData(NamedObject, SelectionBase) :
    """
    Simple wrapper for a data location. To be used for locations
    that are guaranteed to be populated. This could be a location
    on a DST or a location registered to the DataOnDemandSvc.
    Returns output location via outputLocation() method.
    Can be used as a Selection in RequiredSelections field of other
    Selections.

    Example: wrap StdLoosePions

    >>> SelStdLoosePions = AutomaticData(Location = 'Phys/StdLoosePions')
    >>> SelStdLoosePions.outputLocation()
    'Phys/StdLoosePions'
    >>> SelStdLoosePions.name()
    'Phys_StdLoosePions'
    >>> pions = AutomaticData(Location='Phys/StdLoosePions/Particles')
    >>> pions.outputLocation()
    'Phys/StdLoosePions/Particles'
    """

    def __init__(self,
                 Location,
                 Extension='Particles') :

        _extension = ''
        if Extension != '' :
            _extension = ('/'+Extension).replace('//', '/')
        _location = (Location + _extension).replace(_extension+_extension, _extension)

        NamedObject.__init__(self, _location.replace('/', '_'))

        _alg = VoidFilter('SelFilter'+self.name(),
                          Code = "CONTAINS('"+_location+"')>0")

        SelectionBase.__init__(self,
                               algorithm = _alg,
                               outputLocation=_location,
                               requiredSelections = [] )

DataOnDemand = AutomaticData


class VoidEventSelection(UniquelyNamedObject, SelectionBase) :
    """
    Selection class for event selection based on contents of TES location.
    Can be used just like a Selection object.
    Constructor argument Code is LoKi__VoidFilter compatible single-location
    cut, where the location is expressed as '<Location>' and determined
    from the RequiredSelection's outputLocation(). The outputLocation() of
    this algorithm is the same as that of the RequiredSelection.
    Example:
    from PhysSelPython.Wrappers import VoidEventSelection, SelectionSequence
    evtSel = VoidEventSelection( name='MyEvtSel',
                                 Code="CONTAINS('<Location>')>0",
                                 RequiredSelection = AutomaticData(....) )
    help(SelectionSequence)
    selSeq = SelectionSequence('MyEvtSelSeq', TopSelection = evtSel)
    """

    __author__ = "Juan Palacios palacios@physik.uzh.ch"
    
    def __init__(self,
                 name,
                 Code,
                 RequiredSelection ) :

        UniquelyNamedObject.__init__(self, name)

        checkName(self.name())
        
        _code = Code.replace('<Location>',
                             "'"+RequiredSelection.outputLocation()+"'")
        _code = _code.replace('\"\'', '\'').replace('\'\"', '\'').replace('\'\'','\'')
        _alg = VoidFilter(self.name(), Code = _code)

        SelectionBase.__init__(self,
                               algorithm = _alg,
                               outputLocation = RequiredSelection.outputLocation(),
                               requiredSelections = [RequiredSelection])

    
class MergedSelection(NamedObject, SelectionBase) :
    """
    Selection wrapper class for merging output of various Selections.
    Can be used just like any Selection object.
    Example:
    
    from PhysSelPython.Wrappers import MergedSelection, AutomaticData, SelectionSequence
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    merge = MergedSelection('SelMerger', RequiredSelections = [sel00, sel01, sel02, sel03])
    seqMerge = SelectionSequence('SeqMergeSelections, TopSelection = merge)

    This will OR all the selections and place the output in merge.outputLocation().
    """
    def __init__(self,
                 name,
                 RequiredSelections = [],
                 OutputBranch = "Phys",
                 sequencerType=GaudiSequencer) :

        NamedObject.__init__(self, name)
        self.__ctor_dict__ = dict(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']

        self._sel = Selection(name,
                              Algorithm = ConfigurableGenerators.FilterDesktop(Code='ALL'),
                              RequiredSelections = RequiredSelections,
                              OutputBranch = OutputBranch)
        
        self._algos = flatAlgorithmList(self._sel)

        _alg = sequencerType('Seq'+self.name(),
                             Members = self._algos,
                             ModeOR = True,
                             ShortCircuit = False)

        SelectionBase.__init__(self,
                               algorithm = _alg,
                               outputLocation = self._sel.outputLocation(),
                               requiredSelections = [])

    def clone(self, name, **args) :
        new_dict = update_dict_overlap(self.__ctor_dict__, args)
        return MergedSelection(name, **new_dict)

class SelectionSequence(SelSequence) :
    """
    Wrapper class for offline selection sequence creation.
    Takes a Selection object
    corresponding to the top selection algorithm, and recursively uses
    Selection.requiredSelections to form a GaudiSequencer with all the required
    selections needed to run the top selection. Can add list of event selection
    algorithms to be added at the beginning of the sequence, and a list of
    algorithms to be run straight after the selection algoritms.
    Wraps SelSequence, simply adding a method sequence() that creates the
    GaudiSequencer on-demand.
    Example: selection sequence for A -> B(bb), C(cc). Add pre-selectors alg0
             and alg1, and counter counter0.

    # Assume module A2B2bbC2cc defining a Selection object for the decay
    # A -> B(bb), C(cc)
    from A2B2bbC2cc import SelA2B2bbC2cc
    from PhysSelPython.Wrappers import SelectionSequence
    SeqA2B2bbC2cc = SelectionSequence( 'SeqA2B2bbC2cc',
                                       TopSelection = SelA2B2bbC2cc,
                                       EventPreSelector = [alg0, alg1],
                                       PostSelectionAlgs = [counter0]   )
    # use it
    mySelSeq = SeqA2B2bbC2cc.sequence()
    dv = DaVinci()
    dv.UserAlgorithms = [mySelSeq]

    Uses selection.SelSequence and selection.flatAlgorithmList
    help(SelSequence)
    help(flatAlgorithmList)
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 name,
                 TopSelection,
                 EventPreSelector = [],
                 PostSelectionAlgs = [],
                 sequencerType = GaudiSequencer) :
        SelSequence.__init__(self,
                             name,
                             TopSelection,
                             EventPreSelector,
                             PostSelectionAlgs)

        checkName(self.name())
        self._gaudiseq = sequencerType(self.name(), Members = self._algos)

    def sequence(self) :
        return self._gaudiseq

    def clone(self, name, **args) :
        new_dict = update_dict_overlap(self.__ctor_dict__, args)
        return SelectionSequence(name, **new_dict)


    
class MultiSelectionSequence(UniquelyNamedObject) :
    """
    Wrapper class for offline multiple selection sequence creation.
    Takes a list of SelectionSequence objects and produces on demand a
    sequence with an OR of the list of sequences. 
    Uses SelectionSequence:
    help(SelSequence)
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 name,
                 Sequences = [],
                 sequencerType = GaudiSequencer) :

        UniquelyNamedObject.__init__(self, name)
        self._sequences = list(Sequences)
        self._gaudiseq = None
        self._algos = []
        for seq in self._sequences :
            self._algos += seq.algorithms()

        checkName(self.name())
        self._gaudiseq = sequencerType(self.name(),
                                      ModeOR = True,
                                      ShortCircuit = False,
                                      Members = [seq.sequence() for seq in self._sequences])

    def algorithms(self) :
        return list(self._algos)
        
    def outputLocations(self) :
        return [seq.outputLocation() for seq in self._sequences]

    def sequence(self) :
        return self._gaudiseq
