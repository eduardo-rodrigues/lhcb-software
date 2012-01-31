// $Id: PackedParticle.cpp,v 1.3 2010-05-19 09:04:08 jonrob Exp $

// local
#include "Event/PackedParticle.h"

// Checks
#include "Event/PackedEventChecks.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

using namespace LHCb;

void ParticlePacker::pack( const DataVector & parts,
                           PackedDataVector & pparts ) const {
  pparts.data().reserve( parts.size() );
  if ( 0 == pparts.packingVersion()  ) {
    for ( DataVector::const_iterator iD = parts.begin();
          iD != parts.end(); ++iD ) {
      const Data & part = **iD;

      // Make a new packed data object and save
      pparts.data().push_back( PackedData() );
      PackedData & ppart = pparts.data().back();

      // fill ppart from part
      ppart.key = part.key();

      // Particle ID
      ppart.particleID = part.particleID().pid();

      // Mass and error
      ppart.measMass    = m_pack.mass( part.measuredMass()    );
      ppart.measMassErr = m_pack.mass( part.measuredMassErr() );

      // Lorentz vector
      const double px = part.momentum().px();
      const double py = part.momentum().py();
      const double pz = part.momentum().pz();
      ppart.lv_px = m_pack.slope( fabs(pz) > 0 ? px/pz : 0.0 );
      ppart.lv_py = m_pack.slope( fabs(pz) > 0 ? py/pz : 0.0 );
      ppart.lv_pz = m_pack.energy( pz );
      ppart.lv_mass = (float) part.momentum().M() ;

      // reference point
      ppart.refx = m_pack.position( part.referencePoint().x() );
      ppart.refy = m_pack.position( part.referencePoint().y() );
      ppart.refz = m_pack.position( part.referencePoint().z() );

      // Mom Cov
      const double merr00 = std::sqrt( part.momCovMatrix()(0,0) );
      const double merr11 = std::sqrt( part.momCovMatrix()(1,1) );
      const double merr22 = std::sqrt( part.momCovMatrix()(2,2) );
      const double merr33 = std::sqrt( part.momCovMatrix()(3,3) );
      ppart.momCov00 = m_pack.slope( merr00/px );
      ppart.momCov11 = m_pack.slope( merr11/py );
      ppart.momCov22 = m_pack.energy( merr22 );
      ppart.momCov33 = m_pack.energy( merr33 );
      ppart.momCov10 = m_pack.fraction( part.momCovMatrix()(1,0) / (merr11*merr00) );
      ppart.momCov20 = m_pack.fraction( part.momCovMatrix()(2,0) / (merr22*merr00) );
      ppart.momCov21 = m_pack.fraction( part.momCovMatrix()(2,1) / (merr22*merr11) );
      ppart.momCov30 = m_pack.fraction( part.momCovMatrix()(3,0) / (merr33*merr00) );
      ppart.momCov31 = m_pack.fraction( part.momCovMatrix()(3,1) / (merr33*merr11) );
      ppart.momCov32 = m_pack.fraction( part.momCovMatrix()(3,2) / (merr33*merr22) );

      // Pos Cov
      const double perr00 = std::sqrt( part.posCovMatrix()(0,0) );
      const double perr11 = std::sqrt( part.posCovMatrix()(1,1) );
      const double perr22 = std::sqrt( part.posCovMatrix()(2,2) );
      ppart.posCov00 = m_pack.position( perr00 );
      ppart.posCov11 = m_pack.position( perr11 );
      ppart.posCov22 = m_pack.position( perr22 );
      ppart.posCov10 = m_pack.fraction( part.posCovMatrix()(1,0) / (perr11*perr00) );
      ppart.posCov20 = m_pack.fraction( part.posCovMatrix()(2,0) / (perr22*perr00) );
      ppart.posCov21 = m_pack.fraction( part.posCovMatrix()(2,1) / (perr22*perr11) );

      // PosMom Cov
      ppart.pmCov00 = m_pack.fltPacked( part.posMomCovMatrix()(0,0) );
      ppart.pmCov01 = m_pack.fltPacked( part.posMomCovMatrix()(0,1) );
      ppart.pmCov02 = m_pack.fltPacked( part.posMomCovMatrix()(0,2) );
      ppart.pmCov10 = m_pack.fltPacked( part.posMomCovMatrix()(1,0) );
      ppart.pmCov11 = m_pack.fltPacked( part.posMomCovMatrix()(1,1) );
      ppart.pmCov12 = m_pack.fltPacked( part.posMomCovMatrix()(1,2) );
      ppart.pmCov20 = m_pack.fltPacked( part.posMomCovMatrix()(2,0) );
      ppart.pmCov21 = m_pack.fltPacked( part.posMomCovMatrix()(2,1) );
      ppart.pmCov22 = m_pack.fltPacked( part.posMomCovMatrix()(2,2) );
      ppart.pmCov30 = m_pack.fltPacked( part.posMomCovMatrix()(3,0) );
      ppart.pmCov31 = m_pack.fltPacked( part.posMomCovMatrix()(3,1) );
      ppart.pmCov32 = m_pack.fltPacked( part.posMomCovMatrix()(3,2) );

      // extra info
      ppart.firstExtra = pparts.extra().size();
      for ( LHCb::Particle::ExtraInfo::const_iterator iE = part.extraInfo().begin();
            iE != part.extraInfo().end(); ++iE )
      {
        pparts.extra().push_back( PackedDataVector::PackedExtraInfo(iE->first,
                                                                    m_pack.fltPacked(iE->second)) );
      }
      ppart.lastExtra = pparts.extra().size();

      // end vertex
      if ( part.endVertex() )
      {
        ppart.vertex = m_pack.reference( &pparts,
                                         part.endVertex()->parent(),
                                         part.endVertex()->key() );
      }

      // protoparticle
      if ( part.proto() )
      {
        ppart.proto = m_pack.reference( &pparts,
                                        part.proto()->parent(),
                                        part.proto()->key() );
      }

      // daughters
      ppart.firstDaughter = pparts.daughters().size();
      for ( SmartRefVector<LHCb::Particle>::const_iterator iD1 = part.daughters().begin();
            iD1 != part.daughters().end(); ++iD1 )
      {
        const LHCb::Particle * mcP = *iD1;
        if ( mcP )
        {
          pparts.daughters().push_back( m_pack.reference( &pparts,
                                                          mcP->parent(),
                                                          mcP->key() ) );
        }
      }
      ppart.lastDaughter = pparts.daughters().size();

    } // loop over parts
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pparts.packingVersion();
    throw GaudiException( mess.str(), "ParticlePacker", StatusCode::FAILURE );
  }
}

