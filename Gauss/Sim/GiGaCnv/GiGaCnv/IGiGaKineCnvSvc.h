/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 11:13:54  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
/// ===========================================================================
#ifndef    GIGACNV_IGIGAKINECNVSVC_H
#define    GIGACNV_IGIGAKINECNVSVC_H 1
/// ===========================================================================
/// GiGa 
///  GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IIDIGiGaKineCnvSvc.h"

/// forward declarations 
class StatusCode; 
class G4PrimaryVertex;
class IParticlePropertySvc;
class GiGaKineRefTable;

/** @class IGiGaKineCnvSvc IGiGaKineCnvSvc.h GiGa/IGiGaKineCnvSvc.h
 *
 *  definition of abstract interface to Geant 4 conversion service 
 *  which is responsible for conversion from primary event kinematics from 
 *  Gaudi representation to GEANT4 primary event structures.  
 *  
 *   @author Vanya Belyaev
 */

class IGiGaKineCnvSvc: virtual public IGiGaCnvSvc
{

public:
  
  /** unique identifier for the interface 
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IGiGaKineCnvSvc; }
  
  /** retrieve the relation table between Geant4 track/trajectory 
   *  identifiers and the converted MCParticle objects 
   *  @return the reference to relation table  
   */
  virtual GiGaKineRefTable&      table() = 0 ;

  /** accessor to Particle Property Service
   *  @return pointer to Particle Property Service 
   */
  virtual IParticlePropertySvc*  ppSvc() const  = 0 ;  
  
  /// virtual destructor 
  virtual ~IGiGaKineCnvSvc(){} ; 

  ///
};


/// ===========================================================================
#endif   ///<  GIGACNV_IGIGAKINECNVSVC_H
/// ===========================================================================
