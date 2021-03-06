from Gaudi.Configuration  import *
from Configurables import (VeloSim, 
                           SiAmplifierResponse, 
                           SiGeantDepositedCharge)
from GaudiKernel.SystemOfUnits import ns

VSim = VeloSim('VeloSim')
VSim.InputContainers = [ "MC/Velo/Hits",
                         "Prev/MC/Velo/Hits", 
                         "PrevPrev/MC/Velo/Hits", 
                         "Next/MC/Velo/Hits" ]
VSim.InputContainerToLink = "MC/Velo/Hits"
VSim.OutputContainers = [ "MC/Velo/FE" ]
VSim.SimNoisePileUp = False # noise simulation is different for PU

VPUSim = VeloSim('VeloPUSim')
VPUSim.InputContainers = [ "MC/PuVeto/Hits",
                           "Prev/MC/PuVeto/Hits", 
                           "PrevPrev/MC/PuVeto/Hits", 
                           "Next/MC/PuVeto/Hits" ]
VPUSim.InputContainerToLink = "MC/PuVeto/Hits"
VPUSim.OutputContainers =  ["MC/Velo/FE", "MC/Velo/PuFE"] # L0 output for PU
VPUSim.SimNoisePileUp = True # noise simulation is different for PU
VPUSim.StripInefficiency = 0.01

for v in [ VSim, VPUSim ] : # always use same timing curves
    v.InputTimeOffsets = [ 0*ns, -25*ns, -50*ns, 25*ns ]
    v.PulseShapePeakTime = 30.
    v.OffPeakSamplingTime = 4.
    # Setup the amplifer response curves
    v.addTool( SiAmplifierResponse, "SiTimeTool")
    # Note times here are relative to the pulse shape peaking time (~44ns)
    # so used time  = 30 + sample time offset + bunch offset - time of flight
    # From Kazu plots provided 5/2/2010 : from TPScan65116.root
    v.SiTimeTool.times = [
        2.0 ,    3.0 ,    4.0 ,    5.0 ,    6.0 ,
        7.0 ,    8.0 ,    9.0 ,   10.0 ,   11.0 ,   12.0 ,   13.0 ,
        14.0 ,   15.0 ,   16.0 ,   17.0 ,   18.0 ,   19.0 ,   20.0 ,
        21.0 ,   22.0 ,   23.0 ,   24.0 ,   25.0 ,   26.0 ,   27.0 ,
        28.0 ,   29.0 ,   30.0 ,   31.0 ,   32.0 ,   33.0 ,   34.0 ,
        35.0 ,   36.0 ,   37.0 ,   38.0 ,   39.0 ,   40.0 ,   41.0 ,
        42.0 ,   43.0 ,   44.0 ,   45.0 ,   46.0 ,   47.0 ,   48.0 ,
        49.0 ,   50.0 ,   51.0 ,   52.0 ,   53.0 ,   54.0 ,   55.0 ,
        56.0 ,   57.0 ,   58.0 ,   59.0 ,   60.0 ,   61.0 ,   62.0 ,
        63.0 ,   64.0 ,   65.0 ,   66.0 ,   67.0 ,   68.0 ,   69.0 ,
        70.0 ,   71.0 ,   72.0 ,   73.0 ,   74.0 ,   75.0 ,   76.0 ,
        77.0 ,   78.0 ,   79.0 ,   80.0 ,   81.0 ,   82.0 ,   83.0 ,
        84.0 ,   85.0 ,   86.0 ,   87.0 ,   88.0 ,   89.0 ,   90.0 ,
        91.0 ,   92.0 ,   93.0 ,   94.0 ,   95.0 ,   96.0 ,   97.0 ,
        98.0 ,   99.0 ,  100.0 ,  101.0 ,  102.0 ,  103.0 ,  104.0 ,
        105.0 ,  106.0 ,  107.0 ,  108.0 ,  109.0 ]
    # fraction of signal seen in amplifier for this time offset
    # setup so that if sample time offset = 0 and TOF = |z/c| value = 1.0    
    v.SiTimeTool.values = [
        0.000 , -0.001 ,  0.001 ,  0.004 ,  0.011 ,
        0.023 ,  0.041 ,  0.065 ,  0.096 ,  0.136 ,  0.182 ,  0.234 ,
        0.290 ,  0.349 ,  0.411 ,  0.479 ,  0.553 ,  0.628 ,  0.699 ,
        0.760 ,  0.812 ,  0.857 ,  0.895 ,  0.929 ,  0.956 ,  0.976 ,
        0.990 ,  0.998 ,  1.000 ,  0.997 ,  0.988 ,  0.973 ,  0.952 ,
        0.927 ,  0.897 ,  0.865 ,  0.831 ,  0.796 ,  0.759 ,  0.719 ,
        0.675 ,  0.627 ,  0.576 ,  0.527 ,  0.482 ,  0.442 ,  0.404 ,
        0.366 ,  0.329 ,  0.294 ,  0.260 ,  0.230 ,  0.200 ,  0.170 ,
        0.139 ,  0.110 ,  0.085 ,  0.063 ,  0.044 ,  0.027 ,  0.011 ,
        -0.002 , -0.015 , -0.026 , -0.036 , -0.045 , -0.054 , -0.062 ,
        -0.068 , -0.071 , -0.074 , -0.075 , -0.075 , -0.075 , -0.075 ,
        -0.074 , -0.073 , -0.072 , -0.070 , -0.067 , -0.065 , -0.062 ,
        -0.058 , -0.055 , -0.051 , -0.047 , -0.044 , -0.041 , -0.039 ,
        -0.036 , -0.033 , -0.030 , -0.027 , -0.023 , -0.020 , -0.018 ,
        -0.016 , -0.014 , -0.012 , -0.010 , -0.008 , -0.007 , -0.006 ,
        -0.004 , -0.003 , -0.001 , -0.001 ,  0.000 ]
    # now check if we need to "correct" the GEANT charge desposition
    if( "SiGeantDepositedCharge" is v.DepChargeTool ):
        v.addTool(SiGeantDepositedCharge,"DepCharge")
        # GEANT stragging functions narrower than we see in data
        v.DepCharge.applySmearing = True
        # smear by 2.0 adc counts for 300micron track 
        # 601 is ADC to e-, 3.6e-6 is energy in MeV for one e- pair
        v.DepCharge.delta2 = (2.0 * 601. * 3.6e-6 / 0.3)**2 
        # fix so "old" and GEANT methods give rougly the same MPV
        v.DepCharge.scalingFactor = 1.10 # scale by 10% upward
    elif( "SiDepositedCharge" is v.DepChargeTool ):
        pass # "old" default
    else:
        raise RuntimeError("Failed to determine VeloSim deposited charge setting")


