// $Id: CondDBAccessSvc.cpp,v 1.37 2007-05-11 10:04:56 marcocle Exp $
// Include files
#include <sstream>
//#include <cstdlib>
//#include <ctime>


// needed to sleep between retrials
#include "SealBase/TimeInfo.h"
#include "SealKernel/Context.h"
#include "SealKernel/ComponentLoader.h"

#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/IReplicaSortingAlgorithm.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFolderSet.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/Record.h"

#include "CoolApplication/Application.h"
#include "CoolApplication/DatabaseSvcFactory.h"

#include "CoralBase/AttributeList.h"

// local
#include "CondDBAccessSvc.h"
#include "CondDBCache.h"

namespace 
{

  /** @class ReplicaSortAlg
   *
   * Small class implementing coral::IReplicaSortingAlgorithm interface to allow dynamic sorting of
   * database replicas obtained from LFC.
   * 
   * When retrieving the list of DB replicas, LFCReplicaService obtains a list in an arbitrary order.
   * We have to provide to CORAL a class to be used to sort the list of replicas according to our
   * needs. First we want the closest DB, identified by the environment variable LHCBPRODSITE, then
   * the CERN server (LCG.CERN.ch), while the remaining one can be in any order (this implementation
   * uses the natural string ordering).
   *
   * @author Marco Clemencic
   * @date   2007-05-02
   */
  class ReplicaSortAlg: virtual public coral::IReplicaSortingAlgorithm 
  {
    typedef coral::IDatabaseServiceDescription dbDesc_t;
    typedef std::vector< const dbDesc_t * > replicaSet_t;
    
    /** @class  ReplicaSortAlg::Comparator
     *
     * Comparison function defining which replica comes before another.
     *
     * This class is used via the STL algorithm "sort" to order the list the way we need it.
     *
     * @author Marco Clemencic
     * @date   2007-05-02
     */
    class Comparator: public std::binary_function<const dbDesc_t*,const dbDesc_t*,bool>
    {

      std::string site;

    public:
      
      /// Constructor.
      /// @param theSite the local LHCb Production Site (LCG.<i>SITE</i>.<i>country</i>)
      Comparator(const std::string &theSite): site(theSite) {}

      /// Main function
      result_type operator() (first_argument_type a, second_argument_type b) const
      {
        std::string serverA =  a->serviceParameter(a->serverNameParam());
        std::string serverB =  b->serviceParameter(b->serverNameParam());
        
        if ( serverA == serverB )       return false; // equality                   => false
        if ( serverA == site )          return true;  // "SITE" < "anything"        => true
        if ( serverB == site )          return false; // "anything" < "SITE"        => false
        if ( serverA == "LCG.CERN.ch" ) return true;  // "LCG.CERN.ch" < "anything" => true
        if ( serverB == "LCG.CERN.ch" ) return false; // "anything" < "LCG.CERN.ch" => false
        return serverA < serverB;  // for any other case use, alphabetical order
      }

    };

    std::string localSite;
    MsgStream log;

  public:

    /// Constructor.
    /// @param theSite the local LHCb Production Site (LCG.<i>SITE</i>.<i>country</i>)
    ReplicaSortAlg(const std::string &theSite, IMessageSvc *msgSvc):
      localSite(theSite),
      log(msgSvc,"ReplicaSortAlg")
    {
      log << MSG::VERBOSE << "Constructor" << endmsg;
    }
    
    /// Destructor.
    virtual ~ReplicaSortAlg()
    {
      // log << MSG::VERBOSE << "ReplicaSortAlg --> destructor" <<std::endl;
    }