void ParticlePacker::unpack( const PackedDataVector & pparts,
                             DataVector       & parts ) const
{
  parts.reserve( pparts.data().size() );
  if ( 0 == pparts.packingVersion() )
  {
    for ( PackedDataVector::Vector::const_iterator iD = pparts.data().begin();
          iD != pparts.data().end(); ++iD )
    {
      const PackedData & ppart = *iD;

      // make and save new pid in container
      Data * part = new Data();
      parts.add( part );

      // Fill data from packed object

      // particle ID
      part->setParticleID( LHCb::ParticleID(ppart.particleID) );

      // Mass and error
      part->setMeasuredMass   ( m_pack.mass(ppart.measMass) );
      part->setMeasuredMassErr( m_pack.mass(ppart.measMassErr) );

      // Lorentz momentum vector
      const double pz   = m_pack.energy( ppart.lv_pz );
      const double px   = m_pack.slope( ppart.lv_px ) * pz;
      const double py   = m_pack.slope( ppart.lv_py ) * pz;
      const double mass = ppart.lv_mass;
      part->setMomentum( Gaudi::LorentzVector( px, py, pz, 
                                               std::sqrt(px*px+py*py+pz*pz+mass*mass) ) );

      // reference point
      part->setReferencePoint( Gaudi::XYZPoint( m_pack.position(ppart.refx),
                                                m_pack.position(ppart.refy),
                                                m_pack.position(ppart.refz) ) );

      // Mom Cov
      Gaudi::SymMatrix4x4 & momCov = *(const_cast<Gaudi::SymMatrix4x4*>(&part->momCovMatrix()));
      const double merr00 = m_pack.slope( ppart.momCov00 ) * px;
      const double merr11 = m_pack.slope( ppart.momCov11 ) * py;
      const double merr22 = m_pack.energy( ppart.momCov22 );
      const double merr33 = m_pack.energy( ppart.momCov33 );
      momCov(0,0) = std::pow( merr00, 2 );
      momCov(1,1) = std::pow( merr11, 2 );
      momCov(2,2) = std::pow( merr22, 2 );
      momCov(3,3) = std::pow( merr33, 2 );
      momCov(1,0) = merr11*merr00 * m_pack.fraction( ppart.momCov10 );
      momCov(2,0) = merr22*merr00 * m_pack.fraction( ppart.momCov20 );
      momCov(2,1) = merr22*merr11 * m_pack.fraction( ppart.momCov21 );
      momCov(3,0) = merr33*merr00 * m_pack.fraction( ppart.momCov30 );
      momCov(3,1) = merr33*merr11 * m_pack.fraction( ppart.momCov31 );
      momCov(3,2) = merr33*merr22 * m_pack.fraction( ppart.momCov32 );

      // Pos Cov
      Gaudi::SymMatrix3x3 & posCov = *(const_cast<Gaudi::SymMatrix3x3*>(&part->posCovMatrix()));
      const double perr00 = m_pack.position( ppart.posCov00 );
      const double perr11 = m_pack.position( ppart.posCov11 );
      const double perr22 = m_pack.position( ppart.posCov22 );
      posCov(0,0) = std::pow( perr00, 2 );
      posCov(1,1) = std::pow( perr11, 2 );
      posCov(2,2) = std::pow( perr22, 2 );
      posCov(1,0) = perr11*perr00 * m_pack.fraction( ppart.posCov10 );
      posCov(2,0) = perr22*perr00 * m_pack.fraction( ppart.posCov20 );
      posCov(2,1) = perr22*perr11 * m_pack.fraction( ppart.posCov21 );

      // Pos Mom Cov
      Gaudi::Matrix4x3 & pmCov = *(const_cast<Gaudi::Matrix4x3*>(&part->posMomCovMatrix()));
      pmCov(0,0) = m_pack.fltPacked( ppart.pmCov00 );
      pmCov(0,1) = m_pack.fltPacked( ppart.pmCov01 );
      pmCov(0,2) = m_pack.fltPacked( ppart.pmCov02 );
      pmCov(1,0) = m_pack.fltPacked( ppart.pmCov10 );
      pmCov(1,1) = m_pack.fltPacked( ppart.pmCov11 );
      pmCov(1,2) = m_pack.fltPacked( ppart.pmCov12 );
      pmCov(2,0) = m_pack.fltPacked( ppart.pmCov20 );
      pmCov(2,1) = m_pack.fltPacked( ppart.pmCov21 );
      pmCov(2,2) = m_pack.fltPacked( ppart.pmCov22 );
      pmCov(3,0) = m_pack.fltPacked( ppart.pmCov30 );
      pmCov(3,1) = m_pack.fltPacked( ppart.pmCov31 );
      pmCov(3,2) = m_pack.fltPacked( ppart.pmCov32 );

      // extra info
      for ( int iE = ppart.firstExtra; iE < ppart.lastExtra; ++iE )
      {
        const PackedDataVector::PackedExtraInfo& pInfo =  pparts.extra()[iE];
        part->addInfo( pInfo.first, m_pack.fltPacked(pInfo.second) );
      }

      // end vertex
      if ( -1 != ppart.vertex )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( ppart.vertex, &pparts, &parts, hintID, key );
        SmartRef<LHCb::Vertex> ref(&parts,hintID,key);
        part->setEndVertex( ref );
      }

      // protoparticle
      if ( -1 != ppart.proto )
      {
        int hintID(0), key(0);
        m_pack.hintAndKey( ppart.proto, &pparts, &parts, hintID, key );
        SmartRef<LHCb::ProtoParticle> ref(&parts,hintID,key);
        part->setProto( ref );
      }

      // daughters
      
      for ( unsigned short int iiD = ppart.firstDaughter; iiD < ppart.lastDaughter; ++iiD )
      {
        const int & iD1 = pparts.daughters()[iiD];
        int hintID(0), key(0);
        m_pack.hintAndKey( iD1, &pparts, &parts, hintID, key );
        SmartRef<LHCb::Particle> ref(&parts,hintID,key);
        part->addToDaughters( ref );
      }

    } // loop over packed parts
  }
  else
  {
    std::ostringstream mess;
    mess << "Unknown packed data version " << (int)pparts.packingVersion();
    throw GaudiException( mess.str(), "ParticlePacker", StatusCode::FAILURE );
  }
}

