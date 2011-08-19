// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#include "Mint/SpinFactors.h"
#include "Mint/DecayTree.h"
#include "Mint/Utils.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/ZTspin1.h"
#include "Mint/ZTspin2.h"
#include "Mint/LeviCivita.h"

// These come from:
// D. Coffman et al: "Resonant substructure in Kbar pi pi pi decays of D mesons"
// Physical Review D, Vol 45, Number 7, page 2196
// The spin factors are in Table II, page 2201.

DecayTree* SF_DtoPP0_PtoVP1_VtoP2P3::_exampleDecay=0;
DecayTree* SF_DtoAP0_AtoVP1_VtoP2P3::_exampleDecay=0; // see below for D-wave version of this
DecayTree* SF_DtoAP0_AtoSP1_StoP2P3::_exampleDecay=0;
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_S::_exampleDecayS=0;
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_P::_exampleDecayP=0;// ?
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_D::_exampleDecayD=0;
DecayTree* SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::_exampleDecay=0;
DecayTree* SF_DtoVS_VtoP0P1_StoP2P3::_exampleDecay=0;
DecayTree* SF_DtoVS_VtoP0P1_StoP2P3_nonResV::_exampleDecay=0;
DecayTree* SF_DtoPP1P2_PtoP3P4::_exampleDecay=0;
DecayTree* SF_DtoV1P0_V1toV2P1_V2toP2P3::_exampleDecay=0;

DecayTree* SF_DtoVT_VtoP0P1_TtoP2P3_P::_exampleDecay=0;
DecayTree* SF_DtoVT_VtoP0P1_TtoP2P3_D::_exampleDecayD=0;

DecayTree* SF_DtoTS_TtoP0P1_StoP2P3::_exampleDecay=0;

DecayTree* SF_DtoAP0_AtoVP1Dwave_VtoP2P3::_exampleDecayD=0;

using namespace std;
using namespace MINT;

const DecayTree& SF_DtoAP0_AtoVP1Dwave_VtoP2P3::getExampleDecay(){
  // D->a(1) pi, a(1)->rho pi, rho->pipi
  // with a(1) decaying in D-wave
  _exampleDecayD = new DecayTree(421);
  DecayTree a1(20213);
  a1.getVal().setL(2);
  a1.addDgtr(211, 113)->addDgtr(211, -211);
  _exampleDecayD->addDgtr(-211);
  _exampleDecayD->addDgtr(&a1);
  return *_exampleDecayD;
}
const DecayTree& SF_DtoAP0_AtoVP1Dwave_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoPP1P2_PtoP3P4::getExampleDecay(){//Lauren's wide KsSpin
  if(0 == _exampleDecay){
    _exampleDecay = new DecayTree(421);
    _exampleDecay->addDgtr(888310)->addDgtr(211, -211);
    _exampleDecay->addDgtr(321, -321);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoPP1P2_PtoP3P4::exampleDecay(){//Laurens KsSpin
  return getExampleDecay();
}

const DecayTree& SF_DtoPP0_PtoVP1_VtoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-211, 200321)->addDgtr(321, -10323)->addDgtr(-321,211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoPP0_PtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoPP0_PtoVP1_VtoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == 'P' && ! dgtr->isFinalState()) P = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P' &&   dgtr->isFinalState()) fsPS[0] = dgtr;
  }
  if(0==P || 0==fsPS[0]){
    cout << "ERROR in SF_DtoPP0_PtoVP1_VtoP2P3::parseTree"
	 << " Didn't find P or P1 " << P.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< P->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= P->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == 'V' && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P' &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_DtoPP0_PtoVP1_VtoP2P3::parseTree"
	 << " Didn't find V or P2 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  this->printYourself();

  return true;
}

double SF_DtoPP0_PtoVP1_VtoP2P3::getVal(){
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  TLorentzVector pV = p(2) + p(3);
  TLorentzVector qV = p(2) - p(3);

  double MassV = mRes(V);
  
  return (p(1).Dot(qV)
	  -    p(1).Dot(pV) * pV.Dot(qV) / (MassV*MassV))
    /(GeV*GeV)
    ;
}
void SF_DtoPP0_PtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoPP0_PtoVP1_VtoP2P3 "
     << "\n      (p(1).Dot(qV) -p(1).Dot(pV) * pV.Dot(qV) / (MassV*MassV))  /  (GeV*GeV)"
     << "\n      with pV = p(2) + p(3); qV = p(2) - p(3)"
     << "\n      parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//==================================================================