    /// Main function
    virtual void 	sort (std::vector< const coral::IDatabaseServiceDescription * > &replicaSet) 
    {
      if ( log.level() <= MSG::VERBOSE ) {
        log << MSG::VERBOSE << "Original list" << endmsg;
        replicaSet_t::iterator i;
        for ( i = replicaSet.begin(); i != replicaSet.end(); ++i ) {
          log << MSG::VERBOSE << " " << (*i)->serviceParameter((*i)->serverNameParam()) << endmsg;
        }
      }
      
      log << MSG::VERBOSE << "Sorting..." << endmsg;
      std::sort(replicaSet.begin(),replicaSet.end(),Comparator(localSite));

      if ( log.level() <= MSG::VERBOSE ) {
        log << MSG::VERBOSE << "Sorted list" << endmsg;
        replicaSet_t::iterator i;
        for ( i = replicaSet.begin(); i != replicaSet.end(); ++i ) {
          log << MSG::VERBOSE << " " << (*i)->serviceParameter((*i)->serverNameParam()) << endmsg;
        }
      }
    }
    
  };
  
}

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBAccessSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBAccessSvc
//
// 2005-01-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// ==== Static data members
std::auto_ptr<cool::RecordSpecification> CondDBAccessSvc::s_XMLstorageSpec(NULL);
std::auto_ptr<cool::Application> CondDBAccessSvc::s_coolApplication(NULL);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBAccessSvc::CondDBAccessSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc),
  m_rndmSvc(0),
  m_timeOutCheckerThread(0)
{

  declareProperty("ConnectionString", m_connectionString = ""    );
  declareProperty("DefaultTAG",       m_dbTAG            = ""    );
  declareProperty("NoDB",             m_noDB             = false );
  declareProperty("UseCache",         m_useCache         = true  );
  declareProperty("CacheLowLevel",    m_cacheLL          = 10    );
  declareProperty("CacheHighLevel",   m_cacheHL          = 100   );
  //declareProperty("CachePreload",     m_cachePreload=3600*1E9); // ns
  declareProperty("CheckTAGTrials",   m_checkTagTrials   = 1     );
  declareProperty("CheckTAGTimeOut",  m_checkTagTimeOut  = 60    );
  declareProperty("ReadOnly",         m_readonly         = true  );
  declareProperty("UseLFCReplicaSvc", m_useLFCReplicaSvc = false );
  declareProperty("EnableCoralConnectionCleanUp", m_coralConnCleanUp = false );
  
  declareProperty("ConnectionTimeOut", m_connectionTimeOut = 600 );
  
  if ( s_XMLstorageSpec.get() == NULL){
    // attribute list spec template
    s_XMLstorageSpec = std::auto_ptr<cool::RecordSpecification>(new cool::RecordSpecification());
    s_XMLstorageSpec->extend("data", cool::StorageType::String16M);
  }
}
//=============================================================================
// Destructor
//=============================================================================
CondDBAccessSvc::~CondDBAccessSvc() {}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode CondDBAccessSvc::queryInterface(const InterfaceID& riid,
                                           void** ppvUnknown){
  if ( IID_ICondDBAccessSvc.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBAccessSvc*)this;
    addRef();
    return SUCCESS;
  } else if ( IID_ICondDBEditor.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBEditor*)this;
    addRef();
    return SUCCESS;
  } else if ( IID_ICondDBReader.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBReader*)this;
    addRef();
    return SUCCESS;
  } else if ( IID_ICondDBInfo.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBInfo*)this;
    addRef();
    return SUCCESS;
  }

  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBAccessSvc::initialize(){
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );

  log << MSG::DEBUG << "Initialize" << endmsg;

  if ( m_noDB && !m_useCache ) {
    log << MSG::ERROR << "Database access disabled and cache off: I cannot work like that. Ciao!" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( !m_noDB ) {
    if ( connectionString() == "" ) {
      // we need a connection string to connect to the DB
      log << MSG::ERROR << "Connection to database requested and no connection string provided." << endmsg;
      log << MSG::ERROR << "Set the option \"" << name() << ".ConnectionString\"." << endmsg;
      return StatusCode::FAILURE;
    }
    log << MSG::DEBUG << "Connection string = \"" << connectionString() << "\"" << endmsg;
    
    sc = i_openConnection();
    if (!sc.isSuccess()) return sc;
    log << MSG::INFO << "Connected to database \"" << connectionString() << "\"" << endmsg;

    // Check the existence of the provided tag.
    sc = i_checkTag();

    // Try again if requested
    int trials_to_go = m_checkTagTrials - 1; // take into account the trial just done
    while (!sc.isSuccess() && (trials_to_go > 0)){
      log << MSG::INFO << "TAG \"" << tag() << "\" not ready, I try again in " << m_checkTagTimeOut << "s. "
          << trials_to_go << " trials left." << endmsg;
      seal::TimeInfo::sleep(m_checkTagTimeOut);
      sc = i_checkTag();
      --trials_to_go;
    }

    // Fail if the tag is not found
    if (!sc.isSuccess()){
      log << MSG::ERROR << "Bad TAG given: \"" << tag() << "\" not in the database" << endmsg;
      return sc;
    }

    log << MSG::INFO << "Using TAG \"" << tag() << "\"" << endmsg;

  } 
  else {
    log << MSG::INFO << "Database not requested: I'm not trying to connect" << endmsg;
  }

  // set up cache if needed
  if (m_useCache) {
    log << MSG::DEBUG << "Initialize CondDB cache." << endmsg;
    m_cache = new CondDBCache(MsgStream(msgSvc(), name() + ".Cache"),m_cacheHL,m_cacheLL);
    if (m_cache == NULL) {
      log << MSG::ERROR << "Unable to initialize CondDB cache." << endmsg;
      return StatusCode::FAILURE;
    }
  } else {
    log << MSG::DEBUG << "CondDB cache not needed" << endmsg;
    m_cache = NULL;
  }

  // start TimeOut thread
  if (!m_noDB && m_connectionTimeOut) {
    touchLastAccess();
    TimeOutChecker tc(this);
    m_timeOutCheckerThread = std::auto_ptr<boost::thread>(new boost::thread(tc));
  }

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBAccessSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;

  // stop TimeOut thread
  if (NULL != m_timeOutCheckerThread.get()) {
    m_stop.notify_all(); // tell the thread to stop
    m_timeOutCheckerThread->join(); // wait for it
    m_timeOutCheckerThread.reset(); // delete it
  }

  // release the database
  m_db.reset();
  if (m_useCache) {
    // dump the content of the cache
    m_cache->dump();
    // dispose of the cache manager
    delete m_cache;
  }
  if ( m_rndmSvc ) m_rndmSvc->release();

  return Service::finalize();
}

