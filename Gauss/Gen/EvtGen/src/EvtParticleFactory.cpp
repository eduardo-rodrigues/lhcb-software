//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtParticleFactory.cc
//
// Description: Class to describe all particles
//
// Modification history:
//
//    DJL December 27, 1999 Module created.
//
//------------------------------------------------------------------------
// 
#include <iostream>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtParticleNum.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDiracParticle.hh"
#include "EvtGen/EvtScalarParticle.hh"
#include "EvtGen/EvtVectorParticle.hh"
#include "EvtGen/EvtTensorParticle.hh"
#include "EvtGen/EvtPhotonParticle.hh"
#include "EvtGen/EvtNeutrinoParticle.hh"
#include "EvtGen/EvtStringParticle.hh"
#include "EvtGen/EvtRaritaSchwingerParticle.hh"
#include "EvtGen/EvtHighSpinParticle.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtParticleFactory.hh"

EvtParticle* EvtParticleFactory::particleFactory(EvtSpinType::spintype spinType){

  if ( spinType == EvtSpinType::SCALAR ) {
    return new EvtScalarParticle;
  }

  if ( spinType == EvtSpinType::VECTOR ) {
    return new EvtVectorParticle;
  }
  if ( spinType == EvtSpinType::DIRAC ) {
    return new EvtDiracParticle;
  }
  if ( spinType == EvtSpinType::NEUTRINO ) {
    return new EvtNeutrinoParticle;
  }
  if ( spinType == EvtSpinType::PHOTON ) {
    return new EvtPhotonParticle;
  }
  if ( spinType == EvtSpinType::TENSOR ) {
    return new EvtTensorParticle;
  }
  if ( spinType == EvtSpinType::STRING ) {
    return new EvtStringParticle;
  }
  if ( spinType == EvtSpinType::RARITASCHWINGER ) {
    return new EvtRaritaSchwingerParticle;
  }
  if ( spinType == EvtSpinType::SPIN5HALF ) {
    return new EvtHighSpinParticle;
  }
  if ( spinType == EvtSpinType::SPIN3 ) {
    return new EvtHighSpinParticle;
  }
  if ( spinType == EvtSpinType::SPIN7HALF ) {
    return new EvtHighSpinParticle;
  }
  if ( spinType == EvtSpinType::SPIN4 ) {
    return new EvtHighSpinParticle;
  }

  report(ERROR,"EvtGen")<<"Error in EvtParticleFactory::particleFactory"<<endl;
  report(ERROR,"EvtGen")<<"Tried to create non-existing particle"
			<<" with spin type:"<<spinType<<endl;
  report(ERROR,"EvtGen")<<"Will terminate execution"<<endl;


  ::abort();

  return 0;
  

}


EvtParticle* EvtParticleFactory::particleFactory(EvtId id, 
						 EvtVector4R p4,
						 EvtSpinDensity rho){

  EvtSpinType::spintype thisSpin=EvtPDL::getSpinType(id);

  if ( thisSpin == EvtSpinType::SCALAR ) {
    EvtScalarParticle *myPart;
    myPart=new EvtScalarParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }

  if ( thisSpin == EvtSpinType::VECTOR ) {
    EvtVectorParticle *myPart;
    myPart=new EvtVectorParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::DIRAC ) {
    EvtDiracParticle *myPart;
    myPart=new EvtDiracParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::NEUTRINO ) {
    EvtNeutrinoParticle *myPart;
    myPart=new EvtNeutrinoParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::PHOTON ) {
    EvtPhotonParticle *myPart;
    myPart=new EvtPhotonParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::TENSOR ) {
    EvtTensorParticle *myPart;
    myPart=new EvtTensorParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::STRING ) {
    EvtStringParticle *myPart;
    myPart=new EvtStringParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::SPIN3 ) {
    EvtHighSpinParticle *myPart;
    myPart=new EvtHighSpinParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::SPIN5HALF ) {
    EvtHighSpinParticle *myPart;
    myPart=new EvtHighSpinParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::SPIN7HALF ) {
    EvtHighSpinParticle *myPart;
    myPart=new EvtHighSpinParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::RARITASCHWINGER ) {
    EvtRaritaSchwingerParticle *myPart;
    myPart=new EvtRaritaSchwingerParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }
  if ( thisSpin == EvtSpinType::SPIN4 ) {
    EvtHighSpinParticle *myPart;
    myPart=new EvtHighSpinParticle;
    myPart->init(id, p4);
    myPart->setSpinDensityForward(rho);
    return myPart;
  }

  report(ERROR,"EvtGen")<<"Error in EvtParticleFactory::particleFactory"<<endl;
  report(ERROR,"EvtGen")<<"Tried to create non-existing particle"
			<<" with spin type:"<<thisSpin
			<<"  and name:"<<EvtPDL::name(id)<<endl;
  report(ERROR,"EvtGen")<<"Will terminate execution"<<endl;



  ::abort();

  return 0;

}


EvtParticle* EvtParticleFactory::particleFactory(EvtId id, 
						 EvtVector4R p4){

  EvtSpinDensity rho;
  rho.SetDiag(EvtSpinType::getSpinStates(EvtPDL::getSpinType(id)));

  return particleFactory(id,p4,rho);

}





