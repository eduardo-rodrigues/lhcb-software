#!/usr/bin/env python
# =============================================================================
# $Id: MC.py,v 1.3 2007-08-16 11:54:20 ibelyaev Exp $ 
# =============================================================================
## @file
#  collection of utilities for useful 'decoration' of MC-objects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-08-11 
# =============================================================================
"""
Collection of utilities for useful 'decoration' of MC-objects
"""
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

from   LoKiMC.functions import LoKi,LHCb

# =============================================================================
## Get number of child particles :
def nChildren ( p ) :
    """
    Get the number of child particles :

    >>> n = nChildren ( p )
    
    """
    return LoKi.MCChild.nChildren ( p )

nChildren . __doc__  +=  "\n\n" + LoKi.MCChild.nChildren    . __doc__ 

if not hasattr ( LHCb.MCParticle ,     'nChildren'    ) :
    LHCb.MCParticle  . nChildren      = nChildren
if not hasattr ( LHCb.MCParticle ,     'nDaughters'   ) :
    LHCb.MCParticle  . nDaughters     = nChildren
if not hasattr ( LHCb.MCParticle ,   '__nChildren__'  ) :
    LHCb.MCParticle  . __nChildren__  = nChildren
if not hasattr ( LHCb.MCParticle ,   '__nDaughters__' ) :
    LHCb.MCParticle  . __nDaughters__ = nChildren

# =============================================================================
## get the child 
def child ( p , *a ) :
    """
    Get the child for 'particle'
    
    >>> p = ...
    >>> d1  = child ( p , 1 ) 
    >>> d12 = child ( p , 1 , 2 )
    
    ATTTENTION: indices start from 1.
    Index 0 corresponds to the particle itself.
    """
    return LoKi.MCChild.child ( p , *a )

child . __doc__  +=  "\n\n" + LoKi.MCChild.child    . __doc__ 

if not hasattr ( LHCb.MCParticle ,   'child'      ) : LHCb.MCParticle  .   child      = child 
if not hasattr ( LHCb.MCParticle , '__child__'    ) : LHCb.MCParticle  . __child__    = child 
if not hasattr ( LHCb.MCParticle ,   'daughter'   ) : LHCb.MCParticle  .   daughter   = child 
if not hasattr ( LHCb.MCParticle , '__daughter__' ) : LHCb.MCParticle  . __daughter__ = child 


# =============================================================================
## Get all children from the given LHCb::MCParticle 
def children ( o , *a ) :
    """
    Get all children from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in children ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.children ( o , *a )

children . __doc__ += "\n\n" + LoKi.MCChild.children . __doc__  

if not hasattr ( LHCb.MCParticle ,   'children'    ) : LHCb.MCParticle .   children    =  children
if not hasattr ( LHCb.MCParticle , '__children__'  ) : LHCb.MCParticle . __children__  =  children
if not hasattr ( LHCb.MCParticle ,   'daughters'   ) : LHCb.MCParticle .   daughters   =  children
if not hasattr ( LHCb.MCParticle , '__daughters__' ) : LHCb.MCParticle . __daughters__ =  children


# =============================================================================
## Get all descendants from the given LHCb::MCParticle 
def descendants ( o , *a ) :
    """
    Get all descendants from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in descendants ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.descendants ( o , *a )

descendants . __doc__ += "\n\n" + LoKi.MCChild.descendants . __doc__  

if not hasattr ( LHCb.MCParticle ,   'descendants'   ) :
    LHCb.MCParticle .   descendants    =  descendants
if not hasattr ( LHCb.MCParticle , '__descendants__' ) :
    LHCb.MCParticle . __descendants__  =  descendants

# =============================================================================
## Get all ancestors from the given LHCb::MCParticle 
def ancestors ( o ) :
    """
    Get all ancestors from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in ancestors ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.ancestors ( o )

ancestors . __doc__ += "\n\n" + LoKi.MCChild.ancestors . __doc__  

if not hasattr ( LHCb.MCParticle ,   'ancestors'   ) :
    LHCb.MCParticle .   ancestors    =  ancestors
if not hasattr ( LHCb.MCParticle , '__ancestors__' ) :
    LHCb.MCParticle . __ancestors__  =  ancestors

# =============================================================================
## Get all (0 or 1) parents  from the given LHCb::MCParticle 
def parents ( o ) :
    """
    Get all parents from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in parents ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.parents ( o )

