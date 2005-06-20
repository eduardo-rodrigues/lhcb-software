// $Id: GeometryInfoPlus.h,v 1.3 2005-06-20 16:20:33 cattanem Exp $
#ifndef LIB_GEOMETRYINFOPLUS_H 
#define LIB_GEOMETRYINFOPLUS_H 1

// Include files
/** STD and STL includes */ 
#include <iostream>
#include <string>
#include <functional>
#include <algorithm> 
/* GaudiKernel includes */
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
/** DetDesc includes */ 
#include "DetDesc/Services.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/IGeometryInfo.h" 
#include "DetDesc/IDetectorElement.h" 
#include "GeometryInfoException.h"

/** forward declarations */
class GaudiException   ;
class IDataProviderSvc ;
class AlignmentCondition;

/** @class GeometryInfoPlus GeometryInfoPlus.h Lib/GeometryInfoPlus.h
 *  
 *  Based on "most trivial" implementation of Vanya Belyaev and
 *  Sebastien Ponce.
 *
 *
 *  @author Vanya Belyaev 
 *  @author Sebastien Ponce
 *  @author Juan PALACIOS
 *  @date   2005-04-28
 */




class GeometryInfoPlus :   public IGeometryInfo {

  ///  friend class 
  friend class DetectorElement;

public: 

  /// type of vector of children's names 
  typedef std::vector<std::string>     ChildName;

  typedef std::vector<HepTransform3D>::const_iterator matrix_iterator;
  /** constructor, creates "ghost"
   *  @param de pointer to detector element 
   */
  GeometryInfoPlus( IDetectorElement* de ); 
  
  /** constructor, creates "orphan"    
   *  @param de     pointer to detector element 
   *  @param LogVol name of logical volume 
   */
  GeometryInfoPlus( IDetectorElement*   de     ,
                    const std::string&  LogVol ); 
  
  /** constructor, create regular element 
   *  @param de     pointer to detector element 
   *  @param LogVol name of logical volume
   *  @param Support name of support element 
   *  @param ReplicaPath replica path/address 
   *  @param alignmentPath alignment path/address 
   */
  GeometryInfoPlus( IDetectorElement*            de,
                    const std::string&           LogVol, 
                    const std::string&           Support,
                    const ILVolume::ReplicaPath& ReplicaPath,
                    const std::string& alignmentPath="");
  
  /** constructor, create regular element 
   *  @param de              pointer to detector element 
   *  @param LogVol          name of logical volume
   *  @param Support         name of support element 
   *  @param ReplicaNamePath replica path/address 
   *  @param alignmentPath   alignment path/address 
   */
  GeometryInfoPlus( IDetectorElement*  de,
                    const std::string& LogVol, 
                    const std::string& Support,
                    const std::string& ReplicaNamePath,
                    const std::string& alignmentPath="");
  
  virtual ~GeometryInfoPlus( ); ///< Destructor

public:

  StatusCode initialize();
  StatusCode cache();

  /// Is this "geometry object" assosiated with Logical Volume?
  inline bool hasLVolume() const { return m_gi_has_logical; }   //     
                        
  /// Is this "geometry object" supported?
  inline bool hasSupport() const { return m_gi_has_support; }   //

  inline const AlignmentCondition* alignmentCondition() const 
  {
    return m_alignmentCondition;
  }



  /// transformation matrix from global reference
  /// system to the local one
  inline const HepTransform3D&  matrix() const 
  {
    return  *m_matrix;
  }
  
  /// transformation matrix from local  reference
  /// system to the global one.
  /// Full transformation including misalignments.
  const HepTransform3D&  matrixInv() const 
  {
    return *m_matrixInv;  
  }

  /// transformation matrix from global reference
  /// system to the local one.
  /// Ideal transformation with no misalignments.
  const HepTransform3D& idealMatrix() const 
  {
    return *m_idealMatrix;
  }

  /// transformation matrix from local reference
  /// system to the global one.
  /// Ideal geometry with no misalignments.
  inline const HepTransform3D&  idealMatrixInv() const 
  {
    return *m_idealMatrixInv;
  }


  /// Transformation matrix for the volume corresponding
  /// to this IGeometryInfo. Uses the transformations of parent 
  /// volumes that are on this detector element level.
  const HepTransform3D& localIdealMatrix() const;


  const HepTransform3D& localDeltaMatrix() const;
  
  /// Upate the DELTA transformation matrix of this IGeometryInfo.
  /// Will re-do calculation of all matrices.
  StatusCode localDeltaMatrix(const HepTransform3D&);


