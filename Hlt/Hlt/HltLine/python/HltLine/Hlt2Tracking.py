### @file
#
#  Hlt Tracking.
#
#  @author V. Gligorov   Vladimir.Gligorov@cern.ch 
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2010-02-21
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from HltTrackNames import Hlt2LongTracksName, HltSharedTracksPrefix
from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import Hlt2ForwardTracksName, Hlt2SeedingTracksName
from HltTrackNames import Hlt2MatchTracksName, Hlt2DownstreamTracksName   
from HltTrackNames import Hlt2VeloTracksName, HltSharedRZVeloTracksName
###################################################################################
#
# Hlt2 Tracking
#
class Hlt2Tracking(LHCbConfigurableUser):
    __used_configurables__ = [ "HltCaloRecoConf", "HltCaloPIDsConf", "RichRecSysConf" ]
    __slots__ = {
        "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
        , "Hlt2Tracks"                 : Hlt2LongTracksName
        , "Prefix"                     : HltSharedTracksPrefix     # default prefix for all instance names (almost common)
        , "Suffix"                     : "" #or Fitted, FittedRich, etc.
        , "UseRICH"                    : False 
        , "UseCALO"		       : False
        , "DoSeeding"                  : False 
        , "DoCloneKilling"             : False 	
        }
###################################################################################
    #
    # Charged ProtoParticles
    #
    def hlt2ChargedCaloProtos(self):
        """
        Charged protoparticles from Calo (=electrons)
        """
        return _hlt2ChargedCaloProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                     prefix = self.getProp("Prefix"), 
                                     suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"))
###################################################################################
    #
    def hlt2ChargedHadronProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return _hlt2ChargedHadronProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                       prefix = self.getProp("Prefix"),
                                       suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"),
                                       UseRICH= self.getProp("UseRICH"),
				       UseCALO= self.getProp("UseCALO"))
###################################################################################
    #
    def hlt2ChargedProtos(self):
        """
        Charged protoparticles which know nothing about PID
        """
        return _hlt2ChargedProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                       prefix = self.getProp("Prefix"),
                                       suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"))
###################################################################################
    #
    def hlt2MuonProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return _hlt2MuonProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                              DataType = self.getProp("DataType"),
                              prefix = self.getProp("Prefix"), 
			      suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling") )
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        return _hlt2NeutralProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
				 suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling") )