parents . __doc__ += "\n\n" + LoKi.MCChild.parents . __doc__  

if not hasattr ( LHCb.MCParticle ,   'parents'   ) : LHCb.MCParticle .   parents    = parents
if not hasattr ( LHCb.MCParticle , '__parents__' ) : LHCb.MCParticle . __parents__  = parents

# =============================================================================
## Get the mother particle:
def mother ( p ) :
    """
    Get the mother particle:

    >>> p = ...
    >>> m = mother ( p )
    
    """
    return LoKi.MCChild.mother ( p )

mother . __doc__ += "\n\n" + LoKi.MCChild.mother . __doc__  

if not hasattr ( LHCb.MCParticle ,   'mother'   ) : LHCb.MCParticle .   mother     = mother
if not hasattr ( LHCb.MCParticle , '__mother__' ) : LHCb.MCParticle . __mother__   = mother


# =============================================================================
## Get certain MC-particles form the decay tree:
def mcParticles ( p , *a ) :
    """
    Get certain MC-particles form the decay tree:

    >>> p = ...
    >>> daughterPions = mcParticles ( p , 'pi+' == MCABSID , True )

    """
    return LoKi.Dicts.Extract.mcParticles ( p , *a )

mcParticles . __doc__ += "\n\n" + LoKi.Dicts.Extract.mcParticles . __doc__  

getParticles = mcParticles 
particles    = mcParticles
extract      = mcParticles

