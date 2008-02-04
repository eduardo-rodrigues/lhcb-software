// $Id: IBackgroundCategory.h,v 1.3 2008-02-04 16:51:29 pkoppenb Exp $
#ifndef DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H 
#define DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/IAlgTool.h"
#include <map>

//using namespace LHCb;

// forward declarations
//class Particle;
//class MCParticle;

static const InterfaceID IID_IBackgroundCategory ( "IBackgroundCategory", 1, 0 );

/** @class IBackgroundCategory IBackgroundCategory.h pkoppenb/IBackgroundCategory.h
 *
 *  Returns the background category for a reconstructed candidate Particle. Many thanks
 *  go to all the users who tested this tool, but especially to Luis Fernandez,
 *  Patrick Koppenburg and Jeremie Borel for their assistance in the design and extensive
 *  help in ironing out bugs. 
 *
 *Conditions:
 *
 *  -  <b>A:</b> all final-state particles used to form the candidate are matched to 
 *        decay products of the same true MC particle (not necessarily the signal) 
 *
 *  -  <b>B:</b> all final-state MC particles originating from the true MC particle 
 *   defined in A are matched to particles used to form the candidate
 *   (except photons generated by PHOTOS);
 *   in case the decay descriptor of the true MC particle
 *   defined in A corresponds to an inclusive decay, "all final-state MC
 *   particles" only include the particles required in this (semi-)inclusivei
 *   decay. 
 *
 *  -  <b>C:</b> all final-state particles used to form the candidate are correctly 
 *  identified, i.e. have been assigned their correct (true) mass
 *  
 *  -  <b>D:</b> the true MC particle defined in A is a signal decay according 
 *   to the decay descriptor, or the head of a decay chain which differs 
 *  from the signal decay chain only by the presence or absence of 
 *  intermediate resonances but has other otherwise the same head,
 * same final state particles and same topology.
 *
 *  -  <b>E:</b> the true MC particle defined in A is a signal decay according 
 *  to the decay descriptor, and all intermediate states of this 
 * decay are correctly reconstructed (as listed in the decay descriptor)
 *
 *  -  <b>F:</b> the true MC particle defined in A has a mass which does not exceed
 *  the mass of the head of the decay descriptor by more than 100 MeV/c2
 *  (tunable parameter)
 *
 *  -  <b>G:</b> at least one final-state particle used to form the candidate is a ghost
 *  
 *  -  <b>H:</b> final-state particles used to form the candidate are matched to 
 *  true particles from at least two different collisions (pileup)
 *  
 *  -  <b>I:</b> at least one final-state particles used to form the candidate is 
 *  matched to a true decay product of a b-hadron 
 *  (following mother-daughter relationships all the way through)
 *   
 *  -  <b>J:</b> at least one final-state particles used to form the candidate is 
 *  matched to a true decay product of a c-hadron
 *  (following mother-daughter relationships all the way through)
 *  
 *
 *<b>Categories:</b>
 *
 *  -  <b>0:   Signal                  </b>  A &amp;&amp; B &amp;&amp; C &amp;&amp; D &amp;&amp; E
 *
 *  -  <b>1:   Quasi-signal            </b>    A &amp;&amp; B &amp;&amp; C &amp;&amp; D &amp;&amp; !E
 *
 *  -  <b>2:   Phys. back. (full rec.) </b>   A &amp;&amp; B &amp;&amp; C &amp;&amp; !D
 *
 *  -  <b>3:   Reflection (mis-ID)     </b>    A &amp;&amp; B &amp;&amp; !C
 *  
 *  -  <b>4:   Phys. back. (part. rec.)</b>    A &amp;&amp; !B &amp;&amp; !(C &amp;&amp; F)
 *
 *  -  <b>5:   Low-mass background     </b>   A &amp;&amp; !B &amp;&amp; C &amp;&amp; F 
 *
 *  -  <b>6:   Ghost                   </b>  !A &amp;&amp; G (= G)
 *
 *  -  <b>7:   Pileup/FromDifferentPV                  </b>   !A &amp;&amp; !G &amp;&amp; H (= !E &amp;&amp; H)
 *
 *  -  <b>8:   bb event                </b>   !A &amp;&amp; !G &amp;&amp; !H &amp;&amp; I
 *
 *  -  <b>9:   cc event                </b>   !A &amp;&amp; !G &amp;&amp; !H &amp;&amp; !I &amp;&amp; J  
 *   
 *  -  <b>10:   light-flavour event    </b>   !A &amp;&amp; !G &amp;&amp; !H &amp;&amp; !I &amp;&amp; !J
 *
 *
 *
 *<b>Examples:</b>
 *
 *a) A reconstructed Bs -> (Ds -> KKpi) pi decay could be categorized 
 *  as signal with respect to the decay descriptor 
 *  {[[B_s0]nos -> (D_s- => K+ K- pi-) pi+]cc, 
 *   [[B_s0]os  -> (D_s+ => K- K+ pi+) pi-]cc}
 *  even if the MC truth indicates a Bs -> (Ds -> (phi -> KK) pi) pi decay.
 *  
 *b) A reconstructed Bd -> K* mumu should not be categorized as a signal
 *  with respect to the decay descriptor 
 *  {[[B0]nos -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma}) 
 *               (K*(892)0 -> K+ pi-)]cc, 
 *   [[B0]os  -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma})
 *               (K*(892)~0 -> K- pi+)]cc} 
 *  no matter what the MC truth indicates.   
 *   
 *c) A reconstructed Bd -> K* mumu should not be categorized as a signal 
 *  with respect to the decay descriptor 
 *  {[[B0]nos -> mu+ mu- (K*(892)0 -> K+ pi-)]cc, 
 *   [[B0]os -> mu- mu+ (K*(892)~0 -> K- pi+)]cc} 
 *  if the MC truth indicates a Bd -> K* J/psi(mumu) decay.
 *
 *d) A reconstructed Bs -> mu+mu- candidate matched to a true 
 *  J/psi -> mu+mu- decay (could happen if the sidebands around the Bs 
 *  are very large) and a reconstructed Bd -> K+K- matched to a true 
 *  Bs -> K+K- are both classified as fully-reconstructed physics 
 *  background.
 *  
 *e) A reconstructed Bs -> (Ds -> KKpi) pi candidate matched to a 
 *  a true Bd -> (D- -> Kpipi) pi decay (mis-ID of one of the pions 
 *  from the D- as a kaon) is classified as a reflection.
 *  
 *f) A reconstructed B- -> (D0 -> K-pi+) pi- candidate matched to 
 *  to the K-,pi+ and pi- of a true Lambda_b -> (Lambda_c -> p K-pi+) pi-
 *  is classified as a partially-reconstructed physics background.
 *  
 *g) A reconstructed Bs -> (Ds -> KKpi) pi+ candidate with all 4 tracks
 *  matched to the true final state particles of a true 
 *  Bs -> (Bs ->KKpi) pi+pi0 decay is classified as a low-mass background. 
 * 
 * Please note that there are now two additional categories:
 * 
 * FromPV : The candidate particle has at least one final state daughter which
 *          comes directly from the primary vertex or a short lived resonance,
 *          and the event is not a pileup. 
 *
 * AllFromSamePV : The candidate particle's final state daughters all come from
 *                 the same PV, or from short lived resonances from the same PV.
 * 
 *  @author Vladimir Gligorov
 *  @date   2005-11-03
 */
