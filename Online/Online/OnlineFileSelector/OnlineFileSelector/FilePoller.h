#ifndef ICHALKIA_FILEPOLLER_H 
#define ICHALKIA_FILEPOLLER_H 1

// Include files
#include "GaudiOnline/OnlineService.h"
#include "IHandleListenerSvc.h"
#include "IAlarmHandler.h"
#include "RTL/rtl.h"
#include <deque>
#include "dim.hxx"


namespace LHCb  {
  
  /** @class FilePoller FilePoller.h ichalkia/FilePoller.h
   *  
   *
   *  @author Ioannis Chalkiadakis
   *  @date   2014-01-23
   */
  class FilePoller :  virtual public DimTimer,
                      public OnlineService,
                      virtual public IHandleListenerSvc,
                      virtual public IAlarmHandler

  {

  public:

    /// Standard constructor.
    FilePoller(const std::string& nam, ISvcLocator* svc);
  
    /// Standard destructor.
    ~FilePoller();

    // IInterface, queryInterface implementation.
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
    /// IOnlineService, start the timer.
    virtual StatusCode start();

    /// IOnlineService, stop the timer.
    virtual StatusCode stop();
  
    ///IOnlineService initialize.
    virtual StatusCode initialize();

    ///IOnlineService finalize.
    virtual StatusCode finalize();
  
    /// Accessor methods.
    void addTofileNames(const std::string& );
    std::string remFromfileNames();
 
    /// IDimTimer timer handler method.
    virtual void timerHandler();

    /// Poll method.
    StatusCode poller(const std::string scan_path);

    /// IHandleListenerSvc add a listener to the queue.
    virtual StatusCode addListener(IAlertSvc* Listener);

    /// IHandleListenerSvc remove a listener from the queue.
    virtual StatusCode remListener(IAlertSvc* Listener);

    /// IHandleListenerSvc show the listeners waiting.
    virtual const StatusCode showListeners();  
   
    /// IHandlerListenerSvc statusReport.
    virtual StatusCode statusReport(StatusCode status);
    
    /// IAlarmHandler error response.
    virtual const StatusCode issueAlarm(const std::string& msg);
  

  private:


    ///The directory to be scanned.
    std::string m_scanDirectory;

    ///The period of the scanning process.
    int m_alrmTime;

    ///The names of the files found in the scanned directory.
    std::deque<std::string>  m_fileNames;

    ///The listeners waiting for the files.
    std::deque<IAlertSvc*> m_fileListeners;

    lib_rtl_lock_t m_listenerLock;

  };
}


#endif // ICHALKIA_FILEPOLLER_H
