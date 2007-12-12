#! /usr/bin/env python

import gaudimodule
from math import *
# from ROOT import TH1F

# define options files and datacards
HLTJOB = "$HLTSYSROOT/options/HltJob.opts"
MINBIAS = "$HLTSYSROOT/optionss/raw06_minbias_1.opts"
BKPISEL = "$HLTSYSROOT/options/dst06_bkpi_sel.opts"
BSMUMUSEL = "$HLTSYSROOT/options/dst06_bsmumu_sel.opts"

# crete application and initialize
gaudi = gaudimodule.AppMgr(outputlevel=3)
gaudi.config(files = [HLTJOB,BKPISEL])
gaudi.initialize()

# create tools
HLTSUM = gaudi.toolsvc().create("HltSummaryTool",interface="IHltConfSummaryTool")
HLTFAC = gaudi.toolsvc().create("HltFunctionFactory",interface="IHltFunctionFactory")

# STDVectorVertices = gaudimodule.gbl.std.vector("LHCb::RecVertex *")
# STDVectorTrack = gaudimodule.gbl.std.vector("LHCb::Track *") 

TES = gaudi.evtsvc()

def info(name):
    """ returns a function to retrieve the info from the object
    """
    varname = name.split(",")[0]
    id = HLTSUM.confInt("InfoID/"+varname)
    def fun(x):
        return x.info(id,-1)
    return fun

def candidates(selection):
    """ return the candidates of a selection (tracks or vertices)
    """
    if (not HLTSUM.hasSelection(selection)): return None
    type = HLTSUM.confString(selection+"/SelectionType")
    if (type == "Tracks"):
        return HLTSUM.selectionTracks(selection)
    elif (type == "Vertices"):
        return HLTSUM.selectionVertices(selection)
    return None

def l0():
    """ how to get the l0 decision?
        how to get the l0 decision per channel (i.e hadron high threshold)
        hot to get the l0 decision per condition (i.e SPD multiplicity)
    """
    l0 = TES["Trig/L0/L0DUReport"]
    print " L0 decision ",l0.decision()
                 


def l0_candidates():
    """ hot to get the l0 candidates?
    """
    calos = TES["Trig/L0/FullCalo"]
    if (calos):
        hadcalos = filter(lambda calo: (calo.type() == 2),calos)
        print " L0 hadron calo candidates ",len(hadcalos)
        
    muons = TES["Trig/L0/Muon"]
    if (muons):
        print " L0 muon candidates ",muons.size()

def hlt():
    """ how to get the hlt decision?
        how to get the decision of a selection?
        how to get the selections that an event has passed?
    """
    print " HLT decision ", HLTSUM.decision()
    names = ["HadL0Entry","HadPreTriggerSingle","HadTrigger"]
    for name in names:
        print " \t",name," ? ",HLTSUM.selectionDecision(name)

    names = HLTSUM.selections()
    print " HLT selections passed:"
    for name in names: print " \t",name

def hlt_selection_configuration(selection):
    """ how to get the filters applied in one selection?
        how to get the input selection used for a selection?
    """
    filters = HLTSUM.confStringVector(selection+"/Filters")
    print " HLT ",selection ," Filters : "
    for filter in filters: print " \t",filter
    inputs = HLTSUM.confStringVector(selection+"/InputSelections")
    print " HLT ",selection ," Inputs : "
    for input in inputs: print " \t",input

def hlt_candidates_info(selection = "HadPreTriggerSingle"):
    """ how to get the info of the candidates of a selection?
        i.e what is the PT value of the tracks of the HadPreTrigger selection?
    """
    print " HLT ",selection," ? ",HLTSUM.selectionDecision(selection)
    if (not HLTSUM.hasSelection(selection)): return
    objs = candidates(selection)
    if (not objs): return
    print " HLT ",selection," candidates ",objs.size()
    filters = HLTSUM.confStringVector(selection+"/Filters")
    for filter in filters:
        xfun = info(filter)
        vars = map(xfun,objs)
        s = ""
        for var in vars: s = s+" "+str(var)
        print "\t",filter," : ",s

def run(n=10):
    """ run 10 events and print info about L0 and HLT
    """
    for i in range(n):
        print " *** L0 and HLT info *** "
        gaudi.run(1)
        l0()
        l0_candidates()
        hlt()
        hlt_candidates_info("HadPreTriggerSingle")
        hlt_candidates_info("HadTrigger")
    print " *** configuration  *** "
    hlt_selection_configuration("HadPreTriggerSingle")
    hlt_selection_configuration("HadTrigger")
