    
C*********************************************************************  
    
      BLOCK DATA HILUDATA 
      IMPLICIT REAL*8(A-H,O-Z)
   
C...Purpose: to give default values to parameters and particle and  
C...decay data. 
      COMMON/HILUDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200) 
      SAVE /HILUDAT1/ 
      COMMON/HILUDAT2/KCHG(500,3),PMAS(500,4),PARF(2000),VCKM(4,4)    
      SAVE /HILUDAT2/ 
      COMMON/HILUDAT3/MDCY(500,3),MDME(2000,2),BRAT(2000),KFDP(2000,5)    
      SAVE /HILUDAT3/ 
      COMMON/HILUDAT4/CHAF(500)   
      CHARACTER CHAF*8  
      SAVE /HILUDAT4/ 
      COMMON/HILUDATR/MRLU(6),RRLU(100)   
      SAVE /HILUDATR/ 
    
C...LUDAT1, containing status codes and most parameters.    
      DATA MSTU/    
     &    0,    0,    0, 9000,10000,  500, 2000,    0,    0,    2,  
     1    6,    1,    1,    0,    1,    1,    0,    0,    0,    0,  
     2    2,   10,    0,    0,    1,   10,    0,    0,    0,    0,  
     3    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  
     4    2,    2,    1,    4,    2,    1,    1,    0,    0,    0,  
     5   25,   24,    0,    1,    0,    0,    0,    0,    0,    0,  
     6    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  
     7  40*0,   
     1    1,    5,    3,    5,    0,    0,    0,    0,    0,    0,  
     2  60*0,   
     8    7,    2, 1989,   11,   25,    0,    0,    0,    0,    0,  
     9    0,    0,    0,    0,    0,    0,    0,    0,    0,    0/  
      DATA PARU/    
     & 3.1415927, 6.2831854, 0.1973, 5.068, 0.3894, 2.568,   4*0.,  
     1 0.001, 0.09, 0.01,  0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     2   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     3   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     4  2.0,  1.0, 0.25,  2.5, 0.05,   0.,   0., 0.0001, 0.,   0.,  
     5  2.5,  1.5,  7.0,  1.0,  0.5,  2.0,  3.2,   0.,   0.,   0.,  
     6  40*0.,  
     & 0.0072974, 0.230, 0., 0., 0.,   0.,   0.,   0.,   0.,   0.,  
     1 0.20, 0.25,  1.0,  4.0,   0.,   0.,   0.,   0.,   0.,   0.,  
     2  1.0,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     3  70*0./  
      DATA MSTJ/    
     &    1,    3,    0,    0,    0,    0,    0,    0,    0,    0,  
     1    1,    2,    0,    1,    0,    0,    0,    0,    0,    0,  
     2    2,    1,    1,    2,    1,    0,    0,    0,    0,    0,  
     3    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  
     4    1,    2,    4,    2,    5,    0,    1,    0,    0,    0,  
     5    0,    3,    0,    0,    0,    0,    0,    0,    0,    0,  
     6  40*0,   
     &    5,    2,    7,    5,    1,    1,    0,    2,    0,    1,  
     1    0,    0,    0,    0,    1,    1,    0,    0,    0,    0,  
     2  80*0/   
      DATA PARJ/    
     & 0.10, 0.30, 0.40, 0.05, 0.50, 0.50, 0.50,   0.,   0.,   0.,  
     1 0.50, 0.60, 0.75,   0.,   0.,   0.,   0.,  1.0,  1.0,   0.,  
     2 0.35,  1.0,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     3 0.10,  1.0,  0.8,  1.5,  0.8,  2.0,  0.2,  2.5,  0.6,  2.5,  
     4  0.5,  0.9,  0.5,  0.9,  0.5,   0.,   0.,   0.,   0.,   0.,  
     5 0.77, 0.77, 0.77,   0.,   0.,   0.,   0.,   0.,  1.0,   0.,  
     6  4.5,  0.7,  0., 0.003,  0.5,  0.5,   0.,   0.,   0.,   0.,  
     7  10., 1000., 100., 1000., 0.,   0.,   0.,   0.,   0.,   0.,  
     8  0.4,  1.0,  1.0,   0.,  10.,  10.,   0.,   0.,   0.,   0.,  
     9 0.02,  1.0,  0.2,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     &   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     1   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     2  1.5,  0.5, 91.2, 2.40, 0.02,  2.0,  1.0, 0.25,0.002,   0.,  
     3   0.,   0.,   0.,   0., 0.01, 0.99,   0.,   0.,  0.2,   0.,  
     4  60*0./  
    
