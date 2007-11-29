#include <algorithm>
#include <iostream>
#include <vector>

#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGDockableFrame.h>
//#include <TGDoubleSlider.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGListTree.h>
#include <TGListView.h>
#include <TGMenu.h>
#include <TGMsgBox.h>
#include <TGPicture.h>
#include <TGSplitter.h>
#include <TGStatusBar.h>
#include <TGTab.h>
#include <TGTextEdit.h>
#include <TGTextView.h>
#include <TGToolBar.h>
#include <TH1.h>
#include <TH2.h>
#include <TImage.h>
//#include <TList.h>
#include <TMath.h>
//#include <TObjArray.h>
#include <TObject.h>
#include <TPad.h>
#include <TPRegexp.h>
#include <TRootEmbeddedCanvas.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TThread.h>
#include <TTimer.h>

#include <TGResourcePool.h>
// DB, DI (buggy GetId() for WindowID...)
//gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
//gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
//  gVirtualX->Update();

#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"

#include "DbRootHist.h"
#include "PresenterMainFrame.h"
#include "HistogramIdentifier.h"
#include "LoginDialog.h"
#include "HistoPropDialog.h"
#include "PageSaveDialog.h"
#include "SetDimDnsNodeDialog.h"

using namespace pres;

PresenterMainFrame* gPresenter = 0;

ClassImp(PresenterMainFrame)

PresenterMainFrame::PresenterMainFrame(const char* name,
                                       UInt_t x, UInt_t y,
                                       UInt_t width, UInt_t height,
                                       MsgLevel verbosity):
  TGMainFrame(gClient->GetRoot(), width, height, kLHintsExpandX | kLHintsExpandY),
  m_verbosity(verbosity),
  m_refreshTimer(NULL),
  m_refreshing(false),
  m_clearedHistos(false),
  m_histogramDB(NULL),
  m_analysisLib(NULL),
  m_connectedToHistogramDB(false),
  m_msgBoxReturnCode(0),
  m_mainCanvasWidth(width),
  m_mainCanvasHeight(height),
  m_menuDock(NULL),
  m_toolBarDock(NULL),
  m_databaseHistogramsDock(NULL),
  m_pageDock(NULL),
  m_mainCanvasInfoDock(NULL),
  m_dimBrowserDock(NULL),
  m_databasePagesDock(NULL),
  m_treeNode(NULL),
  m_taskNode(NULL),
  m_algorithmNode(NULL),
  m_histogramSetNode(NULL),
  m_histogramNode(NULL),
  m_pageNode(NULL),
  m_folderItems(NULL),
  m_folderItemsIt(NULL),
  m_folderItem(NULL)
{
#ifndef WIN32
  // RICH colour palette -- broken on w32 - see an eventual patch in v5.18?
  const int colNum = 1024;  // Number of colors in the palette
  int startIndex = 1000;    // starting index of allocated colors
  int palette[colNum];
  float val;
  int iCount = 0;
  const int iStep = 1;

  m_RootColourVector.reserve(colNum);

  // blue(0,0,255) -> cyan(0,255,255)
  for (int i = 0; i < 256; i += iStep) {
    val = (float)((float)i/(float)256);
    TColor* color = new TColor(startIndex + iCount, 0, val, 1);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  // cyan (0,255,255) -> green (0,255,0)
  for (int i = 0; i < 256; i += iStep){
    val = (float)((float)i/(float)256);
    TColor* color = new TColor(startIndex + iCount, 0.0, 1.0, 1.0 - val);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  // green (0,255,0) -> yellow (255,255,0)
  for (int i=0; i < 256; i += iStep){
    val = (float)((float)i/(float)256);
    TColor* color = new TColor(startIndex + iCount, val, 1.0, 0.0);
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  // yellow (255,255,0) -> red (255,0,0)
  for (int i = 0; i < 256; i += iStep){
    val = (float)((float)i/(float)256);
    TColor* color = new TColor(startIndex+iCount, 1, 1.0 - val, 0.0); 
    m_RootColourVector.push_back(color);
    palette[iCount] = startIndex + iCount;
    iCount++;
  }

  gStyle->SetPalette(colNum, palette);
#endif

//  gStyle->SetOptStat("emrou"); // nemr
  
  SetCleanup(kDeepCleanup);
  
  // only one session viewer allowed
  if (gPresenter) { return; }
  if (!m_refreshTimer) m_refreshTimer = new TTimer(TimeoutOfMainTimer);
  m_refreshTimer->TurnOff();
  m_refreshTimer->Connect("Timeout()", "PresenterMainFrame", this,
                          "refreshPage()");
  SetWindowName(name);
  Move(x, y);
//  Resize(w, h);
  gPresenter = this;

  m_knownDimServices.reserve(EstimatedDimServiceCount);
  m_histogramTypes.reserve(EstimatedDimServiceCount);
  m_DbHistosOnPage.reserve(EstimatedHistosOnPage);

  buildGUI();

  //TODO: move initDim to member...
  m_dimBrowser = new DimBrowser();  
  if (m_dimBrowser->getServers() == 0) {
    m_message = "Sorry, no DIM server found.";
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }
    m_mainStatusBar->SetText(m_message.c_str());
    new TGMsgBox(fClient->GetRoot(), this, "DIM Error", m_message.c_str(),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  } else {
    m_mainStatusBar->SetText("Ok.");
  }
  refreshDimSvcListTree();
  
  loginToHistogramDB();
}
PresenterMainFrame::~PresenterMainFrame()
{
  TGListTreeItem* node = m_dimSvcListTree->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_dimSvcListTree->DeleteChildren(node);

  node = m_databaseHistogramTreeList->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_databaseHistogramTreeList->DeleteChildren(node);

  node = m_pagesFromHistoDBListTree->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_pagesFromHistoDBListTree->DeleteChildren(node);  
  //Clean up used widgets: frames, buttons, layouthints
  //  if (zoomCanvas) delete zoomCanvas;
  
#ifndef WIN32
  std::vector<TColor*>::const_iterator colourIter;
  std::vector<TColor*>::const_iterator colourIterBegin =
      m_RootColourVector.begin();
  std::vector<TColor*>::const_iterator colourIterEnd   =
      m_RootColourVector.end();
  for (colourIter = colourIterBegin; colourIter != colourIterEnd;
       colourIter++) {
    delete *colourIter;     // m_RootColourVector.~X();
  }
#endif  
  
  removeHistogramsFromPage();
  Cleanup();
  if (m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL;}
  if (m_analysisLib) {delete m_analysisLib; m_analysisLib = NULL;}
  if (m_dimBrowser) {delete m_dimBrowser; m_dimBrowser = NULL;}
  if (gPresenter == this) { gPresenter = 0; }
}
void PresenterMainFrame::buildGUI()
{
  SetCleanup(kDeepCleanup);
  // set minimun size
  // SetWMSizeHints(400 + 200, 370+50, 2000, 1000, 1, 1);
  m_fileText = new TGHotString("&File");
    m_filePrint = new TGHotString("Save/&Print Page to File...");
    m_fileNew = new TGHotString("&New Page");
    m_fileSaveText = new TGHotString("&Save Page to Database...");
    m_fileLoginText = new TGHotString("&Login to Database...");
    m_fileLogoutText = new TGHotString("Log&out...");
    m_fileQuitText = new TGHotString("&Quit");

  m_editText = new TGHotString("&Edit");
    m_editAutoLayoutText = new TGHotString("&Automatic Histogram Layout");
    m_editHistogramPropertiesText = new TGHotString("&Histogram Properties...");
    m_editRemoveHistoText = new TGHotString("&Remove Histogram from Page");    
    m_editSetReferenceHistogramText = new TGHotString("&Set as Reference Histogram");
    m_editPagePropertiesText = new TGHotString("&Page Properties...");
    
  m_viewText = new TGHotString("&View");
    m_viewStartRefreshText = new TGHotString("Start Page &Refresh");
    m_viewStopRefreshText = new TGHotString("&Stop Page Refresh");
    m_viewOverlayReferenceHistogramText = new TGHotString("&Overlay Reference");
    m_viewInspectHistoText = new TGHotString("Inspect &Histogram...");
    m_viewInspectPageText  = new TGHotString("Inspect &Page...");
    m_viewClearHistosText = new TGHotString("&Clear Histograms");
    m_viewUnDockPageText = new TGHotString("&Undock Page");
    m_viewDockAllText = new TGHotString("&Dock All");

  m_toolText = new TGHotString("&Tools");
    m_toolMode = new TGHotString("&Mode");
    m_toolPageEditor = new TGHotString("Page &Editor");
    m_toolOnline = new TGHotString("&Online");
    m_toolOffline = new TGHotString("O&ffline");
    m_toolSetUtgidTaskText = new TGHotString("Set &UTGID for Task...");
    m_toolSetDimDnsText = new TGHotString("Set &DIM DNS...");

  m_helpText = new TGHotString("&Help");    
    m_helpContentsText = new TGHotString("&Contents");
    m_helpAboutText = new TGHotString("&About");

  // list tree
  m_openedFolderIcon = gClient->GetPicture("ofolder_t.xpm");
  m_closedFolderIcon = gClient->GetPicture("folder_t.xpm");

  m_iconH1D = gClient->GetPicture("h1_t.xpm");
  m_iconH2D = gClient->GetPicture("h2_t.xpm");
  m_iconProfile = gClient->GetPicture("profile_t.xpm");
  m_iconCounter = gClient->GetPicture("c_src_t.xpm");
  m_iconPage = gClient->GetPicture("bld_paste_into.png");
  m_iconQuestion = gClient->GetPicture("mdi_help.xpm");

  m_iconTask = gClient->GetPicture("tmacro_t.xpm");
  m_iconAlgorithm = gClient->GetPicture("expression_t.xpm");
  //m_iconSet = gClient->GetPicture("pack-empty.xpm");
  m_iconSet = gClient->GetPicture("pack-empty_t.xpm");
  m_iconLevel = gClient->GetPicture("bld_vbox.png");

  // File menu  
  m_fileMenu = new TGPopupMenu(fClient->GetRoot());
  m_fileMenu->AddEntry(m_fileNew, CLEAR_PAGE_COMMAND);
  m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileSaveText, SAVE_PAGE_TO_DB_COMMAND);
//  m_fileMenu->AddEntry(m_filePrint, SAVE_PAGE_TO_FILE_COMMAND);
  m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileLoginText, LOGIN_COMMAND);
  m_fileMenu->AddEntry(m_fileLogoutText, LOGOUT_COMMAND);
  m_fileMenu->AddSeparator();  
  m_fileMenu->AddEntry(m_fileQuitText, EXIT_COMMAND);
  m_fileMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                      "handleCommand(Command)");
m_fileMenu->DisableEntry(SAVE_PAGE_TO_FILE_COMMAND);

  // Edit menu
  m_editMenu = new TGPopupMenu(fClient->GetRoot());
  m_editMenu->AddEntry(m_editAutoLayoutText, AUTO_LAYOUT_COMMAND);  
  m_editMenu->AddEntry(m_editHistogramPropertiesText, EDIT_HISTO_COMMAND);
  m_editMenu->AddEntry(m_editRemoveHistoText, REMOVE_HISTO_FROM_CANVAS_COMMAND);  
  m_editMenu->AddEntry(m_editSetReferenceHistogramText, SET_REFERENCE_HISTO_COMMAND);
  m_editMenu->AddEntry(m_editPagePropertiesText, EDIT_PAGE_PROPERTIES_COMMAND);
  m_editMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                      "handleCommand(Command)");
m_editMenu->DisableEntry(SET_REFERENCE_HISTO_COMMAND);
m_editMenu->DisableEntry(EDIT_PAGE_PROPERTIES_COMMAND);

  // View menu
  m_viewMenu = new TGPopupMenu(fClient->GetRoot());
  m_viewMenu->AddEntry(m_viewInspectHistoText, INSPECT_HISTO_COMMAND);
  m_viewMenu->AddEntry(m_viewInspectPageText, INSPECT_PAGE_COMMAND);
  m_viewMenu->AddEntry(m_viewStartRefreshText, START_COMMAND);
  m_viewMenu->AddEntry(m_viewStopRefreshText, STOP_COMMAND);
  m_viewMenu->DisableEntry(STOP_COMMAND);
  m_viewMenu->AddEntry(m_viewOverlayReferenceHistogramText, OVERLAY_REFERENCE_HISTO_COMMAND);
  m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
  m_viewMenu->AddEntry(m_viewClearHistosText, CLEAR_HISTOS_COMMAND);
  m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
  m_viewMenu->AddSeparator();
  m_viewMenu->AddEntry(m_viewUnDockPageText, UNDOCK_PAGE_COMMAND);
  m_viewMenu->AddEntry(m_viewDockAllText, DOCK_ALL_COMMAND);
  m_viewMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                      "handleCommand(Command)");