###################################################################################
    #
    # Tracking sequence
    #
    def hlt2Tracking(self):
        """
        Tracks
        """
        return _hlt2Tracking(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
				 suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"))
###################################################################################
    #
    # Velo tracking for the PV making sequence
    #
    def hlt2VeloTracking(self):
        """ 
        Velo tracks
        """
        return _hlt2VeloTracking(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
                                 suffix = self.getProp("Suffix"))
###################################################################################
    #
    # Track fitting
    #
    def hlt2StagedFastFit(self):
        """
	The staged fast fit
	"""
	return _hlt2StagedFastFit(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
                                 suffix = HltBiDirectionalKalmanFitSuffix ,
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"))
#########################################################################################
#########################################################################################
#
# Actual implementation of the functions
#
#########################################################################################
# Compute the track location from the prefix, suffix, etc.
# TODO: move to HltTrackNames 
def _trackLocation(prefix,type,suffix,tracks):
    if (suffix == "") : return prefix + "/" + type + "/" + tracks
    else : return prefix + "/" + type + "/" + suffix + "/" + tracks

# Compute the protos location from the prefix and suffix and Hlt2Tracks
# TODO: move to HltTrackNames
def _protosLocation(prefix,suffix,tracks):
    from HltTrackNames import Hlt2SharedProtoParticleSuffix, Hlt2ChargedProtoParticleSuffix
    if (suffix == "") : return prefix +"/" + Hlt2SharedProtoParticleSuffix + "/" + Hlt2ChargedProtoParticleSuffix + "/" + tracks 
    else :              return prefix +"/" + Hlt2SharedProtoParticleSuffix + "/" + suffix + "/" + Hlt2ChargedProtoParticleSuffix + "/" + tracks

# Compute the output of the tracking based on the track type and whether
# seeding is run or not
def _getOutputOfTracking(tracks, doSeeding) :
    if (tracks == Hlt2LongTracksName) :
        if doSeeding : return Hlt2LongTracksName
        else : return Hlt2ForwardTracksName
    elif (tracks == Hlt2ForwardTracksName) :
        return Hlt2ForwardTracksName
    elif (tracks == Hlt2DownstreamTracksName) :
        return Hlt2DownstreamTracksName
    else : 
	return "Unknown"

# In case something went wrong when specifying the options, warn me before you die
def _printWarningBeforeDeath() :
	log.fatal( '###############################################################')
        log.fatal( '## WARNING You specified an unknown track type %s for the Hlt2 Reconstruction##' % Hlt2Tracks )
        log.fatal( '## WARNING I will now die, you need to make Hlt2Tracking aware of this track type!##')
        log.fatal( '###############################################################')
#########################################################################################
#
# Electron Protos
#
def _hlt2ChargedCaloProtos(Hlt2Tracks = Hlt2LongTracksName,
                          prefix=HltSharedTracksPrefix, suffix = "", doSeeding = False
                , doCloneKilling = False ):
    """
    Charged Calo protoparticles = electrons
    Requires chargedProtos
    """
    from Configurables import ( ChargedProtoParticleAddEcalInfo,
                                ChargedProtoParticleAddBremInfo,
                                ChargedProtoParticleAddPrsInfo,
                                ChargedProtoParticleAddSpdInfo,
#                                ChargedProtoParticleAddVeloInfo,
                                ChargedProtoCombineDLLsAlg )
    #The different add PID algorithms
    #
    # The charged protoparticles and their output location
    #
    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return [] 
    # 
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling)
    chargedProtosOutputLocation = chargedProtos.outputSelection()
    #
    ecal = ChargedProtoParticleAddEcalInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddEcal")
    ecal.ProtoParticleLocation =  chargedProtosOutputLocation
    brem = ChargedProtoParticleAddBremInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddBrem")
    brem.ProtoParticleLocation =  chargedProtosOutputLocation
    prs  = ChargedProtoParticleAddPrsInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddPrs")
    prs.ProtoParticleLocation =  chargedProtosOutputLocation
    spd  = ChargedProtoParticleAddSpdInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddSpd")
    spd.ProtoParticleLocation =  chargedProtosOutputLocation
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPCombDLLsAfterCalo")
    combine.ProtoParticleLocation =  chargedProtosOutputLocation
    # Fill the sequence
    # we need the calo reconstruction, this does not yet work for
    # fitted tracks...
    caloreco = _hlt2Calo(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling)
    from HltLine import bindMembers
    return bindMembers( None, [ caloreco, chargedProtos, ecal,brem,prs,spd,combine ]).setOutputSelection(chargedProtosOutputLocation) 
    
#########################################################################################
#
# Muon Protos
#
def _hlt2MuonProtos(Hlt2Tracks = Hlt2LongTracksName,
                   DataType = "2009", 
                   prefix=HltSharedTracksPrefix,
         	   suffix = "", doSeeding = False
                , doCloneKilling = False ):
    """
    Charged muon protoparticles
    Requires chargedProtos and muon ID
    """
    from Configurables import ( ChargedProtoParticleAddMuonInfo,
                                ChargedProtoCombineDLLsAlg )
   
    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return []
    #
 
    #The different add PID algorithms
    #
    # The charged protoparticles and their output location
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling)
    chargedProtosOutputLocation = chargedProtos.outputSelection()
    #
    
    muon = ChargedProtoParticleAddMuonInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddMuon")
    muon.ProtoParticleLocation =  chargedProtosOutputLocation
    #Enforce naming conventions
    muon.InputMuonPIDLocation = _trackLocation(prefix,"Muon",suffix,"MuonPID")
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPCombDLLsAfterMuon")
    combine.ProtoParticleLocation =  chargedProtosOutputLocation

    muonID = _hlt2Muon(Hlt2Tracks, DataType, prefix, suffix, doSeeding,doCloneKilling)
    from HltLine import bindMembers
    return bindMembers( None, [ muonID, chargedProtos, muon, combine ] ).setOutputSelection(chargedProtosOutputLocation)  

