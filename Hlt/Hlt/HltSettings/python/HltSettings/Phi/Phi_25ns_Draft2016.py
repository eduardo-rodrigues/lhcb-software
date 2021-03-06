from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class Phi_25ns_Draft2016:
    """
    Threshold settings for Hlt2 phi lines for 2016 25 ns data-taking.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author Matthew Kenzie, Jon Harrison 
    @date 2016-03-15
    """

    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = [
                 'Hlt2PhiIncPhi',
                 'Hlt2PhiBs2PhiPhi',
                 'Hlt2PhiPhi2KsKs',
                 'Hlt2PhiPhi2KsKsD0Ctrl'
                ]

        return lines
    
    def Thresholds(self) :
        """Return the trigger thresholds."""

        d = {}

        from Hlt2Lines.Phi.Lines import PhiLines
        d.update( 
            { PhiLines : {
               'PhiIncPhi' : {'TisTosSpec' : "Hlt1IncPhi.*Decision%TOS" ,
                           'KaonPT'     : 1000 * MeV ,
                           'KaonIPS'    : 9 ,
                           'KaonPID'    : 0 ,
                           'TrChi2DOF'  : 5,
                           'PhiPT'      : 2000 * MeV,
                           'PhiVCHI2'   : 20 ,
                           'PhiMassWin' : 20 * MeV},
               'PhiBs2PhiPhi': {'TisTosSpec' : "Hlt1B2PhiPhi_LTUNB.*Decision%TOS" ,
                            },
               'GoodKs' : {'KS_MASS_WINDOW'   : 20 * MeV,
                           'KS_PT_MIN'        : 400 * MeV,
                           'KS_VCHI2NDOF_MAX' : 4,
                           'GHOSTPROB_MAX'    :       0.35,
                           'KS_FD_MIN'        :      10.0 * mm},
               'PhiPhi2KsKs' : {'Phi_MASS_WINDOW'   : 70 * MeV,
                             'Phi_PT_MIN'        : 800 * MeV,
                             'Phi_DOCACHI2_MAX'  : 20,
                             'Phi_VCHI2NDOF_MAX' : 6},
               'PhiPhi2KsKsD0Ctrl' : {'D0_MASS_WINDOW'   : 70 * MeV,
                                   'D0_PT_MIN'        : 800 * MeV,
                                   'D0_DOCACHI2_MAX'  : 20,
                                   'D0_VCHI2NDOF_MAX' : 6},
              }
            }
          )

        return d