m_viewMenu->DisableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);



  // Mode menu
  m_toolMenu = new TGPopupMenu(fClient->GetRoot());
  m_toolMenu->AddEntry(m_toolPageEditor, PAGE_EDITOR_MODE_COMMAND);
  m_toolMenu->CheckEntry(PAGE_EDITOR_MODE_COMMAND);  
//  m_toolMenu->AddEntry(m_toolOnline, ONLINE_MODE_COMMAND);
//  m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
//  m_toolMenu->AddEntry(m_toolOffline, OFFLINE_MODE_COMMAND);
//  m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
  m_toolMenu->AddSeparator();
  m_toolMenu->AddEntry(m_toolSetUtgidTaskText, SET_UTGID_TASK_COMMAND);    
  m_toolMenu->AddSeparator();  
  m_toolMenu->AddEntry(m_toolSetDimDnsText, SET_DIM_DNS_COMMAND);
  m_toolMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                      "handleCommand(Command)");                      
m_toolMenu->DisableEntry(ONLINE_MODE_COMMAND);
m_toolMenu->DisableEntry(OFFLINE_MODE_COMMAND);                                
m_toolMenu->DisableEntry(SET_UTGID_TASK_COMMAND);
m_toolMenu->DisableEntry(SET_DIM_DNS_COMMAND);

  // Help menu
  m_helpMenu = new TGPopupMenu(fClient->GetRoot());
  m_helpMenu->AddEntry(m_helpContentsText, HELP_CONTENTS_COMMAND);
  m_helpMenu->AddEntry(m_helpAboutText, HELP_ABOUT_COMMAND);
  m_helpMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                      "handleCommand(Command)");
m_helpMenu->DisableEntry(HELP_CONTENTS_COMMAND);
m_helpMenu->DisableEntry(HELP_ABOUT_COMMAND);

  m_menuDock = new TGDockableFrame(this);
    m_menuDock->SetWindowName("LHCb Presenter Menu");
  
  m_toolBarDock = new TGDockableFrame(this);
    m_toolBarDock->SetWindowName("LHCb Presenter Toolbar");

  TGLayoutHints* menuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  TGLayoutHints* menuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
//  TGLayoutHints* menuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
  m_menuBar = new TGMenuBar(m_menuDock, 1, 1, kHorizontalFrame);
  m_menuBar->AddPopup(m_fileText, m_fileMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_editText, m_editMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_viewText, m_viewMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_toolText, m_toolMenu, menuBarItemLayout);
//  m_menuBar->AddPopup(m_helpText, m_helpMenu, menuBarHelpLayout);
                                        
  m_menuDock->EnableUndock(kFALSE);
  m_menuDock->EnableHide(kTRUE);
  
  m_toolBarDock->EnableUndock(kTRUE);
  m_toolBarDock->EnableHide(kTRUE);
  
  m_menuDock->AddFrame(m_menuBar, menuBarLayout);
  m_menuDock->AddFrame(new TGHorizontal3DLine(this), menuBarLayout);
  
  // Toolbar
  m_toolBar = new TGToolBar(m_toolBarDock, 60, 20, kHorizontalFrame);
  m_toolbarElement = new ToolBarData_t();

  // New page
  m_toolbarElement->fPixmap = "ed_new.png";
  m_toolbarElement->fTipText = "New page";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = CLEAR_PAGE_COMMAND;
  m_newPageButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_newPageButton->Connect("Clicked()", "PresenterMainFrame", this,
                           "removeHistogramsFromPage()");

  // Save page
//  m_toolbarElement->fPixmap = "ed_save.png";
  m_toolbarElement->fPixmap = "save.xpm";
  m_toolbarElement->fTipText = "Save page to Database";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = SAVE_PAGE_TO_DB_COMMAND;
  m_savePageToDatabaseButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_savePageToDatabaseButton->SetState(kButtonDisabled);
  m_savePageToDatabaseButton->Connect("Clicked()", "PresenterMainFrame", this,
                           "savePageToHistogramDB()");

  // Auto n x n layout
  m_toolbarElement->fPixmap = "ed_compile.png";
  m_toolbarElement->fTipText = "Auto n x n histogram tiling layout";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = AUTO_LAYOUT_COMMAND;
  m_autoCanvasLayoutButton = m_toolBar->AddButton(this,
    m_toolbarElement, 0);
  m_autoCanvasLayoutButton->Connect("Clicked()", "PresenterMainFrame", this,
                                    "autoCanvasLayout()");

  // Login
  m_toolbarElement->fPixmap = "connect.xpm";
  m_toolbarElement->fTipText = "Login";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = LOGIN_COMMAND;
  m_loginButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_loginButton->Connect("Clicked()", "PresenterMainFrame", this,
                         "loginToHistogramDB()");
  // Logout
  m_toolbarElement->fPixmap = "disconnect.xpm";
  m_toolbarElement->fTipText = "Logout";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = LOGOUT_COMMAND;
  m_logoutButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_logoutButton->SetState(kButtonDisabled);
  m_logoutButton->Connect("Clicked()", "PresenterMainFrame", this,
                          "logoutFromHistogramDB()");
  // Start
  m_toolbarElement->fPixmap = "ed_execute.png";
  m_toolbarElement->fTipText = "Start";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = START_COMMAND;
  m_startRefreshButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_startRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "startPageRefresh()");
  // Stop
  m_toolbarElement->fPixmap = "ed_interrupt.png";
  m_toolbarElement->fTipText = "Stop";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = STOP_COMMAND;
  m_stopRefreshButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_stopRefreshButton->SetState(kButtonDisabled);
  m_stopRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                               "stopPageRefresh()");

  // Clear Histos
  m_toolbarElement->fPixmap = "refresh.xpm";
  m_toolbarElement->fTipText = "Clear histograms";
  m_toolbarElement->fStayDown =  true;
  m_toolbarElement->fId = CLEAR_HISTOS_COMMAND;
  m_clearHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
//  m_clearHistoButton->SetState(kButtonDisabled);
  m_clearHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                              "clearHistos()");
                              
  // Reference
  m_toolbarElement->fPixmap = "f2_t.xpm";
  m_toolbarElement->fTipText = "Overlay with reference";
  m_toolbarElement->fStayDown =  true;
  m_toolbarElement->fId = OVERLAY_REFERENCE_HISTO_COMMAND;
  m_overlayReferenceHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
//  m_overlayReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
//                                "startPageRefresh()");
m_overlayReferenceHistoButton->SetState(kButtonDisabled);

  // Set reference
  m_toolbarElement->fPixmap = "f1_t.xpm";
  m_toolbarElement->fTipText = "Set as reference histogram";
  m_toolbarElement->fStayDown =  false;
  m_toolbarElement->fId = SET_REFERENCE_HISTO_COMMAND;
  m_setReferenceHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_setReferenceHistoButton->SetState(kButtonDisabled);
//  m_setReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
//                               "stopPageRefresh()");                              
m_setReferenceHistoButton->SetState(kButtonDisabled);

  // Clone Histo
  m_toolbarElement->fPixmap = "tb_find.xpm";
  m_toolbarElement->fTipText = "Inspect histogram";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = INSPECT_HISTO_COMMAND;
  m_inspectHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 8);
  m_inspectHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                             "inspectHistogram()");

  // Edit Histogram properties
  m_toolbarElement->fPixmap = "bld_edit.png";
  m_toolbarElement->fTipText = "Edit histogram options";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = EDIT_HISTO_COMMAND;
  m_editHistoButton = m_toolBar->AddButton(this, m_toolbarElement, 0);
  m_editHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                             "editHistogramProperties()");

  // Delete histo from Canvas
  m_toolbarElement->fPixmap = "ed_delete.png";
  m_toolbarElement->fTipText = "Remove selected histogram from page";
  m_toolbarElement->fStayDown = false;
  m_toolbarElement->fId = REMOVE_HISTO_FROM_CANVAS_COMMAND;
  m_deleteHistoFromCanvasButton = m_toolBar->AddButton(this,
                                                       m_toolbarElement, 8);
  m_deleteHistoFromCanvasButton->Connect("Clicked()", "PresenterMainFrame",
                                         this, "deleteSelectedHistoFromCanvas()");
  m_toolBarDock->AddFrame(m_toolBar,
           new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));

  Connect("CloseWindow()", "PresenterMainFrame", this, "CloseWindow()");

  // Main horizontal frame
  TGHorizontalFrame* mainHorizontalFrame = new TGHorizontalFrame(this, 544, 498, kHorizontalFrame | kLHintsExpandX | kLHintsExpandY);
    m_databaseHistogramsDock = new TGDockableFrame(mainHorizontalFrame);
    m_databaseHistogramsDock->SetWindowName("Histograms in Database");  
    m_databaseHistogramsDock->EnableUndock(kTRUE);
    m_databaseHistogramsDock->EnableHide(false);
    m_databaseHistogramsDock->Resize(220, 494);
      // databaseHistogramFrame  
  TGVerticalFrame* leftDatabaseHistogramFrame = new TGVerticalFrame(m_databaseHistogramsDock, 220, 494, kVerticalFrame | kFixedWidth);
//TGGroupFrame* leftDatabaseHistogramFrame = new TGGroupFrame(m_databaseHistogramsDock, kVerticalFrame | kFixedWidth);  
  TGGroupFrame* databaseHistogramGroupFrame = new TGGroupFrame(leftDatabaseHistogramFrame, TGString("Histograms in Database"), kVerticalFrame | kFixedWidth);
//                                                  TGGroupFrame(m_databaseHistogramsDock, TGString("Histogram Database"), kVerticalFrame | kFixedWidth);       

      leftDatabaseHistogramFrame->AddFrame(databaseHistogramGroupFrame,
                                         new TGLayoutHints(kLHintsTop |
                                                           kLHintsExpandX |
                                                           kLHintsExpandY,
                                                           2, 2, 2, 2));                                                           
          
      m_databaseHistogramsDock->AddFrame(leftDatabaseHistogramFrame,
                                         new TGLayoutHints(kLHintsTop |
                                                           kLHintsExpandX |
                                                           kLHintsExpandY,
                                                           2, 2, 2, 2));                                                           
        // Datbase histogram filter box
        m_histoDBFilterComboBox = new TGComboBox(databaseHistogramGroupFrame, -1,
                                                 kHorizontalFrame | kDoubleBorder | 
                                                 kSunkenFrame | kOwnBackground);
        m_histoDBFilterComboBox->AddEntry("Folder/Page", FoldersAndPages); // #0
        m_histoDBFilterComboBox->AddEntry("Tasks/Algorithm", Tasks); // #1
        m_histoDBFilterComboBox->AddEntry("Subsystems", Subsystems); // #2
        m_histoDBFilterComboBox->AddEntry("With auto analysis",
                                          HistogramsWithAnalysis); //#3
        m_histoDBFilterComboBox->AddEntry("Produced by analysis",
                                          AnalysisHistograms); //#4
        m_histoDBFilterComboBox->AddEntry("Full list", AllHistograms); // #5
        // @enum FilterCriteria
        m_histoDBFilterComboBox->Resize(149, 22);
        m_histoDBFilterComboBox->Select(Tasks);
        m_histoDBFilterComboBox->Connect("Selected(Int_t)", "PresenterMainFrame",
                                         this, "refreshHistoDBListTree()");
        databaseHistogramGroupFrame->AddFrame(m_histoDBFilterComboBox,
                                         new TGLayoutHints(kLHintsLeft |
                                                           kLHintsTop |
                                                           kLHintsExpandX,
                                                           2, 2, 2, 2));      
        // canvas widget
        m_histoDBCanvas = new TGCanvas(databaseHistogramGroupFrame, 124, 460);
      
        // canvas viewport
        m_histoDBCanvasViewPort = m_histoDBCanvas->GetViewPort();
      
        // list tree
        m_databaseHistogramTreeList = new TGListTree(m_histoDBCanvas,
          kHorizontalFrame);
        m_databaseHistogramTreeList->AddRoot("Histograms");
        m_databaseHistogramTreeList->Connect(
          "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame", this,
          "clickedHistoDBTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");
      
        m_histoDBContextMenu = new TGPopupMenu(fClient->GetRoot());
        m_histoDBContextMenu->AddEntry("Add checked histogram(s) to Page",
          M_AddDBHistoToPage_COMMAND);
        m_histoDBContextMenu->AddEntry("Set properties of checked histogram(s)",
          M_SetHistoPropertiesInDB_COMMAND);
        m_histoDBContextMenu->AddEntry("Delete checked histogram(s)",
          M_DeleteDBHisto_COMMAND);
        m_histoDBContextMenu->AddSeparator();
        m_histoDBContextMenu->AddEntry("Check all children",
          M_DBHistoCheckAllChildren_COMMAND);
        m_histoDBContextMenu->AddEntry("Uncheck all children",
          M_DBHistoUnCheckAllChildren_COMMAND);
        m_histoDBContextMenu->AddSeparator();
        m_histoDBContextMenu->AddEntry("Collsapse all children",
          M_DBHistoCollapseAllChildren_COMMAND);
        m_histoDBContextMenu->AddSeparator();
        m_histoDBContextMenu->AddEntry("Refresh",
          M_RefreshHistoDBListTree_COMMAND);
        m_histoDBContextMenu->Connect("Activated(Int_t)","PresenterMainFrame", this,
                                      "handleCommand(Command)");