#########################################################################################
#
# Charged ProtoParticles
# Does not necessarily use RICH
#
def _hlt2ChargedHadronProtos(Hlt2Tracks = Hlt2LongTracksName,
                            prefix=HltSharedTracksPrefix, 
			    suffix = "",
			    doSeeding = False,
                            doCloneKilling = False,
                            UseRICH=False, 
			    UseCALO = False):
    """
    Charged hadron protoparticles = pion, kaon, proton
    If RICH is on, then requires rich, calo and does dll combination
    This is off by default
    """
    from Configurables import (	ChargedProtoParticleAddRichInfo,
				ChargedProtoParticleAddMuonInfo,
                                ChargedProtoParticleAddHcalInfo,
                                ChargedProtoCombineDLLsAlg)
    from HltLine import bindMembers
    from RichRecSys.Configuration import (RichRecSysConf,
					  RichTrackCreatorConfig,
                               		  RichPixelCreatorConfig
					  )

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return []
    #

    #The different add PID algorithms
    #
    # The charged protoparticles and their output location
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling)
    chargedProtosOutputLocation = chargedProtos.outputSelection()
    #

    if ( UseRICH and not UseCALO):
        
        #configure the rich reco
	richseq = GaudiSequencer("HltRICHReco")
	richseq.MeasureTime = True
	# The RICH COnfigurable
	richConf = RichRecSysConf()
	# Set the sequence to run the RICH PID in
	richConf.setProp("RecoSequencer",richseq)
	# Configure for Fast HLT Global PID
	richConf.Context   = "HLT"
	richConf.PidConfig = "FastGlobal"
	# Don't bother checking ProcStatus online
	richConf.CheckProcStatus = False
	# Initial pixels tracks and photons
	richConf.InitPixels  = True
	richConf.InitTracks  = True
	richConf.InitPhotons = True
	# Turn off ring finding
	richConf.TracklessRingAlgs = []
	# Turn off book keeping for pixels, for speed
	RichPixelCreatorConfig().BookKeeping  = False
	RichPixelCreatorConfig().FindClusters = False
        
	# Create RICH PID using the Fitted tracks
        # This is something of a special case because
        # the RICH ID simply doesn't work without a track fit
        # TODO: write some checks at the __apply_configuration__ stage
        #       that the created configuration makes sense.   
        RichTrackCreatorConfig().InputTracksLocation = _hlt2StagedFastFit(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling).outputSelection() 
        # The Rich info 
        rich = ChargedProtoParticleAddRichInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddRich")
        rich.ProtoParticleLocation =  chargedProtosOutputLocation 
	# The combined DLL
        combine = ChargedProtoCombineDLLsAlg(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPCombDLLsAfterRich")
        combine.ProtoParticleLocation =  chargedProtosOutputLocation

        return bindMembers( None , [ chargedProtos, richseq, rich, combine ] ).setOutputSelection(chargedProtosOutputLocation)
    else :
    	#delegate everything to the charged proto maker
	#because the CALO does not work for now! This needs to be fixed!!!
        return bindMembers( None , [ chargedProtos ] ).setOutputSelection(chargedProtosOutputLocation)

    # The HCAL info
    #hcal = ChargedProtoParticleAddHcalInfo(prefix+suffix+outputOfHlt2Tracking+"ChargedProtoPAddHcal")
    #hcal.ProtoParticleLocation = protoslocation
    # The calo DLL
    #caloID = _hlt2Calo(Hlt2Tracks, prefix, suffix)
    
#########################################################################################
#
# Charged ProtoParticles
#
def _hlt2ChargedProtos(Hlt2Tracks = Hlt2LongTracksName,
                      prefix=HltSharedTracksPrefix, 
                      suffix = "",
                      doSeeding = False,
                      doCloneKilling = False):
    """
    Charged protoparticles
    
    I don't think a prefix different to Hlt works.
    For the moment we have some clumsy naming to start the
    protos with Hlt/...
    Will fix if Hlt2 works as a prefix
    Requires tracks, fitted if necessary
    """

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return []
    #

    from Configurables import ( ChargedProtoParticleMaker,
                                DelegatingTrackSelector  )
    
    chargedProtosOutputLocation = _protosLocation(prefix,suffix,Hlt2Tracks)

    charged = ChargedProtoParticleMaker(prefix+suffix+outputOfHlt2Tracking+'ChargedProtoPAlg')
    charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
    # Next bit separates long and downstream tracks in the particle making
    # TODO: remove hardcoding of track types!
    if (outputOfHlt2Tracking == Hlt2LongTracksName or outputOfHlt2Tracking == Hlt2ForwardTracksName) : 
	charged.TrackSelector.TrackTypes = [ "Long" ] 
    elif (outputOfHlt2Tracking == Hlt2DownstreamTracksName) :
	charged.TrackSelector.TrackTypes = [ "Downstream" ]
   
    # what track type?
    #Need to allow for fitted tracks
    if (suffix.find("Fitted") > -1) : 
	tracks = _hlt2StagedFastFit(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
	charged.InputTrackLocation = _hlt2StagedFastFit(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling).outputSelection()
    else : 
	tracks = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
        charged.InputTrackLocation = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling).outputSelection()

    charged.OutputProtoParticleLocation =  chargedProtosOutputLocation

    from HltLine import bindMembers
    return bindMembers ( None, [ tracks , charged ] ).setOutputSelection(chargedProtosOutputLocation)

#########################################################################################
#
# Neutral ProtoParticles
#
def _hlt2NeutralProtos(Hlt2Tracks = Hlt2LongTracksName,
                      prefix=HltSharedTracksPrefix, 
                      suffix = "",
                            doSeeding = False,
                            doCloneKilling = False ):
    """
    Neutral protoparticles 
    Requires caloID
    """

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return []
    #

    from Configurables import NeutralProtoPAlg
    HltNeutralProtoPAlg = NeutralProtoPAlg(prefix+suffix+outputOfHlt2Tracking+'NeutralProtoPAlg')
    # Overwrite some default offline settings with HLT special settings (
    # taken from CaloReco.opts)
    HltNeutralProtoPAlg.PhotonIDName = "HltPhotonPID"
    from Configurables import CaloPhotonEstimatorTool
    ToolSvc().addTool(CaloPhotonEstimatorTool, name="HltPhotonPID")
    ToolSvc().HltPhotonPID.TableLocation = "Hlt/Calo/ClusterMatch"
    importOptions( "$CALOPIDSROOT/options/HltPhotonPDF.opts" )
    
    caloID = _hlt2Calo(Hlt2Tracks, prefix,suffix,doSeeding,doCloneKilling)
    
    from HltLine import bindMembers
    return bindMembers ( None, [ caloID, HltNeutralProtoPAlg ]) 
    
#########################################################################################
#
# MuonID
#
def _hlt2Muon(Hlt2Tracks = Hlt2LongTracksName
           , DataType = '2009'
           , prefix=HltSharedTracksPrefix 
           , suffix = "",
                            doSeeding = False,
                            doCloneKilling = False):
    """
    Muon ID options 
    Requires tracks
    """

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return []
    #

    from MuonID import ConfiguredMuonIDs
    from Configurables import MuonRec, MuonIDAlg
    cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=DataType)
    HltMuonIDAlg = cm.configureMuonIDAlg(prefix+suffix+outputOfHlt2Tracking+"MuonIDAlg")
    #Enforce naming conventions
    HltMuonIDAlg.TrackLocation        = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling).outputSelection() 
    HltMuonIDAlg.MuonIDLocation       = _trackLocation(prefix,"Muon",suffix,"MuonPID")    # output
    HltMuonIDAlg.MuonTrackLocation    = _trackLocation(prefix,"Track",suffix,"Muon")
    HltMuonIDAlg.MuonTrackLocationAll = _trackLocation(prefix,"Track",suffix,"AllMuon")
    # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
    #       a factor 3-4
    HltMuonIDAlg.FindQuality = False
    tracks = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    
    from HltLine import bindMembers
    return bindMembers ( None, [ tracks, MuonRec(), HltMuonIDAlg ] )