C...LUDAT2, with particle data and flavour treatment parameters.    
      DATA (KCHG(I,1),I=   1, 500)/-1,2,-1,2,-1,2,-1,2,2*0,-3,0,-3,0,   
     &-3,0,-3,6*0,3,9*0,3,2*0,3,46*0,2,-1,2,-1,2,3,11*0,3,0,2*3,    
     &0,3,0,3,12*0,3,0,2*3,0,3,0,3,12*0,3,0,2*3,0,3,0,3,12*0,3,0,2*3,0, 
     &3,0,3,12*0,3,0,2*3,0,3,0,3,12*0,3,0,2*3,0,3,0,3,72*0,3,0,3,28*0,  
     &3,2*0,3,8*0,-3,8*0,3,0,-3,0,3,-3,3*0,3,6,0,3,5*0,-3,0,3,-3,0,-3,  
     &4*0,-3,0,3,6,-3,0,3,-3,0,-3,0,3,6,0,3,5*0,-3,0,3,-3,0,-3,114*0/   
      DATA (KCHG(I,2),I=   1, 500)/8*1,12*0,2,68*0,-1,410*0/    
      DATA (KCHG(I,3),I=   1, 500)/8*1,2*0,8*1,5*0,1,9*0,1,2*0,1,2*0,1, 
     &41*0,1,0,7*1,10*0,9*1,11*0,9*1,11*0,9*1,11*0,9*1,11*0,9*1,    
     &11*0,9*1,71*0,3*1,22*0,1,5*0,1,0,2*1,6*0,1,0,2*1,6*0,2*1,0,5*1,   
     &0,6*1,4*0,6*1,4*0,16*1,4*0,6*1,114*0/ 
      DATA (PMAS(I,1),I=   1, 500)/.0099,.0056,.199,1.35,5.,90.,120.,   
     &200.,2*0.,.00051,0.,.1057,0.,1.7841,0.,60.,5*0.,91.2,80.,15., 
     &6*0.,300.,900.,600.,300.,900.,300.,2*0.,5000.,60*0.,.1396,.4977,  
     &.4936,1.8693,1.8645,1.9693,5.2794,5.2776,5.47972,0.,.135,.5488,   
     &.9575,2.9796,9.4,117.99,238.,397.,2*0.,.7669,.8962,.8921, 
     &2.0101,2.0071,2.1127,2*5.3354,5.5068,0.,.77,.782,1.0194,3.0969,   
     &9.4603,118.,238.,397.,2*0.,1.233,2*1.3,2*2.322,2.51,2*5.73,5.97,  
     &0.,1.233,1.17,1.41,3.46,9.875,118.42,238.42,397.42,2*0.,  
     &.983,2*1.429,2*2.272,2.46,2*5.68,5.92,0.,.983,1.,1.4,3.4151,  
     &9.8598,118.4,238.4,397.4,2*0.,1.26,2*1.401,2*2.372,   
     &2.56,2*5.78,6.02,0.,1.26,1.283,1.422,3.5106,9.8919,118.5,238.5,   
     &397.5,2*0.,1.318,2*1.426,2*2.422,2.61,2*5.83,6.07,0.,1.318,1.274, 
     &1.525,3.5563,9.9132,118.45,238.45,397.45,2*0.,2*.4977,    
     &83*0.,1.1156,5*0.,2.2849,0.,2*2.46,6*0.,5.62,0.,2*5.84,6*0.,  
     &.9396,.9383,0.,1.1974,1.1926,1.1894,1.3213,1.3149,0.,2.454,   
     &2.4529,2.4522,2*2.55,2.73,4*0.,3*5.8,2*5.96,6.12,4*0.,1.234,  
     &1.233,1.232,1.231,1.3872,1.3837,1.3828,1.535,1.5318,1.6724,3*2.5, 
     &2*2.63,2.8,4*0.,3*5.81,2*5.97,6.13,114*0./    
      DATA (PMAS(I,2),I=   1, 500)/22*0.,2.4,2.3,88*0.,.0002,.001,  
     &6*0.,.149,.0505,.0513,7*0.,.153,.0085,.0044,7*0.,.15,2*.09,2*.06, 
     &.04,3*.1,0.,.15,.335,.08,2*.01,5*0.,.057,2*.287,2*.06,.04,3*.1,   
     &0.,.057,0.,.25,.0135,6*0.,.4,2*.184,2*.06,.04,3*.1,0.,.4,.025,    
     &.055,.0135,6*0.,.11,.115,.099,2*.06,4*.1,0.,.11,.185,.076,.0026,  
     &146*0.,4*.115,.039,2*.036,.0099,.0091,131*0./ 
      DATA (PMAS(I,3),I=   1, 500)/22*0.,2*20.,88*0.,.002,.005,6*0.,.4, 
     &2*.2,7*0.,.4,.1,.015,7*0.,.25,2*.01,3*.08,2*.2,.12,0.,.25,.2, 
     &.001,2*.02,5*0.,.05,2*.4,3*.08,2*.2,.12,0.,.05,0.,.35,.05,6*0.,   
     &3*.3,2*.08,.06,2*.2,.12,0.,.3,.05,.025,.001,6*0.,.25,4*.12,4*.2,  
     &0.,.25,.17,.2,.01,146*0.,4*.14,.04,2*.035,2*.05,131*0./   
      DATA (PMAS(I,4),I=   1, 500)/12*0.,658650.,0.,.091,68*0.,.1,.43,  
     &15*0.,7803.,0.,3709.,.32,.128,.131,3*.393,84*0.,.004,26*0.,   
     &15540.,26.75,83*0.,78.88,5*0.,.054,0.,2*.13,6*0.,.393,0.,2*.393,  
     &9*0.,44.3,0.,24.,49.1,86.9,6*0.,.13,9*0.,.393,13*0.,24.6,130*0./  
      DATA PARF/    
     &  0.5, 0.25,  0.5, 0.25,   1.,  0.5,   0.,   0.,   0.,   0.,  
     1  0.5,   0.,  0.5,   0.,   1.,   1.,   0.,   0.,   0.,   0.,  
     2  0.5,   0.,  0.5,   0.,   1.,   1.,   0.,   0.,   0.,   0.,  
     3  0.5,   0.,  0.5,   0.,   1.,   1.,   0.,   0.,   0.,   0.,  
     4  0.5,   0.,  0.5,   0.,   1.,   1.,   0.,   0.,   0.,   0.,  
     5  0.5,   0.,  0.5,   0.,   1.,   1.,   0.,   0.,   0.,   0.,  
     6 0.75,  0.5,   0., 0.1667, 0.0833, 0.1667, 0., 0., 0.,   0.,  
     7   0.,   0.,   1., 0.3333, 0.6667, 0.3333, 0., 0., 0.,   0.,  
     8   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     9   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     & 0.325, 0.325, 0.5, 1.6,  5.0,   0.,   0.,   0.,   0.,   0.,  
     1   0., 0.11, 0.16, 0.048, 0.50, 0.45, 0.55, 0.60,  0.,   0.,  
     2  0.2,  0.1,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,  
     3  1870*0./    
      DATA ((VCKM(I,J),J=1,4),I=1,4)/   
     1  0.95150,  0.04847,  0.00003,  0.00000,  
     2  0.04847,  0.94936,  0.00217,  0.00000,  
     3  0.00003,  0.00217,  0.99780,  0.00000,  
     4  0.00000,  0.00000,  0.00000,  1.00000/  
    