const DecayTree& SF_DtoAP0_AtoVP1_VtoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-321, 10323)->addDgtr(321, 113)->addDgtr(211,-211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoAP0_AtoVP1_VtoP2P3::exampleDecay(){
  return getExampleDecay();
}

//==================================================================
//==================================================================

bool SF_DtoAP0_AtoVP1_VtoP2P3::parseTree(){
  bool debugThis=false;

  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == 'A') A = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P') fsPS[0] = dgtr;
  }
  if(0==A || 0==fsPS[0]){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " Didn't find A or P1 " << A.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< A->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == 'V') V = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P') fsPS[1] = dgtr;
  }
  if(0==V || 0==fsPS[1]){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " Didn't find V or P2 " << V.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoVP1_VtoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V->getDgtrTreePtr(0);
  fsPS[3] = V->getDgtrTreePtr(1);

  normalOrder(fsPS[2], fsPS[3]);

  if(debugThis){
    cout << "parsed Tree: V:\n"
	 << *V
	 << "\n A:"
	 << *A 
	 << endl;
    for(int i=0; i<4; i++){
      cout << "fsPS[" << i << "]\n"
	   << *(fsPS[i])
	   << endl;
    }
  }
  printYourself();
  return true;
}

double SF_DtoAP0_AtoVP1_VtoP2P3::getVal(){
  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

  TLorentzVector pV = p(2) + p(3);

  if(debugThis){
    for(int i=0; i<4; i++){
      cout << " numbered by SF: p(" << i << ") " << p(i) << endl;
    }
    for(int i=0; i<4; i++){
      cout << " numbered 'normally' p(" << i << ") " 
	   << getEvent()->p(i) << endl;
    }
    cout << "got pV " << pV << endl;
  }
  TLorentzVector qV = p(2) - p(3);
  TLorentzVector pA = p(1) + p(2) + p(3);

  double MA    = mRes(A);
  double MassV = mRes(V);
  
  return (p(0).Dot(qV)
	  -    p(0).Dot(pA) * pA.Dot(qV) / (MA*MA)
	  -    p(0).Dot(pV) * pV.Dot(qV) / (MassV*MassV)
	  +    p(0).Dot(pA) * pA.Dot(pV) * pV.Dot(qV) / (MA*MA * MassV*MassV)
	  )
    /(GeV*GeV)
	  ;
}
void SF_DtoAP0_AtoVP1_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoAP0_AtoVP1_VtoP2P3"
     << "\n\t (p(0).Dot(qV)"
     << "\n\t   -    p(0).Dot(pA) * pA.Dot(qV) / (MA*MA)"
     << "\n\t   -    p(0).Dot(pV) * pV.Dot(qV) / (MassV*MassV)"
     << "\n\t   +    p(0).Dot(pA) * pA.Dot(pV) * pV.Dot(qV) / (MA*MA * MassV*MassV)"
     << "\n\t   )"
     << "\n\t /(GeV*GeV)"
     << "\n\t with qV = p(2) - p(3) and pA = p(1) + p(2) + p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoAP0_AtoSP1_StoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-321, 10323)->addDgtr(211, 10311)->addDgtr(321,-211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoAP0_AtoSP1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoAP0_AtoSP1_StoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == 'A' && ! dgtr->isFinalState()) A = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P' &&   dgtr->isFinalState()) fsPS[0] = dgtr;
  }
  if(0==A || 0==fsPS[0]){
    cout << "ERROR in SF_DtoAP0_AtoSP1_StoP2P3::parseTree"
	 << " Didn't find A or P1 " << A.get() << ", " << fsPS[0].get() << endl;
    return false;
  }

  for(int i=0; i< A->nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= A->getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == 'S' && ! dgtr->isFinalState()) S = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P' &&   dgtr->isFinalState()) fsPS[1] = dgtr;
  }
  if(0==S || 0==fsPS[1]){
    cout << "ERROR in SF_DtoAP0_AtoSP1_StoP2P3::parseTree"
	 << " Didn't find S or P2 " << S.get() << ", " << fsPS[1].get() << endl;
    return false;
  }

  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoAP0_AtoSP1_StoP2P3::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);

  normalOrder(fsPS[2], fsPS[3]);

  printYourself();
  return true;
}

