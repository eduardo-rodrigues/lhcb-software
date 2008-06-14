
#include "RichDataModel.h"

#include "Rich/EventDescription.h"
#include "Rich/Hit.h"
#include "Rich/Data.h"
#include "Utils/LogarithmicTools.h"
#include "Utils/MathsConstants.h"
#include "Utils/CheckForNan.h"
#include "Utils/MessageHandler.h"
#include "Rich/Constants.h"
#include "Rich/Small2Vector.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

namespace Lester
{

  // Add/Subtract 5 to allow for tolerance in calculations
  const double RichDataModel::s_max_exponent =
    std::log(boost::numeric::bounds<double>::highest())  - 5.0;
  const double RichDataModel::s_min_exponent =
    std::log(boost::numeric::bounds<double>::smallest()) + 5.0;

  std::string RichDataModel::dataFilesDir()
  {
    const char * env = getenv("RICHMFINDERDATALOCATION");
    return ( env ? std::string(env) : "DATA" );
  }

  void RichDataModel::initialise()
  {
    // read cache
    readCacheFromFile();
    // Create Samplers etc.
    m_richThetaSampler
      = new RichThetaSampler          ( dataFilesDir()+"/"+m_thetaDataFile );
    m_richThetaDistribution
      = new RichThetaDistribution     ( dataFilesDir()+"/"+m_thetaDataFile );
    m_richProbabilityThetaAbove
      = new RichProbabilityThetaAbove ( dataFilesDir()+"/"+m_thetaDataFile );
  }

  void RichDataModel::cleanUp()
  {
    delete m_richThetaSampler;
    m_richThetaSampler = NULL;
    delete m_richThetaDistribution;
    m_richThetaDistribution = NULL;
    delete m_richProbabilityThetaAbove;
    m_richProbabilityThetaAbove = NULL;
  }

  // Guarantees to either return a "reasonable" logProb or else throw Lester::LogarithmicTools::LogOfZero()
  double
  RichDataModel::totalLogProbOfEventDescriptionGivenData( const EventDescription & rp,
                                                          const Data & d ) const
  {

    /* The general idea is to calculate the result by Bayes' Theorem:
     *
     *   totalLogProbOfEventDescriptionGivenData \propto
     *             totalLogProbOfDataGivenEventDescription + totalLogProbOfEventDescription
     *
     */

    // FIRST PART IS likelihood:
    // ---------- totalLogProbOfDataGivenEventDescription ----------------------------------------
    double logAns = totalLogProbOfDataGivenEventDescription(d,rp);

    // SECOND PART IS prior:
    // ---------- totalLogProbOfEventDescription ----------------------------------------
    {
      const double pep  = priorProbabilityOf(rp);
      if ( !(0<pep) )            throw Lester::LogarithmicTools::LogOfZero();
      const double lpep = std::log(pep);
      if ( !Lester::lfin(lpep) ) throw Lester::LogarithmicTools::LogOfZero();
      logAns += lpep;
      //assert(Lester::lfin(logAns) && "step3");
    }

    return logAns;
  }


  double
  RichDataModel::totalLogProbOfDataGivenEventDescription(const Data & d,
                                                         const EventDescription & rp) const
  {
    double logAns=0;

    // expected number of total hits:
    const double mu = meanNumberOfHitsMuExpectedGiven(rp);

    // actual number of hits:
    const int n = d.hits.size();

    // should actually have
    //      ans *= Lester::poissonProb(n,mu);
    // followed by the
    //      ans *= factorial(n)
    // which occurs in the next block to account for a combinatorial factor for the hit probabilities that are about to arrive, so to save time I combine the two factors together into
    const bool muIsTooSmall=!(Lester::lfin(log(mu)));
    if ( mu==0 && n>0 )
    {
      throw Lester::LogarithmicTools::LogOfZero();// infintely unlikely!
    }
    else if (mu==0 && n==0)
    {
      logAns+=0; //ans *= 1;
    }
    else if (muIsTooSmall)
    {
      // what about numerical issues related to taking log of mu when mu is small.
      if ( n==0 )
      {
        logAns -= mu; //ans *= exp(-mu);
      }
      else
      {
        throw Lester::LogarithmicTools::LogOfZero();// infintely unlikely!
      }
    }
    else
    {
      logAns+= (n*std::log(mu)-mu); //ans *= exp(n*log(mu)-mu);
    }

    // check how we are doing!
    assert( Lester::lfin(logAns) && "step1" );

    //      ans *= factorial(n) // not needed as incorporated above
    for ( Data::Hits::const_iterator hit = d.hits.begin();
          hit!=d.hits.end();
          ++hit )
    {
      const double p  = singleHitProbabilityDistributionGivenEventDescription(*hit, rp);
      if ( !(p>0) )            throw Lester::LogarithmicTools::LogOfZero();
      const double lp = std::log(p);
      if ( !Lester::lfin(lp) ) throw Lester::LogarithmicTools::LogOfZero();
      logAns+=lp;
    }

    // check how we are doing!
    assert(Lester::lfin(logAns) && "step2");
    return logAns;
  }


