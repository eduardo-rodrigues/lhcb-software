#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>

#include "HltBase/HltConfigurationHelper.h"

  
int HltConfigurationHelper::getID(Hlt::Configuration& conf,
                                  const std::string& root, 
                                  const std::string& name) {
  std::string key = root+"/"+name; 
  int id = 0;
  if (conf.has_key(key)) { 
    id = conf.retrieve<int>(key);
  } else {
    boost::hash<std::string> hasher; 
    id = hasher(name); 
    conf.add(key,id); 
    key = root+"/"+ boost::lexical_cast<std::string>(id); 
    conf.add(key,name);
  }
  return id;
}
  
std::string HltConfigurationHelper::getName(Hlt::Configuration& conf,
                                            const std::string& root, 
                                            int id) {
  std::string key = root+"/"+boost::lexical_cast<std::string>(id);
  std::string name = "unknown";
  if (conf.has_key(key)) name = conf.retrieve<std::string>(key);
  return name;
}