#########################################################################################
#
# Calo ID
#
def _hlt2Calo(Hlt2Tracks = Hlt2LongTracksName
           , prefix=HltSharedTracksPrefix
           , suffix = "",
                            doSeeding = False,
                            doCloneKilling = False):
    """
    Calo ID
    Does calo reconstruction and Calo ID
    Requires tracks
    @todo : make this track location dependent
    """
    from CaloReco.Configuration import HltCaloRecoConf 
    from CaloPIDs.Configuration import HltCaloPIDsConf
    Hlt2CaloRecoSeq = GaudiSequencer("Hlt2CaloRecoSeq")
    Hlt2CaloPIDsSeq = GaudiSequencer("Hlt2CaloPIDsSeq")
    caloReco = HltCaloRecoConf(Sequence           = Hlt2CaloRecoSeq,
                               Context            = "HLT")
    caloPID  = HltCaloPIDsConf(Sequence           = Hlt2CaloPIDsSeq,
                               Context            = "HLT")
    Hlt2CaloSeq = GaudiSequencer("Hlt2CaloSeq", Context = "HLT" )
    from HltLine import bindMembers
    tracks = _hlt2Tracking(Hlt2Tracks, prefix,suffix,doSeeding,doCloneKilling)
    return bindMembers( None, [ tracks, Hlt2CaloRecoSeq, Hlt2CaloPIDsSeq   ] )
