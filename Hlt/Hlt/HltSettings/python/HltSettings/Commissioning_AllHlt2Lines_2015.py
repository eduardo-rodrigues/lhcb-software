from Commissioning_Physics_2015 import Commissioning_Physics_2015

class Commissioning_AllHlt2Lines_2015(Commissioning_Physics_2015):
    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError( 'Must update HltType when modifying ActiveHlt.Lines()' )

    def L0TCK(self) :
        return '0x0046'

    def HltType(self) :
        self.verifyType( Commissioning_AllHlt2Lines_2015 )
        return          'Commissioning_AllHlt2Lines_2015'

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        allLines =  ['Hlt2KS0_LLTurbo', 'Hlt2PassThrough', 'Hlt2KS0_DDTurbo', 'Hlt2Transparent', 'Hlt2Lumi', 'Hlt2Forward', 'Hlt2ErrorEvent', 'Hlt2DebugEvent', 'Hlt2JPsiReFitPVsTurbo', 'Hlt2CreateReco', 'Hlt2RareStrangeKPiMuMu', 'Hlt2RareStrangeSigmaPMuMu', 'Hlt2CharmHadSpec_D0KsTurbo', 'Hlt2CharmHadSpec_D0LambdaTurbo', 'Hlt2CharmHadD02DCSKPiPiPiTagTurbo', 'Hlt2CharmHadDstD02KshPiK_LL', 'Hlt2CharmHadDsp2KmKpPimPipPipTurbo', 'Hlt2CharmHadD02PiPiPiPiTagTurbo', 'Hlt2CharmHadD02KS0KPi_LLTurbo', 'Hlt2CharmHadDs2KKPi_XSecTurbo', 'Hlt2CharmHadSpec_DstLambdaTurbo', 'Hlt2CharmHadDspToKmKpPipTurbo', 'Hlt2CharmHadSpec_LcLambdaTurbo', 'Hlt2CharmHadDstD02KshPiK_DD', 'Hlt2CharmHadD2KPiPiKs_KsDDTurbo', 'Hlt2CharmHadD02KS0KK_DDTurbo', 'Hlt2CharmHadDstD02KshPiPi_LL', 'Hlt2CharmHadD2EtaPrimePi_3h', 'Hlt2CharmHadD02KPiPiPiWideTurbo', 'Hlt2CharmHadSpec_LcPrTurbo', 'Hlt2CharmHadD2PiPiPiKs_KsLLTurbo', 'Hlt2CharmHadLcpToPimPpPipTurbo', 'Hlt2CharmHadD2KPi0_eegamma', 'Hlt2CharmHadD2KS0Pi_DDTurbo', 'Hlt2CharmHadDstD02PiPiPi0R', 'Hlt2CharmHadLcpToKmPpKpTurbo', 'Hlt2CharmHadXic0ToPpKmKmPipTurbo', 'Hlt2CharmHadD02KPiTurbo', 'Hlt2CharmHadD02KKKPiTurbo', 'Hlt2CharmHadSpec_DpLambdaTurbo', 'Hlt2CharmHadD02DCSKPiPiPiWideTagTurbo', 'Hlt2CharmHadDp2KmKpPimPipPipTurbo', 'Hlt2CharmHadSpec_DpPiTurbo', 'Hlt2CharmHadDst_2D0Pi_D02KPi_XSecTurbo', 'Hlt2CharmHadD02KKPiPiWideTagTurbo', 'Hlt2CharmHadDp2KKPiKs_KsLLTurbo', 'Hlt2CharmHadD2KPiPi_SS_LTUNBTurbo', 'Hlt2CharmHadLc2LambdaPi_DDTurbo', 'Hlt2CharmHadXicpToKmPpPip', 'Hlt2CharmHadDst_2D0Pi_D02KKTurbo', 'Hlt2CharmHadInclDst2PiD02HHXBDT', 'Hlt2CharmHadDstD02KKPi0R', 'Hlt2CharmHadD2KKPiPi0', 'Hlt2CharmHadDst_2DsGamma_Ds2KKPi_XSecTurbo', 'Hlt2CharmHadD02KPi_XSecTurbo', 'Hlt2CharmHadSpec_DstPrTurbo', 'Hlt2CharmHadDs2PiPiPi_XSecTurbo', 'Hlt2CharmHadD02KKPiPiTurbo', 'Hlt2CharmHadDs2KKPiKs_KsLLTurbo', 'Hlt2CharmHadD02KKKPiStarting', 'Hlt2CharmHadDstD02KKPi0M', 'Hlt2CharmHadDstar2PiD0_eegamma', 'Hlt2CharmHadD02KS0KPi_DDTurbo', 'Hlt2CharmHadD02PiPiPiPiWideTurbo', 'Hlt2CharmHadDs2PiPiPiKs_KsLLTurbo', 'Hlt2CharmHadDp2PiPiPiKs_KsLLTurbo', 'Hlt2CharmHadSpec_D0PrTurbo', 'Hlt2CharmHadBachelorPr', 'Hlt2CharmHadDstD02KshKK_DD', 'Hlt2CharmHadSpec_LcPiTurbo', 'Hlt2CharmHadSpec_LcKsTurbo', 'Hlt2CharmHadD02PiPiPiPiTurbo', 'Hlt2CharmHadD02CFKKKPiTagTurbo', 'Hlt2CharmHadD02KPiPiPiForSpectroscopy', 'Hlt2CharmHadSpec_DpEtaTurbo', 'Hlt2CharmHadBachelorPi', 'Hlt2CharmHadSpec_DsPrTurbo', 'Hlt2CharmHadD02KS0KK_LLTurbo', 'Hlt2CharmHadDspToKpKpPimTurbo', 'Hlt2CharmHadDpm2KPiPi_ForKPiAsym', 'Hlt2CharmHadDstar2PiD0_ee', 'Hlt2CharmHadDpToKmKpKpTurbo', 'Hlt2CharmHadDp2KPiPiKs_KsLLTurbo', 'Hlt2CharmHadD2KKPiKs_KsDDTurbo', 'Hlt2CharmHadD02DCSKKKPiTagTurbo', 'Hlt2CharmHadD2EtaK_3hM', 'Hlt2CharmHadD2KS0KS0_2DDTurbo', 'Hlt2CharmHadD2EtaPi_3hM', 'Hlt2CharmHadDs2KPiPiKs_KsDDTurbo', 'Hlt2CharmHadSpec_D0PiTurbo', 'Hlt2CharmHadDspToPimPipPipTurbo', 'Hlt2CharmHadDp2PiPiPiKs_KsDDTurbo', 'Hlt2CharmHadD2EtaPi_3hR', 'Hlt2CharmHadD02PiPiPiPiWideTagTurbo', 'Hlt2CharmHadD2EtaK_3hR', 'Hlt2CharmHadSpec_LcKpmTurbo', 'Hlt2CharmHadD2KS0Pi_LLTurbo', 'Hlt2CharmHadSpec_DstPi0Turbo', 'Hlt2CharmHadSpec_DsKpmTurbo', 'Hlt2CharmHadD02CFKPiPiPiWideTagTurbo', 'Hlt2CharmHadDstD02KshKK_LL', 'Hlt2CharmHadSpec_DstKpmTurbo', 'Hlt2CharmHadD02DCSKKKPiWideTagTurbo', 'Hlt2CharmHadD02CFKKKPiWideTagTurbo', 'Hlt2CharmHadD02KKPiPiStarting', 'Hlt2CharmHadLc2KPK_XSecTurbo', 'Hlt2CharmHadOmega2LambdaK_DDL', 'Hlt2CharmHadLcpToKmPpPip_LTUNBTurbo', 'Hlt2CharmHadDspToKpPimPipTurbo', 'Hlt2CharmHadDst_2D0Pi0_D02KPi_XSecTurbo', 'Hlt2CharmHadD02KKPiPiTagTurbo', 'Hlt2CharmHadDp2KPiPiKs_KsDDTurbo', 'Hlt2CharmHadDp2KmKmKpPipPipTurbo', 'Hlt2CharmHadLcpToKmPpPipTurbo', 'Hlt2CharmHadD2PiPi0_eegamma', 'Hlt2CharmHadDstD02PiPiPi0M', 'Hlt2CharmHadSpec_DsLambdaTurbo', 'Hlt2CharmHadSpec_DsPi0Turbo', 'Hlt2CharmHadD2KKKPi0', 'Hlt2CharmHadDstD02KshKPi_LL', 'Hlt2CharmHadDst_2D0Pi_D02KPiCFTurbo', 'Hlt2CharmHadD02KKKPiWideTurbo', 'Hlt2CharmHadDs2KPiPiKs_KsLLTurbo', 'Hlt2CharmHadSpec_LcEtaTurbo', 'Hlt2CharmHadLc2LambdaPi_LLTurbo', 'Hlt2CharmHadD02PiPiPiPiStarting', 'Hlt2CharmHadD2KPiPiPi0', 'Hlt2CharmHadSigmac_2LcPi_XSecTurbo', 'Hlt2CharmHadD2KPiPiKs_KsLLTurbo', 'Hlt2CharmHadDpToKmPipPipTurbo', 'Hlt2CharmHadD02KPiPiPiTurbo', 'Hlt2CharmHadDpm2KKPi_XSecTurbo', 'Hlt2CharmHadSpec_D0Pi0Turbo', 'Hlt2CharmHadDstD02KshKPi_DD', 'Hlt2CharmHadSpec_DpKsTurbo', 'Hlt2CharmHadDst_2D0Pi_D02KK_LTUNBTurbo', 'Hlt2CharmHadSpec_D0KpmTurbo', 'Hlt2CharmHadDst_2D0Pi_D02PiPiTurbo', 'Hlt2CharmHadD2EtaPrimeK_3h', 'Hlt2CharmHadDp2KKPiKs_KsDDTurbo', 'Hlt2CharmHadD2KS0K_LLTurbo', 'Hlt2CharmHadDstD02KshPiPi_DD', 'Hlt2CharmHadDp2KmPimPipPipPipTurbo', 'Hlt2CharmHadDst_2D0Pi_D02KPiDCS_LTUNBTurbo', 'Hlt2CharmHadSpec_D0EtaTurbo', 'Hlt2CharmHadD2EtaPi_hhgamma', 'Hlt2CharmHadLcpForSpectroscopy', 'Hlt2CharmHadD2KS0K_DDTurbo', 'Hlt2CharmHadD2PiEta_eegamma', 'Hlt2CharmHadD2KKPi_OS_LTUNBTurbo', 'Hlt2CharmHadSpec_LcPi0Turbo', 'Hlt2CharmHadD2EtaPrimePi_hhgamma', 'Hlt2CharmHadD02KS0PiPi_DDTurbo', 'Hlt2CharmHadD2KEta_eegamma', 'Hlt2CharmHadSpec_DsKsTurbo', 'Hlt2CharmHadDst_2D0Pi_D02KPiDCSTurbo', 'Hlt2CharmHadLc2LambdaK_LLTurbo', 'Hlt2CharmHadD02KKTurbo', 'Hlt2CharmHadD02KS0PiPi_LLTurbo', 'Hlt2CharmHadSpec_DsEtaTurbo', 'Hlt2CharmHadD02KPiPiPiStarting', 'Hlt2CharmHadSpec_DstPiTurbo', 'Hlt2CharmHadLcpToPimPpKpTurbo', 'Hlt2CharmHadDsp2KmKmKpPipPipTurbo', 'Hlt2CharmHadSpec_DstKsTurbo', 'Hlt2CharmHadD2EtaPrimeK_hhgamma', 'Hlt2CharmHadDst_2D0Pi_D02K3Pi_XSecTurbo', 'Hlt2CharmHadSpec_DstEtaTurbo', 'Hlt2CharmHadD2PiPiPiKs_KsDDTurbo', 'Hlt2CharmHadLc2KPPi_XSecTurbo', 'Hlt2CharmHadXi2LambdaPi_LLL', 'Hlt2CharmHadD02CFKPiPiPiTagTurbo', 'Hlt2CharmHadD02PiPiTurbo', 'Hlt2CharmHadD2KS0KS0_2LLTurbo', 'Hlt2CharmHadD02KKPiPiWideTurbo', 'Hlt2CharmHadD2KKPiKs_KsLLTurbo', 'Hlt2CharmHadPentaPhiPimPpTurbo', 'Hlt2CharmHadXi2LambdaPi_DDL', 'Hlt2CharmHadDst_2D0Pi_D02PiPi_LTUNBTurbo', 'Hlt2CharmHadDst_2D0Pi_D02KPiCF_LTUNBTurbo', 'Hlt2CharmHadDpToKpPimPipTurbo', 'Hlt2CharmHadDpToKmKpPipTurbo', 'Hlt2CharmHadSpec_DpKpmTurbo', 'Hlt2CharmHadXic0ToPpKmKmPip_LTUNBTurbo', 'Hlt2CharmHadLc2LambdaK_DDTurbo', 'Hlt2CharmHadInclSigc2PiLc2HHXBDT', 'Hlt2CharmHadSpec_DpPi0Turbo', 'Hlt2CharmHadDpm2KPiPi_XSecTurbo', 'Hlt2CharmHadSpec_DpPrTurbo', 'Hlt2CharmHadDstD02KPiPi0R', 'Hlt2CharmHadDst_2D0Gamma_D02KPi_XSecTurbo', 'Hlt2CharmHadDpToPimPipPipTurbo', 'Hlt2CharmHadBachelorK', 'Hlt2CharmHadDstD02KPiPi0M', 'Hlt2CharmHadLc2PiPPi_XSecTurbo', 'Hlt2CharmHadDs2KKPiKs_KsDDTurbo', 'Hlt2CharmHadDspToKmKpKpTurbo', 'Hlt2CharmHadD2PiPiPiPi0', 'Hlt2CharmHadD2KS0KS0_LLDDTurbo', 'Hlt2CharmHadDs2PiPiPiKs_KsDDTurbo', 'Hlt2CharmHadSpec_DsPiTurbo', 'Hlt2CharmHadDsp2KmPimPipPipPipTurbo', 'Hlt2CharmHadOmega2LambdaK_LLL', 'Hlt2CharmHadD2EtaK_hhgamma', 'Hlt2Topo4Body', 'Hlt2Run1TopoE2BodyBBDT', 'Hlt2Run1TopoE4BodyBBDT', 'Hlt2Run1TopoMu3BodyBBDT', 'Hlt2Topo2Body', 'Hlt2Run1Topo4BodySimple', 'Hlt2Run1TopoMu2BodyBBDT', 'Hlt2Run1Topo3BodyBBDT', 'Hlt2Run1Topo4BodyBBDT', 'Hlt2Run1Topo2BodySimple', 'Hlt2TopoMu2Body', 'Hlt2Topo3Body', 'Hlt2Run1TopoMu4BodyBBDT', 'Hlt2Run1TopoE3BodyBBDT', 'Hlt2Run1TopoRad2BodyBBDT', 'Hlt2TopoMu3Body', 'Hlt2TopoMu4Body', 'Hlt2Run1Topo3BodySimple', 'Hlt2Run1Topo2BodyBBDT', 'Hlt2Run1TopoRad2plus1BodyBBDT', 'Hlt2SingleMuon', 'Hlt2SingleMuonRare', 'Hlt2SingleMuonVHighPT', 'Hlt2SingleMuonHighPT', 'Hlt2SingleMuonLowPT', 'Hlt2TrackEff_D0ToKpiPionProbe', 'Hlt2TrackEff_D0ToKpiKaonProbe', 'Hlt2DiMuonPsi2S', 'Hlt2DiMuonB', 'Hlt2DiMuonJPsi', 'Hlt2DiMuonJPsiHighPT', 'Hlt2DiMuon', 'Hlt2DiMuonDetachedHeavy', 'Hlt2DiMuonPsi2SHighPT', 'Hlt2DiMuonDetachedJPsi', 'Hlt2DiMuonDetached', 'Hlt2DiMuonZ', 'Hlt2DiMuonSoft', 'Hlt2DiMuonDetachedPsi2S', 'Hlt2B2PiPi', 'Hlt2Lb2PPi', 'Hlt2B2HH', 'Hlt2B2KPi', 'Hlt2B2KK', 'Hlt2Lb2PK', 'Hlt2Bc2JpsiH', 'Hlt2TFBc2JpsiMuX', 'Hlt2RadiativeB2GammaGammaLL', 'Hlt2RadiativeIncHHHGammaEE', 'Hlt2RadiativeB2GammaGamma', 'Hlt2RadiativeB2GammaGammaDouble', 'Hlt2RadiativeIncHHGamma', 'Hlt2RadiativeIncHHHGamma', 'Hlt2RadiativeBd2KstGammaULUnbiased', 'Hlt2RadiativeBd2KstGamma', 'Hlt2RadiativeLb2L0GammaEELL', 'Hlt2RadiativeB2GammaGammaDD', 'Hlt2RadiativeLb2L0GammaLL', 'Hlt2RadiativeBs2PhiGamma', 'Hlt2RadiativeBs2PhiGammaUnbiased', 'Hlt2RadiativeIncHHGammaEE', 'Hlt2DiMuonJPsiTurbo', 'Hlt2DiMuonPsi2STurbo', 'Hlt2DiMuonBTurbo', 'Hlt2RadiativeB2GammaGammaTurbo', 'Hlt2B2K0pi0', 'Hlt2B2Kpi0', 'Hlt2DPS:2xHc', 'Hlt2DPS:2x2mu', 'Hlt2DPS:2mu&Hc', 'Hlt2TriMuonTau23Mu', 'Hlt2TriMuonDetached', 'Hlt2RareCharmD02PiPieeFilter', 'Hlt2RareCharmD2PiMuMuSSFilter', 'Hlt2RareCharmD02KPiMueFilter', 'Hlt2RareCharmD02PiPiMueFilter', 'Hlt2RareCharmLc2PeeFilter', 'Hlt2RareCharmD02KPieeFilter', 'Hlt2RareCharmD02KMu', 'Hlt2RareCharmD2KMuMuSSFilter', 'Hlt2RareCharmD02EMu', 'Hlt2RareCharmD2KMuMuFilter', 'Hlt2RareCharmD2KMueFilter', 'Hlt2RareCharmD2KeeFilter', 'Hlt2RareCharmD02PiPi', 'Hlt2RareCharmD2PiMueFilter', 'Hlt2RareCharmD02KPi', 'Hlt2RareCharmD02PiPiMuMuFilter', 'Hlt2RareCharmD02KKMuMuFilter', 'Hlt2RareCharmD02KPiMuMuSSFilter', 'Hlt2RareCharmD02KKMueFilter', 'Hlt2RareCharmLc2PMuMuFilter', 'Hlt2RareCharmD2PiMuMuFilter', 'Hlt2RareCharmLc2PMuMuSSFilter', 'Hlt2RareCharmLc2PMueFilter', 'Hlt2RareCharmD02MuMu', 'Hlt2RareCharmD02KPiMuMuFilter', 'Hlt2RareCharmD02KKeeFilter', 'Hlt2RareCharmD2PieeFilter', 'Hlt2EWDiMuonZ', 'Hlt2EWDiMuonDY2', 'Hlt2EWSingleElectronHighPt', 'Hlt2EWSingleElectronLowPt', 'Hlt2EWSingleMuonLowPt', 'Hlt2EWDiElectronHighMass', 'Hlt2EWSingleElectron', 'Hlt2EWSingleMuonHighPt', 'Hlt2EWDiMuonDY1', 'Hlt2EWSingleMuonVHighPt', 'Hlt2EWDiMuonDY4', 'Hlt2EWSingleTauHighPt2Prong', 'Hlt2EWSingleElectronVHighPt', 'Hlt2EWDiMuonDY3', 'Hlt2EWSingleTauHighPt3Prong', 'Hlt2EWDiElectronDY', 'Hlt2DisplVerticesDouble', 'Hlt2DisplVerticesSingleVeryHighFD', 'Hlt2DisplVerticesDoublePS', 'Hlt2DisplVerticesSingle', 'Hlt2DisplVerticesSingleLoosePS', 'Hlt2DisplVerticesSingleHighFD', 'Hlt2DisplVerticesSingleHighMass', 'Hlt2DisplVerticesSinglePS', 'Hlt2Phi2KsKsD0Ctrl', 'Hlt2Bs2PhiPhi', 'Hlt2IncPhi', 'Hlt2Phi2KsKs', 'Hlt2DiPhi', 'Hlt2DiProtonLowMult', 'Hlt2DiProton', 'Hlt2LowMultDiMuon_PS', 'Hlt2LowMultDiMuon', 'Hlt2LowMultMuon', 'Hlt2LowMultDiElectron_noTrFilt', 'Hlt2LowMultDiElectron', 'Hlt2LowMultDiPhoton', 'Hlt2LowMultPi0', 'Hlt2LowMultDiPhoton_HighMass', 'Hlt2LowMultChiC2HHHHWS', 'Hlt2LowMultD2KPi', 'Hlt2LowMultChiC2HHWS', 'Hlt2LowMultLMR2HH_mediumPS', 'Hlt2LowMultChiC2HHHH', 'Hlt2LowMultLMR2HHWS', 'Hlt2LowMultLMR2HHWS_mediumPS', 'Hlt2LowMultLMR2HHWS_heavyPS', 'Hlt2LowMultD2K3PiWS', 'Hlt2LowMultLMR2HH', 'Hlt2LowMultHadron_noTrFilt', 'Hlt2LowMultD2KKPiWS', 'Hlt2LowMultChiC2PP', 'Hlt2LowMultLMR2HHHHWS_heavyPS', 'Hlt2LowMultD2KPiPi', 'Hlt2LowMultChiC2HH', 'Hlt2LowMultD2KKPi', 'Hlt2LowMultLMR2HHHH_heavyPS', 'Hlt2LowMultL2pPiWS', 'Hlt2LowMultLMR2HHHHWS_mediumPS', 'Hlt2LowMultL2pPi', 'Hlt2LowMultChiC2PPWS', 'Hlt2LowMultD2K3Pi', 'Hlt2LowMultLMR2HHHH', 'Hlt2LowMultD2KPiPiWS', 'Hlt2LowMultLMR2HHHHWS', 'Hlt2LowMultLMR2HH_heavyPS', 'Hlt2LowMultD2KPiWS', 'Hlt2LowMultLMR2HHHH_mediumPS', 'Hlt2NonBeamBeamNoBias', 'Hlt2LowMultTechnical_MinBias', 'Hlt2PIDDetJPsiMuMuPosTaggedTurboCalib', 'Hlt2PIDB2KJPsiMuMuSSTaggedTurboCalib', 'Hlt2PIDDs2PiPhiMuMuOSTaggedTurboCalib', 'Hlt2PIDDetPhiMuMuPosTaggedTurboCalib', 'Hlt2PIDB2KJPsiMuMuOSTaggedTurboCalib', 'Hlt2PIDDetPhiMuMuNegTaggedTurboCalib', 'Hlt2PIDDs2PiPhiMuMuSSTaggedTurboCalib', 'Hlt2PIDDetJPsiMuMuNegTaggedTurboCalib', 'Hlt2PIDB2KJPsiEEOSTaggedTurboCalib', 'Hlt2PIDB2KJPsiEESSTaggedTurboCalib', 'Hlt2PIDDetJPsiEEPosTaggedTurboCalib', 'Hlt2PIDDetJPsiEENegTaggedTurboCalib', 'Hlt2PIDLb2LcPiTurboCalib', 'Hlt2PIDScpp2LcPiTurboCalib', 'Hlt2PIDSc02LcPiTurboCalib', 'Hlt2PIDLambda2PPiDDTurboCalib', 'Hlt2PIDLambda2PPiDDveryhighPTTurboCalib', 'Hlt2PIDLambda2PPiDDisMuonTurboCalib', 'Hlt2PIDLambda2PPiLLhighPTTurboCalib', 'Hlt2PIDKs2PiPiLLTurboCalib', 'Hlt2PIDD02KPiPiPiTagTurboCalib', 'Hlt2PIDD02KPiTagTurboCalib', 'Hlt2PIDLambda2PPiDDhighPTTurboCalib', 'Hlt2PIDLambda2PPiLLTurboCalib', 'Hlt2PIDLb2LcMuNuTurboCalib', 'Hlt2PIDLambda2PPiLLisMuonTurboCalib', 'Hlt2PIDD02KPiPiPiTurboCalib', 'Hlt2PIDLambda2PPiLLveryhighPTTurboCalib', 'Hlt2PIDKs2PiPiDDTurboCalib', 'Hlt2PIDB2KJPsiEEL0SSTaggedTurboCalib', 'Hlt2PIDDetJPsiEEL0PosTaggedTurboCalib', 'Hlt2PIDB2KJPsiEEL0OSTaggedTurboCalib', 'Hlt2PIDDetJPsiEEL0NegTaggedTurboCalib', 'Hlt2TrackEffDiMuonVeloMuonPlusMatchedTurboCalib', 'Hlt2TrackEffDiMuonDownstreamMinusTaggedTurboCalib', 'Hlt2TrackEffDiMuonMuonTTPlusMatchedTurboCalib', 'Hlt2TrackEffDiMuonMuonTTMinusTaggedTurboCalib', 'Hlt2TrackEffDiMuonMuonTTPlusTaggedTurboCalib', 'Hlt2TrackEffDiMuonDownstreamMinusMatchedTurboCalib', 'Hlt2TrackEffDiMuonVeloMuonMinusTaggedTurboCalib', 'Hlt2TrackEffDiMuonDownstreamPlusMatchedTurboCalib', 'Hlt2TrackEffDiMuonVeloMuonPlusTaggedTurboCalib', 'Hlt2TrackEffDiMuonDownstreamPlusTaggedTurboCalib', 'Hlt2TrackEffDiMuonVeloMuonMinusMatchedTurboCalib', 'Hlt2TrackEffDiMuonMuonTTMinusMatchedTurboCalib']




        exclude = ['Hlt2KS0_LLTurbo',
                   'Hlt2KS0_DDTurbo']
        for line in allLines:
            if "Run1Topo" in line:
                exclude.append(line)
            if "XSec" in line:
                exclude.append(line)

        technical = ["Hlt2ErrorEvent",
                     "Hlt2NonBeamBeamNoBias",
                     "Hlt2CreateReco",
                     "Hlt2DebugEvent",
                     "Hlt2Transparent",
                     "Hlt2PassThrough",
                     "Hlt2Lumi",
                     "Hlt2Forward"]
        
        

        
        allLines = list(set(allLines) - set(exclude) - set(technical))
        print "###################################"
        print "We have  %i Hlt2 lines." % len(allLines)
        print "Excluding %i lines" % len(exclude)
        print "###################################"
             
        return allLines
    
