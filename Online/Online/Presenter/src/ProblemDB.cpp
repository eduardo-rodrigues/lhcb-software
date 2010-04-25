// $Id: ProblemDB.cpp,v 1.3 2010-04-25 17:56:01 robbep Exp $
// Include files 
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>
// local
#include "ProblemDB.h"

using namespace boost::xpressive ;

//-----------------------------------------------------------------------------
// Implementation file for class : ProblemDB
//
// 2010-03-22 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProblemDB::ProblemDB( std::string address ) {
  m_address = address;
}
//=============================================================================
// Destructor
//=============================================================================
ProblemDB::~ProblemDB() {} 


//=========================================================================
//  
//=========================================================================
int ProblemDB::post( std::string system, std::string username, std::string title, 
                     std::string message, std::string logReference ) {

  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << std::endl ;
    return -1;
  }

  // Send HTTP request to web server
  std::string postArg;
  postArg = "apikey=12345&system_name=" + system
    + "&title=" + urlEncode( title )
    + "&initial_comment=" + urlEncode( message )
    + "&author_name=" + urlEncode( username );
  if ( !logReference.empty() ) postArg = postArg + "&link=" + urlEncode( logReference );
  char argLen[20];
  sprintf( argLen, "%d", (int)postArg.size() );
  
  std::string posting;
  posting = "POST /api/problems/ HTTP/1.0\r\nHost: " + m_address + "\r\nContent-Length: " + argLen + "\r\n\r\n" + postArg;

  std::cout << "Sending '" << posting << "'" << std::endl;

  webStream << posting << std::flush;
  
  std::string line ;

  int status = 0;
  std::getline( webStream , line );
  std::cout << line << std::endl;
  if ( line.find( "201 Created") != std::string::npos ) {
    status = 1; 
    while( std::getline( webStream , line )  ) {
      std::cout << line << std::endl;
      if ( line.find( "Location: " ) == 0 ) {
        m_reference = line.substr( 10 );
      }
    }
  } 
  // Check that the web server answers correctly
  while( std::getline( webStream , line )  ) {
    std::cout << line << std::endl;
  }
  std::cout << "--- end ---" << std::endl;
  
  return status;
}
//=========================================================================
//  
//=========================================================================
std::string  ProblemDB::urlEncode ( std::string src) {
  unsigned char *pd, *p;
  std::string str;
  str.reserve( 3* src.size() );

  pd = (unsigned char *) str.c_str();
  p  = (unsigned char *) src.c_str();
  while ( *p ) {
    if (strchr("%&=#?+/", *p) || *p > 127 || *p < 32) {
      sprintf((char *) pd, "%%%02X", *p);
      pd += 3;
      p++;
    } else if (*p == ' ') {
      *pd++ = '+';
      p++;
    } else {
      *pd++ = *p++;
    }
  }
  *pd = '\0';
  str = std::string( str.c_str() );
  return str;
}
//=============================================================================
//
//=============================================================================
void ProblemDB::getListOfOpenedProblems( std::vector< std::vector< std::string > > & 
					 problems ,
					 const std::string & systemName ) {
  problems.clear() ;
  boost::asio::ip::tcp::iostream webStream( m_address , "http" ) ;

  if ( ! webStream ) {
    std::cout << "Cannot open the Problem Database at " << m_address  << std::endl ;
    return ;
  }

  // Take date of tomorrow to have list of opened problems
  boost::posix_time::ptime now =
    boost::posix_time::second_clock::local_time() ;
  boost::gregorian::date day = now.date() + boost::gregorian::date_duration( 1 ) ;


  // Send HTTP request to web server
  webStream << "GET /api/search/?_inline=True&system_visible=True"
	    << "&open_or_closed_gte=" << boost::gregorian::to_iso_extended_string( day )
	    << " HTTP/1.0\r\n"
	    << "Host:" << m_address << "\r\n"
	    << "\r\n" << std::flush ;

  std::string line ;

  // Check that the web server answers correctly
  std::getline( webStream , line ) ;
  if ( ! boost::algorithm::find_first( line , "200 OK" ) ) {
    std::cerr << "ProblemDB server does not reply" << std::endl ;
    return ;
  }
  

  // Parse the web server answers
  std::string pattern ;
  pattern = "\\N{left-square-bracket}\\N{left-curly-bracket}(.*)" ;
  pattern += "\\N{right-curly-bracket}\\N{right-square-bracket}" ;
  
  const boost::regex e( pattern ) ;

  // retrieve individual fields
  boost::xpressive::mark_tag system(1) , severity(2) , started_at(3) , id(4) , 
    title(5) ;

  boost::xpressive::sregex pb = "{\"system\": \"" >> (system=*~(set= '\"'))
						  >> "\", \"severity\": \""
						  >> (severity=*~(set= '\"'))
						  >> "\", \"started_at\": \""
						  >> (started_at=*~(set= '\"')) 
						  >> "\", \"id\": \""
						  >> (id=*~(set= '\"')) 
						  >> "\", \"title\": \""
						  >> (title=*~(set= '\"')) 
						  >> "\"}" ;

  std::vector< std::string > single_line ;

  while ( std::getline( webStream , line ) ) {
    if ( boost::regex_match( line , e ) ) {
      boost::xpressive::sregex_iterator cur( line.begin() , line.end() , pb ) ;
      boost::xpressive::sregex_iterator end ;
      for ( ; cur != end ; ++cur ) {
	single_line.clear() ;
	boost::xpressive::smatch const &what = *cur ;
	std::string systn = what[ system ] ;
	if ( "" != systemName ) 
	  if ( systemName != systn ) 
	    continue ;
	single_line.push_back( systn ) ;
	single_line.push_back( what[severity] ) ;
	single_line.push_back( what[started_at] ) ;
	single_line.push_back( what[id] ) ;
	single_line.push_back( what[title] ) ;
	
	problems.push_back( single_line ) ;
      }
    }
  }
}
