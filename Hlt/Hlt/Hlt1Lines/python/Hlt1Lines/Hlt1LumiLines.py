##############################
#     HLT Lumi Line(s)
##############################

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from HltLine.HltLinesConfigurableUser import *

from Hlt1Lines.LumiCounterDefinition import LumiCounterDefinitionConf
from Configurables import ( LumiCountVertices,
                            LumiCountTracks,
                            LumiFromL0DU,
                            LumiCountHltTracks,
                            #LumiFlagMethod
                            )
from Configurables import GaudiSequencer as Sequence
from Configurables import DeterministicPrescaler as Scaler


####### create binders...
def _createCounter( counterKind, seqName, seq, enableNonL0Counters ) :
    if counterKind is LumiFromL0DU:
        def _fun (name, value ) :
          from HltLine.HltDecodeRaw import DecodeL0DU
          if [ i for i in DecodeL0DU.members() if i not in seq.Members ] :
              seq.Members +=  DecodeL0DU.members() 
          seq.Members.append( LumiFromL0DU( seqName+name
                                          , InputSelection='Trig/L0/L0DUReport'
                                          , CounterName=name
                                          , ValueName=value
                                          , OutputContainer='Hlt/LumiSummary' ) )
        return _fun
    return lambda name, inputSel : seq.Members.append( counterKind( seqName + name
                                                     , InputSelection = inputSel
                                                     , CounterName = name
                                                     , Enable = enableNonL0Counters
                                                     , OutputContainer='Hlt/LumiSummary' ) )

####### operator, meet arguments...
def _combine( op, arg ) :
    for key,value in arg.iteritems() : op(key,value)
    return arg.keys()

