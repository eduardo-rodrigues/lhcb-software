#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
## @file FitBasic.py
#
#  Set of useful basic utilities to build various fit models 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Set of useful basic utilities to build various fit models"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    ##
    'makeVar'       , ## helper function to create the proper RooRealVar
    'H1D_dset'      , ## convertor of 1D-histo to RooDataHist 
    'H2D_dset'      , ## convertor of 2D-histo to RooDataHist 
    'H1D_pdf'       , ## convertor of 1D-histo to RooHistPdf 
    'H2D_pdf'       , ## convertor of 1D-histo to RooDataPdf
    ##
    'PDF'           , ## useful base class for 1D-models
    'PDF2'          , ## useful base class for 2D-models
    'MASS'          , ## useful base class to create "signal" PDFs for mass-fits
    'Fit1D'         , ## the model for 1D-fit: signal + background + optional components  
    'Fit2D'         , ## the model for 2D-fit: signal + background + optional components
    ##
    'Adjust'        , ## addjust PDF to avoid zeroes (sometimes useful)
    'Convolution'   , ## helper utility to build convolution
    ##
    'Generic1D_pdf' , ## wrapper over imported RooFit (1D)-pdf  
    'Generic2D_pdf' , ## wrapper over imported RooFit (2D)-pdf  
    )
# =============================================================================
import ROOT, math
from   Ostap.Core       import cpp , VE, hID, cpp, rootID
from   Ostap.HistoDeco  import h1_axis , h2_axes
from   Ostap.Utils      import roo_silent 
# =============================================================================
from   Ostap.Logger     import getLogger
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.FitBasic' )
else                       : logger = getLogger ( __name__         )
# =============================================================================
_nemax = 20000  ## number of events per CPU-core 
_ncmax =     6  ## maximal number of CPUs: there are some problems with >= 7
                ## @see https://sft.its.cern.ch/jira/browse/ROOT-4897
_ncpus = []
#
# =============================================================================
## prepare "NumCPU" argument with reasonable choice of #cpu, depending on
#  number of events in dataset 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-31
def ncpu (  events ) :
    """
    Prepare 'NumCPU' argument with reasonable choice of #cpu, depending on
    the number of events in dataset 
    """
    #
    n  = events // _nemax
    if n       <= 1 : return ROOT.RooFit.Save () ## fake!!! 
    # 
    import multiprocessing
    n_cores = multiprocessing.cpu_count()
    if n_cores <= 1 : return ROOT.RooFit.Save () ## fake!!! 
    #
    num = min ( n , n_cores , _ncmax )
    if not _ncpus : _ncpus.append ( num )   
    #
    return ROOT.RooFit.NumCPU ( num )

# =============================================================================
## create/modify  the variable
#  Helper function for creation/modification/adjustment of variable
#  @code
#    v = makeVar ( 10   , 'myvar' , 'mycomment' )
#    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' ,     -1 , 1 )
#    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
#    v = makeVar ( None , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
#    v = makeVar ( None , 'myvar' , 'mycomment' , 10 , 0 , -1 , 1 )
#    v = makeVar ( v    , 'myvar' , 'mycomment' , 10 )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
def makeVar ( var , name , comment , fix = None , *args ) :
    """
    Make/modify  the variable:
    
    v = makeVar ( 10   , 'myvar' , 'mycomment' )
    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' ,     -1 , 1 )
    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
    v = makeVar ( None , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
    v = makeVar ( None , 'myvar' , 'mycomment' , 10 , 0 , -1 , 1 )
    v = makeVar ( v    , 'myvar' , 'mycomment' , 10 )
    
    """
    #
    # var = ( value )
    # var = ( min , max )
    # var = ( value , min , max ) 
    if   isinstance   ( var , tuple ) :
        var = ROOT.RooRealVar ( name , comment , *var )

    # var = value 
    if isinstance   ( var , ( float , int , long ) ) :
        if   not    args  : var = ROOT.RooRealVar ( name , comment , var             )
        elif 2==len(args) : var = ROOT.RooRealVar ( name , comment , var , *args     )
        elif 3==len(args) : var = ROOT.RooRealVar ( name , comment , var , *args[1:] )
        
    ## create the variable from parameters 
    if not isinstance ( var , ROOT.RooAbsReal ) : 
        var = ROOT.RooRealVar ( name , comment , *args )
        
    ## fix it, if needed 
    if isinstance ( fix , ( float , int , long ) ) :
        
        if fix < var.getMin() :
            logger.warning("Min-value for %s is redefined to be %s " % ( var.GetName() , fix ) )
            var.setMin ( fix )
            
        if fix > var.getMax() :
            logger.warning("Max-value for %s is redefined to be %s " % ( var.GetName() , fix ) )
            var.setMax ( fix )
            
        if not var.isConstant () : var.fix    ( fix )
        else                     : var.setVal ( fix )

    return var

# =============================================================================
## helper class to temporary change a range for the variable 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class RangeVar(object) :
    """
    Helper class to temporary change a range for the variable 
    """
    def __init__( self , var , vmin , vmax ) :
        self.var  = var
        self.vmin = min ( vmin , vmax ) 
        self.vmax = max ( vmin , vmax )
        self.omin = self.var.getMin ()
        self.omax = self.var.getMax ()
        
    def __enter__ ( self ) :
        self.omin = self.var.getMin ()
        self.omax = self.var.getMax ()
        self.var.setMin ( self.vmin ) 
        self.var.setMax ( self.vmax )
        return self
    
    def __exit__  ( self , *_ ) :        
        self.var.setMin ( self.omin ) 
        self.var.setMax ( self.omax )


# =============================================================================
## "parse" common arguments for fit 
def fitArgs ( name , dataset = None , *args , **kwargs ) :
    
    _args = []
    for a in args :
        if not isinstance ( a , ROOT.RooCmdArg ) :
            logger.warning ( '%s unknown argument type %s, skip it ' % ( name , type ( a ) ) ) 
            continue
        _args.append ( a )
        
    ncpu_added = False 
    for k,a in kwargs.iteritems() :
        
        if isinstance ( a , ROOT.RooCmdArg ) :
            logger.debug   ( '%s add keyword argument %s' % ( name , k ) )  
            _args.append ( a )
        elif k.upper() in ( 'WEIGHTED'   ,
                            'SUMW2'      ,
                            'SUMW2ERROR' ) and isinstance ( a , bool ) and dataset.isWeighted() :
            _args.append   (  ROOT.RooFit.SumW2Error( a ) )
            logger.debug   ( '%s add keyword argument %s/%s' % ( name , k , a ) )                 
        elif k.upper() in ( 'NCPU'       ,
                            'NCPUS'      ,
                            'NUMCPU'     ,
                            'NUMCPUS'    ) and isinstance ( a , int ) and 1<= a : 
            _args.append   (  ROOT.RooFit.NumCPU( a  ) ) 
            logger.debug   ( '%s add keyword argument %s/%s' % ( name , k , a ) )
            ncpu_added = True
        elif k.upper() in ( 'CONSTRAINT'  ,
                            'CONSTRAINTS' ,
                            'PARS'        ,
                            'PARAMS'      ,
                            'PARAMETER'   ,
                            'PARAMETERS'  ) :
            if   isinstance ( a , ROOT.RooCmdArg ) : _args.append ( a )
            elif isinstance ( a , (tuple,list)   ) :
                for ia in a :
                    if isinstance ( ia , ROOT.RooCmdArg ) : _args.append ( ia )
                    else : logger.warning( '%s skip keyword argument [%s] : %s' % ( name , k , a ) )
            else : logger.warning( '%s skip keyword argument %s: %s' % ( name , k , a ) )
                                        
        else : 
            logger.warning ( '%s unknown/illegal keyword argument type %s/%s, skip it ' % ( name , k , type ( a ) ) )
            continue            
        
    if not ncpu_added :
        logger.debug  ( '%s: NCPU is added ' % name ) 
        _args.append  (  ncpu ( len ( dataset ) ) )
            
    return tuple ( _args )
            