//=============================================================================
// Connect to the database
//=============================================================================
StatusCode CondDBAccessSvc::i_openConnection(){
  MsgStream log(msgSvc(), name() );

  try {
    if (! m_db) { // The database is not yet opened

      if ( s_coolApplication.get() == NULL) {

        log << MSG::DEBUG << "Initializing COOL Application" << endmsg;
        s_coolApplication = std::auto_ptr<cool::Application>(new cool::Application());


        seal::Handle<seal::ComponentLoader> loader = 
          s_coolApplication->context()->component<seal::ComponentLoader>();

        log << MSG::DEBUG << "Getting CORAL Connection Service configurator" << endmsg;

        loader->load( "CORAL/Services/ConnectionService" );
        std::vector< seal::IHandle< coral::IConnectionService > > loadedServices;
        s_coolApplication->context()->query( loadedServices );
        coral::IConnectionServiceConfiguration &connSvcConf = loadedServices.front()->configuration();

        if ( m_useLFCReplicaSvc ) {

          log << MSG::INFO << "Loading CORAL LFCReplicaService" << endmsg;
          loader->load( "CORAL/Services/LFCReplicaService" );
          
          std::string theSite = System::getEnv("LHCBPRODSITE");
          if ( theSite.empty() || theSite == "UNKNOWN" ) {
            theSite = "LCG.CERN.ch";
          }

          connSvcConf.setReplicaSortingAlgorithm(new ReplicaSortAlg(theSite,msgSvc()));

        }
        
        if ( ! m_coralConnCleanUp ) {

          log << MSG::DEBUG << "Disabling CORAL connection automatic clean up" << endmsg;
          connSvcConf.disablePoolAutomaticCleanUp();
          connSvcConf.setConnectionTimeOut( 0 );

        }        
        
      }

      log << MSG::DEBUG << "Get cool::DatabaseSvc" << endmsg;
      cool::IDatabaseSvc &dbSvc = s_coolApplication->databaseService();
      log << MSG::DEBUG << "cool::DatabaseSvc got" << endmsg;

      log << MSG::DEBUG << "Opening connection" << endmsg;
      m_db = dbSvc.openDatabase(connectionString(),m_readonly);
    
    }
    else {
      log << MSG::VERBOSE << "Database connection already established!" << endmsg;
    }
    log << MSG::DEBUG << "Retrieve the root folderset." << endmsg;
    m_rootFolderSet = m_db->getFolderSet("/");
  }
  //  catch ( cool::DatabaseDoesNotExist &e ) {
  catch ( cool::Exception &e ) {
    log << MSG::ERROR << "Problems opening database" << endmsg;
    log << MSG::ERROR << e.what() << endmsg;
    m_db.reset();
    return StatusCode::FAILURE;
  }

  touchLastAccess();
  return StatusCode::SUCCESS;
}