//m_histoDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);
      
        m_histoDBCanvasViewPort->AddFrame(m_databaseHistogramTreeList);
        m_databaseHistogramTreeList->SetLayoutManager(
          new TGHorizontalLayout(m_databaseHistogramTreeList));
        m_databaseHistogramTreeList->MapSubwindows();
        m_histoDBCanvas->SetContainer(m_databaseHistogramTreeList);
        m_histoDBCanvas->MapSubwindows();
        databaseHistogramGroupFrame->AddFrame(m_histoDBCanvas,
                                         new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX |
                                                           kLHintsExpandY, 2, 2, 2, 2));
//TGLabel *leditorCentralVerticalFrame = new TGLabel(editorCentralVerticalFrame, "editorCentralVerticalFrame");
//editorCentralVerticalFrame->AddFrame(leditorCentralVerticalFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandY, 3, 0, 0, 0));

  TGVSplitter* leftVerticalSplitter = new TGVSplitter(mainHorizontalFrame, 2, 2);
    leftVerticalSplitter->SetFrame(leftDatabaseHistogramFrame, kTRUE);

  m_pageDock = new TGDockableFrame(mainHorizontalFrame);
  m_pageDock->SetWindowName("LHCb Presenter Page");  
  m_pageDock->EnableUndock(kTRUE);
  m_pageDock->EnableHide(false);
  m_pageDock->Resize(600, 494);
  m_pageDock->Connect("Docked()", "PresenterMainFrame", this, "enablePageUndocking()");
  m_pageDock->Connect("Undocked()", "PresenterMainFrame", this, "disablePageUndocking()");    
  // centralFrame
  TGVerticalFrame* centralPageFrame = new TGVerticalFrame(m_pageDock, 600, 494, kVerticalFrame);   
  m_pageDock->AddFrame(centralPageFrame,
                       new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                         kLHintsExpandX | kLHintsExpandY,
                                         2, 2, 2, 2));                                         
//  Int_t partsTop[] = {25, 75};
//  m_statusBarTop = new TGStatusBar(editorCentralVerticalFrame, 50, 10, kVerticalFrame);
//  m_statusBarTop->SetParts(partsTop, 2);
//  m_statusBarTop->Draw3DCorner(false);
//  editorCentralVerticalFrame->AddFrame(m_statusBarTop, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));

  // embedded canvas
  editorEmbCanvas = new TRootEmbeddedCanvas(0, centralPageFrame, 600, 494);
  Int_t wm_editorEmbCanvas = editorEmbCanvas->GetCanvasWindowId();
  editorEmbCanvas->SetBit(kNoContextMenu);
  editorCanvas = new TCanvas("editor canvas", 600, 494, wm_editorEmbCanvas);
  editorCanvas->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                        "PresenterMainFrame", this,
                        "EventInfo(Int_t, Int_t, Int_t, TObject*)");
//  editorCanvas->SetGrid();
  editorCanvas->SetBit(kNoContextMenu);
    
//  editorCanvas->SetFixedAspectRatio(true);
  editorEmbCanvas->AdoptCanvas(editorCanvas); 
  centralPageFrame->AddFrame(editorEmbCanvas,
                             new TGLayoutHints(kLHintsRight | kLHintsTop |
                                               kLHintsExpandX | kLHintsExpandY,
                                               2, 2, 2, 2));

//TGDoubleHSlider *fHSlider634 = new TGDoubleHSlider(editorCentralVerticalFrame);
//fHSlider634->SetRange(0, 1023);
//fHSlider634->SetPosition(1000, 1023);
//editorCentralVerticalFrame->AddFrame(fHSlider634, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX 
//                      ,2,2,2,2));
//  fHSlider634->MoveResize(8,16,720,24);

// Info dock
  m_mainCanvasInfoDock = new TGDockableFrame(mainHorizontalFrame);
  m_pageDock->SetWindowName("LHCb Presenter Page");
  
  m_mainCanvasInfoDock->EnableUndock(kTRUE);
  m_mainCanvasInfoDock->EnableHide(kTRUE);

//  editorMainHorizontalFrame->AddFrame(m_mainCanvasInfoDock,
//    new TGLayoutHints(kLHintsLeft | kLHintsBottom | kLHintsExpandX |
//                      kLHintsExpandY, 2, 2, 2, 2));    


    TGTab *fTab515 = new TGTab(m_mainCanvasInfoDock, 400, 100); 
    // container of "Page Description"
    TGCompositeFrame *fCompositeFrame518;
    fCompositeFrame518 = fTab515->AddTab("Page Description");
    //fCompositeFrame518->SetLayoutManager(new TGVerticalLayout(fCompositeFrame518));
    fCompositeFrame518->SetLayoutBroken(kTRUE);
    TGTextEdit *fTextEdit523 = new TGTextEdit(fCompositeFrame518,400,95);
    //fTextEdit523->LoadFile("TxtEdit523");
    fCompositeFrame518->AddFrame(fTextEdit523, new TGLayoutHints(kLHintsLeft | kLHintsBottom | kLHintsExpandX
                          ,2,2,2,2));
    //fTextEdit523->MoveResize(2,2,288,75);    
    // container of "Tab2"
    TGCompositeFrame *fCompositeFrame520;
    fCompositeFrame520 = fTab515->AddTab("Message Log");
    //fCompositeFrame520->SetLayoutManager(new TGVerticalLayout(fCompositeFrame520));
    fCompositeFrame520->SetLayoutBroken(kTRUE);
    TGTextEdit *fTextEdit542 = new TGTextEdit(fCompositeFrame520,400,95);
    //fTextEdit542->LoadFile("TxtEdit542");
    fCompositeFrame520->AddFrame(fTextEdit542, new TGLayoutHints(kLHintsLeft | kLHintsBottom| kLHintsExpandX
                          ,2,2,2,2));
    //fTextEdit542->MoveResize(0,0,290,77);
    fTab515->SetTab(1);
    fTab515->Resize(fTab515->GetDefaultSize());
    m_mainCanvasInfoDock->AddFrame(fTab515, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX
                          ,2,2,2,2));
    //fTab515->MoveResize(64,40,296,104);                                                      

    TGVerticalFrame* rightMiscFrame             = new TGVerticalFrame(mainHorizontalFrame     , 220, 494, kVerticalFrame | kFixedWidth);
  //TGVerticalFrame* leftDatabaseHistogramFrame = new TGVerticalFrame(m_databaseHistogramsDock, 220, 494, kVerticalFrame | kFixedWidth);    
  TGVSplitter* rightVerticalSplitter = new TGVSplitter(mainHorizontalFrame,2,2);  
    rightVerticalSplitter->SetFrame(rightMiscFrame, false);
    
//TGLabel *lrightMiscFrame = new TGLabel(rightMiscFrame, "rightMiscFrame");
//rightMiscFrame->AddFrame(lrightMiscFrame, new TGLayoutHints(kLHintsTop, 3, 0, 0, 0));    

  
    m_dimBrowserDock = new TGDockableFrame(rightMiscFrame);
    m_dimBrowserDock->SetWindowName("DIM Histogram Browser");  
    m_dimBrowserDock->EnableUndock(kTRUE);
    m_dimBrowserDock->EnableHide(false);
    m_dimBrowserDock->Resize(220, 243);
    rightMiscFrame->AddFrame(m_dimBrowserDock,
      new TGLayoutHints(kLHintsRight | kLHintsTop | kLHintsExpandX | kLHintsExpandY,
                        2, 2, 2, 2));

    TGGroupFrame* m_dimBrowserGroupFrame = new TGGroupFrame(m_dimBrowserDock, TGString("DIM Histogram Browser"), kVerticalFrame | kFixedHeight);
    m_dimBrowserDock->AddFrame(m_dimBrowserGroupFrame,
                               new TGLayoutHints(kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 2, 2, 2, 2));