  /// tranform the point from the global reference systemn 
  /// to the local reference system  
  inline HepPoint3D toLocal( const HepPoint3D& globalPoint ) const {
    return ( matrix() * globalPoint );
  }
  
  
  /// tranform the point from the local reference system 
  ///  to the global reference system  
  inline HepPoint3D toGlobal( const HepPoint3D& localPoint  ) const 
  {
    return ( matrixInv() * localPoint  );
  }

  /// is the given point in the global reference system
  /// inside this detector element?
  inline bool isInside( const HepPoint3D& globalPoint ) const 
  {
    return ( hasLVolume() && 0 != lvolume() ) ?
      lvolume()->isInside( toLocal( globalPoint ) ) :
      isInsideDaughter( globalPoint );
  }
  

  ///  return the name of the daughter element to which
  /// the given point belongs to
  std::string belongsToPath( const HepPoint3D& globalPoint );      
  
  /** return the C++ pointer to the daughter element to 
   *  which the given point belongs to
   */    
  IGeometryInfo* belongsTo( const HepPoint3D& globalPoint );    
  
  /** return the name of the daughter element to which 
   *  the given point belongs to (taking into account the level)
   */
  std::string belongsToPath( const HepPoint3D& globalPoint ,       
                             const int         level       );       

  /** return the C++ pointer to the daughter element to 
   *  which the given point belongs to  (taking into account the level)   
   */
  IGeometryInfo* belongsTo( const HepPoint3D& globalPoint ,        
                            const int         level       );      