C...LUDAT3, with particle decay parameters and data.    
      DATA (MDCY(I,1),I=   1, 500)/14*0,1,0,1,5*0,3*1,6*0,1,4*0,1,2*0,  
     &1,42*0,7*1,12*0,1,0,6*1,0,8*1,2*0,9*1,0,8*1,2*0,9*1,0,8*1,2*0,    
     &9*1,0,8*1,2*0,9*1,0,8*1,2*0,9*1,0,8*1,3*0,1,83*0,1,5*0,1,0,2*1,   
     &6*0,1,0,2*1,9*0,5*1,0,6*1,4*0,6*1,4*0,16*1,4*0,6*1,114*0/ 
      DATA (MDCY(I,2),I=   1, 500)/1,9,17,25,33,41,49,57,2*0,65,69,71,  
     &76,78,118,120,125,2*0,127,136,149,166,186,6*0,203,4*0,219,2*0,    
     &227,42*0,236,237,241,250,252,254,256,11*0,276,277,279,285,406,    
     &574,606,607,608,0,609,611,617,623,624,625,626,627,2*0,628,629,    
     &632,635,638,640,641,642,643,0,644,645,650,658,661,670,685,686,    
     &2*0,687,688,693,698,700,702,703,705,707,0,709,710,713,717,718,    
     &719,721,722,2*0,723,726,728,730,734,738,740,744,748,0,752,755,    
     &759,763,765,767,769,770,2*0,771,773,775,777,779,781,784,786,788,  
     &0,791,793,806,810,812,814,816,817,2*0,818,824,835,846,854,862,    
     &867,875,883,0,888,895,903,905,907,909,911,912,2*0,913,921,83*0,   
     &923,5*0,927,0,1001,1002,6*0,1003,0,1004,1005,9*0,1006,1008,1009,  
     &1012,1013,0,1015,1016,1017,1018,1019,1020,4*0,1021,1022,1023, 
     &1024,1025,1026,4*0,1027,1028,1031,1034,1035,1038,1041,1044,1046,  
     &1048,1052,1053,1054,1055,1057,1059,4*0,1060,1061,1062,1063,1064,  
     &1065,114*0/   
      DATA (MDCY(I,3),I=   1, 500)/8*8,2*0,4,2,5,2,40,2,5,2,2*0,9,13,   
     &17,20,17,6*0,16,4*0,8,2*0,9,42*0,1,4,9,3*2,20,11*0,1,2,6,121,168, 
     &32,3*1,0,2,2*6,5*1,2*0,1,3*3,2,4*1,0,1,5,8,3,9,15,2*1,2*0,1,2*5,  
     &2*2,1,3*2,0,1,3,4,2*1,2,2*1,2*0,3,2*2,2*4,2,3*4,0,3,2*4,3*2,2*1,  
     &2*0,5*2,3,2*2,3,0,2,13,4,3*2,2*1,2*0,6,2*11,2*8,5,2*8,5,0,7,8,    
     &4*2,2*1,2*0,8,2,83*0,4,5*0,74,0,2*1,6*0,1,0,2*1,9*0,2,1,3,1,2,0,  
     &6*1,4*0,6*1,4*0,1,2*3,1,3*3,2*2,4,3*1,2*2,1,4*0,6*1,114*0/    
      DATA (MDME(I,1),I=   1,2000)/6*1,-1,7*1,-1,7*1,-1,7*1,-1,7*1,-1,  
     &7*1,-1,85*1,2*-1,7*1,2*-1,3*1,2*-1,6*1,2*-1,6*1,3*-1,3*1,-1,3*1,  
     &-1,3*1,5*-1,3*1,-1,6*1,2*-1,3*1,-1,11*1,2*-1,6*1,2*-1,3*1,-1,3*1, 
     &-1,4*1,2*-1,2*1,-1,488*1,2*0,1275*1/  
      DATA (MDME(I,2),I=   1,2000)/70*102,42,6*102,2*42,2*0,7*41,2*0,   
     &23*41,6*102,45,28*102,8*32,9*0,16*32,4*0,8*32,4*0,32,4*0,8*32,    
     &8*0,4*32,4*0,6*32,3*0,12,2*42,2*11,9*42,6*45,20*46,7*0,34*42, 
     &86*0,2*25,26,24*42,142*0,25,26,0,10*42,19*0,2*13,3*85,0,2,4*0,2,  
     &8*0,2*32,87,88,3*3,0,2*3,0,2*3,0,3,5*0,3,1,0,3,2*0,2*3,3*0,1,4*0, 
     &12,3*0,4*32,2*4,6*0,5*32,2*4,2*45,87,88,30*0,12,32,0,32,87,88,    
     &41*0,12,0,32,0,32,87,88,40*0,12,0,32,0,32,87,88,88*0,12,0,32,0,   
     &32,87,88,2*0,4*42,8*0,14*42,50*0,10*13,2*84,3*85,14*0,84,5*0,85,  
     &974*0/    
      DATA (BRAT(I)  ,I=   1, 525)/70*0.,1.,6*0.,2*.177,.108,.225,.003, 
     &.06,.02,.025,.013,2*.004,.007,.014,2*.002,2*.001,.054,.014,.016,  
     &.005,2*.012,5*.006,.002,2*.001,5*.002,6*0.,1.,28*0.,.143,.111,    
     &.143,.111,.143,.085,2*0.,.03,.058,.03,.058,.03,.058,3*0.,.25,.01, 
     &2*0.,.01,.25,4*0.,.24,5*0.,3*.08,3*0.,.01,.08,.82,5*0.,.09,6*0.,  
     &.143,.111,.143,.111,.143,.085,2*0.,.03,.058,.03,.058,.03,.058,    
     &4*0.,1.,5*0.,4*.215,2*0.,2*.07,0.,1.,2*.08,.76,.08,2*.112,.05,    
     &.476,.08,.14,.01,.015,.005,1.,0.,1.,0.,1.,0.,.25,.01,2*0.,.01,    
     &.25,4*0.,.24,5*0.,3*.08,0.,1.,2*.5,.635,.212,.056,.017,.048,.032, 
     &.035,.03,2*.015,.044,2*.022,9*.001,.035,.03,2*.015,.044,2*.022,   
     &9*.001,.028,.017,.066,.02,.008,2*.006,.003,.001,2*.002,.003,.001, 
     &2*.002,.005,.002,.005,.006,.004,.012,2*.005,.008,2*.005,.037, 
     &.004,.067,2*.01,2*.001,3*.002,.003,8*.002,.005,4*.004,.015,.005,  
     &.027,2*.005,.007,.014,.007,.01,.008,.012,.015,11*.002,3*.004, 
     &.002,.004,6*.002,2*.004,.005,.011,.005,.015,.02,2*.01,3*.004, 
     &5*.002,.015,.02,2*.01,3*.004,5*.002,.038,.048,.082,.06,.028,.021, 
     &2*.005,2*.002,.005,.018,.005,.01,.008,.005,3*.004,.001,3*.003,    
     &.001,2*.002,.003,2*.002,2*.001,.002,.001,.002,.001,.005,4*.003,   
     &.001,2*.002,.003,2*.001,.013,.03,.058,.055,3*.003,2*.01,.007, 
     &.019,4*.005,.015,3*.005,8*.002,3*.001,.002,2*.001,.003,16*.001/   
      DATA (BRAT(I)  ,I= 526, 893)/.019,2*.003,.002,.005,.004,.008, 
     &.003,.006,.003,.01,5*.002,2*.001,2*.002,11*.001,.002,14*.001, 
     &.018,.005,.01,2*.015,.017,4*.015,.017,3*.015,.025,.08,2*.025,.04, 
     &.001,2*.005,.02,.04,2*.06,.04,.01,4*.005,.25,.115,3*1.,.988,.012, 
     &.389,.319,.237,.049,.005,.001,.441,.205,.301,.03,.022,.001,6*1.,  
     &.665,.333,.002,.666,.333,.001,.49,.34,.17,.52,.48,5*1.,.893,.08,  
     &.017,2*.005,.495,.343,3*.043,.019,.013,.001,2*.069,.862,3*.027,   
     &.015,.045,.015,.045,.77,.029,6*.02,5*.05,.115,.015,.5,0.,3*1.,    
     &.28,.14,.313,.157,.11,.28,.14,.313,.157,.11,.667,.333,.667,.333,  
     &1.,.667,.333,.667,.333,2*.5,1.,.333,.334,.333,4*.25,2*1.,.3,.7,   
     &2*1.,.8,2*.1,.667,.333,.667,.333,.6,.3,.067,.033,.6,.3,.067,.033, 
     &2*.5,.6,.3,.067,.033,.6,.3,.067,.033,2*.4,2*.1,.8,2*.1,.52,.26,   
     &2*.11,.62,.31,2*.035,.007,.993,.02,.98,.3,.7,2*1.,2*.5,.667,.333, 
     &.667,.333,.667,.333,.667,.333,2*.35,.3,.667,.333,.667,.333,2*.35, 
     &.3,2*.5,3*.14,.1,.05,4*.08,.028,.027,.028,.027,4*.25,.273,.727,   
     &.35,.65,.3,.7,2*1.,2*.35,.144,.105,.048,.003,.332,.166,.168,.084, 
     &.086,.043,.059,2*.029,2*.002,.332,.166,.168,.084,.086,.043,.059,  
     &2*.029,2*.002,.3,.15,.16,.08,.13,.06,.08,.04,.3,.15,.16,.08,.13,  
     &.06,.08,.04,2*.4,.1,2*.05,.3,.15,.16,.08,.13,.06,.08,.04,.3,.15,  
     &.16,.08,.13,.06,.08,.04,2*.4,.1,2*.05,2*.35,.144,.105,2*.024/ 
      DATA (BRAT(I)  ,I= 894,2000)/.003,.573,.287,.063,.028,2*.021, 
     &.004,.003,2*.5,.15,.85,.22,.78,.3,.7,2*1.,.217,.124,2*.193,   
     &2*.135,.002,.001,.686,.314,.641,.357,2*.001,.018,2*.005,.003, 
     &.002,2*.006,.018,2*.005,.003,.002,2*.006,.005,.025,.015,.006, 
     &2*.005,.004,.005,5*.004,2*.002,2*.004,.003,.002,2*.003,3*.002,    
     &2*.001,.002,2*.001,2*.002,5*.001,4*.003,2*.005,2*.002,2*.001, 
     &2*.002,2*.001,.255,.057,2*.035,.15,2*.075,.03,2*.015,5*1.,.999,   
     &.001,1.,.516,.483,.001,1.,.995,.005,13*1.,.331,.663,.006,.663,    
     &.331,.006,1.,.88,2*.06,.88,2*.06,.88,2*.06,.667,2*.333,.667,.676, 
     &.234,.085,.005,3*1.,4*.5,7*1.,935*0./ 
      DATA (KFDP(I,1),I=   1, 499)/21,22,23,4*-24,25,21,22,23,4*24,25,  
     &21,22,23,4*-24,25,21,22,23,4*24,25,21,22,23,4*-24,25,21,22,23,    
     &4*24,25,21,22,23,4*-24,25,21,22,23,4*24,25,22,23,-24,25,23,24,    
     &-12,22,23,-24,25,23,24,-12,-14,34*16,22,23,-24,25,23,24,-89,22,   
     &23,-24,25,23,24,1,2,3,4,5,6,7,8,21,1,2,3,4,5,6,7,8,11,13,15,17,   
     &37,1,2,3,4,5,6,7,8,11,12,13,14,15,16,17,18,37,4*-1,4*-3,4*-5, 
     &4*-7,-11,-13,-15,-17,1,2,3,4,5,6,7,8,11,13,15,17,21,2*22,23,24,1, 
     &2,3,4,5,6,7,8,11,12,13,14,15,16,17,18,-1,-3,-5,-7,-11,-13,-15,    
     &-17,1,2,3,4,5,6,11,13,15,82,-11,-13,2*2,-12,-14,-16,2*-2,2*-4,-2, 
     &-4,2*89,2*-89,2*89,4*-1,4*-3,4*-5,4*-7,-11,-13,-15,-17,-13,130,   
     &310,-13,3*211,12,14,16*-11,16*-13,-311,-313,-311,-313,-311,-313,  
     &-311,-313,2*111,2*221,2*331,2*113,2*223,2*333,-311,-313,2*-311,   
     &-313,3*-311,-321,-323,-321,2*211,2*213,-213,113,3*213,3*211,  
     &2*213,2*-311,-313,-321,2*-311,-313,-311,-313,4*-311,-321,-323,    
     &2*-321,3*211,213,2*211,213,5*211,213,4*211,3*213,211,213,321,311, 
     &3,2*2,12*-11,12*-13,-321,-323,-321,-323,-311,-313,-311,-313,-311, 
     &-313,-311,-313,-311,-313,-311,-321,-323,-321,-323,211,213,211,    
     &213,111,221,331,113,223,333,221,331,113,223,113,223,113,223,333,  
     &223,333,321,323,321,323,311,313,-321,-323,3*-321,-323,2*-321, 
     &-323,-321,-311,-313,3*-311,-313,2*-311,-313,-321,-323,3*-321/ 
      DATA (KFDP(I,1),I= 500, 873)/-323,2*-321,-311,2*333,211,213,  
     &2*211,2*213,4*211,10*111,-321,-323,5*-321,-323,2*-321,-311,-313,  
     &4*-311,-313,4*-311,-321,-323,2*-321,-323,-321,-313,-311,-313, 
     &-311,211,213,2*211,213,4*211,111,221,113,223,113,223,2*3,-15, 
     &5*-11,5*-13,221,331,333,221,331,333,211,213,211,213,321,323,321,  
     &323,2212,221,331,333,221,2*2,3*0,3*22,111,211,2*22,2*211,111, 
     &3*22,111,3*21,2*0,211,321,3*311,2*321,421,2*411,2*421,431,511,    
     &521,531,2*211,22,211,2*111,321,130,-213,113,213,211,22,111,11,13, 
     &82,11,13,15,1,2,3,4,21,22,11,12,13,14,15,16,1,2,3,4,5,21,22,2*89, 
     &2*0,223,321,311,323,313,2*311,321,313,323,321,421,2*411,421,433,  
     &521,2*511,521,523,513,223,213,113,-213,313,-313,323,-323,82,21,   
     &663,21,2*0,221,213,113,321,2*311,321,421,411,423,413,411,421,413, 
     &423,431,433,521,511,523,513,511,521,513,523,521,511,531,533,221,  
     &213,-213,211,111,321,130,211,111,321,130,443,82,553,21,663,21,    
     &2*0,113,213,323,2*313,323,423,2*413,423,421,411,433,523,2*513,    
     &523,521,511,533,213,-213,10211,10111,-10211,2*221,213,2*113,-213, 
     &2*321,2*311,313,-313,323,-323,443,82,553,21,663,21,2*0,213,113,   
     &221,223,321,211,321,311,323,313,323,313,321,5*311,321,313,323,    
     &313,323,311,4*321,421,411,423,413,423,413,421,2*411,421,413,423,  
     &413,423,411,2*421,411,433,2*431,521,511,523,513,523,513,521/  
      DATA (KFDP(I,1),I= 874,2000)/2*511,521,513,523,513,523,511,2*521, 
     &511,533,2*531,213,-213,221,223,321,130,111,211,111,2*211,321,130, 
     &221,111,321,130,443,82,553,21,663,21,2*0,111,211,-12,12,-14,14,   
     &211,111,211,111,2212,2*2112,-12,7*-11,7*-13,2*2224,2*2212,2*2214, 
     &2*3122,2*3212,2*3214,5*3222,4*3224,2*3322,3324,2*2224,5*2212, 
     &5*2214,2*2112,2*2114,2*3122,2*3212,2*3214,2*3222,2*3224,4*2,3,    
     &2*2,1,2*2,5*0,2112,-12,3122,2212,2112,2212,3*3122,3*4122,4132,    
     &4232,0,3*5122,5132,5232,0,2112,2212,2*2112,2212,2112,2*2212,3122, 
     &3212,3112,3122,3222,3112,3122,3222,3212,3322,3312,3322,3312,3122, 
     &3322,3312,-12,3*4122,2*4132,2*4232,4332,3*5122,5132,5232,5332,    
     &935*0/    
      DATA (KFDP(I,2),I=   1, 496)/3*1,2,4,6,8,1,3*2,1,3,5,7,2,3*3,2,4, 
     &6,8,3,3*4,1,3,5,7,4,3*5,2,4,6,8,5,3*6,1,3,5,7,6,3*7,2,4,6,8,7,    
     &3*8,1,3,5,7,8,2*11,12,11,12,2*11,2*13,14,13,14,13,11,13,-211, 
     &-213,-211,-213,-211,-213,3*-211,-321,-323,-321,-323,2*-321,   
     &4*-211,-213,-211,-213,-211,-213,-211,-213,-211,-213,6*-211,2*15,  
     &16,15,16,15,18,2*17,18,17,18,17,-1,-2,-3,-4,-5,-6,-7,-8,21,-1,-2, 
     &-3,-4,-5,-6,-7,-8,-11,-13,-15,-17,-37,-1,-2,-3,-4,-5,-6,-7,-8,    
     &-11,-12,-13,-14,-15,-16,-17,-18,-37,2,4,6,8,2,4,6,8,2,4,6,8,2,4,  
     &6,8,12,14,16,18,-1,-2,-3,-4,-5,-6,-7,-8,-11,-13,-15,-17,21,22,    
     &2*23,-24,-1,-2,-3,-4,-5,-6,-7,-8,-11,-12,-13,-14,-15,-16,-17,-18, 
     &2,4,6,8,12,14,16,18,-3,-4,-5,-6,-7,-8,-13,-15,-17,-82,12,14,-1,   
     &-3,11,13,15,1,4,3,4,1,3,5,3,6,4,7,5,2,4,6,8,2,4,6,8,2,4,6,8,2,4,  
     &6,8,12,14,16,18,14,2*0,14,111,211,111,-11,-13,16*12,16*14,2*211,  
     &2*213,2*321,2*323,211,213,211,213,211,213,211,213,211,213,211,    
     &213,2*211,213,7*211,213,211,111,211,111,2*211,-213,213,2*113,223, 
     &2*113,221,321,2*311,321,313,4*211,213,113,213,-213,2*211,213,113, 
     &111,221,331,111,113,223,4*113,223,6*211,213,4*211,-321,-311,3*-1, 
     &12*12,12*14,2*211,2*213,2*111,2*221,2*331,2*113,2*223,333,2*321,  
     &2*323,2*-211,2*-213,6*111,4*221,2*331,3*113,2*223,2*-211,2*-213,  
     &113,111,2*211,213,6*211,321,2*211,213,211,2*111,113,2*223,2*321/  
      DATA (KFDP(I,2),I= 497, 863)/323,321,2*311,313,2*311,111,211, 
     &2*-211,-213,-211,-213,-211,-213,3*-211,5*111,2*113,223,113,223,   
     &2*211,213,5*211,213,3*211,213,2*211,2*111,221,113,223,3*321,323,  
     &2*321,323,311,313,311,313,3*211,2*-211,-213,3*-211,4*111,2*113,   
     &2*-1,16,5*12,5*14,3*211,3*213,2*111,2*113,2*-311,2*-313,-2112,    
     &3*321,323,2*-1,3*0,22,11,22,111,-211,211,11,2*-211,111,113,223,   
     &22,111,3*21,2*0,111,-211,111,22,211,111,22,211,111,22,111,5*22,   
     &2*-211,111,-211,2*111,-321,310,211,111,2*-211,221,22,-11,-13,-82, 
     &-11,-13,-15,-1,-2,-3,-4,2*21,-11,-12,-13,-14,-15,-16,-1,-2,-3,-4, 
     &-5,2*21,5,3,2*0,211,-213,113,-211,111,223,211,111,211,111,223,    
     &211,111,-211,2*111,-211,111,211,111,-321,-311,111,-211,111,211,   
     &-311,311,-321,321,-82,21,22,21,2*0,211,111,211,-211,111,211,111,  
     &211,111,211,111,-211,111,-211,3*111,-211,111,-211,111,211,111,    
     &211,111,-321,-311,3*111,-211,211,-211,111,-321,310,-211,111,-321, 
     &310,22,-82,22,21,22,21,2*0,211,111,-211,111,211,111,211,111,-211, 
     &111,321,311,111,-211,111,211,111,-321,-311,111,-211,211,-211,111, 
     &2*211,111,-211,211,111,211,-321,2*-311,-321,-311,311,-321,321,22, 
     &-82,22,21,22,21,2*0,111,3*211,-311,22,-211,111,-211,111,-211,211, 
     &-213,113,223,221,22,211,111,211,111,2*211,213,113,223,221,22,211, 
     &111,211,111,4*211,-211,111,-211,111,-211,211,-211,211,321,311/    
      DATA (KFDP(I,2),I= 864,2000)/2*111,211,-211,111,-211,111,-211,    
     &211,-211,2*211,111,211,111,4*211,-321,-311,2*111,211,-211,211,    
     &111,211,-321,310,22,-211,111,2*-211,-321,310,221,111,-321,310,22, 
     &-82,22,21,22,21,2*0,111,-211,11,-11,13,-13,-211,111,-211,111, 
     &-211,111,22,11,7*12,7*14,-321,-323,-311,-313,-311,-313,211,213,   
     &211,213,211,213,111,221,331,113,223,111,221,113,223,321,323,321,  
     &-211,-213,111,221,331,113,223,111,221,331,113,223,211,213,211,    
     &213,321,323,321,323,321,323,311,313,311,313,2*-1,-3,-1,2203,  
     &2*3201,2203,2101,2103,5*0,-211,11,22,111,211,22,-211,111,22,-211, 
     &111,211,2*22,0,-211,111,211,2*22,0,2*-211,111,22,111,211,22,211,  
     &2*-211,2*111,-211,2*211,111,211,-211,2*111,211,-321,-211,111,11,  
     &-211,111,211,111,22,111,2*22,-211,111,211,3*22,935*0/ 
      DATA (KFDP(I,3),I=   1, 918)/70*0,14,6*0,2*16,2*0,5*111,310,130,  
     &2*0,2*111,310,130,113,211,223,221,2*113,2*211,2*223,2*221,2*113,  
     &221,113,2*213,-213,123*0,4*3,4*4,1,4,3,2*2,6*81,25*0,-211,3*111,  
     &-311,-313,-311,2*-321,2*-311,111,221,331,113,223,211,111,211,111, 
     &-311,-313,-311,2*-321,2*-311,111,221,331,113,223,211,111,211,111, 
     &20*0,3*111,2*221,331,113,223,3*211,-211,111,-211,111,211,111,211, 
     &-211,111,113,111,223,2*111,-311,4*211,2*111,2*211,111,7*211,  
     &7*111,113,221,2*223,2*-211,-213,4*-211,-213,-211,-213,-211,2*211, 
     &2,2*0,-321,-323,-311,-321,-311,2*-321,-211,-213,2*-211,211,-321,  
     &-323,-311,-321,-311,2*-321,-211,-213,2*-211,211,46*0,3*111,113,   
     &2*221,331,2*223,-311,3*-211,-213,8*111,113,3*211,213,2*111,-211,  
     &3*111,113,111,2*113,221,331,223,111,221,331,113,223,113,2*223,    
     &2*221,3*111,221,113,223,4*211,3*-211,-213,-211,5*111,-321,3*211,  
     &3*111,2*211,2*111,2*-211,-213,3*111,221,113,223,6*111,3*0,221,    
     &331,333,321,311,221,331,333,321,311,19*0,3,5*0,-11,0,2*111,-211,  
     &-11,11,2*221,3*0,111,22*0,111,2*0,22,111,5*0,111,12*0,2*21,11*0,  
     &2*21,2*-6,111*0,-211,2*111,-211,3*111,-211,111,211,15*0,111,6*0,  
     &111,-211,9*0,111,-211,9*0,111,-211,111,-211,4*0,111,-211,111, 
     &-211,4*0,-211,4*0,111,-211,111,-211,4*0,111,-211,111,-211,4*0,    
     &-211,3*0,-211,5*0,111,211,3*0,111,10*0,2*111,211,-211,211,-211/   
      DATA (KFDP(I,3),I= 919,2000)/7*0,2212,3122,3212,3214,2112,2114,   
     &2212,2112,3122,3212,3214,2112,2114,2212,2112,50*0,3*3,1,12*0, 
     &2112,43*0,3322,949*0/ 
      DATA (KFDP(I,4),I=   1,2000)/83*0,3*111,9*0,-211,3*0,111,2*-211,  
     &0,111,0,2*111,113,221,111,-213,-211,211,123*0,13*81,37*0,111, 
     &3*211,111,5*0,-211,111,-211,111,2*0,111,3*211,111,5*0,-211,111,   
     &-211,111,50*0,2*111,2*-211,2*111,-211,211,3*111,211,14*111,221,   
     &113,223,2*111,2*113,223,2*111,-1,4*0,-211,111,-211,211,111,2*0,   
     &2*111,-211,2*0,-211,111,-211,211,111,2*0,2*111,-211,96*0,6*111,   
     &3*-211,-213,4*111,113,6*111,3*-211,3*111,2*-211,2*111,3*-211, 
     &12*111,6*0,-321,-311,3*0,-321,-311,19*0,-3,11*0,-11,280*0,111,    
     &-211,3*0,111,29*0,-211,111,5*0,-211,111,50*0,2101,2103,2*2101,    
     &1006*0/   
      DATA (KFDP(I,5),I=   1,2000)/85*0,111,15*0,111,7*0,111,0,2*111,   
     &175*0,111,-211,111,7*0,2*111,4*0,111,-211,111,7*0,2*111,93*0,111, 
     &-211,111,3*0,111,-211,4*0,111,-211,111,3*0,111,-211,1571*0/   
    
