// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlUserDetElemCnv.h,v 1.5 2001-10-17 13:12:43 sponce Exp $

#ifndef DDEXAMPLE_XMLUSERDETELEMCNV_H
#define DDEXAMPLE_XMLUSERDETELEMCNV_H 1

// Include files
#include "DetDesc/XmlBaseDetElemCnv.h"
#include "GaudiKernel/MsgStream.h"

// These two includes are not needed here but are needed inside children
// of this class. In order to avoid the user to forget to include them,
// we put it here
#include "GaudiKernel/CnvFactory.h"
#include "DetDesc/IXmlSvc.h"


/** @class XmlUserDetElemCnv XmlLVolumeCnv.h DetDesc/XmlUserDetElemCnv.h
 *
 * This is an empty, abstract, user specific, converter for DetectorElements.
 * Every actual specific convertor for DetectorElement should inherit from this
 * one. It basically provides the i_createObj, classID methods plus a
 * constructor and a destructor. The only thing to do in order to implement
 * an actual specific converter is thus to implement the second
 * i_fillSpecificObj method.
 *
 * @author Sebastien Ponce
 */
template <class DeType> class XmlUserDetElemCnv : public XmlBaseDetElemCnv {
  
  /// The type of this object
  typedef XmlUserDetElemCnv<DeType> MyType;
  /// Friend needed for instantiation
  friend class CnvFactory<MyType>;
  

 public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlUserDetElemCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  virtual ~XmlUserDetElemCnv() {};
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID() { return DeType::classID(); }


 protected:

  /** Creates the transient representation of an object from a DOM_Element.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (DOM_Element element,
                                  DataObject*& refpObject);  

  /** This fills the current object for specific child.
   * This is a very general method reimplemented from XmlBaseDetElemCnv.
   * It basically calls the other i_fillSpecificObj after casting the
   * current object to its real type.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DetectorElement* refpObject);

  /** This fills the current object for specific child.
   * This should never be called directly but always through the other
   * i_fillSpecificObj. This actually only exist to avoid the user to
   * implement a dynamic cast from DetectorElement into DeType.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeType* dataObj);
};


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
template <class DeType>
XmlUserDetElemCnv<DeType>::XmlUserDetElemCnv (ISvcLocator* svc)
  : XmlBaseDetElemCnv (svc, DeType::classID()) {
} // end constructor


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
template <class DeType>
StatusCode XmlUserDetElemCnv<DeType>::i_createObj(DOM_Element /*element*/,
                                                  DataObject*& /*refpObject*/) {
  MsgStream log(msgSvc(), "XmlUserDetElemCnv" );
  log << MSG::DEBUG << "entering i_createObj" << endreq;
  
  // creates an object for the node found
  refpObject = new DeType();
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
template <class DeType>
StatusCode
XmlUserDetElemCnv<DeType>::i_fillSpecificObj (DOM_Element childElement,
                                              DetectorElement* refpObject) {
  DeType* dataObj = dynamic_cast<DeType*> (refpObject);
  return i_fillSpecificObj (childElement, dataObj);
}

// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
template <class DeType>
StatusCode
XmlUserDetElemCnv<DeType>::i_fillSpecificObj (DOM_Element /*childElement*/,
                                              DeType* /*dataObj*/) {
  return StatusCode::SUCCESS;
}

#endif // DDEXAMPLE_XMLUSERDETELEMCNV_H
