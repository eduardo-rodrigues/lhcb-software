class TopoLines :
    """
    Threshold settings for Hlt2 topological lines.
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION!!!!!!!!
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = ['Hlt2Topo2BodySimple',
                 'Hlt2Topo3BodySimple',
                 'Hlt2Topo4BodySimple',
                 'Hlt2Topo2BodyBBDT',
                 'Hlt2Topo3BodyBBDT',
                 'Hlt2Topo4BodyBBDT',
                 'Hlt2TopoMu2BodyBBDT',
                 'Hlt2TopoMu3BodyBBDT',
                 'Hlt2TopoMu4BodyBBDT',
                 ]
        
        return lines

   
    
    def Thresholds(self) :
        """Returns a dictionary of cuts."""

        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        d.update( { Hlt2TopologicalLinesConf : {
            # single track cuts
            'ALL_PT_MIN'        : 500.0,  # MeV
            'ALL_P_MIN'         : 5000.0, # MeV
            'ALL_MIPCHI2DV_MIN' : 4.0,    # unitless
            'ALL_TRCHI2DOF_MAX' : 4.0,    # unitless
            'KS_MASS_WINDOW'    : 30.0,   # MeV
            'KSPI_MIPCHI2DV_MIN': 16.0,    # unitless
            'KS_BPVVDCHI2_MIN'  : 1000.0,  # unitless 
            'USE_KS'            : True,
            # upfront combo cuts
            'AMAXDOCA_MAX'      : 0.2,    # mm        
            'BPVVDCHI2_MIN'     : 100.0,  # unitless
            'MIN_TRCHI2DOF_MAX' : 2.4,    # unitless
            'V2BODYCHI2_MAX'    : 10,     # unitless
            # bdt cuts
            'BDT_2BODY_MIN'     : 0.65,
            'BDT_3BODY_MIN'     : 0.7,
            'BDT_4BODY_MIN'     : 0.6,
            'BDT_2BODYMU_MIN'   : 0.3,
            'BDT_3BODYMU_MIN'   : 0.33,
            'BDT_4BODYMU_MIN'   : 0.28,
            # bdt param file versions
            'BDT_2BODY_PARAMS'  : 'v1r0',
            'BDT_3BODY_PARAMS'  : 'v1r0',
            'BDT_4BODY_PARAMS'  : 'v1r0',
            # global event cuts
            'USE_GEC'           : False,
            'GEC_MAX'           : 350,
            'HLT1FILTER'        : "",
            # pre and post scales
            'Prescale'   : {}, 
            'Postscale'  : {}
            }})

        return d
