# =============================================================================
# $Id: benderrange.py,v 1.1 2006-10-11 14:45:11 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is helper module for decoration of various LoKi.Range functions """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
## @file
#
# This is helper module for decoration of various LoKi.Range functions
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

_gbl  = gaudimodule.gbl
_LoKi = _gbl.LoKi


_Base = _LoKi.RangeBase_

## helper adapter fuction to provide python semantic : len(object)
def _len_  ( self ) :
    """
    Helper adapter fuction to provide python semantic : len(object)
    """
    return self.size()

_Base.__len__ = lambda s: _len_(s) 

## helper fuction for iteration over the range
def _iter_ ( self ) :
    """
    Helper fuction for iteration over the range
    """
    _r   = self
    _num = _r.size()
    _i   = 0 
    while _i < _num :
        yield _r.at(_i)
        _i += 1

_Base.__iter__ = lambda s: _iter_(s) 


_Range = _LoKi.NamedRange_

Range    = _Range ( 'std::vector<const LHCb::Particle*>'     )
VRange   = _Range ( 'std::vector<const LHCb::VertexBase*>'   )
MCRange  = _Range ( 'std::vector<const LHCb::MCParticle*>'   )
MCVRange = _Range ( 'std::vector<const LHCb::MCVertex*>'     )
GRange   = _Range ( 'std::vector<const HepMC::GenParticle*>' )
GVRange  = _Range ( 'std::vector<const HepMC::GenVertex*>'   )

_ranges_ = ( Range , VRange , MCRange , MCVRange , GRange , GVRange )

print "dir(%s) : %s" % ( __name__ , dir() ) 
