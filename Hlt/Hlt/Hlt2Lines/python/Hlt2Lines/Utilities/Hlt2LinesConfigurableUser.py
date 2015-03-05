from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt2LinesConfigurableUser(HltLinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale'  : {},
                 'Postscale' : {}}
            
    def algorithms(self, stages):
        from copy import deepcopy
        algos = []
        # Get a local instance of the cuts
        common = self.getProps().get('Common', {})
        cuts = deepcopy(self.getProps())
        for k, v in cuts.iteritems():
            if k != 'Common': v.update(common)

        from HltLine.HltLine import bindMembers
        def __flatten(l):
            for i in l:
                if isinstance(i, bindMembers):
                    for j in __flatten(i.members()): yield j
                else:
                    yield i
                
        sd = {}
        from Hlt2Lines.Utilities.Utilities import uniqueEverseen
        for name, stages in stages.iteritems():
            sl = []
            for i in stages:
                if hasattr(i, 'stages'):
                    sl.extend(i.stages(cuts))
                else:
                    sl.append(i)
            sd[name] = list(uniqueEverseen(__flatten(sl)))
        # import pdb; pdb.set_trace()
        return sd

