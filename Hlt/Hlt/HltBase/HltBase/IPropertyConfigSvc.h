// $Id: IPropertyConfigSvc.h,v 1.1 2008-02-15 08:17:33 graven Exp $
#ifndef IPROPERTYCONFIGSVC_H 
#define IPROPERTYCONFIGSVC_H 1

// Include files
#include "GaudiKernel/INamedInterface.h"
#include "HltBase/ConfigTreeNode.h"
#include "HltBase/PropertyConfig.h"


/** @class IPropertyConfigSvc IPropertyConfigSvc.h
 *  
 *  functionality:
 *         Interface for obtaining information about configurations 
 *
 *
 *  @author Gerhard Raven
 *  @date   2008-01-01
 */


class IPropertyConfigSvc : virtual public INamedInterface  {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID();
  virtual ~IPropertyConfigSvc();

  // Return the configuration object for this object...
  virtual PropertyConfig currentConfiguration(const INamedInterface& obj) const = 0;

  // Return the configuration ID for this object...
  PropertyConfig::digest_type currentConfigurationID(const INamedInterface& obj) const 
  { return currentConfiguration(obj).digest(); }

  // Search a configuration tree, specified by 'configTree', for the configuration ID of the 
  // object with name 'name'
  virtual PropertyConfig::digest_type findInTree(const ConfigTreeNode::digest_type& configTree, const std::string& name) const = 0;

  PropertyConfig::digest_type findInTree(const ConfigTreeNode::digest_type& configTree, const INamedInterface& iface) const
  { return findInTree(configTree,iface.name()); }






  // virtual std::vector<MD5::Digest> topLevelConfigsUsing(const CfgID_t& name) const = 0;
  //
  // return name of the object with the specified id in n, provided id is a valid CfgID
  // virtual StatusCode name(const CfgID_t& toplevelConfig, std::string& n) const = 0 ;

  // return type of the object with the specified id in n, provided id is a valid CfgID
  // virtual StatusCode type(const CfgID_t& id, std::string& t) const = 0 ;

  // return string value of the property of the object specified by id
  //  and key key, provided id is a valid CfgID, and the object has a property
  //  with the specified key
  // virtual StatusCode propertyAsString(const CfgID_t& id, const std::string& key,
  //                                    std::string& property) const = 0;
  // bool template<typename T> property(const CfgID_t& id, T& prop) const;
};
#endif // IPROPERTYCONFIGSVC_H