# =============================================================================
## @class PDF
#  The helper base class for implementation of 1D-pdfs 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-08-21
class PDF (object) :
    """
    Useful helper base class for implementation of PDFs for 1D-fit
    """
    def __init__ ( self , name ) :
        self.name         = name
        self._signals     = ROOT.RooArgSet ()
        self._backgrounds = ROOT.RooArgSet ()
        self._components  = ROOT.RooArgSet ()
        ## take care about sPlots 
        self._splots      = []
        
    ## get all declared components 
    def components  ( self ) : return self._components
    ## get all declared signals 
    def signals     ( self ) : return self._signals
    ## get all declared backgrounds 
    def backgrounds ( self ) : return self._backgrounds 

    ## adjust PDF a little bit to avoid zeroes 
    def adjust ( self , value ) :
        """
        """
        if hasattr ( self , 'adjusted' ) :
            logger.warning ( "PDF is already adjusted!")
            return
        
        self.adjusted = Adjust ( self.name , self.mass , self.pdf , value ) 
        self.old_pdf  = self.adjusted.old_pdf
        self.pdf      = self.adjusted.pdf
        
        
    ## Create vector of phases (needed for various polynomial forms)
    def makePhis    ( self , num ) :
        """
        Create vector of phases (needed for various polynomial forms)
        """
        self.phis     = []
        self.phi_list = ROOT.RooArgList()
        from math import pi 
        for i in range( 1 , num + 1 ) :
            phi_i = makeVar ( None ,
                              'phi%d_%s'      % ( i , self.name )  ,
                              '#phi_{%d}(%s)' % ( i , self.name )  ,
                              None , 0 ,  -0.75 * pi  , 1.25 * pi  )
            self.phis.append  ( phi_i ) 
            self.phi_list.add ( phi_i )
            
    ## make the actual fit (and optionally draw it!)
    #  @code
    #  r,f = model.fitTo ( dataset )
    #  r,f = model.fitTo ( dataset , weighted = True )    
    #  r,f = model.fitTo ( dataset , ncpu     = 10   )    
    #  r,f = model.fitTo ( dataset , draw = True , nbins = 300 )    
    #  @endcode 
    def fitTo ( self           ,
                dataset        ,
                draw   = False ,
                nbins  = 100   ,
                silent = False ,
                refit  = False , *args , **kwargs ) :
        """
        Perform the actual fit (and draw it)
        >>> r,f = model.fitTo ( dataset )
        >>> r,f = model.fitTo ( dataset , weighted = True )    
        >>> r,f = model.fitTo ( dataset , ncpu     = 10   )    
        >>> r,f = model.fitTo ( dataset , draw = True , nbins = 300 )    
        """
        if isinstance ( dataset , ROOT.TH1 ) :
            density = kwargs.pop ( 'density' , True   )
            chi2    = kwargs.pop ( 'chi2'    , False  )
            return self.fitHisto ( dataset , draw , silent , density , chi2 , *args , **kwargs ) 
        #
        ## treat the arguments properly
        #
        opts = fitArgs ( "PDF(%s).fitTo:" % self.name , dataset , *args , **kwargs )
        
        #
        ## define silent context
        with roo_silent ( silent ) :
            result =  self.pdf.fitTo ( dataset , ROOT.RooFit.Save () , *opts ) 
            if hasattr ( self.pdf , 'setPars' ) : self.pdf.setPars() 

        st = result.status()
        if 0 != st and silent :
            logger.warning ( 'PDF(%s).fitTo: status is %s. Refit in non-silent regime ' % ( self.name , st  ) )    
            return self.fitTo ( dataset , draw , nbins , False , refit , *args , **kwargs )
        
        for_refit = False
        if 0 != st   :
            for_refit = 'status' 
            logger.warning ( 'PDF(%s).fitTo: Fit status is %s ' % ( self.name , st   ) )
        #
        qual = result.covQual()
        if   -1 == qual and dataset.isWeighted() : pass
        elif  3 != qual :
            for_refit = 'covariance' 
            logger.warning ( 'PDF(%s).fitTo: covQual    is %s ' % ( self.name , qual ) ) 

        #
        ## check the integrals (when possible)
        #
        if hasattr ( self , 'alist2' ) :
            
            nsum = VE()            
            for i in self.alist2 :
                nsum += i.as_VE() 
                if i.getVal() > 0.95 * i.getMax() :
                    logger.warning ( 'PDF(%s).fitTo Variable %s == %s [close to maximum %s]'
                                     % ( self.name , i.GetName() , i.getVal () , i.getMax () ) )
                    
            if not dataset.isWeighted() : 
                nl = nsum.value() - 0.10 * nsum.error()
                nr = nsum.value() + 0.10 * nsum.error()
                if not nl <= len ( dataset ) <= nr :
                    logger.error ( 'PDF(%s).fitTo is problematic:  sum %s != %s ' % ( self.name , nsum , len( dataset ) ) )
                    for_refit = 'integral'

        #
        ## call for refit if needed
        #
        if refit and for_refit :
            logger.info ( 'PDF(%s).fitTo: call for refit:  %s/%s'  % ( self.name , for_refit , refit ) ) 
            if isinstance ( refit , ( int , long ) )  : refit -= 1
            else                                      : refit  = False
            return  self.fitTo ( dataset , draw , nbins , silent , refit , *args , **kwargs ) 


        if hasattr ( self.pdf , 'setPars' ) : self.pdf.setPars()
            
        for s in self.components () : 
            if hasattr ( s , 'setPars' ) : s.setPars()
        for s in self.backgrounds() :  
            if hasattr ( s , 'setPars' ) : s.setPars() 
        for s in self.signals    () : 
            if hasattr ( s , 'setPars' ) : s.setPars() 
                
        if not draw :
            return result, None 
        
        return result, self.draw ( dataset , nbins = nbins , silent = silent )

    
    ## helper method to draw set of components 
    def _draw ( self , what , frame , options , base_color ) :
        """ Helper method to draw set of components """
        i = 0 
        for cmp in what : 
            cmps = ROOT.RooArgSet( cmp ) 
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.Components ( cmps                        ) ,
                ROOT.RooFit.LineColor  ( base_color + i ) , *options ) 
            i += 1
            
            
            
                
    # ================================================================================
    ## draw fit results
    #  @code
    #  r,f = model.fitTo ( dataset )
    #  model.draw ( dataset , nbins = 100 ) 
    #  @endcode
    #  @param dataset  dataset to be drawn 
    #  @param nbins    binning scheme for frame/RooPlot 
    #  @param silent   silent mode ?
    #  @param data_options          drawing options for dataset
    #  @param signal_options        drawing options for `signal'     components    
    #  @param background_options    drawing options for `background' components 
    #  @param component_options     drawing options for 'other'      components 
    #  @param fit_options           drawing options for fit curve    
    #  @param base_signal_color     base color for signal components 
    #  @param base_background_color base color for background components
    #  @param base_component_color  base color for other components 
    #  @param data_overlay          draw points atop of fitting curves?  
    def draw ( self ,
               dataset               = None  ,
               nbins                 = 100   ,   ## Frame binning
               silent                = False ,   ## silent mode ?
               **kwargs                      ) :
        """    
        Visualize the fits results
        >>> r,f = model.draw ( dataset )
        >>> model.draw ( dataset , nbins = 100 )
        >>> model.draw ( dataset , base_signal_color  = ROOT.kGreen+2 )
        >>> model.draw ( dataset , data_options = (ROOT.RooFit.DrawOptions('zp'),) )        
        """
        #
        import Ostap.FitDraw as FD
        #
        ## again the context
        # 
        with roo_silent ( silent ) :
            
            if hasattr ( self , 'draw_var' ) and self.draw_var : drawvar = self.draw_var
            else                                               : drawvar = self.mass
            
            if nbins :  frame = drawvar.frame ( nbins )
            else     :  frame = drawvar.frame ()
            
            #
            ## draw invizible data (for normalzation of fitting curves)
            #
            data_options = kwargs.pop ( 'data_options' , FD.data_options )
            if dataset : dataset .plotOn ( frame , ROOT.RooFit.Invisible() , *data_options )
            
            ## draw various ``background'' terms
            boptions = kwargs.pop (     'background_options' , FD.   background_options )
            bbcolor  = kwargs.pop (  'base_background_color' , FD.base_background_color ) 
            if self.backgrounds () :
                self._draw( self.backgrounds() , frame , boptions , bbcolor )
                
            ## ugly :-(
            ct1options = kwargs.pop (     'crossterm1_options' , FD.   crossterm1_options )
            ct1bcolor  = kwargs.pop (  'base_crossterm1_color' , FD.base_crossterm1_color ) 
            if hasattr ( self , 'crossterms1' ) and self.crossterms1() : 
                self._draw( self.crossterms1() , frame , ct1options , ct1bcolor )

            ## ugly :-(
            ct2options = kwargs.pop (     'crossterm2_options' , FD.   crossterm1_options )
            ct2bcolor  = kwargs.pop (  'base_crossterm2_color' , FD.base_crossterm1_color )                 
            if hasattr ( self , 'crossterms2' ) and self.crossterms2() :
                self._draw( self.crossterms2() , frame , ct2options , ct2bcolor )

            ## draw ``other'' components
            coptions   = kwargs.pop (      'component_options' , FD.    component_options  )
            cbcolor    = kwargs.pop (   'base_component_color' , FD. base_component_color  ) 
            if self.components () :
                self._draw( self.components() , frame , coptions , cbcolor )

            ## draw ``signal'' components
            soptions   = kwargs.pop (         'signal_options' , FD.      signal_options  )
            sbcolor    = kwargs.pop (      'base_signal_color' , FD.   base_signal_color  ) 
            if self.signals    () :
                self._draw( self.signals() , frame , soptions , sbcolor )

            #
            ## the total fit curve
            #
            self.pdf .plotOn ( frame , *kwargs.pop ( 'total_fit_options' , FD. total_fit_options  ) )
            
            #
            ## draw data once more
            #
            if dataset : dataset  .plotOn ( frame , *data_options )            

            #
            ## suppress ugly axis labels
            #
            frame.SetXTitle ( '' )
            frame.SetYTitle ( '' )
            frame.SetZTitle ( '' )

            #
            ## Draw the frame!
            #
            frame.Draw()
            
            if kwargs :
                logger.warning("Ignored unknown options: %s" % kwargs.keys() )
            
            return frame 

    # =========================================================================
    ## fit the histogram (and draw it)
    #  @code
    #  histo = ...
    #  r,f = model.fitHisto ( histo , draw = True ) 
    #  @endcode 
    def fitHisto ( self , histo , draw = False , silent = False , density = True , chi2 = False , *args , **kwargs ) :
        """
        Fit the histogram (and draw it)

        >>> histo = ...
        >>> r,f = model.fitHisto ( histo , draw = True ) 
        
        """
        with RangeVar( self.mass , *(histo.xminmax()) ) : 
            
            ## convert it! 
            hdset     = H1D_dset ( histo , self.mass , density , silent )
            self.hset = hdset.dset
            
            if chi2 : return self.chi2fitTo ( self.hset , draw , None , silent , density , *args , **kwargs )
            else    : return self.fitTo     ( self.hset , draw , None , silent ,           *args , **kwargs )

    # =========================================================================
    ## make chi2-fit for binned dataset or histogram
    #  @code
    #  histo = ...
    #  r,f = model.chi2FitTo ( histo , draw = True ) 
    #  @endcode
    #  @todo add proper parsing of arguments for RooChi2Var 
    def chi2fitTo ( self,  dataset , draw = False , silent = False , density = True , *args , **kwargs ) :
        """ Chi2-fit for binned dataset or histogram
        >>> histo = ...
        >>> result , frame  = model.chi2FitTo ( histo , draw = True ) 
        """
        hdataset = dataset
        histo    = None 
        if isinstance  ( dataset , ROOT.TH1 ) :
            # if histogram, convert it to RooDataHist object:
            xminmax = dataset.xminmax() 
            with RangeVar( self.mass , *xminmax ) :                
                hdset     = H1D_dset ( dataset , self.mass , density , silent )
                self.hset = hdset.dset
                hdataset  = self.hset  
                histo     = dataset 
                
        with roo_silent ( silent ) : 

            lst1 = list ( fitArgs ( self.name , hdataset , *args , **kwargs ) )
            lst2 = []
            
            if       self.pdf.mustBeExtended () : lst2.append ( ROOT.RooFit.Extended ( True  ) )
            elif not self.pdf.canBeExtended  () : lst2.append ( ROOT.RooFit.Extended ( False ) )
            
            if not silent : lst2.append ( ROOT.RooFit.Verbose  () )
            if histo :
                if histo.natural() : lst2.append ( ROOT.RooFit.DataError ( ROOT.RooAbsData.Poisson ) )
                else               : lst2.append ( ROOT.RooFit.DataError ( ROOT.RooAbsData.SumW2   ) )  

            args_ = tuple ( lst2 + lst1  )
            #
            chi2 = ROOT.RooChi2Var ( rootID ( "chi2_" ) , "chi2(%s)" % self.name  , self.pdf , hdataset , *args_ )
            m    = ROOT.RooMinuit  ( chi2 ) 
            m.migrad   () 
            m.hesse    ()
            result = m.save ()
            
        if not draw :
            return result, None 
        
        return result, self.draw ( hdataset , nbins = None , silent = silent )

    # =========================================================================
    ## perform sPlot-analysis 
    #  @code
    #  r,f = model.fitTo ( dataset )
    #  model.sPlot ( dataset ) 
    #  @endcode 
    def sPlot ( self , dataset ) : 
        """ Make sPlot analysis
        >>> r,f = model.fitTo ( dataset )
        >>> model.sPlot ( dataset ) 
        """
        if not hasattr ( self , 'alist2' ) :
            logger.error ('PDF(%s) has not attribute "alist2", no sPlot is possible' % self.name ) 
            raise AttributeError('PDF(%s) his not equipped for sPlot'                % self.name )
        
        splot = ROOT.RooStats.SPlot ( rootID( "sPlot_" ) ,
                                      "sPlot"            ,
                                      dataset            ,
                                      self.pdf           ,
                                      self.alist2        )
        
        self._splots += [ splot ]
        
        return splot 
    