//=============================================================================
// TAG handling
//=============================================================================
const std::string &CondDBAccessSvc::tag() const { return m_dbTAG; }
StatusCode CondDBAccessSvc::setTag(const std::string &_tag){

  if (m_dbTAG == _tag) return StatusCode::SUCCESS; // no need to change

  StatusCode sc = i_checkTag(_tag);
  if ( sc.isSuccess() ) {
    m_dbTAG = _tag;
    // the cache must be cleared if the tag is changed
    clearCache();
    MsgStream log(msgSvc(), name() );
    log << MSG::WARNING << "TAG changed to \"" << _tag << "\"" << endmsg;
  } else {
    MsgStream log(msgSvc(), name() );
    log << MSG::WARNING << "Unable to set TAG \"" << _tag
        << "\": not in the DB. (Still using \"" << tag() << "\")" << endmsg;
  }
  return sc;
}
StatusCode CondDBAccessSvc::i_checkTag(const std::string &tag) const {

  MsgStream log(msgSvc(), name() );
  if ( !m_db ) {
    log << MSG::ERROR << "Check tag \"" << tag
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);

  log << MSG::VERBOSE << "Check availability of tag \"" << tag << "\"" << endmsg;
  if (m_rootFolderSet) {
    // HEAD tags are always good
    //if ( (tag.empty()) || (tag == "HEAD") ) return StatusCode::SUCCESS;
    if ( m_rootFolderSet->isHeadTag(tag) ) {
      log << MSG::VERBOSE << "\"" << tag << "\" is a HEAD tag: OK" << endmsg;
      return StatusCode::SUCCESS;
    }
    // try to resolve the tag (it cannot be checked)
    try {
      m_rootFolderSet->resolveTag(tag);
      log << MSG::VERBOSE << "\"" << tag << "\" found: OK" << endmsg;
      return StatusCode::SUCCESS;
    } catch (cool::TagNotFound) {
      log << MSG::VERBOSE << "\"" << tag << "\" NOT found" << endmsg;
      return StatusCode::FAILURE;
    }
    catch (cool::TagRelationNotFound &e) {
      log << MSG::ERROR << "got a cool::TagRelationNotFound : " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
    catch (std::exception &e) {
      log << MSG::ERROR << "got a std::exception : " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }

  }
  return StatusCode::FAILURE;
}


//=============================================================================
// Return the connection string used to connect to the database.
//=============================================================================
const std::string &CondDBAccessSvc::connectionString() const{
  return m_connectionString;
}

//=============================================================================
// Utilities
//=============================================================================
StatusCode CondDBAccessSvc::createNode(const std::string &path,
                                       const std::string &descr,
                                       StorageType storage,
                                       VersionMode vers) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot create node in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);
  try {
    switch (storage) {
    case FOLDERSET:
      m_db->createFolderSet(path,descr,true);
      break;
    case XML:
      {
        cool::FolderSpecification spec((vers == SINGLE)
                                       ?cool::FolderVersioning::SINGLE_VERSION
                                       :cool::FolderVersioning::MULTI_VERSION,
                                       *s_XMLstorageSpec);
        
        // append to the description the storage type
        std::ostringstream _descr;
        _descr << descr << " <storage_type=" << std::dec << XML_StorageType << ">";
        m_db->createFolder(path,
                           spec,
                           _descr.str(),
                           true);
      }
      break;
    default:
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Unable to create the folder \"" << path
          << "\": unknown StorageType" << endmsg;
      return StatusCode::FAILURE;
    }
  } catch(cool::NodeExists){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the node already exists" << endmsg;
    return StatusCode::FAILURE;
  } catch(cool::Exception &e){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::createNode(const std::string &path,
                                       const std::string &descr,
                                       const std::set<std::string> &fields,
                                       StorageType storage,
                                       VersionMode vers) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot create node in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    switch (storage) {
    case FOLDERSET:
      m_db->createFolderSet(path,descr,true);
      break;
    case XML:
      {
        cool::RecordSpecification recSpec;
        for (std::set<std::string>::const_iterator f = fields.begin(); f != fields.end(); ++f ){
          recSpec.extend(*f, cool::StorageType::String16M);
        }
        cool::FolderSpecification spec((vers == SINGLE)
                                       ?cool::FolderVersioning::SINGLE_VERSION
                                       :cool::FolderVersioning::MULTI_VERSION,
                                       recSpec);

        // append to the description the storage type
        std::ostringstream _descr;
        _descr << descr << " <storage_type=" << std::dec << XML_StorageType << ">";
        m_db->createFolder(path,
                           spec,
                           _descr.str(),
                           true);
      }
      break;
    default:
      MsgStream log(msgSvc(), name() );
      log << MSG::ERROR << "Unable to create the folder \"" << path
          << "\": unknown StorageType" << endmsg;
      return StatusCode::FAILURE;
    }
  } catch(cool::NodeExists){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\": the node already exists" << endmsg;
    return StatusCode::FAILURE;
  } catch(cool::Exception &e){
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to create the folder \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::storeXMLData(const std::string &path, const std::string &data,
                                         const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot store in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the object \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);
  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_db->getFolder(path);
    cool::Record payload(folder->payloadSpecification());
    payload["data"].setValue<cool::String16M>(data);
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound) {

    MsgStream log(msgSvc(), name() );
    if (m_db->existsFolderSet(path))
      log << MSG::ERROR << "Trying to store data into the non-leaf folder \"" <<
        path << '\"' << endmsg;
    else
      log << MSG::ERROR << "Cannot find folder \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;

  } catch (cool::Exception &e){

    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the XML string into \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;

  }
  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::storeXMLData(const std::string &path, const std::map<std::string,std::string> &data,
                                         const Gaudi::Time &since, const Gaudi::Time &until, cool::ChannelId channel) const {
  if ( m_readonly ) {
    MsgStream log(msgSvc(), name() );
    log << "Cannot store in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the object \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try {
    // retrieve folder pointer
    cool::IFolderPtr folder = m_db->getFolder(path);
    cool::Record payload(folder->payloadSpecification());
    for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
      payload[d->first].setValue<cool::String16M>(d->second);
    }
    
    folder->storeObject(timeToValKey(since),timeToValKey(until),payload,channel);

  } catch (cool::FolderNotFound) {

    MsgStream log(msgSvc(), name() );
    if (m_db->existsFolderSet(path))
      log << MSG::ERROR << "Trying to store data into the non-leaf folder \"" <<
        path << '\"' << endmsg;
    else
      log << MSG::ERROR << "Cannot find folder \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;

  } catch (cool::Exception &e){

    MsgStream log(msgSvc(), name() );
    log << MSG::ERROR << "Unable to store the XML string into \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;

  }
  return StatusCode::SUCCESS;
}

