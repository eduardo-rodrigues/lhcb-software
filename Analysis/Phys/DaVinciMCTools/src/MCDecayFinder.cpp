// $Id: MCDecayFinder.cpp,v 1.9 2004-03-16 18:48:36 pkoppenb Exp $
// Include files 
#include <list>
#include <functional>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// from EventSys
#include "Event/MCParticle.h"

// local
#include "MCDecayFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCDecayFinder
//
// 20/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MCDecayFinder>          s_factory ;
const        IToolFactory& MCDecayFinderFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCDecayFinder::MCDecayFinder( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : AlgTool ( type, name , parent ),
    m_ppSvc(0), m_source("B0 -> pi+ pi-"), m_decay(0), m_members(0)
{
  if( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = serviceLocator()->service("ParticlePropertySvc",m_ppSvc);
  }
  if( !m_ppSvc ) {
    throw GaudiException( "ParticlePropertySvc not found",
                          "MCDecayFinderException",
                          StatusCode::FAILURE );
  }

  declareInterface<IMCDecayFinder>(this);

  declareProperty( "Decay", m_source );
  declareProperty( "ResonanceThreshold", m_resThreshold = 1e-15*second );
}

//=============================================================================
// Standard destructor
//=============================================================================

MCDecayFinder::~MCDecayFinder( )
{
  if( m_decay )
    delete m_decay;
  if( m_members )
    delete m_members;
}

//=============================================================================

StatusCode MCDecayFinder::initialize()
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Initializing" << endreq;

  StatusCode sc = service("EventDataSvc", m_EDS, true);   
  if( sc.isFailure() ) {
    log << MSG::FATAL << " Unable to locate Event Data Service" << endreq;
    return sc;
  }

  if( m_source.length() == 0 )
  {
    log << MSG::WARNING << "No decay specified!" << endreq;
    return StatusCode::SUCCESS;
  }
  if( compile(m_source) )
  {
    log << MSG::DEBUG << "The compilation of the decay was successfull"
        << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::DEBUG << "Could not compile the decay description" << endreq;
  return StatusCode::FAILURE;
}

StatusCode MCDecayFinder::setDecay( std::string decay )
{
  MsgStream log( msgSvc(), name() );

  Descriptor *old_decay = m_decay;
  std::vector<ParticleMatcher *> *old_members = m_members;
  // Be sure we don't get a old piece if the feature is not used.
  m_decay = NULL;
  m_members = NULL;

  log << MSG::DEBUG << "Setting decay to " << decay << endreq;
  if( compile(decay) ) {
    log << MSG::DEBUG << "The compilation of the decay was successfull"
        << endreq;
    m_source = decay;
    if( old_decay )
      delete old_decay;
    if( old_members ) {
      std::vector<ParticleMatcher *>::iterator pm_i;
      for( pm_i = old_members->begin(); pm_i != old_members->end(); pm_i++ )
        delete *pm_i;
      old_members->clear();
      delete old_members;
    }
    return StatusCode::SUCCESS;
  }

  // Restore previous decay if compilation failed.
  if( m_decay && (m_decay != old_decay) )
    delete m_decay;
  m_decay = old_decay;
  if( m_members && (m_members != old_members) ) {
    std::vector<ParticleMatcher *>::iterator pm_i;
    for( pm_i = m_members->begin(); pm_i != m_members->end(); pm_i++ )
      delete *pm_i;
    m_members->clear();
    delete m_members;
  }
  m_members = old_members;
  log << MSG::DEBUG << "Could not compile the decay description" << endreq;

  return StatusCode::FAILURE;
}

std::string MCDecayFinder::revert( void )
{
  std::string result = "";
  if( m_decay == NULL )
    return result;
  bool alt = m_decay->getAlternate() != NULL;
  if( alt )
    result += "{ ";
  Descriptor *a = m_decay;
  while( a ) {
    result += a->describe();
    if( a->getAlternate() ) {
      result += ", ";
    }
    a = a->getAlternate();
  }
  if( alt )
    result += " }";
  return result;
}

#include "mclexer.icpp"
#include "mcparser.icpp"

bool MCDecayFinder::compile( std::string &source )
{
  MsgStream log(msgSvc(), name());
  yy_buffer_state *bs = yy_scan_string( source.c_str() );
  try
  {
    if( yparse() )
      throw("Syntax Error");
  }
  catch( DescriptorError e )
  {
    log << MSG::ERROR << "Invalid decay description '"
        << source << "'" << endreq;
    log << MSG::ERROR << e.cause() << endreq;
    yy_delete_buffer(bs);
    return false;
  }
  yy_delete_buffer(bs);
  log << MSG::DEBUG << "Result of the compilation:\n"
      << revert() << endreq;
  return true;
}

bool MCDecayFinder::hasDecay( const std::vector<MCParticle*> &event )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "About to test the event" << endreq;
  const MCParticle *drop_me = NULL;
  if( m_decay )
    return m_decay->test( event.begin(), event.end(), drop_me );
  else
  {
    log << MSG::WARNING << "Trying to find an unspecified decay!" << endreq;
    return false;
  }
}