# =============================================================================
## helper base class for implementation  of various helper pdfs 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class MASS(PDF) :
    """
    Helper base class for implementation of various pdfs 
    """
    def __init__ ( self            ,
                   name            ,
                   mn       = None ,
                   mx       = None , 
                   mass     = None ,
                   mean     = None ,
                   sigma    = None ) : 

        ## intialize the base 
        PDF.__init__ ( self , name )
        
        m_name  = "m_%s"     % name
        m_title = "mass(%s)" % name

        if isinstance ( mass , tuple ) :
            mn      = mass [0]
            mx      = mass [1]
            mass    = None
            
        if isinstance ( mass , ROOT.TH1   ) :
            mn,mx   = mass.xminmax()
            m_title = mass.GetTitle ()
            mass    = None
            
        if isinstance ( mass , ROOT.TAxis ) :
            mn      = mass.GetXmin()
            mx      = mass.GetXmax()
            mass    = None
            
        if mass is None :
            if not isinstance ( mn , ( float , int , long ) ) :
                raise AttributeError( "MASS(%s): invalid 'min'-parameter %s" % ( name , mn ) ) 
            if not isinstance ( mx , ( float , int , long ) ) :
                raise AttributeError( "MASS(%s): invalid 'max'-parameter %s" % ( name , mx ) )
            
        #
        ## adjust the mass and edges, create if needed
        #
        self.mass = makeVar ( mass   ,
                              m_name , m_title ,
                              mass   , 
                              min ( mn , mx ) , max( mn , mx ) )
        #
        self._mn = self.mass.getMin ()
        self._mx = self.mass.getMax ()
        #
        _dm      = self._mx - self._mn 
        
        #
        ## mean-value
        # 
        self.mean = makeVar ( mean              ,
                              "mean_%s"  % name ,
                              "mean(%s)" % name , mean ,  self._mn  , self._mx )
        #
        if self.mean.isConstant() :
            if not self._mn <= self.mean.getVal() <= self._mx :
                raise AttributeError ( 'MASS(%s): Fixed mass %s is not in mass-range (%s,%s)' % ( name , self.mean.getVal() , self._mn , self._mx ) )
        elif hasattr ( self.mean , 'setMin' ) and hasattr( self.mean , 'setMax' ) : 
            self.mean.setMin ( max ( self.mean.getMin () , self.mass.getMin() - 0.1 * _dm ) )
            self.mean.setMax ( min ( self.mean.getMax () , self.mass.getMax() + 0.1 * _dm ) )
            logger.debug ( 'MASS(%s) Mean range is redefined to be (%s,%s)' % ( name , self.mean.getMin() , self.mean.getMax() ) ) 
            
        #
        ## sigma
        #
        sigma_max  = 2.0 * _dm / math.sqrt ( 12 )
        self.sigma = makeVar ( sigma               ,
                               "sigma_%s"   % name ,
                               "#sigma(%s)" % name , sigma , 0.01 * sigma_max , 0 , sigma_max )
        


