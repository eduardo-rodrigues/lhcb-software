"""
High-level configuration tool for running L0Emulation
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import LHCbApp
#, L0Conf, L0DUFromRawAlg
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
GaudiSequencer, DstConf, L0Conf, CondDB, GlobalRecoConf, RawEventJuggler, DecodeRawEvent,
RawEventFormatConf, LumiAlgsConf, InputCopyStream, RecombineRawEvent)

class Tesla(LHCbConfigurableUser):
    __used_configurables__ = [ LHCbApp, LumiAlgsConf, RawEventJuggler, DecodeRawEvent, RawEventFormatConf, DstConf, RecombineRawEvent]

    __slots__ = {
            "EvtMax"		: -1    	# Maximum number of events to process
          , "SkipEvents"	: 0		# Skip to event
          , "Simulation"	: True 		# True implies use SimCond
          , "DataType"		: '2012' 	# Data type, can be [ 'DC06','2008' etc...]
          , "DDDBtag" 		: 'default' 	# default as set in DDDBConf for DataType
          , "CondDBtag" 	: 'default' 	# default as set in DDDBConf for DataType
          , 'Persistency' 	: '' 		# None, Root or Pool?
          , 'OutputLevel' 	: 4 		# Er, output level
          , "outputFile" 	: 'Tesla.dst' 	# output filename
          , 'WriteFSR'    	: False 	# copy FSRs as required
          , 'PV'	        : "Online"      # Associate to the PV chosen by the HLT or the offline one
          , 'PreSplit'	        : False         # Are you looking at a time before the HLT was split?
          , 'ReportVersion'	: 2	        # Do we have the normal or extendedselection reports
          , 'TriggerLines'	: ["Hlt2IncPhi"]# Which trigger line
          , 'Mode'	        : "Offline"     # "Online" (strip unnecessary banks and run lumi algorithms) or "Offline"?
          , 'RawFormatVersion'	: 0.2           # Which banks form the Turbo stream
          }
    _propertyDocDct ={
            "EvtMax"		: "Maximum number of events to process, default all"
            , "SkipEvents"	: "Events to skip, default     0"
            , "Simulation"	: "True implies use SimCond"
            , "DataType"	: "Data type, can be [ 'DC06','2008' ...]"
            , "DDDBtag" 	: "Databse tag, default as set in DDDBConf for DataType"
            , "CondDBtag" 	: "Databse tag, default as set in DDDBConf for DataType"
            , 'Persistency' 	: "Root or Pool?"
            , 'OutputLevel' 	: "Output level"
            , "outputFile" 	: 'output filename, automatically selects MDF or InputCopyStream'
            , 'WriteFSR'    	: 'copy FSRs as required'
            , 'PV'     	        : 'Associate to the PV chosen by the HLT or the offline one'
            , 'PreSplit'     	: 'Are you looking at a time before the HLT was split?'
            , 'ReportVersion'   : '1: Normal HLT reports, 2: New extended reports'
            , 'TriggerLines'    : 'Which trigger line to process'
            , 'Mode'     	: '"Online" (strip unnecessary banks and run lumi algorithms) or "Offline"?'
            , 'RawFormatVersion': 'Which banks form the Turbo stream'
            }

    writerName = "DstWriter"
    teslaSeq = GaudiSequencer("TeslaSequence")
    
    def _safeSet(self,conf,param):
        for p in param:
            if (not self.isPropertySet(p)) or conf.isPropertySet(p):
                continue
            conf.setProp(p,self.getProp(p))
        
    def _configureDataOnDemand(self) :
        if 'DataOnDemandSvc' in ApplicationMgr().ExtSvc : 
            ApplicationMgr().ExtSvc.pop('DataOnDemandSvc')
        else: 
            from Configurables import DataOnDemandSvc
            dod = DataOnDemandSvc()
        if dod not in ApplicationMgr().ExtSvc :
            ApplicationMgr().ExtSvc.append( dod ) 

    def _configureForOnline(self):
        #
        DecodeRawEvent().DataOnDemand=False
        writer=InputCopyStream( self.writerName )
        DstConf().setProp("SplitRawEventOutput", self.getProp("RawFormatVersion"))
        
        # Use RawEventJuggler to create the Turbo stream raw event format
        tck = "0x409f0045" # DUMMY
        TurboBanksSeq=GaudiSequencer("TurboBanksSeq")
        RawEventJuggler().TCK=tck
        RawEventJuggler().Input="Moore"
        RawEventJuggler().Output=self.getProp("RawFormatVersion")
        RawEventJuggler().Sequencer=TurboBanksSeq
        RawEventJuggler().WriterOptItemList=writer
        RawEventJuggler().KillExtraNodes=True
        RawEventJuggler().KillExtraBanks=True
        RawEventJuggler().KillExtraDirectories = True
        self.teslaSeq.Members += [TurboBanksSeq]
        
        # Begin Lumi configuration
        lumiSeq = GaudiSequencer("LumiSeq")
        #
        # Add ODIN decoder to LumiSeq ***
        from DAQSys.Decoders import DecoderDB
        CreateODIN=DecoderDB["createODIN"].setup()
        #********************************
        #
        # Main algorithm config
        lumiCounters = GaudiSequencer("LumiCounters")
        lumiCounters.Members+=[CreateODIN]
        lumiSeq.Members += [ lumiCounters ]
        LumiAlgsConf().LumiSequencer = lumiCounters
        LumiAlgsConf().OutputLevel = self.getProp('OutputLevel')
        LumiAlgsConf().InputType = "MDF"
        #
        # Filter out Lumi only triggers from further processing, but still write to output
        from Configurables import HltRoutingBitsFilter
        physFilter = HltRoutingBitsFilter( "PhysFilter", RequireMask = [ 0x0, 0x4, 0x0 ] )
        lumiFilter = HltRoutingBitsFilter( "LumiFilter", RequireMask = [ 0x0, 0x2, 0x0 ] )
        lumiSeq.Members += [ lumiFilter, physFilter ]
        lumiSeq.ModeOR = True
        #
        from Configurables import RecordStream
        FSRWriter = RecordStream( "FSROutputStreamDstWriter")
        FSRWriter.OutputLevel = INFO
        #
        # Sequence to be executed if physics sequence not called (nano events)
        notPhysSeq = GaudiSequencer("NotPhysicsSeq")
        notPhysSeq.ModeOR = True
        notPhysSeq.Members = [ physFilter ]
        writer.AcceptAlgs += ["LumiSeq","NotPhysicsSeq"]
        self.teslaSeq.Members += [lumiSeq, notPhysSeq]
    
    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        
        #retrieve the persistency
        persistency=None
        if hasattr(self, "Persistency"):
            if self.getProp("Persistency") is not None:
                persistency=self.getProp("Persistency")
        
        from GaudiConf import IOExtension, IOHelper
        iox=IOExtension(persistency)
        
        writer = InputCopyStream(self.writerName)
        #DstConf().Writer     = self.writerName
        #DstConf().AlwaysCreate=True
        #DstConf().DstType="DST"

        seq=GaudiSequencer('TeslaReportAlgoSeq')
        writer.RequireAlgs += ['TeslaReportAlgoSeq']
        #
        if self.getProp('Mode') is "Online":
            from DAQSys.Decoders import DecoderDB
            Hlt1VertexReportsDecoder=DecoderDB["HltVertexReportsDecoder/Hlt1VertexReportsDecoder"].setup()
            Hlt2SelReportsDecoder=DecoderDB["HltSelReportsDecoder/Hlt2SelReportsDecoder"].setup()
            seq.Members += [ Hlt1VertexReportsDecoder, Hlt2SelReportsDecoder ]
        #
        lines = self.getProp('TriggerLines')
        for l in lines:
            trig1 = self._configureReportAlg(l)
            seq.Members+=[trig1]
            # NEED TO SET THE LOCATIONS IN THE REPORT ALGORITHM
            writer.ItemList+=[ l + "/Particles#1" 
                    , l + "/Protos#1"
                    , l + "/Vertices#1"
                    , l + "/Tracks#1"
                    , l + "/RichPIDs#1"
                    , l + "/MuonPIDs#1"
                    , l + "/Primary#1"
                    , l + "/Particle2VertexRelations#1"
                    ]
        self.teslaSeq.Members += [ seq ]
        IOHelper(persistency,persistency).outStream(fname,writer,writeFSR=self.getProp('WriteFSR'))
    
    def _configureReportAlg(self,line):
        from Configurables import TeslaReportAlgo
        trig1 = TeslaReportAlgo("TeslaReportAlgo"+line)
        trig1.OutputPrefix=line
        trig1.ReportVersion=self.getProp('ReportVersion')
        trig1.PV=self.getProp('PV')
        trig1.PreSplit=self.getProp('PreSplit')
        trig1.TriggerLine=line
        trig1.OutputLevel=self.getProp('OutputLevel')
        return trig1
    
    def __apply_configuration__(self):
        #GaudiKernel.ProcessJobOptions.PrintOff()
        
        ############## Set other properties ###########
        self._safeSet( LHCbApp(), ['EvtMax','SkipEvents','Simulation', 'DataType' , 'CondDBtag','DDDBtag'] )
        ApplicationMgr().AppName="Tesla, utilising DaVinci"
        #
        if self.getProp('Mode') is "Online":
            self.setProp('WriteFSR',True)
            self._configureForOnline()
        else:
            DecodeRawEvent().DataOnDemand=True
            RecombineRawEvent()
        #
        self._configureOutput()
        #
        from Configurables import EventSelector
        EventSelector().PrintFreq = 1000

        ApplicationMgr().TopAlg+=[self.teslaSeq]
