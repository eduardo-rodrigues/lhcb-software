#-- GAUDI jobOptions generated on Fri Aug  1 14:56:16 2014
#-- Contains event types : 
#--   90000000 - 68 files - 3010194 events - 310.18 GBytes


#--  Extra information about the data processing phases:

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000423_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000424_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000425_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000426_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000427_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000428_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000429_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000430_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000432_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000434_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000435_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000436_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000437_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004273_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004274_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004275_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004276_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004277_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004278_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004279_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004280_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004281_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004282_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004283_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004284_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004285_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004287_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004288_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004289_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004290_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004291_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004292_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004293_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004294_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004295_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004296_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004297_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004298_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004299_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004301_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004302_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004303_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004304_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004305_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004306_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004307_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004308_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004309_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004310_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004311_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004312_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004313_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004314_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004315_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004317_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004319_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004320_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004321_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004322_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004323_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004324_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004325_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016682_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016888_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017010_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017230_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038118_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038644_1.full.dst'
], clear=True)

FileCatalog().Catalogs = [ 'xmlcatalog_file:Reco14_Run130396.xml' ]