bool MCDecayFinder::findDecay( const std::vector<MCParticle*> &event,
                                   const MCParticle *&previous_result )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "About to test the event" << endreq;
  if( m_decay )
    return m_decay->test( event.begin(), event.end(), previous_result );
  else
  {
    log << MSG::WARNING << "Trying to find an unspecified decay!" << endreq;
    return false;
  }
}

bool MCDecayFinder::hasDecay( const MCParticles &event )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "About to test the event" << endreq;
  const MCParticle *drop_me = NULL;
  if( m_decay )
    return m_decay->test( event.begin(), event.end(), drop_me );
  else
  {
    log << MSG::WARNING << "Trying to find an unspecified decay!" << endreq;
    return false;
  }
}

bool MCDecayFinder::findDecay( const MCParticles &event,
                               const MCParticle *&previous_result )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "About to test the event" << endreq;
  if( m_decay )
    return m_decay->test( event.begin(), event.end(), previous_result );
  else
  {
    log << MSG::WARNING << "Trying to find an unspecified decay!" << endreq;
    return false;
  }
}

bool MCDecayFinder::hasDecay( void )
{
  MsgStream log( msgSvc(), name() );
  SmartDataPtr<MCParticles> mcparts(m_EDS, MCParticleLocation::Default );
  if( !mcparts )
  {
    log << MSG::FATAL << "Enable to find MC particles at '"
        << MCParticleLocation::Default << "'" << endreq;
    return false;
  }
  return hasDecay( *mcparts );
}

bool MCDecayFinder::findDecay( const MCParticle *&previous_result )
{
  MsgStream log( msgSvc(), name() );
  SmartDataPtr<MCParticles> mcparts(m_EDS, MCParticleLocation::Default );
  if( !mcparts )
  {
    log << MSG::FATAL << "Enable to find MC particles at '"
        << MCParticleLocation::Default << "'" << endreq;
    return false;
  }
  return findDecay( *mcparts, previous_result );
}

void MCDecayFinder::descendants( const MCParticle *head,
                                 std::vector<MCParticle *>&result,
                                 bool leaf )
{
  bool term = true;
  SmartRefVector<MCVertex>::const_iterator vi;
  for( vi = head->endVertices().begin(); vi != head->endVertices().end(); vi++ )
  {
    SmartRefVector<MCParticle>::const_iterator pi;
    for( pi = (*vi)->products().begin(); pi != (*vi)->products().end(); pi++ )
    {
      term = false;
      descendants( *pi, result, leaf );
    }
  }
 
  if( !leaf || term )
    result.push_back(const_cast<MCParticle *>(head));
}

