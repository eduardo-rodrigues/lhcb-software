/// =========================================================================== 
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/04 18:15:54  ibelyaev
/// Shared cell algorimth implementation
///
/// =========================================================================== 
#ifndef COMPONENT_SHAREDCELLS_H 
#define COMPONENT_SHAREDCELLS_H 1
/// =========================================================================== 
#include <cmath>
// Include files
#include "CLHEP/Geometry/Point3D.h"
/** 
 *  set of inline functions used for CaloSharedCellalg 
 *
 *  @author Ivan Belyaev
 *  @date   04/07/2001
 */

namespace SharedCells
{

  /// =========================================================================
  /** Redistribute the energy proportional to a given weights 
   *  @param   List    a "list" of clusters for given cell 
   *  @param   Weight  "list"  of weights  
   *  @return  status code 
   */
  /// =========================================================================
  template <class L, class W>
  inline StatusCode redistribute( L& List , const W& Weight )
  {
    ///
    if( List.size() != Weight.size() ) { return StatusCode::FAILURE; }
    /// total weight 
    double wTot = std::accumulate( Weight.begin() , Weight.end() , 0.0 );
    /// redistribute the energy
    for( typename L::iterator iC = List.begin() ; List.end() != iC ; ++iC )
      {
        const CaloCluster* cluster = iC->first ;
        /// ignore artificial zeros
        if( 0 == cluster ) { continue ; }     ///< CONTINUE !!!
        const unsigned index = iC - List.begin() ;
        const double   frac  = Weight[index] / wTot ;
        iC->second->second.setFraction( frac )   ;
      }  
    ///
    return StatusCode::SUCCESS;
  };
  
