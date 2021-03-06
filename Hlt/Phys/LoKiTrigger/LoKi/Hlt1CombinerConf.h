// $Id: Hlt1CombinerConf.h 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
#ifndef LOKI_HLT1COMBINERCONF_H
#define LOKI_HLT1COMBINERCONF_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/GetDecay.h"
#include "Kernel/GetParticlesForDecay.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/ATypes.h"
#include "LoKi/Functor.h"
#include "LoKi/ToCpp.h"
#include "LoKi/TrgToCpp.h"
// ============================================================================
/** @file  LoKi/Hlt1CombinerConf.h
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Matthew KENZIE matthew.kenzie@cern.ch
 *  @date   2014-11-27
 *
 *                    $Revision: 180655 $
 *  Last Modification $Date: 2014-11-25 11:38:48 +0100 (Tue, 25 Nov 2014) $
 *                 by $Author: mkenzie $
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class Hlt1CombinerConf Hlt1CombinerConf.h LoKi/Hlt1CombinerConf.h
     *
     *
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date   2014-11-27
     */
    class GAUDI_API Hlt1CombinerConf
    {
    public:
      // =====================================================================
      ///  constructor from the decay descriptor, cuts and combiner
      Hlt1CombinerConf
        ( const std::string&                                                decay         ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut       ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut       ,
          const std::string&                                                combiner = "" ) ;
      // =====================================================================
      ///  constructor from vector of decay descriptors, cuts and combiner
      Hlt1CombinerConf
        ( const std::vector<std::string>&                                   decays        ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut       ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut       ,
          const std::string&                                                combiner = "" ) ;
      // =====================================================================
      ///  constructor from the decay descriptor, cuts and combiner
      Hlt1CombinerConf
        ( const std::string&                                                decay     ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut   ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut   ,
          const std::string&                                                combiner  = "" ) ;
      // =====================================================================
      ///  constructor from vector of decay descriptors, cuts and combiner
      Hlt1CombinerConf
        ( const std::vector<std::string>&                                   decays   ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut  ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut  ,
          const std::string&                                                combiner  = "" ) ;
      // =====================================================================
      ///  constructor from the decay descriptor, cuts and combiner
      Hlt1CombinerConf
        ( const std::string&                                                decay      ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
          const std::string&                                                combiner   = "" ) ;
      // =====================================================================
      ///  constructor from vector of decay descriptors, cuts and combiner
      Hlt1CombinerConf
        ( const std::vector<std::string>&                                   decays     ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
          const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
          const std::string&                                                combiner   = "" ) ;
      /// virtual destructor
      virtual ~Hlt1CombinerConf () ;                           // virtual destructor
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor id disabled
      Hlt1CombinerConf() ;                    // the default constructor id disabled
      // ======================================================================
    public: // the properties
      // ======================================================================
      /// the decay string
      const std::vector<std::string>&                       decaystrs() const
      { return m_decstrings ; }
      /// the combination cut
      const LoKi::Functor<LoKi::ATypes::Combination,bool>&  acut() const
      { return m_acut ; }
      /// the combination12 cut
      const LoKi::Functor<LoKi::ATypes::Combination,bool>&  acut12() const
      { return m_acut12 ; }
      /// the combination123 cut
      const LoKi::Functor<LoKi::ATypes::Combination,bool>&  acut123() const
      { return m_acut123 ; }
      /// the mother cut
      const LoKi::Functor<const LHCb::Particle*,bool>&      cut() const
      { return m_cut ; }
      /// the combiner tool
      const std::string&                                    combiner() const
      { return m_combinertool ; }
      // ======================================================================
    public:
      // ======================================================================
      /// apply the cuts
      bool combcut    ( const LoKi::ATypes::Combination c ) const
      { return m_acut   .fun ( c ) ; }
      bool combcut12  ( const LoKi::ATypes::Combination c ) const
      { return m_acut12 .fun ( c ) ; }
      bool combcut123 ( const LoKi::ATypes::Combination c ) const
      { return m_acut123.fun ( c ) ; }
      bool mothcut    ( const LHCb::Particle*           p ) const
      { return m_cut    .fun ( p ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// printout
      std::ostream& fillStream ( std::ostream& s ) const ; 
      std::string   toString   () const ;
      std::string   toCpp      () const ;
      // ======================================================================
    private:
      // ======================================================================
      // combination criteria
      // ======================================================================
      /// the decay string
      std::vector<std::string>                                  m_decstrings   ;
      /// the combination cut
      LoKi::FunctorFromFunctor<LoKi::ATypes::Combination, bool> m_acut         ;
      /// the combination cut for N>2 combinations
      LoKi::FunctorFromFunctor<LoKi::ATypes::Combination, bool> m_acut12       ;
      /// the combination cut for N>3 combinations
      LoKi::FunctorFromFunctor<LoKi::ATypes::Combination, bool> m_acut123      ;
      /// the mother cut
      LoKi::FunctorFromFunctor<const LHCb::Particle*, bool>     m_cut          ;
      /// the combiner tool name
      std::string                                               m_combinertool ;
      // ======================================================================
    private: 
      // ======================================================================
      /// C++ image 
      std::string   m_toCpp ;
      // ======================================================================
    };
    // ========================================================================
    /// output operator to ostream
    inline
    std::ostream& operator<<( std::ostream&                       s ,
                              const LoKi::Hlt1::Hlt1CombinerConf& o )
    { return o.fillStream ( s ) ; }
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::Hlt1CombinerConf& o ) ;
    // ========================================================================
  }
  // ==========================================================================
}
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_HLT1COMBINERCONF_H
// ============================================================================


