#ifndef __NetworkChannel_H__
#define __NetworkChannel_H__

#include "NET/defs.h"
#if defined(_VMS) || defined(VMS)                        /* _VMS   */
typedef unsigned short __NetworkChannel__;               /* _VMS   */
#else  // _OSK, WIN32 and linux
typedef int            __NetworkChannel__;
#ifdef _WIN32
typedef int socklen_t;
#endif
#endif

typedef unsigned short     __NetworkPort__;
typedef unsigned short     __NetworkFamily__;
typedef struct sockaddr_in __NetworkAddress__;
typedef struct in_addr     __NetworkSubAddress__;

#ifdef __cplusplus
#include "NET/defs.h"
/// Definition of the general purpose NetworkChannel data structure
/** @class NetworkChannel NetworkChannel.h NET/NetworkChannel.h
*
* General purpose networking object, which allows sending and receiving with timeouts
*/
class NetworkChannel  {
public:
  //@Man: Public type definitions:
  /// Family   == __NetworkFamily__;
  typedef __NetworkFamily__  Family;
  /// Channel  == __NetworkChannel__;
  typedef __NetworkChannel__ Channel;
  /// Port     == __NetworkPort__;
  typedef __NetworkPort__    Port;
  /// Address  == __NetworkAddress__;
  typedef __NetworkAddress__ Address;
protected:
  //@Man: protected member variables
  /// Boolean indicating that the timer fired and the I/O go cancelled
  bool m_bCancel;
  /// Boolean indicating the validity of the connection after initialisation
  bool m_bValid;
  /// Channel to hold the socket connection 
  Channel  m_socket;
  /// Port number
  Port     m_port;
  /// Alarm identifier
  unsigned int m_alarmID;
  /// Buffer to store status code
  unsigned int m_errno;
  //@Man: protected member functions
  /// Start timer before receiving data, if you don't want to wait forever
  void StartTimer( int tmo );    
  /// Cancel timer
  void StopTimer ();
public:
  //@Man: public member functions
  /// Default constructor
  NetworkChannel();
  /// Virtual destructor
  virtual ~NetworkChannel();
  /// AST called on timeout for receiving data on socket
  static int TmoAST(void* par);
  /// Retieve cancel flag
  bool _IsCancelled()   const {
    return m_bCancel;
  }
  /// Indicate if the connection is valid
  bool _IsValid() const  {
    return m_bValid;
  }
  /// Return errno of socket
  unsigned int _Error() const   {
    return m_errno;
  }
  /// Return channel/path number
  Channel _Channel() const  {
    return m_socket;
  }
  /// Return Error string
  const char* _ErrMsg();
  /// Abstract method to cancel eventually pending I/O requests
  virtual int _Cancel() = 0;
  /// Abstract method to Bind Address (Acceptor)
  virtual int _Bind   (const Address& addr, int con_pend = 5) = 0;
  /// Abstract method to Connect to target
  virtual int _Connect(const Address& addr, int tmo = 0) = 0;
  /// Abstract method: send data on network
  virtual int _Send   (void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0) = 0;
  /// Abstract method: receive data from network.
  virtual int _Recv   (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0) = 0;
};
#endif  /* __cplusplus           */
#endif   /* __NetworkChannel_H__  */
