#ifndef __TcpCONNECTION__H__
#define __TcpCONNECTION__H__

#include "NET/defs.h"
//======================================================================
//
// This is a tricky: NetworkAddress should not be used
// as a variable type at all!
// Instead use the defined variable type of the specific
// connection class:
// e.g.   TcpConnection::Address  new_address;
// The wrapped address type contains the required structure
// and only this structure, i.e. the meory mapping and
// size is identical. Obviously socket calls should not be 
// exported.
//
//======================================================================
//
#include <cstring>
#include "NET/NetworkConnection.h"
#include "NET/TcpNetworkChannel.h"
//
/// Definition of the NetworkAddress data structure for TCP sockets
/** 
        {\Large{\bf Class NetworkAddress}}

 Small wrapper containing the inet socket address.
*/
class TcpNetworkAddress : public NetworkAddress {
  public:
  //@Man: Class specific enumerations and typedefs
  /// Address data type
  typedef TcpNetworkChannel::Address Data;
  //@Man: Public member variables
  /// Structure holding an inet address
  Data _addr;
  /// Name of network connection host
  char         _cHost[64];
  //@Man: Public member functions
  /// Automatic cast to the data object
  operator Data& () {
      return _addr;
  }
  /// Retrieve Name of Network Host
  const char* HostName() const {
    return _cHost;
  }
  /// Copy operator
  virtual NetworkAddress& operator = (const NetworkAddress& copy) {
    TcpNetworkAddress* cp = (TcpNetworkAddress*)&copy;
    memcpy(_cHost, cp->_cHost,sizeof(_cHost));
    memcpy(&_addr,&cp->_addr,sizeof(_addr));
    return *this;
  }
  void SetHostName();
};

/// Definition of a network connection based on TCP sockets
/**
    {\Large{\bf Class TcpConnection}}

 Definition of the TcpConnection class.
*/
class TcpConnection  : public NetworkConnection {
public:
  //@Man: Class specific enumerations and typedefs
  /// Address :== NetworkAddress
  typedef TcpNetworkAddress  Address;
  /// Socket  :== TcpNetworkChannel::Channel
  typedef TcpNetworkChannel::Channel Socket;
  /// Port    :== TcpNetworkChannel::Port
  typedef TcpNetworkChannel::Port    Port;
  /// Family  :== TcpNetworkChannel::Family
  typedef TcpNetworkChannel::Family  Family;
  /// Indicate status of the connection request
  enum ConnectionStatus {
    CONNECTION_ERROR   = NetworkConnection::NETCONNECTION_ERROR,
    CONNECTION_SUCCESS = NetworkConnection::NETCONNECTION_SUCCESS
  };
protected:
  //@Man: Protected member variables
  /// Storage for the address
  Address	            m_sin;
  /// TCP Network channel
  TcpNetworkChannel   m_channel;
  /// Pointer keeping the name of the service
  char               *_pcc_service;
public:
  //@Man: Public member functions
  /// Standard constructor with given service name
  TcpConnection( const char* service = "UserService" );
  /// Standard constructor with given port number
  TcpConnection( Port port );
  /// Standard destructor
  virtual ~TcpConnection();
  /// Initialize the connection (called by constructor(s))
  int Initialize( Port port );
  /// return Network channel 
  virtual NetworkChannel& _SendChannel ();
  /// return Network channel 
  virtual NetworkChannel& _RecvChannel ();
  /// Return name to the service the connection represents
  const char* Service() const;
  /// return Port number
  Port _Port () const;
  /// Return family type
  Family _Family () const;
  /// Address the connection points to (may be invalid)
  virtual const NetworkAddress& _Address () const;
  /// Virtual method to receive data
  virtual int Receive  (BasicRequest* req, NetworkAddress&from, Iosb& ios);
  /// Virtual method to send data
  virtual int Send     (BasicRequest* req, NetworkAddress& to, Iosb& ios);
  /// Virtual method to listen to the connection socket
  virtual int Listen( EventHandler *handler );
};
#endif
