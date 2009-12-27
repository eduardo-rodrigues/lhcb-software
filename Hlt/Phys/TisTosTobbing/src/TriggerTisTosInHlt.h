// $Id: TriggerTisTosInHlt.h,v 1.2 2009-12-27 13:19:52 graven Exp $
#ifndef TRIGGERTISTOSINHLT_H 
#define TRIGGERTISTOSINHLT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface
#include "HltBase/IHltInspector.h"

#include "TriggerSelectionTisTosInHlt.h"

/** @class TriggerTisTosInHlt TriggerTisTosInHlt.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2009-10-22
 * 
 *  This is a special version intended for use in Hlt2 to Tis/Tos Hlt1 triggers 
 *     bypassing HltSelReports and going directly to HltDataSvc.
 *     Based on old implementation of the offline version.
 *
 *  Hit based implementation of Tis,Tos'ing Trigger specified via trigger name pattern.
 *  @sa  TriggerSelectionTisTosInHlt for inherited member functions (e.g. to define Offline Input).
 *  @sa  ITriggerTisTos docs for more explanation.
 *  This interface also defines inlined shortcuts to set Offline Input and or Trigger Input and get an output in one call. 
 */
class TriggerTisTosInHlt : public TriggerSelectionTisTosInHlt, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  TriggerTisTosInHlt( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TriggerTisTosInHlt( ); ///< Destructor

  virtual StatusCode         initialize(); 

  /// erase previous Trigger Input 
  void setTriggerInput( );

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  void addToTriggerInput( const std::string & selectionNameWithWildChar);
  
  /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  std::vector< std::string > triggerSelectionNames( unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement      = kAnything,
                                                    unsigned int tosRequirement      = kAnything );

  /// list of HltObjectSummaries from Selections satisfying TOS,TIS requirements (define Trigger and Offline Input before calling)
  std::vector<const LHCb::HltObjectSummary*> hltObjectSummaries( unsigned int tisRequirement      = kAnything,
                                                                 unsigned int tosRequirement      = kAnything );
  
   /// calculate decision,Tis,Tos  (for previously defined Offline and Trigger Inputs)
  void triggerTisTos( bool & decision, bool & tis, bool & tos);
  
private:

  /// obtain all known trigger names
  void getTriggerNames();

  static const std::vector< std::string > m_empty_selections;

  /// content of Trigger Input (list of trigger selection names) 
  std::vector< std::string > m_triggerInput_Selections;

  /// all known trigger names 
  std::vector< std::string > m_triggerNames;

  /// Inspector 
  Hlt::IInspector *m_inspector;

  /// if true then warning about empty trigger inputs are printed
  bool m_trigInputWarn;
  /// flag to allow TisTossing intermediate trigger selections (i.e. non-decision like)
  bool m_allowIntermediateSelections;
};
#endif // TRIGGERTISTOSINHLT_H
