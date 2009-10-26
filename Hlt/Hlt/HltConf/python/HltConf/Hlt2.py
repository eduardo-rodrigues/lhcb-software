"""
 script to configure HLT2 trigger lines

 @author P. Koppenburg
 @date 2009-07-08
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.37 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Hlt2Lines.Hlt2B2DXLines            import Hlt2B2DXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines         import Hlt2B2JpsiXLinesConf
from Hlt2Lines.Hlt2B2PhiXLines          import Hlt2B2PhiXLinesConf
from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveMuonLines   import Hlt2InclusiveMuonLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines    import Hlt2InclusivePhiLinesConf
from Hlt2Lines.Hlt2TopologicalLines     import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2B2XGammaLines        import Hlt2B2XGammaLinesConf
from Hlt2Lines.Hlt2B2HHLines            import Hlt2B2HHLinesConf
from Hlt2Lines.Hlt2B2LLXLines           import Hlt2B2LLXLinesConf
from Hlt2Lines.Hlt2DisplVerticesLines   import Hlt2DisplVerticesLinesConf
from Hlt2Lines.Hlt2CommissioningLines   import Hlt2CommissioningLinesConf

# Define what categories stand for
# There are the strings used in HltThresholdSettings


class Hlt2Conf(LHCbConfigurableUser):
    from Configurables import Hlt2PID
    from CaloReco.Configuration   import HltCaloRecoConf 
    from CaloPIDs.Configuration   import HltCaloPIDsConf
    __used_configurables__ = [ Hlt2PID, HltCaloRecoConf, HltCaloPIDsConf
                             , Hlt2TopologicalLinesConf
                             , Hlt2B2DXLinesConf 
                             , Hlt2InclusiveDiMuonLinesConf
                             , Hlt2InclusiveMuonLinesConf 
                             , Hlt2InclusivePhiLinesConf 
                             , Hlt2B2JpsiXLinesConf
                             , Hlt2B2PhiXLinesConf
                             , Hlt2XGammaLinesConf
                             , Hlt2B2HHLinesConf
                             # , Hlt2B2LLXLinesConf                         # DO NOT RUN UNTIL CALO IS FIXED
                             , Hlt2DisplVerticesLinesConf
                             , Hlt2CommissioningLinesConf 
                             ]
    __slots__ = {
           "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
         , "Hlt2Requires"               : 'L0+Hlt1'  # require L0 and Hlt1 pass before running Hlt2
         , "ThresholdSettings"          : {} # ThresholdSettings predefined by Configuration
         , "Hlt2Tracks"                 : "Hlt/Track/Long"
         , "WithMC"                     : False 
         }

###################################################################################
#
# Threshold settings
#
# This is copied and edited from Hlt1.confType().
# One could outsource that to some function, but who cares?
#
    def hlt2Lines(self,Hlt2):
        """
        The actual lines
        """
        from HltLine.HltLine     import Hlt2Line
        Hlt2Line( "Global", HLT= "HLT_PASS_SUBSTR('Hlt2') ", priority = 255, PV = False, Reco = False )
        Hlt2.Members += [ Sequence('Hlt2Lines',ModeOR=True,ShortCircuit=False) ]
        ThresholdSettings = self.getProp("ThresholdSettings")
        #
        # Loop over thresholds
        #
        from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
        for i in self.__used_configurables__ :
            if not issubclass(i,HltLinesConfigurableUser) : continue
            from ThresholdUtils import setThresholds
            setThresholds(ThresholdSettings,i)
                    
###################################################################################
#
# Reco
#
    def hlt2Reconstruction(self,Hlt2):
        """
        Reconstruction
        """
        #  Full reconstruction of all tracks 
        from HltLine.HltReco import HltRecoSequence
        HltRecoSequence.Members += [ self.hlt2Tracks() ] # charged

###################################################################################
#
# Charged Particle making
#
    def hlt2Tracks(self):
        """
        Charged particles
        """
        from Configurables import HltInsertTrackErrParam
        SeqHlt2Charged = Sequence('SeqHlt2Charged'
                                  , MeasureTime = True
                                  , IgnoreFilterPassed = False)        
        #
        # MC truth associated tracks
        #
        if (self.getProp('WithMC')):
              SeqHlt2Charged.Members += [ Sequence('SeqTrueSignalTracks') ] #  debug
        #
        # Hacking of errors
        #
        HltInsertTrackErrParam = HltInsertTrackErrParam()
        HltInsertTrackErrParam.InputLocation = self.getProp("Hlt2Tracks")
        SeqHlt2Charged.Members += [ HltInsertTrackErrParam ]
        return SeqHlt2Charged
       
###################################################################################
#
# Requirements
#
    def hlt2Requirements(self,Hlt2):
        """
        Requirements
        """
        Hlt2.Members += [ Sequence("Hlt2Requirements") ]
        Sequence("Hlt2Requirements").Members = [ ]
        reqs = self.getProp('Hlt2Requires')
        if reqs.upper() != "NONE" :
            from Configurables import LoKi__HDRFilter   as HltFilter
            from Configurables import LoKi__L0Filter    as L0Filter
            from HltLine.HltDecodeRaw import DecodeL0DU
            L0accept = Sequence(name='Hlt2L0Requirements', Members = DecodeL0DU.members() + [ L0Filter( 'L0Pass', Code = "L0_DECISION" )])
            
            hlt2requires = { 'L0'   : L0accept 
                           , 'Hlt1' : HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" )
                             }
            for i in reqs.split('+') :
                if i : Sequence("Hlt2Requirements").Members.append( hlt2requires[i] )

###################################################################################
#
# PID
#
    def configurePID(self):
        from Configurables import Hlt2PID
        Hlt2PID().DataType = self.getProp("DataType")
        Hlt2PID().Hlt2Tracks = self.getProp("Hlt2Tracks")
      
###################################################################################
#
# Calo, used much later
#
    def configureCalo(self):
        """
        Configure calo sequences
        """   
        Hlt2CaloSeq = GaudiSequencer("Hlt2CaloSeq")
        Hlt2CaloSeq.Context = "HLT"
        Hlt2CaloRecoSeq = GaudiSequencer("Hlt2CaloRecoSeq")
        Hlt2CaloRecoSeq.Context = "HLT"
        Hlt2CaloPIDsSeq = GaudiSequencer("Hlt2CaloPIDsSeq")
        Hlt2CaloPIDsSeq.Context = "HLT"
        Hlt2CaloSeq.Members = [ Hlt2CaloRecoSeq, Hlt2CaloPIDsSeq ]
        from CaloReco.Configuration   import HltCaloRecoConf 
        from CaloPIDs.Configuration   import HltCaloPIDsConf
        caloReco = HltCaloRecoConf(Sequence           = Hlt2CaloRecoSeq,
                                   Context            = "HLT")
        caloPID  = HltCaloPIDsConf(Sequence           = Hlt2CaloPIDsSeq,
                                   Context            = "HLT")
        
###################################################################################
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        Hlt2 configuration
        """
        Hlt2 = Sequence("Hlt2", Context = 'HLT' )
        if Hlt2 not in Sequence("Hlt").Members : Sequence("Hlt").Members += [ Hlt2 ]
        # requirements
        self.hlt2Requirements(Hlt2)        
        # reco
        self.hlt2Reconstruction(Hlt2)
        if self.getProp('WithMC'):
              importOptions( "$HLTCONFROOT/options/HltTrackAssociator.py" )
        # set Hlt2 PID
        self.configureCalo()
        # set Hlt2 PID
        self.configurePID()
        # lines
        self.hlt2Lines(Hlt2)
        
