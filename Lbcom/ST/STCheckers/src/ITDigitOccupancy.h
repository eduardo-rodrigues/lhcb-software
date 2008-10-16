// $Id: ITDigitOccupancy.h,v 1.1 2008-10-16 13:12:03 mneedham Exp $

#ifndef ITDigitOccupancy_H 
#define ITDigitOccypancy_H !

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "ITOccupancy.h"
#include "ITOccupancy.icpp"
#include "Event/STDigit.h"


typedef ITOccupancy<LHCb::STDigit> ITDigitOccupancy;

template<>
inline const std::string& ITDigitOccupancy::dataLocation() const{
  return LHCb::STDigitLocation::ITDigits;
}

template <>
inline const std::string ITDigitOccupancy::histoDirName() const{
  return "ITDigitOccupancy";
}

template <>
inline double ITDigitOccupancy::defaultThreshold() const{
  return 0.0; // no threshold for clusters
}

//template class ITOccupancy<LHCb::STDigit>;

DECLARE_ALGORITHM_FACTORY( ITDigitOccupancy );

#endif