//  TGHSplitter* miscHorizontalSplitter = new TGHSplitter(rightMiscFrame, 2, 2);
//    miscHorizontalSplitter->SetFrame(m_dimBrowserGroupFrame, kTRUE);
//  rightMiscFrame->AddFrame(miscHorizontalSplitter, new TGLayoutHints(kLHintsLeft | kLHintsExpandX));    
                                                                                                            
                        
    // canvas widget
    m_dimSvcListTreeContainterCanvas = new TGCanvas(m_dimBrowserGroupFrame, 220, 243);  
    // canvas viewport
    TGViewPort* fViewPort664 = m_dimSvcListTreeContainterCanvas->GetViewPort();
      
    m_dimSvcListTree = new TGListTree(m_dimSvcListTreeContainterCanvas,
      kHorizontalFrame);
  //  m_dimSvcListTree->SetAutoCheckBoxPic(false); ROOT v5.16(?)...
    m_dimSvcListTree->AddRoot("DIM");
    m_dimSvcListTree->Connect(
      "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame", this,
      "clickedDimTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");
  
    fViewPort664->AddFrame(m_dimSvcListTree);
    m_dimSvcListTree->SetLayoutManager(new TGHorizontalLayout(
      m_dimSvcListTree));
    m_dimSvcListTree->MapSubwindows();
    m_dimSvcListTreeContainterCanvas->SetContainer(m_dimSvcListTree);
    m_dimSvcListTreeContainterCanvas->MapSubwindows();
    m_dimBrowserGroupFrame->AddFrame(m_dimSvcListTreeContainterCanvas,
      new TGLayoutHints(kLHintsRight | kLHintsTop | kLHintsExpandX | kLHintsExpandY,
                        2, 2, 2, 2));
  
    m_dimContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_dimContextMenu->AddEntry("Add checked to Database",
      M_AddDimToDB_COMMAND);
    m_dimContextMenu->AddEntry("Add checked to Page",
      M_AddDimToPage_COMMAND);
    m_dimContextMenu->AddSeparator();
    m_dimContextMenu->AddEntry("Check all children",
      M_DimCheckAllChildren_COMMAND);
    m_dimContextMenu->AddEntry("Uncheck all children",
      M_DimUnCheckAllChildren_COMMAND);
    m_dimContextMenu->AddSeparator();
    m_dimContextMenu->AddEntry("Collsapse all children",
      M_DimCollapseAllChildren_COMMAND);
    m_dimContextMenu->AddSeparator();
    m_dimContextMenu->AddEntry("Refresh",
      M_RefreshHistoDIMListTree_COMMAND);
    m_dimContextMenu->Connect("Activated(Int_t)","PresenterMainFrame", this,
                              "handleCommand(Command)");
    
    
    m_databasePagesDock = new TGDockableFrame(rightMiscFrame);
    m_databasePagesDock->SetWindowName("Database Page Browser");  
    m_databasePagesDock->EnableUndock(kTRUE);
    m_databasePagesDock->EnableHide(false);
    m_databasePagesDock->Resize(220, 243);
    rightMiscFrame->AddFrame(m_databasePagesDock,
                             new TGLayoutHints(kLHintsRight | kLHintsBottom |
                                               kLHintsExpandX | kLHintsExpandY,
                                               2, 2, 2, 2));
                                               
    TGGroupFrame* databasePagesGroupFrame = new TGGroupFrame(m_databasePagesDock, TGString("Database Page Browser"), kVerticalFrame | kFixedHeight);
    m_databasePagesDock->AddFrame(databasePagesGroupFrame,
                               new TGLayoutHints(kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 2, 2, 2, 2));    
                                               
    // canvas widget
    TGCanvas* fCanvas652 = new TGCanvas(databasePagesGroupFrame, 220, 243);
    // canvas viewport
    m_pagesFromHistoDBViewPort = fCanvas652->GetViewPort();
  
    // list tree
    m_pagesFromHistoDBListTree = new TGListTree(fCanvas652, kHorizontalFrame);
    m_pagesFromHistoDBListTree->AddRoot("Pages");
    m_pagesFromHistoDBListTree->Connect(
      "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame", this,
      "clickedPageTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");
  
    m_pagesFromHistoDBViewPort->AddFrame(m_pagesFromHistoDBListTree);
    m_pagesFromHistoDBListTree->SetLayoutManager(
      new TGHorizontalLayout(m_pagesFromHistoDBListTree));
    m_pagesFromHistoDBListTree->MapSubwindows();
    fCanvas652->SetContainer(m_pagesFromHistoDBListTree);
    fCanvas652->MapSubwindows();
    databasePagesGroupFrame->AddFrame(fCanvas652,
                                  new TGLayoutHints(kLHintsRight | kLHintsBottom |
                                                    kLHintsExpandX | kLHintsExpandY,
                                                    2, 2, 2, 2));
                                                      
    m_pagesContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_pagesContextMenu->AddEntry("Load Page", M_LoadPage_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Delete Page", M_DeletePage_COMMAND);
    m_pagesContextMenu->AddEntry("Delete Folder", M_DeleteFolder_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Refresh", M_RefreshDBPagesListTree_COMMAND);
    m_pagesContextMenu->Connect("Activated(Int_t)", "PresenterMainFrame", this,
                                "handleCommand(Command)");

  mainHorizontalFrame->AddFrame(m_databaseHistogramsDock,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsExpandY,
                                                  2, 2, 2, 2));
  mainHorizontalFrame->AddFrame(leftVerticalSplitter, new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  mainHorizontalFrame->AddFrame(m_pageDock,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsExpandX | kLHintsExpandY,
                                                   2, 2, 2, 2));   
  mainHorizontalFrame->AddFrame(rightVerticalSplitter, new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  mainHorizontalFrame->AddFrame(rightMiscFrame,
                                new TGLayoutHints(kLHintsLeft |
                                                  kLHintsExpandY,
                                                  2, 2, 2, 2));   
  Int_t parts[] = {35, 25, 40};
  m_mainStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
    m_mainStatusBar->SetParts(parts, 3);
    m_mainStatusBar->SetText("Launching Presenter...");  

  AddFrame(m_menuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
  AddFrame(m_toolBarDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
  AddFrame(mainHorizontalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  AddFrame(m_mainStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 0, 0));

  m_menuDock->MapWindow();
  m_toolBarDock->MapWindow();
  m_databaseHistogramsDock->MapWindow();
  m_pageDock->MapWindow();
  m_dimBrowserDock->MapWindow();
  m_databasePagesDock->MapWindow();
//  m_mainCanvasInfoDock->MapWindow();

  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputPrimaryApplicationModal);
  MapSubwindows();
  Resize();
  MapWindow();
}
void PresenterMainFrame::CloseWindow()
{
  if (m_refreshing && m_refreshTimer) {
    m_refreshTimer->Stop();
  }
  gApplication->Terminate();
}
void PresenterMainFrame::dockAllFrames()
{
  // TODO: FindObject IsA...
  m_pageDock->DockContainer();
//  m_mainCanvasInfoDock->DockContainer();
  m_toolBarDock->DockContainer();
  m_menuDock->DockContainer();
  m_databaseHistogramsDock->DockContainer();
  m_dimBrowserDock->DockContainer();
  m_databasePagesDock->DockContainer();
}
void PresenterMainFrame::handleCommand(Command cmd)
{
  // ROOT GUI workaround: slot mechanism differs on different signals from
  // different widgets... void* 4 messages :-(
  if (X_ENIGMA_COMMAND == cmd) {
    TGButton* btn = reinterpret_cast<TGButton*>(gTQSender);
    cmd = static_cast<Command>(btn->WidgetId());
  }
  switch (cmd) {
    case EXIT_COMMAND:
      CloseWindow();
      break;
    case START_COMMAND:
      startPageRefresh();
      break;
    case STOP_COMMAND:
      stopPageRefresh();
      break;
    case CLEAR_HISTOS_COMMAND:
      clearHistos();
      break;      
    case CLEAR_PAGE_COMMAND:
      removeHistogramsFromPage();
      break;
    case SAVE_PAGE_TO_DB_COMMAND:
      savePageToHistogramDB();
      break;    
    case SAVE_PAGE_TO_FILE_COMMAND:
      savePageToFile();
      break;          
    case LOGOUT_COMMAND:
      logoutFromHistogramDB();
      break;
    case LOGIN_COMMAND:
      loginToHistogramDB();
      break;
    case INSPECT_HISTO_COMMAND:
      inspectHistogram();
      break;
    case INSPECT_PAGE_COMMAND:
      inspectPage();
      break;      
    case AUTO_LAYOUT_COMMAND:
      autoCanvasLayout();
      break;
    case EDIT_HISTO_COMMAND:
      editHistogramProperties();
      break;      
    case REMOVE_HISTO_FROM_CANVAS_COMMAND:
      deleteSelectedHistoFromCanvas();
      break;
    case UNDOCK_PAGE_COMMAND:
      m_pageDock->UndockContainer();
      break;
    case DOCK_ALL_COMMAND:
      dockAllFrames();
      break;      
    case M_AddDimToDB_COMMAND:
      addDimSvcToHistoDB();
      break;
    case M_AddDimToPage_COMMAND:
      addDimSvcToPage();
      break;
    case M_DimCheckAllChildren_COMMAND:
      dimCheckAllChildren();
      break;
    case M_DimUnCheckAllChildren_COMMAND:
      dimUnCheckAllChildren();
      break;
    case M_DimCollapseAllChildren_COMMAND:
      dimCollapseAllChildren();
      break;
    case M_RefreshHistoDIMListTree_COMMAND:
      refreshDimSvcListTree();
      break;
    case M_AddDBHistoToPage_COMMAND:
        addDbHistoToPage();
      break;
    case M_DBHistoCheckAllChildren_COMMAND:
      dbHistoCheckAllChildren();
      break;
    case M_DBHistoUnCheckAllChildren_COMMAND:
      dbHistoUnCheckAllChildren();
      break;
    case M_DBHistoCollapseAllChildren_COMMAND:
      dbHistoCollapseAllChildren();
      break;
    case M_DeleteDBHisto_COMMAND:
      deleteSelectedHistoFromDB();
      break;
    case M_SetHistoPropertiesInDB_COMMAND:
      setHistogramPropertiesInDB();
      break;
    case M_RefreshHistoDBListTree_COMMAND:
      refreshHistoDBListTree();
      m_histogramDB->refresh();
      break;
    case M_RefreshDBPagesListTree_COMMAND:
      refreshPagesDBListTree();
       m_histogramDB->refresh();
      break;
    case M_LoadPage_COMMAND:
      loadSelectedPageFromDB();
      break;
    case M_DeletePage_COMMAND:
      deleteSelectedPageFromDB();
      break;
    case M_DeleteFolder_COMMAND:
      deleteSelectedFolderFromDB();
      break;      
    default:
      if (m_verbosity >= Debug) {
        std::cout << "zut. TGButton WidgetId from gTQSender is corrupt"
          << std::endl;
      }
      break;
  }
}
void PresenterMainFrame::enableAutoCanvasLayoutBtn()
{
  m_autoCanvasLayoutButton->SetState(kButtonEngaged);
  m_autoCanvasLayoutButton->SetState(kButtonUp);
}
void PresenterMainFrame::disableAutoCanvasLayoutBtn()
{
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
}
void PresenterMainFrame::savePageToFile()
{
}
void PresenterMainFrame::savePageToHistogramDB()
{
  if (m_connectedToHistogramDB) {
    fClient->WaitFor(new PageSaveDialog(this, 493, 339, m_verbosity));
//  m_savePageToDatabaseButton->SetState(kButtonDisabled);

    refreshPagesDBListTree();
    refreshHistoDBListTree();
//    statusBar()
  }
}
bool PresenterMainFrame::connectToHistogramDB(std::string dbPassword,
                                              std::string dbUsername,
                                              std::string dbName)
{
  try {
    if (m_histogramDB && m_connectedToHistogramDB) {
      delete m_histogramDB;
      m_histogramDB = NULL;      
    }
    
    m_histogramDB = new OnlineHistDB(dbPassword, dbUsername, dbName);

    // storage allocate vectors:
    m_histogramTypes.reserve(m_histogramDB->nHistograms());
    m_localDatabaseIDS.reserve(m_histogramDB->nHistograms());
    m_localDatabasePages.reserve(m_histogramDB->nPages());
    m_localDatabaseFolders.reserve(m_histogramDB->nPageFolders());
    
    m_analysisLib = new OMAlib(m_histogramDB);

    m_dbName = dbName;
    m_mainStatusBar->SetText(dbUsername.c_str(), 2);

    m_message = "Successfully connected to OnlineHistDB.";
    m_mainStatusBar->SetText(m_message.c_str());

    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }

    m_logoutButton->SetState(kButtonUp);
    m_loginButton->SetState(kButtonDisabled);
    m_savePageToDatabaseButton->SetState(kButtonEngaged);
    m_savePageToDatabaseButton->SetState(kButtonUp);

    m_fileMenu->DisableEntry(LOGIN_COMMAND);
    m_fileMenu->EnableEntry(LOGOUT_COMMAND);
    m_fileMenu->EnableEntry(SAVE_PAGE_TO_DB_COMMAND);

//    showHistogramDatabaseTools();
//    statusBar()->message(tr("Successfully connected to OnlineHistDB."));
    m_connectedToHistogramDB = true;

    m_histogramDB->setExcLevel(AllExceptions);
    m_histogramDB->setDebug(m_verbosity);
  } catch (std::string sqlException) {
    // TODO: add error logging backend - MsgStream?
    m_mainStatusBar->SetText(sqlException.c_str());
    if (m_verbosity >= Verbose) {
      std::cout << sqlException << std::endl;
    }

    new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                 Form("Could not connect to the OnlineHistDB server:\n\n%s\n"
                      "This program cannot function correctly",
                      sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);

    //  TODO: disable/hideHistogramDatabaseTools();
    if (m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL;}
    m_connectedToHistogramDB = false;
  }
  return m_connectedToHistogramDB;
}
void PresenterMainFrame::loginToHistogramDB()
{
  //Modal dialog
  fClient->WaitFor(new LoginDialog(this, 350, 310));

  //  histoDBFilterComboBox->HideFrame();
  if (m_connectedToHistogramDB) {
    showDBTools();
  } else {
    hideDBTools();
  }
  //  TODO: refactor refresh* methods to use only 1 DB readout...
  refreshPagesDBListTree();
  refreshHistoDBListTree();

  removeHistogramsFromPage();
  enablePageLoading();
}
void PresenterMainFrame::logoutFromHistogramDB()
{
  if (m_histogramDB && m_connectedToHistogramDB) {
    new TGMsgBox(fClient->GetRoot(), this, "Logout from Database",
                 "Do you really want to logout from Database?",
                 kMBIconQuestion, kMBYes | kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
      case kMBNo:
        return;
    }
  }
  delete m_histogramDB;
  m_histogramDB = NULL;
  if (m_analysisLib) {delete m_analysisLib; m_analysisLib = NULL;}  
  m_connectedToHistogramDB = false;
  removeHistogramsFromPage();
  m_message = "Disconnected from histogramDB";
  m_mainStatusBar->SetText(m_message.c_str());
  if (m_verbosity >= Verbose) {
    std::cout << m_message << std::endl;
  }

  m_loginButton->SetState(kButtonUp);
  m_logoutButton->SetState(kButtonDisabled);
  m_savePageToDatabaseButton->SetState(kButtonDisabled);

  m_fileMenu->EnableEntry(LOGIN_COMMAND);
  m_fileMenu->DisableEntry(LOGOUT_COMMAND);
  m_fileMenu->DisableEntry(SAVE_PAGE_TO_DB_COMMAND);

  //  histoDBFilterComboBox->ShowFrame();
  hideDBTools();

  //  TODO: refactor refresh* methods to use only 1 DB readout...
  refreshPagesDBListTree();
  refreshHistoDBListTree();

  removeHistogramsFromPage();
}
void PresenterMainFrame::startPageRefresh()
{
  m_message = "Refresh started.";
  m_mainStatusBar->SetText(m_message.c_str());
  if (m_verbosity >= Verbose) {
    std::cout << m_message << std::endl;
  }

  m_stopRefreshButton->SetState(kButtonUp);
  m_viewMenu->EnableEntry(STOP_COMMAND);
  m_startRefreshButton->SetState(kButtonDisabled);
  m_viewMenu->DisableEntry(START_COMMAND);
//  m_clearHistoButton->SetState(kButtonUp);

  //  m_refreshTimer->Start(50);
  enablePageRefresh();
  m_refreshTimer->TurnOn();
  m_refreshing = true;
}
void PresenterMainFrame::stopPageRefresh()
{
  disablePageRefresh();
  if (m_refreshTimer) {
    m_message = "Refresh stopped.";
    m_mainStatusBar->SetText(m_message.c_str());
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }

    m_startRefreshButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);    
    m_viewMenu->DisableEntry(STOP_COMMAND);
//    m_clearHistoButton->SetState(kButtonDisabled);

    //    m_refreshTimer->Stop();
//    if (m_clearedHistos) { clearHistos();}
    m_refreshTimer->TurnOff();
    m_refreshing = false;
  }
}
void PresenterMainFrame::printPage()
{
}
void PresenterMainFrame::clearHistos()
{
  if (m_clearedHistos) {
    m_message = "Reverted to integral.";
    m_mainStatusBar->SetText(m_message.c_str());
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
    m_clearedHistos = false;

    disableHistogramClearing();

  } else {
    m_message = "Histograms cleared.";
    m_mainStatusBar->SetText(m_message.c_str());
    m_clearHistoButton->SetState(kButtonDown);
    m_clearedHistos = true;
    m_viewMenu->CheckEntry(CLEAR_HISTOS_COMMAND);    
    enableHistogramClearing();
  }
}
OMAlib* PresenterMainFrame::analysisLib()
{
  return m_analysisLib;
}
OnlineHistDB* PresenterMainFrame::histogramDB()
{
  if (!m_connectedToHistogramDB) {
    loginToHistogramDB();
  }
  return m_histogramDB;
}
bool PresenterMainFrame::isConnectedToHistogramDB()
{
  return m_connectedToHistogramDB;
}
void PresenterMainFrame::listFromHistogramDB(TGListTree* listView,
  FilterCriteria filterCriteria, bool histograms)
{
  if (m_connectedToHistogramDB && listView) {
    try {
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
      
      m_treeNode = listView->GetFirstItem();
      deleteTreeChildrenItemsUserData(m_treeNode);
      listView->DeleteChildren(m_treeNode);      
      listView->RenameItem(m_treeNode, TString(m_dbName));
      m_localDatabaseFolders.clear();
      m_localDatabaseIDS.clear();
      m_histogramTypes.clear();
      switch (filterCriteria) {
        case FoldersAndPages:
          m_histogramDB->getPageFolderNames(m_localDatabaseFolders, "_ALL_");
          break;
        case Subsystems:
          m_histogramDB->getSubsystems(m_localDatabaseFolders);
          break;
        case Tasks:
          m_histogramDB->getTasks(m_localDatabaseFolders);
          break;
        case HistogramsWithAnalysis:
          m_histogramDB->getHistogramsWithAnalysis(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AnalysisHistograms:
          m_histogramDB->getAnalysisHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AllHistograms:
          m_histogramDB->getAllHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        default:
          break;
      }
      std::sort(m_localDatabaseFolders.begin(),
                m_localDatabaseFolders.end());

      if (filterCriteria == FoldersAndPages ||
          filterCriteria == Subsystems ||
          filterCriteria == Tasks) {
        for (m_folderIt = m_localDatabaseFolders.begin();
             m_folderIt != m_localDatabaseFolders.end(); ++m_folderIt) {
          m_folderItems = TString(*m_folderIt).Tokenize("/");
          m_folderItemsIt = m_folderItems->MakeIterator();
          m_treeNode = listView->GetFirstItem();
          while (m_folderItem = m_folderItemsIt->Next()) {
            if (listView->FindChildByName(m_treeNode,
                                          m_folderItem->GetName()) &&
                NULL == m_treeNode->GetUserData()) {
              m_treeNode = listView->FindChildByName(m_treeNode,
                                                     m_folderItem->GetName());
            } else {
              if (filterCriteria != Tasks) {
                m_treeNode = listView->AddItem(m_treeNode,
                                               m_folderItem->GetName());
              }
            }
            m_treeNode->SetUserData(0);
            if (m_folderItem == m_folderItems->Last() &&
                filterCriteria == FoldersAndPages) {
              m_localDatabasePages.clear();
              m_histogramDB->getPageNamesByFolder(*m_folderIt,
                                                  m_localDatabasePages);
              std::sort(m_localDatabasePages.begin(),
                        m_localDatabasePages.end());
              for (m_pageIt = m_localDatabasePages.begin();
                   m_pageIt != m_localDatabasePages.end(); ++m_pageIt) {
                std::string pageName = std::string(*m_pageIt).erase(0,
                                                   (*m_folderIt).length()+1);
                m_pageNode = listView->AddItem(m_treeNode, pageName.c_str());
                m_pageNode->SetUserData(new TString(*m_pageIt));
                listView->CheckItem(m_pageNode, false);
                setTreeNodeIcon(m_pageNode, PAGE);
                if (histograms) {
                  m_localDatabaseIDS.clear();
                  m_histogramTypes.clear();
                  m_histogramDB->getHistogramsByPage(*m_pageIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                  fillTreeNodeWithHistograms(listView, m_pageNode,
                                             &m_localDatabaseIDS,
                                             &m_histogramTypes);
                }
              }
            } else if (m_folderItem == m_folderItems->Last() &&
                       filterCriteria != FoldersAndPages) {
              if (histograms) {
                m_localDatabaseIDS.clear();
                m_histogramTypes.clear();
                if (filterCriteria == Tasks) {
                  m_histogramDB->getHistogramsByTask(*m_folderIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                } else if (filterCriteria == Subsystems) {
                  m_histogramDB->getHistogramsBySubsystem(*m_folderIt,
                    NULL, &m_localDatabaseIDS, &m_histogramTypes);
                }
                fillTreeNodeWithHistograms(listView, m_treeNode,
                  &m_localDatabaseIDS, &m_histogramTypes);
              }
            }
          }
          m_folderItems->Delete();
          delete m_folderItems;
          m_folderItems = NULL;
          delete m_folderItemsIt;
          m_folderItemsIt = NULL;
        }
      } else if (filterCriteria == HistogramsWithAnalysis ||
                 filterCriteria == AnalysisHistograms ||
                 filterCriteria == AllHistograms) {
          fillTreeNodeWithHistograms(listView, listView->GetFirstItem(),
                                     &m_localDatabaseIDS, &m_histogramTypes);
      }
      fClient->NeedRedraw(listView);
      m_message = "Histograms read from Database.";

gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
      
    } catch (std::string sqlException) {
        // TODO: add error logging backend
        m_mainStatusBar->SetText(sqlException.c_str());
        if (m_verbosity >= Verbose) {
          std::cout << sqlException << std::endl;
        }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
            Form("OnlineHistDB server:\n\n%s\n",
                 sqlException.c_str()),
            kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  } else if (!m_connectedToHistogramDB) {
    m_message = "Histograms not read from Database.";
  }
  m_mainStatusBar->SetText(m_message.c_str());
  if (m_verbosity >= Verbose) {
    std::cout << m_message << std::endl;
  }
}
void PresenterMainFrame::fillTreeNodeWithHistograms(TGListTree* listView,
  TGListTreeItem* node, std::vector<std::string>*  histogramList,
  std::vector<std::string>*  histogramTypes)
{
  m_histogramType = histogramTypes->begin();
  for (m_histogramIt = histogramList->begin();
       m_histogramIt != histogramList->end();
       ++m_histogramIt, ++m_histogramType) {
    HistogramIdentifier histogramIdentifier =
      HistogramIdentifier(*m_histogramIt);
    m_taskNode = node;
    // Taskname
    if (listView->FindChildByName(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str())) {
      m_taskNode = listView->FindChildByName(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str());
    } else {
      m_taskNode = listView->AddItem(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str());
      setTreeNodeIcon(m_taskNode, TASK);
    }
    m_taskNode->SetUserData(0);
    // Algorithmname
    m_algorithmNode = m_taskNode;
    if (listView->FindChildByName(m_algorithmNode,
      histogramIdentifier.algorithmName().c_str())) {
        m_algorithmNode = listView->FindChildByName(
          m_algorithmNode, histogramIdentifier.algorithmName().c_str());
    } else {
      m_algorithmNode = listView->AddItem(m_algorithmNode,
        histogramIdentifier.algorithmName().c_str());
      setTreeNodeIcon(m_algorithmNode, ALGORITHM);
    }
    m_algorithmNode->SetUserData(0);
    // histogramsetname
    m_histogramSetNode = m_algorithmNode;
    if (histogramIdentifier.isFromHistogramSet()) {
      if (listView->FindChildByName(m_histogramSetNode,
        histogramIdentifier.histogramSetName().c_str())) {
          m_histogramSetNode = listView->FindChildByName(
            m_histogramSetNode,
            histogramIdentifier.histogramSetName().c_str());
      } else {
        m_histogramSetNode = listView->AddItem(m_histogramSetNode,
          histogramIdentifier.histogramSetName().c_str());
        setTreeNodeIcon(m_histogramSetNode, SET);
      }
      m_histogramSetNode->SetUserData(0);
    }
    // Histogramame
    m_histogramNode = m_histogramSetNode;
    m_histogramIdItems = TString(histogramIdentifier.histogramName()).
                                 Tokenize("/");
    m_histogramIdItemsIt = m_histogramIdItems->MakeIterator();

    while (m_histogramIdItem = m_histogramIdItemsIt->Next()) {
      if (m_histogramIdItem != m_histogramIdItems->Last()) {
        if (listView->FindChildByName(m_histogramNode,
          m_histogramIdItem->GetName())) {
            m_histogramNode = listView->FindChildByName(m_histogramNode,
              m_histogramIdItem->GetName());
        } else {
          m_histogramNode = listView->AddItem(m_histogramNode,
            m_histogramIdItem->GetName());
        }
        m_histogramNode->SetUserData(0);
        setTreeNodeIcon(m_histogramNode, LEVEL);
      }
      if (m_histogramIdItem == m_histogramIdItems->Last()) {
        m_histogramNode = listView->AddItem(m_histogramNode,
          m_histogramIdItem->GetName());
        listView->SetCheckBox(m_histogramNode, kTRUE);
        listView->ToggleItem(m_histogramNode);
        setTreeNodeIcon(m_histogramNode, *m_histogramType);
        m_histogramNode->SetUserData(new TString(*m_histogramIt));
      }
    }
    m_histogramIdItems->Delete();
    delete m_histogramIdItems;
    m_histogramIdItems = NULL;
    delete m_histogramIdItemsIt;
    m_histogramIdItemsIt = NULL;
  }
  sortTreeChildrenItems(listView, node);
}
void PresenterMainFrame::changeDimDnsNode()
{
  fClient->WaitFor(new SetDimDnsNodeDialog(this, 320,150, m_verbosity));
}
std::string PresenterMainFrame::histogramDBName()
{
  return m_dbName;
}
// TODO: move all below to an eventual static helper class
// TODO: void PresenterMainFrame::WaitCursor()
void PresenterMainFrame::setTreeNodeIcon(TGListTreeItem* node,
  std::string type)
{
  const TGPicture*  m_icon;
  if (H1D == type) {
    m_icon = m_iconH1D;
  } else if (H2D == type) {
    m_icon = m_iconH2D;
  } else if (P1D == type ||
             HPD == type ||
             P2D == type ) {
    m_icon = m_iconProfile;
  } else if (CNT == type) {
    m_icon = m_iconCounter;
  } else if (PAGE == type) {
    m_icon = m_iconPage;
  } else if (TASK == type) {
    m_icon = m_iconTask;
  } else if (ALGORITHM == type) {
    m_icon = m_iconAlgorithm;
  } else if (SET == type) {
    m_icon = m_iconSet;
  } else if (LEVEL == type) {
    m_icon = m_iconLevel;
  } else {
    m_icon = m_iconQuestion;
  }
  if (node) { node->SetPictures(m_icon, m_icon); }
}
void PresenterMainFrame::checkedTreeItems(TGListTree* selected,
  TGListTree* treeList)
{
  if (!selected) return;
  TGListTreeItem* node;
  node = treeList->GetFirstItem();
  if (node->IsChecked() && node->GetUserData()) {
    selected->AddItem(0, 0, node->GetUserData());
  }
  while(node) {
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::checkedTreeItemsChildren(TGListTreeItem *node,
  TGListTree* selected)
{
  while (node) {
    if (node->IsChecked() && node->GetUserData()) {
      selected->AddItem(0, 0, node->GetUserData());
    }
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::checkTreeChildrenItems(TGListTreeItem* node,
  bool check)
{
  if (node && NULL == node->GetFirstChild() &&
      node->GetUserData()) {
    node->CheckItem(check);
  }
  if (node && node->GetFirstChild()) {
    checkTreeChildrenItemsChildren(node->GetFirstChild(), check);
  }
}
void PresenterMainFrame::checkTreeChildrenItemsChildren(TGListTreeItem* node,
  bool check)
{
  while (node) {
    if (NULL == node->GetFirstChild() && node->GetUserData()) {
      node->CheckItem(check);
    }
    if (node->GetFirstChild()) {
      checkTreeChildrenItemsChildren(node->GetFirstChild(), check);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::collapseTreeChildrenItems(TGListTree* treeList,
  TGListTreeItem* node)
{
  if (node && treeList) {
    treeList->CloseItem(node);
  }
  while (node) {
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::collapseTreeChildrenItemsChildren(
  TGListTree* treeList, TGListTreeItem* node)
{
  while (node) {
    if (node && treeList) {
      treeList->CloseItem(node);
    }
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::sortTreeChildrenItems(TGListTree* treeList,
  TGListTreeItem* node)
{
  if (node && treeList) {
    treeList->SortChildren(node);
  }
  while (node) {
    if (node->GetFirstChild()) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::sortTreeChildrenItemsChildren(
  TGListTree* treeList, TGListTreeItem* node)
{
  while (node) {
    if (node && treeList) {
      treeList->SortChildren(node);
    }
    if (node->GetFirstChild()) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserData(TGListTreeItem* node)
{
  if (node && node->GetUserData()) {
    delete (TString*)(node->GetUserData());
    node->SetUserData(NULL);  
  }
  if (node && node->GetFirstChild()) {
    deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserDataChildren(TGListTreeItem* node)
{
  while (node) {
    if (node->GetUserData()) {
      delete (TString*)(node->GetUserData());
      node->SetUserData(NULL);
    }
    if (node->GetFirstChild()) {
      deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::setStatusBarText(const char* text, int slice)
{
  m_mainStatusBar->SetText(text, slice);
}
TH1* PresenterMainFrame::selectedHistogram()
{
  if (gPad) {
    TIter nextPadElem(gPad->GetListOfPrimitives());
    TObject *histoCandidate;
    while (histoCandidate = nextPadElem()) {
      if (histoCandidate->InheritsFrom(TH1::Class())) {
// TODO: When refhistos will come... editorFrame->
// 0 == std::string((*m_DbHistosOnPageIt)->rootHistogram->GetName()).compare(histoCandidate->GetName())) {
        return dynamic_cast<TH1*>(histoCandidate);
      }
    } return 0;
  } else {
    return 0;
  }
}
void PresenterMainFrame::editHistogramProperties()
{
//  bool stopped = false;
//  if (m_refreshing) {
//    stopPageRefresh();
//    stopped = true;
//  }
  if (TH1* selectedHisto = selectedHistogram()) {
    selectedHisto->DrawPanel();
  }
//  if (stopped) {
////    startPageRefresh();
//  }
}
void PresenterMainFrame::inspectHistogram()
{
  // TODO: stopped needs to be killed off...
//  bool stopped = false;
//  if (m_refreshing) {
//    stopPageRefresh();
//    stopped = true;
//  }
  // TODO: draw rather ClonePad
  if (TH1* selectedHisto = selectedHistogram()) {
    TCanvas* zoomCanvas =  new TCanvas();
//    zoomCanvas->SetBit(kCanDelete);
    zoomCanvas->cd();
    zoomCanvas->SetTitle(selectedHisto->GetTitle());
    selectedHisto->DrawCopy();
    zoomCanvas->ToggleEventStatus();
    zoomCanvas->ToggleAutoExec();
    zoomCanvas->ToggleToolBar();
    zoomCanvas->ToggleEditor();
  }
//  if (stopped) {
//    startPageRefresh();
//  }
}
void PresenterMainFrame::inspectPage()
{
  // TODO: stopped needs to be killed off...
//  bool stopped = false;
//  if (m_refreshing) {
//    stopPageRefresh();
//    stopped = true;
//  }
  TCanvas* inspectCanvas =  new TCanvas();
//  inspectCanvas->SetBit(kCanDelete);
  inspectCanvas->cd();
  inspectCanvas->SetTitle("Inspect Page");
  editorCanvas->DrawClonePad();
  inspectCanvas->ToggleEventStatus();
  inspectCanvas->ToggleAutoExec();
  inspectCanvas->ToggleToolBar();
  inspectCanvas->ToggleEditor();

//  if (stopped) {
//    startPageRefresh();
//  }
}
void PresenterMainFrame::hideDBTools()
{
  m_histoDBFilterComboBox->SetEnabled(false);

  m_histoDBCanvasViewPort->HideFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->HideFrame(m_pagesFromHistoDBListTree);
//  m_databaseHistogramTreeList->SetToolTipText(
//      std::string("Please login to DB for Histogram list").c_str(),
//      1, 1, 250);
  m_dimContextMenu->DisableEntry(M_AddDimToDB_COMMAND);
  m_dimContextMenu->EnableEntry(M_AddDimToPage_COMMAND);
}
void PresenterMainFrame::showDBTools()
{
  m_histoDBFilterComboBox->SetEnabled(true);
//  m_databaseHistogramTreeList->SetToolTipText(
//    std::string("Please select Histogram from list").c_str(), 1, 1, 20);
  m_databaseHistogramTreeList->SetBackgroundColor(GetWhitePixel());
  m_histoDBCanvasViewPort->ShowFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->ShowFrame(m_pagesFromHistoDBListTree);

  m_dimContextMenu->EnableEntry(M_AddDimToDB_COMMAND);
  m_dimContextMenu->DisableEntry(M_AddDimToPage_COMMAND);
}
void PresenterMainFrame::refreshHistoDBListTree()
{
  //TODO: add threading(?!)...
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
  listFromHistogramDB(m_databaseHistogramTreeList,
    static_cast<FilterCriteria>(m_histoDBFilterComboBox->GetSelected()),
    WithHistograms);
}
void PresenterMainFrame::refreshPagesDBListTree()
{
  if (m_connectedToHistogramDB) {
    m_histoDBContextMenu->EnableEntry(M_AddDBHistoToPage_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_DeleteDBHisto_COMMAND);
    //TODO: add threading...
    listFromHistogramDB(m_pagesFromHistoDBListTree, FoldersAndPages,
                          WithoutHistograms);    
  } else {
    m_histoDBContextMenu->DisableEntry(M_AddDBHistoToPage_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_DeleteDBHisto_COMMAND);
  }
}
void PresenterMainFrame::refreshDimSvcListTree()
{
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());
  
  // See what we can do about DIM services...
  m_knownDimServices.clear();
  m_histogramTypes.clear();

  if (m_dimSvcListTree && m_dimBrowser) {
    const int nDimServers = m_dimBrowser->getServers();
    if (m_verbosity >= Verbose) {
      std::cout << "nDimServers:\t" << nDimServers << std::endl;
    }

    TString dimServerName;
    TString dimServerNodeName;
    TString dimServiceName;
    TString statusMessage;

    // If some servers found, discover what services are available
    if (nDimServers > 0) {
      // First look for a DNS
      const char* dimDnsServerNode = DimClient::getDnsNode();

      if (m_verbosity >= Verbose) {
        std::cout << std::endl << "DNS: " << dimDnsServerNode << std::endl;
      }

      setStatusBarText(dimDnsServerNode, 0);

      char* dimServer;
      char* dimServerNode;
      char* dimService;
      char* dimFormat;
      int   dimType;

      TGListTreeItem* node = m_dimSvcListTree->GetFirstItem();

      // name DNS root m_treeNode
      m_dimSvcListTree->RenameItem(node, dimDnsServerNode);
      deleteTreeChildrenItemsUserData(node);
      m_dimSvcListTree->DeleteChildren(node);

      while (m_dimBrowser->getNextServer(dimServer, dimServerNode)) {
        m_dimBrowser->getServerServices(dimServer);
        dimServerName =  TString(dimServer);
        dimServerNodeName =  TString(dimServerNode);

        // get name of this server
        if (m_verbosity >= Verbose) {
          std::cout << "\t|_ " << dimServerNodeName << "\t-\t" <<
            dimServerName << std::endl;
        }

        // insert DNS root m_treeNode
        if (dimServerName.BeginsWith("DIS_DNS")) {
          m_dimSvcListTree->RenameItem(node, dimDnsServerNode);
        } else {
          while(dimType = m_dimBrowser->getNextServerService(dimService,
                dimFormat)) {
            if (TString(dimService).BeginsWith(H1D) ||
                TString(dimService).BeginsWith(H2D) ||
                TString(dimService).BeginsWith(P1D) ||
                TString(dimService).BeginsWith(HPD) ||
                TString(dimService).BeginsWith(P2D) ||
                TString(dimService).BeginsWith(CNT) ) {
              if (!TString(dimService).EndsWith("gauchocomment")) {
                HistogramIdentifier histogram = HistogramIdentifier(dimService);
                if (histogram.isDimFormat()) {
                  m_knownDimServices.push_back(std::string(dimService));
                  m_histogramTypes.push_back(histogram.histogramType());
                } else {
                  new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                    Form("The DIM servicename\n%s\n does not appear to follow the" \
                      "convention\nPlease use the following format:\n" \
                      "TYP/UTGID/Algorithmname/Histogramname\n where the UTGID " \
                      "normally has the following format:\n node00101_taskname_1 " \
                      "or simply taskname, without \"_\"", dimService),
                    kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
                }
              }
            }
          }
        }
      }
      fillTreeNodeWithHistograms(m_dimSvcListTree,
        m_dimSvcListTree->GetFirstItem(), &m_knownDimServices,
        &m_histogramTypes);
    }
  } else {
    m_dimSvcListTree->RenameItem(m_dimSvcListTree->GetFirstItem(),
      "No DIM");
//    if (m_verbosity >= Verbose) {
//      std::cout << "Sorry, no DIM server found." << std::endl;
//    }
  new TGMsgBox(fClient->GetRoot(), this, "DIM Error",
           "Sorry, no DIM server found", kMBIconExclamation,
          kMBOk, &m_msgBoxReturnCode);
  }

gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
  
  fClient->NeedRedraw(m_dimSvcListTree);
}
void PresenterMainFrame::clickedDimTreeItem(TGListTreeItem* node,
                                            MouseButton btn,
                                            Int_t x, Int_t y)
{
  // if right button, popup context menu
  if (node && btn == RightMouseButton) {
    m_dimContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::clickedHistoDBTreeItem(TGListTreeItem* node,
                                                MouseButton btn,
                                                Int_t x, Int_t y)
{
  // if right button, popup context menu
  if (node && btn == RightMouseButton) {
    m_histoDBContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::clickedPageTreeItem(TGListTreeItem* node,
                                             MouseButton btn,
                                             Int_t x, Int_t y)
{
  // if right button, popup context menu
  if (node && btn == RightMouseButton) {
    m_pagesContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::addDimSvcToHistoDB()
{

  disableAutoCanvasLayoutBtn();

  disableHistogramClearing();

  TGListTree *list = new TGListTree();
  checkedTreeItems(list, m_dimSvcListTree);

  TGListTreeItem *currentNode;
  currentNode = list->GetFirstItem();
  while(currentNode) {
    try {
      if (m_histogramDB) {
        m_histogramDB->declareHistByServiceName(
          std::string(*static_cast<TString*>(currentNode->GetUserData())));
      }
    } catch (std::string sqlException) {
      // TODO: add error logging backend
      if (m_verbosity >= Verbose) { std::cout << sqlException; }

      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
    currentNode = currentNode->GetNextSibling();
  }
  try {
    if (m_histogramDB) {
      m_histogramDB->commit(); 
      refreshHistoDBListTree();
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }

    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",
                      sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  list->Delete();
  delete list;
  list = NULL;

  checkTreeChildrenItems(m_dimSvcListTree->GetFirstItem(),
    UncheckTreeItems);
  fClient->NeedRedraw(m_dimSvcListTree);

  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::addDimSvcToPage()
{
  disableAutoCanvasLayoutBtn();
  // TODO: add locking, exclusive, disable autolayout
  disableHistogramClearing();

  fClient->WaitFor(dynamic_cast<TGWindow*>(
    new HistoPropDialog(this, 646, 435, m_verbosity)));

  TGListTree *list = new TGListTree();
  checkedTreeItems(list, m_dimSvcListTree);

  TGListTreeItem *currentNode;
  currentNode = list->GetFirstItem();

  while (currentNode) {
    int newHistoInstance = 0;
    // see if the histogram object exists already
    for (m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
        m_DbHistosOnPageIt != m_DbHistosOnPage.end();
        ++m_DbHistosOnPageIt) {
      int existingHistoInstance = (*m_DbHistosOnPageIt)->instance();

      if (0 == (convDimToHistoID(
          std::string(*static_cast<TString*>(currentNode->GetUserData()))))
            .compare((*m_DbHistosOnPageIt)->identifier()) &&
          (newHistoInstance < existingHistoInstance) ) {
        newHistoInstance = existingHistoInstance;
      }
    }
    //TODO: ? limit
    if (newHistoInstance > 998) { newHistoInstance = 0; }
    newHistoInstance++;
    //  Identifier, dimServiceName, refreshrate, instance, histogramDB
    DbRootHist* dbRootHist = new DbRootHist(
      convDimToHistoID(
        std::string(*static_cast<TString*>(currentNode->GetUserData()))),
        std::string(*static_cast<TString*>(currentNode->GetUserData())),
      2, newHistoInstance, NULL, analysisLib(), NULL);

    m_DbHistosOnPage.push_back(dbRootHist);

    // Set Properties
    TString paintDrawXLabel = m_bulkHistoOptions.m_xLabel;
    TString paintDrawYLabel = m_bulkHistoOptions.m_yLabel;
    int paintLineWidth = m_bulkHistoOptions.m_lineWidth;
    int paintFillColour = m_bulkHistoOptions.m_fillColour;
    int paintFillStyle = m_bulkHistoOptions.m_fillStyle;
    int paintLineStyle = m_bulkHistoOptions.m_lineStyle;
    int paintLineColour = m_bulkHistoOptions.m_lineColour;
    bool paintStats = m_bulkHistoOptions.m_statsOption;
    dbRootHist->rootHistogram->SetXTitle(paintDrawXLabel.Data());
    dbRootHist->rootHistogram->SetYTitle(paintDrawYLabel.Data());
    dbRootHist->rootHistogram->SetLineWidth(paintLineWidth);
    dbRootHist->rootHistogram->SetFillColor(paintFillColour);
    dbRootHist->rootHistogram->SetFillStyle(paintFillStyle);
    dbRootHist->rootHistogram->SetLineStyle(paintLineStyle);
    dbRootHist->rootHistogram->SetLineColor(paintLineColour);
    dbRootHist->rootHistogram->SetStats((bool)paintStats);

    TString paintDrawOption;
    if (H1D == dbRootHist->hstype() ||
        P1D == dbRootHist->hstype() ||
        HPD == dbRootHist->hstype()) {
      m_drawOption = m_bulkHistoOptions.m_1DRootDrawOption;
      paintDrawOption = TString(m_drawOption + TString(" ") +
        m_bulkHistoOptions.m_genericRootDrawOption).Data();
    } else if (H2D == dbRootHist->hstype()) {
      m_drawOption = m_bulkHistoOptions.m_2DRootDrawOption;
      paintDrawOption = TString(m_drawOption + TString(" ") +
        m_bulkHistoOptions.m_genericRootDrawOption).Data();
    }

    dbRootHist->rootHistogram->SetOption(paintDrawOption.Data());

    paintHist(dbRootHist);

    currentNode = currentNode->GetNextSibling();
  }
  list->Delete();
  delete list;
  list = NULL;

  checkTreeChildrenItems(m_dimSvcListTree->GetFirstItem(),
    UncheckTreeItems);
  fClient->NeedRedraw(m_dimSvcListTree);

  editorCanvas->Update();

  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::addDbHistoToPage()
{
  disableAutoCanvasLayoutBtn();
  try {
    if (m_histogramDB) {
      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);
      TGListTreeItem* currentNode = list->GetFirstItem();
      while (currentNode) {
        int newHistoInstance = 0;
        // see if the histogram object exists already
        for (m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
             m_DbHistosOnPageIt != m_DbHistosOnPage.end();
             ++m_DbHistosOnPageIt) {
          int existingHistoInstance = (*m_DbHistosOnPageIt)->instance();
          if (0 == (std::string(*static_cast<TString*>(currentNode->
                    GetUserData()))).compare((*m_DbHistosOnPageIt)->
                    identifier()) &&
              newHistoInstance < existingHistoInstance) {
            newHistoInstance = existingHistoInstance;
          }
        }
        // ? limit
        if (newHistoInstance > 998) { newHistoInstance = 0; }
        newHistoInstance++;

//OnlineHistogram* dimQuery = new OnlineHistogram(
// *(dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)),
// std::string(*static_cast<TString*>(currentNode->GetUserData())),
// std::string("_NONE_"), 1);
 
  OnlineHistogram* dimQuery = m_histogramDB->getHistogram(
    std::string(*static_cast<TString*>(currentNode->GetUserData())) );

        //  Identifier, dimServiceName, refreshrate, instance, histogramDB        
        DbRootHist* dbRootHist = new DbRootHist(
          std::string(*static_cast<TString*>(currentNode->GetUserData())),
          dimQuery->dimServiceName(), 2, newHistoInstance, m_histogramDB,
          analysisLib(), dimQuery);

        m_DbHistosOnPage.push_back(dbRootHist);
        paintHist(dbRootHist);

        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }

    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",
                       sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }

  checkTreeChildrenItems(
    m_databaseHistogramTreeList->GetFirstItem(), UncheckTreeItems);
  fClient->NeedRedraw(m_databaseHistogramTreeList);

  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::dimCheckAllChildren()
{
  if (m_dimSvcListTree && m_dimSvcListTree->GetSelected()) {
    checkTreeChildrenItems(
      m_dimSvcListTree->GetSelected(), CheckTreeItems);
    fClient->NeedRedraw(m_dimSvcListTree);
  }
}
void PresenterMainFrame::dimUnCheckAllChildren()
{
  if (m_dimSvcListTree && m_dimSvcListTree->GetSelected()) {
    checkTreeChildrenItems(
      m_dimSvcListTree->GetSelected(), UncheckTreeItems);
    fClient->NeedRedraw(m_dimSvcListTree);
  }
}
void PresenterMainFrame::dimCollapseAllChildren()
{
  if (m_dimSvcListTree && m_dimSvcListTree->GetSelected()) {
    collapseTreeChildrenItems(
      m_dimSvcListTree, m_dimSvcListTree->GetSelected());
    fClient->NeedRedraw(m_dimSvcListTree);
  }
}
void PresenterMainFrame::dbHistoCheckAllChildren()
{
  if (m_databaseHistogramTreeList &&
      m_databaseHistogramTreeList->GetSelected()) {
    checkTreeChildrenItems(
      m_databaseHistogramTreeList->GetSelected(), CheckTreeItems);
    fClient->NeedRedraw(m_databaseHistogramTreeList);
  }
}
void PresenterMainFrame::dbHistoUnCheckAllChildren()
{
  if (m_databaseHistogramTreeList &&
      m_databaseHistogramTreeList->GetSelected()) {
    checkTreeChildrenItems(
      m_databaseHistogramTreeList->GetSelected(), UncheckTreeItems);
    fClient->NeedRedraw(m_databaseHistogramTreeList);
  }
}
void PresenterMainFrame::dbHistoCollapseAllChildren()
{
  if (m_databaseHistogramTreeList &&
      m_databaseHistogramTreeList->GetSelected()) {
    collapseTreeChildrenItems(
      m_databaseHistogramTreeList,
      m_databaseHistogramTreeList->GetSelected());
    fClient->NeedRedraw(m_databaseHistogramTreeList);
  }
}
std::string PresenterMainFrame::convDimToHistoID(std::string dimSvcName)
{
  HistogramIdentifier histogram = HistogramIdentifier(dimSvcName);
  if (histogram.isDimFormat()) {
    return histogram.histogramIdentifier();
  } else {
    new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                 Form("The DIM servicename\n%s\n does not appear to follow the" \
                   "convention\nPlease use the following format:\n" \
                   "HnD/UTGID/Algorithmname/Histogramname\n where the UTGID " \
                   "normally has the following format:\n node00101_taskname_1 " \
                   "or simply taskname, without \"_\"", dimSvcName.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    return 0;
  }
}
void PresenterMainFrame::setHistogramPropertiesInDB()
{
  try {
    if (m_histogramDB) {
      fClient->WaitFor(dynamic_cast<TGWindow*>(
        new HistoPropDialog(this, 646, 435, m_verbosity)));

      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem* currentNode;
      currentNode = list->GetFirstItem();
      while(currentNode) {
        OnlineHistogram* onlineHistogramToSet = m_histogramDB->
          getHistogram(std::string(*static_cast<TString*>(
                       currentNode->GetUserData())));

        std::string paintDrawXLabel = m_bulkHistoOptions.
          m_xLabel.Data();
        std::string paintDrawYLabel = m_bulkHistoOptions.
          m_yLabel.Data();
        int paintLineWidth = m_bulkHistoOptions.m_lineWidth;
        int paintFillColour = m_bulkHistoOptions.m_fillColour;
        int paintFillStyle = m_bulkHistoOptions.m_fillStyle;
        int paintLineStyle = m_bulkHistoOptions.m_lineStyle;
        int paintLineColour = m_bulkHistoOptions.m_lineColour;
        //bool paintStats = m_bulkHistoOptions.m_statsOption;

        if (H1D == onlineHistogramToSet->hstype() ||
            P1D == onlineHistogramToSet->hstype() ||
            HPD == onlineHistogramToSet->hstype()) {
          m_drawOption = m_bulkHistoOptions.m_1DRootDrawOption;
        } else if (H2D == onlineHistogramToSet->hstype()) {
          m_drawOption = m_bulkHistoOptions.m_2DRootDrawOption;
        }
        std::string paintDrawOption = TString(
          m_drawOption + TString(" ") +
          m_bulkHistoOptions.m_genericRootDrawOption).Data();

        onlineHistogramToSet->setDisplayOption("LABEL_X",
                                                   &paintDrawXLabel);
        onlineHistogramToSet->setDisplayOption("LABEL_Y",
                                                   &paintDrawYLabel);
        onlineHistogramToSet->setDisplayOption("FILLSTYLE",
                                                   &paintFillStyle);
        onlineHistogramToSet->setDisplayOption("FILLCOLOR",
                                                   &paintFillColour);
        onlineHistogramToSet->setDisplayOption("LINESTYLE",
                                                   &paintLineStyle);
        onlineHistogramToSet->setDisplayOption("LINECOLOR",
                                                   &paintLineColour);
        onlineHistogramToSet->setDisplayOption("LINEWIDTH",
                                                   &paintLineWidth);
	//        onlineHistogramToSet->setDisplayOption("STATS",
        //                                           &paintStats);
        onlineHistogramToSet->setDisplayOption("DRAWOPTS",
                                                   &paintDrawOption);

        onlineHistogramToSet->saveDisplayOptions();
        
        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
      m_histogramDB->commit();
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }

    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
        Form("OnlineHistDB server:\n\n%s\n",
             sqlException.c_str()),
             kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  refreshHistoDBListTree();
}
void PresenterMainFrame::deleteSelectedHistoFromDB()
{
  new TGMsgBox(fClient->GetRoot(), this, "Delete histogram",
               "Are you sure to delete selected histogram from the database?",
               kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
  switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
  }
  try {
    if (m_histogramDB) {
      TGListTree *list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem *currentNode;
      currentNode = list->GetFirstItem();
      while (currentNode) {
	std::string hist_id = std::string
	  (*static_cast<TString*>(currentNode->GetUserData()));
        OnlineHistogram* histoToDelete = m_histogramDB->getHistogram(hist_id);
	if(histoToDelete) {
	  // remove this histogram from current page if present
	  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
	  while (m_DbHistosOnPageIt  != m_DbHistosOnPage.end() ) {
	    if ( (*m_DbHistosOnPageIt)->identifier() == hist_id ) {
	      delete *m_DbHistosOnPageIt;
	      *m_DbHistosOnPageIt = NULL;
	      m_DbHistosOnPageIt = m_DbHistosOnPage.erase(m_DbHistosOnPageIt);
	    }
	    else {
	      m_DbHistosOnPageIt++;
	    }
	  }
	  editorCanvas->Update();
	  // remove from DB
	  m_histogramDB->removeHistogram(histoToDelete, false);
	}
	currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
      m_histogramDB->commit();
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }
    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",
                      sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  refreshHistoDBListTree();
}
void PresenterMainFrame::enablePageLoading() {
  m_pagesContextMenu->EnableEntry(M_LoadPage_COMMAND);
}
void PresenterMainFrame::loadSelectedPageFromDB()
{
  if (m_connectedToHistogramDB) {    
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());

    disableAutoCanvasLayoutBtn();
    removeHistogramsFromPage();
    
    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();
    if (node && node->GetUserData()) {
      std::string path = std::string(*static_cast<TString*>(
                                     node->GetUserData()));
      try {
        double xlow = 0.0;
        double ylow = 0.0;
        double xup = 0.0;
        double yup = 0.0;

        removeHistogramsFromPage();
        m_onlineHistosOnPage.clear();        
        OnlineHistPage* page = m_histogramDB->getPage(path);
        if (page) {
          page->getHistogramList(&m_onlineHistosOnPage);
          m_onlineHistosOnPageIt = m_onlineHistosOnPage.begin();
          while (m_onlineHistosOnPageIt != m_onlineHistosOnPage.end()) {
            DbRootHist* dbRootHist = new DbRootHist(
              (*m_onlineHistosOnPageIt)->identifier(),
              (*m_onlineHistosOnPageIt)->dimServiceName(),
              1, 2, m_histogramDB, analysisLib(), 
	      *m_onlineHistosOnPageIt);

            m_DbHistosOnPage.push_back(dbRootHist);

            page->getHistLayout(
              *m_onlineHistosOnPageIt, xlow, ylow, xup, yup,
              (*m_onlineHistosOnPageIt)->instance());
            TPad* pad = new TPad(
              dbRootHist->histoRootName().c_str(), TString(""),
              TMath::Abs(xlow), TMath::Abs(ylow),
              TMath::Abs(xup), TMath::Abs(yup));
            pad->SetBit(kNoContextMenu);
            pad->SetFillColor(10);
            editorCanvas->cd();
            pad->Draw();
            pad->cd();

            if (dbRootHist && pad) {
              if (dbRootHist->rootHistogram) {
//dbRootHist->Draw();
dbRootHist->Draw(pad);
              }
              dbRootHist->hostingPad = pad;
            }
            m_onlineHistosOnPageIt++;
          }
        }

      } catch (std::string sqlException) {
        // TODO: add error logging backend - MsgStream?
        setStatusBarText(sqlException.c_str(), 0);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could save the page to OnlineHistDB:\n\n%s\n",
                       sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }

gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
    
  }
  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::deleteSelectedPageFromDB()
{
  new TGMsgBox(fClient->GetRoot(), this, "Delete Page",
               "Are you sure to delete selected page from the database?",
               kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
  switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
  }

  if (m_connectedToHistogramDB) {
    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();

    if (node && node->GetUserData()) {
      std::string path = std::string(*static_cast<TString*>(
                                     node->GetUserData()));
      try {
        OnlineHistPage* page = m_histogramDB->getPage(path);
        if (page) {
          m_histogramDB->removePage(page);
          m_histogramDB->commit();
          refreshPagesDBListTree();
        }
      } catch (std::string sqlException) {
        // TODO: add error logging backend - MsgStream?
        setStatusBarText(sqlException.c_str(), 0);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not delete the page to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}
void PresenterMainFrame::deleteSelectedFolderFromDB()
{
// new TGMsgBox(fClient->GetRoot(), this, "Delete Folder",
//              "Are you sure to delete selected folder from the Database?",
//              kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//  switch (m_msgBoxReturnCode) {
//    case kMBNo:
//      return;
//  }
  if (m_connectedToHistogramDB) {

    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();

    if (node) {
      char path[1024];
      m_pagesFromHistoDBListTree->GetPathnameFromItem(node, path);
      std::string folder = std::string(path);
      // Drop DB url
      folder = folder.erase(0, std::string(
        m_pagesFromHistoDBListTree->GetFirstItem()->GetText()).length()+1);
      try {
        m_histogramDB->removePageFolder(folder);
        m_histogramDB->commit();
        refreshPagesDBListTree();
      } catch (std::string sqlException) {
        // TODO: add error logging backend - MsgStream?
        setStatusBarText(sqlException.c_str(), 0);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could delete the page to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}
void PresenterMainFrame::setHistogramParametersFromDB(TH1* histogram,
                                    OnlineHistogram* onlineHistogram)
{
  histogram->SetNameTitle(onlineHistogram->dimServiceName().c_str(),
                          onlineHistogram->hsname().c_str());
}
void PresenterMainFrame::paintHist(DbRootHist* histogram)
{
  //  xlow [0, 1] is the position of the bottom left point of the pad
  //             expressed in the mother pad reference system
  //  ylow [0, 1] is the Y position of this point.
  //  xup  [0, 1] is the x position of the top right point of the pad
  //             expressed in the mother pad reference system
  //  yup  [0, 1] is the Y position of this point.

  double xlow = 0.1 + 0.01*m_histoPadOffset;
  double ylow = xlow;
  double xup = 0.5 + 0.01*m_histoPadOffset;
  double yup = xup;

  if (0.5 == xlow) {
    m_histoPadOffset = 0;
  }

  m_histoPadOffset++;

  TPad* pad;
  if (histogram) {
    pad = new TPad(histogram->histoRootName().c_str(), TString("Title"),
                   xlow, ylow, xup, yup);
    pad->SetBit(kNoContextMenu);
    pad->SetFillColor(10);
    editorCanvas->cd();
    pad->Draw();
    pad->cd();
    
    if (pad && histogram->rootHistogram) {
//      if (H2D != histogram->hstype()) {

// histogram->rootHistogram->Draw();
histogram->Draw(pad);

//      } else {
//        TImage *img = TImage::Create();
//        img->SetImage((const Double_t *)(((TH2D*)(histogram->rootHistogram))->GetArray()),
//                      histogram->rootHistogram->GetNbinsX() + 1, 
//                      histogram->rootHistogram->GetNbinsY() + 1, gHistImagePalette);
//        img->Draw(); 
//      }
//    histogram->setHostPad(pad);
      histogram->hostingPad = pad;
    }
  }
}
void PresenterMainFrame::autoCanvasLayout()
{
  double xmargin = 0.01;
  double ymargin = 0.01;
  int nx = (int) ceil(sqrt((double)m_DbHistosOnPage.size()));
  int ny = nx;

  if (nx <= 0) { nx = 1; }
  if (ny <= 0) { ny = 1; }
  Int_t ix, iy;
  double x1, y1, x2, y2;
  double dx, dy;

  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();

  if (xmargin > 0 && ymargin > 0) {
    dy = 1/double(ny);
    dx = 1/double(nx);
    for (iy = 0; iy < ny; ++iy) {
      if (m_DbHistosOnPage.end() == m_DbHistosOnPageIt)  { break; }
      y2 = 1 - iy*dy - ymargin;
      y1 = y2 - dy + 2*ymargin;
      if (y1 < 0) { y1 = 0; }
      if (y1 > y2) { continue; }
      for (ix = 0; ix < nx; ++ix) {
        if (m_DbHistosOnPage.end() == m_DbHistosOnPageIt)  { break; }
        x1 = ix*dx + xmargin;
        x2 = x1 + dx - 2*xmargin;
        if (x1 > x2) { continue; }
        if ((*m_DbHistosOnPageIt) && (*m_DbHistosOnPageIt)->hostingPad) {
          ((*m_DbHistosOnPageIt)->hostingPad)->SetPad(x1, y1, x2, y2);
          ((*m_DbHistosOnPageIt)->hostingPad)->Modified();
        }
        ++m_DbHistosOnPageIt;
      }
    }
  }
  editorCanvas->Update();
}
void PresenterMainFrame::deleteSelectedHistoFromCanvas()
{
  bool foundSelectedHisto = false;
  if (gPad) {
    m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
    while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
      if (0 == ((*m_DbHistosOnPageIt)->histoRootName()).
                compare((gPad->GetName()))) {
        foundSelectedHisto = true;
        break;
      }
      ++m_DbHistosOnPageIt;
    }
    if (foundSelectedHisto) {
      new TGMsgBox(fClient->GetRoot(), this, "Remove Histogram",
                   "Are you sure to remove selected histogram from the page?",
                   kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
      switch (m_msgBoxReturnCode) {
        case kMBYes:
          delete (*m_DbHistosOnPageIt);
          (*m_DbHistosOnPageIt) = NULL;
          m_DbHistosOnPage.erase(m_DbHistosOnPageIt);
          editorCanvas->Update();
          return;
          break;
        default:
          break;
        }
      } else {
        new TGMsgBox(fClient->GetRoot(), this, "Remove Histogram",
                     "Please select a histogram with the middle mouse button",
                     kMBIconStop, kMBOk, &m_msgBoxReturnCode);
      }
  }
}
void PresenterMainFrame::refreshPage()
{
  if (m_verbosity >= Verbose) {
    std::cout << "refreshing." << std::endl;
  }
  
  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
  while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
    (*m_DbHistosOnPageIt)->fillHistogram();
    (*m_DbHistosOnPageIt)->normalizeReference();
    ++m_DbHistosOnPageIt;
  }
  editorCanvas->Update();
}
void PresenterMainFrame::enableHistogramClearing()
{
  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
  while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
    (*m_DbHistosOnPageIt)->enableClear();
    ++m_DbHistosOnPageIt;
  }
}
void PresenterMainFrame::disableHistogramClearing()
{
  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
  while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
    (*m_DbHistosOnPageIt)->disableClear();
    ++m_DbHistosOnPageIt;
  }
}
void PresenterMainFrame::enablePageRefresh()
{
  editorCanvas->SetEditable(false);
  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
  while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
    (*m_DbHistosOnPageIt)->disableEdit();
    ++m_DbHistosOnPageIt;
  }
}
void PresenterMainFrame::disablePageRefresh()
{
  editorCanvas->SetEditable(true);
  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
  while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
    (*m_DbHistosOnPageIt)->enableEdit();
    ++m_DbHistosOnPageIt;
  }
}
void PresenterMainFrame::enablePageUndocking()
{
  m_viewMenu->EnableEntry(UNDOCK_PAGE_COMMAND);
}
void PresenterMainFrame::disablePageUndocking()
{
  m_viewMenu->DisableEntry(UNDOCK_PAGE_COMMAND);
}
void PresenterMainFrame::removeHistogramsFromPage()
{
//  new TGMsgBox(fClient->GetRoot(), this, "Clear Page",
//        "Do you really want to clear the current page?",
//        kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//    switch (m_msgBoxReturnCode) {
//      case kMBNo:
//        return;
//  }
  disablePageRefresh();
  stopPageRefresh();
  editorCanvas->SetEditable(true);

  if (m_verbosity >= Verbose) {
    std::cout << "clearCanvas." << std::endl;
  }

  m_DbHistosOnPageIt = m_DbHistosOnPage.begin();
  while (m_DbHistosOnPageIt != m_DbHistosOnPage.end()) {
    if (*m_DbHistosOnPageIt) {
      delete *m_DbHistosOnPageIt;
      *m_DbHistosOnPageIt = NULL;
    }
    ++m_DbHistosOnPageIt;
  }
  m_DbHistosOnPage.clear();
  m_histoPadOffset = 0;

  editorCanvas->cd();
  editorCanvas->Clear();
  editorCanvas->Update();
}
void PresenterMainFrame::EventInfo(int , int px, int py, TObject *selected)
{
  const char *text0, *text1, *text3;
  text0 = selected->GetTitle();
  setStatusBarText(text0, 0);
  text1 = selected->GetName();
  setStatusBarText(text1, 1);
  text3 = selected->GetObjectInfo(px, py);

  setStatusBarText(text3, 2);
}
