// $Id: HltDecReportsMaker.cpp,v 1.5 2008-10-24 19:33:22 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltSummary.h"
#include "Event/HltDecReports.h"

// local
#include "HltDecReportsMaker.h"

using namespace LHCb;


///-----------------------------------------------------------------------------
// Implementation file for class : HltDecReportsMaker
//
// 2008-07-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecReportsMaker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecReportsMaker::HltDecReportsMaker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{

  declareProperty("InputHltSummaryLocation",
    m_inputHltSummaryLocation= LHCb::HltSummaryLocation::Default);  
  declareProperty("OutputHltDecReportsLocation",
    m_outputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);  

  declareProperty("DebugEventPeriod",m_debugPeriod = 0 );

  declareProperty("InfoLevelDecision", m_infoLevelDecision = ((unsigned int)kAlwaysInfoLevel) );
  declareProperty("InfoLevelDecisionDebug", m_infoLevelDecisionDebug = ((unsigned int)kAlwaysInfoLevel) );
  
  declareProperty("InfoLevelNonDecision", m_infoLevelNonDecision = ((unsigned int)kNoInfoLevel) );
  declareProperty("InfoLevelNonDecisionDebug", m_infoLevelNonDecisionDebug = ((unsigned int)kPassOrErrorsInfoLevel) );
  
  declareProperty("SelectionInfoLevel", m_infoLevel );
  declareProperty("SelectionInfoLevelDebug", m_infoLevelDebug );

  m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltDecReportsMaker::~HltDecReportsMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsMaker::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltBaseAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  m_debugMode=0;
  m_event=0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

 // determine output mode 
  ++m_event;
  if( m_event == m_debugPeriod ){
    // debug
    m_debugMode = 1;
    m_event =0;    
  } else {
    // normal
    m_debugMode = 0;
  }

  HltDecReports* outputSummary(0);  
  if( exist<HltDecReports>(m_outputHltDecReportsLocation) ){  
    // get existing decision report if exists (to be amended)
    outputSummary = get<HltDecReports>(m_outputHltDecReportsLocation);
  } else {
    // create output container and put it on TES
    outputSummary = new HltDecReports();
    put( outputSummary, m_outputHltDecReportsLocation );
  }
  
  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  

  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); 
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );

  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );


  // --------------------------------------------------------------------
  // if HltSummary exists then add their decisions to the decision reports
  // --------------------------------------------------------------------

  if( exist<HltSummary>(m_inputHltSummaryLocation) ){    
    HltSummary* inputSummary = get<HltSummary>(m_inputHltSummaryLocation);
    for( GaudiUtils::VectorMap< std::string,LHCb::HltSelectionSummary >::const_iterator 
           is=inputSummary->selections().begin();
           is!=inputSummary->selections().end();
         ++is){
      const std::string & selName = is->first;

      // skip if already there
      if( outputSummary->hasSelectionName( selName ) )continue;
      
     // filter selection for persistency
     unsigned int infoLevel = infoLevelFlag( selName );
     if( !infoLevel )continue;

     const LHCb::HltSelectionSummary& selSumIn = is->second;

     if( !(kAlwaysInfoLevel & infoLevel) ){       
       if( infoLevel & kPassInfoLevel ){      
         if( !selSumIn.decision() )continue;
       }
       //if( infoLevel & kErrorsInfoLevel ){
       //  if( !selSumIn.errors() )continue;
       //}
     }

     // save selection decision ---------------------------

     // int selection id
     int intSelID(0);   
     for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
          si!=selectionNameToIntMap.end();++si){
       if( si->first == selName ){
         intSelID=si->second;
         break;
       }
     }
     if( !intSelID ){
       Warning( " selectionName="+selName+ " not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 20 );
       continue;
     }

   
     // decision
     int dec(0);
     // errorbits
     unsigned int errorBits(0);
     // number of candidates
     int noc(0);
     
     if( selSumIn.decision() )dec=1;
     // errorBits = selSumIn.errors(); 
     noc = selSumIn.data().size();
     if( !noc ){ noc=selSumIn.particles().size();   }      
     
     HltDecReport* selSumOut = new HltDecReport( dec, errorBits, noc, intSelID );

     if( selSumOut->invalidIntSelectionID() ){
       delete selSumOut;
       std::ostringstream mess;
       mess << " selectionName=" << selName << " has invalid intSelectionID=" << intSelID << " Skipped. ";
       Warning( mess.str(), StatusCode::SUCCESS, 20 );
       continue;
     }
     
     // insert selection into the container
     if( outputSummary->insert( selName, *selSumOut ) == StatusCode::FAILURE ){
       Warning( " Failed to add HltDecReport selectionName=" + selName 
                + " to its container ", StatusCode::SUCCESS, 20 );
     }    
     
    }
  }

  // ----------------------------------------------------------------
  // form Global decisions if missing 
  // ----------------------------------------------------------------


  // Hlt1Global

  if( !outputSummary->hasSelectionName("Hlt1Global") ){
    
    int dec(0);
    unsigned int errors(0);  
    int noc(0);

    for(HltDecReports::Container::const_iterator it=outputSummary->begin();
        it!=outputSummary->end();++it){      
      const std::string & selName = it->first;
      if( ( selName.find("Hlt1") == 0 ) && 
          ( selName.find("Decision") != std::string::npos ) ) 
        if( it->second.decision() ){
          ++noc;
        }
    }
    if( noc )dec=1;
    
    if( !noc ){
      
      // see if configured (use dataSvc)
      bool config(false);
      std::vector<stringKey> selectionIDs = dataSvc().selectionKeys(); 
      for( std::vector<stringKey>::const_iterator si=selectionIDs.begin();
           si != selectionIDs.end(); ++si){
        const std::string & selName = si->str();

        if( selName.find("Hlt1") == 0 ){
          if( selName.find("Decision") != std::string::npos ){
            config=true;
            break;            
          }
        }
      }
      if( !config ){
        errors |= kDecisionNotConfigured;
      }
    }

    outputSummary->insert( "Hlt1Global", *(new HltDecReport( dec, errors, noc, kHlt1GlobalID )) );
    
  }

  // Hlt2Global

  if( !outputSummary->hasSelectionName("Hlt2Global") ){
    
    int dec(0);
    unsigned int errors(0);  
    int noc(0);

    for(HltDecReports::Container::const_iterator it=outputSummary->begin();
        it!=outputSummary->end();++it){      
      const std::string & selName = it->first;
      if( ( selName.find("Hlt2") == 0 ) && 
          ( selName.find("Decision") != std::string::npos ) ) 
        if( it->second.decision() ){
          ++noc;
        }
    }
    if( noc )dec=1;
    
    if( !noc ){
      
      // see if configured (use dataSvc)
      bool config(false);
      std::vector<stringKey> selectionIDs = dataSvc().selectionKeys(); 
      for( std::vector<stringKey>::const_iterator si=selectionIDs.begin();
           si != selectionIDs.end(); ++si){
        const std::string & selName = si->str();

        if( selName.find("Hlt2") == 0 ){
          if( selName.find("Decision") != std::string::npos ){
            config=true; 
            break;            
          }
        }
      }
      if( !config ){
        // for Hlt2 also try HltANNSvc
        for( std::vector<IANNSvc::minor_value_type>::const_iterator si=hlt2.begin();
             si!=hlt2.end();++si){
          const std::string & selName = si->first;
          if( selName.find("Decision") != std::string::npos ){
            if( selName.find("Hlt2") == 0 ){
              config=true;
              break;
            }
          }
        }
      }
      if( !config ){
        errors |= kDecisionNotConfigured;
      }
    }

    outputSummary->insert( "Hlt2Global", *(new HltDecReport( dec, errors, noc, kHlt2GlobalID )) );
    
  }

  if ( msgLevel(MSG::VERBOSE) ){
    // debugging info
    verbose() << " ====== HltDecReports container size=" << outputSummary->size() << endmsg;  
    verbose() << *outputSummary << endmsg;
  }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltDecReportsMaker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return HltBaseAlg::finalize();  // must be called after all other actions
}

