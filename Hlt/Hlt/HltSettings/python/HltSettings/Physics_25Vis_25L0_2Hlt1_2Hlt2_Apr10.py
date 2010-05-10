from Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Apr10 import Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Apr10 
class Physics_25Vis_25L0_2Hlt1_2Hlt2_Apr10 ( Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Apr10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds' )

    def HltType(self) :
        self.verifyType( Physics_25Vis_25L0_2Hlt1_2Hlt2_Apr10 )
        return          'Physics_25Vis_25L0_2Hlt1_2Hlt2_Apr10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        thresholds = Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Apr10.Thresholds(self)

        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        if Hlt1MBLinesConf not in thresholds : thresholds[Hlt1MBLinesConf] = dict()
        thresholds[Hlt1MBLinesConf].update( { 'Postscale': { 'Hlt1MBMicroBias.*(?<!RateLimited)$' : 0.001        # must veto 'RateLimited' at the end...
                                                           , 'Hlt1MBMicroBias.*RateLimited$'      : 'RATE(500)'  # must have 'RateLimited' at the end...
                                                           }
                                            } 
                                          )
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        if Hlt1L0LinesConf not in thresholds : thresholds[Hlt1L0LinesConf] = dict()
        thresholds[Hlt1L0LinesConf].update( { 'Postscale' : { 'Hlt1L0Any$'           : 0.001
                                                            , 'Hlt1L0AnyRateLimited' : 'RATE(100)' 
                                                            }
                                            }
                                          )
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        if Hlt1LumiLinesConf not in thresholds : thresholds[Hlt1LumiLinesConf] = dict()
        thresholds[Hlt1LumiLinesConf].update( { 'Prescale'  : { 'Hlt1LumiLow.*(?<!RateLimited)$'    : 0.004 
                                                              , 'Hlt1LumiMid.*(?<!RateLimited)$'    : 0.04
                                                              }
                                              , 'Postscale' : { 'Hlt1LumiLowNoBeamRateLimited'      : 'RATE(5)' 
                                                              , 'Hlt1LumiLowBeam2RateLimited'       : 'RATE(10)' 
                                                              , 'Hlt1LumiLowBeam1RateLimited'       : 'RATE(15)' 
                                                              , 'Hlt1LumiLowBeamCrossingRateLimited': 'RATE(70)' 
                                                              }
                                              }
                                            )
        from Hlt1Lines.Hlt1BeamGasLines         import Hlt1BeamGasLinesConf
        if Hlt1BeamGasLinesConf not in thresholds : thresholds[Hlt1BeamGasLinesConf] = dict()
        thresholds[Hlt1BeamGasLinesConf].update( { 'ForcedInputRateLimit'  : 10000 } )

        # Hlt2
        from DefaultHlt2Lines import DefaultHlt2Lines         
        thresholds.update( DefaultHlt2Lines().Thresholds() )
        
        return thresholds
    
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Apr10.ActiveHlt1Lines(self)
        lines += [ i + 'RateLimited' for i in lines 
                                     if i.startswith('Hlt1MBMicroBias') 
                                     #or i.startswith('Hlt1LumiLow') # do not use rate limited lumi lines to avoid confusing the lumi bookkeeping
                                     or i.startswith('Hlt1L0Any') ]
        return lines

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        # return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent']
        list = Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Apr10.ActiveHlt2Lines(self)
        if 'Hlt2Forward' not in list : list.extend('Hlt2Forward')   ## does what ?
        from DefaultHlt2Lines import DefaultHlt2Lines 
        list.extend( DefaultHlt2Lines().ActiveHlt2Lines() )
        return list
        
    