cool::ValidityKey CondDBAccessSvc::timeToValKey(const Gaudi::Time &time) const {
  // ValidityKey is an uInt64 of which only 63 bits used (0 -> 9223372036854775807),
  // while time.nd() is a positive signed Int64! (the same thing)
  return time.ns();
}

Gaudi::Time CondDBAccessSvc::valKeyToTime(const cool::ValidityKey &key) const {
  return Gaudi::Time(key);
}

StatusCode CondDBAccessSvc::tagLeafNode(const std::string &path, const std::string &tagName,
                                        const std::string &description) {
  MsgStream log(msgSvc(),name());

  if ( m_readonly ) {
    log << "Cannot tag in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    log << MSG::ERROR << "Unable to tag the leaf node \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  DataBaseOperationLock dbLock(this);
  try {
    log << MSG::DEBUG << "entering tagLeafNode: \"" << path << '"' << endmsg;

    cool::IFolderPtr folder = m_db->getFolder(path);
    if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION){
      log << MSG::WARNING << "Not tagging leaf node \"" << path << "\": single-version" << endmsg;
    } else {
      log << MSG::DEBUG << "Tagging leaf node \"" << path << "\": " << tagName << endmsg;
      folder->tagCurrentHead(tagName, description);
    }

  } catch (cool::FolderNotFound) {

    if (m_db->existsFolderSet(path))
      log << MSG::ERROR << "Node \"" << path << "\" is not leaf." << endmsg;
    else
      log << MSG::ERROR << "Cannot find node \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;

  } catch (cool::Exception &e){

    log << MSG::ERROR << "Unable tag leaf node \"" << path
        << "\" (cool::Exception): " << e.what() << endmsg;
    return StatusCode::FAILURE;

  }

  return StatusCode::SUCCESS;
}

