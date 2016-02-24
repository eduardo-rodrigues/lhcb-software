// Class: ReadBDT
// Automatically generated by MethodBase::MakeClass
//

/* configuration options =====================================================

#GEN -*-*-*-*-*-*-*-*-*-*-*- general info -*-*-*-*-*-*-*-*-*-*-*-

Method         : BDT::BDT
TMVA Release   : 4.1.4         [262404]
ROOT Release   : 5.34/05       [336389]
Creator        : laura
Date           : Thu Sep 19 15:43:56 2013
Host           : Linux ubuntu 3.2.0-23-generic #36-Ubuntu SMP Tue Apr 10 20:39:51 UTC 2012 x86_64 x86_64 x86_64 GNU/Linux
Dir            : /home/laura/root/tmva_original/test
Training events: 101865
Analysis type  : [Classification]


#OPT -*-*-*-*-*-*-*-*-*-*-*-*- options -*-*-*-*-*-*-*-*-*-*-*-*-

# Set by User:
V: "False" [Verbose output (short form of "VerbosityLevel" below - overrides the latter one)]
H: "False" [Print method-specific help message]
NTrees: "50" [Number of trees in the forest]
BoostType: "AdaBoost" [Boosting type for the trees in the forest]
AdaBoostBeta: "5.000000e-01" [Parameter for AdaBoost algorithm]
SeparationType: "giniindex" [Separation criterion for node splitting]
nEventsMin: "200" [Minimum number of events required in a leaf node (default: Classification: max(40, N_train/(Nvar^2)/10), Regression: 10)]
nCuts: "20" [Number of steps during node cut optimisation]
PruneMethod: "nopruning" [Method used for pruning (removal) of statistically insignificant branches]
MaxDepth: "3" [Max depth of the decision tree allowed]
# Default:
VerbosityLevel: "Default" [Verbosity level]
VarTransform: "None" [List of variable transformations performed before training, e.g., "D_Background,P_Signal,G,N_AllClasses" for: "Decorrelation, PCA-transformation, Gaussianisation, Normalisation, each for the given class of events ('AllClasses' denotes all events of all classes, if no class indication is given, 'All' is assumed)"]
CreateMVAPdfs: "False" [Create PDFs for classifier outputs (signal and background)]
IgnoreNegWeightsInTraining: "False" [Events with negative weights are ignored in the training (but are included for testing and performance evaluation)]
RenormByClass: "False" [Individually re-normalize each event class to the original size after boosting]
AdaBoostR2Loss: "quadratic" [Type of Loss function in AdaBoostR2t (Linear,Quadratic or Exponential)]
UseBaggedGrad: "False" [Use only a random subsample of all events for growing the trees in each iteration. (Only valid for GradBoost)]
GradBaggingFraction: "6.000000e-01" [Defines the fraction of events to be used in each iteration when UseBaggedGrad=kTRUE. (Only valid for GradBoost)]
Shrinkage: "1.000000e+00" [Learning rate for GradBoost algorithm]
UseRandomisedTrees: "False" [Choose at each node splitting a random set of variables]
UseNvars: "3" [Number of variables used if randomised tree option is chosen]
UsePoissonNvars: "True" [Interpret "UseNvars" not as fixed number but as mean of a Possion distribution in each split]
UseNTrainEvents: "101865" [Number of randomly picked training events used in randomised (and bagged) trees]
UseWeightedTrees: "True" [Use weighted trees or simple average in classification from the forest]
UseYesNoLeaf: "True" [Use Sig or Bkg categories, or the purity=S/(S+B) as classification of the leaf node]
NodePurityLimit: "5.000000e-01" [In boosting/pruning, nodes with purity > NodePurityLimit are signal; background otherwise.]
UseFisherCuts: "False" [Use multivariate splits using the Fisher criterion]
MinLinCorrForFisher: "8.000000e-01" [The minimum linear correlation between two variables demanded for use in Fisher criterion in node splitting]
UseExclusiveVars: "False" [Variables already used in fisher criterion are not anymore analysed individually for node splitting]
PruneStrength: "0.000000e+00" [Pruning strength]
PruneBeforeBoost: "False" [Flag to prune the tree before applying boosting algorithm]
PruningValFraction: "5.000000e-01" [Fraction of events to use for optimizing automatic pruning.]
NNodesMax: "100000" [Max number of nodes in tree]
DoBoostMonitor: "False" [Create control plot with ROC integral vs tree number]
NegWeightTreatment: "inverseboostnegweights" [How to treat events with negative weights in the BDT training (particular the boosting) : Ignore;  Boost With inverse boostweight; Pair events with negative and positive weights in traning sample and *annihilate* them (experimental!); Randomly pair events with negative and positive weights in leaf node and do not boost them (experimental!) ]
##


#VAR -*-*-*-*-*-*-*-*-*-*-*-* variables *-*-*-*-*-*-*-*-*-*-*-*-

NVar 6
track_minIPchi2          track_minIPchi2          track_minIPchi2          track_minIPchi2                                                 'F'    [8.7323722255e-05,499.771636963]
track_pvdis_mu                    track_pvdis_mu                    track_pvdis_mu                    PVdis                         mm                                'F'    [-192.893432617,199.680450439]
tracksvdis_mu                    tracksvdis_mu                    tracksvdis_mu                    SVdis                         mm                                'F'    [-199.778869629,192.869934082]
track_doca_mu                     track_doca_mu                     track_doca_mu                     doca                          mm                                'F'    [2.37707041606e-07,18.9456481934]
track_angle_mu                    track_angle_mu                    track_angle_mu                    angle                         rad                               'F'    [0.000116512797831,0.650962591171]
track_fc_mu                       track_fc_mu                       track_fc_mu                       fc                                                              'F'    [0.00243311771192,0.995779037476]
NSpec 0


============================================================================ */