############# start building the lumi line(s)...
class Hlt1LumiLinesConf(HltLinesConfigurableUser) :
    __used_configurables__ = [ LumiCounterDefinitionConf ]

    __slots__ = { 'TriggerType'            : 'LumiTrigger'  # ODIN trigger type accepted for Lumi
                , 'L0Channel'              : ['CALO']     # L0 channels accepted for LowLumi
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                , 'LumiLines'              : ['Count','VDM']
                , 'EnableReco'             : True 
                , 'OutputLevel'            : WARNING
                }

    def __create_lumi_algos__(self, BXType, postfix=''):
        '''
        returns algorithm sequences for Hlt1 Lumi Lines
        '''
        # get counters
        counters = LumiCounterDefinitionConf().defineCounters()
        
        # debugging options
        debugOPL = self.getProp('OutputLevel')
        from HltLine.HltReco import PV2D

        # define reco scaler
        recoScaler = Scaler( 'LumiRecoScaler' ,  AcceptFraction = 1 if self.getProp('EnableReco') else 0 )  

        # define empty reco sequence
        seqRecoName = 'LumiReco'

        lumiRecoSequence = Sequence(seqRecoName+'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , OutputLevel = debugOPL
                                    , IgnoreFilterPassed = True
                                    , Members = [] # reset so we build the same things several times TODO: move out of loop...
                                    , MeasureTime = True)

        # define empty sequence to collect counters
        seqCountName = 'LumiCount'+postfix+BXType
        lumiCountSequence = Sequence(seqCountName +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , OutputLevel = debugOPL
                                    , IgnoreFilterPassed = True
                                    , MeasureTime = True)

        # LumiLow lines must be flagged - do not flag the traditional method, would interfere
        if postfix == 'Low':
            lumiCountSequence.Members.append( LumiFlagMethod( seqCountName+'FlagMethod'
                                                              , CounterName='Method'
                                                              , ValueName='L0RateMethod'
                                                              , OutputContainer='Hlt/LumiSummary' ) )
            
        # populate count sequence from the definition
        createdCounters = []
        histoThresholds = []
        histoMaxBins = []
        for key, definition in counters.iteritems():
            # example 'CaloEt' : [LumiFromL0DU , True , 'Sum(Et)' , 500, 6000],
            (op, flag, inputDef, threshold, bins) = definition
            if flag:
                createdCounters.extend( 
                    _combine( _createCounter( op, seqCountName, lumiCountSequence, True ),
                              { key : inputDef } ) )
                histoThresholds.extend( [threshold] )
                histoMaxBins.extend( [bins] )
                if debugOPL <= DEBUG:
                    print '# DEBUG   : Hlt1LumiLines::HistoMaker:', postfix+BXType, key, threshold, bins
                
        lumiRecoSequence.Members.append( Sequence('LumiTrackRecoSequence' ,
                                                   Members = [  recoScaler ] + PV2D.members(),
                                                   MeasureTime = True ) ) 

        # filter to get backward tracks (make sure it always passes by wrapping inside a sequence)
        from Configurables import HltTrackFilter
        lumiRecoFilterSequence = Sequence( 'LumiRecoFilterSequence', Members = [] ) # reset, always build the same seq...
        lumiRecoFilterSequence.Members.append( recoScaler )
        lumiRecoFilterSequence.Members.append(
            Sequence('HltRZVeloBWSequence'
                     , Members  = [ HltTrackFilter('HltPrepareRZVeloBW'
                                                   , InputSelection   = 'TES:Hlt/Track/RZVelo'
                                                   , AddInfo = False
                                                   , FilterDescriptor = ['IsBackward,>,0.5']
                                                   , OutputSelection     = 'RZVeloBW'
                                                   , RequirePositiveInputs = False
                                                   ) ]
                     , MeasureTime = True
                     , ModeOR = True
                     , ShortCircuit = False
                     ) )
        lumiRecoSequence.Members.append(lumiRecoFilterSequence)
        
        # sequence to get TT tracks
        from HltLine.HltDecodeRaw import DecodeTT
        from Configurables import TTGenericTracking
        from Configurables import RawBankToSTClusterAlg
        lumiTTTSequence = Sequence( 'LumiTTTSequence', Members = [] ) # reset, always build the same seq...
        lumiTTTSequence.Members.append( recoScaler )
        lumiTTTSequence.Members.append(
            Sequence('TTTSequence'
                     # , Members  = DecodeTT.members() + [
                     , Members  = [ RawBankToSTClusterAlg("createTTClusters"),
                                    TTGenericTracking("lumiTTT"
                                                     , MaxNumClusters = 2000
                                                     # , InputData = DecodeTT.members()[0].getProp('clusterLocation') // hardwired to use STClusters, not lite ones...
                                                     , OutputLocation = 'Hlt/Track/TTIP'
                                                     , WindowCenter = [0,0,0]
                                                     , HalfWindowXSize = 15
                                                     , HalfWindowYSize = 15
                                                     , OutputLevel = WARNING
                                                      )
                                    ]
                     , MeasureTime = True
                     , ModeOR = True
                     , ShortCircuit = False
                     ) )
        lumiRecoSequence.Members.append(lumiTTTSequence)

        # define histogrammers
        #from Configurables import LumiHistoMaker, LumiHisto2dSPD
        #HistoMembers=[]
        #HistoMembers.append(LumiHistoMaker('Histo'+postfix+BXType,
        #                                   InputVariables = createdCounters,
        #                                   Thresholds = histoThresholds,
        #                                   MaxBins = histoMaxBins,
        #                                   OutputLevel = debugOPL
        #                                   ))
        #HistoMembers.append(LumiHisto2dSPD('Histo2D'+postfix+BXType,
        #                                   HistoTitle=str(postfix+BXType),
        #                                   OutputLevel = debugOPL ))

        #lumiHistoSequence = Sequence('LumiHisto'+postfix+BXType +'Seq'
        #                             , Members = HistoMembers
        #                             , ModeOR = True
        #                             , ShortCircuit = False
        #                             , MeasureTime = True
        #                             , OutputLevel = debugOPL
        #                             )

        #return [ lumiRecoSequence, lumiCountSequence, lumiHistoSequence ] 
        return [ lumiRecoSequence, lumiCountSequence ] 


    def __create_lumi_line__(self, BXType):
        '''
        returns an Hlt1 "Line" including input filter, reconstruction sequence and counting
        adds histogramming
        '''
        from HltLine.HltLine import Hlt1Line   as Line
        return Line ( 'Lumi'+BXType
                    , prescale = self.prescale
                    , ODIN = ' ( ODIN_TRGTYP == LHCb.ODIN.%s ) & ( ODIN_BXTYP == LHCb.ODIN.%s)' % ( self.getProp('TriggerType'), BXType )
                    , algos = self.__create_lumi_algos__( BXType, '' )
                    , postscale = self.postscale
                    ) 

    def __create_lumi_low_line__(self, BXType):
        '''
        returns an Hlt1 "Line" including input filter, reconstruction sequence and counting
        adds histogramming
        '''
        postfix = 'Low'
        from HltLine.HltLine import Hlt1Line   as Line
        l0du = ' | '.join( [ (" ( L0_CHANNEL('%s') ) "%(x)) for x in  self.getProp('L0Channel') ] )
        return Line ( 'Lumi'+postfix+BXType
                    , prescale = self.prescale
                    , ODIN = ' ( ODIN_TRGTYP <= LHCb.ODIN.%s ) & ( ODIN_BXTYP == LHCb.ODIN.%s)' % ( self.getProp('TriggerType'), BXType )
                    , L0DU  = l0du
                    , algos = self.__create_lumi_algos__( BXType, postfix )
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        from HltLine.HltLine import Hlt1Line   as Line
        # LumiTrigger lines
        map( self.__create_lumi_line__, self.getProp('BXTypes') )
        # PhysicsTrigger lines
        # map( self.__create_lumi_low_line__, self.getProp('BXTypes') )


        
