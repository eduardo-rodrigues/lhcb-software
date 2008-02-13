#ifndef DBROOTHIST_H
#define DBROOTHIST_H 1
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "HistogramIdentifier.h"
#include "OMAlib/OMAlib.h"
#include "dic.hxx"

#include <TH1.h>
#include <TPad.h>
//class TImage;
class DimInfo;
//class TPad;

class DbRootHist : public HistogramIdentifier
{
  public:
    DbRootHist(const std::string & identifier,
               const std::string & dimServiceName,
               int refreshTime,
               int instance,
               OnlineHistDB* histogramDB,
               OMAlib* analysisLib,
               OnlineHistogram* onlineHist);

    DbRootHist (const DbRootHist & );
    DbRootHist & operator= (const DbRootHist &);

    virtual ~DbRootHist();

//    virtual bool setdbHist(OnlineHistogram*  oh);

    // Binning is only known from DIM, so create ROOT histo with DIM
    void initHistogram();

    // Once we have our ROOT histos, this will be called to get new values
    // TODO: if binning changes on the fly, reinit ...MonObj?
    void fillHistogram();

    // Enable / disable pad with SetEditable and move
    void enableEdit();
    void disableEdit();

    // Clearing histos with an offset value and restoring them
    void enableClear();
    void disableClear();
    void beEmptyHisto();
    bool isEmptyHisto() { return m_isEmptyHisto; }
    bool isCleared() { return m_cleared; }

    std::string histoRootName() {return std::string(m_histoRootName.Data());}
    std::string hstype() { return m_hstype; }
    int instance() { return m_instance; }
    void setDimServiceName(std::string newDimServiceName);
    bool isAnaHist() { return m_isAnaHist;}
    const std::vector<DbRootHist*> *anaSources() {return &m_anaSources;}
    std::string creationAlgorithm() { return m_creationAlgorithm;}
    std::vector<float> *anaParameters() { return &m_parameters;}
    // actual ROOT histo
    TH1*    rootHistogram;
    // Histos live in pads, so each histo must have a home
    TPad*   hostingPad;
    // H2Ds are rendered as images
//    TImage* histogramImage;

    /// histogram identifier
    std::string identifier()  { return m_identifier;}
    /// corresponding OnlineHistogram object
    OnlineHistogram* onlineHistogram() {return m_onlineHistogram;}
    /// link to an existing OnlineHistogram object, returns true on success
    virtual bool setOnlineHistogram(OnlineHistogram* newOnlineHistogram);
    /// link to an existing ROOT TH1 object, returns true on success
    bool setRootHistogram(TH1* newRootHistogram);
    /// true if object knows an existing DB session
    bool connected() { return (m_session != NULL); }
    /// returns link to DB session
    OnlineHistDB* dbSession() { return m_session; }
    /// connect to DB session, returns true if the corresponding DB histogram entry is found
    bool connectToDB(OnlineHistDB* session,
                     std::string page = "_NONE_",
                     int instance = 1);
    /// updates ROOT TH1 display properties from Histogram DB (via OnlineHistogram object)
    /// (normally called when connecting)
    void setTH1FromDB();
    /// updates current drawing options from Histogram DB (via OnlineHistogram object)
    void setDrawOptionsFromDB(TPad* &pad);
    /// saves current ROOT display options to OnlineHistogram object and to Histogram DB
    bool saveTH1ToDB(TPad* pad = NULL);
    /// save provided histogram as a reference in the standard file, with optional run period and data type
    bool setReference(TH1 *ref,
                      int startRun =1,
                      std::string dataType = "default");
    /// get reference histogram if available
    TH1* getReference(int startRun = 1,
                      std::string dataType = "default");
    // TH1 drawing methods
    /// calls TH1 Draw method, calls setDrawOptions()
    virtual void Draw(TPad* &pad);
    /// normalize reference (if existing and requested) to current plot
    void normalizeReference();

  private:
    DimInfo*  m_gauchocommentDimInfo;
    // TODO: have a stack of offset for bracketing
    // state of histo @ clr/rst
    TH1*      m_offsetHistogram;

//    HistogramIdentifier m_histogramIdentifier;

    bool      m_isAnaHist;
    // source histograms for analysis histogram
    std::vector<DbRootHist*> m_anaSources;
    bool      m_anaLoaded;
    OMAlib*   m_analysisLib;
    std::string m_creationAlgorithm;
    std::vector<std::string> m_sourcenames;
    std::vector<float> m_parameters;

    // DIM regular time interval
    int       m_refreshTime;
    // dimbuffer
    float*    m_histoDimData;
    // overloaded from DimInfo
//    void      infoHandler();
    // generated ROOT histo name for identification
    TString   m_histoRootName;
    // generated ROOT histo title from DIM gauchocomment
    TString   m_histoRootTitle;
    // flag for refresh/static histo
//    bool      m_toRefresh;
    // flag for clear/integrate
    bool      m_cleared;
    std::string m_hstype;
    std::string m_hname;
    int m_instance;
    int m_waitTime;
    int m_msgBoxReturnCode;
    int m_serviceSize;

    std::string m_identifier;
    OnlineHistDB* m_session;
    OnlineHistogram* m_onlineHistogram;
    bool m_isEmptyHisto;

    std::string m_refOption;
    TH1* m_reference;
    int  m_startRun;
    std::string m_dataType;
    std::string m_dimServiceName;
    DimInfo* m_dimInfo;

    void cleanAnaSources();
    void loadAnaSources();
    void initRootFromDim();
    void fillRootFromDim();

    bool updateDBOption(std::string opt, void* value, bool isDefault);
    void drawReference();
};

#endif // DBROOTHIST_H
