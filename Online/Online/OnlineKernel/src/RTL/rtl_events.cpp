#include "rtl_internal.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <cerrno>
#include <fcntl.h>
#include <cstdio>
#include <iostream>

//inline int rtl_printf(const char* fmt, ...)  {
//  va_list args;
//  va_start(args, fmt);
//  return vprintf(fmt, args);
//}
inline int rtl_printf(const char* , ...)  {
  return 1;
}

using namespace RTL;
static std::auto_ptr<lib_rtl_thread_map_t> s_thrMap(new lib_rtl_thread_map_t);
lib_rtl_thread_map_t& RTL::waitEventThreads() {
  return *s_thrMap.get();
}

static std::auto_ptr<lib_rtl_event_map_t> s_evtMap(new lib_rtl_event_map_t);
lib_rtl_event_map_t& RTL::allEventFlags() {
  return *s_evtMap.get();
}

extern "C" int lib_rtl_event_exithandler() {
  if ( s_evtMap.get() )  {
    lib_rtl_event_map_t m = allEventFlags();
    lib_rtl_event_map_t::iterator i = m.begin();
    for( ; i != m.end(); ++i ) {
      // ::lib_rtl_printf("Deleting event flag:%s\n",(*i).first.c_str());
      ::lib_rtl_delete_event((*i).second);
    }
    delete s_evtMap.release();
  }
  return 1;
}

/// Create named event for inter process communication
int lib_rtl_create_event (const char* name, lib_rtl_event_t* event_flag)    {
  std::auto_ptr<rtl_event> h(new rtl_event);
  ::memset(h->name,0,sizeof(h->name));
  bool created = false;
  if ( name )  {
    ::sprintf(h->name,"/%s",name);
    h->name[sizeof(h->name)-1] = 0;
  }
#if defined(USE_PTHREADS)
  h->handle = h->name[0] ? ::sem_open(h->name, O_CREAT|O_EXCL, 0777, 1) : &h->handle2;
  if (h->name[0] && !h->handle) {
    h->handle = ::sem_open(h->name, O_CREAT, 0777, 1);
    if ( !h->handle ) {
      ::perror("SEVERE: sem_open: ");
      return 0;
    }
  }
  else if ( h->name[0] ) {
    created = true;
  }
  if ( h->name[0] ) {
      std::string nn="/dev/shm/sem.";
      nn+=name;
      int scc = ::chmod(nn.c_str(),0666);
      // ::lib_rtl_printf("Settint protection of %s\n",nn.c_str());
      if ( 0 != scc ) ::perror("chmod.");
  }
  int sc = h->handle ? ::sem_init(h->handle, h->name[0] ? 1 : 0, 1) : (errno=EBADR); 
  if ( sc != 0 )  {
    h->handle = 0;
  }
#elif defined(_WIN32)
  h->handle = ::CreateEvent(0,TRUE,FALSE,name ? h->name : 0);
  if ( h->handle != 0 && ::GetLastError() == ERROR_ALREADY_EXISTS )   {
    ::CloseHandle(h->handle);
    h->handle = ::OpenEvent(EVENT_ALL_ACCESS,FALSE,name ? h->name : 0);
  }
#endif
  if ( h->handle == 0 )  {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to create %s event flag [%s]", 
      name ? name : "<unnamed>");
    *event_flag = 0;
    return 0;
  }
  if ( name && created )  {
    RTL::allEventFlags().insert(std::make_pair(h->name,h.get()));
  }
  *event_flag = h.release();
  return 1;
}

int lib_rtl_delete_event(lib_rtl_event_t handle)   {
  if ( handle )  {
    std::auto_ptr<rtl_event> h(handle);
#if defined(USE_PTHREADS)
    if ( h->name[0] ) {
      ::sem_close(h->handle);
      ::sem_unlink(h->name);
    }
    else  {
      ::sem_destroy(h->handle);
    }
#elif defined(_WIN32)
    HRESULT sc = ::CloseHandle(h->handle);
    if ( sc != S_OK )  {
      // return ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to delete event flag \"%s\" 0x%08X", h->name, h->handle);
    }
#endif
    if ( h->name[0] ) {
      lib_rtl_event_map_t& m = allEventFlags();
      lib_rtl_event_map_t::iterator i = m.find(h->name);
      if ( i != m.end() ) {
        m.erase(i);
      }
    }
    return 1;
  }
  ::lib_rtl_signal_message(LIB_RTL_DEFAULT,"Failed to delete event flag [UNKNOWN EVENT FLAG]");
  return 0;
}

int lib_rtl_clear_event(lib_rtl_event_t h) {
  if ( h )  {
#if defined(USE_PTHREADS)
    ::sem_trywait(h->handle);
#elif defined(_WIN32)
    if ( ::ResetEvent(h->handle) != 0 )
#endif
    {
      return 1;
    }
    ::lib_rtl_signal_message(LIB_RTL_OS,"Failed to clear event flag 0x%08X", h->handle);
    return 0;
  }
  return 1;
}