  /// =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to a total(summed)
   *  energy of clusters. 
   *  the method is iterative(recursive)! 
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *  @param   List  a "list" of clusters for given cell 
   *  @param   NiT   number of iterations 
   *  @return  status code 
   */
  /// =========================================================================
  template <class L>
  inline StatusCode summedEnergyAlgorithm( L& List , int NiT )
  {
    /// does one need more iterations? 
    if( 0 >= NiT         ) { return StatusCode::SUCCESS ; }  
    /// empty cluster list ( nothing to be done)
    if( 0 == List.size() ) { return StatusCode::SUCCESS ; }
    ///
    /// prepare the container of weights 
    std::vector<double>  weight( List.size() , 1.0 );
    /// loop over all clusters and calculate the weight for each cluster  
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const CaloCluster* cluster = iC->first ;
        /// ignore artificial zeros 
        if( 0 == cluster ) { continue ; }     ///< CONTINUE !!!
        const unsigned index = iC - List.begin() ;
        /// calculate the energy of each cluster
        const double   eClu  = 
          ClusterFunctors::energy( cluster->digits().begin() ,
                                   cluster->digits().end  () ) ;
        /// define the weight for this cluster 
        weight[index]  = eClu ;
      }
    /// make redistribution of energy
    SharedCells::redistribute( List , weight );
    /// make iteration  
    return SharedCells::summedEnergyAlgorithm( List , --NiT  ) ;  
  };
  
  /// =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to energies of seed cells  
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *  @param   List   a "list" of clusters for given cell 
   *  @param   type   "type" of seed cell 
   *  @return  status code 
   */
  /// =========================================================================
  template <class L, class T>
  inline StatusCode seedEnergyAlgorithm( L& List , const T& type )
  {
    /// empty cluster list 
    if( 0 == List.size()     ) { return StatusCode::SUCCESS ; }
    /// prepare container of weights 
    std::vector<double> weight( List.size() , 1.0 );
    ///< collect the information
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const CaloCluster* cluster = iC->first ;
        /// ignore artificial zeroes
        if( 0 == cluster ) { continue ; }  
        /// locate (first) seed cell 
        CaloCluster::Digits::const_iterator iSeed =
          ClusterFunctors::locateDigit( cluster->digits().begin() ,
                                        cluster->digits().end  () ,
                                        type                      );
        /// success ???
        if( cluster->digits().end() == iSeed ) { return StatusCode::FAILURE ; }
        const CaloDigit* seed = iSeed->first;
        if( 0 == seed )                        { return StatusCode::FAILURE ; }
        /// get the energy of the seed cell 
        const double     e  = seed->e() ;
        /// set the weight 
        const unsigned index = iC - List.begin() ;
        weight[index]  = e  ;
      }
    /// redistribute the  energy  
    return SharedCells::redistribute( List , weight );
  };
  
  /// =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to energies of seed cells  
   *  weighted with distance function between center of the seed cell and 
   *  the given cell center
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *  @param   List   a "list" of clusters for given cell 
   *  @param   Det    source of geometry information 
   *  @param   type   "type" of seed cell 
   *  @param   ID     cell ID of given cell 
   *  @param   size   vector of shower size measures 
   *  @return  status code 
   */
  /// =========================================================================
  template <class L, class DE, class T, class SIZE>
  inline StatusCode seedDistanceAlgorithm( L&                List , 
                                           const DE&         Det  ,
                                           const T&          type ,
                                           const CaloCellID& ID   ,
                                           const SIZE&       size )
  {
    /// valid detector information ??
    if( !Det                 ) { return StatusCode::FAILURE ; }
    /// empty cluster list 
    if( 0 == List.size()     ) { return StatusCode::SUCCESS ; }
    /// prepare container of weights 
    std::vector<double> weight( List.size() , 1.0 );
    const HepPoint3D position( Det->cellCenter( ID ) );
    /// collect the information
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const CaloCluster* cluster = iC->first ;
        /// ignore artificial zeroes
        if( 0 == cluster ) { continue ; }  
        /// locate (first) seed cell 
        CaloCluster::Digits::const_iterator iSeed =
          ClusterFunctors::locateDigit( cluster->digits().begin() ,
                                        cluster->digits().end  () ,
                                        type                      );
        /// success ???
        if( cluster->digits().end() == iSeed ) { return StatusCode::FAILURE ; }
        const CaloDigit* seed = iSeed->first ;
        if( 0 == seed                        ) { return StatusCode::FAILURE ; }
        /// get the energy of the seed cell
        const double     e  = seed->e() ;
        /// calculate the distance between seed cell and give cell 
        const double  dist  = 
          ( position - Det->cellCenter( seed->cellID() ) ).mag() ;
        /// set the weight 
        const unsigned index = iC - List.begin() ;
        /// calculate weight 
        weight[index]  = 
          e * exp( -1.0 * dist / size[ ID.area() ] ) ;
      }
    /// redistribute the energies  
    return SharedCells::redistribute( List , weight );
  };


  /// =========================================================================
  /** concrete method for redistribution of energy for shared cells. 
   *  It redistributes the energy proportionally to energies of seed cells  
   *  weighted with distance function between center of gravity of cluster 
   *  and center of given cell 
   *  method is iterative(recursive) 
   *  the method is artificially templated to avoid 
   *  a lot of explicit class specifications
   *  @param   List   a "list" of clusters for given cell 
   *  @param   Det    source of geometry information 
   *  @param   ID     cell ID of given cell 
   *  @param   size   vector of shower size measures 
   *  @param   NiT    number of iterations 
   *  @return  status code 
   */
  /// =========================================================================
  template <class L, class DE, class SIZE>
  inline StatusCode summedDistanceAlgorithm( L&                List , 
                                             const DE&         Det  ,
                                             const CaloCellID& ID   ,
                                             const SIZE&       size ,
                                             int               NiT  )
  {
    /// does one need more iterations ?
    if( 0 >= NiT             ) { return StatusCode::SUCCESS; }
    /// valid detector information ??
    if( !Det                 ) { return StatusCode::FAILURE ; }
    /// empty cluster list 
    if( 0 == List.size()     ) { return StatusCode::SUCCESS ; }
    /// prepare container of weights 
    std::vector<double> weight( List.size() , 1.0 );
    const HepPoint3D pos( Det->cellCenter( ID ) );
    /// collect the information
    for( typename L::const_iterator iC = List.begin() ; 
         List.end() != iC ; ++iC )
      {
        const CaloCluster* cluster = iC->first ;
        /// ignore artificial zeroes
        if( 0 == cluster ) { continue ; }
        double x , y , e ;
        StatusCode sc = 
          ClusterFunctors::calculateEXY( cluster->digits().begin() ,
                                         cluster->digits().end  () ,
                                         Det , e , x , y           );
        /// success ???
        if( sc.isFailure() ) { return sc ; }
        /// calculate the distance between seed cell and give cell 
        const double  dist  = 
          sqrt( ( pos.x() - x ) * ( pos.x() - x ) +  
                ( pos.y() - y ) * ( pos.y() - y ) ) ;
        /// set the weight 
        const unsigned index = iC - List.begin() ;
        /// calculate weight 
        weight[index]  = 
          e * exp( -1.0 * dist / size[ ID.area() ] ) ;
      }
    /// redistribute the energies  
    SharedCells::redistribute( List , weight );
    /// make iterations 
    return 
      SharedCells::summedDistanceAlgorithm( List, Det , ID , size, --NiT ) ;
  };


}; ///< end of namespace 


/// =========================================================================== 
#endif // COMPONENT_SHAREDCELLS_H
/// =========================================================================== 