C...LUDAT4, with character strings. 
      DATA (CHAF(I)  ,I=   1, 331)/'d','u','s','c','b','t','l','h', 
     &2*' ','e','nu_e','mu','nu_mu','tau','nu_tau','chi','nu_chi',  
     &2*' ','g','gamma','Z','W','H',6*' ','Z''','Z"','W''','H''','H"',  
     &'H',2*' ','R',40*' ','specflav','rndmflav','phasespa','c-hadron', 
     &'b-hadron','t-hadron','l-hadron','h-hadron','Wvirt','diquark',    
     &'cluster','string','indep.','CMshower','SPHEaxis','THRUaxis', 
     &'CLUSjet','CELLjet','table',' ','pi',2*'K',2*'D','D_s',2*'B', 
     &'B_s',' ','pi','eta','eta''','eta_c','eta_b','eta_t','eta_l', 
     &'eta_h',2*' ','rho',2*'K*',2*'D*','D*_s',2*'B*','B*_s',' ','rho', 
     &'omega','phi','J/psi','Upsilon','Theta','Theta_l','Theta_h',  
     &2*' ','b_1',2*'K_1',2*'D_1','D_1s',2*'B_1','B_1s',' ','b_1',  
     &'h_1','h''_1','h_1c','h_1b','h_1t','h_1l','h_1h',2*' ','a_0', 
     &2*'K*_0',2*'D*_0','D*_0s',2*'B*_0','B*_0s',' ','a_0','f_0',   
     &'f''_0','chi_0c','chi_0b','chi_0t','chi_0l','chi_0h',2*' ','a_1', 
     &2*'K*_1',2*'D*_1','D*_1s',2*'B*_1','B*_1s',' ','a_1','f_1',   
     &'f''_1','chi_1c','chi_1b','chi_1t','chi_1l','chi_1h',2*' ','a_2', 
     &2*'K*_2',2*'D*_2','D*_2s',2*'B*_2','B*_2s',' ','a_2','f_2',   
     &'f''_2','chi_2c','chi_2b','chi_2t','chi_2l','chi_2h',2*' ','K_L', 
     &'K_S',58*' ','pi_diffr','n_diffr','p_diffr',22*' ','Lambda',5*' ',    
     &'Lambda_c',' ',2*'Xi_c',6*' ','Lambda_b',' ',2*'Xi_b',6*' '/  
      DATA (CHAF(I)  ,I= 332, 500)/'n','p',' ',3*'Sigma',2*'Xi',' ',    
     &3*'Sigma_c',2*'Xi''_c','Omega_c', 
     &4*' ',3*'Sigma_b',2*'Xi''_b','Omega_b',4*' ',4*'Delta',   
     &3*'Sigma*',2*'Xi*','Omega',3*'Sigma*_c',2*'Xi*_c','Omega*_c', 
     &4*' ',3*'Sigma*_b',2*'Xi*_b','Omega*_b',114*' '/  
    
C...LUDATR, with initial values for the random number generator.    
      DATA MRLU/19780503,0,0,97,33,0/   
    
      END   
