"""
Module with stripping selection line builder modules.
All line builders available via function lineBuilders().
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import StrippingB2DX
import StrippingBs2JpsiPhiPrescaledAndDetatched
import StrippingB2hhLTUnbiased
import StrippingNeuroBayesMuMu
import StrippingB2nbody
import StrippingHb2Charged2Body
import StrippingBd2DstarMuNu
import StrippingB0q2DplusMuX
import StrippingBd2DstarTauNu
import StrippingB2XuMuNu
import StrippingB2DMuNuX
import StrippingBu2D0h_D02KShh_NoPID
import StrippingBu2D0h_D02KShh_NoPID_WS
import StrippingNoPIDDstarWithD02RSKPi
import StrippingInclPhi
import StrippingV0ForPID
import StrippingB2JpsiXforBeta_s
import StrippingB2Psi2SX
import StrippingB2Psi2SXMuMu
import StrippingBs2JpsieePhi
import StrippingB2CharmlessQuasi2Body
import StrippingBs2PhiPhi
import StrippingBs2Kst0Kst0
import StrippingBs2PhiKst0
import StrippingBs2EtacPhi
import StrippingBs2ChicPhi_Chic2KKPiPi
import StrippingBs2ChicPhi_Chic2PiPiPiPi

import StrippingBs2MuMuPhi
import StrippingB2XMuMu
import StrippingBd2KstarMuMu
import StrippingB2XMuMuSS
import StrippingTriMuons
import StrippingB2XGamma
import StrippingBs2MuMuLines
import StrippingB2MuMuMuMuLines
import StrippingBu2LLK
import StrippingBd2eeKstar
import StrippingBd2JpsieeKstar

import StrippingDiMuonNew
import StrippingBc2JpsiHNew
import StrippingJpsiMuMuforD0MuMu
import StrippingBuToKX3872
import StrippingSingleTrackTIS
import StrippingDiElectronNew
import StrippingElectronID
import StrippingCcbar2PpbarNew
import StrippingDForBSemi
import StrippingD0ForBXX
import StrippingJpsippForD0MuMu

import StrippingZ02MuMu
import StrippingZ02ee
import StrippingWMu
import StrippingWe
import StrippingDY2MuMu
import StrippingDY2ee
import StrippingMuMuSS
import StrippingLowMult
import StrippingDiPhotonDiMuon
import StrippingPromptCharm

import StrippingDisplVertices

import StrippingMiniBias

from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

from StrippingUtils.Utils import getLineBuildersFromModule as _getter

_lineBuilders = {}

for _sm in _strippingModules :
    _lineBuilders.update(_getter(_sm))

def lineBuilders() :
    """
    Return all the line builders in the module.
    """
    return dict(_lineBuilders)