  /** return the full geometry info for a given point in the 
   * global reference system 
   */
  StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                  const int                level      , 
                                  IGeometryInfo*&          start      , 
                                  ILVolume::PVolumePath&   volumePath );

  /**  return the full geometry info for a given point in the
   *   global reference system 
   */
  StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                  const int                level      , 
                                  IGeometryInfo*&          start      , 
                                  ILVolume::ReplicaPath&   volumePath );

  /** return the full geometry info for a given point in the 
   *  global reference system 
   */
  StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                  const int                level      , 
                                  std::string&             start      , 
                                  ILVolume::PVolumePath&   volumePath );

  /** return the full geometry info for a given point in the
   *  global reference system 
   */
  StatusCode fullGeoInfoForPoint( const HepPoint3D&        point      , 
                                  const int                level      , 
                                  std::string&             start      , 
                                  ILVolume::ReplicaPath&   volumePath );

  /// return the name of the assosiated logical volume 
  inline const std::string& lvolumeName() const  { return m_gi_lvolumeName; }
  

  /// return the C++ pointer4 to the assosiated logical volume 
  const ILVolume* lvolume() const ;
  
  /// return the location (address) of this geometry object  
  StatusCode location( IGeometryInfo*&              start , 
                       ILVolume::ReplicaPath&       replicaPath ) const ;

  /// return the location (address) of this geometry object  
  StatusCode location( std::string&                 start , 
                       ILVolume::ReplicaPath&       replicaPath ) const ;

  ///  return the IGeometryInfo supporting this one.
  IGeometryInfo* const supportIGeometryInfo() const;
  

  /// return the name of the logical volume with a given address 
  std::string lvolumePath( const std::string&           start       , 
                           const ILVolume::ReplicaPath& replicaPath );

  /// return the C++ pointer to  the logical volume with a given address 
  const ILVolume* lvolume( const std::string&           start       , 
                           const ILVolume::ReplicaPath& replicaPath );
  
  /// return the name of the logical volume with a given address 
  std::string lvolumePath( IGeometryInfo*               start       , 
                           const ILVolume::ReplicaPath& replicaPath );

  /// return the C++ pointer to  the logical volume with a given address 
  const ILVolume* lvolume( IGeometryInfo*               start       , 
                           const ILVolume::ReplicaPath& replicaPath );

  /// pointer to the parent IGeometryInfo 
  IGeometryInfo* parentIGeometryInfo();

  /// pointer to the parent IGeometryInfo (const version)
  const IGeometryInfo* parentIGeometryInfo() const;

  /// (reference to) container of children IGeometryInfo 
  inline IGeometryInfo::IGIChildrens& childIGeometryInfos() 
  {
    if( !m_gi_childLoaded ) loadChildren(); 
    return m_gi_childrens; 
  }
  /// (reference to) container of children IGeometryInfo 
  inline const IGeometryInfo::IGIChildrens& childIGeometryInfos() const 
  {
    if( !m_gi_childLoaded ) loadChildren();
    return m_gi_childrens; 
  }
  /// iterators for manipulation of daughter elements 
  /// begin iterator
  inline IGeometryInfo::IGIChildrens::const_iterator childBegin() const
  {
     if( !m_gi_childLoaded ) {  
       try{ loadChildren(); }
       catch(...) { return m_gi_childrens.end(); }
     }
     return m_gi_childrens.begin();
  }
  

  /// begin iterator (const version)
  inline IGeometryInfo::IGIChildrens::iterator childBegin()
  {  
    if( !m_gi_childLoaded ) 
    {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
    return m_gi_childrens.begin(); 
  }
  
  /// end  iterator 
  inline IGeometryInfo::IGIChildrens::iterator childEnd() 
  {
    if( !m_gi_childLoaded ) 
    {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
    return m_gi_childrens.end(); 
  }
  

  /// end  iterator (const version)
  inline IGeometryInfo::IGIChildrens::const_iterator childEnd() const 
  {
    if( !m_gi_childLoaded ) 
    {  try{ loadChildren(); } catch(...){ return m_gi_childrens.end(); }  }
    return m_gi_childrens.end(); 
  }
  

  /// IInspectable interface
  bool acceptInspector( IInspector* );
  bool acceptInspector( IInspector* ) const ; 
  /// serialization  for reading 
  StreamBuffer& serialize( StreamBuffer& ); 
  /// serialization  for writing 
  StreamBuffer& serialize( StreamBuffer& ) const ;
  /// overloades printout to  std::ostream 
  std::ostream& printOut   ( std::ostream& ) const;
  /// overloaded printout to the MsgStream
  MsgStream&    printOut   ( MsgStream&    ) const;
  /// reset to the initial state 
  IGeometryInfo*  reset();

  /** query the interface
   *  @param ID interface identifier
   *  @param ppI placeholder for returned interface
   *  @return status code
   */ 
  StatusCode queryInterface( const InterfaceID& ID , void** ppI ) ;
  /// add reference
  unsigned long addRef() 
  {
    return 0;
  }
  
  /// release 
  unsigned long release() 
  {
    return 0;
  }
  
  
  /// retrive the replica path (mistrerious "rpath")
  ILVolume::ReplicaPath& supportPath() const ; 


protected:


private:

  IMessageSvc*       msgSvc() const;
  IDataProviderSvc* dataSvc() const;

  inline MsgStream log() const  { return *m_log; }

  inline bool isInsideDaughter( const HepPoint3D& globalPoint ) const 
  {
    if( !childLoaded() || loadChildren().isFailure() )  return false;
    IGeometryInfo::IGIChildrens::const_iterator it = 
      std::find_if( childBegin() , childEnd  () , 
                    IGeometryInfo_isInside( globalPoint ) ) ;  
    return ( ( childEnd() == it ) ? false : true );
  }
  
  inline bool childLoaded() const { return m_gi_childLoaded ; }

  StatusCode calculateMatrices();

  StatusCode combineMatrices(matrix_iterator deltaFirst,
                             matrix_iterator deltaEnd,
                             matrix_iterator pvFirst,
                             matrix_iterator pvEnd);
  
  /// Calculate the combined ideal and delta matrix connecting
  /// this IGeometryInfo to the global frame.
  /// Set the corresponding inverse matrix.
  StatusCode calculateFullMatrices(matrix_iterator deltaFirst,
                                   matrix_iterator deltaEnd,
                                   matrix_iterator pvFirst);

  /// Calculate the total ideal matrix connecting 
  /// this IGeometryInfo to the global frame.
  /// Set the corresponding inverse matrix.
  void calculateIdealMatrix(matrix_iterator pvFirst,
                            matrix_iterator pvEnd);

  StatusCode getAlignmentCondition();

  void clearMatrices();

  inline bool needsAlignmentCondition() const
  {
    return m_hasAlignmentPath;
  }

  inline void needsAlignmentCondition(const bool& needsAlignment)
  {
    m_hasAlignmentPath = needsAlignment;
  }

  inline bool hasAlignmentCondition() const
  {
    return m_hasAlignment;
  }

  inline void hasAlignmentCondition(const bool& hasAlignment) 
  {
    m_hasAlignment = hasAlignment;
  }

  inline AlignmentCondition* myAlignmentCondition() const 
  {
    return m_alignmentCondition;
  }  

  IDetectorElement* parentIDetectorElement(IDetectorElement* iDetElem) 
  {
    return (0!=iDetElem) ? iDetElem->parentIDetectorElement() : 0; 
  }

  IGeometryInfo* geoByName ( const std::string& name ) const ;

  ILVolume* findLogical() const ; 
  /// could throw GeometryInfoException
  StatusCode loadChildren() const ; 

  inline IDetectorElement* detElem() const { 
    return m_gi_iDetectorElement ;
  }

  /// Assertion JUAN: beware: dummies do nothing!
  inline void Assert( bool assertion, 
                      const std::string& name ) const {
    if( !assertion ) { throw GeometryInfoException( name , this ); }
  }; 
  
  /// Assertion 
  inline void Assert( bool assertion, const std::string& name,
                      const GaudiException& ge ) const {
    if( !assertion ) { throw GeometryInfoException( name , ge , this ); }
  };

  HepTransform3D* accumulateMatrices(const ILVolume::PVolumePath& volumePath) const;
  


  bool idealMatrixLoaded() { return (0!= m_idealMatrix); }
  
private:

  MsgStream* m_log;

  /// flag for logical volume association 
  bool                                 m_gi_has_logical     ;    
  /** name of logical volume ( full path (address) 
   *  in the Transient Store )  
   */
  std::string                          m_gi_lvolumeName     ;  
  /// C++ pointer to the logical volume (loaded on demand only!)
  mutable ILVolume*                    m_gi_lvolume; 

  bool m_hasAlignment;

  std::string m_alignmentPath;

  bool m_hasAlignmentPath;

  AlignmentCondition* m_alignmentCondition;

  /// Transformation from the  global reference system
  /// to the local reference system.
  /// Total matrix, including ideal alignment plus deltas.
  mutable HepTransform3D*              m_matrix;

  /// Transformation from the  global reference system
  /// to the local reference system.
  /// Ideal geometry with no misalignments.
  mutable HepTransform3D*              m_idealMatrix;

  mutable HepTransform3D*              m_localIdealMatrix;
  mutable HepTransform3D*              m_localDeltaMatrix;

  /** transformation FROM local reference system  to the global 
   *  reference system
   */

  /** transformation FROM local reference system  to the global 
   *  reference system
   */

  mutable HepTransform3D*              m_matrixInv;
  mutable HepTransform3D*              m_idealMatrixInv;

  /// flag for support association 
  bool                                 m_gi_has_support     ; 
  /**  name of DetectorElement (full path(address) in the 
   *   Transient Store) , which supports the addres)
   */
  std::string                          m_gi_supportName     ;   
  /// C++ pointer to element, which supports the address (loaded on demand) 
  mutable IGeometryInfo*               m_gi_support         ;  
  /// the address itself (numeric replic apath) 
  mutable ILVolume::ReplicaPath        m_gi_supportPath     ;   
  /// another form of address  (name replica path)
  std::string                          m_gi_supportNamePath ;   
  //
  ///  The corresponding IDtectorElement object 
  mutable IDetectorElement*            m_gi_iDetectorElement ;

  /// flag for  parent object 
  mutable bool                         m_gi_parentLoaded   ;
  /// C++ pointer to parent object (resolved on demand only) 
  mutable IGeometryInfo*               m_gi_parent         ; 
  /// flag for children objects 
  mutable bool                         m_gi_childLoaded    ;
  /**  container of pointers to children objects 
   *  (resolved on demand only)
   */
  mutable IGeometryInfo::IGIChildrens  m_gi_childrens      ;
  /** container of names of children objects 
   * (resolved on demand only)
   */
  mutable GeometryInfoPlus::ChildName      m_gi_childrensNames ;
  
  /// object/reference counter  
  static unsigned long m_count;
  
  /// reference to services
  DetDesc::Services* m_services;

  /// Keep for now the local and all the parents ideal and delta
  /// transformation matrices. 
  /// The code is written such that these could become automatic, 
  /// temporary objects in one algorithm. Juan.
  std::vector<HepTransform3D> m_pvMatrices;
  std::vector<HepTransform3D> m_deltaMatrices;

  
  /// flag for alignment condition
  bool                                 m_has_condition     ; 
  /// Path on data store where alignment condition is to be found
  std::string                          m_gi_condPath     ;


  inline matrix_iterator idealBegin() const
  {
    return m_pvMatrices.begin();
  }
  
  inline matrix_iterator idealEnd() const
  {
    return m_pvMatrices.end();
  }

  inline matrix_iterator deltaBegin() const
  {
    return m_deltaMatrices.begin();
  }
  
  inline matrix_iterator deltaEnd() const
  {
    return m_deltaMatrices.end();
  }
};
#endif // LIB_GEOMETRYINFOPLUS_H
