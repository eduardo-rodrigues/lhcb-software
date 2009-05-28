# =============================================================================
# $Id: HltL0Lines.py,v 1.1.1.1 2009-05-28 15:37:38 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines which are plain L0 lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
 script to configure Hlt lines corresponding to L0 triggers
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1.1.1 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line   as Line
from HltLine.HltLine     import hlt1Lines
from Hlt1Lines.HltL0Candidates import *

class HltL0LinesConf(HltLinesConfigurableUser) :
   __slots__ = { 'Postscale' : { '.*' : 0.000001 } # set new default Postscale for these lines!
               , 'L0Channels' : []  # if empty, use all pre-defined channels
               }

   def __apply_configuration__(self):
        channels = self.getProp('L0Channels')
        if not channels : channels = L0Channels()
        for channel in channels :
            Line ( 'L0' + channel 
                 , prescale = self.prescale
                 , L0DU  = "L0_CHANNEL('"+channel+"')"
                 , algos = [ convertL0Candidates(channel) ]
                 , postscale = self.postscale
                 )