#include <vector>
#include <cmath>
#include <string>
#include <iostream>

#define NN_bS new BDTNode_Bs
   
#ifndef BDTNode_Bs__def
#define BDTNode_Bs__def
   
class BDTNode_Bs {
   
public:
   
   // constructor of an essentially "empty" node floating in space
   BDTNode_Bs ( BDTNode_Bs* left,BDTNode_Bs* right,
                          int selector, double cutValue, bool cutType, 
                          int nodeType, double purity, double response ) :
   fLeft         ( left         ),
   fRight        ( right        ),
   fSelector     ( selector     ),
   fCutValue     ( cutValue     ),
   fCutType      ( cutType      ),
   fNodeType     ( nodeType     ),
   fPurity       ( purity       ),
   fResponse     ( response     ){
   }

   virtual ~BDTNode_Bs();

   // test event if it decends the tree at this node to the right
   virtual bool GoesRight( const std::vector<double>& inputValues ) const;
   BDTNode_Bs* GetRight( void )  {return fRight; };

   // test event if it decends the tree at this node to the left 
   virtual bool GoesLeft ( const std::vector<double>& inputValues ) const;
   BDTNode_Bs* GetLeft( void ) { return fLeft; };   

   // return  S/(S+B) (purity) at this node (from  training)

   double GetPurity( void ) const { return fPurity; } 
   // return the node type
   int    GetNodeType( void ) const { return fNodeType; }
   double GetResponse(void) const {return fResponse;}

private:

   BDTNode_Bs*   fLeft;     // pointer to the left daughter node
   BDTNode_Bs*   fRight;    // pointer to the right daughter node
   int                     fSelector; // index of variable used in node selection (decision tree)   
   double                  fCutValue; // cut value appplied on this node to discriminate bkg against sig
   bool                    fCutType;  // true: if event variable > cutValue ==> signal , false otherwise
   int                     fNodeType; // Type of node: -1 == Bkg-leaf, 1 == Signal-leaf, 0 = internal 
   double                  fPurity;   // Purity of node from training
   double                  fResponse; // Regression response value of node
}; 

#endif
   
