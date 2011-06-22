// $Id: FarmLineDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for displays for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmLineDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
#ifndef ROMON_FARMLINEDISPLAY_H
#define ROMON_FARMLINEDISPLAY_H 1

// Framework includes
#include "ROMon/FarmDisplayBase.h"
/*
 *   ROMon namespace declaration
 */
namespace ROMon {


  /**@class ClusterLine ROMon.h GaudiOnline/Display.h
   *
   *   Display entry for the boot status of one subfarm
   *
   *   @author M.Frank
   */
  class ClusterLine  {
  protected:
    std::string        m_name;
    int                m_svc;
    size_t             m_position;
    FarmLineDisplay*   m_parent;
    Nodeset*           m_cluster;
    char*              m_ptr;

    /// End update cycle.
    void begin_update(const char* text=0);
    void end_update();

  public:
    /// Initializing constructor
    ClusterLine(FarmLineDisplay* p, int pos, const std::string& n);
    /// Default destructor
    virtual ~ClusterLine();
    /// Connect to data service
    void connect(const std::string& name);
    /// Name of the clusterline (=subfarm name)
    const std::string& name() const           { return m_name;      }
    const Nodeset* cluster() const            { return m_cluster;   }
    size_t position() const                   { return m_position;  }
    void check(time_t now);
    /// Hightlight cursor if current
    void set_cursor();
    /// Display function drawing on pasteboard of current display
    virtual void display();
    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
  };

  /**@class FarmLineDisplay ROMon.h GaudiOnline/FarmLineDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmLineDisplay : public FarmDisplayBase  {
  protected:
    typedef std::map<std::string, ClusterLine*> SubDisplays;
    typedef std::vector<std::string> Farms;
    SubDisplays                      m_farmDisplays;
    std::auto_ptr<PartitionListener> m_listener;
    std::string                      m_partition;
    std::string                      m_match;
    /// vector with all farm displays
    Farms                            m_farms;
    int                              m_height;
    int                              m_width;
    int                              m_dense;

    ClusterLine* m_currentLine;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

public:
    /// Standard constructor
    FarmLineDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmLineDisplay();

    /// Get the name of the currently selected cluster
    virtual std::string selectedCluster() const;

    /// Get the name of the currently selected cluster and node
    virtual std::pair<std::string,std::string> selectedNode() const;

    /// Number of sub-nodes in a cluster
    size_t selectedClusterSize() const;

    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    /// Get farm display from cursor position
    ClusterLine* currentDisplay()  const;

    /// Get farm display name from cursor position
    virtual std::string currentDisplayName()  const;

    /// Accessor to sub-displays of main panel
    SubDisplays& subDisplays() {  return m_farmDisplays; }

    /// Accessor to current subfarm display
    ClusterDisplay* subfarmDisplay() const {  return m_subfarmDisplay; }

    /// Set cursor to position
    virtual void set_cursor();

    /// Set cursor to position
    virtual void set_cursor(InternalDisplay* d) 
    { this->InternalDisplay::set_cursor(d); }

    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// Connect to data resources
    virtual void connect()  {  InternalDisplay::connect(); }
    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);
    /// DIM command service callback
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }

    /// DIM command service callback
    static void dnsDataHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_FARMLINEDISPLAY_H */