void MCDecayFinder::decayMembers( const MCParticle *head,
                                 std::vector<MCParticle *>&members )
{
  if( m_members ) {
    std::vector<MCParticle *> flat;
    descendants( head, flat, false );
    std::vector<MCParticle *>::const_iterator pi;
    for( pi = flat.begin(); pi != flat.end(); pi++ ) {
      std::vector<ParticleMatcher *>::const_iterator mi;
      for( mi = m_members->begin(); mi != m_members->end(); mi++ )
        if( (*mi)->test(*pi) ) {
          members.push_back(*pi);
          break;
        }
    }
  }

  m_decay->test(head, &members);
}

MCDecayFinder::Descriptor::Descriptor( IParticlePropertySvc *ppSvc,
                                       double rThre)
  : mother(0), daughters(0), skipResonnance(false),
    elipsis(false), m_resThreshold(rThre), m_ppSvc(ppSvc), alternate(0)
{}

MCDecayFinder::Descriptor::Descriptor( Descriptor &copy )
  : mother(0), daughters(0), skipResonnance(false),
    elipsis(false), m_resThreshold(0), m_ppSvc(0), alternate(0)
{
  if( copy.mother )
    mother = new ParticleMatcher(*copy.mother);
  std::vector<Descriptor *>::iterator d;
  for( d=copy.daughters.begin(); d!=copy.daughters.end(); d++ ) {
    daughters.push_back(new Descriptor(**d));
  }
  skipResonnance = copy.skipResonnance;
  elipsis  = copy.elipsis;
  m_resThreshold = copy.m_resThreshold;
  m_ppSvc = copy.m_ppSvc;
}

MCDecayFinder::Descriptor::Descriptor( ParticleMatcher *m,
                                       IParticlePropertySvc *ppSvc,
                                       double rThre)
  : mother(m), daughters(0), skipResonnance(false),
    elipsis(false), m_resThreshold(rThre), m_ppSvc(ppSvc), alternate(0)
{}

MCDecayFinder::Descriptor::~Descriptor()
{
  if( mother )
    delete mother;
  std::vector<Descriptor *>::iterator di;
  for( di = daughters.begin(); di != daughters.end(); di++ )
    delete *di;
  if( alternate )
    delete alternate;
}

std::string MCDecayFinder::Descriptor::describe( void )
{
  std::string result = "";
  if( !daughters.empty() )
    result += '(';
  if( mother )
    result += mother->describe();
  else
    result += "pp";
  if( !daughters.empty() ) {
    if( skipResonnance )
      result += " => ";
    else
      result += " -> ";
    std::vector<Descriptor *>::const_iterator di;
    for( di=daughters.begin(); di!=daughters.end(); di++ ) {
      result += (*di)->describe();
      if( *di != daughters.back() )
        result += ' ';
    }
  }
  if( elipsis )
    result += " ...";
  if( !daughters.empty() )
    result += ')';
  return result;
}

bool MCDecayFinder::Descriptor::test( const MCParticle *part,
                                      std::vector<MCParticle*> *collect )
{
  std::vector<MCParticle*> local_collect(0);
  std::vector<MCParticle*> *local = NULL;
  if( collect )
    local = &local_collect;
  bool result = false;
  if( mother && mother->test(part,local) )
  {
    if( daughters.empty() )
      result = true; // Nothing to test for the daughters.
    else {
      std::list<const MCParticle *> parts;
      SmartRefVector<MCVertex>::const_iterator vi;
      for ( vi = part->endVertices().begin();
            vi != part->endVertices().end(); vi++)
      {
        SmartRefVector<MCParticle>::const_iterator idau;
        for ( idau = (*vi)->products().begin();
              idau != (*vi)->products().end(); idau++ )
        {
          parts.push_back(*idau);
        }
      }
      if( skipResonnance )
        filterResonnances( parts );

      result = testDaughters(parts,local);
    }
  }
  if( result ) {
    if( collect )
      collect->insert( collect->end(),
                       local_collect.begin(), local_collect.end() );
    return true;
  }
  if( alternate )
    return alternate->test(part,collect);
  return false;
}

