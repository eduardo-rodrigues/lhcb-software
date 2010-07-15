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
            'Hlt2CharmD02KKForD02MuMu'


             ]
        
        return lines

   
