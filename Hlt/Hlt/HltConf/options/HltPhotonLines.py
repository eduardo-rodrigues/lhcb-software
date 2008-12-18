#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltPhotonLines.py,v 1.12 2008-12-18 14:03:47 graven Exp $
# =============================================================================
## @file
#  Configuration of Photon Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
'''
 script to configure Photon trigger lines
'''
# =============================================================================
__author__  = 'Gerhard Raven Gerhard.Raven@nikhef.nl'
__version__ = 'CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.12 $'
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import PatMatchTool
from Configurables import HltL0CaloPrepare

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 

#//---------------------------
#// HLT Photon Alley
#//--------------------------
from HltConf.HltL0Candidates import *
L0CandidateConverter = convertL0Candidates('Photon')
L0Candidates = HltL0Candidates('Photon')

Line ('Photon' 
      , L0DU = "L0_CHANNEL('Photon')"
      , algos = 
      [ L0CandidateConverter
      , Member ('TF', 'Photon' 
                , InputSelection     = L0Candidates
                , FilterDescriptor = ['IsPhoton,>,-0.1','L0ET,>,2800.'])
      , GaudiSequencer('Hlt1RecoRZVeloSequence')
      , Member ('TF', 'RZVelo'
               , InputSelection     = 'RZVelo'
               , FilterDescriptor = ['rIP_PV2D,||[],0.10,3.0']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { 'rIP' : ('rIP',-1.,3.,400), 'rIPBest' : ('rIPBest',-1.,3.,400)}
               )
      , Member ('TU', 'Velo' , RecoName = 'Velo') #  Velo Reco
      , Member ('TF', 'Velo' #  3D IP selection
               , FilterDescriptor = ['IP_PV2D,||[],0.15,3.0']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
               )
      , GaudiSequencer('Hlt1RecoSequence')
      , Member ('TF', 'SecondVelo' 
               , InputSelection     = 'Velo'
               , FilterDescriptor = ['IP_PV2D,||[],0.15,3.0']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
               )
      , Member ('TU' ,'Forward' , RecoName = 'Forward') #  upgrade to Forward 
      , Member ('TF', 'Forward' #  Pt cut (call it pretrigger sice could not implement veloTT)
               , FilterDescriptor = ['PT,>,650.']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
               )
      , Member ('VM2', 'DiTrack' # two track vertex: DOCA
               , InputSelection1 = '%TFForward'
               , InputSelection2 = '%TFSecondVelo'
               , FilterDescriptor = [ 'DOCA,<,0.2']
               , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
               )
      , Member ('VF', 'DiTrack' #two track vertex: DZ
               , FilterDescriptor = [ 'VertexDz_PV2D,>,0.']
               , HistoDescriptor = { 'VertexDz_PV2D':('VertexDz_PV2D',-3.,3.,100), 'VertexDz_PV2DBest':('VertexDz_PV2D',-3.,3.,100) }
               )
      , Member ('VU', 'DiTrack' , RecoName = 'Forward')
      , Member ('VF', 'SecondDiTrack' 
               , FilterDescriptor = [ 'VertexMinPT,>,650.']
               )
      , Member ( 'AddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
               , InputSelection1 = '%VFSecondDiTrack'
               , InputSelection2 = '%TFPhoton'
               , OutputSelection = '%Decision'
               )
      ] )

from Configurables import L0ConfirmWithT

#L0CandidateConverter = convertL0Candidates('Electron')
#L0Candidates = L0CandidateConverter.OutputSelection
Line ('PhoFromEle' 
      , L0DU = "L0_CHANNEL('Electron')"
      , algos = 
      [ HltL0CaloPrepare('L0PhoFromEleDecision', CaloType = 'Electron', MinEt = 2800.0 )
      , Member ('TF', 'AntiEle'
               , InputSelection = 'L0PhoFromEleDecision'
               , FilterDescriptor = ['AntiEleConf,>,0.5'] 
               , tools = [ Tool( L0ConfirmWithT, particleType = 2 )]
               )
      , Member ('TF', 'Photon'
               , FilterDescriptor = ['IsPhoton,>,-0.1']
               )
      , GaudiSequencer('Hlt1RecoRZVeloSequence')
      , Member ('TF', 'RZVelo'
               , InputSelection     = 'RZVelo'
               , FilterDescriptor = ['rIP_PV2D,||[],0.10,3.0']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { 'rIP' : ('rIP',-1.,3.,400), 'rIPBest' : ('rIPBest',-1.,3.,400)}
               )
      , Member ('TU', 'Velo' , RecoName = 'Velo') #  Velo Reco
      , Member ('TF', 'Velo' #  3D IP selection
               , FilterDescriptor = ['IP_PV2D,||[],0.15,3.0']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
               )
      , GaudiSequencer('Hlt1RecoSequence')
      , Member ('TF', 'SecondVelo' 
               , InputSelection     = 'Velo'
               , FilterDescriptor = ['IP_PV2D,||[],0.15,3.0']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
               )
      , Member ('TU' ,'Forward' , RecoName = 'Forward') #  upgrade to Forward 
      , Member ('TF', 'Forward' #  Pt cut (call it pretrigger sice could not implement veloTT)
               , FilterDescriptor = ['PT,>,650.']
               , HistogramUpdatePeriod = 0
               , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
               )
      , Member ('VM2', 'DiTrack' # two track vertex: DOCA
               , InputSelection1 = '%TFForward'
               , InputSelection2 = '%TFSecondVelo'
               , FilterDescriptor = [ 'DOCA,<,0.2']
               , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
               )
      , Member ('VF', 'DiTrack' #two track vertex: DZ
               , FilterDescriptor = [ 'VertexDz_PV2D,>,0.']
               , HistoDescriptor = { 'VertexDz_PV2D':('VertexDz_PV2D',-3.,3.,100), 'VertexDz_PV2DBest':('VertexDz_PV2D',-3.,3.,100) }
               )
      , Member ('VU', 'DiTrack' , RecoName = 'Forward')
      , Member ('VF', 'SecondDiTrack' 
               , FilterDescriptor = [ 'VertexMinPT,>,650.']
               )
      , Member ( 'AddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
               , InputSelection1 = '%VFSecondDiTrack'
               , InputSelection2 = '%TFPhoton'
               , OutputSelection = '%Decision'
               )
      ] )