bool
MCDecayFinder::Descriptor::testDaughters(std::list<const MCParticle*> &parts,
                                         std::vector<MCParticle*> *collect)
{
  std::vector<Descriptor *>::iterator di;
  for( di = daughters.begin();
       (di != daughters.end()) && !parts.empty(); di++ )
  {
    std::list<const MCParticle *>::iterator p = parts.begin();
    while( p != parts.end() && ((*di)->test(*p,collect) == false) )
      p++;
    if( p == parts.end() )
      return false;   // None of the parts has matched the test
    parts.erase(p);
  }
  if( di != daughters.end() ) // not enough particles
    return false;
  else if( parts.empty() )    // just the right number of particles
    return true;
  else if( elipsis )          // too many particles but we don't care
    return true;
  return false;               // too many particles
}

void MCDecayFinder::Descriptor::addDaughter( Descriptor *daughter )
{
  if( daughter->mother == NULL )
    return; // empty daughter of a {}

  if( daughter->mother->getQmark() )
  {
    daughters.push_back( daughter );
    return;
  }
  else if( daughter->mother->getExact() )
  {
    daughters.insert( daughters.begin(), daughter );
    return;
  }
  std::vector<Descriptor *>::iterator d;
  d=daughters.begin();
  while( d!=daughters.end() && (*d)->mother->getExact() )
    d++;
  daughters.insert( d, daughter );
}

void MCDecayFinder::Descriptor::addNonResonnantDaughters(
                                           std::list<const MCParticle*> &parts,
                                           const MCParticle *part )
{
  SmartRefVector<MCVertex>::const_iterator vi;
  for ( vi = part->endVertices().begin();
        vi != part->endVertices().end(); vi++ )
  {
    SmartRefVector<MCParticle>::const_iterator idau;
    for ( idau = (*vi)->products().begin();
          idau != (*vi)->products().end(); idau++ )
    {
      ParticleProperty *pp = 
        m_ppSvc->findByStdHepID( (*idau)->particleID().pid() );
      if( pp->lifetime() >= m_resThreshold )
        parts.push_front(*idau);
      else
        addNonResonnantDaughters( parts, *idau );
    }
  }
}

void MCDecayFinder::Descriptor::filterResonnances( std::list<const MCParticle*>
                                                   &parts )
{
  std::list<const MCParticle*>::iterator pi;
  std::list<const MCParticle*>::iterator npi;
  for( pi=parts.begin(); pi!=parts.end(); pi = npi )
  {
    ParticleProperty *pp = 
      m_ppSvc->findByStdHepID( (*pi)->particleID().pid() );
    if( pp->lifetime() < m_resThreshold )
    {
      const MCParticle *part = *pi;
      npi = pi;
      npi++;
      parts.erase(pi);
      addNonResonnantDaughters( parts, part ); // Daughters are prepended.
    }
    else
      npi = ++pi;
  }
}

void MCDecayFinder::Descriptor::conjugate( void )
{
  if( mother )
    mother->conjugateID();
  std::vector<Descriptor *>::iterator d;
  for( d = daughters.begin(); d != daughters.end(); d++ )
    (*d)->conjugate();
}

MCDecayFinder::ParticleMatcher::ParticleMatcher( IParticlePropertySvc *ppSvc )
  : type(notest), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{}

MCDecayFinder::ParticleMatcher::ParticleMatcher( ParticleMatcher &copy )
{
  type = copy.type;
  lift = copy.lift;
  empty_f = copy.empty_f;
  qmark = copy.qmark;
  conjugate = copy.conjugate;
  oscillate = copy.oscillate;
  noscillate = copy.noscillate;
  inverse = copy.inverse;
  stable = copy.stable;
  m_ppSvc = copy.m_ppSvc;
  switch( type )
  {
  case id:
    parms.stdHepID = copy.parms.stdHepID;
    break;
  case quark:
    parms.quarks.q1 = copy.parms.quarks.q1;
    parms.quarks.q2 = copy.parms.quarks.q2;
    parms.quarks.q3 = copy.parms.quarks.q3;
    break;
  case quantum:
    parms.relation.q = copy.parms.relation.q;
    parms.relation.r = copy.parms.relation.r;
    parms.relation.d = copy.parms.relation.d;
    break;
  case notest:
  default:
    break;
  }
}

