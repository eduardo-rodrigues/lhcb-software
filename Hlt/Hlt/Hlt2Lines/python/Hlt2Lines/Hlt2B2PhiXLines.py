# $Id: Hlt2B2PhiXLines.py,v 1.3 2009-08-13 20:53:17 graven Exp $

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2PhiXLinesConf(HltLinesConfigurableUser) :
    '''
    Selection for Bs -> Phi Phi

    @author KIM Young Min  <s0679231@sms.ed.ac.uk>
    @date 29-07-2009
    '''

    __slots__ = { 'KaonPtCut'               : 500 #MeV
                 ,'PhiMassWindow'           : 20  #MeV 
                 ,'BsMassWindow'            : 500 #MeV
                 ,'BsDaughtersPtProductCut' : 6000000 #MeV^2
                 ,'BsDIRACut'               : 0.999
                 ,'BsVertexChi2Cut'         : 50
                 ,'Prescale'                : {'Hlt2Bs2PhiPhi' : 0.1 }
                 ,'Postscale'               : {'Hlt2Bs2PhiPhi' : 1.  }
                 }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiPhiDecision" : 55375 } )

        # Make the phi
        phiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , InputLocations = [NoCutsKaons] #[Hlt2GoodKaons]
                                 , DaughtersCuts = { "K+" : "(PT>%(KaonPtCut)s)"% self.getProps() }
                                 , CombinationCut = "(ADAMASS('phi(1020)')<%(PhiMassWindow)s*MeV)"% self.getProps()
                                 , MotherCut = "(ALL)" #"(M<1100*MeV) & (PT>500*MeV)"
                                 )
								 
        # Make the Bs
        BsCombine = Hlt2Member(  CombineParticles
                                 , "BsCombine"
                                 , DecayDescriptor = "B_s0 ->  phi(1020) phi(1020)"
                                 , InputLocations  = [phiCombine]
                                 , CombinationCut = "(ADAMASS('B_s0')<%(BsMassWindow)s*MeV) & (ACHILD(PT,1)*ACHILD(PT,2)>%(BsDaughtersPtProductCut)s) "% self.getProps()
                                 , MotherCut = "(BPVDIRA>%(BsDIRACut)s) & (VFASPF(VCHI2)<%(BsVertexChi2Cut)s)"%self.getProps()
                                 )
       
        line = Hlt2Line('Bs2PhiPhi'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [NoCutsKaons, phiCombine, BsCombine]
                        , PV = True
                        )
