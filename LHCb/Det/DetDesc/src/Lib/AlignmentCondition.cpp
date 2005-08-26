// $Id: AlignmentCondition.cpp,v 1.4 2005-08-26 09:34:19 jpalac Exp $
// Include files

#include "DetDesc/AlignmentCondition.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"
//-----------------------------------------------------------------------------
// Implementation file for class : AlignmentCondition
//
// 2005-04-12 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor
//=============================================================================
AlignmentCondition::AlignmentCondition(  ) : 
  Condition(),
  m_services(DetDesc::services()),
  m_translationString("dPosXYZ"),
  m_rotationString("dRotXYZ"),
  m_pivotString("pivotXYZ")
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Constructing default AlignmentCondition, classID " 
      << classID()
      << endmsg;

}
//=============================================================================
AlignmentCondition::AlignmentCondition(const std::vector<double>& translation,
                                       const std::vector<double>& rotation,
                                       const std::vector<double>& pivot) : 
  Condition(),
  m_services(DetDesc::services()),
  m_translationString("dPosXYZ"),
  m_rotationString("dRotXYZ"),
  m_pivotString("pivotXYZ")
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Constructing AlignmentCondition from transformation parameters. classID " 
      << classID()
      << endmsg;

  setTransformation(translation, rotation, pivot);

}

//=============================================================================
// Constructor
//=============================================================================
AlignmentCondition::AlignmentCondition(const ITime& since, const ITime& till )
  :
  Condition (since, till),
  m_services(DetDesc::services()),
  m_translationString("dPosXYZ"),
  m_rotationString("dRotXYZ"),
  m_pivotString("pivotXYZ")
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Constructing AlignmentCondition classID "
      << classID()
      << endmsg;

}

//=============================================================================
// Destructor
//=============================================================================
AlignmentCondition::~AlignmentCondition() {}; 

//=============================================================================
/// initialize
StatusCode AlignmentCondition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  return makeMatrices();
}

//=============================================================================
const HepTranslate3D* AlignmentCondition::XYZTranslation(const std::vector<double>& coefficients) const
{
  return (coefficients.size()==3) ? new HepTranslate3D(coefficients[0],
                                                       coefficients[1],
                                                       coefficients[2]) :
    new HepTranslate3D();
}
//=============================================================================
const HepTransform3D* AlignmentCondition::XYZRotation(const std::vector<double>& coefficients) const
{
  if (coefficients.size()!=3) return new HepTransform3D();
  
  HepTransform3D* result = new HepRotateX3D(coefficients[0]);
  *result = *result * HepRotateY3D(coefficients[1]);
  *result = *result * HepRotateZ3D(coefficients[2]);
  return result;
  
}
//=============================================================================
StatusCode AlignmentCondition::makeMatrices() 
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Making transformation matrix" << endmsg;
  
  std::vector<double> translations = paramAsDoubleVect (m_translationString);
  std::vector<double> rotations    = paramAsDoubleVect (m_rotationString);
  std::vector<double> pivot = (exists(m_pivotString) ) ? 
    paramAsDoubleVect(m_pivotString) : std::vector<double>(3);

  std::transform(pivot.begin(), pivot.end(), 
                 pivot.begin(), std::negate<double>());
  
  if (translations.size()==3  && rotations.size()==3 && pivot.size()==3) {

    m_matrixInv =  
      ( *XYZTranslation( translations ) ) *
      ( ( *XYZTranslation( pivot )).inverse() *
      ( ( *XYZRotation( rotations )       ) *
        ( *XYZTranslation( pivot )        ) ));

    m_matrix = m_matrixInv.inverse();
    return StatusCode::SUCCESS;
  } else {
    log << MSG::ERROR << "Translations vector has funny size: "
        << translations.size() << ". Assigning identity matrices" << endmsg;
    m_matrixInv=HepTranslate3D();
    m_matrix=m_matrixInv;
    return StatusCode::FAILURE;
  }

}
//=============================================================================
IMessageSvc* AlignmentCondition::msgSvc() const {
  return m_services->msgSvc();
}
//=============================================================================
