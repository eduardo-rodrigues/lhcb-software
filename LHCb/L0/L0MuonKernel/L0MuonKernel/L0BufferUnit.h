#ifndef L0MUONKERNEL_L0BUFFERUNIT_H
#define L0MUONKERNEL_L0BUFFERUNIT_H 

/** @class L0Buffer L0Buffer.h  L0MuonKernel/BuildL0Buffer.h
 
  class for constructing the L0Buffer 
  It is a register containing the field: 
  identification field  (32 bits)
  input field (432 bits) : optical link and neighbours 
  output field (48 bits) : address for candidates + status word  

  @author Luisanna Tocco
  @date 2 September 2003

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "ProcessorKernel/Unit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MTile.h"

namespace L0Muon {
  
  class L0BufferUnit : public Unit {

  public:

    /// Default Constructor
    L0BufferUnit();

    /// Constructor
    L0BufferUnit(MuonTileID &pu, std::vector<int> & foix, std::vector<int> & foiy, int & type);
    /// Destructor
    ~L0BufferUnit();
    
    /// Return x foi in station sta
    int xFoi(int sta);
  
    /// Return y foi in station sta
    int yFoi(int sta);

    /// Return x foi in station sta
    int xHardFoi(int sta);
  
    /// Return y foi in station sta
    int yHardFoi(int sta);

    /// Return the type of the buffer 
    int type(int type){return m_type;}
    
    /// Fill the bit of the output registers
    void setL0buf();
    void setL0bufStd();
    void setL0bufPLL();
    void setOLPLL();

/*     /// Read the external file describing the buffer and return the L0MTileList */
/*     std::vector<L0MTile> tileListFromMap(int type); */

/*     /// Generic method to extract a tile list from the map */
/*     std::vector<L0MTile> readTileListFromMap(FILE *file,int max); */

    /// Open the output file 
    void setOutputFile(std::string suffixe);
 
    /// Write the l0buffer on the output file
    void writeEvent();
    void writeHeader();

   
    void initialize();

    void execute();
    
    void finalize();

    /// Give a static type name to the unit
    std::string type() {
      return "L0BufferUnit";
    }

  


  private:

    /// PU ID
    MuonTileID m_pu;

    /// Fois (one vector element per station)
    std::vector<int> m_xfoi;
    std::vector<int> m_yfoi;
    
    /// Type of the Unit : "standard" or PLL type.
    int m_type;

    /// Pointer to the file where to write the L0Buffer
    FILE *m_l0bufferFile;
    
    /// Event counter
    unsigned long int m_l0EventNumber;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERUNIT_H