#########################################################################################
#
# Staged fast fit
#
def _hlt2StagedFastFit(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
                , suffix = HltBiDirectionalKalmanFitSuffix,
                            doSeeding = False,
                            doCloneKilling = False) :
    """
    The staged fast-fit. Currently set to
    be bidirectional and with smoothing to allow PID information
    to be used; special cases need to be justified separately
    """
    
    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
        return []
    #

    hlt2StagedFastFitOutputLocation = _trackLocation(prefix,"Track",suffix,Hlt2Tracks)
 
    from Configurables import TrackEventFitter, TrackMasterFitter
    Hlt2StagedFastFitSeq = GaudiSequencer( prefix+suffix+outputOfHlt2Tracking+"StagedFastFitSeq" )
    
    Hlt2StagedFastFit = TrackEventFitter(prefix+suffix+outputOfHlt2Tracking+'StagedFastFit')
    Hlt2StagedFastFitSeq.Members = [ Hlt2StagedFastFit ]

    Hlt2StagedFastFit.TracksInContainer  = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling).outputSelection() 
    Hlt2StagedFastFit.TracksOutContainer =  hlt2StagedFastFitOutputLocation  

    Hlt2StagedFastFit.addTool(TrackMasterFitter, name = 'Fitter')
    from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
    fitter = ConfiguredFastFitter( getattr(Hlt2StagedFastFit,'Fitter'))

    fitter.NodeFitter.BiDirectionalFit = True
    fitter.NodeFitter.Smooth = True
    fitter.AddDefaultReferenceNodes = True    # says Wouter
    
    from HltLine import bindMembers
    #Make the original tracks in case this has not been run already
    tracks = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    return bindMembers( None, [tracks, Hlt2StagedFastFitSeq] ).setOutputSelection(hlt2StagedFastFitOutputLocation)