# =============================================================================
## @class PDF2
#  The helper base class for implementation of 2D-pdfs 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-08-21
class PDF2 (PDF) :
    """
    Useful helper base class for implementation of PDFs for 2D-fit
    """
    def __init__ ( self , name , xvar = None , yvar = None ) : 

        PDF.__init__ ( self , name )
        
        self.varx         = xvar 
        self.vary         = yvar 
        self.x            = xvar 
        self.y            = yvar 
        self.m1           = xvar ## ditto
        self.m2           = yvar ## ditto 

    # =========================================================================
    ## make the actual fit (and optionally draw it!)
    #  @code
    #  r,f = model.fitTo ( dataset )
    #  r,f = model.fitTo ( dataset , weighted = True )    
    #  r,f = model.fitTo ( dataset , ncpu     = 10   )    
    #  r,f = model.fitTo ( dataset , draw = True , nbins = 300 )    
    #  @endcode 
    def fitTo ( self           , 
                dataset        ,
                draw   = False ,
                nbins  =    50 ,
                ybins  =  None , 
                silent = False ,
                refit  = False , *args , **kwargs ) :
        """
        Perform the actual fit (and draw it)
        >>> r,f = model.fitTo ( dataset )
        >>> r,f = model.fitTo ( dataset , weighted = True )    
        >>> r,f = model.fitTo ( dataset , ncpu     = 10   )    
        >>> r,f = model.fitTo ( dataset , draw = True , nbins = 300 )    
        """
        if isinstance ( dataset , ROOT.TH2 ) :
            density = kwargs.pop ( 'density' , True  ) 
            chi2    = kwargs.pop ( 'chi2'    , False ) 
            return self.fitHisto ( dataset   , draw , silent , density , chi2 , *args , **kwargs )

        
        result,f = PDF.fitTo ( self    ,
                               dataset ,
                               False   , ## false here!
                               nbins   ,
                               silent  ,
                               refit   , *args , **kwargs ) 
        if not draw :
            return result , None
        
        ## 2D 
        if 1< nbins and isinstance ( ybins , ( int , long ) ) and 1<ybins :
            return result, self.draw ( None , dataset , nbins , ybins , silent = silent )
        
        if     1<= nbins : return result, self.draw1 ( dataset ,  nbins , silent = silent )
        elif  -1>= nbins : return result, self.draw2 ( dataset , -nbins , silent = silent )

        ## return 2D 
        return result, self.draw ( None , dataset , silent = silent )
    
    # =========================================================================
    ## draw the projection over 1st variable
    #
    #  @code
    #  r,f = model.fitTo ( dataset ) ## fit dataset
    #  fx  = model.draw1 ( dataset , nbins = 100 ) ## draw results
    #
    #  f1  = model.draw1 ( dataset , nbins = 100 , in_range = (2,3) ) ## draw results
    #
    #  model.m2.setRange ( 'QUQU2' , 2 , 3 ) 
    #  f1  = model.draw1 ( dataset , nbins = 100 , in_range = 'QUQU2') ## draw results
    #
    #  @endcode 
    def draw1 ( self            ,
                dataset  = None ,
                nbins    = 100  ,
                silent   = True ,
                in_range = None , *args , **kwargs ) :
        """ Draw the projection over 1st variable
        
        >>> r,f = model.fitTo ( dataset ) ## fit dataset
        >>> fx  = model.draw1 ( dataset , nbins = 100 ) ## draw results
        
        >>> f1  = model.draw1 ( dataset , nbins = 100 , in_range = (2,3) ) ## draw results

        >>> model.m2.setRange ( 'QUQU2' , 2 , 3 ) 
        >>> f1  = model.draw1 ( dataset , nbins = 100 , in_range = 'QUQU2') ## draw results
        
        """
        if in_range and isinstance ( in_range , tuple ) and 2 == len ( in_range ) :
            self.m2.setRange ( 'aux_rng2' , in_range[0] , in_range[1] )
            in_range = 'aux_rng2'
            
        return self.draw ( self.m1  , 
                           dataset  ,
                           nbins    ,
                           20       , ## fake 
                           silent   ,
                           in_range , *args , **kwargs )
    
    # =========================================================================
    ## draw the projection over 2nd variable
    #
    #  @code
    #  r,f = model.fitTo ( dataset ) ## fit dataset
    #  fy  = model.draw2 ( dataset , nbins = 100 ) ## draw results
    #
    #  f2  = model.draw2 ( dataset , nbins = 100 , in_range = (2,3) ) ## draw results
    #
    #  model.m1.setRange ( 'QUQU1' , 2 , 3 ) 
    #  f2  = model.draw2 ( dataset , nbins = 100 , in_range = 'QUQU1') ## draw results
    #
    #  @endcode 
    def draw2 ( self            ,
                dataset  = None ,
                nbins    = 100  ,
                silent   = True ,
                in_range = None , *args , **kwargs ) :
        """
        Draw the projection over 2nd variable
        
        >>> r,f = model.fitTo ( dataset ) ## fit dataset
        >>> fy  = model.draw2 ( dataset , nbins = 100 ) ## draw results
        
        >>> f2  = model.draw2 ( dataset , nbins = 100 , in_range = (2,3) ) ## draw results

        >>> model.m1.setRange ( 'QUQU1' , 2 , 3 ) 
        >>> f2  = model.draw2 ( dataset , nbins = 100 , in_range = 'QUQU1') ## draw results

        """
        if in_range and isinstance ( in_range , tuple ) and 2 == len ( in_range ) :
            self.m1.setRange ( 'aux_rng1' , in_range[0] , in_range[1] )
            in_range = 'aux_rng1'

        return self.draw ( self.m2 ,
                           dataset ,
                           nbins   ,
                           20      , ## fake
                           silent  , in_range , *args , **kwargs )

    # =========================================================================
    ## draw as 2D-histograms 
    def draw_H2D ( self           ,
                   dataset = None ,  
                   xbins   = 20   ,
                   ybins   = 20   ) :
        """
        Make/draw 2D-histograms 
        """
        
        _xbins = ROOT.RooFit.Binning ( xbins ) 
        _ybins = ROOT.RooFit.Binning ( ybins ) 
        _yvar  = ROOT.RooFit.YVar    ( self.m2 , _ybins )
        _clst  = ROOT.RooLinkedList  ()
        hdata  = self.pdf.createHistogram ( hID() , self.m1 , _xbins , _yvar )
        hpdf   = self.pdf.createHistogram ( hID() , self.m1 , _xbins , _yvar )
        hdata.SetTitle(';;;')
        hpdf .SetTitle(';;;')
        _lst   = ROOT.RooArgList ( self.m1 , self.m2 )  
        if dataset : dataset.fillHistogram( hdata , _lst ) 
        self.pdf.fillHistogram  ( hpdf , _lst )
        
        if not ROOT.gROOT.IsBatch() : 
            hdata.lego ()
            hpdf .Draw ( 'same surf')
        
        return hpdf , hdata 
    
    # =========================================================================
    ## make 1D-plot
    def draw ( self                         ,
               drawvar               = None ,
               dataset               = None ,
               nbins                 =  100 ,
               ybins                 =   20 ,
               silent                = True ,
               in_range              = None ,
               **kwargs                     ) : 
        """
        Make 1D-plot:
        """
        
        #
        ## special case:  do we need it? 
        # 
        if not drawvar : return self.draw_H2D( dataset , nbins , ybins )

        ## copy arguments:
        args = kwargs.copy ()
        
        import Ostap.FitDraw as FD
        if in_range :
            data_options        = args.pop (       'data_options' , FD.         data_options )
            background_options  = args.pop ( 'background_options' , FD. background2D_options )
            signal_options      = args.pop (     'signal_options' , FD.       signal_options )
            component_options   = args.pop (  'component_options' , FD.    component_options )
            crossterm1_options  = args.pop ( 'crossterm1_options' , FD.   crossterm1_options )
            crossterm2_options  = args.pop ( 'crossterm2_options' , FD.   crossterm2_options )
            total_fit_options   = args.pop (  'total_fit_options' , FD.    total_fit_options )
            
            data_options       += ROOT.RooFit.CutRange        ( in_range ) , 
            signal_options     += ROOT.RooFit.ProjectionRange ( in_range ) , 
            background_options += ROOT.RooFit.ProjectionRange ( in_range ) , 
            component_options  += ROOT.RooFit.ProjectionRange ( in_range ) , 
            crossterm1_options += ROOT.RooFit.ProjectionRange ( in_range ) , 
            crossterm2_options += ROOT.RooFit.ProjectionRange ( in_range ) , 
            total_fit_options  += ROOT.RooFit.ProjectionRange ( in_range ) , 
            
            args [       'data_options' ] =       data_options
            args [     'signal_options' ] =     signal_options
            args [ 'background_options' ] = background_options
            args [  'component_options' ] =  component_options
            args [ 'crossterm1_options' ] = crossterm1_options
            args [ 'crossterm2_options' ] = crossterm2_options
            args [  'total_fit_options' ] =  total_fit_options
            
        background_options    = args.pop ( 'background_options'    , FD.background2D_options    )
        base_background_color = args.pop ( 'base_background_color' , FD.base_background2D_color )
        args [ 'background_options'    ] = background_options
        args [ 'base_background_color' ] = base_background_color
        
        
        #
        ## redefine the drawing variable:
        # 
        self.draw_var = drawvar
        
        #
        ## delegate the actual drawing to the base class
        # 
        return PDF.draw ( self    ,
                          dataset ,
                          nbins   ,
                          silent  ,  **args ) 
    
    # =========================================================================
    ## fit the 2D-histogram (and draw it)
    #
    #  @code
    #
    #  histo = ...
    #  r,f = model.fitHisto ( histo )
    #
    #  @endcode
    def fitHisto ( self            ,
                   histo           ,
                   draw    = False ,
                   silent  = False ,
                   density = True  ,
                   chi2    = False , *args , **kwargs ) :
        """
        Fit the histogram (and draw it)
        
        >>> histo = ...
        >>> r,f = model.fitHisto ( histo , draw = True )
        
        """

        xminmax = histo.xminmax()
        yminmax = histo.yminmax()        
        with RangeVar( self.m1 , *xminmax ) , RangeVar ( self.m2 , *yminmax ): 
            
            ## convert it! 
            self.hdset = H2D_dset ( histo , self.m1 , self.m2  , density , silent )
            self.hset  = self.hdset.dset
                
            ## fit it!!
            return self.fitTo ( self.hset      ,
                                draw           ,
                                histo.nbinsx() ,
                                histo.nbinsy() ,
                                silent         , *args , **kwargs ) 


