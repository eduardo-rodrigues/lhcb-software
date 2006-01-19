// ----------------------------------------------------------------------------
//
//                                      M.Frank
// ----------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "TAN/taninterface.h"

#define MAXTASKS 20
#ifdef _VMS
#define lib_signal(x) lib$signal(x)
#else
#define lib_signal(x) {printf("%s\n",strerror(x)); }
#endif

extern "C" int rtl_tan_inquire_test ( int argc, char* argv[] )  {
  TanInterface& interf = TanInterface::Instance();
  int status, num_inq = 0, tot_inq = 0, inc = 500, succ = 0, fail = 0, notfnd = 0;
  bool continuous = false, quiet = false;
  char *c, buff[64], host[32];
  sockaddr_in addr;
  gethostname(host,sizeof(host));
  while( --argc > 0 )      {
    if ( *(c = *++argv) == '-' )   {
      switch( *++c | 0x20 )  {
   case 'q': quiet = true;         break;
   case 'c': continuous = true;      break;
   case 'n': strncpy(host,c+2,sizeof(host));   break;
   default:
     printf("Usage : inquire -opt <opts>\n");
     printf("        -c           contious mode\n");
     printf("        -n           supply host name; default:%s\n",host);
     printf("        -q           quiet mode(statistics only\n");
     printf("        -?, -h       get this help\n");
     exit(0);
      }
    }
  }
#define _PRINT  succ++; if ( !quiet ) printf
#define _PRINTERROR(buff,status)  {             \
  if ( status == TAN_SS_TASKNOTFOUND ) notfnd++;          \
  if ( !quiet || status != TAN_SS_TASKNOTFOUND )  {       \
  printf("GetAddressByName(%-24s) %4d ", buff, status);   \
  lib_signal(status);                    \
  } fail++;                      \
}
  if ( interf.Status() == TAN_SS_SUCCESS ) {
    /// Check non-exisiting task
    sprintf(buff,"%s::IDIOTIC",host);
    status = tan_get_address_by_name(buff,&addr);
    _PRINTERROR(buff,status);
    status = tan_dump_dbase ( host );
    time_t start = time(0);
    while ( 1 )    {
      if ( num_inq > inc )  {
        tot_inq += num_inq;
        num_inq = 0;
        printf("->%-4d sec<-  Queries:%6d Success:%-4d TaskNotFound:%-4d Failure:%-4d.\n",
          time(0) - start, tot_inq, succ, notfnd, fail);
      }
      for ( int i = 0; i < MAXTASKS; i++ )         {
        int nalias = 0;
        for ( int j = 0; j < i; j++ )           {
          // Now check the aliases
          sprintf(buff,"%s::MYTASK_%02d_%02d",host,i,j);
          status = tan_get_address_by_name (buff,&addr);
          num_inq++;
          if ( status == TAN_SS_SUCCESS )  {
            _PRINT ("GetAddressByName: %s Port:%04X Fam:%1X Saddr:%08X (%s)\n",
              buff, addr.sin_port, addr.sin_family, addr.sin_addr.s_addr,
              inet_ntoa(addr.sin_addr) );
          }
          else {
            nalias++;
            _PRINTERROR(buff,status);
          }
        }
        sprintf(buff,"%s::MYTASK_%02d",host,i);
        status = tan_get_address_by_name (buff,&addr);
        num_inq++;
        if ( status == TAN_SS_SUCCESS )  {
          _PRINT ("GetAddressByName: %s Port:%04X Fam:%1X Saddr:%08X (%s)\n",
            buff, addr.sin_port, addr.sin_family, addr.sin_addr.s_addr,
            inet_ntoa(addr.sin_addr) );
        }
        else  {
          _PRINTERROR(buff,status);
        }
        if ( !continuous ) return 0x1;   // Just stop if non continuous
      }
    }
  }
  else  {
    _PRINTERROR("Error! Status:", interf.Status());
  }
  return 0x1;
}
