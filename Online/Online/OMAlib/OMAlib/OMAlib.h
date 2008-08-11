// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAlib.h,v 1.7 2008-08-11 08:05:15 ggiacomo Exp $
#ifndef OMALIB_OMALIB_H
#define OMALIB_OMALIB_H 1
/** @class  OMAlib OMAlib.h OMAlib/OMAlib.h
 * 
 *  Online Monitoring Analysis library
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 3/10/2007
 */
#include <map>
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OMAlib/OMAcommon.h"
#include "OMAlib/OMAalg.h"
class OnlineHistDB;


class OMAlib : public OMAcommon
{
 public:
  /// constructor to be used if already connected to HistDB (use NULL for not using HistDB)
  OMAlib (OnlineHistDB* HistDB);
  /// constructor opening a new DB session
  OMAlib (std::string DBpasswd, 
	  std::string DBuser = OnlineHistDBEnv_constants::ACCOUNT, 
	  std::string DB = OnlineHistDBEnv_constants::DB);
  /// constructor with a new default (read-only) DB session
  OMAlib();  
  virtual ~OMAlib ();
  /// open DB session
  void openDBSession(std::string DBpasswd, 
                     std::string DBuser , 
                     std::string DB);
  /// retrieve algorithm from the known algorithm list
  inline OMAalg* getAlg(std::string Name){    
    return m_algorithms[Name];}

 private:
  void doAlgList();
  void syncList();
  bool m_localDBsession;
  std::map<std::string, OMAalg*> m_algorithms;
};
#endif // OMALIB_OMALIB_H
