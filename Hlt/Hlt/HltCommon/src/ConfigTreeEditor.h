// $Id: ConfigTreeEditor.h,v 1.5 2009-06-30 13:41:41 graven Exp $
#ifndef CONFIGTREEEDITOR_H 
#define CONFIGTREEEDITOR_H 1
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IConfigTreeEditor.h"
#include "HltBase/IPropertyConfigSvc.h" 
#include "HltBase/IConfigAccessSvc.h" 
#include <vector>
#include <map>
#include <string>


// Include files
// from Gaudi

/** @class ConfigTreeEditor ConfigTreeEditor.h
 *  
 * functionality:
 *
 *  @author Gerhard Raven
 *  @date   2008-02-21
 */
class ConfigTreeEditor : public GaudiTool,
                         virtual public IConfigTreeEditor 
{
public:
   ConfigTreeEditor( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );
   ~ConfigTreeEditor();

   StatusCode initialize();

   // multimap of 'component' -> 'key','value'
   ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                              const std::multimap<std::string,
                                                                  std::pair<std::string,std::string> >& updates,
                                              const std::string& label) const;

   ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                              const std::vector<std::string>& updates,
                                              const std::string& label) const ;

private:
    std::string         s_propertyConfigSvc, s_configAccessSvc;
    IPropertyConfigSvc *m_propertyConfigSvc;
    IConfigAccessSvc   *m_configAccessSvc;
};

#endif