# =============================================================================
## simple convertor of 1D-histo to data set
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H1D_dset(object) :
    """
    Simple convertor of 1D-histogram into data set

    h1   = ...
    dset = H1D_dset ( h1 )
    """
    def __init__ ( self            , 
                   histo           ,
                   mass    = None  ,
                   density = True  ,
                   silent  = False ) :
        #
        ## use mass-variable
        #
        name = histo.GetName() 
        if mass : self.mass = mass 
        else    : self.mass = makeVar ( mass , 'm_%s' % name , 'mass(%s)' % name , None , *(histo.xminmax()) )

        self.impDens = density 
        
        with roo_silent ( silent ) :  
            
            self.var1    = self.mass
            self.x       = self.mass
            self.vlst    = ROOT.RooArgList    ( self.mass )
            self.vimp    = ROOT.RooFit.Import ( histo     , density )
            self.dset    = ROOT.RooDataHist   (
                rootID ( 'hds_' ) ,
                "Data set for histogram '%s'" % histo.GetTitle() ,
                self.vlst  ,
                self.vimp  )
            
# =============================================================================
## simple convertor of 1D-histogram into PDF
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H1D_pdf(H1D_dset,PDF) :
    """
    Simple convertor of 1D-histogram into PDF 
    """
    def __init__ ( self            ,
                   name            ,
                   histo           ,
                   mass    = None  ,
                   density = True  ,
                   silent  = False ) :
        
        H1D_dset.__init__ ( self , histo , mass , density , silent )
        PDF     .__init__ ( self , name  )
        
        with roo_silent ( silent ) : 
            #
            ## finally create PDF :
            self.vset  = ROOT.RooArgSet  ( self.mass )        
            self.pdf   = ROOT.RooHistPdf (
                'hpdf_%s'            % name ,
                'HistoPDF(%s/%s/%s)' % ( name , histo.GetName() , histo.GetTitle() ) , 
                self.vset  , 
                self.dset  )
            
        ## and declare it be be a "signal"
        self.signals().add ( self.pdf ) 
        