//=============================================================================

//=============================================================================
// qualify trigger selections for persistency
//=============================================================================
unsigned int HltDecReportsMaker::infoLevelFlag( const std::string & selName )
{
  if( m_debugMode ){
    // DEBUG MODE

    // set info level
    const SelectionSetting & infoLevelDebug =  m_infoLevelDebug.value();    
    for( SelectionSetting::const_iterator i=infoLevelDebug.begin();
         i!=infoLevelDebug.end(); ++i){
      if( selName == i->first ){
        if( selName.find("Decision")!=std::string::npos )return (i->second | kPassInfoLevel);
        return i->second;
      }
    } 
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      return (m_infoLevelDecisionDebug.value() | kPassInfoLevel);      
    }
    return m_infoLevelNonDecisionDebug.value();

  } else {
    
    // NORMAL MODE

    // set info level
    const SelectionSetting & infoLevel =  m_infoLevel.value();    
    for( SelectionSetting::const_iterator i=infoLevel.begin();
         i!=infoLevel.end(); ++i){
      if( selName == i->first ){
        if( selName.find("Decision")!=std::string::npos )return (i->second | kPassInfoLevel);
        return i->second;
      }
    }
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      return (m_infoLevelDecision.value() | kPassInfoLevel);      
    }
    return m_infoLevelNonDecision.value();
  }

}
