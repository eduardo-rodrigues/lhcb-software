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
// Module: EvtCPUtil.cc
//
// Description: Utilities needed for generation of CP violating
//              decays.
//
// Modification history:
//
//    RYD     March 24, 1998         Module created
//
//------------------------------------------------------------------------
// 
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtScalarParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtCPUtil.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include <assert.h>


//added two functions for finding the fraction of B0 tags for decays into 
//both CP eigenstates and non-CP eigenstates -- NK, Jan. 27th, 1998

void EvtCPUtil::fractB0CP(EvtComplex Af, EvtComplex Abarf, 
                          double /*deltam*/, double beta, double &fract) {
//this function returns the number of B0 tags for decays into CP-eigenstates
//(the "probB0" in the new EvtOtherB)

  //double gamma_B = EvtPDL::getWidth(B0);   
  //double xd = deltam/gamma_B;
  //double xd = 0.65;
  double ratio = 1/(1 + 0.65*0.65);
  
  EvtComplex rf, rbarf;

  rf = EvtComplex(cos(2.0*beta),sin(2.0*beta))*Abarf/Af;
  rbarf = EvtComplex(1.0)/rf;

  double A2 = real(Af)*real(Af) + imag(Af)*imag(Af);
  double Abar2 = real(Abarf)*real(Abarf) + imag(Abarf)*imag(Abarf);
  
  double rf2 = real(rf)*real(rf) + imag(rf)*imag(rf);    
  double rbarf2 = real(rbarf)*real(rbarf) + imag(rbarf)*imag(rbarf);    

  fract = (Abar2*(1+ rbarf2 + (1 - rbarf2)*ratio))/(Abar2*(1+ rbarf2 + (1 - rbarf2)*ratio) + A2*(1+ rf2 + (1 - rf2)*ratio));  
  return; 

}
void EvtCPUtil::fractB0nonCP(EvtComplex Af, EvtComplex Abarf, 
			     EvtComplex Afbar, EvtComplex Abarfbar, 
			     double deltam, double beta, 
			     int flip, double &fract) {

//this function returns the number of B0 tags for decays into non-CP eigenstates
//(the "probB0" in the new EvtOtherB)
//this needs more thought... 

  //double gamma_B = EvtPDL::getWidth(B0);
  //report(INFO,"EvtGen") << "gamma " << gamma_B<< std::endl;
  //double xd = deltam/gamma_B;

  //why is the width of B0 0 in PDL??

  double xd = 0.65;
  double gamma_B = deltam/xd;
  double IAf, IAfbar, IAbarf, IAbarfbar;
  EvtComplex rf, rfbar, rbarf, rbarfbar;
  double rf2, rfbar2, rbarf2, rbarfbar2;
  double Af2, Afbar2, Abarf2, Abarfbar2;

  rf = EvtComplex(cos(2.0*beta),sin(2.0*beta))*Abarf/Af;
  rfbar = EvtComplex(cos(2.0*beta),sin(2.0*beta))*Abarfbar/Afbar; 
  rbarf = EvtComplex(cos(-2.0*beta),sin(-2.0*beta))*Af/Abarf;
  rbarfbar = EvtComplex(cos(-2.0*beta),sin(-2.0*beta))*Afbar/Abarfbar;
  
  
  rf2 = real(rf)*real(rf) + imag(rf)*imag(rf);
  rfbar2 = real(rfbar)*real(rfbar) + imag(rfbar)*imag(rfbar);
  rbarf2 = real(rbarf)*real(rbarf) + imag(rbarf)*imag(rbarf);
  rbarfbar2 = real(rbarfbar)*real(rbarfbar) + imag(rbarfbar)*imag(rbarfbar);

  Af2 = real(Af)*real(Af) + imag(Af)*imag(Af);
  Afbar2 = real(Afbar)*real(Afbar) + imag(Afbar)*imag(Afbar); 
  Abarf2 = real(Abarf)*real(Abarf) + imag(Abarf)*imag(Abarf);
  Abarfbar2 = real(Abarfbar)*real(Abarfbar) + imag(Abarfbar)*imag(Abarfbar);


//
//IAf = integral(gamma(B0->f)), etc.
//

  IAf = (Af2/(2*gamma_B))*(1+rf2+(1-rf2)/(1+xd*xd));
  IAfbar = (Afbar2/(2*gamma_B))*(1+rfbar2+(1-rfbar2)/(1+xd*xd));
  IAbarf = (Abarf2/(2*gamma_B))*(1+rbarf2+(1-rbarf2)/(1+xd*xd));
  IAbarfbar = (Abarfbar2/(2*gamma_B))*(1+rbarfbar2+(1-rbarfbar2)/(1+xd*xd));
  
//flip specifies the relative fraction of fbar events
 
  fract = IAbarf/(IAbarf+IAf) + flip*IAbarfbar/(IAfbar+IAbarfbar);


  return;  
} 