MCDecayFinder::ParticleMatcher::ParticleMatcher( std::string *name,
                                                 IParticlePropertySvc *ppSvc )
  : type(id), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{
  ParticleProperty *pp = m_ppSvc->find(*name);
  if( pp )
    parms.stdHepID = pp->jetsetID();
  else
    throw DescriptorError(std::string("Unknown particle '")+*name+"'");
}

MCDecayFinder::ParticleMatcher::ParticleMatcher(Quarks q1, Quarks q2, Quarks q3,
                                                IParticlePropertySvc *ppSvc )
  : type(quark), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{
  parms.quarks.q1 = q1;
  parms.quarks.q2 = q2;
  parms.quarks.q3 = q3;
}

MCDecayFinder::ParticleMatcher::ParticleMatcher(Quantums q,Relations r,double d,
                                                IParticlePropertySvc *ppSvc )
  : type(quantum), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{
  parms.relation.q = q;
  parms.relation.r = r;
  parms.relation.d = d;
}

std::string MCDecayFinder::ParticleMatcher::describe( void )
{
  std::string result = "";
  if( stable )     result += '(';
  if( lift )       result += '^';
  if( qmark )      result += '?';
  if( conjugate )  result += '[';
  if( oscillate )  result += '[';
  if( noscillate ) result += '[';
  if( inverse )    result += '!';
  switch( type ) {
  case notest:
    result += "## MUST NOT COMPILE ##";
    break;
  case id:
    result += m_ppSvc->findByStdHepID(parms.stdHepID)->particle();
    break;
  case quark:
    result += "<X";
    switch( parms.quarks.q1 ) {
    case up:          result +='u';  break;
    case down:        result +='d';  break;
    case charm:       result +='c';  break;
    case strange:     result +='s';  break;
    case top:         result +='t';  break;
    case bottom:      result +='b';  break;
    case antiup:      result +="u~"; break;
    case antidown:    result +="d~"; break;
    case anticharm:   result +="c~"; break;
    case antistrange: result +="s~"; break;
    case antitop:     result +="t~"; break;
    case antibottom:  result +="b~"; break;
    case empty:                      break;
    }
    switch( parms.quarks.q2 ) {
    case up:          result +='u';  break;
    case down:        result +='d';  break;
    case charm:       result +='c';  break;
    case strange:     result +='s';  break;
    case top:         result +='t';  break;
    case bottom:      result +='b';  break;
    case antiup:      result +="u~"; break;
    case antidown:    result +="d~"; break;
    case anticharm:   result +="c~"; break;
    case antistrange: result +="s~"; break;
    case antitop:     result +="t~"; break;
    case antibottom:  result +="b~"; break;
    case empty:                      break;
    }
    switch( parms.quarks.q3 ) {
    case up:          result +='u';  break;
    case down:        result +='d';  break;
    case charm:       result +='c';  break;
    case strange:     result +='s';  break;
    case top:         result +='t';  break;
    case bottom:      result +='b';  break;
    case antiup:      result +="u~"; break;
    case antidown:    result +="d~"; break;
    case anticharm:   result +="c~"; break;
    case antistrange: result +="s~"; break;
    case antitop:     result +="t~"; break;
    case antibottom:  result +="b~"; break;
    case empty:                      break;
    }
    result += '>';
    break;
  case quantum:
    result += "## NOT IMPLEMENTED ##";
    break;
  }
  if( noscillate ) result += "]nos";
  if( oscillate )  result += "]os";
  if( conjugate )  result += "]cc";
  if( stable )     result += ')';
  return result;
}

static inline int DIGIT( int n, int id )
{
  int base = 1;
  for( ; n; n-- ) base *= 10;
  return ((abs(id)%(10*base))/base);
}

static inline int quarkOne( int id )
{
  return DIGIT(3,id);
}

static inline int quarkTwo( int id )
{
  return DIGIT(2,id);
}

