/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.10  2001/07/23 13:11:44  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
#ifndef           GIGACNV_IGIGACNVSVC_H 
#define           GIGACNV_IGIGACNVSVC_H  1 
/// ===========================================================================
// STL 
#include <vector> 
// GaudiKernel
#include "GaudiKernel/IConversionSvc.h" 
#include "GaudiKernel/IService.h" 
//
#include "GiGaCnv/IIDIGiGaCnvSvc.h"
//
class IGiGaSvc;
class IGiGaSetUpSvc;
class IParticlePropertySvc;
//

/** @class IGiGaCnvSvc     IGiGaCnvSvc.h GiGa/IGiGaCnvSvc.h
 *
 *  definition of abstract interface to Geant 4 conversion service 
 *  which is responsible for conversion from primary event kinematics from 
 *  Gaudi representation to GEANT4 primary event structures 
 *  
 *  @author Vanya Belyaev
 */

class IGiGaCnvSvc: virtual public IConversionSvc ,
                   virtual public IService   
{  
  ///
public:
  ///
  class Leaf
  {
    ///
  public:
    /// constructor
    Leaf( const std::string&  Path  = "" , const CLID&         Clid  = CLID() ,
          const std::string&  Addr1 = "" , const std::string&  Addr2 = ""     ) 
      : m_path  ( Path  ) 
      , m_clid  ( Clid  ) 
      , m_addr1 ( Addr1 ) 
      , m_addr2 ( Addr2 ) {};
    /// virtual destructor 
    virtual ~Leaf(){};
    //
    inline const std::string& path () const { return m_path  ; } 
    inline const CLID&        clid () const { return m_clid  ; } 
    inline const std::string& addr1() const { return m_addr1 ; } 
    inline const std::string& addr2() const { return m_addr2 ; } 
    //
    inline Leaf& setPath ( const std::string& Path ) 
    { m_path  = Path; return *this ; } 
    inline Leaf& setClid ( const CLID&        Clid ) 
    { m_clid  = Clid; return *this ; } 
    inline Leaf& setAddr1( const std::string& Addr ) 
    { m_addr1 = Addr; return *this ; } 
    inline Leaf& setAddr2( const std::string& Addr ) 
    { m_addr2 = Addr; return *this ; } 
    ///
  private:
    ///
    std::string     m_path  ;
    CLID            m_clid  ;
    std::string     m_addr1 ;
    std::string     m_addr2 ;
    ///
  };
  ///
  typedef std::vector<Leaf> Leaves;
  ///
public: 
  ///  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaCnvSvc; }
  ///
  virtual IGiGaSvc*              gigaSvc  ()  = 0 ;  
  ///
  virtual IGiGaSetUpSvc*         setupSvc ()  = 0 ; 
  ///
  virtual StatusCode             declareObject( const Leaf & ) = 0 ;
  ///
  virtual ~IGiGaCnvSvc(){}; 
  ///
};
///

/// ===========================================================================
#endif ///<  GIGACNV_IGIGACNVSVC_H 
/// ===========================================================================




