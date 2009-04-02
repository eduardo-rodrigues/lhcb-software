//  ====================================================================
//  MEPInjector.cpp
//  --------------------------------------------------------------------
//
//  Author    : David Svantesson
//              Jean-Christophe Garnier 
//
//  Description : 
//  
//  This file contains the implementation of the MEP injector. It consists 
//  in a GAUDI service which reads raw events from buffer managers, produces
//  Tell1's MEPs, and then it sends them to the HLT. The buffer managers are feeded 
//  by slice reader services.
//  Each slice reader used with the injector is there to provide different kind of events.
//  At least one slice reader is compulsory to provide events, other are facultatives.
//
//  XXX Right now only one reader for plain events and one reader for lumi events can be used
//  in a very unelegant way, but more tests about reader/injector communication have to be performed 
//  before to start thinking about it
//
//  It implements also a MEP request manager and an Odin MEP manager in order
//  to be integrated in the DAQ network.
//
//  Each task is carried out by a dedicated thread.
//
//  Be sure to allocate buffer manager large enough, else events cannot be stored completely 
//  and then they are truncated and the hlt will complain about incomplete event. 
//
//  ====================================================================

#ifndef _WIN32

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/MEPRxSys.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiOnline/MEPRxSvc.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "GaudiKernel/Stat.h"

#include "GaudiOnline/MEPInjector.h"
#include "MBM/Requirement.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/poll.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <math.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>



#define ERRMSG(a,x) do {  \
  (a) << MSG::ERROR << (x) << " " << " in " << __PRETTY_FUNCTION__<< ":"  << __FILE__<< ":(" << __LINE__ << ")" << endmsg;} while(0);

#define PUBCNT(name, desc) do {m_ ## name = 0; m_MonSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_MonSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size() * sizeof(int), desc, this);} while(0);


//XXX
static std::map<int, std::map<int, int> > BKSTATMAP;  
static int BKSTATI=0;
void saveHisto();

using MBM::Requirement;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPInjector)
using namespace LHCb;

#define MSGTAG 			0x1234abcd
#define MEPEVENTOFFSET		 4
#define IPHDRSZ			20
#define MEPHDRSZ	        12
#define ODFRAGSZ		52 /// Odin Fragment size, with Frag Header and only 1 bank
#define BKHDRSZ			 8
#define FRAGHDRSZ		 4
#define MDFHDRSZ 48 //12 word of 32 bits, or 4 bytes

static bool finalSent=false;

typedef union {
    u_int32_t word;
    struct {
	u_int16_t evtid;
	u_int16_t len;
    } part;
} EventHdr_t;

/// Streambuffer manages correctly the memory, reallocating if need more than it currently has, and freeing once the streambuffer is destroyed, i.e. in the end of the program because the streambuffer is an attribute of the MEPInjector
/// This function also sets the real size of the MEP in the first 4 bytes of the buffer, but the MEPEvent constructor of encodeMEP function sets it to 0 ... before to set it back to the same value
static void *extendBuffer(void *p, size_t len)
{
    StreamBuffer *s = (StreamBuffer *) p;
    s->reserve(len);
    unsigned int *pui = (unsigned int *) s->data();
    *pui = len;
    return s->data();
}




/** Extended service constructor
*/
MEPInjector::MEPInjector(const std::string & name, ISvcLocator * pSvcLocator):Service(name, pSvcLocator), m_IncidentSvc(0), m_MonSvc(0) 
{
    m_ProcName = RTL::processName(); 

    declareProperty("PackingFactor", m_PackingFactor = 10);	 


    declareProperty("MEPProto", m_MEPProto = 0xF2);
    declareProperty("MEPReqProto", m_MEPReqProto = 0xFD);


    declareProperty("HLTIPAddrTo", m_HLTStrIPAddrTo = "0.0.0.0");

    declareProperty("MEPSockBuf", m_MEPBufSize = 0x100000);
    declareProperty("MEPReqSockBuf", m_MEPReqBufSize = 0x10000);

    declareProperty("L0ID", m_L0ID = 0x00000000);
    declareProperty("AutoMode", m_AutoMode = true);

    declareProperty("OdinIf", m_OdinIfIPAddr = "0.0.0.0"); 
    declareProperty("OdinIPAddr", m_StrOdinIPAddr = "0.0.0.0");
    declareProperty("HLTIf", m_HLTIfIPAddr = "0.0.0.0");
    
    declareProperty("OdinEthInterface", m_OdinEthInterface=m_OdinIfIPAddr[m_OdinIfIPAddr.length()]);
    declareProperty("HLTEthInterface", m_HLTEthInterface=m_HLTIfIPAddr[m_HLTIfIPAddr.length()]);
    

    declareProperty("TimeOut", m_TimeOut = 10000);

    /// Partition ID set to all detectors by default, but copied from Odin in TFC mode
    declareProperty("PartitionID", m_PartitionID = 0xffffffff);

    declareProperty("Tell1Boards", m_Tell1Boards);
    declareProperty("HLTNodes", m_HLTNodes);

    declareProperty("ARPME", m_ARPMe=false);

    declareProperty("Offline", m_Offline=true);

    declareProperty("Debug", m_Debug = false); 
 
//    declareProperty("EventBufferFlags", m_EventBufferFlags);

    declareProperty("BufferNames", m_BufferNames);
    declareProperty("BufferOptions", m_BufferOptions);

    declareProperty("Requirements", m_Req);

    declareProperty("TFCMask", m_TFCMask = 0x0000ffff);
    declareProperty("TapeMask", m_TapeMask = 0xffff0000);

    m_InjState = NOT_READY;
}

/** IInterface implementation : queryInterface
 */
StatusCode MEPInjector::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IRunable::interfaceID().versionMatch(riid) )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppIf);
}


/** Buffer manager initialization
 * Uses attributes BufferNames and BufferOptions
 * Default options: -s=70000, -e=100, -u=20 
 * No default buffer name: the run control should define at least one
 */
StatusCode MEPInjector::initBuffers()
{
    MsgStream msgLog(msgSvc(), name());

    std::vector<char *> items;
//    items.push_back(strcpy(new char[16], "mbm_install"));
    static char type[64]="mbm_install";
    items.push_back(type);
    if(m_BufferOptions.size() < 3) 
    {
        msgLog << MSG::WARNING << "Invalid buffer options, the injector will use default values" << endmsg;
        items.push_back(strcpy(new char[10], "-s=70000"));
        items.push_back(strcpy(new char[10], "-e=100"));
        items.push_back(strcpy(new char[10], "-u=20"));
    }
    else
    {
        for(std::vector<std::string>::iterator ite = m_BufferOptions.begin(); ite != m_BufferOptions.end(); ++ite) 
            items.push_back(strcpy(new char[ite->size()], ite->c_str()));
    }

    ///Get a point to the last string of the buffer manager options.
    ///It will contain the buffer name
    items.push_back(new char[64]);
    char **pName = &(items.back());
    items.push_back(strcpy(new char[3], "-c")); 
    

    /// Attach to the buffers
    for(std::vector<std::string>::iterator iteNames = m_BufferNames.begin(); iteNames != m_BufferNames.end(); ++iteNames)
    {
        std::string tmp = "-i="+*iteNames;
        strcpy(*pName, tmp.c_str());

        int sc = mbm_install(items.size(), &items[0]);
        if ( sc != MBM_NORMAL )  {
           ::lib_rtl_output(LIB_RTL_ERROR,"Unable to install MBM buffers...\n");
           return StatusCode::FAILURE;
        }
    
        msgLog << MSG::DEBUG << "Buffer " << *iteNames << " installed" << endmsg;
        
        BMID bmid= mbm_include(iteNames->c_str(), m_ProcName.c_str(), m_PartitionID); 
        if(bmid==MBM_INV_DESC)
        {
            msgLog << MSG::ERROR << "Unable to attach to buffer manager "<< *iteNames << " as " << m_ProcName << endmsg;
            return StatusCode::FAILURE;
        }
        msgLog << MSG::DEBUG << "Buffer " << *iteNames << " included" << endmsg;
        m_EventBuffers[*iteNames]=bmid;  
    }    

    for(std::vector<char *>::iterator ite=items.begin()+1; ite!=items.end(); ++ite) {
        delete [] *ite;
    }
    items.clear(); 

    msgLog << MSG::INFO << "Buffer initialized" << endmsg;

    return StatusCode::SUCCESS;
}

/** Initialization of the buffer consumers
 * 
 */
StatusCode MEPInjector::initConsumers()
{
    static MsgStream msgLog(msgSvc(), name());	//Message stream for output
    for(std::map<std::string, BMID>::iterator ite=m_EventBuffers.begin(); ite != m_EventBuffers.end(); ++ite)
    {
        //use the requirem,etns to compute the probability of choosing buffers?
        for(Requirements::iterator i=m_Req.begin(); i!=m_Req.end(); ++i)  {
            Requirement r;
            r.parse(*i);
            //pcons->addRequest(r);
            if(MBM_NORMAL != mbm_add_req(ite->second, r.evtype, r.trmask, r.vetomask, r.maskType, r.userType, r.freqType, r.freq))
            {
                msgLog << MSG::ERROR << "Unable to add a request to the queue " << ite->first << endmsg;
                return StatusCode::FAILURE; 
            }  
        }
    }
    return StatusCode::SUCCESS; 
}

/** Destruction of the buffer consumers
 * 
 */
StatusCode MEPInjector::deleteConsumers()
{
    static MsgStream msgLog(msgSvc(), name());	//Message stream for output
    for(std::map<std::string, BMID>::iterator ite = m_EventBuffers.begin(); ite != m_EventBuffers.end(); ++ite)
    {   
        mbm_cancel_request(ite->second);

        for(Requirements::iterator i=m_Req.begin(); i!=m_Req.end(); ++i)  {
            Requirement r;
            r.parse(*i);
            if(MBM_NORMAL != mbm_del_req(ite->second, r.evtype, r.trmask, r.vetomask, r.maskType, r.userType))
            {
                msgLog << MSG::WARNING << "Problem deleting request, continue" << endmsg;
            }
        } 
        mbm_free_space(ite->second);
        mbm_exclude(ite->second);
    }

    /// XXX
    /// deinstall buffers, could it have been easier ?
    /// basically do the same processing as for initialization ...
    std::vector<char *> items;
    static char type[64]="mbm_install";
    items.push_back(type);
    if(m_BufferOptions.size() < 3)
    {   
        msgLog << MSG::WARNING << "Invalid buffer options, the injector will use default values" << endmsg;
        items.push_back(strcpy(new char[10], "-s=70000"));
        items.push_back(strcpy(new char[10], "-e=100"));
        items.push_back(strcpy(new char[10], "-u=20"));
    }
    else
    {   
        for(std::vector<std::string>::iterator ite = m_BufferOptions.begin(); ite != m_BufferOptions.end(); ++ite)
            items.push_back(strcpy(new char[ite->size()], ite->c_str()));
    }

    items.push_back(new char[64]);
    char **pName = &(items.back());
    items.push_back(strcpy(new char[3], "-c"));

    for(std::vector<std::string>::iterator iteNames = m_BufferNames.begin(); iteNames != m_BufferNames.end(); ++iteNames)
    {
        std::string tmp = "-i="+*iteNames;
        strcpy(*pName, tmp.c_str());

        int sc = mbm_deinstall(items.size(), &items[0]);
        if ( sc != MBM_NORMAL )  {
           ::lib_rtl_output(LIB_RTL_ERROR,"Unable to install MBM buffers...\n");
           return StatusCode::FAILURE;
        }

        msgLog << MSG::DEBUG << "Buffer " << *iteNames << " deinstalled" << endmsg;
    }

    m_EventBuffers.clear();

    return StatusCode::SUCCESS;
}

/** Extended service initialization
 * Run threads, declare everything, ...
 */