std::string CondDBAccessSvc::generateUniqueTagName(const std::string &base,
                                                   const std::set<std::string> &reserved) const {

  if ( !m_db->isOpen() ) {
    throw GaudiException("Database not open","CondDBAccessSvc::generateUniqueTagName",StatusCode::FAILURE);
  }
  if ( ! m_rndmSvc ) {
    IRndmGenSvc *svc;
    StatusCode sc = service("RndmGenSvc",svc,true);
    const_cast<CondDBAccessSvc*>(this)->m_rndmSvc = svc;
    if ( ! sc.isSuccess() ) {
      throw GaudiException("Cannot get a pointer to RndmGenSvc","CondDBAccessSvc::generateUniqueTagName",sc);
    }
  }
  
  std::string tag = "";
  do {
    // start with the signature
    tag = "_auto_";
    // add the base, if any
    if (!base.empty()) tag += base + "-";
    // append 6 randomly chosen chars in set [0-9A-Za-z]
    for ( int i = 0; i<6; ++i ) {
      char c=(char) ( 62.0 * m_rndmSvc->engine()->rndm() );
      if ( c > 61 ) GaudiException("Generator failure","CondDBAccessSvc::generateUniqueTagName",StatusCode::FAILURE); // c %= 62;
      if ( c >= 36 ) tag += c + 61;
      else if ( c >= 10 ) tag += c + 55;
      else tag += c + 48;
    }
    // check if the random name already exists or is reserved
  } while ( m_db->existsTag(tag) || (reserved.find(tag) != reserved.end()) );

  return tag;
}


StatusCode CondDBAccessSvc::recursiveTag(const std::string &path, const std::string &tagName,
                                         const std::string &description) {
  std::set<std::string> reserved;
  DataBaseOperationLock dbLock(this);
  return i_recursiveTag(path,tagName,description,tagName,reserved);
}

