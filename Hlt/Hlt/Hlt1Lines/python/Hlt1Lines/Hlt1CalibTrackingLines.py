# =============================================================================
## @@file
#  Configuration of CalibTracking Lines
#  Get two tracks near the D0 mass
#  @@author Matthew Kenzie matthew.william.kenzie@cern.ch
#  @@date 2012-07-28
# =============================================================================
"""
 script to configure CalibTracking trigger lines
"""
# =============================================================================
__author__  = "Matthew Kenzie matthew.william.kenzie@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 0.01 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt1CalibTrackingLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {  'TrackPT'           : 500     # MeV
                ,'TrackP'            : 3000    # MeV
                ,'TrackChi2DOF'      : 4       # dimensionless
                ,'ParticlePT'        : 500     # MeV
                ,'CombMaxDaughtPT'   : 800     # MeV
                ,'CombMaxDaughtP'    : 8000    # MeV
                ,'CombAPT'           : 1500    # MeV
                ,'CombDOCA'          : 0.2     # mm
                ,'CombVCHI2'         : 20      # dimensionless
                ,'CombVCHI2DOF'      : 4       # dimensionless
                ,'CombDIRA'          : 0.9     # dimensionless
                ,'CombTAU'           : 0.2     # ps
                ,'D0MassWinLoose'    : 150     # MeV
                ,'D0MassWin'         : 100     # MeV
                ,'B0MassWinLoose'    : 250     # MeV
                ,'B0MassWin'         : 200     # MeV
                ,'PhiMassWinLoose'   : 80      # MeV
                ,'PhiMassWin'        : 40      # MeV
                ,'PhiPT'             : 1800    # MeV
                ,'B0SUMPT'           : 4000    # MeV
                ,'GAMMA_PT_MIN'      : 3000    # MeV
                ,'Velo_Qcut'         : 3       # dimensionless
                ,'TrNTHits'          : 16.
                ,'ValidateTT'        : False
      }

  # Tracking stuff
  def hh_TrackingUnit( self, name, props ) :

    from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack )
    hh_Preambulo = [  TrackCandidates('CalibTracking'),
                      FitTrack ]

    hh_LineCode = """
    TrackCandidates
    >>  FitTrack
    >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) )
    >>  ( ( TrNVELOMISS < %(Velo_Qcut)s ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass Velo', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nVelo' , LoKi.Monitoring.ContextSvc ) )
    >>  ( (TrTNORMIDC > %(TrNTHits)s ) & ( TrPT > %(TrackPT)s * MeV ) & ( TrP  > %(TrackP)s  * MeV ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass Kaon P/PT', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nP' , LoKi.Monitoring.ContextSvc ) )
    >>  ( ( TrCHI2PDOF < %(TrackChi2DOF)s ) )
    >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackChi2', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nChi2' , LoKi.Monitoring.ContextSvc ) )
    >>  TC_TOPROTOPARTICLES( '' )
    >>  SINK( 'Hlt1CalibTrackingProtos' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_TrackUnit = HltUnit(
        'Hlt1'+name+'TrackUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = hh_Preambulo,
        Code = hh_LineCode
        )

    return hlt1CalibTrackingLine_TrackUnit

  # Kaon stuff
  def hh_KaonUnit( self, name, props ) :

    kaon_LineCode = """
    SELECTION( 'Hlt1CalibTrackingProtos' )
    >>  TC_TOPARTICLES( 'K-',  '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKaons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKaons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingKaons' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_KaonUnit = HltUnit(
        'Hlt1'+name+'KaonUnit',
        #OutputLevel = 1,
        Monitor = True,
        #Preambulo = [],
        Code = kaon_LineCode 
        )

    return hlt1CalibTrackingLine_KaonUnit

  def hh_PionUnit( self, name, props ) :
    
    pion_LineCode = """
    SELECTION( 'Hlt1CalibTrackingProtos' )
    >>  TC_TOPARTICLES( 'pi+', '', (PT>%(ParticlePT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPions', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPions',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingPions' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_PionUnit = HltUnit(
        'Hlt1'+name+'PionUnit',
        #OutputLevel = 1,
        Monitor = True,
        #Preambulo = [],
        Code = pion_LineCode
        )

    return hlt1CalibTrackingLine_PionUnit

  def KPi_Unit( self, name, props ) :
    
    props['KaonCuts']        = 'ALL' 
    props['PionCuts']        = 'ALL'
    props['KPiDecay']        = '\'[D0 -> K- pi+]cc\'' #'strings( [ \'[D0 -> K- pi+]cc\', \'D0 -> K+ K-\', \'D0 -> pi+ pi-\' ] )'
    props['KPiCombCut']      = ("( (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) " +
                                "| ( (DAMASS('B0')>-%(B0MassWinLoose)s*MeV) " +
                                "& (DAMASS('B_s0')<%(B0MassWinLoose)s*MeV) ) ) " +
                                "& (APT>%(CombAPT)s*MeV) " +
                                "& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) " +
                                "& (AMAXCHILD(P)>%(CombMaxDaughtP)s*MeV) " +
                                "& (ACUTDOCA(%(CombDOCA)s*mm,''))") %props
    props['KPiMothCut']      = ("( (ADMASS('D0')<%(D0MassWin)s*MeV) " +
                                "| ( (DMASS('B0')>-%(B0MassWin)s*MeV) " +
                                "& (DMASS('B_s0')<%(B0MassWin)s*MeV) ) ) " +
                                "& (BPVDIRA > %(CombDIRA)s) " +
                                "& (VFASPF(VCHI2)<%(CombVCHI2)s) " +
                                "& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s) " +
                                "& (BPVLTIME()>%(CombTAU)s*ps)") %props

    KPi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD",
                  "from LoKiPhys.decorators import PT",
                  "KPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KPiDecay)s, %(KPiCombCut)s, %(KPiMothCut)s )" %props
                    ]

    KPi_LineCode = """
    TC_HLT1COMBINER( 'Hlt1CalibTrackingKPis', 
                     KPiCombinationConf, 
                     'Hlt1CalibTrackingKaons', %(KaonCuts)s ,
                     'Hlt1CalibTrackingPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKPis',         LoKi.Monitoring.ContextSvc ) )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_KPiUnit = HltUnit(
        'Hlt1'+name+'KPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KPi_Preambulo,
        Code = KPi_LineCode
        )

    return hlt1CalibTrackingLine_KPiUnit

  def KK_Unit( self, name, props ) :
    
    props['KaonCuts']        = 'ALL' 
    props['KKDecay']         = '\'D0 -> K+ K-\''
    props['KKCombCut']       = ("( (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) " +
                                "| (ADAMASS('phi(1020)')<%(PhiMassWinLoose)s*MeV) " +
                                "| ( (DAMASS('B0')>-%(B0MassWinLoose)s*MeV) " +
                                "& (DAMASS('B_s0')<%(B0MassWinLoose)s*MeV) ) ) " +
                                "& (APT>%(CombAPT)s*MeV) " +
                                "& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) " +
                                "& (AMAXCHILD(P)>%(CombMaxDaughtP)s*MeV) " +
                                "& (ACUTDOCA(%(CombDOCA)s*mm,'')) ") %props
    props['KKMothCut']       = ("( (ADMASS('D0')<%(D0MassWin)s*MeV) " +
                                "| (ADMASS('phi(1020)')<%(PhiMassWin)s*MeV) " +
                                "| ( (DMASS('B0')>-%(B0MassWin)s*MeV) " +
                                "& (DMASS('B_s0')<%(B0MassWin)s*MeV) ) ) " +
                                "& (BPVDIRA > %(CombDIRA)s) " +
                                "& (VFASPF(VCHI2)<%(CombVCHI2)s) " +
                                "& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)") %props

    KK_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD",
                  "from LoKiPhys.decorators import PT",
                  "KKCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KKDecay)s, %(KKCombCut)s, %(KKMothCut)s )" %props
                    ]

    KK_LineCode = """
    TC_HLT1COMBINER( 'Hlt1CalibTrackingKKs', 
                     KKCombinationConf, 
                     'Hlt1CalibTrackingKaons', %(KaonCuts)s ) 
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKKs', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKKs',         LoKi.Monitoring.ContextSvc ) )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_KKUnit = HltUnit(
        'Hlt1'+name+'KKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KK_Preambulo,
        Code = KK_LineCode
        )

    return hlt1CalibTrackingLine_KKUnit

  def PiPi_Unit( self, name, props ) :
    
    props['PionCuts']        = 'ALL' 
    props['PiPiDecay']       = '\'D0 -> pi+ pi-\''
    props['PiPiCombCut']     = ("( (ADAMASS('D0')<%(D0MassWinLoose)s*MeV) " +
                                "| ( (DAMASS('B0')>-%(B0MassWinLoose)s*MeV) " +
                                "& (DAMASS('B_s0')<%(B0MassWinLoose)s*MeV) ) ) " +
                                "& (APT>%(CombAPT)s*MeV) " +
                                "& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) " +
                                "& (AMAXCHILD(P)>%(CombMaxDaughtP)s*MeV) " +
                                "& (ACUTDOCA(%(CombDOCA)s*mm,'')) ") %props
    props['PiPiMothCut']     = ("( (ADMASS('D0')<%(D0MassWin)s*MeV) " +
                                "| ( (DMASS('B0')>-%(B0MassWin)s*MeV) " +
                                "& (DMASS('B_s0')<%(B0MassWin)s*MeV) ) ) " +
                                "& (BPVDIRA > %(CombDIRA)s) " +
                                "& (VFASPF(VCHI2)<%(CombVCHI2)s) " +
                                "& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s) " +
                                "& (BPVLTIME()>%(CombTAU)s*ps)") %props

    PiPi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD",
                  "from LoKiPhys.decorators import PT",
                  "PiPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(PiPiDecay)s, %(PiPiCombCut)s, %(PiPiMothCut)s )" %props
                    ]

    PiPi_LineCode = """
    TC_HLT1COMBINER( 'Hlt1CalibTrackingPiPis', 
                     PiPiCombinationConf, 
                     'Hlt1CalibTrackingPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPiPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPiPis',         LoKi.Monitoring.ContextSvc ) )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_PiPiUnit = HltUnit(
        'Hlt1'+name+'PiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = PiPi_Preambulo,
        Code = PiPi_LineCode
        )

    return hlt1CalibTrackingLine_PiPiUnit

  def D2KPi_Unit( self, name, props ) :

    D2KPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKPis' )
    >>  (ADMASS('D0')<%(D0MassWin)s*MeV)
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2KPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2KPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2KPiUnit = HltUnit(
        'Hlt1'+name+'D2KPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2KPi_LineCode
        )

    return hlt1CalibTrackingLine_D2KPiUnit

  def D2KK_Unit( self, name, props ) :

    D2KK_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  ( (ADMASS('D0')<%(D0MassWin)s*MeV) & (BPVLTIME()>%(CombTAU)s) ) 
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2KK', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2KKs',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKKDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2KKUnit = HltUnit(
        'Hlt1'+name+'D2KKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2KK_LineCode
        )
    
    return hlt1CalibTrackingLine_D2KKUnit

  def D2PiPi_Unit( self, name, props ) :

    D2PiPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingPiPis' )
    >>  (ADMASS('D0')<%(D0MassWin)s*MeV)
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2PiPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2PiPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingPiPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2PiPiUnit = HltUnit(
        'Hlt1'+name+'D2PiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2PiPi_LineCode
        )

    return hlt1CalibTrackingLine_D2PiPiUnit

  def B2KPi_Unit( self, name, props ) :

    B2KPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKPis' )
    >>  ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) & (SUMTREE(PT,(ISBASIC & HASTRACK))>%(B0SUMPT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass B2KPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nB2KPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2HH_LTUNB_KPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2KPiUnit = HltUnit(
        'Hlt1'+name+'B2KPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = B2KPi_LineCode
        )
    
    return hlt1CalibTrackingLine_B2KPiUnit

  def B2KK_Unit( self, name, props ) :

    B2KK_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) & (SUMTREE(PT,(ISBASIC & HASTRACK))>%(B0SUMPT)s*MeV) & (BPVLTIME()>%(CombTAU)s) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass B2KK', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nB2KKs',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2HH_LTUNB_KKDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2KKUnit = HltUnit(
        'Hlt1'+name+'B2KKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = B2KK_LineCode
        )
    
    return hlt1CalibTrackingLine_B2KKUnit

  def B2PiPi_Unit( self, name, props ) :

    B2PiPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingPiPis' )
    >>  ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) & (SUMTREE(PT,(ISBASIC & HASTRACK))>%(B0SUMPT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass B2PiPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nB2PiPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2HH_LTUNB_PiPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2PiPiUnit = HltUnit(
        'Hlt1'+name+'B2PiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = B2PiPi_LineCode
        )
  
    return hlt1CalibTrackingLine_B2PiPiUnit

  def IncPhi_Unit( self, props ) :

    IncPhi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  (ADMASS('phi(1020)')<%(PhiMassWin)s*MeV)
    >>  tee ( monitor( TC_SIZE > 0, '# pass IncPhi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nIncPhis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1IncPhiDecision')
    >>  ~TC_EMPTY
    """ %props
    
    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_IncPhiUnit = HltUnit(
        'Hlt1IncPhiUnit',
        #OutputLevel = 1,
        Monitor = True,
        Code = IncPhi_LineCode
        )
    
    return hlt1CalibTrackingLine_IncPhiUnit

  def LTUNBPhi_Unit( self, name, props ) :

    LTUNBPhi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  (ADMASS('phi(1020)')<%(PhiMassWin)s*MeV)
    >>  tee ( monitor( TC_SIZE > 0, '# pass IncPhi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nIncPhis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1LTUNBPhis')
    >>  ~TC_EMPTY
    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_LTUNBPhiUnit = HltUnit(
        'Hlt1'+name+'IncPhiUnit',
        #OutputLevel = 1,
        Monitor = True,
        Code = LTUNBPhi_LineCode
        )
    
    return hlt1CalibTrackingLine_LTUNBPhiUnit

  def B2PhiPhi_Unit( self, name, props ) :
    
    props['PhiCuts']        = ("(PT>%(PhiPT)s*MeV)") %props
    props['PhiPhiDecay']    = '\'B_s0 -> phi(1020) phi(1020)\''
    props['PhiPhiCombCut']  = "(ADAMASS('B_s0')<%(B0MassWinLoose)s*MeV)" %props
    props['PhiPhiMothCut']  = ("( (ADMASS('B_s0')<%(B0MassWin)s*MeV) " +
                               "& (BPVLTIME()>%(CombTAU)s) )") %props

    B2PhiPhi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD",
                           "from LoKiPhys.decorators import PT",
                           "B2PhiPhiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(PhiPhiDecay)s, %(PhiPhiCombCut)s, %(PhiPhiMothCut)s )" %props ]
    
    B2PhiPhi_LineCode = """
    TC_HLT1COMBINER( '',
                     B2PhiPhiCombinationConf,
                     'Hlt1LTUNBPhis', %(PhiCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKKs', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKKs',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1B2PhiPhi_LTUNBDecision' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2PhiPhiUnit = HltUnit(
        'Hlt1'+name+'B2PhiPhiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = B2PhiPhi_Preambulo,
        Code = B2PhiPhi_LineCode
        )

    return hlt1CalibTrackingLine_B2PhiPhiUnit

  def Gamma_Unit( self, name, props ) :
    
    from HltTracking.Hlt1Tracking import L0CaloCandidates

    Gamma_Preambulo = [L0CaloCandidates('Photon')]

    Gamma_LineCode = """
    L0CaloCandidates
    >>  tee  ( monitor( TC_SIZE > 0, '# pass CaloCandidates', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nCaloCandidates' , LoKi.Monitoring.ContextSvc ) )
    >>  TC_FROMCALOTOPARTICLES( 'gamma',  '', (PT>%(GAMMA_PT_MIN)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPhotons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPhotons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2PhiGamma_Photons')
    >>  ~TC_EMPTY
    """ % props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_GammaUnit = HltUnit(
        'Hlt1'+name+'GammaUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = Gamma_Preambulo,
        Code = Gamma_LineCode
        )

    return hlt1CalibTrackingLine_GammaUnit

  def B2PhiGamma_Unit( self, name, props ) :
    
    props['PhiCuts']          = ("(PT>%(PhiPT)s*MeV)") %props
    props['GammaCuts']        = 'ALL'
    props['PhiGammaDecay']    = '\'B_s0 -> phi(1020) gamma\''
    props['PhiGammaCombCut']  = "(ADAMASS('B_s0')<%(B0MassWinLoose)s*MeV)" %props
    props['PhiGammaMothCut']  = ("( (ADMASS('B_s0')<%(B0MassWin)s*MeV) " +
                               "& (BPVLTIME()>%(CombTAU)s) )") %props

    B2PhiGamma_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD",
                             "from LoKiPhys.decorators import PT",
                             "B2PhiGammaCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(PhiGammaDecay)s, %(PhiGammaCombCut)s, %(PhiGammaMothCut)s )" %props ]
    
    B2PhiGamma_LineCode = """
    TC_HLT1COMBINER( '',
                     B2PhiGammaCombinationConf,
                     'Hlt1LTUNBPhis',          %(PhiCuts)s   ,
                     'Hlt1B2PhiGamma_Photons', %(GammaCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKKs', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKKs',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1B2PhiGamma_LTUNBDecision' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2PhiGammaUnit = HltUnit(
        'Hlt1'+name+'B2PhiGammaUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = B2PhiGamma_Preambulo,
        Code = B2PhiGamma_LineCode
        )

    return hlt1CalibTrackingLine_B2PhiGammaUnit

  def Tracking_Streamer(self,name,props):
    
    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from HltTracking.HltPVs import PV3D

    return [ Hlt1GECUnit( 'Loose' ), PV3D('Hlt1'), self.hh_TrackingUnit(name, props) ]

  def KPi_Streamer(self,name,props):

    return self.Tracking_Streamer(name,props) + [ self.hh_KaonUnit(name,props), self.hh_PionUnit(name,props), self.KPi_Unit(name,props) ]

  def KK_Streamer(self,name,props):

    return self.Tracking_Streamer(name,props) + [ self.hh_KaonUnit(name,props), self.KK_Unit(name,props) ]

  def PiPi_Streamer(self,name,props):

    return self.Tracking_Streamer(name,props) + [ self.hh_PionUnit(name,props), self.PiPi_Unit(name,props) ]

  def D2KPi_Streamer(self,name,props):
    
    return self.KPi_Streamer(name,props) + [ self.D2KPi_Unit(name,props) ]

  def D2KK_Streamer(self,name,props):

    return self.KK_Streamer(name,props) + [ self.D2KK_Unit(name,props) ]
  
  def D2PiPi_Streamer(self,name,props):

    return self.PiPi_Streamer(name,props) + [ self.D2PiPi_Unit(name,props) ]

  def B2KPi_Streamer(self,name,props):
    
    return self.KPi_Streamer(name,props) + [ self.B2KPi_Unit(name,props) ]

  def B2KK_Streamer(self,name,props):

    return self.KK_Streamer(name,props) + [ self.B2KK_Unit(name,props) ]
  
  def B2PiPi_Streamer(self,name,props):

    return self.PiPi_Streamer(name,props) + [ self.B2PiPi_Unit(name,props) ]

  def IncPhi_Streamer(self,name,props):

    return self.KK_Streamer(name,props) + [ self.IncPhi_Unit(props) ]

  def B2PhiPhi_Streamer(self,name,props):

    return self.KK_Streamer(name,props) + [ self.LTUNBPhi_Unit(name,props), self.B2PhiPhi_Unit(name,props) ]

  def B2PhiGamma_Streamer(self,name,props):

    return self.KK_Streamer(name,props) + [ self.LTUNBPhi_Unit(name,props), self.Gamma_Unit(name,props), self.B2PhiGamma_Unit(name,props) ]

  def __apply_configuration__(self) :

    from HltLine.HltLine import Hlt1Line

    Hlt1Line ( 'CalibTrackingKPi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.D2KPi_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'CalibTrackingKK'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.D2KK_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'CalibTrackingPiPi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.D2PiPi_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'B2HH_LTUNB_KPi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.B2KPi_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'B2HH_LTUNB_KK'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.B2KK_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'B2HH_LTUNB_PiPi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.B2PiPi_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'IncPhi'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.IncPhi_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'B2PhiPhi_LTUNB'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.B2PhiPhi_Streamer( 'CalibTracking', self.getProps() )
          )

    Hlt1Line ( 'B2PhiGamma_LTUNB'
           , prescale = self.prescale
           , postscale = self.postscale
           , L0DU = "L0_ALL"
           , ODIN = ''
           , algos = self.B2PhiGamma_Streamer( 'CalibTracking', self.getProps() )
          )