double SF_DtoAP0_AtoSP1_StoP2P3::getVal(){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

  TLorentzVector pV = p(2) + p(3);

  TLorentzVector pA =  p(1) + p(2)  + p(3);
  TLorentzVector qA = (p(2) + p(3)) - p(1);

  double MA = mRes(A);
  
  return (p(0).Dot(qA)
	  -    p(0).Dot(pA) * pA.Dot(qA) / (MA*MA)
	  )    
    /(GeV*GeV)
    ;
}
void SF_DtoAP0_AtoSP1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor  SF_DtoAP0_AtoSP1_StoP2P3"
     << "\n\t ( p(0).Dot(qA) -  p(0).Dot(pA) * pA.Dot(qA) / (MA*MA) )  /  (GeV*GeV)"
     << "\n\t with pV = p(2) + p(3), pA =  p(1) + p(2)  + p(3),  qA = (p(2) + p(3)) - p(1)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S::getExampleDecay(){
  if(0==_exampleDecayS){
    _exampleDecayS = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayS->addDgtr( 313)->addDgtr( 321, -211);
    _exampleDecayS->addDgtr(-313)->addDgtr(-321,  211);
  }
  return *_exampleDecayS;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_P::getExampleDecay(){
  if(0==_exampleDecayP){
    _exampleDecayP = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayP->addDgtr( 313)->addDgtr( 321, -211);
    _exampleDecayP->addDgtr(-313)->addDgtr(-321,  211);

    _exampleDecayP->getVal().setL(1);
  }
  return *_exampleDecayP;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_D::getExampleDecay(){
  if(0==_exampleDecayD){
    _exampleDecayD = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayD->addDgtr( 313)->addDgtr( 321, -211);
    _exampleDecayD->addDgtr(-313)->addDgtr(-321,  211);

    _exampleDecayD->getVal().setL(2);
  }
  return *_exampleDecayD;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S::exampleDecay(){
  return getExampleDecay();
}
const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_P::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_D::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoV1V2_V1toP0P1_V1toP2P3_S::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }
  V1 = theDecay().getDgtrTreePtr(0);
  V2 = theDecay().getDgtrTreePtr(1);
  
  if(0==V1 || 0==V2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S::parseTree"
	 << " Didn't find V1 or V2 " << V1.get() << ", " << V2.get() << endl;
    return false;
  }
  if(V1->nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S::parseTree"
	 << " V1 should have 2 daughters, but it says it has "
	 << V1->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = V1->getDgtrTreePtr(0);
  fsPS[1] = V1->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(V2->nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S::parseTree"
	 << " V2 should have 2 daughters, but it says it has "
	 << V2->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V2->getDgtrTreePtr(0);
  fsPS[3] = V2->getDgtrTreePtr(1);

  normalOrder(fsPS[2], fsPS[3]);

  printYourself();

  return true;
}

double SF_DtoV1V2_V1toP0P1_V1toP2P3_S::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

    TLorentzVector pV1 = p(0) + p(1);
    TLorentzVector qV1 = p(0) - p(1);
    TLorentzVector pV2 = p(2) + p(3);
    TLorentzVector qV2 = p(2) - p(3);

    double MV1 = mRes(V1);
    double MV2 = mRes(V2);

    
    double returnVal= (qV1.Dot(qV2)
	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)
	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)
	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) 
	         / (MV1*MV1 * MV2*MV2)
	    )
      /(GeV*GeV)
      ;
    
    if(dbThis){
      ZTspin1 tV1(qV1, pV1, MV1);
      ZTspin1 tV2(qV2, pV2, MV2);

      double zResult = tV1.Contract(tV2);
      cout << "SF_DtoV1V2_V1toP0P1_V1toP2P3_S compare: " << zResult << " / "
	   << returnVal << " = " << zResult/returnVal << endl;
    }
    return returnVal;

}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_S::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_S"
     << "\n\t        (  qV1.Dot(qV2)"
     << "\n\t 	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)"
     << "\n\t 	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)"
     << "\n\t 	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) "
     << "\n\t 	         / (MV1*MV1 * MV2*MV2)"
     << "\n\t 	    )"
     << "\n\t      /(GeV*GeV)"
     << "\n\t with pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -------------------------

