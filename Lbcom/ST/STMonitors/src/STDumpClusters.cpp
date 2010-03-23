// $Id: STDumpClusters.cpp,v 1.1 2010-03-23 09:19:35 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/STCluster.h"
#include "Event/ODIN.h"
#include "Kernel/ISTReadoutTool.h"

// local
#include "STDumpClusters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STDumpClusters
//
// 2010-03-15 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STDumpClusters );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STDumpClusters::STDumpClusters( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : ST::TupleAlgBase ( name , pSvcLocator )
{
  // Input location for clusters...
  declareSTConfigProperty("ClusterLocation", m_clusterLocation, 
                          LHCb::STClusterLocation::TTClusters);
  /* Set the maximum number of clusters in the event (sets the size of the 
     tuple arrays) */
  declareProperty("MaxNumberClusters", m_nClusters=5000);
}
//=============================================================================
// Destructor
//=============================================================================
ST::STDumpClusters::~STDumpClusters() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STDumpClusters::initialize() {
  StatusCode sc = ST::TupleAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STDumpClusters::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  counter("Number of events") += 1;
  if(exist<LHCb::STClusters>(m_clusterLocation)){
    LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);
    
    const unsigned int nClusters = clusters->size();
    if ( msgLevel(MSG::DEBUG) ) debug() << "Number of clusters in " << m_clusterLocation << " is " << nClusters << endmsg;

    const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    const Gaudi::Time odinTime = odin->eventTime();

    Tuple tuple = nTuple( "Clusters", "Information stored on the cluster" );

    std::vector<double> clusterCharge(m_nClusters,0.);
    std::vector<unsigned int> clusterSize(m_nClusters,0);
    std::vector<unsigned int> uniqueSector(m_nClusters,0);
    std::vector<unsigned int> channelID(m_nClusters,0);
    std::vector<unsigned int> strip(m_nClusters,0);
    std::vector<unsigned int> station(m_nClusters,0);
    std::vector<unsigned int> layer(m_nClusters,0);
    std::vector<unsigned int> detRegion(m_nClusters,0);
    std::vector<unsigned int> sector(m_nClusters,0);
    std::vector<double> interStripFrac(m_nClusters,0.);
    std::vector<unsigned int> tell1ID(m_nClusters,0);
    std::vector<unsigned int> tell1Channel(m_nClusters,0);
    //    std::vector<unsigned int> countCluster(m_nClusters,0);
    std::vector< std::vector<unsigned int> > adcs(4, std::vector<unsigned int>(m_nClusters,0));
    std::vector< std::vector<unsigned int> > strips(4, std::vector<unsigned int>(m_nClusters,0));
    // Loop over clusters
    LHCb::STClusters::const_iterator itClus = clusters->begin();
    unsigned int iClus = 0;
    if(nClusters > m_nClusters) debug() << "Number of clusters bigger than buffer size: n=" << nClusters 
                                        << ",buf=" << m_nClusters << endreq;
    for(; itClus != clusters->end(); ++itClus, ++iClus) {
      // only fill ntuple for clusters less than tuple array size
      if(iClus < m_nClusters) {
        const LHCb::STCluster* cluster = (*itClus);
        clusterSize[iClus]    = cluster->size();
        clusterCharge[iClus]  = cluster->totalCharge();
        uniqueSector[iClus]   = cluster->channelID().uniqueSector();
        channelID[iClus]      = cluster->channelID();
        strip[iClus]          = cluster->strip();
        station[iClus]        = cluster->station();
        layer[iClus]          = cluster->layer();
        detRegion[iClus]      = cluster->detRegion();
        sector[iClus]         = cluster->sector();
        interStripFrac[iClus] = cluster->interStripFraction();
        const unsigned int sourceID = cluster->sourceID();
        tell1ID[iClus] = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
        tell1Channel[iClus] = cluster->tell1Channel();
        //      countCluster[iClus] = iClus;
        unsigned int iStrip=0;
        std::vector<std::pair<int,unsigned int> >::const_iterator iStp = cluster->stripValues().begin();
        for(; iStp != cluster->stripValues().end(); ++iStp, ++iStrip) {
          strips[iStrip][iClus] = strip[iClus]+(*iStp).first;
          adcs[iStrip][iClus]   = (*iStp).second;
        }
      } 
    }// End of cluster iterator
    tuple->column( "run",   odin->runNumber() );
    tuple->column( "event", static_cast<unsigned int>(odin->eventNumber()) );
    tuple->column( "nClus", nClusters ); 
    if( fullDetail() ) {
      tuple->column( "day",    odinTime.day(0));
      tuple->column( "month",  odinTime.month(0)+1);
      tuple->column( "year",   odinTime.year(0));
      tuple->column( "hour",   odinTime.hour(0)); 
      tuple->column( "minute", odinTime.minute(0)); 
      tuple->column( "second", odinTime.second(0)); 
    }
    //    tuple->farray( "cluster",   countCluster,   "n", m_nClusters );
    tuple->farray( "tell1",     tell1ID,        "n", m_nClusters );
    tuple->farray( "tellChan",  tell1Channel,   "n", m_nClusters );
    tuple->farray( "channelID", channelID,      "n", m_nClusters );
    tuple->farray( "strip",     strip,          "n", m_nClusters );
    tuple->farray( "station",   station,        "n", m_nClusters );
    tuple->farray( "layer",     layer,          "n", m_nClusters );
    tuple->farray( "detRegion", station,        "n", m_nClusters );
    tuple->farray( "sector",    sector,         "n", m_nClusters );
    tuple->farray( "fracStrip", interStripFrac, "n", m_nClusters );
    tuple->farray( "size",      clusterSize,    "n", m_nClusters ); 
    tuple->farray( "charge",    clusterCharge,  "n", m_nClusters ); 
    tuple->farray( "strip1",    strips[0],      "n", m_nClusters );
    tuple->farray( "strip2",    strips[1],      "n", m_nClusters );
    tuple->farray( "strip3",    strips[2],      "n", m_nClusters );
    tuple->farray( "strip4",    strips[3],      "n", m_nClusters );
    tuple->farray( "adc1",      adcs[0],        "n", m_nClusters );
    tuple->farray( "adc2",      adcs[1],        "n", m_nClusters );
    tuple->farray( "adc3",      adcs[2],        "n", m_nClusters );
    tuple->farray( "adc4",      adcs[3],        "n", m_nClusters );

    tuple->write();

  }// end of exist condition

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STDumpClusters::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return ST::TupleAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
