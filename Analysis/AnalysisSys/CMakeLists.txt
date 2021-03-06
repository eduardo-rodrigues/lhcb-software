################################################################################
# Package: AnalysisSys
################################################################################
gaudi_subdir(AnalysisSys v16r0)

gaudi_depends_on_subdirs(Analysis/PyAna
                         Analysis/Ostap
                         Ex/LoKiExample
                         Ex/MicroDSTExample
                         Hlt/HltSelChecker
                         MicroDST/MicroDSTAlgorithm
                         MicroDST/MicroDSTBase
                         MicroDST/MicroDSTConf
                         MicroDST/MicroDSTInterfaces
                         MicroDST/MicroDSTTools
                         Phys/AnalysisConf
                         Phys/AnalysisPython
                         Phys/DSTWriterTests
                         Phys/DSTWriters
                         Phys/DaVinciAssociators
                         Phys/DaVinciMCTools
                         Phys/DaVinciMonitors
                         Phys/DaVinciTrackRefitting
                         Phys/DaVinciTrackScaling
                         Phys/DaVinciAngleCalculators
                         Phys/DecayTreeTuple
                         Phys/DecayTreeTupleBase
                         Phys/DecayTreeTupleDalitz
                         Phys/DecayTreeTupleJets
                         Phys/DecayTreeTupleMC
                         Phys/DecayTreeTupleMuonCalib
                         Phys/DecayTreeTupleReco
                         Phys/DecayTreeTupleTrigger
                         Phys/DecayTreeTupleTracking
                         Phys/DecayTreeTupleANNPID
                         Phys/DecayTreeTupleHerschel
                         Phys/DisplVerticesMC
                         Phys/FlavourTaggingChecker
                         Phys/JetAccessoriesMC
                         Phys/LoKiAlgoMC
                         Phys/LoKiGenMC
                         Phys/LoKiPhysMC
                         Phys/Particle2MCTruth
                         Phys/ParticleMakerChecker
                         Phys/TeslaTools
                         Phys/TrackSmearing
                         Phys/VertexFitCheck
                         Phys/CommonMCParticles
                         Rich/RichPIDQC
                         Tutorial/Analysis)