void EvtCPUtil::OtherB( EvtParticle *p,double &t, EvtId &otherb, double probB0){

  //Can not call this recursively!!!
  static int entryCount=0;
  entryCount++;

  //added by Lange Jan4,2000
  static EvtId B0B=EvtPDL::getId("anti-B0");
  static EvtId B0=EvtPDL::getId("B0");

  int isB0=EvtRandom::Flat(0.0,1.0)<probB0;
  
  int idaug;
  
  p->setLifetime();
  
  // now get the time between the decay of this B and the other B!
  
  EvtParticle *parent=p->getParent();
  
  EvtParticle *other;
  
  // PR   if (parent==0) {

    //report(ERROR,"EvtGen") << 
    //  "Warning CP violation with B having no parent!"<<std::endl;
    p->setLifetime();
    t=p->getLifetime();
    if (p->getId()==B0) otherb=B0B;
    if (p->getId()==B0B) otherb=B0;
    entryCount--;
    return;
    // PR   }
    // PR  else{
    // PR   if (parent->getDaug(0)!=p){
    // PR     other=parent->getDaug(0);
    // PR     idaug=0;
    // PR   }
    // PR   else{
    // PR     other=parent->getDaug(1);
    // PR     idaug=1;
    // PR   }
    // PR }
  
  if (parent != 0 ) {

    //if (entryCount>1){
    //  report(INFO,"EvtGen") << "Double CP decay:"<<entryCount<<std::endl;
    //}

    //kludge!! Lange Mar21, 2003 	 
    // if the other B is an alias... don't change the flavor.. 	 
    if ( other->getId().isAlias() ) { 	 
      OtherB(p,t,otherb); 	 
      return; 	 
      
    }
    
    if (entryCount==1){
    
      EvtVector4R p_init=other->getP4();
      //int decayed=other->getNDaug()>0;
      bool decayed = other->isDecayed();

      other->deleteTree();
    
      EvtScalarParticle* scalar_part;
      
      scalar_part=new EvtScalarParticle;
      if (isB0) {
	scalar_part->init(B0,p_init);
      }
      else{
	scalar_part->init(B0B,p_init);
      }
      other=(EvtParticle *)scalar_part;
      //    other->set_type(EvtSpinType::SCALAR);
      other->setDiagonalSpinDensity();      
    
      parent->insertDaugPtr(idaug,other);
    
      if (decayed){
	//report(INFO,"EvtGen") << "In CP Util calling decay \n";
	other->decay();
      }

    }

    otherb=other->getId();

    other->setLifetime();
    t=p->getLifetime()-other->getLifetime();
    
    otherb = other->getId();
    
  }
  else {
    report(INFO,"EvtGen") << "We have an error here!!!!"<<std::endl;
    otherb = EvtId(-1,-1); 
  }
  
  entryCount--;
  return ;
}



void EvtCPUtil::OtherB( EvtParticle *p,double &t, EvtId &otherb){


  static EvtId BSB=EvtPDL::getId("anti-B_s0");
  static EvtId BS0=EvtPDL::getId("B_s0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId UPS4=EvtPDL::getId("Upsilon(4S)");

  if (p->getId()==BS0||p->getId()==BSB){
    static double ctauL=EvtPDL::getctau(EvtPDL::getId("B_s0L"));
    static double ctauH=EvtPDL::getctau(EvtPDL::getId("B_s0H"));
    static double ctau=ctauL<ctauH?ctauH:ctauL;
    t=-log(EvtRandom::Flat())*ctau;
    EvtParticle* parent=p->getParent();
    if (parent!=0&&(parent->getId()==BS0||parent->getId()==BSB)){
      if (parent->getId()==BS0) otherb=BSB;
      if (parent->getId()==BSB) otherb=BS0;
      parent->setLifetime(t);
      return;
    }
    if (p->getId()==BS0) otherb=BSB;
    if (p->getId()==BSB) otherb=BS0;
    p->setLifetime(t);
    return;
  }



  
   p->setLifetime();


// now get the time between the decay of this B and the other B!
  
  EvtParticle *parent=p->getParent();

  if (parent==0||parent->getId()!=UPS4) {
    //report(ERROR,"EvtGen") << 
    //  "Warning CP violation with B having no parent!"<<std::endl;
    t=p->getLifetime();
    if (p->getId()==B0) otherb=B0B;
    if (p->getId()==B0B) otherb=B0;
    if (p->getId()==BS0) otherb=BSB;
    if (p->getId()==BSB) otherb=BS0;
    return;
  }
  else{
    if (parent->getDaug(0)!=p){
      otherb=parent->getDaug(0)->getId();
      parent->getDaug(0)->setLifetime();
      t=p->getLifetime()-parent->getDaug(0)->getLifetime();
    }
    else{
     otherb=parent->getDaug(1)->getId();
      parent->getDaug(1)->setLifetime();
      t=p->getLifetime()-parent->getDaug(1)->getLifetime();
   }
  }


  return ;
}


void EvtCPUtil::incoherentMix(const EvtId id, double &t, int &mix){

  //added by Lange Jan4,2000
  static EvtId BS0=EvtPDL::getId("B_s0");

  static EvtId BSL=EvtPDL::getId("B_s0L");
  static EvtId BSH=EvtPDL::getId("B_s0H");

  double ctauL=EvtPDL::getctau(BSL);
  double ctauH=EvtPDL::getctau(BSH);
  double ctau=0.5*(ctauL+ctauH);
  double y=(ctauH-ctauL)/ctau;
  

  //need to figure out how to get these parameters into the code...
  double qoverp=1.0;
  double x=25.0;


  double fac;

  if(id==BS0){
    fac=1.0/(qoverp*qoverp);
  }
  else{
    fac=qoverp*qoverp;
  }

  double mixprob=(x*x+y*y)/(x*x+y*y+fac*(2+x*x-y*y));

  int mixsign;

  mixsign=(mixprob>EvtRandom::Flat(0.0,1.0))?-1:1;

  double prob;

  do{
    t=-log(EvtRandom::Flat())*ctauL;
    prob=1+exp(y*t/ctau)+mixsign*2*exp(0.5*y*t/ctau)*cos(x*t);
  }while(prob<4*EvtRandom::Flat());

  mix=0;

  if (mixsign==-1) mix=1;
  
  return;


}