StatusCode ParticlePacker::check( const DataVector & dataA,
                                  const DataVector & dataB,
                                  GaudiAlgorithm & parent ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // checker
  const DataPacking::DataChecks ch(parent);

  // Loop over data containers together and compare
  DataVector::const_iterator iA(dataA.begin()), iB(dataB.begin());
  for ( ; iA != dataA.end() && iB != dataB.end(); ++iA, ++iB )
  {
    // assume OK from the start
    bool ok = true;
 
    // checks here

    // PID
    ok &= (*iA)->particleID() == (*iB)->particleID();

    // Mass
    ok &= ch.compareDoubles( "MeasuredMass", 
                             (*iA)->measuredMass(), (*iB)->measuredMass() );
    ok &= ch.compareDoubles( "MeasuredMassError", 
                             (*iA)->measuredMassErr(), (*iB)->measuredMassErr() );

    // momentum
    ok &= ch.compareLorentzVectors( "Momentum",
                                    (*iA)->momentum(), (*iB)->momentum() );

    // reference position
    ok &= ch.comparePoints( "ReferencePoint", 
                            (*iA)->referencePoint(), (*iB)->referencePoint() );

    // Mom Cov
    ok &= ch.compareMatrices<Gaudi::SymMatrix4x4,4,4>( "MomCov", 
                                                       (*iA)->momCovMatrix(), 
                                                       (*iB)->momCovMatrix() );

    // Pos Cov
    ok &= ch.compareMatrices<Gaudi::SymMatrix3x3,3,3>( "PosCov", 
                                                       (*iA)->posCovMatrix(), 
                                                       (*iB)->posCovMatrix() );

    // PosMom Cov
    ok &= ch.compareMatrices<Gaudi::Matrix4x3,4,3>( "PosMomCov", 
                                                    (*iA)->posMomCovMatrix(), 
                                                    (*iB)->posMomCovMatrix() );

    // Extra info
    const bool extraSizeOK = (*iA)->extraInfo().size() == (*iB)->extraInfo().size();
    ok &= extraSizeOK;
    if ( extraSizeOK )
    {
      LHCb::Particle::ExtraInfo::const_iterator iEA = (*iA)->extraInfo().begin();
      LHCb::Particle::ExtraInfo::const_iterator iEB = (*iB)->extraInfo().begin();
      for ( ; iEA != (*iA)->extraInfo().end() && iEB != (*iB)->extraInfo().end();
            ++iEA, ++iEB )
      {
        const bool keyOK   = iEA->first == iEB->first;
        const bool valueOK = ch.compareDoubles( "ExtraInfo", iEA->second, iEB->second );
        ok &= keyOK && valueOK;
      }
    }
    else
    {
      parent.warning() << "ExtraInfo different sizes" << endmsg;
    }

    // end vertex
    ok &= ch.comparePointers( "EndVertex", (*iA)->endVertex(), (*iB)->endVertex() );

    // proto particle
    ok &= ch.comparePointers( "ProtoParticle", (*iA)->proto(), (*iB)->proto() );

    // daughters
    const bool dauSizeOK = (*iA)->daughters().size() == (*iB)->daughters().size();
    ok &= dauSizeOK;
    if ( dauSizeOK )
    {
      SmartRefVector<LHCb::Particle>::const_iterator iDA = (*iA)->daughters().begin();
      SmartRefVector<LHCb::Particle>::const_iterator iDB = (*iB)->daughters().begin();
      for ( ; iDA != (*iA)->daughters().end() && iDB != (*iB)->daughters().end();
            ++iDA, ++iDB )
      {
        ok &= ch.comparePointers( "Daughters", &**iDA, &**iDB );
      }
    }
    else
    {
      parent.warning() << "Daughters different sizes" << endmsg;
    }

    // force printout for tests
    //ok = false;
    // If comparison not OK, print full information
    if ( !ok )
    {
      parent.warning() << "Problem with Particle data packing :-" << endmsg
                       << "  Original Particle : " << **iA
                       << endmsg
                       << "  Unpacked Particle : " << **iB
                       << endmsg;
      sc = StatusCode::FAILURE;
    }
  }

  // Return final status
  return sc;
}
