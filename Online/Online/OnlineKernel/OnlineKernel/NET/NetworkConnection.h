#ifndef __NETWORKCONNECTION__H__
#define __NETWORKCONNECTION__H__
#include "RTL/rtl.h"
#include "CPP/SmartObject.h"
class BasicRequest;
class NetworkChannel;
class NetworkAddress  {
public:
    NetworkAddress() {
    }
    virtual ~NetworkAddress() {
    }
    virtual NetworkAddress& operator = (const NetworkAddress& copy)  {
        return *this;
    }
};

/// Defintion of the basic interface of ANY networking connection.
/**
    {\Large{\bf Class NetworkConnection}}
    Basic interface of ANY networking connection.

 This is a tricky: NetworkAddress should not be used
 as a variable type at all!
 Instead use the defined variable type of the specific
 connection class:
 e.g.   UdpConnection::Address  new_address;
 The wrapped address type contains the required structure
 and only this structure, i.e. the meory mapping and
 size is identical.

*/
class NetworkConnection  : public SmartObject<NetworkConnection> {
public:
  //@Man: Class specific enumerations and typedefs
  /// IOSB type definition
  typedef _IOSB          Iosb;
  /// Status enum of the networking connection
  enum NetConnectionStatus {
    NETCONNECTION_ERROR = 0,
    NETCONNECTION_SUCCESS
  };
protected:
public:
  //@Man: Public member functions
  /// Standard constructor
  NetworkConnection() {
  }
  /// Standard destructor
  virtual ~NetworkConnection() {
  }
  /// Address the connection points to (may be invalid)
  virtual const NetworkAddress& _Address () const = 0;
  /// Virtual method to manipulate the channel
  virtual NetworkChannel& _SendChannel ()  = 0;
  /// Virtual method to manipulate the channel
  virtual NetworkChannel& _RecvChannel ()  = 0;
  /// Virtual method to receive data.
  virtual int Receive  (BasicRequest* req, NetworkAddress& addr, Iosb& ios) = 0;
  /// Virtual method to send data
  virtual int Send     (BasicRequest* req, NetworkAddress& addr, Iosb& ios) = 0;
};

#endif
