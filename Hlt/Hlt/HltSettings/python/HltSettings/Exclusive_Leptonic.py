class Exclusive_Leptonic :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2010-07-14
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            # Dstar for D2mumu
            'Hlt2Dst2PiD02KMu', 
            'Hlt2Dst2PiD02KPi', 
            'Hlt2Dst2PiD02KPiD0SB', 
            'Hlt2Dst2PiD02KPiDMSB', 
            'Hlt2Dst2PiD02KPiSignal', 
            'Hlt2Dst2PiD02MuMu', 
            'Hlt2Dst2PiD02MuMuD0SB', 
            'Hlt2Dst2PiD02MuMuDMSB', 
            'Hlt2Dst2PiD02MuMuSignal', 
            'Hlt2Dst2PiD02PiMu',
            'Hlt2Dst2PiD02PiPi', 
            'Hlt2Dst2PiD02PiPiD0SB', 
            'Hlt2Dst2PiD02PiPiDMSB', 
            'Hlt2Dst2PiD02PiPiSignal',

            # another D2mumu
            'Hlt2CharmD02MuMu',
            'Hlt2CharmD02PiPiForD02MuMu',
            'Hlt2CharmD02KPiForD02MuMu',
            'Hlt2CharmD02KKForD02MuMu',
            
            # Bc2JpsiX
            'Hlt2TFBc2JpsiH', 
            'Hlt2TFBc2JpsiHDetached', 
            'Hlt2TFBc2JpsiHPrescaled', 
            'Hlt2TFBc2JpsiMuX', 
            'Hlt2TFBc2JpsiMuXSignal'
             ]
        
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2Dst2D2XXLines import Hlt2Dst2D2XXLinesConf
                    
        d.update( { Hlt2Dst2D2XXLinesConf : { 
            ## Cut values
            'D0MinPT'            : 1600.      # MeV
            , 'Prescale'   : { 'Hlt2Dst2PiD02PiPi' : 0.1
                               , 'Hlt2Dst2PiD02KPi'        :  0.05
                               , 'Hlt2Dst2PiD02KPiD0SB'    :  0.05
                               , 'Hlt2Dst2PiD02KPiDMSB'    :  0.05
                               , 'Hlt2Dst2PiD02KPiSignal'  :  0.1
                               }
            }}
                  )
        
        return d