static inline int quarkThree( int id )
{
  return DIGIT(1,id);
}

static inline bool isIon( int id )
{
  return( 1==DIGIT(10,id) );
}

static inline bool isHadron( int id )
{
  return abs(id)>=110;
}

static inline bool isMeson( int id )
{
  return( isHadron(id) && !isIon(id) && 0==quarkOne(id) );
}

static const int QuarksCharge[] = { 0, -1, 1, -1, 1, -1, 1 };

static int firstQuark( int id )
{
  int q;

  if( !isHadron(id) || isIon(id) )
    return 0;

  if( isMeson(id) )
  {
    q = quarkTwo(id);
    q *= QuarksCharge[q];
  }
  else
    q = quarkOne(id);

  return q*(id>0 ? 1 : -1);
}

static int secondQuark( int id )
{
  int q;

  if( !isHadron(id) || isIon(id) )
    return 0;

  if( isMeson(id) )
  {
    q = quarkThree(id);
    int r = quarkTwo(id);
    if( QuarksCharge[q] == QuarksCharge[r] )
      q *= -QuarksCharge[q];
    else
      q *= QuarksCharge[q];
  }
  else
    q = quarkTwo(id);

  return q*(id>0 ? 1 : -1);
}

static int thirdQuark( int id )
{
  int q;

  if( !isHadron(id) || isIon(id) )
    return 0;

  if( isMeson(id) )
    q = 0;
  else
    q = quarkThree(id);

  return q*(id>0 ? 1 : -1);
}

bool
MCDecayFinder::ParticleMatcher::test( const MCParticle *part,
                                      std::vector<MCParticle*> *collect )
{
  bool result = false;
  switch( type ) {
  case notest:
    result = true;
    break;
  case id:
    result = (parms.stdHepID == part->particleID().pid());
    if( conjugate ) {
      int cc_id = conjugatedID( parms.stdHepID );
      result = result || (cc_id == part->particleID().pid());
    }
    if( oscillate )  result = result && part->hasOscillated();
    if( noscillate ) result = result && (!part->hasOscillated());
    if( inverse )    result = !result;
    if( stable ) {
      int n = 0;
      SmartRefVector<MCVertex>::const_iterator vi;
      for ( vi = part->endVertices().begin();
            vi != part->endVertices().end(); vi++)
        n += (*vi)->products().size();
      result = result && (n == 0);
    }
    break;
  case quark:
    {
      static Quarks Q[] = { empty, down, up, strange, charm, bottom, top };
      static Quarks AQ[] = { empty, antidown, antiup, antistrange,
                              anticharm, antibottom, antitop };
      int q = firstQuark(part->particleID().pid());
      Quarks q1 = (q<0 ? AQ[-q] : Q[q]);
      Quarks cq1 = (q<0 ? Q[-q] : AQ[q]); // cc hypothesis
      q = secondQuark(part->particleID().pid());
      Quarks q2 = (q<0 ? AQ[-q] : Q[q]);
      Quarks cq2 = (q<0 ? Q[-q] : AQ[q]); // cc hypothesis
      q = thirdQuark(part->particleID().pid());
      Quarks q3 = (q<0 ? AQ[-q] : Q[q]);
      Quarks cq3 = (q<0 ? Q[-q] : AQ[q]); // cc hypothesis
      
      // Shortcuts
      Quarks pq1 = parms.quarks.q1;
      Quarks pq2 = parms.quarks.q2;
      Quarks pq3 = parms.quarks.q3;

      // We don't care of the ordering so we check all permutations.
      // q1, q2, q3
      if( (pq1 == q1 || pq1 == empty) && (pq2 == q2 || pq2 == empty) &&
          (pq3 == q3 || pq3 == empty) )
        result = true;
      // q1, q3, q2
      else if( (pq1 == q1 || pq1 == empty) && (pq2 == q3 || pq2 == empty) &&
               (pq3 == q2 || pq3 == empty) )
        result = true;
      // q2, q1, q3
      else if( (pq1 == q2 || pq1 == empty) && (pq2 == q1 || pq2 == empty) &&
               (pq3 == q3 || pq3 == empty) )
        result = true;
      // q2, q3, q1
      else if( (pq1 == q2 || pq1 == empty) && (pq2 == q3 || pq2 == empty) &&
               (pq3 == q1 || pq3 == empty) )
        result = true;
      // q3, q1, q2
      else if( (pq1 == q3 || pq1 == empty) && (pq2 == q1 || pq2 == empty) &&
               (pq3 == q2 || pq3 == empty) )
        result = true;
      // q3, q2, q1
      else if( (pq1 == q3 || pq1 == empty) && (pq2 == q2 || pq2 == empty) &&
               (pq3 == q1 || pq3 == empty) )
        result = true;

      // Should we check for the charge conjugated particle ?
      if( !conjugate )
        break; // No. Ok. Everything done.

      // cq1, cq2, cq3
      if( (pq1 == cq1 || pq1 == empty) && (pq2 == cq2 || pq2 == empty) &&
          (pq3 == cq3 || pq3 == empty) )
        result = true;
      // cq1, cq3, cq2
      else if( (pq1 == cq1 || pq1 == empty) && (pq2 == cq3 || pq2 == empty) &&
               (pq3 == cq2 || pq3 == empty) )
        result = true;
      // cq2, cq1, cq3
      else if( (pq1 == cq2 || pq1 == empty) && (pq2 == cq1 || pq2 == empty) &&
               (pq3 == cq3 || pq3 == empty) )
        result = true;
      // cq2, cq3, cq1
      else if( (pq1 == cq2 || pq1 == empty) && (pq2 == cq3 || pq2 == empty) &&
               (pq3 == cq1 || pq3 == empty) )
        result = true;
      // cq3, cq1, cq2
      else if( (pq1 == cq3 || pq1 == empty) && (pq2 == cq1 || pq2 == empty) &&
               (pq3 == cq2 || pq3 == empty) )
        result = true;
      // cq3, cq2, cq1
      else if( (pq1 == cq3 || pq1 == empty) && (pq2 == cq2 || pq2 == empty) &&
               (pq3 == cq1 || pq3 == empty) )
        result = true;
    }
    break;
  case quantum:
    // ******* NOT IMPLEMENTED YES *******
    return false;
  }
  if( result && lift && collect )
    collect->push_back( const_cast<MCParticle*>(part) );
  return result;
}