# =============================================================================
## simple convertor of 2D-histo to data set
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H2D_dset(object) :
    """
    Simple convertor of 2D-histogram into data set
    """
    def __init__ ( self            ,
                   histo2          ,
                   mass    = None  ,
                   mass2   = None  ,
                   density = True  ,
                   silent  = False ) :
        #
        ## use mass-variable
        #
        name         = histo2.GetName() 
        self.mass    = makeVar ( mass  , 'm_%s'  % name , 'mass (%s)' % name , None , *(histo2.xminmax()) )
        self.mass1   = self.mass 
        self.mass2   = makeVar ( mass2 , 'm2_%s' % name , 'mass2(%s)' % name , None , *(histo2.yminmax()) )

        self.impDens = density 
        self.var1    = self.mass1
        self.var2    = self.mass2
        self.x       = self.var1 
        self.y       =   self.var2
        
        with roo_silent ( silent ) : 

            self.vlst  = ROOT.RooArgList    ( self.mass1 , self.mass2 )
            self.vimp  = ROOT.RooFit.Import ( histo2 , density )
            self.dset  = ROOT.RooDataHist   (
                rootID ( 'hds_' ) ,
                "Data set for histogram '%s'" % histo2.GetTitle() ,
                self.vlst  ,
                self.vimp  )
            

# =============================================================================
## simple convertor of 2D-histogram into PDF
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H2D_pdf(H2D_dset,PDF2) :
    """
    Simple convertor of 2D-histogram into PDF 
    """
    def __init__ ( self            ,
                   name            ,
                   histo2          ,
                   mass    = None  , 
                   mass2   = None  ,
                   density = True  ,
                   silent  = False ) :
        
        H2D_dset.__init__ ( self , histo2 ,      mass  ,      mass2 , density , silent )
        PDF2    .__init__ ( self , name   , self.mass1 , self.mass2 ) 

        self.vset  = ROOT.RooArgSet  ( self.mass , self.mass2 )
        
        #
        ## finally create PDF :
        #
        with roo_silent ( silent ) : 
            self.pdf    = ROOT.RooHistPdf (
                'hpdf_%s'            % name ,
                'HistoPDF(%s/%s/%s)' % ( name , histo2.GetName() , histo2.GetTitle() ) , 
                self.vset  , 
                self.dset  )

# =============================================================================
## @class Fit1D
#  The actual model for 1D-mass fits 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Fit1D (PDF) :
    """
    The actual model for generic 1D-fits 
    """
    def __init__ ( self                       , 
                   signal                     , ## the main signal 
                   background       = None    , ## the main background 
                   othersignals     = []      , ## additional signal         components
                   otherbackgrounds = []      , ## additional background     components
                   others           = []      , ## additional non-classified components 
                   suffix           = ''      ,
                   name             = ''      ,
                   bpower           = 0       ) :  

        #
        if not name and signal.name : name = signal.name
        #
        PDF.__init__ ( self , name + suffix )
        #
        self.suffix     = suffix 
        self.signal     =      signal 
        self.mass       = self.signal.mass
        #
        from Ostap.FitBkgModels import makeBkg
        if background : self.background = makeBkg ( background , 'Background' + suffix , self.mass )
        else          : self.background = makeBkg ( bpower     , 'Background' + suffix , self.mass )
        #
        self.s = makeVar ( None , "S"+suffix , "Signal"     + suffix , None , 0 , 1.e+6 )
        self.b = makeVar ( None , "B"+suffix , "Background" + suffix , None , 0 , 1.e+6 )
        
        self.S = self.s
        self.B = self.b
        
        self.S_name = self.s.GetName()
        self.B_name = self.b.GetName()

        #
        self.alist1 = ROOT.RooArgList (
            self.signal     .pdf ,
            self.background .pdf 
            )
        # 
        self.alist2 = ROOT.RooArgList (
            self.s  ,
            self.b
            )
        
        self.nums = [ self.s , self.b ]

        #
        ## update the lists of PDFs (for drawing)
        #
        self.signals     ().add ( self.signal     .pdf )
        self.backgrounds ().add ( self.background .pdf )

        self.more_signals       = othersignals
        self.more_backgrounds   = otherbackgrounds
        self.more_components    = others

        #
        ## treat additional signals
        #
        for c in self.more_signals : 
            
            i = len ( self.signals() )
            
            if   isinstance ( c ,  ROOT.RooAbsPdf ) :
                self.alist1    .add ( c     )
                self.signals() .add ( c     ) 
            elif hasattr    ( c ,'pdf' )            :
                self.alist1    .add ( c.pdf )
                self.signals() .add ( c.pdf ) 
            else :
                logger.warning('Fit1D(%s) Unknown signal component type %s, skip it!' % ( self.name , type(c) ) ) 
                continue 
            
            si = makeVar ( None                            ,
                           "S%s_%d"  % ( suffix , i ) ,
                           "S(%d)%s" % ( i , suffix ) , None  ,  10 , 0  ,  1.e+6 )            
            self.alist2.add  ( si )
            
            setattr ( self , si.GetName() , si )             
            self.nums.append ( si ) 

        #
        ## treat additional backgounds 
        #
        for c in self.more_backgrounds : 
            
            i = len ( self.backgrounds() )
            
            if   isinstance ( c ,  ROOT.RooAbsPdf ) :
                self.alist1         .add ( c     )
                self.backgrounds () .add ( c     ) 
            elif hasattr    ( c ,'pdf' )            :
                self.alist1         .add ( c.pdf )
                self.backgrounds () .add ( c.pdf ) 
            else :
                logger.warning('Fit1D(%s) Unknown background component type %s, skip it!' % ( self.name , type(c) ) ) 
                continue 
            
            si = makeVar ( None                       ,
                           "B%s_%d"  % ( suffix , i ) ,
                           "B(%d)%s" % ( i , suffix ) , None  ,  10 , 0  ,  1.e+6 )            
            self.alist2.add  ( si )
            
            setattr ( self , si.GetName() , si )             
            self.nums.append ( si ) 


        #
        ## treat additional components
        #
        for c in self.more_components : 
            
            i = len ( self.components() )
            
            if   isinstance ( c ,  ROOT.RooAbsPdf ) :
                self.alist1         .add ( c     )
                self.components () .add ( c     ) 
            elif hasattr    ( c ,'pdf' )            :
                self.alist1         .add ( c.pdf )
                self.components () .add ( c.pdf ) 
            else :
                logger.warning('Fit1D(%s) Unknown additional component type %s, skip it!' % ( self.name , type(c) ) ) 
                continue 
            
            si = makeVar ( None                       ,
                           "O%s_%d"  % ( suffix , i ) ,
                           "O(%d)%s" % ( i , suffix ) , None  ,  10 , 0  ,  1.e+6 )            
            self.alist2.add  ( si )
            
            setattr ( self , si.GetName() , si )             
            self.nums.append ( si ) 
            
            
        #
        ## final PDF
        # 
        self.pdf  = ROOT.RooAddPdf  ( "model_"    + suffix ,
                                      "model(%s)" % suffix ,
                                      self.alist1 ,
                                      self.alist2 )


