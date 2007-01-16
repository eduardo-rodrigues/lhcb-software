#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "MBM/Consumer.h"

namespace {
   static void help()  {
    ::lib_rtl_printf("mbm_cons -opt [-opt]\n");
    ::lib_rtl_printf("    -n=<name>              buffer member name\n");
    ::lib_rtl_printf("    -s=<number>            sleep interval between events [milli seconds]\n");
    ::lib_rtl_printf("    -b=<name>              Buffer identifier \n");
    ::lib_rtl_printf("    -p(artition)=<number>  Partition ID\n");
    ::lib_rtl_printf("    -q                     Quiet mode (do not print trigger number mismatch)\n");
    ::lib_rtl_printf("    -a(synchronous)        Asynchonous mode (default is synchronous)\n");
    ::lib_rtl_printf("    -o(ne)                 ONE consumer (REQ type ONE)\n");
    ::lib_rtl_printf("    -u(sermode)            USER mode (SPY) consumer\n");
  }
  struct Cons : public MBM::Consumer  {
    int m_nbad, m_trnumber, m_quiet, m_sleep, m_check;
    Cons(const std::string& b,const std::string& n,int pid,int sleep,bool q, bool chk) 
    : MBM::Consumer(b,n,pid), m_nbad(0), m_trnumber(-1), m_quiet(q), m_sleep(sleep), m_check(chk?1:0) {
    }
    int eventAction() {
      const MBM::EventDesc& e = event();
      if ( m_trnumber == -1 )  {
        m_trnumber = *e.data;
      }
      else if( m_trnumber != *e.data ) {
        if ( !m_quiet && m_check )  {
          ::lib_rtl_printf("======= Mismatch [%d] found %d %d [%p]\n",
            ++m_nbad, m_trnumber, *e.data, (void*)e.data);
        }
        m_trnumber = *e.data;
      }
      m_trnumber++;
      if ( m_sleep ) lib_rtl_sleep(m_sleep);
      return Consumer::eventAction();
    }
  };
}

extern "C" int mbm_cons(int argc,char **argv) {  
  RTL::CLI cli(argc, argv, help);
  std::string name = "consumer", buffer="0";
  int partID = 0x103, sleep = 0;
  bool one   = cli.getopt("one",1) != 0;
  bool quiet = cli.getopt("quiet",1) != 0;
  bool async = cli.getopt("asynchronous",1) != 0;
  bool user  = cli.getopt("usermode",1) != 0;
  unsigned int vetomask[4] = {0,0,0,0};
  unsigned int trmask[4]   = {~0x0,~0x0,~0x0,~0x0};

  cli.getopt("name",1,name);
  cli.getopt("buffer",1,buffer);
  cli.getopt("sleep",1,sleep);
  cli.getopt("partitionid",1,partID);
  int status = wtc_init();
  if( status != WT_SUCCESS ) exit(status);
  ::lib_rtl_printf("Consumer \"%s\" (pid:%d) running in buffer:\"%s\"\n",
    name.c_str(),Cons::pid(),buffer.c_str());

  Cons c(buffer,name,partID,sleep,quiet,!(one||user));
  if ( async )   {
    ::lib_rtl_printf("  Running in asynchronous mode.\n");
    c.setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
  }
  if ( one ) {
    c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ONE,BM_FREQ_PERC,100.);
    ::lib_rtl_printf("  Event request type is BM_REQ_ONE.\n");
  }
  else if ( user ) {
    c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_USER,BM_FREQ_PERC,100.);
    ::lib_rtl_printf("  Event request type is BM_REQ_USER.\n");
  }
  else {
    c.addRequest(1,trmask,vetomask,BM_MASK_ANY,BM_REQ_ALL,BM_FREQ_PERC,100.);
    ::lib_rtl_printf("  Event request type is BM_REQ_ALL.\n");
  }
  return c.run();
}
