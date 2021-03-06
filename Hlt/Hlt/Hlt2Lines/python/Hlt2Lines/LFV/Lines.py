""" Hlt2 trigger lines for lepton flavour violating decays.

    Authors: Johannes Albrecht <johannes.albrecht@cern.ch>
             Kevin Dungs <kevin.dungs@cern.ch>
"""

from GaudiKernel.SystemOfUnits import MeV
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
from HltLine.HltLine import Hlt2Line


class LFVLines(Hlt2LinesConfigurableUser):
    m_jpsi = 3096
    __slots__ = {'Prescale': {},
                 'Common': {},
                 'JpsiMuE': {'CombMassHigh': (m_jpsi + 550) * MeV,
                             'CombMassLow': (m_jpsi - 1100) * MeV,
                             'ElectronProbNn': 0.93,
                             'ElectronTrChi2DoF': 3,
                             'ElectronTrGhostProb': 0.2,
                             'MassHigh': (m_jpsi + 500) * MeV,
                             'MassLow': (m_jpsi - 1000) * MeV,
                             'MuonProbNn': 0.9,
                             'MuonTrChi2DoF': 3,
                             'MuonTrGhostProb': 0.1,
                             'VertexChi2DoF': 3},
                 'SpdCut': {'NSPD': 250}}

    def __apply_configuration__(self):
        from Stages import SelJpsiMuE
        stages = {'LFVJpsiMuETurbo': [SelJpsiMuE]}

        for name, algos in self.algorithms(stages):
            Hlt2Line(name,
                     L0DU="(L0_DATA('Spd(Mult)') < {})".format(self.getProp('SpdCut')['NSPD']),
                     prescale=self.prescale,
                     algos=algos,
                     postscale=self.postscale,
                     Turbo=True)