void MCDecayFinder::ParticleMatcher::conjugateID( void )
{
  if( type == id )
    parms.stdHepID = conjugatedID( parms.stdHepID );
  else if( type != notest )
    throw DescriptorError("Charge conjugate only allowed"
                          " on explicit particle or '?'");
}

int MCDecayFinder::ParticleMatcher::conjugatedID( int id )
{
  int cc_id = -id;
  switch( cc_id )
  { // Particles which are their own anti-particle
  case -12:     // nu_e
  case -22:     // gamma
  case -23:     // Z0
  case -25:     // H_10
  case -35:     // H_20
  case -36:     // H_30
  case -111:    // pi0
  case -113:    // rho(770)
  case -115:    // a_2(1320)0
  case -130:    // KS0
  case -221:    // eta
  case -223:    // omega(782)
  case -310:    // K0L
  case -331:    // eta'
  case -333:    // phi(1020)
  case -441:    // eta_c(1S)
  case -443:    // J/psi(1S)
  case -445:    // chi_c2(1P)
  case -10441:  // chi_c0(1P)
  case -20113:  // a_1(1260)0
  case -20443:  // chi_c1(1P)
  case -100443: // psi(2S)
  case -9000111:// a_0(980)0
    cc_id *= -1;
    break;
    //  Interesting case of CPT violation ;-)
    //  case -310:    // KS0~ <-> KL0
    //    cc_id = 130;
    //    break;
    //  case -130:    // KL0~ <-> KS0
    //    cc_id = 310;
    //    break;
  default:
    break;
  }
  return cc_id;
}