double SF_DtoV1V2_V1toP0P1_V1toP2P3_P::getVal(){
  //bool dbThis=true;
  // double check this!!
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV1 = p(0) + p(1);
  TLorentzVector qV1 = p(0) - p(1);
  TLorentzVector pV2 = p(2) + p(3);
  TLorentzVector qV2 = p(2) - p(3);
  
  TLorentzVector pD = pV1 + pV2;
  TLorentzVector qD = pV1 - pV2;
  
  /*
  double sum=0;
  for(int a=0; a<4; a++){
    for(int b=0; b<4; b++){
      if(a==b) continue;
      for(int c=0; c<4; c++){
	if(a==c || b==c) continue;
	for(int d=0; d<4; d++){
	  if(a==d || b==d || c==d) continue;
	  sum += LeviCita(a, b, c, d)
	    * pD[  (a + 3)%4 ] // coping with the stupid, stupid
	    * qD[  (b + 3)%4 ] // stupid, stupid convention in TLorentzVector
	    * qV1[ (c + 3)%4 ] // that index 0 = p_x, 1=py, 2=pz, 3=Energy
	    * qV2[ (d + 3)%4 ] // arrrrgh. ARRRRRRRGH!!! Rene!!! Why????
	    ; // (0 + 3)%4 = 3, (1+3)%4 = 0, (2+3)%4 = 1 (3+3)%4 = 2
	}
      }
    }
  }
  
  if(dbThis){
    cout << " compare leviCivitas " << sum << ", " 
	 << LeviCivita(pD, qD, qV1, qV2)
	 << endl;
  }
  return sum;
  */
  return LeviCivita(pD, qD, qV1, qV2)/(GeV*GeV*GeV*GeV);
}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_P::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_P"
     << "\n\t LeviCivita(pD, qD, qV1, qV2)"
     << "\n\t with pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -------------------------

double SF_DtoV1V2_V1toP0P1_V1toP2P3_D::getVal(){
  // double check this!!
  bool dbThis=false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV1 = p(0) + p(1);
  TLorentzVector qV1 = p(0) - p(1);
  TLorentzVector pV2 = p(2) + p(3);
  TLorentzVector qV2 = p(2) - p(3);
  
  double MV1 = mRes(V1);
  double MV2 = mRes(V2);
  
  double returnVal = 
         (  qV1.Dot(pV2) - qV1.Dot(pV1) * pV1.Dot(pV2)/(MV1*MV1)
	 )*( 
	    qV2.Dot(pV1) - qV2.Dot(pV2) * pV2.Dot(pV1)/(MV2*MV2)
	 )
         / (GeV*GeV*GeV*GeV)
    ;
  
  if(dbThis){
    TLorentzVector pD = -(pV1 + pV2);
    TLorentzVector qD = pV1 - pV2;
    double mD = pD.M();

   
    ZTspin1 tV1(qV1, pV1, MV1);
    ZTspin1 tV2(qV2, pV2, MV2);
    //ZTspin2 tD(pD + pV1, pD - pV1, mD);
    ZTspin2 tD(qD, pD, mD);
    //ZTspin2 tD(qD, pD, mD);
    
    double zResult = tV1.Contract(tD.Contract(tV2))/(GeV*GeV*GeV*GeV);
    cout << "SF_DtoV1V2_V1toP0P1_V1toP2P3_D compare: " << zResult << " / "
	 << returnVal << " = " << zResult/returnVal << endl;

  }

  return returnVal;
 
}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_D::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_S"
     << "\n\t (  qV1.Dot(pV2) - qV1.Dot(pV1) * pV1.Dot(pV2)/(MV1*MV1)"
     <<	"\n\t  )*( "
     << "\n\t	    qV2.Dot(pV1) - qV2.Dot(pV2) * pV2.Dot(pV1)/(MV2*MV2)"
     << "\n\t	 )"
     << "\n\t    / (GeV*GeV*GeV*GeV)"
     << "\n\t with pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//=========================================================

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(-321,  321);
    _exampleDecay->addDgtr( 113)->addDgtr( 211, -211);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  int P_index=0;
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == 'V' && ! dgtr->isFinalState()) V2 = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P' && dgtr->isFinalState()){
      fsPS[P_index++] = dgtr;
    }
  }
  if(0==V2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree"
	 << " Didn't find V " << V2.get() << endl;
    return false;
  }
  if(P_index != 2 || 0==(fsPS[0]) || 0==(fsPS[1])  ){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree"
	 << " Didn't find P1, P2 " << endl;
    return false;
  }
  if(V2->nDgtr() != 2){
    cout << "ERROR in SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V2->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = V2->getDgtrTreePtr(0);
  fsPS[3] = V2->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);
  
  printYourself();
  return true;
}

double SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::getVal(){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

    TLorentzVector pV1 = p(0) + p(1);
    TLorentzVector qV1 = p(0) - p(1);
    TLorentzVector pV2 = p(2) + p(3);
    TLorentzVector qV2 = p(2) - p(3);

    double MV1 = pV1.M();
    double MV2 = mRes(V2);

    return (qV1.Dot(qV2)
	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)
	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)
	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) 
	    / (MV1*MV1 * MV2*MV2)
	    )
      /(GeV*GeV)
      ;

}
void SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1V2_V1toP0P1_V1toP2P3_S_nonResV1"
     << " - I recommend not to use this, use nonRes particles insted "
     << " anyway, this is what I do:"
     << "\n\t (qV1.Dot(qV2)"
     << "\n\t 	    -    qV1.Dot(pV1) * pV1.Dot(qV2) / (MV1*MV1)"
     << "\n\t 	    -    qV1.Dot(pV2) * pV2.Dot(qV2) / (MV2*MV2)"
     << "\n\t 	    +    qV1.Dot(pV1) * pV1.Dot(pV2) * pV2.Dot(qV2) "
     << "\n\t 	    / (MV1*MV1 * MV2*MV2)"
     << "\n\t 	    )"
     << "\n\t   /(GeV*GeV)"
     << "\n\t with: pV1 = p(0) + p(1), qV1 = p(0) - p(1), pV2 = p(2) + p(3) qV2 = p(2) - p(3);"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr(    333)->addDgtr( 321, -321); // V
    _exampleDecay->addDgtr(9010221)->addDgtr(-211,  211); // S
  }
  return *_exampleDecay; // this example is not necessarily physical.
}

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoVS_VtoP0P1_StoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if     (dgtr->getVal().SVPAT() == 'V' && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == 'S' && ! dgtr->isFinalState()) S = dgtr;
  }
  if(0==V || 0==S){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3::parseTree"
	 << " Didn't find V or S " << V.get() << ", " << S.get() << endl;
    return false;
  }
  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = V->getDgtrTreePtr(0);
  fsPS[1] = V->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  printYourself();
  return true;
}

double SF_DtoVS_VtoP0P1_StoP2P3::getVal(){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

    TLorentzVector pS = p(2) + p(3);
    TLorentzVector pV = p(0) + p(1);
    TLorentzVector qV = p(0) - p(1);

    double MassV = mRes(V);

    return (pS.Dot(qV)
	    -    pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV)
	    )
      /(GeV*GeV)
      ;
}
void SF_DtoVS_VtoP0P1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVS_VtoP0P1_StoP2P3"
     << "\n\t ( pS.Dot(qV) - pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV) )  / (GeV*GeV)"
     << "\n\t with: pS = p(2) + p(3), pV = p(0) + p(1), qV = p(0) - p(1)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3_nonResV::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr( 321, -321);                   // V (non res)
    _exampleDecay->addDgtr(9010221)->addDgtr(-211,  211); // S
  }
  return *_exampleDecay; // this example is not necessarily physical.
}

const DecayTree& SF_DtoVS_VtoP0P1_StoP2P3_nonResV::exampleDecay(){
  return getExampleDecay();
}

bool SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  int P_index=0;
  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == 'S' && ! dgtr->isFinalState()) S = dgtr;
    else if(dgtr->getVal().SVPAT() == 'P' && dgtr->isFinalState()){
      fsPS[P_index++] = dgtr;
    }
  }
  if(0==S){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree"
	 << " Didn't find S " << S.get() << endl;
    return false;
  }
  if(P_index != 2 || 0==(fsPS[0]) || 0==(fsPS[1])  ){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree"
	 << " Didn't find P1, P2 " << endl;
    return false;
  }
  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoVS_VtoP0P1_StoP2P3_nonResV::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  printYourself();
  return true;
}

double SF_DtoVS_VtoP0P1_StoP2P3_nonResV::getVal(){
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

    TLorentzVector pS = p(2) + p(3);
    TLorentzVector pV = p(0) + p(1);
    TLorentzVector qV = p(0) - p(1);

    double MassV = pV.M();

    return (pS.Dot(qV)
	    -    pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV)
	    )
      /(GeV*GeV)
      ;
}
void SF_DtoVS_VtoP0P1_StoP2P3_nonResV::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVS_VtoP0P1_StoP2P3_nonResV"
     << " - recommend not to use this, use nonRes particles instead"
     << "\n anyway, I return:"
     << "\n\t (pS.Dot(qV) - pS.Dot(pV) * pV.Dot(qV) / (MassV*MassV) ) / (GeV*GeV)"
     << "\n\t with: pS = p(2) + p(3), pV = p(0) + p(1), qV = p(0) - p(1)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// ================================================

//=========================================================

const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_P::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr( 225)->addDgtr( 221, -211);
    _exampleDecay->addDgtr( 333)->addDgtr(-321,  321);
  }
  return *_exampleDecay;
}