# =============================================================================
## Count number of 'interesting' elements in the decay tree or container,
def count_if ( s , *a ) :
    """
    Count number of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> number_of_pions = count_if ( o , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.count_if ( s , *a )

count_if . __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.count_if . __doc__  

# =============================================================================
## Check the presence of 'interesting' elements in the decay tree or container,
def found ( s , *a ) :
    """
    check the presence of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> good = found ( o , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.found ( s , *a )

found . __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.found . __doc__  


# =============================================================================
## Accumulate through the decay tree or container,
def accumulate  ( s , *a ) :
    """
    Accumulate through the decay tree or container,

    >>> o = ...
    >>> ptOfAllPions= accumulate ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.accumulate ( s , *a )

accumulate. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.accumulate . __doc__  


# =============================================================================
## Find the minimal value through decay tree or  container  
def min_value ( s , *a ) :
    """
    Find the minimal value through decay tree or  container  

    >>> o = ...
    >>> minPT = min_value ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.min_value ( s , *a )

min_value. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.min_value . __doc__  

# =============================================================================
## Find the maximal value through decay tree or  container  
def max_value ( s , *a ) :
    """
    Find the maximal value through decay tree or  container  

    >>> o = ...
    >>> maxPT = max_value ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.max_value ( s , *a )

max_value. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.max_value . __doc__  

# =============================================================================
## Find the minimal element through decay tree or  container  
def min_element ( s , *a ) :
    """
    Find the minimal element through decay tree or  container  

    >>> o = ...
    >>> pion = min_element ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.min_element ( s , *a )

min_element. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.min_element . __doc__  


# =============================================================================
## Find the maximal element through decay tree or  container  
def max_element ( s , *a ) :
    """
    Find the maximal element through decay tree or  container  

    >>> o = ...
    >>> pion = max_element ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.max_element ( s , *a )

max_element. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.max_element . __doc__  

for t in ( LHCb.MCParticle , LHCb.MCVertex   ) :
    
    #@ MCAlgs:
    if not hasattr ( t ,   'count_if'       ) : t. count_if        = count_if  
    if not hasattr ( t , '__count_if__'     ) : t. __count_if__    = count_if  
    if not hasattr ( t ,   'found'          ) : t.   found         = found 
    if not hasattr ( t , '__found__'        ) : t. __found__       = found  
    if not hasattr ( t ,   'accumulate'     ) : t.   accumulate    = accumulate
    if not hasattr ( t , '__accumulate__'   ) : t. __accumulate__  = accumulate  
    if not hasattr ( t ,   'min_value'      ) : t.   min_value     = min_value
    if not hasattr ( t , '__min_value__'    ) : t. __min_value__   = min_value
    if not hasattr ( t ,   'max_value'      ) : t.   max_value     = max_value
    if not hasattr ( t , '__max_value__'    ) : t. __max_value__   = max_value
    if not hasattr ( t ,   'min_element'    ) : t.   min_element   = min_element
    if not hasattr ( t , '__min_element__'  ) : t. __min_element__ = min_element
    if not hasattr ( t ,   'max_element'    ) : t.   max_element   = max_element
    if not hasattr ( t , '__max_element__'  ) : t. __max_element__ = max_element
    
    if not hasattr ( t ,   'mcParticles'    ) : t.   mcParticles    = mcParticles 
    if not hasattr ( t , '__mcParticles__'  ) : t. __mcParticles__  = mcParticles 
    if not hasattr ( t ,   'getParticles'   ) : t.   getParticles   = mcParticles 
    if not hasattr ( t , '__getParticles__' ) : t. __getParticles__ = mcParticles 
    if not hasattr ( t ,   'particles'      ) : t.   particles      = mcParticles 
    if not hasattr ( t , '__particles__'    ) : t. __particles__    = mcParticles 
    if not hasattr ( t ,   'extract'        ) : t.   extract        = mcParticles 
    if not hasattr ( t , '__extract__'      ) : t. __extract__      = mcParticles 


# =============================================================================
## print the decay  
def printDecay ( s , *a ) :
    """
    Print the decay of LHCb::MCParticle

    >>> p = ...
    >>> printDecay ( p )
    
    """
    return LoKi.Print.printMCDecay ( s , *a )

printDecay. __doc__ += "\n\n" + LoKi.Print.printDecay . __doc__  

if not hasattr ( LHCb.MCParticle ,     'printDecay'   ) :
    LHCb.MCParticle .  printDecay   =   printDecay
if not hasattr ( LHCb.MCParticle ,   '__printDecay__' ) :
    LHCb.MCParticle. __printDecay__ =   printDecay


import LoKiCore.functions  as _LCF 

_LCF.nChildren     . __doc__ += "\n" + LoKi.MCChild.nChildren         . __doc__ 
_LCF.child         . __doc__ += "\n" + LoKi.MCChild.child             . __doc__
_LCF.daughter      . __doc__ += "\n" + LoKi.MCChild.child             . __doc__ 
_LCF.parents       . __doc__ += "\n" + LoKi.MCChild.parents           . __doc__
_LCF.children      . __doc__ += "\n" + LoKi.MCChild.children          . __doc__
_LCF.daughters     . __doc__ += "\n" + LoKi.MCChild.children          . __doc__ 
_LCF.descendants   . __doc__ += "\n" + LoKi.MCChild.descendants       . __doc__ 
_LCF.ancestors     . __doc__ += "\n" + LoKi.MCChild.ancestors         . __doc__ 

_LCF.extract       . __doc__ += "\n" + LoKi.Dicts.Extract.mcParticles . __doc__ 
_LCF.particles     . __doc__ += "\n" + LoKi.Dicts.Extract.mcParticles . __doc__ 
_LCF.getParticles  . __doc__ += "\n" + LoKi.Dicts.Extract.mcParticles . __doc__ 

_LCF.count_if      . __doc__ += "\n" + LoKi.Dicts.MCAlgs.count_if     . __doc__ 
_LCF.found         . __doc__ += "\n" + LoKi.Dicts.MCAlgs.found        . __doc__ 
_LCF.accumulate    . __doc__ += "\n" + LoKi.Dicts.MCAlgs.accumulate   . __doc__ 
_LCF.min_value     . __doc__ += "\n" + LoKi.Dicts.MCAlgs.min_value    . __doc__ 
_LCF.max_value     . __doc__ += "\n" + LoKi.Dicts.MCAlgs.max_value    . __doc__ 
_LCF.min_element   . __doc__ += "\n" + LoKi.Dicts.MCAlgs.min_element  . __doc__ 
_LCF.max_element   . __doc__ += "\n" + LoKi.Dicts.MCAlgs.max_element  . __doc__ 


_LCF.printDecay    . __doc__ += "\n" + LoKi.Print.printMCDecay        . __doc__ 


# =============================================================================
if '__main__' == __name__ :
    print __doc__ , '\n' , __author__
    for i in dir() : print i 
    

# =============================================================================
# The END
# =============================================================================