  // were in RichPriors.h: ------------------------------------------------------------

  Small2Vector RichDataModel::sampleFromCircleCentreDistribution() const
  {
    return Small2Vector(RandGauss::shoot(circleCenXMean,circleCenXSig),
                        RandGauss::shoot(circleCenYMean,circleCenYSig));
  }

  int RichDataModel::sampleFromNumberOfCirclesDistribution() const
  {
    return RandPoisson::shoot(meanNumberOfRings);
  }

  int RichDataModel::sampleFromNumberOfHitsDueToCircle(const CircleParams & cp) const
  {
    return RandPoisson::shoot(meanNumberOfHitsMuExpectedOn(cp));
  }
  // probability counterpart not yet implemented

  int RichDataModel::sampleFromNumberOfHitsDueToBackground() const
  {
    return RandPoisson::shoot(meanNumberOfHitsMuExpectedFromBg());
  }
  // probability counterpart not yet implemented

  Hit RichDataModel::sampleHitDueToBackground() const
  {
    return Hit ( RandGauss::shoot(0,backgroundRadius),
                 RandGauss::shoot(0,backgroundRadius) );
  }

  Hit RichDataModel::sampleHitDueToCircle(const CircleParams & c) const
  {
    const double theta    = RandFlat::shoot(0.,MathsConstants::twoPi);
    const double r0       = c.radius();
    const double epsilon  = circleProbabilityDistributionEpsilonParameter;
    // the maths of the next bit depends on alpha being 2 (seelab book 5) so:
    assert (circleProbabilityDistributionAlphaParameter==2);
    const double t        = RandGauss::shoot(log(r0),epsilon);
    const double rWobbled = std::exp(t);
    const double x        = rWobbled*std::cos(theta)+c.centre().x();
    const double y        = rWobbled*std::sin(theta)+c.centre().y();
    return Hit(x,y);
  }

  double RichDataModel::priorProbabilityOfHitDueToCircle(const Small2Vector & p,
                                                         const CircleParams & cp) const
  {
    const double &alpha   = circleProbabilityDistributionAlphaParameter;
    const double alphaSq  = alpha*alpha;
    const double &epsilon = circleProbabilityDistributionEpsilonParameter;
    const double epsSq    = epsilon*epsilon;
    const double r        = (p-cp.centre()).mag();
    const double r0       = cp.radius();
    const double logg     = std::log(r/r0);
    const double logsq    = logg*logg;
    const double two      = 2;
    const double myPow    = ( alpha==2 ? 1 : std::pow(r/r0,alpha-two) );
    const double moo      = -0.5*(logsq/epsSq+alphaSq*epsSq);
    const double expon    = ( moo < s_min_exponent ? std::exp(s_min_exponent) :
                              moo > s_max_exponent ? std::exp(s_max_exponent) :
                              std::exp(moo) );
    const double ans      = 1.0/(MathsConstants::twoPi)/std::sqrt(MathsConstants::twoPi*epsSq)*myPow*expon / (r*r);
    return ans;
}