const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_D::getExampleDecay(){
  if(0==_exampleDecayD){
    _exampleDecayD = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecayD->addDgtr( 225)->addDgtr( 221, -211);
    _exampleDecayD->addDgtr( 333)->addDgtr(-321,  321);

    _exampleDecayD->getVal().setL(2);
  }
  return *_exampleDecayD;
}


const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_P::exampleDecay(){
  return getExampleDecay();
}
const DecayTree& SF_DtoVT_VtoP0P1_TtoP2P3_D::exampleDecay(){
  return getExampleDecay();
}

const DecayTree& SF_DtoTS_TtoP0P1_StoP2P3::getExampleDecay(){
  if(0==_exampleDecay){
    _exampleDecay = new DecayTree(421);
    // remark: addDgtr always returns a pointer to the 
    // last daughter that was added, thus allowing these
    // chains:
    _exampleDecay->addDgtr( 225)->addDgtr( 221, -211);
    _exampleDecay->addDgtr( 9010221)->addDgtr(-321,  321);
  }
  return *_exampleDecay;
}
const DecayTree& SF_DtoTS_TtoP0P1_StoP2P3::exampleDecay(){
  return getExampleDecay();
}



bool SF_DtoTS_TtoP0P1_StoP2P3::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }

  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == 'T' && ! dgtr->isFinalState()) T = dgtr;
    else if(dgtr->getVal().SVPAT() == 'S' && ! dgtr->isFinalState()) S = dgtr;
  }

  if(0==S || 0==T){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " Didn't find T or S " << T.get() << ", " << S.get() << endl;
    return false;
  }
  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = T->getDgtrTreePtr(0);
  fsPS[1] = T->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(S->nDgtr() != 2){
    cout << "ERROR in SF_DtoTS_TtoP0P1_StoP2P3::parseTree"
	 << " S should have 2 daughters, but it says it has "
	 << S->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = S->getDgtrTreePtr(0);
  fsPS[3] = S->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  printYourself();
  return true;
}