# =============================================================================
## @class Fit2D
#  The actual model for 2D-fits
#
#  @code
# 
#  model   = Models.Fit2D (
#      signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
#      signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y ) ,
#      power1   = 1 , 
#      power2   = 1 )
#
#  r,f = model.fitTo ( dataset ) ## fit dataset 
#
#  print r                       ## get results  
#
#  fx  = model.draw1 ()          ## visualize X-projection
#  fy  = model.draw2 ()          ## visualize X-projection
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Fit2D (PDF2) :
    """
    The actual model for 2D-fits
    
    >>>  model   = Models.Fit2D (
    ...      signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
    ...      signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y ) ,
    ...      power1   = 1 , 
    ...      power2   = 1 )
    >>> r,f = model.fitTo ( dataset ) ## fit dataset 
    >>> print r                       ## get results  
    >>> fx  = model.draw1 ()          ## visualize X-projection
    >>> fy  = model.draw2 ()          ## visualize X-projection

    """
    def __init__ ( self              ,
                   #
                   signal_1          , 
                   signal_2          ,
                   suffix = ''       ,
                   #
                   bkg1       = None ,
                   bkg2       = None ,
                   bkgA       = None ,
                   bkgB       = None ,
                   bkg2D      = None ,
                   #
                   power1     = 0    ,
                   power2     = 0    ,
                   powerA     = 0    ,
                   powerB     = 0    ,
                   #
                   ## main components:
                   ss         = None , ## signal    (1) * signal     (2)
                   sb         = None , ## signal    (1) * bakcground (2) 
                   bs         = None , ## background(1) * signal     (2)
                   bb         = None , ## background-2D 
                   ## additional components 
                   components = []   ,
                   name       = ''   ) :
        
        self._crossterms1 = ROOT.RooArgSet()
        self._crossterms2 = ROOT.RooArgSet()
        
        self.suffix    = suffix 
        self.signal1   = signal_1
        self.signal2   = signal_2

        #
        ## initialize base class
        #
        if not name and signal_1.name and signal_2.name :
            name = signal_1.name +'_AND_' + signal_2.name + ' _ '+ suffix
            
        PDF2.__init__ ( self , name , signal_1.mass , signal_2.mass ) 
        
        #
        ## First component: Signal(1) and Signal(2)
        # 
        self.ss_pdf = ROOT.RooProdPdf ( "S1S2pdf" + suffix ,
                                        "Sig(1) x Sig(2)"  ,
                                        self.signal1.pdf   ,
                                        self.signal2.pdf   )

        from Ostap.FitBkgModels import makeBkg 
        if bkg1 : self.bkg1 = makeBkg ( bkg1   , 'Bkg(1)' + suffix , self.m1 )
        else    : self.bkg1 = makeBkg ( power1 , 'Bkg(1)' + suffix , self.m1 )
        
        #
        ## Second component: Background(1) and Signal(2)
        # 
        self.bs_pdf = ROOT.RooProdPdf ( "B1S2pdf" + suffix  ,
                                        "Bkg(1) x Sig(2)"   ,
                                        self.bkg1.pdf       ,
                                        self.signal2.pdf    )
        
        from Ostap.FitBkgModels import makeBkg 
        if bkg2 : self.bkg2 = makeBkg ( bkg2   , 'Bkg(2)' + suffix , self.m2 )
        else    : self.bkg2 = makeBkg ( power2 , 'Bkg(2)' + suffix , self.m2 )
        
        #
        ## Third component:  Signal(1) and Background(2)
        # 
        self.sb_pdf = ROOT.RooProdPdf ( "S1B2pdf" + suffix  ,
                                        "Sig(1) x Bkg(2)"   ,
                                        self.signal1.pdf    ,
                                        self.bkg2.pdf       )
        
        ## 
        self._bkgs = ( bkg1 , bkg2 , bkgA , bkgB ) 
        #
        ## fourth component: Background(1) and Background(2) 
        #
        if bkg2D : self._bb2D  = bkg2D
        # 
        if   bkg2D and isinstance ( bkg2D , ROOT.RooAbsPdf ) : self.bb_pdf = bkg2D 
        elif bkg2D and hasattr    ( bkg2D , 'pdf'          ) : self.bb_pdf = bkg2D.pdf
        else     :            
            
            from Ostap.FitBkgModels import makeBkg 
            if bkgA : self.bkgA = makeBkg ( bkgA   , 'Bkg(A)' + suffix , self.m1 )
            else    : self.bkgA = makeBkg ( powerA , 'Bkg(A)' + suffix , self.m1 )
            if bkgB : self.bkgB = makeBkg ( bkgB   , 'Bkg(B)' + suffix , self.m2 )
            else    : self.bkgB = makeBkg ( powerB , 'Bkg(B)' + suffix , self.m2 )
            
            self.bb_pdf = ROOT.RooProdPdf ( "B1B2pdf" + suffix ,
                                            "Bkg(A) x Bkg(B)"  ,
                                            self.bkgA.pdf      ,
                                            self.bkgB.pdf      )
        #
        ## coefficients
        #
        self.ss = makeVar ( ss   ,
                            "S1S2"          + suffix ,
                            "Sig(1)&Sig(2)" + suffix , None , 1000  , 0 ,  1.e+6 )
        self.sb = makeVar ( sb   ,
                            "S1B2"          + suffix ,
                            "Sig(1)&Bkg(2)" + suffix , None ,  100  , 0 ,  1.e+6 )
        self.bs = makeVar ( bs   ,
                            "B1S2"          + suffix ,
                            "Bkg(1)&Sig(2)" + suffix , None ,  100  , 0 ,  1.e+6 )
        self.bb = makeVar ( bb   ,
                            "B1B2"          + suffix ,
                            "Bkg(1)&Bkg(2)" + suffix , None ,   10  , 0 ,  1.e+6 )

        self.SS_name = self.ss.GetName()
        self.SB_name = self.sb.GetName()
        self.BS_name = self.bs.GetName()
        self.BB_name = self.bb.GetName()
        
        self.alist1 = ROOT.RooArgList ( self.ss_pdf , self.sb_pdf ,
            self.bs_pdf ,
            self.bb_pdf )
        self.alist2 = ROOT.RooArgList (
            self.ss ,
            self.sb ,
            self.bs ,
            self.bb )

        #
        ## treat additional components (if specified)
        # 
        self.other       = components
        self._cmps       = []
        icmp = 0 
        for cmp in self.other :

            icmp += 1
            
            if   isinstance ( cmp , ROOT.RooAbsPdf         ) : pass 
            elif hasattr    ( cmp , 'pdf'                  ) :
                self._cmps += [ cmp ] 
                cmp = cmp.pdf 
            elif isinstance ( cmp , ( float , int , long ) ) and not isinstance ( cmp , bool ) :
                px  = ROOT.RooPolynomial ( 'Px%d'    % icmp + suffix ,
                                           'Px(%d)'  % icmp + suffix , self.m1 ) 
                py  = ROOT.RooPolynomial ( 'Py%d'    % icmp + suffix ,
                                           'Py(%d)'  % icmp + suffix , self.m2 ) 
                cmp = ROOT.RooProdPdf    ( "Pxy%d"   % icmp + suffix ,
                                           "Pxy(%d)" % icmp + suffix , px , py )  
                self._cmps += [ px,py,cmp]
            else :
                logger.error( 'Unknown type of component %d %d ' % ( imcp , type(cmp) ) )

                
            nn = makeVar ( None ,
                           'Cmp%d'   % icmp + suffix ,
                           'Cmp(%d)' % icmp + suffix ,
                           None ,  100  ,  0 , 1.e+6 )  
            self._cmps += [ nn ]

            self.alist1.add ( cmp )
            self.alist2.add ( nn  )
            
            self.components ().add ( cmp ) 
            
        #
        ## build the final PDF 
        # 
        self.pdf  = ROOT.RooAddPdf  ( "model2D"      + suffix ,
                                      "Model2D(%s)"  % suffix ,
                                      self.alist1 ,
                                      self.alist2 )


        self.signals     ().add ( self.ss_pdf )
        self.backgrounds ().add ( self.bb_pdf )
        self.crossterms1 ().add ( self.sb_pdf      ) ## cross-terms 
        self.crossterms2 ().add ( self.bs_pdf      ) ## cross-terms 


    ## get all declared components 
    def crossterms1 ( self ) : return self._crossterms1
    ## get all declared components 
    def crossterms2 ( self ) : return self._crossterms2
        

