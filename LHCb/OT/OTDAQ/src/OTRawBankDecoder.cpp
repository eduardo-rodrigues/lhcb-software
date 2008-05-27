// $Id: OTRawBankDecoder.cpp,v 1.11 2008-05-27 13:46:27 hterrier Exp $
// Include files
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Event
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// local
#include "RawHit.h"
#include "GolHeaderDC06.h"
#include "GolHeaderV3.h"
#include "OTSpecificHeader.h"

//#include "Event/DataWord.h"

// GSL
#include <algorithm>

// local
#include "Event/OTBankVersion.h"

// Tool interface
#include "OTDAQ/IOTReadOutWindow.h"
#include "OTDAQ/IOTChannelMapTool.h"

// local
#include "OTRawBankDecoder.h"


namespace OTRawBankDecoderHelpers
{


  class Module
  {
  public:
    Module() : m_detelement(0), m_isdecoded(false), m_size(0), m_data(0) { m_ottimes.reserve(16) ; }
    void clearevent() { m_isdecoded=false ; m_size=0; m_data=0 ; m_ottimes.clear() ; }
    void setData( unsigned int size, const unsigned short* data, int bankversion) { 
      m_size = size ;  m_data = data ; m_bankversion = bankversion ; }
    bool isDecoded() const { return m_isdecoded ; }
    void setIsDecoded(bool b=true) { m_isdecoded = b ; }
    const unsigned short* data() const { return m_data ; }
    unsigned int size() const { return m_size ; }
    LHCb::OTLiteTimeContainer& ownedottimes() { return m_ottimes ; }
    LHCb::OTLiteTimeRange ottimes() const { return LHCb::OTLiteTimeRange(m_ottimes.begin(),m_ottimes.end()) ; }
    void setDetElement( const DeOTModule& e ) ;
    void setChannelMap( const OTDAQ::ChannelMap::Module& map ) { m_channelmap = &map ; }
    const DeOTModule& detelement() const { return *m_detelement ; }
    
    size_t decodeDC06(double tdcconversion) ;
    size_t decodeV3(double tdcconversion) ;
    size_t decode(double tdcconversion) ;
    
  private:
    void addHit(unsigned short data, float tdcconversion) ;
    
  private:
    const DeOTModule* m_detelement ;
    const OTDAQ::ChannelMap::Module* m_channelmap ;
    unsigned int m_station ;
    unsigned int m_layer ;
    unsigned int m_quarter ;
    unsigned int m_module ;
    
    bool m_isdecoded ;
    unsigned int m_size ;
    const unsigned short* m_data ;
    int m_bankversion ;
    LHCb::OTLiteTimeContainer m_ottimes ;
  } ;
  
  void Module::setDetElement( const DeOTModule& e) 
  {
    // cache for speed
    m_detelement = &e ;
    LHCb::OTChannelID moduleid = e.elementID() ;
    m_station = moduleid.station() ;
    m_layer   = moduleid.layer() ;
    m_quarter = moduleid.quarter() ;
    m_module  = moduleid.module() ;
  }
  
  inline void Module::addHit(unsigned short data, float tdcconversion) 
  { 
    OTDAQ::RawHit hit(data) ;
    assert( hit.otis()*32 +  hit.channelInOtis() == hit.channel() ) ;
    unsigned int straw = m_channelmap->straw( hit.channel() ) ;
    unsigned int tdctime = hit.time() ;
    LHCb::OTChannelID channelid(m_station,m_layer,m_quarter,m_module,straw,tdctime) ;
    float t0 = m_detelement->strawT0( straw ) ;
    m_ottimes.push_back( LHCb::OTLiteTime( channelid, tdctime * tdcconversion - t0) );
  } 
  
