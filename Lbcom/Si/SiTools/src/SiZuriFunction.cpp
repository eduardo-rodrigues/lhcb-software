// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "SiZuriFunction.h"


DECLARE_TOOL_FACTORY( SiZuriFunction )

SiZuriFunction::SiZuriFunction( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ):
  SiAmplifierResponseBase( type, name, parent )
{
  // constructor
  declareProperty("risetime", m_riseTime = 15.0*Gaudi::Units::ns);
  declareProperty("tRange", m_tRange = 100*Gaudi::Units::ns);
  declareProperty("m_couplingOffset", m_couplingOffset = 0.0* Gaudi::Units::ns);
}

StatusCode SiZuriFunction::initialize()
{
  StatusCode sc = SiAmplifierResponseBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // find the maximum....
  m_tMin = 0.0;
  m_tMax = m_tRange;
  auto max = findMax();

  // fill the arrays from the zuriFun...
  double t = 0.0;
  while(t < m_tMax) {
    m_times.push_back(t-max.first);
    m_values.push_back(zuriFun(t)/max.second);
    t += m_printDt;
  } // loop times

  // Store the first and last entry of the vector of times
  m_tMin = m_times.front();
  m_tMax = m_times.back();

  // Fit the spline to the data
  if (m_type == "signal") {
    m_responseSpline.reset( new GaudiMath::SimpleSpline( m_times, m_values,
                                                    typeFromString() ) );
  } else{
    // differentiate if cap coupling
    GaudiMath::SimpleSpline tempSpline{ m_times, m_values, typeFromString()};
    m_times.clear(); m_values.clear();
    for ( double t = m_tMin; t < m_tMax; t += m_printDt ) {
      m_times.push_back(t);
      m_values.push_back(tempSpline.deriv(t+m_couplingOffset));
    } // loop times and make derivative
    m_responseSpline.reset( new GaudiMath::SimpleSpline( m_times, m_values,
                                                    typeFromString() ) );
  }

  // dump to screen
  if (m_printToScreen) printToScreen();
  if (m_printForRoot) printForRoot();

  return sc;
}

double SiZuriFunction::zuriFun(const double t) const{

  double value = 0.0;

  if (t > m_tMin && t < m_tMax){
  value = (std::pow(t/m_riseTime,2)/2.0 -
          std::pow(t/m_riseTime,3)/6.0) *
          exp(-t/m_riseTime);
  }

  return value;
}

std::pair<double,double> SiZuriFunction::findMax() const{

  // it is this simple
  double tMax = (3. - sqrt(3.)) * m_riseTime;
  return {tMax, zuriFun(tMax)};
}
