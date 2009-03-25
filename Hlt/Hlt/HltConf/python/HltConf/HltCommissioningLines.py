# =============================================================================
# $Id: HltCommissioningLines.py,v 1.6 2009-03-25 09:35:40 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for commissioning
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
 script to configure Hlt lines for commissioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.6 $"
# =============================================================================

from HltConf.HltLinesConfigurableUser import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from Configurables   import HltIncidentFilter


class HltCommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt1Physics'  : 1 # update default prescales for these lines
                              , 'Hlt1Random'   : 1 # @OnlineEnv.AcceptRate
                              , 'Hlt1L0Forced' : 0 
                              }
               }
   def __apply_configuration__(self):
        Line('Physics' ,  ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Random' ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'
            , prescale = self.prescale
            , postscale = self.postscale
            )
        #Line('L0Forced', L0DU = 'L0_FORCEBIT'
        #    , prescale = self.prescale
        #    , postscale = self.postscale
        #    )
        Line('RawBankConversion'
            , ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
            , algos = [ HltTriggerRawConversion() ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Incident'
            , ODIN = 'ODIN_ALL'
            , algos = [ HltIncidentFilter() ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