class IBackgroundCategory : virtual public IAlgTool {
public: 

  /// Destructor
  virtual ~IBackgroundCategory(){};

  /// enum defining categories. From 1000 on, specific tools can add codes.

  enum categories { Undefined = -1, 
                    Signal = 0,
                    QuasiSignal = 10,
                    FullyRecoPhysBkg = 20,
                    Reflection = 30,
                    PartRecoPhysBkg = 40,
                    LowMassBkg = 50,
                    Ghost = 60,
                    FromPV = 70,
                    AllFromSamePV = 80,
                    FromDifferentPV = 100,
                    bbar = 110 ,
                    ccbar = 120 ,
                    uds = 130,
                    LastGlobal = 1000 } ;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBackgroundCategory; }

  /// Method that returns the category
  virtual IBackgroundCategory::categories category(const LHCb::Particle*) = 0;
  
  /// Method that returns the MC mother particle associated to the background if any
  /// For a signal it returns the true signal particle, and for any background with a 
  /// category < 60 it returns the mc_mother of the final state particles
  virtual const LHCb::MCParticle* origin( const LHCb::Particle* ) = 0 ;

  //Method to return the daughters of the candidate Particle as used by BackCat
  virtual const std::vector<std::pair<const LHCb::Particle*, const LHCb::MCParticle*> > 
  getDaughtersAndPartners( const LHCb::Particle* ) = 0 ;

  /// Returns the map between enum values and enum names of the categories
  const std::map<int, std::string>& getCategoryMap(){return m_cat;};

protected:
  std::map<int, std::string> m_cat;

private:

};
#endif // DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H