bool SF_DtoVT_VtoP0P1_TtoP2P3_P::parseTree(){
  //  bool debugThis=false;
  if(fsPS.size() < 4) fsPS.reserve(4);
  if(! theDecay().nDgtr() == 2){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_S::parseTree"
	 << " expected exactly 2 daughers of D, have "
	 << theDecay().nDgtr();
    return false;
  }

  for(int i=0; i< theDecay().nDgtr(); i++){
    const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
    if(dgtr->getVal().SVPAT() == 'V' && ! dgtr->isFinalState()) V = dgtr;
    else if(dgtr->getVal().SVPAT() == 'T' && ! dgtr->isFinalState()) T = dgtr;
  }

  if(0==V || 0==T){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_S::parseTree"
	 << " Didn't find V or T " << V.get() << ", " << T.get() << endl;
    return false;
  }
  if(V->nDgtr() != 2){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_S::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[0] = V->getDgtrTreePtr(0);
  fsPS[1] = V->getDgtrTreePtr(1);
  normalOrder(fsPS[0], fsPS[1]);

  if(T->nDgtr() != 2){
    cout << "ERROR in SF_DtoVT_VtoP0P1_TtoP2P3_S::parseTree"
	 << " T should have 2 daughters, but it says it has "
	 << T->nDgtr() << "."
	 << endl;
    return false;
  }
  fsPS[2] = T->getDgtrTreePtr(0);
  fsPS[3] = T->getDgtrTreePtr(1);
  normalOrder(fsPS[2], fsPS[3]);

  printYourself();
  return true;
}

double SF_DtoVT_VtoP0P1_TtoP2P3_P::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV = p(0) + p(1);
  TLorentzVector qV = p(0) - p(1);
  TLorentzVector pT = p(2) + p(3);
  TLorentzVector qT = p(2) - p(3);
  
  double MV = mRes(V);
  double MT = mRes(T);
  
  ZTspin1 tV(qV, pV, MV);
  ZTspin2 tT(qT, pT, MT);
  
  const double units = GeV*GeV*GeV*GeV;

  double returnVal = (tT.Contract(tV)).Dot(pV) / units;

  if(dbThis){
    cout << " SF_DtoVT_VtoP0P1_TtoP2P3_P::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}
void SF_DtoVT_VtoP0P1_TtoP2P3_P::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVT_VtoP0P1_TtoP2P3_P"
     << "\n\t T1(V)_{\\mu}  T2(T)^{\\mu\\nu} pV_{\\nu}"
     << "\n\t implemented as: (tT.Contract(tV)).Dot(pV) / GeV^4"
     << "\n\t with T1(V) = ZTspin1 tV(qV, pV, MV), T2(T)= ZTspin2 tT(qT, pT, MT);"
     << "\n\t and pV = p(0) + p(1), qV = p(0) - p(1), pT = p(2) + p(3), qT = p(2) - p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
// -------------------------------------

double SF_DtoVT_VtoP0P1_TtoP2P3_D::getVal(){
  bool dbThis=true;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pV = p(0) + p(1);
  TLorentzVector qV = p(0) - p(1);
  TLorentzVector pT = p(2) + p(3);
  TLorentzVector qT = p(2) - p(3);

  TLorentzVector pD = pT + pV;
  TLorentzVector qD = pT - qV;
  
  double MV = mRes(V);
  double MT = mRes(T);
  
  ZTspin1 tV(qV, pV, MV);
  ZTspin2 tT(qT, pT, MT);
  TLorentzVector vecT(tT.Contract(pV));

  const double units = GeV*GeV * GeV*GeV * GeV*GeV;

  double returnVal = LeviCivita(tV, vecT, qD, pD) /units;


  if(dbThis){
    cout << " SF_DtoVT_VtoP0P1_TtoP2P3_D::getVal "
	 << " returning " << returnVal
	 << endl;
    double altVal = LeviCivita(p(0), p(1), p(2), p(3))/(GeV*GeV*GeV*GeV);
    cout << "check: " << altVal << " ratio: " << altVal/returnVal << endl;
  }
  return returnVal;
  
}
void SF_DtoVT_VtoP0P1_TtoP2P3_D::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoVT_VtoP0P1_TtoP2P3_D"
     << "\n\t ZTspin1 tV(qV, pV, MV);"
     << "\n\t  ZTspin2 tT(qT, pT, MT);"
     << "\n\t   TLorentzVector vecT(tT.Contract(pV));"
     << "\n\t  const double units = GeV*GeV * GeV*GeV * GeV*GeV;"
     << "\n\t  double returnVal = LeviCivita(tV, vecT, qD, pD) /units;"
     << "\n\t with pV = p(0) + p(1), qV = p(0) - p(1), pT = p(2) + p(3), qT = p(2) - p(3)"
     << "\n\t and  pD = pT + pV, qD = pT - qV;"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

// -----------------------------------------------

double SF_DtoTS_TtoP0P1_StoP2P3::getVal(){
  bool dbThis=false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();
  
  TLorentzVector pT = p(0) + p(1);
  TLorentzVector qT = p(0) - p(1);
  TLorentzVector pS = p(2) + p(3);
  TLorentzVector qD = pT - pS;
  
  double MT = mRes(T);
  
  ZTspin2 tT(qT, pT, MT);
  
  const double units = GeV*GeV*GeV*GeV;

  double returnVal = (tT.Contract(qD)).Dot(qD) / units;

  if(dbThis){
    cout << " SF_DtoTS_TtoP0P1_StoP2P3::getVal "
	 << " returning " << returnVal
	 << endl;
  }
  return returnVal;
  
}

void SF_DtoTS_TtoP0P1_StoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoTS_TtoP0P1_StoP2P3"
     << "\n\t  ZTspin2 tT(qT, pT, MT);"
     << "\n\t  return: (tT.Contract(qD)).Dot(qD) / GeV^4"
     << "\n\t with pT = p(0) + p(1), qT = p(0) - p(1), pS = p(2) + p(3), qD = pT - qV;"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}


// ==========================================
// provided by Loic Esteve:
const DecayTree& SF_DtoV1P0_V1toV2P1_V2toP2P3::getExampleDecay(){
 if(0==_exampleDecay){
   _exampleDecay = new DecayTree(421);
   // remark: addDgtr always returns a pointer to the 
   // last daughter that was added, thus allowing these
   // chains:
   _exampleDecay->addDgtr(310, 223)->addDgtr(111, 113)->addDgtr(211, -211);
 }
 return *_exampleDecay;
}
const DecayTree& SF_DtoV1P0_V1toV2P1_V2toP2P3::exampleDecay(){
 return getExampleDecay();
}


bool SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree(){
 //  bool debugThis=false;
 if(fsPS.size() < 4) fsPS.reserve(4);
 for(int i=0; i< theDecay().nDgtr(); i++){
   const_counted_ptr<AssociatedDecayTree> dgtr= theDecay().getDgtrTreePtr(i);
   if     (dgtr->getVal().SVPAT() == 'P' &&   dgtr->isFinalState()) fsPS[0] = dgtr;
   else if(dgtr->getVal().SVPAT() == 'V' && ! dgtr->isFinalState()) V1 = dgtr;
 }
 if(0==V1 || 0==fsPS[0]){
   cout << "ERROR in SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree"
	 << " Didn't find V1 or P0 " << V1.get() << ", " << fsPS[0].get() << endl;
   return false;
 }

 for(int i=0; i< V1->nDgtr(); i++){
   const_counted_ptr<AssociatedDecayTree> dgtr= V1->getDgtrTreePtr(i);
   if     (dgtr->getVal().SVPAT() == 'V' && ! dgtr->isFinalState()) V2 = dgtr;
   else if(dgtr->getVal().SVPAT() == 'P' &&   dgtr->isFinalState()) fsPS[1] = dgtr;
 }
 if(0==V2 || 0==fsPS[1]){
   cout << "ERROR in SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree"
	 << " Didn't find V2 or P1 " << V2.get() << ", " << fsPS[1].get() << endl;
   return false;
 }

 if(V2->nDgtr() != 2){
   cout << "ERROR in SF_DtoV1P0_V1toV2P1_V2toP2P3::parseTree"
	 << " V should have 2 daughters, but it says it has "
	 << V2->nDgtr() << "."
	 << endl;
   return false;
 }
 fsPS[2] = V2->getDgtrTreePtr(0);
 fsPS[3] = V2->getDgtrTreePtr(1);
 normalOrder(fsPS[2], fsPS[3]);

 printYourself();
 return true;
}

double SF_DtoV1P0_V1toV2P1_V2toP2P3::getVal(){
 if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

 TLorentzVector pV1 = p(1) + p(2) + p(3);
 TLorentzVector pP1 = p(0);

 TLorentzVector qV1 = (p(2) + p(3)) - p(1);
 TLorentzVector qV2 = p(2) - p(3);

 double units = GeV*GeV*GeV*GeV;
 return LeviCivita(pV1, qV1, pP1, qV2)/units;
}

void SF_DtoV1P0_V1toV2P1_V2toP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;
  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoV1P0_V1toV2P1_V2toP2P3"
     << "\n\t  return: LeviCivita(pV1, qV1, pP1, qV2)/ / GeV^4"
     << "\n\t with pV1 = p(1) + p(2) + p(3), pP1 = p(0)"
     << "\n\t and qV1 = (p(2) + p(3)) - p(1), qV2 = p(2) - p(3)"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}

