// $Id: GaussMain.cpp,v 1.2 2005-04-22 15:00:03 gcorti Exp $
//------------------------------------------------------------------------------
//
//  Package    : GaudiConf
//
//  Description: Main Program for Gaudi applications
//               Sets up default job options path as ../options/job.opts
//
//------------------------------------------------------------------------------
// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include <iostream>

#include "GiGa/IGiGaRunManager.h"

//--- Example main program
int main ( int argc, char** argv ) {
  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( IID_IProperty, iface );
  SmartIF<IAppMgrUI>     appMgr  ( IID_IAppMgrUI, iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  // Get the input configuration file from arguments
  std:: string opts = (argc>1) ? argv[1] :  "";

  propMgr->setProperty( "JobOptionsPath", opts );

  // Set the program name and version using macros defined in compilation flags
#if defined APPNAME
  propMgr->setProperty( "AppName",    APPNAME );
#endif
#if defined APPVERS
  propMgr->setProperty( "AppVersion", APPVERS );
#endif

  if( opts != "" && opts.substr( opts.length() - 3, 3 ) == ".py" ) {
    propMgr->setProperty( "EvtSel",         "NONE" );
    propMgr->setProperty( "JobOptionsType", "NONE" );
    propMgr->setProperty( "DLLs",           "['GaudiPython']" );
    propMgr->setProperty( "Runable",        "PythonScriptingSvc" );
  }

  // Run the application manager and process events
  appMgr->run();

  // Release Application Manager
  iface->release();

  // destroy Geant4 
  IGiGaRunManager::destroy() ;
  
  // All done - exit
  return 0;
}