#ifndef IClassifierReader__def
#define IClassifierReader__def

class IClassifierReader {

 public:

   // constructor
   IClassifierReader() : fStatusIsClean( true ) {}
   virtual ~IClassifierReader() {}

   // return classifier response
   virtual double GetMvaValue( const std::vector<double>& inputValues ) const = 0;

   // returns classifier status
   bool IsStatusClean() const { return fStatusIsClean; }

 protected:

   bool fStatusIsClean;
};

#endif

class ReadBDT_bS : public IClassifierReader {

 public:

   // constructor
   ReadBDT_bS( std::vector<std::string>& theInputVars ) 
      : IClassifierReader(),
        fClassName( "ReadBDT_bS" ),
        fNvars( 6 ),
        fIsNormalised( false )
   {      
      // the training input variables
      const char* inputVars[] = { "track_minIPchi2", "track_pvdis_mu", "tracksvdis_mu", "track_doca_mu", "track_angle_mu", "track_fc_mu" };

      // sanity checks
      if (theInputVars.size() <= 0) {
         std::cout << "Problem in class \"" << fClassName << "\": empty input vector" << std::endl;
         fStatusIsClean = false;
      }

      if (theInputVars.size() != fNvars) {
         std::cout << "Problem in class \"" << fClassName << "\": mismatch in number of input values: "
                   << theInputVars.size() << " != " << fNvars << std::endl;
         fStatusIsClean = false;
      }

      // validate input variables
      for (size_t ivar = 0; ivar < theInputVars.size(); ivar++) {
         if (theInputVars[ivar] != inputVars[ivar]) {
            std::cout << "Problem in class \"" << fClassName << "\": mismatch in input variable names" << std::endl
                      << " for variable [" << ivar << "]: " << theInputVars[ivar].c_str() << " != " << inputVars[ivar] << std::endl;
            fStatusIsClean = false;
         }
      }

      // initialize min and max vectors (for normalisation)
      fVmin[0] = 8.73237222549506e-05;
      fVmax[0] = 499.771636962891;
      fVmin[1] = -192.893432617188;
      fVmax[1] = 199.680450439453;
      fVmin[2] = -199.778869628906;
      fVmax[2] = 192.869934082031;
      fVmin[3] = 2.37707041605972e-07;
      fVmax[3] = 18.9456481933594;
      fVmin[4] = 0.000116512797831092;
      fVmax[4] = 0.650962591171265;
      fVmin[5] = 0.00243311771191657;
      fVmax[5] = 0.995779037475586;

      // initialize input variable types
      fType[0] = 'F';
      fType[1] = 'F';
      fType[2] = 'F';
      fType[3] = 'F';
      fType[4] = 'F';
      fType[5] = 'F';

      // initialize constants
      Initialize();

   }

   // destructor
   virtual ~ReadBDT_bS() {
      Clear(); // method-specific
   }

   // the classifier response
   // "inputValues" is a vector of input values in the same order as the 
   // variables given to the constructor
   double GetMvaValue( const std::vector<double>& inputValues ) const;

 private:

   // method-specific destructor
   void Clear();

   // common member variables
   const char* fClassName;

   const size_t fNvars;
   size_t GetNvar()           const { return fNvars; }
   char   GetType( int ivar ) const { return fType[ivar]; }

   // normalisation of input variables
   const bool fIsNormalised;
   bool IsNormalised() const { return fIsNormalised; }
   double fVmin[6];
   double fVmax[6];
   double NormVariable( double x, double xmin, double xmax ) const {
      // normalise to output range: [-1, 1]
      return 2*(x - xmin)/(xmax - xmin) - 1.0;
   }

   // type of input variable: 'F' or 'I'
   char   fType[6];

   // initialize internal variables
   void Initialize();
   double GetMvaValue__( const std::vector<double>& inputValues ) const;

   // private members (method specific)
   std::vector<BDTNode_Bs*> fForest;       // i.e. root nodes of decision trees
   std::vector<double>                fBoostWeights; // the weights applied in the individual boosts
};