#########################################################################################
#
# Track reconstruction
#
def _hlt2Tracking(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
 		, suffix = ""
                , doSeeding = False
                , doCloneKilling = False):
    """
    Track reconstruction
    @todo allow to set tracks in reco sequence
    """

    #
    # Some "inevitable" special stuff here
    # If we ask for long tracks and no seeding, the final output will be
    # the forward tracking, otherwise the final output is the long 
    # (forward + match) tracking.
    #
    # First we figure out what the final output container will be
    outputOfHlt2Tracking = _getOutputOfTracking(Hlt2Tracks,doSeeding)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath()
	return []  
    hlt2TrackingOutput = _trackLocation(prefix,"Track","",outputOfHlt2Tracking)
    #
    # The copy sequence, which merges the forward and
    # match tracks into the long track container
    # This only runs if the seeding is run and we ask for Long tracks
    # TODO: fix this ugly hacked way of getting the output locations...
    hlt2TracksToMergeIntoLong = [ _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix)[0].OutputTracksName, _hlt2MatchTracking(Hlt2Tracks,prefix,suffix)[0].MatchOutput ]
    from Configurables import CreateFastTrackCollection
    #### CreateFastTrackCollection
    # this can only be done ONCE per long track output
    recoCopy = CreateFastTrackCollection(prefix+'TrackCopy'
                , InputLocations = hlt2TracksToMergeIntoLong
                , OutputLocation = _trackLocation(prefix,"Track","",Hlt2LongTracksName)
                , SlowContainer = True)
  
    from HltLine import bindMembers
    #Now let us make the reconstruction sequence here
    #The VELO parts
    hlt2Velotracking = _hlt2VeloTracking(Hlt2Tracks,prefix,suffix)
    # The tracking decoding
    from HltDecodeRaw import DecodeTT, DecodeIT
    hlt2TrackerDecoding = DecodeTT.members() + DecodeIT.members()
    # The forward tracking
    hlt2ForwardTracking = _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix)   
    # The seeding
    hlt2SeedTracking = _hlt2SeedTracking(Hlt2Tracks,prefix,suffix)
    # The matching
    hlt2MatchTracking = _hlt2MatchTracking(Hlt2Tracks,prefix,suffix)
    # The downstream tracking 
    hlt2DownstreamTracking = _hlt2DownstreamTracking(Hlt2Tracks,prefix,suffix)
    # The clone killing
    from Configurables import TrackEventCloneKiller, TrackCloneFinder
    cloneKiller = TrackEventCloneKiller(prefix+'FastCloneKiller'
                , TracksInContainers = [hlt2TrackingOutput]
                , SkipSameContainerTracks = False
                , CopyTracks = False)
    cloneKiller.addTool(TrackCloneFinder, name = 'CloneFinderTool')
    cloneKiller.CloneFinderTool.RestrictedSearch = True
    # Finally make the sequence
    # The sequence called depends on the track type, so far we recognise two track types
    # for Long tracks the options doSeeding, doCloneKilling, etc. are meaningful
    # for SeedTT tracks the seeding is mandatory and the CloneKilling is irrelevant 
    # This part gets done in either case
    trackRecoSequence = GaudiSequencer( prefix+outputOfHlt2Tracking+'TrackRecoSequence'
                                  ,  Members = hlt2Velotracking.members() + hlt2TrackerDecoding + hlt2ForwardTracking) 
    if (Hlt2Tracks == Hlt2LongTracksName) :
	# Do the seeding +matching if required
	if doSeeding : 
		trackRecoSequence.Members += hlt2SeedTracking
		trackRecoSequence.Members += hlt2MatchTracking
	        trackRecoSequence.Members += [recoCopy]
	# Do the clone killing if required 
	if doCloneKilling : trackRecoSequence.Members += [cloneKiller]  
    elif (Hlt2Tracks == Hlt2DownstreamTracksName ) :
	# The seeding is mandatory for downstream tracks
	trackRecoSequence.Members += hlt2SeedTracking
	# Now do the downstream tracking
	trackRecoSequence.Members += hlt2DownstreamTracking
    # Bind it all together
    HltRecoSequence = bindMembers(None, trackRecoSequence.Members ) 
    #
    # Hacking of errors
    # @todo Needs to be revised
    #
    # check if the context contains the magic phrase 'Fitted', if so
    # either the tracks were already fitted, 
    # or we need to do the Kalman fit on-demand afterwards, but
    # either way we do NOT parametarize the errors
    # Also, do not insert errors for downstream tracks as these would be meaningless
    if ((suffix.find("Fitted") > -1) or (Hlt2Tracks == Hlt2DownstreamTracksName)) :
	return bindMembers( None, [ HltRecoSequence ] ).setOutputSelection(hlt2TrackingOutput)
    else :
        from Configurables import HltInsertTrackErrParam
   	HltInsertTrackErrParam = HltInsertTrackErrParam(prefix+"InsertTrackErrParam")
   	HltInsertTrackErrParam.InputLocation = hlt2TrackingOutput
   	#
  	# Reco sequence
 	#
 	return bindMembers( None, [ HltRecoSequence, HltInsertTrackErrParam ]).setOutputSelection(hlt2TrackingOutput) 
