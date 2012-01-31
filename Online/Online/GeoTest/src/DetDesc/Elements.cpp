#include "DetDesc/Elements.h"
#include "XML/Evaluator.h"
#include <iostream>

#include "DetDesc/lcdd/LCDD.h"

namespace DetDesc  {
  XmlTools::Evaluator& evaluator();
}

namespace {
  XmlTools::Evaluator& eval(DetDesc::evaluator());
}

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

int DetDesc::Geometry::_toInt(const string& value)  {
  string s(value);
  size_t idx = s.find("(int)");
  if ( idx != string::npos ) 
    s.erase(idx,5);
  while(s[0]==' ')s.erase(0,1);
  double result = eval.evaluate(s.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  return (int)result;
}

bool   DetDesc::Geometry::_toBool(const string& value)   {
  return value == "true";
}

float DetDesc::Geometry::_toFloat(const string& value)   {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  return (float)result;
}

double DetDesc::Geometry::_toDouble(const string& value)   {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  return result;
}

void DetDesc::Geometry::_toDictionary(const string& name, const string& value)  {
  string n=name, v=value;
  size_t idx = v.find("(int)");
  if ( idx != string::npos ) 
    v.erase(idx,5);
  idx = v.find("(float)");
  if ( idx != string::npos ) 
    v.erase(idx,7);
  while(v[0]==' ')v.erase(0,1);
  double result = eval.evaluate(v.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
  }
  eval.setVariable(n.c_str(),result);
}

string DetDesc::Geometry::_toString(bool value)    {
  char text[32];
  ::sprintf(text,"%s",value ? "true" : "false");
  return text;
}

string DetDesc::Geometry::_toString(int value)   {
  char text[32];
  ::sprintf(text,"%d",value);
  return text;
}

string DetDesc::Geometry::_toString(float value)   {
  char text[32];
  ::sprintf(text,"%f",value);
  return text;
}

string DetDesc::Geometry::_toString(double value)   {
  char text[32];
  ::sprintf(text,"%f",value);
  return text;
}
namespace DetDesc { namespace Geometry {

  template <typename T> void Element_type<T>::bad_assignment(const type_info& from, const type_info& to) {
    string msg = "Wrong assingment from ";
    msg += from.name();
    msg += " to ";
    msg += to.name();
    msg += " not possible!!";
    throw std::runtime_error(msg);
  }

  template <typename T> const char* RefElement_type<T>::name() const  {
    TNamed *p = (TNamed*)this->m_element;
    return p ? p->GetName() : "";
  }

  template <typename T> const char* RefElement_type<T>::refName() const  {
    TNamed *p = (TNamed*)this->m_element;
    return p ? p->GetName() : "";
  }

  template <typename T> void RefElement_type<T>::setName(const string& new_name)  {
    TNamed *p = (TNamed*)this->m_element;
    if ( p ) p->SetName(new_name.c_str());
  }

  template <typename T> void RefElement_type<T>::assign(T* n, const std::string& nam, const std::string& tit) {
    this->m_element = n;
    TNamed *p = (TNamed*)n;
    if ( !nam.empty() ) p->SetName(nam.c_str());
    if ( !tit.empty() ) p->SetTitle(tit.c_str());
  }


  template struct Element_type<TNamed>;
}}

const char* DetDesc::Geometry::RefElement_type<TNamed>::name() const  {
  return m_element ? m_element->GetName() : "";
}

const char* DetDesc::Geometry::RefElement_type<TNamed>::refName() const  {
  return m_element ? m_element->GetName() : "";
}

void DetDesc::Geometry::RefElement_type<TNamed>::setName(const string& new_name)  {
  if ( m_element ) m_element->SetName(new_name.c_str());
}

void DetDesc::Geometry::RefElement_type<TNamed>::assign(TNamed* n, const std::string& nam, const std::string& tit) {
  m_element = n;
  if ( !nam.empty() ) m_element->SetName(nam.c_str());
  if ( !tit.empty() ) m_element->SetTitle(tit.c_str());
}

#include "TMap.h"
#include "TColor.h"

#define INSTANTIATE(X)  \
  template struct DetDesc::Geometry::Element_type<X>; \
  template struct DetDesc::Geometry::RefElement_type<X>



INSTANTIATE(TObject);

#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoElement.h"
INSTANTIATE(TGeoElement);
INSTANTIATE(TGeoMaterial);
INSTANTIATE(TGeoMedium);

#include "TGeoMatrix.h"
INSTANTIATE(TGeoMatrix);
INSTANTIATE(TGeoRotation);
INSTANTIATE(TGeoTranslation);
INSTANTIATE(TGeoIdentity);
INSTANTIATE(TGeoCombiTrans);
INSTANTIATE(TGeoGenTrans);


#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoBoolNode.h"
#include "TGeoVolume.h"
#include "TGeoCompositeShape.h"
INSTANTIATE(TGeoVolume);
INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoCompositeShape);