StatusCode MEPInjector::initialize()
{
    StatusCode sc = Service::initialize();
    static MsgStream msgLog(msgSvc(), name());	//Message stream for output
    msgLog << MSG::ALWAYS << __PRETTY_FUNCTION__ << endmsg;
    if (!sc.isSuccess())
    {
        msgLog << MSG::ERROR << "Failed to initialize service base class." << endmsg;
        return sc;
    }

    if(!service("IncidentSvc", m_IncidentSvc).isSuccess()) {
        ERRMSG(msgLog, "Failed to access to incident service.");
        return StatusCode::FAILURE;
    }
/*
    m_IncidentSvc->addListener(this, "DAQ_CANCEL", 0, true, true);
    m_IncidentSvc->addListener(this, "DAQ_ENABLE", 0, true, true);
*/

    if(!service("MonitorSvc", m_MonSvc).isSuccess()) {
        ERRMSG(msgLog, "Failed to access to monitor service.");
        return StatusCode::FAILURE;
    }
    else {
      if (setupCounters()) {
        return error("Failed to setup counters");
      }
    }


    sc = initBuffers();
    if(sc.isFailure())
    {
        msgLog << MSG::ERROR << "Failed to make buffers" << endmsg;
        return sc; 
    }


    sc = initConsumers();
    if(sc.isFailure())
    {   
        msgLog << MSG::ERROR << "Failed to instanciate consumers" << endmsg;
        return sc;
    }

    m_gotOdin = false;
    m_gotHLT = false; 
 

    std::string strAllocatedMEPs = "";

    for(std::vector<std::string>::iterator ite=m_Tell1Boards.begin(); ite != m_Tell1Boards.end(); ite+=3) 
    {
        if(!m_AutoMode && strncmp((ite+1)->c_str(), "tfc", 3)==0) {
            msgLog << MSG::INFO << "No MEP for " << *ite << endmsg;
            continue; 
        }
        u_int32_t tell1id = MEPRxSys::IPStringToBits(*ite);
        /// Seems to be managed correctly alone if it needs to be increased        
        /// Cause a tell1 can send different bank types (XXX check with niko), impossible to compute the length of the MEP from the length of the first bank found, 5000 seems enough to contain the biggest banks and some others  
        void *memory = extendBuffer(&(m_MapStreamBuffers[tell1id]), m_PackingFactor * 5000 );
        bzero(memory, m_PackingFactor * 5000);
        m_MapTell1MEPs[tell1id] = new(memory) MEPEvent(0);
        m_MapTell1MEPs[tell1id]->setSize(0);   
        
        strAllocatedMEPs = strAllocatedMEPs + MEPRxSys::dotted_addr(tell1id) + "; "; 
    }

    msgLog << MSG::INFO << "Allocated MEPs : "<< endmsg;
    msgLog << MSG::INFO << strAllocatedMEPs << endmsg;
    msgLog << MSG::INFO << "Number : " << m_MapTell1MEPs.size() << endmsg; 
   

    m_OdinBufSize = m_MEPBufSize;
 
    m_HLTReqsIte = m_HLTReqs.begin();

    m_OdinEthInterface = m_OdinIfIPAddr[m_OdinIfIPAddr.length() -1] -48;
    m_HLTEthInterface = m_HLTIfIPAddr[m_HLTIfIPAddr.length() -1] -48;
    

    /// Try to trap all signals, parent process should if child process wants
/*
    sigset_t injSet;
    sigfillset(&injSet); 
    sigdelset(&injSet, SIGILL);
    sigdelset(&injSet, SIGFPE);
    sigdelset(&injSet, SIGSEGV);
    sigdelset(&injSet, SIGBUS);
    sigdelset(&injSet, SIGKILL);
    sigdelset(&injSet, SIGSTOP);

    if(pthread_sigmask(SIG_BLOCK, &injSet, NULL))
    {   
        ERRMSG(msgLog, "sig mask error");
        return StatusCode::FAILURE;
    }
*/

    if (m_MEPProto < 0 || m_MEPProto > 255) {
	ERRMSG(msgLog, "IPProtoIn is an unsigned 8 bit quantity");
        return StatusCode::FAILURE;
    }
    std::string errmsg;

    m_ManagerStop = false;
/*
    if (MEPRxSys::parse_addr(m_StrOdinIPAddr, m_BitOdinIPAddr) != 0) {
        std::string msg;
        if(MEPRxSys::addr_from_name(m_StrOdinIPAddr, m_BitOdinIPAddr, msg) != 0) {
            ERRMSG(msgLog, " Odin address conversion : " + msg);
            return StatusCode::FAILURE;
        }
    }
*/
    m_BitOdinIPAddr = MEPRxSys::IPStringToBits(m_StrOdinIPAddr);

    /// This socket allows us to edit IP header
    if ((m_ToHLTSock =  MEPRxSys::open_sock_arb_source(m_MEPProto, m_MEPBufSize, errmsg)) < 0)    {
        ERRMSG(msgLog, "Failed to open socket:" + errmsg);
	return StatusCode::FAILURE;
    }

    m_HLTIPAddrTo=0;
    m_OdinTell1ID = -1;

    if (!m_AutoMode) {

        void *memory = extendBuffer(&m_OdinData, m_OdinBufSize);
        m_OdinMEP = new(memory) MEPEvent(0);

        if (m_OdinMEP == NULL) {
            ERRMSG(msgLog, "Odin MEP Buffer Allocation failed");
            return StatusCode::FAILURE;
        }

        msgLog << MSG::DEBUG << "IP to send to odin : "<<m_OdinIfIPAddr << " interface : "<<m_OdinEthInterface << endmsg; 

	if ((m_FromOdinSock = MEPRxSys::open_sock(m_MEPProto, m_OdinBufSize, m_OdinEthInterface, m_OdinIfIPAddr, false, errmsg)) < 0) {
            ERRMSG(msgLog, "Failed to open socket:" + errmsg);
            return StatusCode::FAILURE;
	}

        if ((m_FromHLTSock = MEPRxSys::open_sock(m_MEPReqProto, 
                                              m_MEPReqBufSize, 
					      m_HLTEthInterface, m_HLTIfIPAddr, 
                                              true, errmsg)) < 0) {
            ERRMSG(msgLog, "Failed to open socket:" + errmsg);
            return StatusCode::FAILURE;
        }
 
        if ((m_ToOdinSock = MEPRxSys::open_sock(m_MEPReqProto, 
                                              m_MEPReqBufSize, 
                                              m_OdinEthInterface, m_OdinIfIPAddr, 
                                              true, errmsg)) < 0) { 
            ERRMSG(msgLog, "Failed to open socket:" + errmsg);
            return StatusCode::FAILURE;
        }


        if(sem_init(&m_OdinCount, 0, 0) == -1) {
            ERRMSG(msgLog, "Failed to initialize semaphore");
            perror("sem_init");
            exit(errno);
        }
  
        if(sem_init(&m_MEPReqCount, 0, 0) == -1) {
            ERRMSG(msgLog, "Failed to initialize semaphore");
            perror("sem_init");
            exit(errno);
        }  


	if (pthread_mutex_init(&m_SyncMainOdin, NULL)) {
            ERRMSG(msgLog, "Failed to initialize mutex");
            return StatusCode::FAILURE;
	}
        if (pthread_mutex_init(&m_SyncReqOdin, NULL)) {
            ERRMSG(msgLog, "Failed to initialize mutex");
            return StatusCode::FAILURE;
        }


	if(pthread_create(&m_ThreadOdinMEPManager, NULL, OdinMEPMgrStartup, this)) {
	    ERRMSG(msgLog, "Failed to start MEP Odin Manager Thread ");
            return StatusCode::FAILURE;
	}
	if(pthread_create(&m_ThreadMEPReqManager, NULL, MEPReqMgrStartup, this)) {
	    ERRMSG(msgLog, "Failed to start MEP Request Manager Thread ");
            return StatusCode::FAILURE;
	}
    }
    else
        m_EvtID = m_L0ID & 0x0000ffff;

/*
    if(pthread_create(&m_InjectorProcessing, NULL, injectorProcessingStartup, this)) {
        ERRMSG(msgLog, "Failed to start Injector Processing Thread ");
        return StatusCode::FAILURE;
    }
*/
/*
    cpu_set_t mask;
    CPU_ZERO(&mask);
    if(pthread_getaffinity_np(m_InjectorProcessing, CPU_SETSIZE, &mask)) {
        ERRMSG(msgLog, "Failed to get InjectorProcessing Affinity");
        return StatusCode::FAILURE;
    }

    for(int i=0; i<8; ++i) {
        if(CPU_ISSET(i, &mask)) msgLog << MSG::INFO << "CPU " << i << " is set for InjectorProcessing Thread" << endmsg;
    }
    exit(0);
*/

    if(m_ARPMe)
        pingHLTNodes();


    if(!m_AutoMode)
    {
        m_InjState = READY;
        msgLog << MSG::DEBUG << "Injector initialized in TFC mode" << endmsg;
    }
    else
    { 
        m_InjState = READY; //RUNNING
        msgLog << MSG::DEBUG << "Injector initialized in automode" << endmsg;
    }


/*
    if(clock_gettime(CLOCK_REALTIME, &m_RTInitTime)) {
        ERRMSG(msgLog, "clock_gettime failed");
        return StatusCode::FAILURE;
    }
*/

    if(m_Debug) {  
        std::string strFile = "/tmp/injectorcredits";
        time_t date = time(NULL);
        if(date != -1) {
            strFile.append(ctime(&date));
        }
   
     
        if( ( m_CreditsFD = open(strFile.c_str(), O_WRONLY | O_CREAT ) ) < 0) {
            ERRMSG(msgLog, "credits log file could not be written");
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;
}

/** DEBUGGING ONLY: log received MEP requests into a file
 * 
 */
StatusCode MEPInjector::logMEPReqs() 
{
    MsgStream msgLog(msgSvc(), name());     //Message stream for output
    if(m_CreditsFD < 0) {
        ERRMSG(msgLog, "Bad log file descriptor");
        return StatusCode::FAILURE;
    }
    int err=0;
    char buf[32];
    memcpy(buf, &m_TotElapsedTime, sizeof(int));
    memcpy(buf + sizeof(int), &m_TotCreditConsumed, sizeof(int));
    memcpy(buf + 2*sizeof(int), &m_TotMEPReqTx, sizeof(int));
    memcpy(buf + 3*sizeof(int), &m_TotMEPReqRx, sizeof(int));
    err= write(m_CreditsFD, buf, sizeof(buf));
    if(err < 0) { 
        ERRMSG(msgLog, "Error on write");
        return StatusCode::FAILURE;
    }
    if(err < (int) sizeof(buf)) 
    {
        ERRMSG(msgLog, "Log bytes were not all written ..."); 
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}


/** Ping the destination nodes in order to add them to the ARP table
 * @deprecated since default route through the switch has been configured, 
 * and its MAC address has been added statically to the ARP table
 */
StatusCode MEPInjector::pingHLTNodes()
{
    MsgStream msgLog(msgSvc(), name());     //Message stream for output

    char icmpDatagram[84];
    bzero(icmpDatagram, 84);
    msgLog << MSG::DEBUG << "ping to HLT nodes" << endmsg;
    if(m_AutoMode) 
    {
        int n =
            MEPRxSys::send_msg_arb_source(m_ToHLTSock, 0x01, MEPRxSys::IPStringToBits(m_HLTIfIPAddr),
                                          inet_addr(m_HLTStrIPAddrTo.c_str()), icmpDatagram, 84);
        if(n!=84) {
            msgLog << MSG::ERROR << "Ping failed" << endmsg;
            return StatusCode::FAILURE;
        }
        msgLog << MSG::DEBUG << m_HLTStrIPAddrTo << " pinged in order to add it to the ARP table" << endmsg;
    }
    else
    {
        for(std::vector<std::string>::iterator ite = m_HLTNodes.begin(); ite != m_HLTNodes.end(); ++ite)
        { 
            msgLog << MSG::DEBUG << " ping from " << m_HLTIfIPAddr << " to " << *ite << endmsg; 
            int n =
                MEPRxSys::send_msg_arb_source(m_ToHLTSock, 0x01, MEPRxSys::IPStringToBits(m_HLTIfIPAddr),
                                          inet_addr(ite->c_str()), icmpDatagram, 84);
            if(n!=84) {
                msgLog << MSG::ERROR << "Ping failed" << endmsg;
                return StatusCode::FAILURE;  
            } 
            msgLog << MSG::DEBUG << *ite << " pinged in order to add it to the ARP table" << endmsg;
        }
    }
    return StatusCode::SUCCESS;
}

/** Merge the information of the src Odin MEP into the dest Odin MEP
 * Event type, calibration step and orbit comes from tape.
 * The word 9 bitmask can be defined in the option file, the default value has been agreed after the first FEST week:
 *     LSB comes from TFC
 *     MSG comes from Tape
 */
void MEPInjector::copyOdinBank(OnlineRunInfo **dest, OnlineRunInfo **src) 
{
    unsigned int *word9tfc, *word9tape;
    MsgStream msgLog(msgSvc(), name());     //Message stream for output
    if(dest != NULL && src != NULL) {
        if(*dest != NULL && *src !=NULL) {
            if(m_Debug) {
                msgLog << MSG::DEBUG << "        | From TFC | From Tape " << endmsg;
                msgLog << MSG::DEBUG << "Run     |"<< (*dest)->Run << " | " << (*src)->Run << endmsg; 
                msgLog << MSG::DEBUG << "EvtType |"<< (*dest)->EventType << " | " << (*src)->EventType << endmsg; 
                msgLog << MSG::DEBUG << "Calib   |"<< (*dest)->CalibrationStep << " | " << (*src)->CalibrationStep << endmsg; 
                msgLog << MSG::DEBUG << "Orbit   |"<< (*dest)->Orbit << " | " << (*src)->Orbit << endmsg; 
                msgLog << MSG::DEBUG << "L0Id    |"<< (*dest)->L0ID << " | " << (*src)->L0ID << endmsg; 
                msgLog << MSG::DEBUG << "GPS     |"<< (*dest)->GPSTime << " | " << (*src)->GPSTime << endmsg; 
                unsigned int *word8tfc, *word8tape;
                word8tfc=(unsigned int*) *dest;
                word8tfc+=7;
                word8tape=(unsigned int*) *src;
                word8tape+=7;
                msgLog << MSG::DEBUG << "Word 8  |"<< *word8tfc   << " | " << *word8tape << endmsg;

            }
    
            (*dest)->EventType = (*src)->EventType;
            (*dest)->CalibrationStep = (*src)->CalibrationStep;
            (*dest)->Orbit = (*src)->Orbit;

 //           (*dest)->detStatus = (*src)->detStatus; //word 8
 //           (*dest)->errors = (*src)->errors;       
 //           (*dest)->bunchID = (*src)->bunchID;     //word 9
/*
            (*dest)->TAEWindow = (*src)->TAEWindow;   
            (*dest)->pad0 = (*src)->pad0;
            (*dest)->triggerType = (*src)->triggerType;
            (*dest)->readoutType = (*src)->readoutType;
            (*dest)->forceBit = (*src)->forceBit;
            (*dest)->bxType = (*src)->bxType;
            (*dest)->bunchCurrent = (*src)->bunchCurrent;
*/            

            word9tfc = (((unsigned int *) (*dest))+8);
            word9tape = (((unsigned int *) (*src))+8); 
            if(m_Debug) 
                msgLog << MSG::DEBUG << "Word 9  |"<< *word9tfc   << " | " << *word9tape << endmsg;  
            *word9tfc = ((*word9tfc)&m_TFCMask) | ((*word9tape)&m_TapeMask);

        }     
        else
        {
            if(*dest == NULL) msgLog << MSG::WARNING << "No Odin bank where to write information" << endmsg;
            if(*src == NULL) msgLog << MSG::WARNING << "No Odin bank where to get information" << endmsg;
        }
    }
}

/** Display a buffer in hexadecimal
 */
void displayBuff(char *buf, int size) 
{   
    int *ibuf = (int *) buf;
    for(int i=0; i<size/4; ++i) {
        if(i%4 == 0) printf("\n%5x:", i);
        printf(" %8x ", ibuf[i]);
    }
    printf("\n");
}


/** Get an event from the buffer managers, according to the trigger type of the Odin MEP
 * @param nbEv: the current offset in the MEP production, in order to get the good Odin bank 
 */
StatusCode MEPInjector::getEvent(int nbEv)
{
    static MsgStream msgLog(msgSvc(), name());

    static int *p;
    if(p==NULL) 
        p=new int[TWOMB]; ///XXX BERK
    Requirement r;

    BMID bmid;

    int eventActual = 0;
    do 
    {
        if(m_EventBuffers.size() < 1) {
            msgLog << MSG::ERROR << "No input to get event from ..." << endmsg;
            return StatusCode::FAILURE;
        } 
        else if(m_EventBuffers.size() == 1 || m_AutoMode) 
        {
            bmid = m_EventBuffers.begin()->second;
        }
        else  
        {
            // Select the stream according to the trigger type
            char *ccur = (((char *) m_OdinMEP) + MEPEVENTOFFSET);
            OnlineRunInfo *ori = ( OnlineRunInfo *) (ccur+IPHDRSZ+MEPHDRSZ+nbEv*(ODFRAGSZ) + FRAGHDRSZ + BKHDRSZ);
            msgLog << MSG::INFO << "Online Trigger Type: " << ori->triggerType << endmsg; 
            switch(ori->triggerType) {
                    // Static "map" not very elegant but easier to implement and to control via job options 
                case 1 : //LUMI
                        msgLog << MSG::INFO << "LUMI event selected"<<endmsg;
                        bmid = m_EventBuffers[LUMI];
                        break;
                case 0 : //PHYS, should not be seen, process it like PERA (periodic A)
                      
                case 2 : //AUXI, should not be seen, process it like PERA
                case 3 : //SEQU, don't know what it is, process it like PERA
                case 5 : //UNKN
                case 6 : //TIMI, don't know what it is, process it like PERA
                case 7 : //CALX, don't know what it is, process it like PERA
                case 4 : //PERA or PERB
                        msgLog << MSG::INFO << "PERA event selected"<<endmsg;
                        bmid = m_EventBuffers[PERA];
                        break;
                default : msgLog << MSG::ERROR << "Odin trigger type decoding error" << endmsg;
                        return StatusCode::FAILURE;
            
            } 
      
        }   
        if(mbm_events_actual(bmid, &eventActual) != MBM_NORMAL)
        {   
            msgLog << MSG::WARNING << "Error checking number of event in buffer " << LUMI << endmsg;
        }
        else
        {
            if(eventActual <= 0) {
                static int i = 0;
                if(++i %1000 == 0) msgLog << MSG::WARNING << "oups ca fait longtemps" << endmsg;
                MEPRxSys::microsleep(1000);
            }
        }

    }
    while(eventActual <= 0 && m_InjState == RUNNING);

    if(m_InjState != RUNNING) { 
        msgLog << MSG::INFO << "End of injection : mbm_get_event cancelled" << endmsg;
        return StatusCode::RECOVERABLE;
    }

    if(mbm_get_event(bmid,&p, &m_CurEventSize, &r.evtype, r.trmask, m_PartitionID) != MBM_NORMAL)
    {
        msgLog << MSG::ERROR << "mbm_get_event failed" << endmsg;
        return StatusCode::FAILURE;
    }
    memcpy(m_CurEvent, p, m_CurEventSize); 
    mbm_free_event(bmid);
    ++m_TotEvtsRead;
    return StatusCode::SUCCESS;

}

/** Read banks from msg queue, and store each banks in the future MEP
 */
StatusCode MEPInjector::readEvent()
{
    static int icalled = 0;
    icalled++;

    static MsgStream msgLog(msgSvc(), name());	//Message stream for output

    static unsigned int nbEv =0;
    int tell1id = 0;
    int len = 0;
 
    ///XXX Get event
    StatusCode sc = getEvent(nbEv); 
    if(sc.isRecoverable()) {
        msgLog << MSG::INFO << "End of injection : exiting readEvent" << endmsg;
        return StatusCode::RECOVERABLE;
    } 

    if(sc.isSuccess() && m_InjState == RUNNING)
    {
        int bytesRead = MDFHDRSZ;
        
        /// for each bank
        while(bytesRead<m_CurEventSize)
        { 
            RawBank *hdr = (RawBank*) (m_CurEvent+bytesRead);
            bytesRead+=hdr->totalSize();

            if (hdr->totalSize() == 0 || hdr->magic() != 0xCBCB ) {
                ERRMSG(msgLog, "Severe error in fragment");
                return StatusCode::FAILURE;  
	    }

             
            /// Processing of the Odin banks, basically merge tape and TFC bank contents according to what was defined 
            if(hdr->type() == RawBank::ODIN && !m_AutoMode)
            {
                if(m_OdinMEP->size() == 0) {
                    msgLog << MSG::ERROR << "No Odin MEP received, the algorithm should not be there ..." << endmsg;
                    return StatusCode::FAILURE;
                }

                msgLog << MSG::DEBUG << "nbEv = " << nbEv << "; packing factor = " << m_PackingFactor << endmsg;   

                if(m_Offline) {
                    char *ccur = (((char *) m_OdinMEP) + MEPEVENTOFFSET);
                    OnlineRunInfo *ori_to = ( OnlineRunInfo *) (ccur+IPHDRSZ+MEPHDRSZ+nbEv*(ODFRAGSZ) + FRAGHDRSZ + BKHDRSZ); 
                    OnlineRunInfo *ori_tape = (OnlineRunInfo * )(hdr->data());
                    copyOdinBank(&ori_to, &ori_tape);   
	        }
                continue; /// Do not process the Odin bank as the others
            } 
            /// END OF ODIN BANKS PROCESSING

            MEPEvent *curmep = NULL;
            char *ccur = NULL;
            MEPHdr *mh = NULL;
            MEPFrgHdr *mf = NULL; 
      
            if(hdr->type() == RawBank::OT
                   || hdr->type() == RawBank::OTError
                   || hdr->type() == RawBank::OTRaw )
            {
                unsigned int src = hdr->sourceID();
                unsigned int tmp = (src/100 * 16 * 16) + (((src/10) %10) * 16 ) + (src%10);
                hdr->setSourceID(tmp);
            }
    
		tell1id = getTell1IP(hdr->type(), hdr->sourceID());
		if (tell1id == 0) { 
                msgLog << MSG::DEBUG<<"Problem finding Tell1 IP: Unknown source "<<hdr->sourceID()<<" or type "<<RawBank::typeName(hdr->type()) << ", execution continue "<<endmsg; 
		    continue;
		}

		len = hdr->totalSize();	/// I need len = hdr + body + padding, because the bank array contains the padding    
 
            /////////////////////////////////////////
            // Encoding while reading
            ////////////////////////////////////////
            std::map<unsigned long, MEPEvent *>::iterator ite = m_MapTell1MEPs.find(tell1id);
            if(ite == m_MapTell1MEPs.end()) 
            {
                continue; // it means that we have to ignore this tell1, as the map is initialized in the initialize procedure
               
            }

            curmep = m_MapTell1MEPs[tell1id]; 
            ccur = (((char *) curmep) + MEPEVENTOFFSET);
  	        mh = (MEPHdr *) (ccur + IPHDRSZ);
               
            /// Use the mepevent 4 first byte as "pointer" to the beginning of the current fragment, to set up the header
            /// Use the MEp tot len as the current tot len 
            /// Only when reading the last event of a MEP(or the first but if so has to be updated each time ... ), same with Fragment Header and banks
            /// Set the MEP Header
            if(nbEv == 0) 
            {
/*
                /// XXX OT FIX
                if(bkType == RawBank::OT 
                   || bkType == RawBank::OTError 
                   || bkType == RawBank::OTRaw ) 
                { 
                     bzero(curmep, MEPHDRSZ + IPHDRSZ + MEPEVENTOFFSET + m_PackingFactor*(BKHDRSZ + FRAGHDRSZ) + m_PackingFactor*2000 );
                }
                else
*/
                /// Only sets the first headers to 0, and then everything should be fine 
                bzero(curmep, MEPHDRSZ + IPHDRSZ + MEPEVENTOFFSET + BKHDRSZ + FRAGHDRSZ );

                curmep->setSize(IPHDRSZ + MEPHDRSZ ); 
                mh->m_totLen  = MEPHDRSZ;
                mh->m_partitionID = m_PartitionID;
                
                mh->m_l0ID = m_L0ID;
            }
            mh->m_nEvt = nbEv+1;
            
            /// Sets the Fragment header, care, multi banks per fragments ... 
            ccur = (((char *) curmep) + curmep->size() + MEPEVENTOFFSET);    
            mf = ( MEPFrgHdr *) ccur;
            /// The first bank of a fragment is encountered if we still point to the old fragment with mf and the evtId has been incremented
            /// Or if the evtId = 0 and the size of the fragment has never been written 
            if(mf->m_l0IDlow != m_EvtID || (m_EvtID ==0 && mf->m_len == 0 )  ) /// If a bank has not been already registred for this event, i.e. first bank for cur event
            {
                /// First update the pointer to beginning of cur frag = end of previous frag
                curmep->setSize(mh->m_totLen + IPHDRSZ);
                ccur = (((char *) curmep) + curmep->size() + MEPEVENTOFFSET);
                mf = ( MEPFrgHdr *) ccur; 
                mf->m_l0IDlow = m_EvtID;
                mf->m_len = len ;
                mh->m_totLen += FRAGHDRSZ;
            }
            else
            {
                mf->m_len +=len;
            }
            ccur = ((char *)curmep) + MEPEVENTOFFSET + IPHDRSZ + mh->m_totLen; 
            mh->m_totLen +=len;
 
            /// Copy the bank opaque body + hdr + padding
            memcpy(ccur, hdr, len); 
	}			//end for all banks of the event
        if(++nbEv == m_PackingFactor)
        {
            nbEv=0;
        }

    } //end if get event success
    else
        return StatusCode::FAILURE; 
    return StatusCode::SUCCESS;
}

/** A fix for empty and missing OT banks
 * @deprecated since new files producted for 2nd FEST week, in march
 */
StatusCode MEPInjector::OTFIX(int iot, int nbEv) {
    /// Src IDs are from 1 to 47, src > 24 -> A side(12) else C side (16)
    /// bks are ordered from 1 to 47
    MEPBkHdr emptyBk;
    emptyBk.m_type=RawBank::OT;
    emptyBk.m_version=0x3F;
    emptyBk.m_len=9;
    emptyBk.m_magic = 0xCBCB;   
    emptyBk.m_sourceID = iot;

    int tell1id = getTell1IP(RawBank::OT, emptyBk.m_sourceID);
    if (tell1id == 0) {
        return StatusCode::SUCCESS;
    }
    std::map<unsigned long, MEPEvent *>::iterator ite = m_MapTell1MEPs.find(tell1id);
    if(ite == m_MapTell1MEPs.end())
    {   
        return StatusCode::SUCCESS; // it means that we have to ignore this tell1, as the map is initialized in the initialize procedure
    }
    MEPEvent *curmep = ite->second;
    char *ccur = NULL;
    MEPHdr *mh = NULL; 
    MEPFrgHdr *mf = NULL;

    ccur = (((char *) curmep) + MEPEVENTOFFSET);
    mh = (MEPHdr *) (ccur + IPHDRSZ);
    if(nbEv == 0)
    {
        bzero(curmep, MEPHDRSZ + IPHDRSZ + MEPEVENTOFFSET + m_PackingFactor*(BKHDRSZ + FRAGHDRSZ) + m_PackingFactor*2000 );
        curmep->setSize(IPHDRSZ + MEPHDRSZ );
        mh->m_totLen  = MEPHDRSZ;
        mh->m_partitionID = m_PartitionID; 
        mh->m_l0ID = m_L0ID;
    }
    mh->m_nEvt = nbEv+1;                                                                  
    /// Sets the Fragment header, care, multi banks per fragments ...                                    ccur = (((char *) curmep) + curmep->size() + MEPEVENTOFFSET);
    mf = ( MEPFrgHdr *) ccur;
    /// The first bank of a fragment is encountered if we still point to the old fragment with mf and the evtId has been incremented
    /// Or if the evtId = 0 and the size of the fragment has never been written 
    if(mf->m_l0IDlow != m_EvtID || (m_EvtID ==0 && mf->m_len == 0 )  ) /// If a bank has not been already registred for this event, i.e. first bank for cur event
    {
        /// First update the pointer to beginning of cur frag = end of previous frag
        curmep->setSize(mh->m_totLen + IPHDRSZ);
        ccur = (((char *) curmep) + curmep->size() + MEPEVENTOFFSET);
        mf = ( MEPFrgHdr *) ccur;
        mf->m_l0IDlow = m_EvtID;
        mf->m_len = 12;
        mh->m_totLen += FRAGHDRSZ;
    }
    else
    {
        mf->m_len +=12;
    }
    ccur = ((char *)curmep) + MEPEVENTOFFSET + IPHDRSZ + mh->m_totLen;
    mh->m_totLen += sizeof(emptyBk) +4;
    memcpy(ccur, (void *) &emptyBk, sizeof(emptyBk));
    bzero(ccur+sizeof(emptyBk), 4); //useless as it should be already 0 cause of the clean of the buffer, but who knows XXX check it once fest end

    return StatusCode::SUCCESS;
}

/** This function prints out the contents of a MEP.
 *
 *  ( The output is printed in hexadecimal numbers with 16 bytes on each row )
 */
void MEPInjector::printMEP(MEPEvent *me, int size) {
  //int size = me->size();
  const char* data = me->start();
  FILE *fic = fopen("/tmp/odinMEP", "w");
  if(fic == NULL ) return;
  std::fprintf(fic, "size : %d\n", size);
  for (int i=0; i< (size+20); i++) {
    if ((i % 16)==0) {
      std::fprintf(fic,"\n");
      std::fprintf(fic, "%08X :",i);
      
    }
    std::fprintf(fic, " %02X",(unsigned char) data[i]);
  }
  std::fprintf(fic, "\n");
  fclose(fic) ;
}

/** Main processing for injection
 * Select an available HLT node
 * Select the First Odin MEP of the FIFO
 * Build events and sends them
 */
StatusCode MEPInjector::injectorProcessing()
{
    static MsgStream msgLog(msgSvc(), name());

///XXX Try to trap all signals
/*
    sigset_t injSet;
    sigfillset(&injSet);
   
    if(pthread_sigmask(SIG_BLOCK, &injSet, NULL)) 
    {
        ERRMSG(msgLog, "sig mask error");
        return StatusCode::FAILURE;
    }
*/
    StatusCode sc = StatusCode::RECOVERABLE;
    while ((sc.isSuccess() || sc.isRecoverable()) && m_InjState == RUNNING ) {
        //XXX optimization, all get HLT et get Odin in main loop, not in alternative inside the readThenSend, 
        //XXX as we could go back to main loop once mep factor reached, to do when spare time 
        if(sc.isRecoverable()) 
        {
            if(!m_AutoMode) {
                if(!m_gotHLT) {
                    sc = getHLTInfo();
                    if(sc.isRecoverable()) continue;
                    if(sc.isFailure()) {
                        ERRMSG(msgLog, " Selecting a HLT");
                        return StatusCode::FAILURE;
                    }
                    m_gotHLT = true;
                }
  
                if(!m_gotOdin) {
                    sc = getOdinInfo();
                    if(sc.isRecoverable()) continue;
                    if(sc.isFailure()) {
                        ERRMSG(msgLog, " Copying data from Odin MEP");
                        return StatusCode::FAILURE;
                    }
                    m_gotOdin = true;
                }
            }
        }
        else if (sc.isFailure())
        {
            ERRMSG(msgLog, "Main processing failed, aborting !");
            return sc;
        }
        sc = readThenSend();
    }
 
    return StatusCode::SUCCESS;
}

/** Reads events to build MEPs, then sends MEPs
 */
StatusCode MEPInjector::readThenSend()
{
    static MsgStream msgLog(msgSvc(), name());	//Message stream for output


    if((m_TotEvtsSent%500000) == 0 && !m_AutoMode)
    {   
        msgLog << MSG::INFO << "+++++++++++++++++++++++" << endmsg;
        msgLog << MSG::INFO << "-----------------------" << endmsg;
        msgLog << MSG::INFO << "Time of run (ms) :     " << m_TotElapsedTime << endmsg;
        msgLog << MSG::INFO << "Number of Odin MEP :   " << m_TotOdinMEPRx << endmsg;
        msgLog << MSG::INFO << "-----------------------" << endmsg; 
        msgLog << MSG::INFO << "Number of event sent : " << m_TotEvtsSent << endmsg;
        msgLog << MSG::INFO << "Number of event read : " << m_TotEvtsRead << endmsg; 
        msgLog << MSG::INFO << "-----------------------" << endmsg; 
        msgLog << MSG::INFO << "Credits received :     " << m_TotMEPReqRx << endmsg; 
        msgLog << MSG::INFO << "Credits sent :         " << m_TotMEPReqTx << endmsg;
        msgLog << MSG::INFO << "Credits consumed :     " << m_TotCreditConsumed << endmsg; 
        msgLog << MSG::INFO << "-----------------------" << endmsg;
        msgLog << MSG::INFO << "+++++++++++++++++++++++" << endmsg;
        if(m_Debug) logMEPReqs();  
    }


    StatusCode sc;
    static unsigned int nbEv = 1;	// Used in Odin Mode to read the L0Id of the good bank    // Used to count nb event read
    MEPEvent *me = NULL;

    sc = readEvent();
    if (sc.isRecoverable() )  /// End of the job
    {
        msgLog << MSG::INFO<< "End of injection : Exiting readThenSend"<<endmsg; 
        return StatusCode::RECOVERABLE;
    }
    if (sc.isFailure()) {
	msgLog << " Reading an event from the buffer managers" << endmsg;;
        return StatusCode::FAILURE;
    }
  
    finalSent = true;
 
    //If we have reached packing factor
    if (nbEv == m_PackingFactor ) {
	nbEv = 1;

        finalSent = false;
	if (!m_AutoMode) {

            u_int32_t addrFrom = m_BitOdinIPAddr + ((32<<24)&0xff000000); 
            msgLog << MSG::DEBUG<<__FUNCTION__<<"MEP SIZE : " << m_OdinMEP->size() << endmsg;

	    sc = sendMEP(addrFrom, m_OdinMEP);
            if(sc.isFailure()) {
                ERRMSG(msgLog, "Could not send Odin MEP");
                return StatusCode::FAILURE;
            }
	}

        for(std::map<unsigned long, MEPEvent *>::iterator iteTell1ID = m_MapTell1MEPs.begin(); iteTell1ID != m_MapTell1MEPs.end(); ++iteTell1ID)
        {
            me = iteTell1ID->second;
	    if(me != NULL)
            {
                sc = sendMEP(iteTell1ID->first, me);
   	        if (sc.isFailure()) {
	  	    ERRMSG(msgLog, " sendMEP error for Tell1 IP " + iteTell1ID->first);
                    return StatusCode::FAILURE;
                }
	    }
            else
                ERRMSG(msgLog, "NULL MEP pointer, not send : " + MEPRxSys::dotted_addr(iteTell1ID->first));
	} /// end for

        m_TotEvtsSent += m_PackingFactor;
/*
        if(clock_gettime(CLOCK_REALTIME, &m_RTCurTime)) {
            ERRMSG(msgLog, "clock_gettime failed");
            return StatusCode::FAILURE;
        }
        m_RTTotElapsedTime = (m_RTCurTime.tv_sec - m_RTInitTime.tv_sec) * 1000 + (m_RTCurTime.tv_nsec - m_RTInitTime.tv_nsec)/1000000 ;
*/  
        gettimeofday(&m_CurTime, NULL); 
        m_TotElapsedTime = (m_CurTime.tv_sec - m_InitTime.tv_sec)*1000 + (m_CurTime.tv_usec - m_InitTime.tv_usec)/1000 ;
 

	if (!m_AutoMode) {
            m_gotOdin = false;
            m_gotHLT = false;
            /// Get information from Odin and HLT
            StatusCode sc; 
            sc = getHLTInfo();
            if(sc.isRecoverable()) return sc;
            if(sc.isFailure()) {
                ERRMSG(msgLog, " Selecting a HLT");
                return StatusCode::FAILURE;
            }
            m_gotHLT = true;

            sc = getOdinInfo();
            if(sc.isRecoverable()) return sc;
            if(sc.isFailure()) {
                ERRMSG(msgLog, " Copying data from Odin MEP");
                return StatusCode::FAILURE;
            } 
            m_gotOdin = true;

	} else {
	    /// The IP datagram ID is set to the least significant 15 bits of the first 
	    /// event ID of the MEP, which are the least significant 15 bits of the 
	    /// MEP event id   
	    /// If we are in ODIN mode, this Id should be given by ODIN, 
	    /// else we increment  it each time, starting from a job option valu
	    m_L0ID += m_PackingFactor;
	    m_EvtID = 0x0000ffff & m_L0ID;
	} /// end if(!m_AutoMode)  

    } else {  ///Packing factor not reached
	if (m_AutoMode)
	    ++m_EvtID;
	else {
            MEPFragment *mepfrag = (MEPFragment *) ( ((char *) m_OdinMEP) + MEPEVENTOFFSET +
                                   IPHDRSZ + MEPHDRSZ + nbEv * ODFRAGSZ);
            m_EvtID = mepfrag->eventID();
            msgLog << MSG::DEBUG << "EvtID read from Frag Header (and used) : " << m_EvtID << endmsg;
        }
	++nbEv;
 
    }///end if(packing_factor)

/*
    if(m_Debug)
    { 
        ///XXX To do histograms
        if(++BKSTATI > 30000) {
            saveHisto();
            exit(1);
        } 
    }
*/
    return StatusCode::SUCCESS;
}


/** Get the MTU set in the system configuration, 
 */
int MEPInjector::getMTU(int netdev)
{
    struct ifreq eth1req;
    char netdev_name[10];
    sprintf(netdev_name, netdev < 0 ? "lo" : "eth%d", netdev);
    strcpy(eth1req.ifr_name, netdev_name);
    ioctl(m_ToHLTSock, SIOCGIFMTU, &eth1req);

    return eth1req.ifr_mtu;
}


/** Get Tell1 IP, return 0 if no tell1 were found  
 * @param type: Type of the raw bank produced by a Tell1
 * @param src: Source of the raw bank produced by a Tell1
 * @return The IP address of the Tell1 which could have produce the bank bank containing type and src
 */
in_addr_t MEPInjector::getTell1IP(int type, int src)
{
    static MsgStream msgLog(msgSvc(), name());	//Message stream for output
    
//    static stdmap<int, int> MuonMap
    in_addr_t ipNet= ( (192 + (169<<8))&0x0000ffff);
    switch (type) {
    case RawBank::L0CaloFull:	// 37
    case RawBank::L0CaloError:
    case RawBank::L0Calo:
	return (/*inet_addr("192.169.2.0")*/ ipNet|(6<<16)  | ((src + 1) << 24));	//L0Calo = L0Suppression result for Hcal and Ecal
    case RawBank::L0DUError:
    case RawBank::L0DU:
	return (/*inet_addr("192.169.7.2")*/ ipNet|(7<<16) | (2 << 24));
    case RawBank::Velo:
        if(src >= 128) {      //L0PU
            ipNet |= (15<<16);
            switch(src) {
            case 128 : return (ipNet | (2<<24));  
            case 130 : return (ipNet | (3<<24));  
            case 129 : return (ipNet | (4<<24));  
            case 131 : return (ipNet | (5<<24)); 
            default : 
                //msgLog << MSG::WARNING << "Unknown source "<< src << " for Velo bank type"<<endmsg;   
                return 0;
            }
        }
        else 
        {

        ipNet |= (11<<16);
        switch(src) {  /// Don't worry I did not write it by myself, script helps ;-), XXX replace by maps for TT and Velo cause the switch is to big so performances are crap for these bank types
        case 65: return (ipNet | (80<<24));
        case 1: return (ipNet | (61<<24));
        case 3: return (ipNet | (79<<24));
        case 67: return (ipNet | (62<<24));
        case 69: return (ipNet | (78<<24));
        case 5: return (ipNet | (60<<24));
        case 7: return (ipNet | (76<<24));
        case 71: return (ipNet | (59<<24));
        case 73: return (ipNet | (77<<24));
        case 9: return (ipNet | (63<<24));
        case 11: return (ipNet | (64<<24));
        case 75: return (ipNet | (82<<24));
        case 77: return (ipNet | (69<<24));
        case 13: return (ipNet | (81<<24));
        case 15: return (ipNet | (68<<24));
        case 79: return (ipNet | (83<<24));
        case 81: return (ipNet | (74<<24));
        case 17: return (ipNet | (87<<24));
        case 19: return (ipNet | (70<<24));
        case 83: return (ipNet | (88<<24));
        case 85: return (ipNet | (71<<24));
        case 21: return (ipNet | (89<<24));
        case 23: return (ipNet | (72<<24));
        case 87: return (ipNet | (84<<24));
        case 89: return (ipNet | (73<<24));
        case 25: return (ipNet | (85<<24));
        case 27: return (ipNet | (65<<24));
        case 91: return (ipNet | (86<<24));
        case 93: return (ipNet | (67<<24));
        case 29: return (ipNet | (92<<24));
        case 31: return (ipNet | (75<<24));
        case 95: return (ipNet | (90<<24));
        case 97: return (ipNet | (66<<24));
        case 33: return (ipNet | (91<<24));
        case 35: return (ipNet | (57<<24));
        case 99: return (ipNet | (54<<24));
        case 101: return (ipNet | (56<<24));
        case 37: return (ipNet | (52<<24));
        case 39: return (ipNet | (55<<24));
        case 103: return (ipNet | (51<<24));
        case 105: return (ipNet | (58<<24));
        case 41: return (ipNet | (53<<24));
        case 0: return (ipNet | (24<<24));
        case 64: return (ipNet | (35<<24));
        case 66: return (ipNet | (26<<24));
        case 2: return (ipNet | (32<<24));
        case 4: return (ipNet | (22<<24));
        case 68: return (ipNet | (33<<24));
        case 70: return (ipNet | (17<<24));
        case 6: return (ipNet | (31<<24));
        case 8: return (ipNet | (21<<24));
        case 72: return (ipNet | (34<<24));
        case 74: return (ipNet | (23<<24));
        case 10: return (ipNet | (39<<24));
        case 12: return (ipNet | (25<<24));
        case 76: return (ipNet | (36<<24));
        case 78: return (ipNet | (42<<24));
        case 14: return (ipNet | (37<<24));
        case 16: return (ipNet | (40<<24));
        case 80: return (ipNet | (30<<24));
        case 82: return (ipNet | (19<<24));
        case 18: return (ipNet | (38<<24));
        case 20: return (ipNet | (41<<24));
        case 84: return (ipNet | (29<<24));
        case 86: return (ipNet | (20<<24));
        case 22: return (ipNet | (27<<24));
        case 24: return (ipNet | (18<<24));
        case 88: return (ipNet | (28<<24));
        case 90: return (ipNet | (8<<24));
        case 26: return (ipNet | (12<<24));
        case 28: return (ipNet | (7<<24));
        case 92: return (ipNet | (15<<24));
        case 94: return (ipNet | (1<<24));
        case 30: return (ipNet | (9<<24));
        case 32: return (ipNet | (3<<24));
        case 96: return (ipNet | (11<<24));
        case 98: return (ipNet | (4<<24));
        case 34: return (ipNet | (10<<24));
        case 36: return (ipNet | (6<<24));
        case 100: return (ipNet | (14<<24));
        case 102: return (ipNet | (5<<24));
        case 38: return (ipNet | (13<<24));
        case 40: return (ipNet | (2<<24));
        case 104: return (ipNet | (16<<24));
        default : 
            //msgLog << MSG::WARNING << "Unknown source "<< src << " for Velo bank type"<<endmsg;   
            return 0;
        }
        }
        break;
    case RawBank::Rich: // Lets assume that the relation is sourceID +1 for last digit, as they seems to do not care
        

 	return (/*inet_addr("192.169.14.0")*/ (ipNet | (14<<16) ) | ((src)<< 24));
    case RawBank::TTFull:
    case RawBank::TTError:
    case RawBank::TTPedestal:
    case RawBank::TTProcFull:
    case RawBank::TT:
        ipNet |= (10<<16);
        switch(src) {
        case 0: return (ipNet | (17<<24));
        case 1: return (ipNet | (18<<24));
        case 2: return (ipNet | (19<<24));
        case 3: return (ipNet | (20<<24));
        case 4: return (ipNet | (1<<24));
        case 5: return (ipNet | (2<<24));
        case 6: return (ipNet | (3<<24));
        case 7: return (ipNet | (33<<24));
        case 8: return (ipNet | (34<<24));
        case 9: return (ipNet | (35<<24));
        case 10: return (ipNet | (36<<24));
        case 32: return (ipNet | (21<<24));
        case 33: return (ipNet | (22<<24));
        case 34: return (ipNet | (23<<24));
        case 35: return (ipNet | (24<<24));
        case 36: return (ipNet | (4<<24));
        case 37: return (ipNet | (5<<24));
        case 38: return (ipNet | (6<<24));
        case 39: return (ipNet | (37<<24));
        case 40: return (ipNet | (38<<24));
        case 41: return (ipNet | (39<<24));
        case 42: return (ipNet | (40<<24));
        case 64: return (ipNet | (25<<24));
        case 65: return (ipNet | (26<<24));
        case 66: return (ipNet | (27<<24));
        case 67: return (ipNet | (28<<24));
        case 68: return (ipNet | (7<<24));
        case 69: return (ipNet | (8<<24));
        case 70: return (ipNet | (9<<24));
        case 71: return (ipNet | (10<<24));
        case 72: return (ipNet | (11<<24));
        case 73: return (ipNet | (41<<24));
        case 74: return (ipNet | (42<<24));
        case 75: return (ipNet | (43<<24));
        case 76: return (ipNet | (44<<24));
        case 96: return (ipNet | (29<<24));
        case 97: return (ipNet | (30<<24));
        case 98: return (ipNet | (31<<24));
        case 99: return (ipNet | (32<<24));
        case 100: return (ipNet | (12<<24));
        case 101: return (ipNet | (13<<24));
        case 102: return (ipNet | (14<<24));
        case 103: return (ipNet | (15<<24));
        case 104: return (ipNet | (16<<24));
        case 105: return (ipNet | (45<<24));
        case 106: return (ipNet | (46<<24));
        case 107: return (ipNet | (47<<24));
        case 108: return (ipNet | (48<<24));
        default : 
            //msgLog << MSG::WARNING << "Unknown source "<< src << " for TT bank type"<<endmsg;   
            return 0; 
        } 
	break;
    case RawBank::ITFull:
    case RawBank::ITError:
    case RawBank::ITPedestal:
    case RawBank::ITProcFull:
    case RawBank::IT:
        ipNet |= (13<<16);
	if (src < 16)
	    return (/*inet_addr("192.169.13.0")*/ ipNet | ((src + 1) << 24));
	// if (src < 32) return 1 + 1 + 88 + 18 + 48 + 14 + (src - 11);
	if (src < 64)
	    return (/*inet_addr("192.169.13.0")*/ ipNet | ((src - 17) << 24));
	if (src < 90)
	    return (/*inet_addr("192.169.13.0")*/ ipNet | ((src - 35) << 24));
	break;
    case RawBank::OTRaw:	// 32
    case RawBank::OTError:
    case RawBank::OT:
	char T = (src >> 8) & 0x0F;   
	char L = (src >> 4) & 0x0F;
	char Q = (src >> 0) & 0x0F;
//        if(src>24)
	if ((Q & 0x1) == 1)	// A side
	{
          ipNet+=(12<<16); 
	} 
        else			// C side 
	{
          ipNet+=(16<<16); 
	}
        return (ipNet+ ( ( (((Q >> 1)& 0x1)*12) + ((T-1)*4) + L + 1 )<< 24) );
//        return (ipNet + ((((src) %24)+1) << 24));

    case RawBank::MuonFull:
    case RawBank::MuonError:
    case RawBank::Muon:
	switch (src) {
	case 0:
	    return (/*inet_addr("192.169.18.6")*/ ipNet | (18<<16)|(6<<24));
	case 1:
	    return (/*inet_addr("192.169.18.7")*/ ipNet | (18<<16)|(7<<24));
	case 2:
	    return (/*inet_addr("192.169.9.7")*/ ipNet | (9<<16)|(7<<24));
	case 3:
	    return (/*inet_addr("192.169.9.6")*/ ipNet | (9<<16)|(6<<24));
	case 4:
	    return (/*inet_addr("192.169.18.4")*/ ipNet | (18<<16)|(4<<24));
	case 5:
	    return (/*inet_addr("192.169.18.5")*/ ipNet | (18<<16)|(5<<24));
	case 6:
	    return (/*inet_addr("192.169.9.5")*/ ipNet | (9<<16)|(5<<24));
	case 7:
	    return (/*inet_addr("192.169.9.4")*/ ipNet | (9<<16)|(4<<24));
	case 8:
	    return (/*inet_addr("192.169.18.3")*/ ipNet | (18<<16)|(3<<24));
	case 9:
	    return (/*inet_addr("192.169.9.3")*/ ipNet | (9<<16) |(3<<24));
	case 10:
	    return (/*inet_addr("192.169.18.2")*/ ipNet | (18<<16) |(2<<24));
	case 11:
	    return (/*inet_addr("192.169.9.2")*/ ipNet | (9<<16) |(2<<24));
	case 12:
	    return (/*inet_addr("192.169.18.1")*/ ipNet | (18<<16) |(1<<24));
	case 13:
	    return (/*inet_addr("192.169.9.1")*/ ipNet | (9<<16) |(1<<24));
	default:
            //msgLog << MSG::WARNING << "Unknown source "<< src << " for Muon bank type"<<endmsg;   
            return 0;

	}
        break;
    case RawBank::L0PU:
        ipNet |= (15<<16);
        switch(src) {
        case 0 : return (ipNet | (1<<24));   /// Not sure about only this one, if an unknown src appear, it should be it ;) /* Dear Martin mentionned 2 but it seems it is 0*/
        default : 
            //msgLog << MSG::WARNING << "Unknown source "<< src << " for L0PU bank type"<<endmsg;   
            return 0;
        }  
	return (/*inet_addr("192.169.15.0")*/ ipNet |(15<<16) + ((src + 1) << 24));	//TPU
    case RawBank::DAQ:
	return ipNet; //XXX
    case RawBank::ODIN:  ///XXX Not used in TFC mode, maybe wrong when used in AutoMode
        return m_BitOdinIPAddr + ((32<<24)&0xff000000); 
    case RawBank::EcalE:
    case RawBank::EcalTrig: 
    case RawBank::EcalPackedError: 
    case RawBank::EcalPacked:
	return (/*inet_addr("192.169.3.0")*/ ipNet |(3<<16) | ((src + 1) << 24));
    case RawBank::HcalE:
    case RawBank::HcalTrig:
    case RawBank::HcalPackedError:
    case RawBank::HcalPacked:
	return (/*inet_addr("192.169.2.0")*/ ipNet |(2<<16) | ((src + 1) << 24));
    case RawBank::PrsE:
    case RawBank::PrsTrig:
    case RawBank::PrsPackedError:
    case RawBank::PrsPacked:
	return (/*inet_addr("192.169.8.0")*/ ipNet |(8<<16) | ((src + 1) << 24));
    case RawBank::L0MuonCtrlAll:	// 39
    case RawBank::L0MuonProcCand:	// 40
    case RawBank::L0MuonProcData:	// 41
    case RawBank::L0MuonRaw:	// 42
    case RawBank::L0MuonError:	// 43
    case RawBank::L0Muon: // TMU
        ipNet+= (4<<16);
        switch(src) {
        case 3 :  return (ipNet | (4<<24));
        case 4 :  return (ipNet | (5<<24));
        case 5 :  return (ipNet | (6<<24));
        case 2 :  return (ipNet | (1<<24));
        case 1 :  return (ipNet | (2<<24));
        case 0 :  return (ipNet | (3<<24));
        default : 
            //msgLog << MSG::WARNING << "Unknown source "<< src << " for L0Muon bank type"<<endmsg;   
            return 0;
        } 
             
	// case RawBank:: HLT:        return 1 + 1 + 88 + 18 + 42 + 48 + 24 + 36 + 2 + 1 + src;
    default:
        break;
    }
    return 0;
}

/** Sends a MEP to a HLT node, manages the fragmentation of the IP datagram
 * @param tell1IP: The HLT node IP address
 * @param me: The MEP to send
 */
StatusCode MEPInjector::sendMEP(int tell1IP, MEPEvent * me)
{
    static MsgStream msgLog(msgSvc(), name());

    if(me->size() == 0) { 
        msgLog << MSG::WARNING << " Empty MEP for Tell1 " << MEPRxSys::dotted_addr(tell1IP) << endmsg;
        return StatusCode::SUCCESS; //no data to send
    }
    me->setSize(0);
    //Apply an offset of 4 to the MEPEvent structure to be sent, because the first 4 bytes are the size of the whole structure
    char *datagram = (char *) me + 4;

    struct MEPHdr *mh = NULL;
    mh = (struct MEPHdr *) &(((u_int8_t *) me->start())[20]);        // The MEP-header starts after the 20 byte IP-header
    int MEPSize = mh->m_totLen;


    u_int16_t datagramID =
	((u_int16_t) (0x000000FF & m_L0ID) << 8) |
	((u_int16_t) (0x0000FF00 & m_L0ID) >> 8);

    int iDatagramSize = MEPSize +20 ;

    /// The size which interests us is the size of the IP datagram
    /// So lets remove the ethernet header from the max size 
    int iIPMTU = getMTU(m_HLTEthInterface) - 14;

    u_int32_t addrTo;
    if(m_AutoMode)
    {/*
        if (MEPRxSys::parse_addr(m_HLTStrIPAddrTo, addrTo) != 0) {
            std::string msg;
            if(MEPRxSys::addr_from_name(m_HLTStrIPAddrTo, addrTo, msg) != 0) {
   	        ERRMSG(msgLog, " Automode HLT Address conversion ");
                return StatusCode::FAILURE;
            }
        }
      */
      addrTo = MEPRxSys::IPStringToBits(m_HLTStrIPAddrTo);
    }
    else
        addrTo = m_HLTIPAddrTo;
    /// If the frame size, i.e. the size of the datagram + the ethernet header, is less than the MTU, send it in one block, else fragment it  
    if (iDatagramSize < iIPMTU) {
	/// Do not fragment

	int n =
	    MEPRxSys::send_msg_arb_source(m_ToHLTSock, m_MEPProto, tell1IP,
					  addrTo, datagram, iDatagramSize,
					  datagramID);

	if (n == (int) iDatagramSize) {
            m_TotMEPsTx++;
            m_TotBytesTx += MEPSize; 
	    return StatusCode::SUCCESS;
	}
	if (n == -1) {
	    ERRMSG(msgLog, " MEP sending ");
	    return StatusCode::FAILURE;
	}
	ERRMSG(msgLog, " MEP corrupted on send! Sent length:" + n);
        return StatusCode::FAILURE;
    }


    /// Fragmentation needed
    /// Fragments are send in the same order as the data 

    /// Pointer to the MEP data, skipping the ip header
    char *cFullMEP = (char *) me + 4 + 20;

    /// Buffer which will store the fragments to send
    char *cBuf = (char *) malloc(iIPMTU);
    if (cBuf == NULL) {
	ERRMSG(msgLog, " No memory for buffer ");
	return StatusCode::FAILURE;
    }
    struct iphdr *hdr = (struct iphdr *) cBuf;
    hdr->frag_off = 0x0000;


    /// We can put MTU - IP header len - ETH header len data in each frame, and it has to be aligned on 8 bytes, because the 3 lower bytes from the fragment offset field are always 0
    int iAlign = (iIPMTU - 20) % 8;
    unsigned int uiMaxData = (iIPMTU - 20) - iAlign;

    int iNbIter = MEPSize / uiMaxData;

    /// Last datagram size without headers, it contains the bytes which could not get aligned in the full datagrams 
    int iLastDGramSize = MEPSize - uiMaxData * iNbIter;

    if(m_Debug) {
        msgLog << MSG::DEBUG << " Number of fragments : " << iNbIter << " of size "<< iIPMTU << endmsg;
        msgLog << MSG::DEBUG << " Size of last fragment : " << iLastDGramSize <<endmsg;
    }
    int iBytesSent = 0;

    for (int iCur = 1; iCur <= iNbIter; ++iCur)	/// 
    {
	/// Copy the data from the original datagram into the new one
	memcpy(cBuf + 20, cFullMEP, uiMaxData);

	/// Set the flag "More fragment" 
	hdr->frag_off |= 0x0020;

	/// Send the datagram (format the header) 
	int n =
	    MEPRxSys::send_msg_arb_source(m_ToHLTSock, m_MEPProto, tell1IP,
					  addrTo, cBuf, uiMaxData + 20,
					  datagramID);
	if (n == -1) {
	    ERRMSG(msgLog, " MEP sending ");
	    return StatusCode::FAILURE;
	}
        m_TotMEPsTx++;
	iBytesSent += n;

	/// Prepare next datagram header
	/// Increment IP fragment offset 
	unsigned short tmpFragOff = (uiMaxData * iCur) >> 3;
	unsigned short cMask = 0x0000;
	cMask = tmpFragOff >> 8;
	hdr->frag_off = tmpFragOff << 8;
	hdr->frag_off |= cMask;

	hdr->frag_off &= 0xff1f;

	cFullMEP += uiMaxData;
    }

    /// Send the last datagram

    memcpy(cBuf + 20, cFullMEP, iLastDGramSize);

    /// Unset the flag "More fragment"
    hdr->frag_off &= 0xffDf;

    int n =
	MEPRxSys::send_msg_arb_source(m_ToHLTSock, m_MEPProto, tell1IP,
				      addrTo,
				      cBuf, iLastDGramSize + 20,
				      datagramID);

    iBytesSent += n;

    msgLog << MSG::
	DEBUG << "Number of bytes sent : " << iBytesSent << "out of " <<
	MEPSize << endmsg;

    free(cBuf);

    if (n == -1) {
	ERRMSG(msgLog, " MEP sending ");
	return StatusCode::FAILURE;
    }
    if (iBytesSent - ((iNbIter + 1) * 20) == MEPSize ) {
	msgLog << MSG::
	    DEBUG << "MEP successfully fragmented and sent!" << endmsg;

        m_TotMEPsTx++;
        m_TotBytesTx += iBytesSent; 
	return StatusCode::SUCCESS;
    }
    ERRMSG(msgLog, " MEP corrupted on send! Sent length:" + n);
    return StatusCode::FAILURE;
}

/** Send a MEP request packet to the Readout Supervisor
 * @param req: The MEP request
 */
StatusCode MEPInjector::sendMEPReq(MEPReq * req)
{
    static MsgStream msgLog(msgSvc(), name()); 

    int n =
	MEPRxSys::send_msg(m_ToOdinSock, m_BitOdinIPAddr, MEP_REQ_TOS, req, MEP_REQ_LEN,
			   0);
    if (n == MEP_REQ_LEN) {
        msgLog << MSG::DEBUG<<"MEP Req successfully sent"<<endmsg;
	m_TotMEPReqTx += req->nmep;
	m_TotMEPReqPktTx++;
	return StatusCode::SUCCESS;
    }
    if (n == -1) {
	ERRMSG(msgLog, " Send MEP request");
        return StatusCode::FAILURE;
    }
    ERRMSG(msgLog, " MEPRequest corrupted on send!");
    return StatusCode::FAILURE;
}

/** Receive a MEP from the Readout Supervisor
 * @param bufMEP: The buffer in which the MEP will be stored
 * @param retLen: The size of the Odin MEP
 */
StatusCode MEPInjector::receiveOdinMEP(char *bufMEP, int *retLen)
{
    static MsgStream msgLog(msgSvc(), name());

    int n = MEPRxSys::rx_poll(m_FromOdinSock, m_TimeOut);	 
  
 
    if (n != 1) {
        /// Recoverable will ask for one more iteration of the manager
        /// which checks boolean which asks for end of injection
        return StatusCode::RECOVERABLE;
    }

    /// Reception with IP header, some cooking needed ?
    int len =
	MEPRxSys::recv_msg(m_FromOdinSock, bufMEP,
			   m_OdinBufSize,
			   0);
    if (len < 0) {
	if (!MEPRxSys::rx_would_block()) {
	    ERRMSG(msgLog, " recvmsg");
            return StatusCode::FAILURE;
        }

	///XXX MANAGE THIS BETTER ! 
    }
    *retLen = len;
 
    m_TotOdinMEPRx++;

    RTL::IPHeader * iphdr = (RTL::IPHeader *) (bufMEP);

    ///Source address = 192.169.5.X + 32 (where OdinIPAdress for MEP Requests is 192.169.5.X)
    u_int32_t odinIPAddr = m_BitOdinIPAddr + ((32<<24)&0xff000000);  
 
    
    /// Check that the Odin MEP sender IP is the one we want 
    msgLog << MSG::DEBUG <<"IP match : Packet from "<< MEPRxSys::dotted_addr(iphdr->saddr) << ", expected " << MEPRxSys::dotted_addr(odinIPAddr) << endmsg;
    if (iphdr->saddr != odinIPAddr) {
	ERRMSG(msgLog, " Unexepected Odin source for received data");
        return StatusCode::RECOVERABLE;   ///XXX RECOVER THIS ERROR
    }
    msgLog << MSG::DEBUG<<"ODIN MEP RECEIVED"<<endmsg;
 
    return StatusCode::SUCCESS;
}

/** 
 */
StatusCode MEPInjector::receiveMEPReq(char *req)
{
    static MsgStream msgLog(msgSvc(), name());
 
    if (req == NULL)
	return StatusCode::FAILURE;

    int n = MEPRxSys::rx_poll(m_FromHLTSock, m_TimeOut);	
    if (n != 1) {
        /// Recoverable will ask for one more iteration of the manager
        /// which checks boolean which asks for end of injection
        return StatusCode::RECOVERABLE;
    }
  
    int len =
	MEPRxSys::recv_msg(m_FromHLTSock, req, MEP_REQ_LEN + IPHDRSZ, 0);
    
    if (len < 0) {
	if (!MEPRxSys::rx_would_block()) {
	    ERRMSG(msgLog, "recvmsg");
            return StatusCode::FAILURE;
        }
    }
    if (len != MEP_REQ_LEN + IPHDRSZ) {
	ERRMSG(msgLog, " Packet received is not a MEP Request ");
        return StatusCode::FAILURE;
    }
    msgLog << MSG::DEBUG<<"MEP REQUEST RECEIVED"<<endmsg;
    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::manageMEPRequest()
{
    static MsgStream msgLog(msgSvc(), name());
    StatusCode sc;
    char req[MEP_REQ_LEN + IPHDRSZ];
    memset(req, 0, sizeof(req));
 
    while (!m_ManagerStop) {
        if(m_InjState != RUNNING) { 
            MEPRxSys::microsleep(1000);
            continue;   //If injector is not running, idle  
        }
 
	sc = receiveMEPReq(req);
        if(sc.isRecoverable()) /// Should be the end of injection but continue to check 
        { 
            continue;
        } 
	if (sc.isFailure()) {
	    ERRMSG(msgLog, " MEP Request Receive failed");
	    return sc;
	}

        msgLog << MSG::DEBUG<< "Got a MEP Request, forward it"<<endmsg;
        /// XXX Not tested yet : this send has been move from end of function to this point 
	/// Forward the MEP Request received, skip headers, default will be better for us 
        sc = sendMEPReq((MEPReq *) (req + 20));	
	if (sc.isFailure()) {
	    ERRMSG(msgLog, " MEP Request Send");
	    return sc;
	}
 

        if (pthread_mutex_lock(&m_SyncReqOdin)) {
            ERRMSG(msgLog, " Locking mutex");
            return StatusCode::FAILURE;
        }

        /// Increment the number of MEP request from the HLT in the map
	struct iphdr *hdr = (struct iphdr *) req;
	MEPReq *mreq = (MEPReq *) (req + 20);
	if (m_HLTReqs.find(hdr->saddr) == m_HLTReqs.end()) {
	    m_HLTReqs[hdr->saddr] = (unsigned int) (mreq->nmep &0x000000ff);
	} else {
	    m_HLTReqs[hdr->saddr] += (unsigned int) (mreq->nmep &0x000000ff);
	}

	m_TotMEPReqRx += mreq->nmep;
	m_TotMEPReqPktRx++;
 
        m_CurMEPReqCredits += mreq->nmep;    
  
        if(m_Debug) {
            msgLog << MSG::DEBUG << "FARM IP ADDRESS : "<< MEPRxSys::dotted_addr(hdr->saddr) <<" ; Credit asked " << m_HLTReqs[hdr->saddr] <<endmsg;
            msgLog << MSG::DEBUG << "    Total of credits : " << m_HLTReqs[hdr->saddr] << endmsg;   
            msgLog << MSG::DEBUG << "Total of MEP requested : " << m_TotMEPReqRx << endmsg;
        }

       

 
        for(unsigned int i=0; i< (unsigned int) (mreq->nmep &0x000000ff); ++i) {
            
	    if(sem_post(&m_MEPReqCount)==-1)
            {
                ERRMSG(msgLog, "Posting on the semaphore");
                perror("sem_post");
                exit(errno);  
            } 
        }
        int testsem = 0;
        sem_getvalue(&m_MEPReqCount, &testsem);
        msgLog << MSG::DEBUG << "MEPReq count = "<<testsem << endmsg;


        if (pthread_mutex_unlock(&m_SyncReqOdin)) {
            ERRMSG(msgLog, " Unlocking mutex");
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::manageOdinMEP()
{
    static MsgStream msgLog(msgSvc(), name());
    StatusCode sc;

    char *bufMEP = (char *) malloc(m_OdinBufSize); /// I don't like calloc
    if(bufMEP == NULL) {
        ERRMSG(msgLog, " Memory problem allocating Odin MEP buffer");
        return StatusCode::FAILURE;
    }
    memset(bufMEP, 0, m_OdinBufSize);
  
    int len;

    while (!m_ManagerStop) {
        if(m_InjState != RUNNING) {
            MEPRxSys::microsleep(1000);
            continue;   //If injector is not running, idle  
        }

	sc = receiveOdinMEP(bufMEP, &len);
        if(sc.isRecoverable())  /// Should be the end of injection, continue is to check it 
        {
            continue;
        } 
	if (sc.isFailure()) {
	    ERRMSG(msgLog, "Failed to receive Odin Information : MEP reception");
            return StatusCode::FAILURE;
	}

	if (pthread_mutex_lock(&m_SyncMainOdin)) {
	    ERRMSG(msgLog, "Failed locking mutex");
	    return StatusCode::FAILURE;
	}

        /// Need something (sem) to tell that the copy has been done? or a good algo
         
        /// Add the Odin MEP to the queue
        char* tmp = (char *)malloc(len+4);
        memcpy(tmp+4, bufMEP, len);
        int *itmp = (int *) tmp;
        itmp[0] = len;
        m_QueueOdinMEP.push(tmp);
        m_CurOdinMEPs = m_QueueOdinMEP.size(); 

        msgLog << MSG::DEBUG << "SIZE OF ODIN MEP BUFFER : " << m_QueueOdinMEP.size() << endmsg; 
        int testsem = 0;
        sem_getvalue(&m_OdinCount, &testsem);
        msgLog << MSG::DEBUG << "Odin SEM before post = "<<testsem << endmsg;

        if(sem_post(&m_OdinCount)==-1)
        {
            ERRMSG(msgLog, "Posting on the semaphore");
            perror("sem_post"); 
            exit(errno);
        }

	if (pthread_mutex_unlock(&m_SyncMainOdin)) {
	    ERRMSG(msgLog, "Failed unlocking mutex");
	    return StatusCode::FAILURE;
	}

    }
    free(bufMEP);
    return StatusCode::SUCCESS;
}


StatusCode MEPInjector::getInfoFromOdinMEP() 
{

    /// XXX get all data from all banks, according to the pf, it means copy the complete mep (memcpy)   
    /// Set packing factor, L0ID, and all "global" variable according to the Odin MEP received
    struct MEPHdr *mh =
        (struct MEPHdr *) &(((u_int8_t *) (m_OdinMEP)->start())[20]);

    m_L0ID = mh->m_l0ID;
    m_PackingFactor = mh->m_nEvt;
    m_PartitionID = mh->m_partitionID;


    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::getHLTInfo() 
{
    static MsgStream msgLog(msgSvc(), name());    //Message stream for output
    StatusCode ret=StatusCode::RECOVERABLE;
    int retVal=0;
  
    int testsem = 0;
    sem_getvalue(&m_MEPReqCount, &testsem);
    msgLog << MSG::DEBUG << "HLT SEM = "<<testsem << endmsg;
 
//    while((retVal=sem_wait(&m_MEPReqCount))==-1 && errno == EINTR) continue;
    retVal=sem_wait(&m_MEPReqCount);
    if(retVal == -1) {
        perror("sem_wait");
        exit(errno);
    }
 
    if(m_InjState != RUNNING) {
        msgLog << MSG::INFO << "End of injection : Exiting getHLTInfo" << endmsg;
        return StatusCode::RECOVERABLE;
    }
    

    msgLog << MSG::DEBUG << "After sem wait" << endmsg; 

    if (pthread_mutex_lock(&m_SyncReqOdin)) {
        ERRMSG(msgLog, "Failed locking mutex");
        return StatusCode::FAILURE;
    }

    /// Select an HLT farm where to send the MEPs, round robin
    if (m_HLTReqs.size() > 0) {
	//std::map < unsigned int, unsigned int >::iterator ite =
	//std::max_element(m_HLTReqs.begin(), m_HLTReqs.end(),
	//		 value_comparer);
    
        if(m_HLTReqsIte == m_HLTReqs.end()) m_HLTReqsIte = m_HLTReqs.begin();
        while(m_HLTReqsIte != m_HLTReqs.end() && m_HLTReqsIte->second == 0) {
            m_HLTReqsIte++; 
        }
        if(m_HLTReqsIte == m_HLTReqs.end()) {
            msgLog << MSG::INFO << "No MEP Requests received" << endmsg;
        }
        else {
         
            m_HLTIPAddrTo = m_HLTReqsIte->first;

            --(m_HLTReqsIte->second);
            --m_CurMEPReqCredits;
            ++m_TotCreditConsumed;
            if(m_Debug) {
                msgLog << MSG::DEBUG << "Get HLT information" << endmsg; 
                msgLog << MSG::DEBUG << "	" << "Farm selected : " << MEPRxSys::dotted_addr(m_HLTIPAddrTo) << endmsg;
                msgLog << MSG::DEBUG << "      " << "Nb Reqs : " << m_HLTReqsIte->second << endmsg;  
                msgLog << MSG::DEBUG << "Credits consumed : " << m_TotCreditConsumed << endmsg;
            }
 
            m_HLTReqsIte++;
            ret = StatusCode::SUCCESS;
        }
    } 

    if (pthread_mutex_unlock(&m_SyncReqOdin)) {
        ERRMSG(msgLog, "Failed unlocking mutex");
        return StatusCode::FAILURE;
    }

    return ret;
}

StatusCode MEPInjector::getOdinInfo()
{

    StatusCode ret=StatusCode::RECOVERABLE;
    static MsgStream msgLog(msgSvc(), name());    //Message stream for output
    int retVal=0;

    int testsem = 0;
    sem_getvalue(&m_OdinCount, &testsem);
    msgLog << MSG::DEBUG << "Odin SEM = "<<testsem << endmsg;

//    while ( (retVal=sem_wait(&m_OdinCount)==-1) && errno == EINTR) continue;
    retVal=sem_wait(&m_OdinCount);
    if(retVal ==-1) {
        perror("sem_wait");
        exit(errno);
    }

    if(m_InjState != RUNNING) {
        msgLog << MSG::INFO << "End of injection : Exiting getOdinInfo" << endmsg;
        return StatusCode::RECOVERABLE;
    } 

    if (pthread_mutex_lock(&m_SyncMainOdin)) {
        ERRMSG(msgLog, "Failed locking mutex");
        return StatusCode::FAILURE;
    }
   
    
 
    /// Get the first Odin MEP from the queue
    if(m_QueueOdinMEP.empty()==false)
    {
        char *tmp = m_QueueOdinMEP.front(); 
        int *itmp = (int *) tmp; 
        memcpy(m_OdinData.data(), tmp, itmp[0]+4 ); 
        m_OdinMEP->setSize(itmp[0]);

        m_QueueOdinMEP.pop();
        free(tmp);

        //printMEP(m_OdinMEP, m_OdinMEP->size());

        MEPHdr *mephdr = (MEPHdr *) ( ((char *) m_OdinMEP) +  MEPEVENTOFFSET + IPHDRSZ);
        m_EvtID = mephdr->m_l0ID;
        m_L0ID= mephdr->m_l0ID;
        m_PartitionID = mephdr->m_partitionID;
        m_PackingFactor = mephdr->m_nEvt;

        if(m_Debug) { 
            msgLog << MSG::DEBUG << "Odin information read : " <<endmsg;
            msgLog << MSG::DEBUG << "	" << "EvtID = "<< m_EvtID << endmsg;
            msgLog << MSG::DEBUG << "	" << "PartitionID = "<< m_PartitionID << endmsg;
            msgLog << MSG::DEBUG << "	" << "PackingFactor = "<< m_PackingFactor << endmsg;
            msgLog << MSG::DEBUG << "	" << "L0ID = " << m_L0ID << endmsg;
        }


        ++m_TotOdinMEPTx; 
         
        msgLog << MSG::DEBUG << "NUMBER OF ODIN MEP MANAGED = " << m_TotOdinMEPTx << endmsg;

        ret = StatusCode::SUCCESS; 
    }
    else {
        m_OdinMEP->setSize(0);
    } 
    m_CurOdinMEPs = m_QueueOdinMEP.size(); 

    if (pthread_mutex_unlock(&m_SyncMainOdin)) {
        ERRMSG(msgLog, "Failed unlocking mutex");
        return StatusCode::FAILURE;
    }

    return ret;
}


StatusCode MEPInjector::error(const std::string & msg)
{
    static MsgStream msgLog(msgSvc(), name());
    msgLog << MSG::ERROR << msg << endmsg;
    return StatusCode::FAILURE;
}

void saveHisto()
{
    for (int bkType = RawBank::L0Calo; bkType < RawBank::LastType;
             ++bkType)
    {
        std::string str = "HISTOS/"+RawBank::typeName((RawBank::BankType)bkType);
        FILE *output = fopen(str.c_str(), "w");  
        
        fprintf(output, "NB EVENT = %d\n", BKSTATI);
        fprintf(output, "%s;SourceID;Count\n", RawBank::typeName((RawBank::BankType)bkType).c_str());   
        for(std::map<int, int>::iterator ite = BKSTATMAP[bkType].begin(); ite != BKSTATMAP[bkType].end(); ite ++) {
            fprintf(output, "%sSTAT;%d;%d\n", RawBank::typeName((RawBank::BankType)bkType).c_str(),ite->first,ite->second);   

        }
        fclose(output); 
    }
}


StatusCode MEPInjector::finalize()
{
    static MsgStream msgLog(msgSvc(), name());

    msgLog << MSG::ALWAYS << __PRETTY_FUNCTION__ << endmsg;

    if(m_Debug) close(m_CreditsFD);


    StatusCode sc;

    MEPEvent *me = NULL;

    m_InjState = NOT_READY;

    m_ManagerStop = true;

    deleteConsumers();

    /// No more need of synchronisation to access to the queues/maps, min value = 1
    /// and they should exit  
    if(sem_post(&m_MEPReqCount)==-1)
    {
        ERRMSG(msgLog, "Posting on the semaphore");
        perror("sem_post");
        exit(errno);
    } 
    if(sem_post(&m_OdinCount)==-1) 
    {
        ERRMSG(msgLog, "Posting on the semaphore");
        perror("sem_post");
        exit(errno);
    }
    
/*
    if(pthread_kill(m_InjectorProcessing, SIGTERM))
    {
        ERRMSG(msgLog, "Kill SIGTERM InjectorProcessing");
        return StatusCode::FAILURE;
    }
*/
/*
    if(pthread_join(m_InjectorProcessing, NULL))
    {
       ERRMSG(msgLog, "injectorProcessing thread join");
       return StatusCode::FAILURE;
    }
*/

    ///XXX Do something in Automode to empty the buffer? should be done automatically as in automode it should loop in the run procedure 

    /// Once the file is completely read, it is possible that we have to send a MEP which has not reached the Packing Factor
    /// Just do as usual, the encodeMEP method deal with that 
    /// and will set the correct number of event 
    /// Done only if the file was completely read, if finalize cause of dim command, 
    /// do nothing, just close (if i find how to use this dim stuff)
    if(finalSent) 
    {      
        /// Use the first stored MEP to find the last packing factor  
        std::map<unsigned long, MEPEvent *>::iterator iteTell1ID = m_MapTell1MEPs.begin(); ///Hope that the first ip is not an OT ... 
        if(iteTell1ID != m_MapTell1MEPs.end())
        { 
            me = (iteTell1ID->second);
            if(me != NULL)
            {      
                MEPHdr *mephdr = (MEPHdr *) ( ((char *) me) +  MEPEVENTOFFSET + IPHDRSZ);
                m_PackingFactor = mephdr->m_nEvt;
        
                sc = sendMEP(iteTell1ID->first, me);
                if (sc.isFailure())
                    return sc;
            } ///end if
        
            for(++iteTell1ID; iteTell1ID != m_MapTell1MEPs.end(); ++iteTell1ID)
            {
                me = (iteTell1ID->second);
                if(me != NULL)
                {
   	             sc = sendMEP(iteTell1ID->first, me);
   	             if (sc.isFailure())
	                return sc;
                } ///end if
            } ///end for 
        } /// end if
        if (!m_AutoMode) {
            ///XXX Here, modify the packing factor of the Odin MEP in order for the HLT to accept the incomplete MEPS
            /// else you get that ERROR Bad packet from ottella15-d1 [mismatch in packing factor]
            /// First send normal MEPS, get their pf and send the odin mep with the pf

            u_int32_t addrFrom = m_BitOdinIPAddr +  ((32<<24)&0xff000000);

            MEPHdr *mephdr = (MEPHdr *) ( ((char *) m_OdinMEP) +  MEPEVENTOFFSET + IPHDRSZ);
            
            m_OdinMEP->setSize(m_OdinMEP->size() - ODFRAGSZ * (mephdr->m_nEvt - m_PackingFactor)); 
            mephdr->m_totLen-=(ODFRAGSZ * (mephdr->m_nEvt - m_PackingFactor)) ;
            mephdr->m_nEvt = m_PackingFactor;

            sc = sendMEP(addrFrom, m_OdinMEP);
            if(sc.isFailure()) {
                ERRMSG(msgLog, "Could not send Odin MEP");
                return StatusCode::FAILURE;
            }
            msgLog << MSG::DEBUG << "Odin MEP sent";
        }
 
        m_TotEvtsSent += m_PackingFactor; 
    } /// end if finalSent
    if(! m_AutoMode) {
/*
        if(pthread_kill(m_ThreadOdinMEPManager, SIGTERM))
        {
            ERRMSG(msgLog, "Kill SIGTERM OdinMEPManager");
            return StatusCode::FAILURE;
        } 
        if(pthread_kill(m_ThreadMEPReqManager, SIGTERM))
        {
            ERRMSG(msgLog, "Kill SIGTERM MEPReqManager");
            return StatusCode::FAILURE;
        }
*/
        if(pthread_join(m_ThreadOdinMEPManager, NULL))
        {
            ERRMSG(msgLog, "OdinMEPManager thread join");
            return StatusCode::FAILURE;
        }
        if(pthread_join(m_ThreadMEPReqManager, NULL)) 
        {
            ERRMSG(msgLog, "MEPReqManager thread join");
            return StatusCode::FAILURE;
        }


        ///XXX Should be tested
        close(m_FromOdinSock);
        close(m_ToOdinSock); 
        close(m_FromHLTSock);

        sem_destroy(&m_OdinCount);
        sem_destroy(&m_MEPReqCount);

    }

    close(m_ToHLTSock);

    msgLog << MSG::DEBUG <<"Injector finalized"<<endmsg;

 
    if(m_IncidentSvc) {
        m_IncidentSvc->removeListener(this);
        m_IncidentSvc->release();
        m_IncidentSvc = 0;
    }
    
    if (m_MonSvc) {
        m_MonSvc->undeclareAll(this);
        m_MonSvc->release();
        m_MonSvc = 0;
    }

    msgLog << MSG::INFO << "Finalization done" << endmsg;
    msgLog << MSG::INFO << "A few stats" << endmsg;
    msgLog << MSG::INFO << "Total of MEPs sent : " << m_TotMEPsTx << endmsg;
    msgLog << MSG::INFO << "Total of MEP Requests received : " << m_TotMEPReqRx << endmsg;

    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::run() {
    static MsgStream msgLog(msgSvc(), name());
    m_IncidentSvc->addListener(this, "DAQ_CANCEL", 0, true, true);

/*
    while(m_InjState != RUNNING){
        switch(m_InjState) {
        case STOPPED:
        case NOT_READY:
            msgLog << MSG::DEBUG << "Exiting from reading loop" << endmsg;
            return StatusCode::SUCCESS;
        case READY:
            MEPRxSys::microsleep(100000); // 100 ms
            break;
        default: continue;
        }
    }
*/

    msgLog << MSG::ALWAYS << __PRETTY_FUNCTION__ << endmsg;

    m_InjState = RUNNING;

    StatusCode sc = StatusCode::SUCCESS; 
    while(sc.isSuccess() && m_InjState == RUNNING)
    {
         sc=injectorProcessing();
    }
/*
    if(m_IncidentSvc) {
        m_IncidentSvc->removeListener(this);
        m_IncidentSvc->release();
        m_IncidentSvc = 0;
    }
*/
    msgLog << MSG::INFO << "End of injection : End of run" << endmsg;

    return StatusCode::SUCCESS;
}


template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
} 


void MEPInjector::publishCounters()
{
    PUBCNT(TotBytesTx,         "Total amount of bytes sent to the farms");
    PUBCNT(TotMEPReqRx,        "Total MEP requests received from farms");
    PUBCNT(TotMEPReqPktRx,     "Total MEP request packets received from farms");  
    PUBCNT(TotOdinMEPRx,       "Total Odin MEPs received");
    PUBCNT(TotMEPReqTx,        "Total MEP requests sent to Odin");
    PUBCNT(TotMEPReqPktTx,     "Total MEP request packets sent to Odin");
    PUBCNT(TotEvtsRead,        "Total read events");
    PUBCNT(TotEvtsSent,        "Total sent events");
    PUBCNT(TotElapsedTime,     "Total time since last initialization");
    PUBCNT(TotMEPsTx,          "Total sent MEPs");   
 
    PUBCNT(TotCreditConsumed,  "Total of MEP Requests managed"); 
    PUBCNT(CurMEPReqCredits,   "MEP credits stored for each farm node");
 
    PUBCNT(CurOdinMEPs,        "Current number of Odin MEP in reception buffer");
 
//  PUBCNT(totRxOct,           "Total received bytes");
//  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
}

void MEPInjector::clearCounters() {
//  resetCounters(m_rxOct, m_nSrc);
//  m_totMEPReq          = 0;

    m_CurOdinMEPs = 0;
    m_CurMEPReqCredits = 0 ;

    m_TotBytesTx = 0;
    m_TotMEPReqRx = 0;
    m_TotMEPReqPktRx = 0;
    m_TotOdinMEPRx = 0;
    m_TotMEPReqTx = 0;
    m_TotCreditConsumed = 0;
    m_TotMEPReqPktTx = 0;
    m_TotOdinMEPTx = 0;
    m_TotEvtsRead = 0;
    m_TotEvtsSent = 0;
    m_TotElapsedTime = 0;
    m_TotMEPsTx = 0;
    gettimeofday(&m_InitTime, NULL);
}

int MEPInjector::setupCounters() {
  MsgStream msgLog(msgSvc(),name());
  clearCounters();
  publishCounters();
  
  // create cstring for source names
  std::string all_names = "";
  for (unsigned i = 0; i < m_Tell1Boards.size(); ++i)  all_names = all_names + '\0' + m_Tell1Boards[i];
  if (!(m_AllNames = new char[all_names.size()+1]))
    return 1;
   ::memcpy(m_AllNames, (const char *) all_names.data(), all_names.size() +1);
  
  m_MonSvc->declareInfo("srcName", "C:", m_AllNames, sizeof(m_Tell1Boards), "Source IP names", this);
//  msgLog << MSG::INFO << all_names << ", " << m_Tell1Boards.size() << endmsg;
  return 0;
}



void MEPInjector::handle(const Incident& inc) {
  MsgStream msgLog(msgSvc(),name());
  msgLog << MSG::ALWAYS << __PRETTY_FUNCTION__ << endmsg;
  msgLog << MSG::ALWAYS << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  { //but if I remember well the PVSS interface was leading the injector in not ready no ? XXX check this once Alba is back 
      m_InjState = STOPPED;
      /// No more need of synchronisation to access to the queues/maps, min value = 1
      /// and they should exit  
      if(sem_post(&m_MEPReqCount)==-1)
      {
          ERRMSG(msgLog, "Posting on the semaphore");
          perror("sem_post");
          exit(errno);
      }
      if(sem_post(&m_OdinCount)==-1)
      {
          ERRMSG(msgLog, "Posting on the semaphore");
          perror("sem_post");
          exit(errno);
      }

  }
  else if (inc.type() == "DAQ_ENABLE") {
      m_InjState = RUNNING;
  }
}

// These startup functions are helper functions. They have normal C linkage.
void *MEPReqMgrStartup(void *object)
{
    MEPInjector *injector = (MEPInjector *) object;

    printf("THREAD : Running thread object in a new thread : MEP Request Manager\n");

    StatusCode sc = injector->manageMEPRequest();
    //printf("Deleting object\n");
    //delete tgtObject;
    if (sc.isFailure())
	return (void *) -1;
    else
	return (void *) 0;
}

void *OdinMEPMgrStartup(void *object)
{
    MEPInjector *injector = (MEPInjector *) object;

    printf("THREAD : Running thread object in a new thread : Odin MEP Manager\n");

    StatusCode sc = injector->manageOdinMEP();
    //printf("Deleting object\n");
    //delete tgtObject;
    if (sc.isFailure())
	return (void *) -1;
    else
	return (void *) 0;
}

void *injectorProcessingStartup(void *object)
{
    MEPInjector *injector = (MEPInjector *) object;

    printf("THREAD : Running thread object in a new thread : Injector Process\n");

    StatusCode sc = injector->injectorProcessing();
    //printf("Deleting object\n");
    //delete tgtObject;
    if (sc.isFailure())
        return (void *) -1;
    else
        return (void *) 0;

}


#endif //ifndef _WIN32
