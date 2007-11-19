// $Id: RawDataConnection.cpp,v 1.1 2007-11-19 19:27:32 frankb Exp $
#include "MDF/RawDataConnection.h"
#include "MDF/StreamDescriptor.h"

using namespace LHCb;
using namespace Gaudi;

/// Standard constructor
RawDataConnection::RawDataConnection(const IInterface* owner, const std::string& fname)
: IDataConnection(owner,fname)
{
  m_bind.ioDesc = m_access.ioDesc = 0;
}

/// Standard destructor      
RawDataConnection::~RawDataConnection()   {
}

StatusCode RawDataConnection::connectRead()  {
  m_bind = StreamDescriptor::bind(m_pfn);
  if ( m_bind.ioDesc > 0 )   {
    m_access = StreamDescriptor::accept(m_bind);
    if ( m_access.ioDesc != 0 ) return StatusCode::SUCCESS;
    StreamDescriptor::close(m_bind);
  }
  return StatusCode::FAILURE;
}

StatusCode RawDataConnection::connectWrite(IoType typ)  {
  switch(typ)  {
    case CREATE:
    case RECREATE:
      m_access = StreamDescriptor::connect(m_name);
      return m_access.ioDesc == 0 ? StatusCode::FAILURE : StatusCode::SUCCESS;
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Read raw byte buffer from input stream
StatusCode RawDataConnection::read(void* const data, size_t len)  {
  return StreamDescriptor::read(m_access,data,len) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Write raw byte buffer to output stream
StatusCode RawDataConnection::write(const void* data, int len)  {
  return StreamDescriptor::write(m_access,data,len) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Seek on the file described by ioDesc. Arguments as in ::seek()
long long int RawDataConnection::seek(long long int where, int origin)  {
  return StreamDescriptor::seek(m_access,where,origin);
}

StatusCode RawDataConnection::disconnect()    {
  if ( m_access.ioDesc != 0 )
    StreamDescriptor::close(m_access);
  if ( m_bind.ioDesc != 0 )
    StreamDescriptor::close(m_bind);
  m_bind.ioDesc = m_access.ioDesc = 0;
  return StatusCode::SUCCESS;
}