//=========================================================

double SF_DtoAP0_AtoVP1Dwave_VtoP2P3::getVal(){
  //  bool debugThis = false;

  // from pg 35 of Phys.Rev.D75:052003,2007
  // at http://prd.aps.org/abstract/PRD/v75/i5/e052003
  // and at http://arxiv.org/abs/hep-ex/0701001
  // except that we use MA^2 instead of p_a^2 and MV^2 instead of p_v^2

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) parseTree();

  TLorentzVector pV = p(2) + p(3);
  TLorentzVector qV = p(2) - p(3);

  TLorentzVector pA = p(1) + p(2) + p(3);
  TLorentzVector qA = p(1) - (p(2) + p(3));


  double MA = mRes(A);
  double MV = mRes(V);
  
  ZTspin1 tA(qA, pA, MA);
  ZTspin1 tV(qV, pV, MV);

  double units = GeV*GeV*GeV*GeV;

  return p(0).Dot(tA) * tV.Dot(pA) / units;
}
void SF_DtoAP0_AtoVP1Dwave_VtoP2P3::printYourself(ostream& os) const{
  //  bool debugThis = false;

  if(! ( fsPS[0] && fsPS[1] && fsPS[2] && fsPS[3]) ) return;
  os << "spin factor SF_DtoAP0_AtoVP1Dwavve_VtoP2P3"
     << " with\t pV = p(2) + p(3), qV = p(2) - p(3) "
     << "\n\t and pA = p(1) + p(2) + p(3), qA = p(1) - (p(2) + p(3));"
     << "\n\t evaluates as:"
     << "\n\t ZTspin1 tA(qA, pA, MA);"
     << "\n\t ZTspin1 tV(qV, pV, MV);"
     << "\n\t p(0).Dot(tA) * tV.Dot(pA) / (GeV*GeV*GeV*GeV);"
     << "\n\t    parsed tree " << theDecay().oneLiner()
     << "\n      like this:" << endl;
  this->printParsing(os);
}
//=========================================================
//
//