# =============================================================================
## simple class to adjust certaint PDF to avoid zeroes 
class Adjust(object) :
    """
    Simple class to adjust certain PDF to avoid zeroes 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,
                   mass             , 
                   pdf              ,
                   value    = 1.e-3 ) : 

        self.name    = name 
        self.mass    = mass
        self.old_pdf = pdf
        
        self.p0_pdf  = ROOT.RooPolynomial( 'p0_%s'     % name ,
                                           'poly0(%s)' % name , self.mass ) 

        
        self.num_f   = makeVar ( None  , 'valueT_%s'   % name ,
                                 'value/true(%s)'      % name ,
                                 None  ,
                                 value , 
                                 0     , 1 )
        
        self.alist1 = ROOT.RooArgList (
            self.p0_pdf  ,   
            self.old_pdf 
            )
        
        self.alist2 = ROOT.RooArgList (
            self.num_f    ,
            )
        #
        ## final PDF
        # 
        self.pdf  = ROOT.RooAddPdf  ( "adjust_"    + name ,
                                      "Adjust(%s)" % name ,
                                      self.alist1 ,
                                      self.alist2 )
        
        
# =============================================================================
## @class Convolution
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-07-13
class Convolution(object):
    """
    Helper class to make a convolution PDF 
    """
    def __init__ ( self           ,
                   name           ,
                   pdf            ,
                   mass           , 
                   convolution    ,
                   useFFT  = True ) :
        
        ## store "old" pdf 
        self.orig_pdf = pdf
        self.mass     = mass

        if   isinstance ( convolution ,   ROOT.RooAbsPdf       ) : self.convolution = convolution
        elif isinstance ( convolution , ( float , int , long ) ) and not isinstance ( convolution , bool ) :
            self.cnv_mean  = makeVar (
                0.0  ,
                'CnvMean'       + name ,
                'cnv_mean (%s)' % name , 
                0.0  , 0 ) 
            self.cnv_sigma = makeVar (
                None ,
                'CnvSigma'      + name ,
                'cnv_sigma(%s)' % name ,
                convolution ,
                convolution ,
                0.05 * convolution , 10 * convolution )
            self.cnv_gauss = ROOT.RooGaussian (
                'CnvGauss'     + name , 
                'CnvGauss(%s)' % name ,
                self.mass , self.cnv_mean , self.cnv_sigma )
            self.convolution = self.cnv_gauss
        else :
            raise AttributeError ( 'Unknown convolution type %s ' % convolution )

        #
        if useFFT :
            
            nb = 40000
            if hasattr ( self , 'cnv_sigma' ) :
                dm  = mass.getMax() - mass.getMin()
                dm /= self.cnv_sigma
                nb  = max ( nb , 100 * int (  dm ) ) 
                logger.debug('Convolution: choose #bins %d' % nb )
                
            self.mass.setBins ( nb , 'cache' ) 
            self.pdf = ROOT.RooFFTConvPdf ( 'FFT'     + name ,
                                            'FFT(%s)' % name ,
                                            self.mass , self.orig_pdf , self.convolution )
            self.pdf.setBufferFraction ( 0.25 )
            
        else      :
            self.pdf = ROOT.RooNumConvPdf ( 'CNV'     + name ,
                                            'CNV(%s)' % name ,
                                            self.mass , self.orig_pdf , self.convolution )
            if isinstance ( self.convolution , ROOT.RooGaussian ) :
                if hasattr ( self , 'cnv_mean' ) and hasattr ( self , 'cnv_sigma' ) :
                    self.pdf.setConvolutonWindow ( self.cnv_mean , self.cnv_sigma , 7 )



# =============================================================================
## @class Generic1D_pdf
#  "Wrapper" over generic RooFit (1D)-pdf
#  @code
#     
#  raw_pdf = RooGaussian  ( ...     )
#  pdf     = Generic1D_pdf ( raw_pdf , varx = x )  
# 
#  @endcode 
#  If more functionality is required , more actions are possible:
#  @code
#  ## for sPlot 
#  pdf.alist2 = ROOT.RooArgList ( n1 , n2 , n3 ) ## for sPlotting 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-03-29
class Generic1D_pdf(PDF) :
    """
    Wrapper for generic RooFit pdf
    # 
    # raw_pdf = RooGaussian   ( ...     )
    # pdf     = Generic1D_pdf ( raw_pdf , varx = x )
    # 
    """
    ## constructor 
    def __init__ ( self , pdf , varx = None , name = None ) :
        if not name : name = pdf.GetName()
        PDF  . __init__ ( self , name )
        self.pdf  = pdf
        self.varx = varx
        self.mass = varx
        self.x    = varx
        self.signals().add ( self.pdf )

# =============================================================================
## @class Generic2D_pdf
#  "Wrapper" over generic RooFit (2D)-pdf
#  @code
#     
#  raw_pdf = 
#  pdf     = Generic1D_pdf ( raw_pdf )  
# 
#  @endcode 
#  If more functionality is required , more actions are possible:
#  @code
#  ## for sPlot 
#  pdf.alist2 = ROOT.RooArgList ( n1 , n2 , n3 ) ## for sPlotting 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-03-29
class Generic2D_pdf(PDF) :
    """
    Wrapper for generic RooFit pdf
    # 
    # raw_pdf = 
    # pdf     = Generic2D_pdf ( raw_pdf )
    # 
    """
    ## constructor 
    def __init__ ( self , pdf , varx = None , vary = None , name = None ) :
        if not name : name = pdf.GetName()
        PDF2  . __init__ ( self , name , varx , vary )
        self.pdf = pdf

        
# =============================================================================
if '__main__' == __name__ :
    
    from Ostap.Line import line 
    logger.info ( __file__ + '\n' + line  )
    logger.info ( 80*'*' )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