  inline size_t Module::decodeDC06(double tdcconversion) 
  {
    if(!m_isdecoded) {
      if( m_size != 0 ) {
        // now, DC06 has a padding problem: The padded hits appears before the last 
        // hit. So, we need to fix that. 
        bool haspaddinghit = m_data[m_size-2] == 0 ;
        const unsigned short* begin = m_data ;
        const unsigned short* end   = begin + (haspaddinghit ? m_size -2 : m_size) ;
        m_ottimes.reserve( m_size ) ;
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)
          addHit(*ihit,tdcconversion) ;
        if(haspaddinghit) addHit(m_data[m_size-1],tdcconversion) ;
      }
      m_isdecoded = true ;
    }
    return m_ottimes.size() ;
  }

  inline size_t Module::decodeV3(double tdcconversion) 
  {
    if(!m_isdecoded) {
      if( m_size != 0 ) {
        const unsigned short* begin = m_data ;
        const unsigned short* end   = begin + m_size ;
        m_ottimes.reserve( m_size ) ;
        for( const unsigned short* ihit = begin ; ihit != end ; ++ihit)
          addHit(*ihit,tdcconversion) ;
      }
      m_isdecoded = true ;
    }
    return m_ottimes.size() ;
  }
  
  inline size_t Module::decode(double tdcconversion)
  {
    size_t rc(0) ;
    switch(m_bankversion) {
    case OTBankVersion::DC06: 
      rc = decodeDC06(tdcconversion) ;
      break;
    case OTBankVersion::v3:
      rc = decodeV3(tdcconversion) ;
      break;
    }
    return rc ;
  }
  
  class Detector
  {
  public:
    Detector(const DeOTDetector& det,
             const OTDAQ::ChannelMap::Detector& channelmap) : m_golHeadersLoaded(false) 
    {  
      for(DeOTDetector::Modules::const_iterator imod = det.modules().begin() ;
          imod != det.modules().end(); ++imod) {
        Module& amodule = module((*imod)->stationID(),(*imod)->layerID(),(*imod)->quarterID(),(*imod)->moduleID()) ;
        amodule.setDetElement(**imod) ;
        amodule.setChannelMap( channelmap.module((*imod)->stationID(),(*imod)->layerID(),(*imod)->quarterID(),(*imod)->moduleID()) ) ;
      }
    }
      
    void clearevent() {
      m_golHeadersLoaded = false ;
      for(iterator imod = begin(); imod!= end(); ++imod)
        imod->clearevent() ;
    }
    Module& module(const unsigned int station, const unsigned int layer, 
                   const unsigned int quarter, const unsigned int module) {
      return m_modules[station-1][layer][quarter][module-1] ;
    }
    bool golHeadersLoaded() const { return m_golHeadersLoaded ; }
    void setGolHeadersLoaded(bool b=true) { m_golHeadersLoaded = b ; }
    typedef Module* iterator  ;
    iterator begin() { return &(m_modules[0][0][0][0]) ; }
    iterator end() { return &(m_modules[NumStations-1][NumLayers-1][NumQuadrants-1][NumModules]); }

    bool isvalidID(const unsigned int station, const unsigned int layer, 
                   const unsigned int quarter, const unsigned int module) const {
      return station-1<=NumStations-1 && layer<=NumLayers-1  && quarter<=NumQuadrants-1 && module-1<=NumModules-1 ; }

  private: 
    enum { OffsetStations=1, OffsetLayers=1, OffsetQuadrants=1, OffsetModules=0 } ;
    enum { NumStations=3, NumLayers=4, NumQuadrants=4, NumModules=9 } ;
    Module m_modules[NumStations][NumLayers][NumQuadrants][NumModules];
    bool m_golHeadersLoaded ;
  } ;
  
}

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTRawBankDecoder );
  

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTRawBankDecoder::OTRawBankDecoder( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_countsPerBX(64),
    m_numberOfBX(3),
    m_timePerBX(25*Gaudi::Units::ns),
    m_forcebankversion(OTBankVersion::UNDEFINED),
    m_otdet(0),
    m_nsPerTdcCount(m_timePerBX/m_countsPerBX),
    m_detectordata(0)
{
  declareInterface<IOTRawBankDecoder>(this);
  declareProperty("countsPerBX", m_countsPerBX );
  declareProperty("numberOfBX", m_numberOfBX );
  declareProperty("timePerBX", m_timePerBX );
  declareProperty("ForceBankVersion", m_forcebankversion = OTBankVersion::UNDEFINED );
  declareProperty("RawEventLocation", m_rawEventLocation = RawEventLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
OTRawBankDecoder::~OTRawBankDecoder() {}

//=============================================================================

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTRawBankDecoder::initialize()
{
  
  debug()<<"initializing OTRawBankDecoder"<<endmsg;
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // Setup incident services
  incSvc()->addListener( this, IncidentType::EndEvent );
  
  // Loading OT Geometry from XML
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

  // access to the channel map
  m_channelmaptool = tool<IOTChannelMapTool>("OTChannelMapTool");

  // initialize the decoder data. this things basically contains the decoded event
  m_detectordata = new OTRawBankDecoderHelpers::Detector(*m_otdet,m_channelmaptool->getChannelMap()) ;
  
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  
  m_nsPerTdcCount = m_timePerBX/ double(m_countsPerBX);
  
  if( m_forcebankversion != OTBankVersion::UNDEFINED )
    warning() << "Forcing bank version to be " << m_forcebankversion << endreq ;
  
  info() << " countsPerBX = " << m_countsPerBX << endmsg;
  info() << " numberOfBX  = " << m_numberOfBX << endmsg;
  info() << " timePerBX = " << m_timePerBX << endmsg;
  info() << " ForceBankVersion = " << m_forcebankversion << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Finalize
//=============================================================================
StatusCode OTRawBankDecoder::finalize()
{
  if( m_detectordata ) {
    delete m_detectordata ;
    m_detectordata=0;
  }
  return GaudiTool::finalize() ;
}

//=============================================================================
// Handle for incident service
//=============================================================================

void OTRawBankDecoder::handle ( const Incident& incident )
{
  if ( IncidentType::EndEvent == incident.type() ) m_detectordata->clearevent() ;
}


//=============================================================================
//Decode the gol headers in the raw banks. Each header is assigned
//to its corresponding module. The contents of the gol are only
//decoded once the module data is asked for. (That is the 'decoding
//on demand' part.)
//=============================================================================
StatusCode OTRawBankDecoder::decodeGolHeadersDC06(const RawBank& bank, int bankversion) const
{
  // There is one word which contains no data. (In real data this is
  // the OTSpecificHeader). We just skip it.
  const unsigned int* begin = bank.data() + 1 ;
  const unsigned int* end   = bank.data() + bank.size()/4 ;
  const unsigned int* idata ;  
  unsigned int station,layer,quarter,module,numhits ;
  bool decodingerror(false) ;
  for( idata = begin ; idata < end ; ++idata ) {
    OTDAQ::GolHeaderDC06 golHeader(*idata) ;
    numhits = golHeader.numberOfHits() ;
    // We skip empty headers without issuing a warning
    if( numhits > 0 ) {
      station = golHeader.station();
      layer = golHeader.layer();
      quarter = golHeader.quarter();
      module = golHeader.module();
      if(!m_detectordata->isvalidID(station,layer,quarter,module) ) {
        warning() << "Invalid gol header "<< golHeader << endmsg ;
        decodingerror = true ;
      } else {
        const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1) ;
        m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bankversion) ; 
        if (msgLevel(MSG::DEBUG)) debug() << "Reading gol header " << golHeader << endmsg ;
      }
      idata += golHeader.hitBufferSize() ;
    }
  }
  
  if(idata != end) {
    warning() << "OTRawBankDecoder: gol headers do not add up to buffer size. " << idata << " " << end << endreq ;
    decodingerror = true ;
  }
  
  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

StatusCode OTRawBankDecoder::decodeGolHeadersV3(const RawBank& bank, int bankversion) const
{
  bool decodingerror(false) ;
  // The first 4 bytes are the OTSpecificHeader
  OTDAQ::OTSpecificHeader otheader(*bank.data()) ;
  if( msgLevel(MSG::DEBUG)) 
    debug() << "OTSpecificHeader in bank:" << otheader << endmsg ;
  if( otheader.error() ) 
    warning() << "OTSpecificHeader has error bit set in bank " << bank.sourceID() << endreq ;
  // The data starts at the next 4byte
  const unsigned int* begin = bank.data() + 1 ;
  const unsigned int* end   = bank.data() + bank.size()/4 ;
  const unsigned int* idata ;
  unsigned int station,layer,quarter,module,numhits,numgols(0) ;
  for( idata = begin ; idata < end; ++idata) {
    // decode the header
    OTDAQ::GolHeaderV3 golHeader(*idata) ;
    // if there are no hits, issue a warning
    numhits = golHeader.numberOfHits() ;
    if( 0 == numhits ) {
      warning() << "Found empty GOL header " << golHeader << endmsg ;
    } else {
      // Decode the GOL ID
      station = golHeader.station();
      layer = golHeader.layer();
      quarter = golHeader.quarter();
      module = golHeader.module();
      // check that the GOL ID is valid  
      if(!m_detectordata->isvalidID(station,layer,quarter,module) ) {
        warning() << "Invalid gol header "<< golHeader << endmsg ;
        decodingerror = true ;
      } else {
        const unsigned short* firsthit = reinterpret_cast<const unsigned short*>(idata+1) ;
        m_detectordata->module(station,layer,quarter,module).setData(numhits,firsthit,bankversion) ; 
        if (msgLevel(MSG::DEBUG)) debug() << "Reading gol header " << golHeader << endmsg ;
      }
      // skip the actual hits
      idata += golHeader.hitBufferSize() ;
    }
    ++numgols ;
  }

  // check that everything is well aligned
  if(idata != end) {
    warning() << "GOL headers do not add up to buffer size. " << idata << " " << end << endreq ;
    decodingerror = true ;
  }
  
  // check that we have read the right number of GOLs
  if( numgols != otheader.numberOfGOLs() ) {
    warning() << "Found " << otheader.numberOfGOLs() << " in bank header, but read only " 
              << numgols << " from bank." << endmsg ;
    decodingerror = true ;
  }
    
  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
}

StatusCode OTRawBankDecoder::decodeGolHeaders() const
{
  
  // Retrieve the RawEvent:
  RawEvent* event;
  if ( exist<RawEvent>(m_rawEventLocation) )
  {
    event = get<RawEvent>(m_rawEventLocation);
    
    // Get the buffers associated with OT
    const std::vector<RawBank*>& OTBanks = event->banks(RawBank::OT );
    
    // Report the number of banks
    if (msgLevel(MSG::DEBUG)) 
      debug() << "Decoding GOL headers in OTRawBankDecoder. Number of OT banks is " 
              << OTBanks.size() << endreq ;
    
    for (std::vector<RawBank*>::const_iterator  ibank = OTBanks.begin();
         ibank != OTBanks.end() ; ++ibank) {
      
      // Report the bank size and version
      if (msgLevel(MSG::DEBUG))
        debug() << "OT Bank sourceID= " << (*ibank)->sourceID() 
                << " size=" << (*ibank)->size()/4 << " bankversion=" << (*ibank)->version() << endmsg;
      
      // Choose decoding based on bank version
      int bVersion = m_forcebankversion != OTBankVersion::UNDEFINED ? m_forcebankversion : (*ibank)->version();
      m_channelmaptool->setBankVersion( bVersion ) ;
      StatusCode sc ;
      switch( bVersion ) {
      case OTBankVersion::DC06:
        sc = decodeGolHeadersDC06(**ibank,bVersion) ;
        break ;
      case OTBankVersion::v3:
        sc = decodeGolHeadersV3(**ibank,bVersion) ;
        break ;
      default:
        warning() << "Cannot decode OT raw buffer bank version "
                  << bVersion << " with this version of OTDAQ" << endmsg;
      } ;
      // ignore errors
      sc.ignore() ;
    }
  
    // make sure we don't call this until the next event
    m_detectordata->setGolHeadersLoaded(true) ; 
  }
  else
  {
    warning() << " RawEvent does not exist at " << m_rawEventLocation << " location " << endmsg;
  }
  
  return StatusCode::SUCCESS ;
}

size_t OTRawBankDecoder::decodeModule( OTRawBankDecoderHelpers::Module& moduledata ) const 
{
  if( !m_detectordata->golHeadersLoaded() ) decodeGolHeaders().ignore() ;
  if( !moduledata.isDecoded() ) moduledata.decode( m_nsPerTdcCount ) ;
  return moduledata.ottimes().size() ;
}

LHCb::OTLiteTimeRange OTRawBankDecoder::decodeModule( const LHCb::OTChannelID& moduleid ) const
{
  OTRawBankDecoderHelpers::Module& moduledata =
    m_detectordata->module(moduleid.station(),moduleid.layer(),moduleid.quarter(),moduleid.module()) ;
  if( !moduledata.isDecoded() ) OTRawBankDecoder::decodeModule(moduledata) ;
  return moduledata.ottimes() ;
}

StatusCode OTRawBankDecoder::decode( LHCb::OTLiteTimeContainer& ottimes ) const
{
  // decode all modules. keep track of total number of hits.
  size_t numhits(0) ;
  for( OTRawBankDecoderHelpers::Module* imod = m_detectordata->begin(); 
       imod != m_detectordata->end(); ++imod)
    numhits += decodeModule( *imod ) ;
  // reserve and copy
  ottimes.clear() ;
  ottimes.reserve( numhits ) ;
  for( OTRawBankDecoderHelpers::Module* imod = m_detectordata->begin(); 
       imod != m_detectordata->end(); ++imod)
    ottimes.insert(ottimes.end(), imod->ottimes().begin(), imod->ottimes().end() ) ;
  // We'll need some proper error handling.
  return StatusCode::SUCCESS ;
}
