import sys
sys.path.append('../python')
from PhysSelPython.Configurabloids import *

def test_instantiate_ConfigurableGenerator() :
    confMap = ConfigurableGenerator()

def test_ConfigurableGenerator_is_singleton() :
    confGen0 = ConfigurableGenerator()
    confGen1 = ConfigurableGenerator()
    assert confGen0.__dict__ == confGen1.__dict__

def test_clone_with_used_name_overwrites_original() :
    '''Mimic the bad behaviour of GaudiAlgorithm configurables'''
    dummy = DummyAlgorithm('Dummy00',
                           InputLocations = ['sel0', 'sel1'])
    clone = dummy.clone('Dummy00', InputLocations = ['clone0', 'clone1'])
    assert dummy.InputLocations == ['clone0', 'clone1']
    
def test_make_new_configurable() :
    dummy = DummyAlgorithm('Dummy01',
                           InputLocations = ['sel0', 'sel1'])
    assert dummy.name() == 'Dummy01'
    assert dummy.InputLocations == ['sel0', 'sel1']
    assert type(dummy) == DummyAlgorithm

def test_clone_alg_with_new_InputLocations() :
    alg = DummyAlgorithm('Alg',
                         InputLocations = ['loc0', 'loc1'])
    clone = alg.clone('Clone',
                      InputLocations = ['clone0', 'clone1', 'clone2'])

    assert alg.name() == 'Alg'
    assert len(alg.InputLocations) == 2
    assert alg.InputLocations.count('loc0') == 1
    assert alg.InputLocations.count('loc1') == 1

    assert clone.name() == 'Clone'
    assert len(clone.InputLocations) == 3
    assert clone.InputLocations.count('clone0') == 1
    assert clone.InputLocations.count('clone1') == 1
    assert clone.InputLocations.count('clone2') == 1


'''
def test_clone_alg_with_same_name_and_new_InputLocations_raises() :
    alg = DummyAlgorithm('Alg05',
                         InputLocations = ['loc0', 'loc1'])
    try :
        clone = alg.clone('Alg05',
                          InputLocations = ['clone0', 'clone1', 'clone2'])

        assert alg.name() == 'Alg05'
        assert len(alg.InputLocations) == 2
        assert alg.InputLocations.count('loc0') == 1
        assert alg.InputLocations.count('loc1') == 1

        assert clone.name() == 'Alg05'
        assert len(clone.InputLocations) == 3
        assert clone.InputLocations.count('clone0') == 1
        assert clone.InputLocations.count('clone1') == 1
        assert clone.InputLocations.count('clone2') == 1
    except KeyError:
        print 'KeyError caught'
'''
    
def test_change_DummyAlgorithm_InputLocations() :
    dummy0 = DummyAlgorithm('Dummy02',
                            InputLocations = ['sel0', 'sel1'])
    assert dummy0.name() == 'Dummy02'
    assert dummy0.InputLocations == ['sel0', 'sel1']
    assert type(dummy0) == DummyAlgorithm

    dummy0.InputLocations = ['sel2', 'sel3']
    assert dummy0.InputLocations == ['sel2', 'sel3']
    
def test_get_existing_DummyAlgorithm() :
    d = DummyAlgorithm('Dummy03', InputLocations = ['sel0', 'sel1'])
    dummy1 = DummyAlgorithm('Dummy03')
    assert dummy1.name() == 'Dummy03'
    assert dummy1.InputLocations == ['sel0', 'sel1']
    assert type(dummy1) == DummyAlgorithm

def test_get_existing_configurabloid_with_wrong_type_raises_TypeError() :
    confGen = ConfigurableGenerator()
    dummy1 = confGen('Dummy04', DummyAlgorithm)

    try :
        seq0 = confGen('Dummy04', DummySequencer)
    except TypeError :
        print "TypeError caught"

def test_get_existing_DummyAlgorithm_with_new_InputLocations() :
    confGen = ConfigurableGenerator()
    dummy = DummyAlgorithm('Dummy05',
                           InputLocations = ['sel0', 'sel1'])

    dummy1 = DummyAlgorithm('Dummy05',
                            InputLocations = ['clone0','clone1'])

    assert dummy1.name() == 'Dummy05'
    assert dummy1.InputLocations == ['clone0', 'clone1']
    assert type(dummy1) == DummyAlgorithm