int lib_rtl_wait_for_event(lib_rtl_event_t h)    {
  if ( h )  {
#if defined(USE_PTHREADS)
    if ( 0 == ::sem_wait(h->handle) )
#elif defined(_WIN32)
    if ( ::WaitForSingleObjectEx(h->handle,INFINITE, TRUE) == WAIT_OBJECT_0 )  
#endif
      return 1;
  }
  return 0;
}

int lib_rtl_timedwait_for_event(lib_rtl_event_t h, int milliseconds)    {
  if ( h )  {
#if defined(USE_PTHREADS)
    timespec sp;
    ::clock_gettime(CLOCK_REALTIME, &sp);    
    milliseconds += sp.tv_nsec/1000000;
    sp.tv_sec  += milliseconds/1000;
    sp.tv_nsec  = (milliseconds%1000)*1000000;
    int sc = milliseconds==LIB_RTL_INFINITE 
      ? ::sem_wait(h->handle) 
      : ::sem_timedwait(h->handle, &sp);
    if ( sc != 0 && errno == ETIMEDOUT ) sc = 0;
    if ( sc == 0 )
#elif defined(_WIN32)
    DWORD diff = (milliseconds>0) ? milliseconds : INFINITE;
    if ( ::WaitForSingleObjectEx(h->handle,diff, TRUE) == WAIT_OBJECT_0 )  
#endif
      return 1;
  }
  return 0;
}

int lib_rtl_set_event(lib_rtl_event_t h)   {
  if ( h )  {
#if defined(USE_PTHREADS)
    int sc = ::sem_post(h->handle);
#elif defined(_WIN32)
    int sc = ::SetEvent(h->handle)==0 ? -1 : 0;
#endif
    if ( sc == 0 )
      return 1;
  }
  return 0;
}

int lib_rtl_set_global_event(const char* name)   {
  typedef std::map<std::string,lib_rtl_event_t> event_map;
  static event_map* events = new event_map;
  event_map::const_iterator i = events->find(std::string(name));
  lib_rtl_event_t h;
  if ( i == events->end() ) {
    int sc = ::lib_rtl_create_event(name, &h);
    if ( sc == 1 ) {
      events->insert(event_map::value_type(name,h));
    }
  }
  else {
    h = (*i).second;
  }
  return lib_rtl_set_event(h);
}

int lib_rtl_wait_event_a_call(void* param)  {
  lib_rtl_action* pars = (lib_rtl_action*)param;
  while(pars->flag)  {
    try {
      ::rtl_printf("wait...\n");
      ::lib_rtl_wait_for_event(pars->flag);
      ::lib_rtl_clear_event(pars->flag);
      if ( pars->action )  {
        ::rtl_printf("action...\n");
        (*pars->action)(pars->param);
        ::rtl_printf("action...done.\n");
      }
    }
    catch(...) {
      ::rtl_printf("Exception!!!\n");
    }
  }
  delete pars;
  return 1;
}

/// Wait asynchronously for an event flag
int lib_rtl_wait_for_event_a(lib_rtl_event_t flag, lib_rtl_thread_routine_t action, void* param)  {
  int sc = 0;
  if ( waitEventThreads().find(flag) == waitEventThreads().end() )  {
    lib_rtl_action* act = new lib_rtl_action;
    act->action = action;
    act->param  = param;
    act->flag   = flag;
    lib_rtl_thread_t thread;
    sc = ::lib_rtl_start_thread(lib_rtl_wait_event_a_call, act, &thread);
    if ( !lib_rtl_is_success(sc) )  {
      ::lib_rtl_signal_message(0,"Failed to manipulate asynchronous wait event thread");
      return 0;
    }
    waitEventThreads().insert(std::make_pair(flag,thread));
  }
  ::rtl_printf("Set event...\n");
  ::lib_rtl_set_event(flag);
  return 1;
}

static void help_set_event()  {
  ::lib_rtl_printf("rtl_set_event -opt [-opt]\n");
  ::lib_rtl_printf("    -n=<name>      Event flag name \n");
}
extern "C" int rtl_set_event(int argc, char** argv)  {
  lib_rtl_event_t flag;
  std::string name;
  RTL::CLI cli(argc, argv, help_set_event);
  cli.getopt("name",1,name);
  int sc = ::lib_rtl_create_event(name.c_str(), &flag);
  if ( !lib_rtl_is_success(sc) )  {
    return sc;
  }
  sc = ::lib_rtl_set_global_event(name.c_str());
  if ( !lib_rtl_is_success(sc) )  {
    return sc;
  }
  return sc;
}
