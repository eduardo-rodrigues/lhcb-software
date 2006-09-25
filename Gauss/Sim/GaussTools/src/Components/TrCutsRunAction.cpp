/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// G4
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
/// GaussTools
#include "MinEkineCuts.h"
#include "LoopCuts.h"
#include "WorldCuts.h"
/// Local 
#include "TrCutsRunAction.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : TrCutsRunAction
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 *  @date 10/04/2003 
 */
// ============================================================================

// ============================================================================
/// Factory business
// ============================================================================
IMPLEMENT_GiGaFactory( TrCutsRunAction ) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 */
// ============================================================================
TrCutsRunAction::TrCutsRunAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaRunActionBase( type , name , parent )
    , m_ecut(1.0) //MeV
    , m_phcut(1.0) //MeV
    , m_pKpicut( 10.0 ) //MeV
    , m_ocut(0.0) //MeV
    , m_ncut(10.0) //MeV
    , m_nucut(0.0) //MeV
    , m_mcut(10.0) //MeV
    , m_killloops(true)
    , m_maxsteps(100)
    , m_minstep(0.001)
    , m_minx(-10000.0), m_miny(-10000.0), m_minz(-5000.0)
    , m_maxx(10000.0), m_maxy(10000.0), m_maxz(25000.0)
{  
  declareProperty("ElectronTrCut", m_ecut);
  declareProperty("GammaTrCut", m_phcut);
  declareProperty("pKpiCut" , m_pKpicut ) ;
  declareProperty("NeutronTrCut", m_ncut);
  declareProperty("NeutrinoTrCut", m_nucut);
  declareProperty("MuonTrCut", m_mcut);
  declareProperty("OtherTrCut" , m_ocut ) ;
  declareProperty("KillLoops", m_killloops);
  declareProperty("MaxNumberSteps", m_maxsteps);
  declareProperty("MininumStep", m_minstep);
  declareProperty("MinX", m_minx);
  declareProperty("MinY", m_miny);
  declareProperty("MinZ", m_minz);
  declareProperty("MaxX", m_maxx);
  declareProperty("MaxY", m_maxy);
  declareProperty("MaxZ", m_maxz);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
TrCutsRunAction::~TrCutsRunAction()
{
};

// ============================================================================
/** performe the action at the begin of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void TrCutsRunAction::BeginOfRunAction( const G4Run* run )
{
  if ( 0 == run ) 
  { Warning ( "BeginOfRunAction:: G4Run* points to NULL!" ) ; }
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  if ( 0 == particleTable ) 
  { Error ( "G4ParticleTable points to NULL!; return! ") ; return ; }
  
  int ii, ptbSiz = particleTable->size();
  
  for(ii = 0; ii < ptbSiz; ii++)
    {
      G4ParticleDefinition* particle = particleTable->GetParticle( ii );
      
      if ( 0 == particle ) 
      {
        Warning ( "G4ParticleDefinition* points to NULL, skip it" ) ;
        continue ;
      }
      
      int particleCode = particle->GetPDGEncoding();
      double acut;

      if(abs(particleCode)==11)
        {
          acut=m_ecut;
        }
      else if(particleCode==22)
        {
          acut=m_phcut;
        }
      else if(abs(particleCode)==13)
        {
          acut=m_mcut;
        }
      else if(abs(particleCode)==2112)
        {
          acut=m_ncut;
        }
      else if(abs(particleCode)==12 || abs(particleCode)==14 
              || abs(particleCode)==16)
        {
          // do not track neutrinos
//    acut=100000000.0;
          acut=m_nucut;
        }
      else if ( (abs(particleCode) == 2212 ) || (abs(particleCode) == 321 )
                || (abs(particleCode) == 211 ) ) 
        {
          acut=m_pKpicut;  
        }
      else 
        {
          acut = m_ocut ;
        }      

      std::string pname=particle->GetParticleName();
      
      if ( (pname!="opticalphoton") && ( ! particle->IsShortLived() ) )
        {          
          G4ProcessManager* procMgr = particle->GetProcessManager();
          if ( 0 == procMgr ) 
          {
            Error("G4ProcessManager* points to NULL!") ;
            return ;
          }
          
          procMgr->AddDiscreteProcess(new MinEkineCuts("MinEkineCut",acut) );
          procMgr->AddDiscreteProcess(new WorldCuts("WorldCut",
                                                    m_minx,m_miny,m_minz,
                                                    m_maxx,m_maxy,m_maxz));
          if ( (pname=="e-" || pname=="gamma" ) && m_killloops)  
            procMgr->
              AddDiscreteProcess(new LoopCuts("LoopCuts",m_maxsteps,m_minstep));
          
        }
    }
};
// ============================================================================

// ============================================================================
/** performe the action at the end of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void TrCutsRunAction::EndOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("EndOfRunAction:: G4Run* points to NULL!") ; }

};
// ============================================================================

// ============================================================================
// The End
// ============================================================================

