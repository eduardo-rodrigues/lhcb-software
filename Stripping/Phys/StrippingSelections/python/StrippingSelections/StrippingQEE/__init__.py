"""
Module importing stripping selection line builder modules
for QEE WG.
"""


_selections = (
  'StrippingDisplVertices',
  'StrippingH24MuLines',
  'StrippingInclbJets',
  'StrippingLowMultINC',
  'StrippingMuMuSS',
  'StrippingSingleTrackTIS',
  'StrippingWMu',
  'StrippingWmuAKTJets',
  'StrippingWeAKTJets',
  'StrippingZ02MuMu',
)

for _sel in _selections :  
  try :
    __import__( '%s.%s'  % ( __name__, _sel ) )
  except Exception, x:
    print '[WARNING] Submodule %s.%s raises the excetpion "%s" and will be skipped !' % ( __name__,_sel,x )

_strippingModules = [ val for key,val in dict(locals()).iteritems() if key.startswith('Stripping') ]
