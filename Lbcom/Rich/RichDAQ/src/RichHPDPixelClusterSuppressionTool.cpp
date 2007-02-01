
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.cpp
 *
 * Implementation file for class : RichHPDPixelClusterSuppressionTool
 *
 * CVS Log :-
 * $Id: RichHPDPixelClusterSuppressionTool.cpp,v 1.16 2007-02-01 17:42:29 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichHPDPixelClusterSuppressionTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( HPDPixelClusterSuppressionTool );

// Standard constructor
HPDPixelClusterSuppressionTool::
HPDPixelClusterSuppressionTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : HighOccHPDSuppressionTool ( type, name, parent )
{
  // Define interface
  declareInterface<IPixelSuppressionTool>(this);
  // job options
  declareProperty( "MaxPixelClusterSize",    m_maxPixClusterSize  = 10 );
  declareProperty( "MinHPDOccForClustering", m_minHPDocc          = 10 );
}

StatusCode HPDPixelClusterSuppressionTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HighOccHPDSuppressionTool::initialize();
  if ( sc.isFailure() ) return sc;

  // sanity checks
  if ( m_maxPixClusterSize > m_minHPDocc ) m_minHPDocc = m_maxPixClusterSize;

  info() << "  Max HPD pixel cluster size            = " << m_maxPixClusterSize << endreq
         << "  Min HPD occupancy for clustering      = " << m_minHPDocc << endreq;

  return sc;
}

bool
HPDPixelClusterSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{
  // check overall HPD suppression and update running average occupancy
  bool suppress =
    HighOccHPDSuppressionTool::applyPixelSuppression( hpdID, smartIDs );
  if ( suppress ) return true;

  // number of pixels before suppression
  const unsigned int startSize = smartIDs.size();

  // if occ below min for clustering, just return false (no suppression)
  if ( startSize < m_minHPDocc ) return false;

  // Make a local pixel data object
  PixelData pixelData( smartIDs );

  // cluster numbers
  int clusterID(0);

  // loop over pixels
  // requires them to be sorted by row then column
  // this should be automatic via decoding but its fast so safer to be sure
  SmartIDSorter::sortByRegion(smartIDs);
  for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    // get row and column data
    const int col     = (*iS).pixelCol();
    const int row     = (*iS).pixelRow();
    const int lastrow = row - 1;
    const int lastcol = col - 1;
    const int nextcol = col + 1;

    // Null cluster pointer
    PixelData::Cluster * clus(NULL);

    // check neighbouring pixels

    // last row and last column
    clus = pixelData.getCluster(lastrow,lastcol);

    // last row and same column
    PixelData::Cluster * newclus1 = pixelData.getCluster(lastrow,col);
    if ( newclus1 )
    {
      if ( clus && clus != newclus1 )
      { clus = pixelData.mergeClusters(clus,newclus1); }
      else { clus = newclus1; }
    }

    // last row and next column
    PixelData::Cluster * newclus2 = pixelData.getCluster(lastrow,nextcol);
    if ( newclus2 )
    {
      if ( clus && clus != newclus2 )
      { clus = pixelData.mergeClusters(clus,newclus2); }
      else { clus = newclus2; }
    }

    // this row and last column
    PixelData::Cluster * newclus3 = pixelData.getCluster(row,lastcol);
    if ( newclus3 )
    {
      if ( clus && clus != newclus3 )
      { clus = pixelData.mergeClusters(clus,newclus3); }
      else { clus = newclus3; }
    }

    // Did we find a neighbouring pixel cluster ?
    // If not, this is a new cluster
    if (!clus) { clus = pixelData.createNewCluster(++clusterID); }

    // assign final cluster to this pixel
    pixelData.setCluster(row,col,clus);

  } // pixel loop

    // Print out clustering results
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << hpdID << endreq << pixelData << endreq;
  }

  // apply pixel cluster suppression
  pixelData.suppressIDs(smartIDs,m_maxPixClusterSize);

  // was anything removed ?
  suppress = ( startSize != smartIDs.size() );

  if ( m_sumPrint && suppress )
  {
    // Print message
    std::ostringstream hpd;
    hpd << hpdID;
    Warning( "Partially suppressed HPD "+hpd.str(), StatusCode::SUCCESS, 10 );
  }

  // return status
  return suppress;
}

// ======================= Methods for PixelData ================================

MsgStream& HPDPixelClusterSuppressionTool::
PixelData::fillStream ( MsgStream & os ) const
{
  // column numbers
  os << " c    |";
  for ( int col = 0; col < PixClusInfo::nPixelRowsOrCols; ++col )
  {
    os << format("%3i",col);
  }
  os << endreq;

  // print out each row
  for ( int row = 0; row < PixClusInfo::nPixelRowsOrCols; ++row )
  {
    os << format( " r %2i | ", row );
    for ( int col = 0; col < PixClusInfo::nPixelRowsOrCols; ++col )
    {
      const Cluster * clus = getCluster(row,col);
      if ( clus ) { os << format("%2i ",clus->id()); }
      else        { os << " . "; }
    }
    os << endreq;
  }

  return os;
}

void HPDPixelClusterSuppressionTool::
PixelData::suppressIDs( LHCb::RichSmartID::Vector & smartIDs,
                        const unsigned int maxSize )
{
  LHCb::RichSmartID::Vector newSmartIDs;
  newSmartIDs.reserve(smartIDs.size());
  for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    if ( getCluster((*iS).pixelRow(),(*iS).pixelCol())->size() <= maxSize )
    {
      newSmartIDs.push_back(*iS);
    }
  }
  smartIDs = newSmartIDs;
}