StatusCode CondDBAccessSvc::i_recursiveTag(const std::string &path, const std::string &base,
                                           const std::string &description,
                                           const std::string &tagName,
                                           std::set<std::string> &reserved) {
  MsgStream log(msgSvc(),name());

  if ( m_readonly ) {
    log << MSG::ERROR << "Cannot tag in read-only mode" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( !m_db ) {
    log << MSG::ERROR << "Unable to tag the inner node \"" << path
        << "\": the database is not opened!" << endmsg;
    return StatusCode::FAILURE;
  }

  try {
    // start reserving the tag name we want to apply to the current folderset
    reserved.insert(tagName);

    // get the list of child nodes (both types)
    cool::IFolderSetPtr this_folderset = m_db->getFolderSet(path);    
    std::vector<std::string> folders = this_folderset->listFolders();
    std::vector<std::string> foldersets = this_folderset->listFolderSets();

    // loop over leaf nodes and apply the tags
    std::vector<std::string>::iterator f;
    for ( f = folders.begin(); f != folders.end(); ++f ) {

      std::string auto_tag = generateUniqueTagName(base,reserved);
      cool::IFolderPtr child_folder = m_db->getFolder(*f);

      if (child_folder->versioningMode() == cool::FolderVersioning::MULTI_VERSION) {
        // only multi-version folders can be tagged
        child_folder->tagCurrentHead(auto_tag, description);
        // associate the child folder tag with the parent one
        child_folder->createTagRelation(tagName, auto_tag);
      }

    }

    // loop over inner nodes and recurse
    for ( f = foldersets.begin(); f != foldersets.end(); ++f ) {

      std::string auto_tag = generateUniqueTagName(base,reserved);
      StatusCode sc = i_recursiveTag(*f,base,description,auto_tag,reserved);
      if (!sc.isSuccess()) return sc;

      cool::IFolderSetPtr child_folderset = m_db->getFolderSet(*f);
      child_folderset->createTagRelation(tagName, auto_tag);

    }
  }
  catch (cool::FolderSetNotFound) {
    if (m_db->existsFolder(path))
      log << MSG::ERROR << "Node \"" << path << "\" is a leaf." << endmsg;
    else
      log << MSG::ERROR << "Cannot find node \"" << path << '\"' << endmsg;
    return StatusCode::FAILURE;
  }
  catch (cool::Exception &e) {
    log << MSG::ERROR << "Problems tagging" << endmsg;
    log << MSG::ERROR << e.what() << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CondDBAccessSvc::getObject(const std::string &path, const Gaudi::Time &when,
                                      DataPtr &data,
                                      std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel){

  try {
    if (m_useCache) {
      cool::ValidityKey vk_when = timeToValKey(when);
      cool::ValidityKey vk_since, vk_until;
      if (!m_cache->get(path,vk_when,channel,vk_since,vk_until,descr,data)) {
        // not found
        if (!m_noDB) {
          DataBaseOperationLock dbLock(this);
          if (database()->existsFolderSet(path)) {
            // with FolderSets, I put an empty entry and clear the shared_ptr
            m_cache->addFolderSet(path,"");
            data.reset();
            return StatusCode::SUCCESS;
          }
          // go to the database
          cool::IFolderPtr folder = database()->getFolder(path);
          cool::IObjectPtr obj;
          if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
              || tag().empty() || tag() == "HEAD" ){
            obj = folder->findObject(vk_when,channel);
          } else {
            obj = folder->findObject(vk_when,channel,folder->resolveTag(tag()));
          }
          m_cache->insert(folder,obj,channel);
          // now the object is in the cache
          m_cache->get(path,vk_when,channel,vk_since,vk_until,descr,data);
        } else {
          // we are not using the db: no way of getting the object from it
          return StatusCode::FAILURE;
        }
      }
      since = valKeyToTime(vk_since);
      until = valKeyToTime(vk_until);
    } else if (!m_noDB){
      
      DataBaseOperationLock dbLock(this);
      if (database()->existsFolderSet(path)) {
        // with FolderSets, I clear the shared_ptr (it's the folderset signature)
        data.reset();
        return StatusCode::SUCCESS;
      }

      cool::IFolderPtr folder = database()->getFolder(path);
      descr = folder->description();

      cool::IObjectPtr obj;
      if (folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION
          || tag().empty() || tag() == "HEAD"){
        obj = folder->findObject(timeToValKey(when),channel);
      } else {
        obj = folder->findObject(timeToValKey(when),channel,folder->resolveTag(tag()));
      }

      // deep copy of the attr. list
      data = DataPtr(new cool::Record(obj->payload()));

      since = valKeyToTime(obj->since());
      until = valKeyToTime(obj->until());

    } else {
      //log << MSG::ERROR << "Object not found in cache and database is off" << endmsg;
      return StatusCode::FAILURE;
    }

  } catch ( cool::FolderNotFound /*&e*/) {
    //log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  } catch (cool::ObjectNotFound /*&e*/) {
    //log << MSG::ERROR << "Object not found in \"" << path <<
    //  "\" for tag \"" << (*accSvc)->tag() << "\" ("<< now << ')' << endmsg;
    //log << MSG::DEBUG << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names) {

  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering \"getChildNodes\"" << endmsg;

  node_names.clear();

  try {

    if (!m_noDB) { // If I have the DB I always use it!
      DataBaseOperationLock dbLock(this);
      if (database()->existsFolderSet(path)) {
        log << MSG::DEBUG << "FolderSet \"" << path  << "\" exists" << endmsg;
        
        cool::IFolderSetPtr folderSet = database()->getFolderSet(path);

        std::vector<std::string> fldr_names = folderSet->listFolders();
        std::vector<std::string> fldrset_names = folderSet->listFolderSets();

        for ( std::vector<std::string>::iterator f = fldr_names.begin(); f != fldr_names.end(); ++f ) {
          log << MSG::DEBUG << *f << endmsg;
          node_names.push_back(f->substr(f->rfind('/')));
        }
        for ( std::vector<std::string>::iterator f = fldrset_names.begin(); f != fldrset_names.end(); ++f ) {
          log << MSG::DEBUG << *f << endmsg;
          node_names.push_back(f->substr(f->rfind('/')));
        }

        log << MSG::DEBUG << "got " << node_names.size() << " sub folders" << endmsg;
      } else {
        // cannot get the sub-nodes of a folder!
        return StatusCode::FAILURE;
      }
    } else if (m_useCache) {
      // if no db, but cache, let's assume we know everything is in there
      m_cache->getSubNodes(path,node_names);
    } else {
      // no cache and no db
      return StatusCode::FAILURE;
    }
  } catch ( cool::FolderNotFound /*&e*/) {
    //log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

//=========================================================================
// Add database name and TAG to the passed vector.
//=========================================================================
void CondDBAccessSvc::defaultTags ( std::vector<LHCb::CondDBNameTagPair>& tags ) const {
  /// @todo This shold be something like
  /// <quote>
  /// tags.push_back(LHCb::CondDBNameTagPair(database()->dbName(),tag()));
  /// </quote>
  /// but COOL API does not provide that function yet.

  std::string dbName;
  // Parsing of COOL connection string to find database name
  // - first type: <tech>://<server>;schema=<schema>;dbname=<dbname>
  std::string::size_type pos = connectionString().find("dbname=");
  if ( std::string::npos != pos ) {
    pos += 7;
    std::string::size_type pos2 = connectionString().find(';',pos);
    if ( std::string::npos != pos2 )
      dbName = connectionString().substr(pos,pos2-pos);
    else
      dbName = connectionString().substr(pos);
  } else {
    // - second type: <alias>/<dbname>
    pos = connectionString().find_last_of('/');
    if ( std::string::npos != pos ) {
      dbName = connectionString().substr(pos+1);
    } else {
      throw GaudiException("Cannot understand COOL connection string",
                           "CondDBAccessSvc::defaultTags",StatusCode::FAILURE);
    }
  }
  // If the tag is a "HEAD" tag, I want to show "HEAD"
  std::string tagName = tag();
  if (m_rootFolderSet->isHeadTag(tagName)) {
    tagName = "HEAD";
  }

  tags.push_back(LHCb::CondDBNameTagPair(dbName,tagName));
  
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddFolder(const std::string &path, const std::string &descr,
                                           const cool::IRecordSpecification& spec) {
  if (!m_useCache) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cache not in use: I cannot add a folder to it." << endmsg;
    return StatusCode::FAILURE;
  }
  return m_cache->addFolder(path,descr,spec) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddFolderSet(const std::string &path, const std::string &descr) {
  if (!m_useCache) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cache not in use: I cannot add a folder-set to it." << endmsg;
    return StatusCode::FAILURE;
  }
  return m_cache->addFolderSet(path,descr) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLFolder(const std::string &path) {
  std::ostringstream _descr;
  _descr << " <storage_type=" << std::dec << XML_StorageType << ">";
  return cacheAddFolder(path,_descr.str(),*s_XMLstorageSpec);
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLFolder(const std::string &path, const std::set<std::string> &fields) {
  std::ostringstream _descr;
  _descr << " <storage_type=" << std::dec << XML_StorageType << ">";
  cool::RecordSpecification spec;
  for (std::set<std::string>::const_iterator f = fields.begin(); f != fields.end(); ++f ){
    spec.extend(*f, cool::StorageType::String16M);
  }
  return cacheAddFolder(path,_descr.str(),spec);
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddObject(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                           const cool::IRecord& payload, cool::ChannelId channel) {
  if (!m_useCache) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cache not in use: I cannot add an object to it." << endmsg;
    return StatusCode::FAILURE;
  }
  return m_cache->addObject(path,timeToValKey(since),timeToValKey(until),payload,channel)
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                            const std::string &data, cool::ChannelId channel) {
  /// @todo this is affected by the evolution in COOL API
  cool::Record payload(*s_XMLstorageSpec);
  payload["data"].setValue<cool::String16M>(data);
  return cacheAddObject(path,since,until,payload,channel);
}

//=========================================================================
//
//=========================================================================
StatusCode CondDBAccessSvc::cacheAddXMLData(const std::string &path, const Gaudi::Time &since, const Gaudi::Time &until,
                                            const std::map<std::string,std::string> &data, cool::ChannelId channel) {
  cool::RecordSpecification spec;
  for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
    spec.extend(d->first,cool::StorageType::String16M);
  }
  
  cool::Record payload(spec);

  for (std::map<std::string,std::string>::const_iterator d = data.begin(); d != data.end(); ++d ){
    payload[d->first].setValue<cool::String16M>(d->second);
  }
  return cacheAddObject(path,since,until,payload,channel);
}

//=========================================================================
//
//=========================================================================
void CondDBAccessSvc::clearCache()
{
  if (m_useCache) {
    m_cache->clear();
  }
}

//=========================================================================
//
//=========================================================================
void CondDBAccessSvc::dumpCache() const {
  if (m_useCache) m_cache->dump();
}