#########################################################################################
#
# Hlt2 Velo Reconstruction
#
def _hlt2VeloTracking(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
                , suffix = "") :
    """
    Velo track reconstruction for Hlt2
    """
    from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking, Tf__PatVeloSpaceTool
    #From HltReco we just get the shared stuff between Hlt1 and Hlt2
    from HltReco import MinimalRZVelo
    from HltLine import bindMembers 
 
    veloTracksOutputLocation = _trackLocation(prefix,"Track","",Hlt2VeloTracksName)
   
    recoVelo = Tf__PatVeloSpaceTracking(prefix+'RecoVelo'
                                   , InputTracksName = _trackLocation(HltSharedTracksPrefix,"Track","",HltSharedRZVeloTracksName) 
                                   , OutputTracksName = veloTracksOutputLocation )

    recoVelo.addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
    recoVelo.PatVeloSpaceTool.MarkClustersUsed=True

    recoVeloGeneral = Tf__PatVeloGeneralTracking(prefix+'RecoVeloGeneral'
                                   , OutputTracksLocation = veloTracksOutputLocation )
    return bindMembers(None, MinimalRZVelo.members() + [recoVelo,recoVeloGeneral]).setOutputSelection(veloTracksOutputLocation)
#########################################################################################
#
# Hlt2 Forward Reconstruction
#
def _hlt2ForwardTracking(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
                , suffix = "") :
    """
    Forward track reconstruction for Hlt2
    """
    from Configurables import PatForward, PatForwardTool
    forwardTrackOutputLocation = _trackLocation(prefix,"Track","",Hlt2ForwardTracksName)

    recoForward = PatForward( prefix+'RecoForward'
                        , InputTracksName = _hlt2VeloTracking(Hlt2Tracks,prefix,suffix).outputSelection() 
                        , OutputTracksName = forwardTrackOutputLocation )
    PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" )
    recoForward.addTool(PatForwardTool, name='PatForwardTool')
    recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
    return [recoForward]
#########################################################################################
#
# Hlt2 Seeding for forward,downstream reconstruction
#
def _hlt2SeedTracking(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
                , suffix = "") :
    """
    Seeding in the trackers for later use in Match Forward reconstruction
    or in downstream tracking.
    """
    from Configurables import PatSeeding, PatSeedingTool

    seedTrackOutputLocation = _trackLocation(prefix,"Track","",Hlt2SeedingTracksName)

    #### Seeding 
    recoSeeding = PatSeeding(prefix+'Seeding', OutputTracksName = seedTrackOutputLocation)
    recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
    recoSeeding.PatSeedingTool.UseForward = True
    recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
    #TODO: Should fix this bit of hardcoded output type...
    recoSeeding.PatSeedingTool.InputTracksName = _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix)[0].OutputTracksName

    return [recoSeeding]

#########################################################################################
#
# Hlt2 Match Forward Reconstruction
#
def _hlt2MatchTracking(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
                , suffix = ""):
    """
    Forward track reconstruction for Hlt2 using seeding
    """
    from Configurables import PatMatch

    matchTrackOutputLocation = _trackLocation(prefix,"Track","",Hlt2MatchTracksName)

    #### Matching
    #TODO: Should fix this bit of hardcoded output type...
    recoMatch = PatMatch(prefix+'Match'
                     , VeloInput = _hlt2VeloTracking(Hlt2Tracks,prefix,suffix).outputSelection()
                     , SeedInput = _hlt2SeedTracking(Hlt2Tracks,prefix,suffix)[0].OutputTracksName
                             , MatchOutput = matchTrackOutputLocation)

    return [recoMatch]

#########################################################################################
#
# Hlt2 Downstream Forward Reconstruction
#
def _hlt2DownstreamTracking(Hlt2Tracks = Hlt2LongTracksName
                , prefix=HltSharedTracksPrefix
                , suffix = ""):
    """
    Downstream track reconstruction for Hlt2 using seeding
    """
    from Configurables import PatDownstream

    downstreamTrackOutputLocation = _trackLocation(prefix,"Track","",Hlt2DownstreamTracksName)

    ### Downstream tracking
    #TODO: Should fix this bit of hardcoded output type...
    PatDownstream = PatDownstream(prefix+'PatDownstream')
    PatDownstream.InputLocation = _hlt2SeedTracking(Hlt2Tracks,prefix,suffix)[0].OutputTracksName
    PatDownstream.OutputLocation = downstreamTrackOutputLocation
    #PatDownstream.UseForward = True
    #PatDownstream.SeedFilter = True
    PatDownstream.RemoveUsed = True
    PatDownstream.RemoveAll = True

    return [PatDownstream]