  double
  RichDataModel::singleHitProbabilityDistributionGivenEventDescription(const Hit & hit,
                                                                       const EventDescription & ed) const
{
  double ans=0;
  double sigmaMu=0;
  for ( EventDescription::Circs::const_iterator it=ed.circs.begin();
        it!=ed.circs.end(); ++it )
  {
    const double mu = meanNumberOfHitsMuExpectedOn(*it);
    sigmaMu+= mu;
    ans += mu*priorProbabilityOfHitDueToCircle(hit,*it);
  }
  sigmaMu += meanNumberOfHitsMuExpectedFromBgGiven(ed);
  ans     += meanNumberOfHitsMuExpectedFromBgGiven(ed)*priorProbabilityOfHitDueToBackground(hit);
  return ( sigmaMu>0 ? ans/sigmaMu : 0 );
}

// --------------------------------------------------------------


double RichDataModel::meanNumberOfHitsMuExpectedGiven(const EventDescription & ed) const
{
  double ans=0;
  for ( EventDescription::Circs::const_iterator it=ed.circs.begin();
        it != ed.circs.end();
        ++it )
  {
    ans += meanNumberOfHitsMuExpectedOn(*it);
  }
  ans += meanNumberOfHitsMuExpectedFromBgGiven(ed);
  return ans;
}

// Were in CirclePriors ----------------------------

std::string RichDataModel::getCacheLocation() const
{
  const char * env = getenv("RICHMFINDERDATALOCATION");
  const std::string senv( env ? std::string(env)+"/" : "" );
  return ( senv + m_cacheDataFile );
}

double RichDataModel::sampleFromApproximateCoPointSeparationDistribution() const
{
  // CoPointSeparationDistribution is the distrib of
  // distances between two points on a random circle
  // Here we approximate it in the limit that the circles are whole and sharp
  const double r     = sampleFromCircleRadiusDistribution();
  const double theta = RandFlat::shoot(MathsConstants::pi);
  const double two   = 2;
  const double rho   = r*std::sqrt(two-two*cos(theta));
  return rho;
}

void RichDataModel::readCacheFromFile()
{
  m_cacheLocation = getCacheLocation();
  if ( m_enableFileCache && m_cache.empty() )
  {
    Lester::messHandle().info() << "Opening cache file '" << m_cacheLocation << "'"
                                << Lester::endmsg;
    std::ifstream f(m_cacheLocation.c_str());
    if ( f.is_open() )
    {
      double key(0),ans(0);
      while ( f>>key )
      {
        f >> ans;
        m_cache[key] = ans;
        Lester::messHandle().debug() << " -> Read approxCoPointSep[ " << key << " ] = "
                                     << ans << " from file" << Lester::endmsg;
      }
    }
    else
    {
      Lester::messHandle().error() << "Failed to open cache file '" << m_cacheLocation
                                   << "' -> Will re-calculate from scratch (SLOW)"
                                   << Lester::endmsg;
    }
  }
  else
  {
    Lester::messHandle().warning() << "File Cache is DISABLED" << Lester::endmsg;
  }
}

double RichDataModel::approxCoPointSepFunctionPart1(const double deltaOnTwo) const
{

  const double one = 1;
  const double tol = 0.02; // interpolate at 2% intervals
  const double deltaOnTwoMax = deltaOnTwo*(one+tol);
  const double deltaOnTwoMin = deltaOnTwo/(one+tol);

  typedef std::vector<CacheMap::const_iterator> Possibilities;
  Possibilities possibilities;

  CacheMap::const_iterator geRhoIt = m_cache.lower_bound(deltaOnTwo);
  if ( geRhoIt != m_cache.end() )
  {
    const double key=geRhoIt->first;
    if (key == deltaOnTwo)
    {
      // we found an exact match
      return geRhoIt->second;
    }
    if (geRhoIt->first <= deltaOnTwoMax )
    {
      possibilities.push_back(geRhoIt);
    }
  }
  if ( geRhoIt != m_cache.begin() )
  {
    --geRhoIt;
    if (geRhoIt->first>=deltaOnTwoMin)
    {
      possibilities.push_back(geRhoIt);
    }
  }

  int tindex=-1;
  if (possibilities.size()==1)
  {
    tindex=0;
  }
  else if (possibilities.size()==2)
  {
    if (fabs(possibilities[0]->first-deltaOnTwo)<fabs(possibilities[1]->first-deltaOnTwo))
    {
      tindex=0;
    }
    else
    {
      tindex=1;
    }
  }

  double ans(0);
  if (tindex==-1)
  {
    ans = fillCache(deltaOnTwo);
  }
  else
  {
    // serve from cache
    assert(tindex>=0 && tindex<static_cast<int>(possibilities.size()));
    ans = possibilities[tindex]->second;
  }

  return ans;
}

double RichDataModel::fillCache(const double deltaOnTwo) const
{
  //calculate and cache!
  const double convergenceTol=0.0000001; // evaluate to appro 1% acc
  int n=0;
  double sum=0;
  double avg=0;
  double lastAvg=0;
  const double rmin=deltaOnTwo;

  try
  {
    const int warmUpSteps=10; // must be at least 2
    assert (warmUpSteps>=2);
    for (; n<warmUpSteps || (n>=warmUpSteps && fabs((avg-lastAvg)/(avg+lastAvg))>convergenceTol  ); ++n)
    {
      const double r = sampleFromCircleRadiusDistributionAbove(rmin);
      const double rSq = r*r;
      const double contrib = approxCoPointSepFunctionPart2(deltaOnTwo, rSq);
      lastAvg=avg;
      sum+= contrib;
      avg = sum/static_cast<double>(n);
    }
    // the answer should now be in avg, all bar the accounting for the rmin parameter used above ...
    avg*=priorProbabilityOfRadiusAbove(rmin);
  }
  catch (RichDataModel::SampleIsImpossible&)
  {
    //Lester::messHandle().verbose() << " SampleIsImpossibleInCop at " <<deltaOnTwo<< Lester::endmsg;
    avg=0;
  }

  // Save this result to the cache map
  Lester::messHandle().debug() << "Computed approxCoPointSep[ " << deltaOnTwo <<" ] = "
                               << avg << Lester::endmsg;
  m_cache[deltaOnTwo] = avg;

  if ( m_enableFileCache )
  {
    try
    {
      // need to add read/write idea here
      std::ofstream cf(m_cacheLocation.c_str(),std::ios::app);
      if ( cf.is_open() )
      {
        if ( Lester::lfin(deltaOnTwo) && Lester::lfin(avg))
        {
          cf << std::setprecision(25) << deltaOnTwo << " " << avg << std::endl;
          Lester::messHandle().debug() << "  -> Wrote to cache file" << Lester::endmsg;
        }
      }
      else
      {
        Lester::messHandle().warning() << "Failed to open cache file '" << m_cacheLocation
                                       << "' for writting -> New cache values not saved." 
                                       << Lester::endmsg;
        // disable file caching from now on
        m_enableFileCache = false;
      }
    }
    catch ( const std::exception & expt )
    {
      Lester::messHandle().warning() << "Exception '" << expt.what()
                                     << "' caught writing to cache -> results not saved" 
                                     << Lester::endmsg;
      // disable file caching from now on
      m_enableFileCache = false;
    }
  }

  // return the new result
  return avg;

}

double
RichDataModel::priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius( const Small2Vector & a,
                                                                                 const Small2Vector & b,
                                                                                 const Small2Vector & c,
                                                                                 const double r ) const
{
  const double rProb = priorProbabilityOfRadius(r);
  if ( !(rProb>0) ) return 0;

  const double averageHitsFromACircle =
    ( MathsConstants::pi * circleMeanRadiusParameter * circleHitsPerUnitLengthParameter );
  const double averageHitsFromCircles = averageHitsFromACircle * meanNumberOfRings;
  const double averageHitsFromBg      = backgroundMeanParameter;
  const double averageHitsTotal       = averageHitsFromCircles+averageHitsFromBg;
  const double signalPurity           = ( averageHitsTotal>0 ? averageHitsFromCircles/averageHitsTotal : 0 );
  const double signalPurityCubed      = signalPurity*signalPurity*signalPurity;
  const double nC                     = meanNumberOfRings;

  const double PS                     = ( nC>0 ? signalPurityCubed/(nC*nC) : 0 );
  // mean number of hits per unit length on circles
  const double mu                     = circleHitsPerUnitLengthParameter;

  const double aAll      = areaScaleForEverything;
  const double aSig      = areaScaleForSignal;
  const double aAllCubed = aAll*aAll*aAll;
  const double rSq       = r*r;

  const double modCross = fabs( CircleTheorems::twoVectorCross(a,b) +
                                CircleTheorems::twoVectorCross(b,c) +
                                CircleTheorems::twoVectorCross(c,a) );

  const double extraTerm = averageHitsTotal*(averageHitsTotal-1.0)*(averageHitsTotal-2.0);

  const double bot = 2.0*aAllCubed*mu*mu*mu*rProb*rSq*PS;

  const double ans = 1.0/(1.0+((extraTerm*aSig*modCross*(1.0-PS))/bot));
  return ans;
}

double
RichDataModel::PROPTO_priorProbabilityOfTwoPointsBeingOnACircle( const Small2Vector & a,
                                                                 const Small2Vector & b ) const
{
  // See sheets one two and three stapled into lab book 5 at 19/11/2003
  // Assumptions:
  // *   Assumes circles are reasonably narrow (well focused).
  // Good points:
  // *   Takes into account that the number of hits on circle is
  //     proportional to circumference
  // *   Takes prior distribution of circle radii into account.
  // Bad points:
  // *   assumes circles are distributed evenly across space, so makes no use
  //     of any knowledge that could be gleaned from the distribution of centres.
  const double delta      = std::sqrt( (a-b).mag2() );
  const double deltaOnTwo = delta / 2.0;

  const double sepFun     = approxCoPointSepFunctionPart1(deltaOnTwo);
  if ( !(sepFun>0) ) return 0;

  // mean number of hits per unit length on circles
  const double mu   = circleHitsPerUnitLengthParameter;
  const double muSq = mu*mu;

  const double averageAreaScale = areaScaleForEverything*areaScaleForEverything / areaScaleForSignal;

  // Estimation of PS could be improved by looking at the present number of hits rather
  // than using the average number, and by removing crude dependence on average radius etc.
  // This would be very important if the circle radius distribution became bimodal or hits
  // per unit arc length came to depend upon r." << Lester::endmsg;
  // for example, could base estimate on evidence of current fit!

  const double averageHitsFromACircle = MathsConstants::pi * circleMeanRadiusParameter * circleHitsPerUnitLengthParameter;
  const double averageHitsFromCircles = averageHitsFromACircle * meanNumberOfRings;
  const double averageHitsFromBg      = backgroundMeanParameter;
  const double averageHitsTotal       = averageHitsFromCircles+averageHitsFromBg;
  const double signalPurity           = ( averageHitsTotal>0 ? averageHitsFromCircles/averageHitsTotal : 0 );
  const double signalPuritySq         = signalPurity*signalPurity;
  const double PS                     = ( meanNumberOfRings>0 ? signalPuritySq/meanNumberOfRings : 0 );

  const double extraTerm = averageHitsTotal*(averageHitsTotal-1.0);
  const double fiddle    = PS * averageAreaScale * muSq * sepFun;
  const double ans       = 1.0/(1.0+(1.0-PS)*extraTerm/fiddle);

  return ans;
}

// were in EventDescription.h --------------------------------------------------------

EventDescription RichDataModel::sampleAnEventDescriptionFromPrior() const
{
  EventDescription ans;
  const int n=sampleFromNumberOfCirclesDistribution();
  ans.circs.reserve(n);
  for ( int i=0; i<n; ++i )
  {
    ans.circs.push_back(sampleCircle());
  }
  return ans;
}

double RichDataModel::priorProbabilityOf(const EventDescription & ed) const
{
  //#warning "Need to treat meanBackground correctly!"
  double ans = 1;

  // in principle I should have
  //        ans *= Lester::poissonProb(ed.circs.size(),meanNumberOfRings);
  // followed by
  //        ans *= factorial(ed.circs.size())
  // to accound for a combinatorial factor in what is about to follow.
  // however this involves a needless double (cancelling) evaluation of n!,
  // where n=ed.circs.size(), so instead I write the following faster combination
  // of the two:

  // actually I have just realised that contrary to the comments above, since in my case the order DOES count, as I don't always sort my circles into some order, I should NOT multiply by the factorial factor mentioned above, so I should just stick  to using the poisson factor.  As a result, I comment out the three lines below and replace them by the one four below!
  //const double n = static_cast<double>(ed.circs.size());
  //const double mu = meanNumberOfRings;
  //ans *= exp(n*log(mu)-mu);  /* times n! divided by n! */
  // SUPERSEDED BY NEXT LINE      ans *= poissonProb(ed.circs.size(),meanNumberOfRings);
  ans *= priorProbabilityOfNumberOfCircles(ed.circs.size());
  for ( EventDescription::Circs::const_iterator it = ed.circs.begin();
        it!=ed.circs.end(); ++it )
  {
    const CircleParams & c = *it;
    const double contribution = priorProbabilityOf(c);
    ans *= contribution;
  }

  // and now the part I left out for ages (by accident!) which ensures that we compare hypotheses with different numbers of circles on an equivalent footing.  This insertion is somewhat guessed, so would be a good idea to get a statistician to confirm what I am doing.

  // Basically, for every circle I multiply the prior by one over the expectation (under the circles prior) of the circles prior itself!

  const double strangeFactor = (2.0*MathsConstants::sqrtPi);

  const double oneOnCharacteristicAreaForCentresWandering = strangeFactor*strangeFactor*(circleCenXSig*circleCenYSig);
  const double oneOnCharacteristicLengthForRadiiWandering = strangeFactor*(circleRadiusSigmaAboutMean);
  const double characteristicThingForOneCircle
    = oneOnCharacteristicAreaForCentresWandering
    * oneOnCharacteristicLengthForRadiiWandering;

  const double characteristicThingForAllCircles
    = std::pow( characteristicThingForOneCircle, (int)ed.circs.size() );

  ans *= characteristicThingForAllCircles;

  return ans;
}

}
