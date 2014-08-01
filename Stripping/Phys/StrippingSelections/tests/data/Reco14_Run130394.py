#-- GAUDI jobOptions generated on Fri Aug  1 15:00:14 2014
#-- Contains event types : 
#--   90000000 - 460 files - 21615122 events - 2225.51 GBytes


#--  Extra information about the data processing phases:

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000350_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000351_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000352_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000354_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000355_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000356_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000358_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000359_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000360_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000361_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000362_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000363_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000364_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000365_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000366_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000367_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000368_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000369_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000370_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000371_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000373_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000374_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000375_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000376_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000377_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000378_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000379_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000380_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000381_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000382_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000383_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000384_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000385_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000386_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000387_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000388_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000389_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000390_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000392_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000393_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000394_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000395_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000396_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000397_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000398_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000399_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000400_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000401_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000403_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000404_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000405_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000406_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000407_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000408_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000409_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000410_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000411_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000412_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000413_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000415_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000417_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000418_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000419_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000420_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000421_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000422_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003884_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003886_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003888_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003889_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003890_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003891_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003892_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003893_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003895_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003896_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003897_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003898_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003899_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003900_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003901_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003902_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003903_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003904_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003905_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003906_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003907_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003908_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003909_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003910_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003911_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003912_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003913_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003914_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003915_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003916_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003917_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003918_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003919_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003920_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003921_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003922_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003924_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003926_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003927_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003928_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003929_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003930_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003931_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003932_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003933_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003934_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003935_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003936_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003937_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003938_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003939_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003941_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003942_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003943_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003944_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003945_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003946_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003947_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003948_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003949_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003950_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003952_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003953_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003955_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003956_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003957_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003958_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003959_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003960_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003961_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003964_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003965_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003966_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003967_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003968_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003969_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003970_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003971_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003972_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003973_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003975_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003976_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003977_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003978_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003979_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003980_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003981_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003982_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003983_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003984_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003985_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003986_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003987_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003988_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003989_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003990_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003991_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003992_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003993_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003994_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003995_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003996_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003997_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003998_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003999_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004000_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004001_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004002_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004003_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004004_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004005_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004006_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004007_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004008_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004009_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004010_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004011_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004012_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004013_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004014_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004015_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004016_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004017_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004019_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004020_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004022_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004023_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004024_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004025_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004030_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004031_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004032_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004033_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004034_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004037_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004038_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004039_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004040_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004041_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004042_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004043_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004044_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004045_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004046_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004047_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004048_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004049_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004050_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004053_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004055_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004056_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004057_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004058_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004059_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004061_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004062_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004063_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004067_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004068_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004069_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004070_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004071_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004072_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004073_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004074_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004075_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004076_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004077_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004078_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004079_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004080_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004081_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004082_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004083_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004084_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004088_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004089_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004090_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004091_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004092_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004093_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004094_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004095_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004096_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004097_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004098_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004099_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004101_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004102_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004103_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004104_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004105_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004106_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004108_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004109_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004110_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004111_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004112_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004113_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004114_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004115_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004116_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004117_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004118_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004119_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004120_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004121_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004122_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004123_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004124_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004125_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004126_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004127_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004129_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004130_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004131_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004132_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004133_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004134_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004135_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004136_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004137_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004138_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004140_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004141_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004142_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004143_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004144_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004145_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004146_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004147_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004148_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004149_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004151_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004152_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004153_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004154_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004155_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004156_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004157_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004158_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004159_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004160_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004161_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004162_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004163_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004164_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004165_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004166_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004167_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004168_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004169_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004170_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004172_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004173_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004174_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004175_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004176_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004177_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004178_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004179_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004181_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004183_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004184_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004185_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004186_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004187_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004188_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004190_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004191_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004192_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004193_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004194_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004195_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004196_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004197_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004198_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004200_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004201_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004203_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004204_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004205_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004206_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004207_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004208_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004211_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004212_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004213_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004214_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004215_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004217_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004219_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004220_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004222_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004223_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004224_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004225_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004226_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004227_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004228_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004229_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004232_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004233_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004235_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004236_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004237_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004238_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004239_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004241_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004242_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004243_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004244_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004245_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004246_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004247_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004248_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004249_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004250_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004251_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004252_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004253_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004254_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004255_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004256_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004257_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004259_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004260_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004261_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004262_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004263_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004264_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004265_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004266_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004267_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004268_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004269_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004270_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016638_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016749_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016752_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016757_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016777_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016793_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016836_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016868_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016923_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016930_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016948_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017007_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017008_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017009_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017136_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017186_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017204_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017255_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017256_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017257_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017265_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00037951_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00037952_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00037954_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038057_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038062_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038149_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038151_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038156_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038164_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038196_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038210_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038234_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038245_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038262_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038269_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038288_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038302_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038316_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038334_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038349_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038374_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038384_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038402_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038492_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038548_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038556_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038589_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038590_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038906_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038939_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00039077_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00039154_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0004/00020736_00040008_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0004/00020736_00040417_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0004/00020736_00040634_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0004/00020736_00042711_1.full.dst'
], clear=True)

FileCatalog().Catalogs = [ 'xmlcatalog_file:Reco14_Run130394.xml' ]
