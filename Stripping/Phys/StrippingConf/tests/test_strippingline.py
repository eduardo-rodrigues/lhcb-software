#!/usr/bin/env python
'''
 Test suite for StrippingLine configuration
'''
__author__ = 'Juan Palacios juan.palacios@nikhef.nl'

from py.test import raises

from StrippingConf.Configuration import StrippingLine, StrippingStream, StrippingConf

from PhysSelPython.Wrappers import AutomaticData

def test_instantiate_empty_line() :
    line = StrippingLine('EmptyLine')
    assert line.name() == 'StrippingEmptyLine'
    assert line.subname() == 'EmptyLine'
    assert line.isAppended() == False
    assert line.outputLocation() == None
    assert line.selection() == None
    
def test_instantiate_from_Selection_object() :
    selection = AutomaticData(Location = 'Phys/Selection/Particles')
    line = StrippingLine('SelectionLine', algos = [selection])
    assert line.name() == 'StrippingSelectionLine'
    assert line.subname() == 'SelectionLine'
    assert line.isAppended() == False
    assert line.outputLocation() == 'Phys/SelectionLine/Particles'
    assert line.selection() != None
    assert line.selection().outputLocation() == line.outputLocation()
    
def test_duplicate_name_raises_ValueError() :
    line0 = StrippingLine('DuplicateLine')
    raises(ValueError, StrippingLine, 'DuplicateLine' )


def test_filterMembers() :
    '''
    It is necessary to construct a StrippingStream and a StrippingConf
    to test the filterMembers of the line. The line by itself is useless.
    '''
    selection = AutomaticData(Location = 'Phys/Selection/Particles')
    name = 'FilterMembersTest'
    line = StrippingLine('FilterMembersTest', algos = [selection])
    assert line.name() == 'Stripping' + name
    assert line.subname() == name
    assert line.isAppended() == False
    assert line.outputLocation() == 'Phys/FilterMembersTest/Particles'
    stream = StrippingStream('stream', Lines = [line])
    conf = StrippingConf('conf', Streams = [stream])
    assert  [m.name() for m in line.filterMembers()] == ['checkPVmin1',
                                                         'SelFilterPhys_Selection_Particles',
                                                         'FilterMembersTest']

if '__main__' == __name__ :

    import sys

    test_names = filter(lambda k : k.count('test_') > 0, locals().keys())

    __tests = filter( lambda x : x[0] in test_names, locals().items())
    

    message = ''
    summary = '\n'
    length = len(sorted(test_names,
                        cmp = lambda x,y : cmp(len(y),len(x)))[0]) +2
    
    for test in __tests :
        try :
            test[1]()
            message = 'PASS'
        except :
            message = "FAIL"
        summary += test[0].ljust(length) + ':' + message.rjust(10) + '\n'

    if summary.count('FAIL') > 0 :
        message = 'FAIL'
        wr = sys.stderr.write
    else :
        message = 'PASS'
        wr = sys.stdout.write

    summary += 'Global'.ljust(length) + ':' + message.rjust(10) + '\n\n'
    wr(summary)
