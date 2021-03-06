// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/lcdd/LCDD.h"

// ROOT include files
#include "TColor.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <stdexcept>
#include <strstream>

using namespace std;
using namespace DetDesc::Geometry;

namespace DetDesc  { namespace Geometry  {

  template <> struct Value<TGeoNodeMatrix,PlacedVolume::Object> 
    : public TGeoNodeMatrix, public PlacedVolume::Object  
  {
    Value(const TGeoVolume* v, const TGeoMatrix* m) : TGeoNodeMatrix(v,m), PlacedVolume::Object() {
      magic = magic_word();
    }
  };

  template <> struct Value<TGeoVolume,Volume::Object> 
    : public TGeoVolume, public Volume::Object  
  {
    Value(const char* name, TGeoShape* s=0, TGeoMedium* m=0) : TGeoVolume(name,s,m) {
      magic = magic_word();
    }
    virtual ~Value() {}
    virtual void AddNode(const TGeoVolume *vol, Int_t copy_no, TGeoMatrix *mat, Option_t* = "") {
      TGeoMatrix *matrix = mat;
      if (matrix==0) matrix = gGeoIdentity;
      else           matrix->RegisterYourself();
      if (!vol) {
	Error("AddNode", "Volume is NULL");
	return;
      }
      if (!vol->IsValid()) {
	Error("AddNode", "Won't add node with invalid shape");
	printf("### invalid volume was : %s\n", vol->GetName());
	return;
      }   
      if (!fNodes) fNodes = new TObjArray();   

      if (fFinder) {
	// volume already divided.
	Error("AddNode", "Cannot add node %s_%i into divided volume %s", vol->GetName(), copy_no, GetName());
	return;
      }

      TGeoNodeMatrix *node = new Value<TGeoNodeMatrix,PlacedVolume::Object>(vol, matrix);
      //node = new TGeoNodeMatrix(vol, matrix);
      node->SetMotherVolume(this);
      fNodes->Add(node);
      TString name = TString::Format("%s_%d", vol->GetName(), copy_no);
      if (fNodes->FindObject(name))
	Warning("AddNode", "Volume %s : added node %s with same name", GetName(), name.Data());
      node->SetName(name);
      node->SetNumber(copy_no);
    }
  };
}}


Volume::Volume(LCDD& lcdd, const string& name)    {
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str());
  lcdd.addVolume(*this);
}

Volume::Volume(LCDD& lcdd, const string& name, const Solid& s, const Material& m) {
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str(),s);
  setMaterial(m);
  lcdd.addVolume(*this);
}

void Volume::setMaterial(const Material& m)  const  {
  if ( m.isValid() )   {
    TGeoMedium* medium = m._ptr<TGeoMedium>();
    if ( medium )  {
      m_element->SetMedium(medium);
      return;
    }
    throw runtime_error("Volume: Medium "+string(m.name())+" is not registered with geometry manager.");
  }
  throw runtime_error("Volume: Attempt to assign invalid material.");
}

void Volume::setSolid(const Solid& solid)  const  {
  m_element->SetShape(solid);
}

static PlacedVolume _addNode(TGeoVolume* par, TGeoVolume* daughter, TGeoMatrix* transform) {
  Value<TGeoVolume,Volume::Object>* parent = (Value<TGeoVolume,Volume::Object>*)par;
  TObjArray* a = parent->GetNodes();
  Int_t id = a ? a->GetEntries() : 0;
  parent->AddNode(daughter,id,transform);
  TGeoNodeMatrix* n = dynamic_cast<TGeoNodeMatrix*>(parent->GetNode(id));
  return PlacedVolume(n);
}

PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoRotation rotation("",rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
    TGeoCombiTrans* transform = new TGeoCombiTrans(nam.c_str(),pos.x,pos.y,pos.z,0);
    transform->SetRotation(rotation);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoTranslation* transform = new TGeoTranslation(nam.c_str(),pos.x,pos.y,pos.z);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoRotation* transform = new TGeoRotation(nam.c_str(),rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityPos& /* pos */)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    return _addNode(m_element,volume,new TGeoIdentity(nam.c_str()));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityRot& /* rot */)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    return _addNode(m_element,volume,new TGeoIdentity(nam.c_str()));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::setRegion(const Region& obj)  const   {
  data<Object>()->region = obj;
}

void Volume::setLimitSet(const LimitSet& obj)  const   {
  data<Object>()->limits = obj;
}

void Volume::setVisAttributes(const VisAttr& attr) const   {
  if ( attr.isValid() )  {
    VisAttr::Object* vis = attr.data<VisAttr::Object>();
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    m_element->SetFillColor(bright);
    m_element->SetLineColor(dark);
    m_element->SetLineStyle(vis->lineStyle);
    m_element->SetLineWidth(10);
    m_element->SetVisibility(vis->visible ? kTRUE : kFALSE);
    m_element->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
  data<Object>()->vis = attr;
}

void Volume::setVisAttributes(const LCDD& lcdd, const string& name)  const {
  if ( !name.empty() )   {
    VisAttr attr = lcdd.visAttributes(name);
    data<Object>()->vis = attr;
    setVisAttributes(attr);
  }
  else  {
    /*
    string tag = this->name();
    if ( ::strstr(tag.c_str(),"_slice") )       // Slices turned off by default
      setVisAttributes(lcdd.visAttributes("InvisibleNoDaughters"));
    else if ( ::strstr(tag.c_str(),"_layer") )  // Layers turned off, but daughters possibly visible
      setVisAttributes(lcdd.visAttributes("InvisibleWithDaughters"));
    else if ( ::strstr(tag.c_str(),"_module") ) // Tracker modules similar to layers
      setVisAttributes(lcdd.visAttributes("InvisibleWithDaughters"));
    else if ( ::strstr(tag.c_str(),"_module_component") ) // Tracker modules similar to layers
      setVisAttributes(lcdd.visAttributes("InvisibleNoDaughters"));
    */
  }
}

/// Attach attributes to the volume
void Volume::setAttributes(const LCDD& lcdd,
			   const string& region, 
			   const string& limits, 
			   const string& vis)   const
{
  if ( !region.empty() ) setRegion(lcdd.region(region));
  if ( !limits.empty() ) setLimitSet(lcdd.limitSet(limits));
  setVisAttributes(lcdd,vis);
}

void Volume::setSensitiveDetector(const SensitiveDetector& obj) const  {
  data<Object>()->sens_det = obj;
}

Solid Volume::solid() const   {
  return Solid((*this)->GetShape());
}

Material Volume::material() const   {
  return Handle<TGeoMaterial>(m_element->GetMaterial());
}

VisAttr Volume::visAttributes() const   {
  return data<Object>()->vis;
}

Ref_t Volume::sensitiveDetector() const    {
  return data<Object>()->sens_det;
}

Region Volume::region() const   {
  return data<Object>()->region;
}

LimitSet Volume::limitSet() const   {
  return data<Object>()->limits;
}

PlacedVolume& PlacedVolume::addPhysVolID(const std::string& name, int value) {
  Object* obj = data<Object>();
  obj->volIDs[name] = value;
  return *this;
}

/// Volume material
Material PlacedVolume::material() const {
  return Material::handle_t(m_element ? m_element->GetMedium()->GetMaterial() : 0);
}

/// Logical volume of this placement
Volume   PlacedVolume::volume() const {
  return Volume::handle_t(m_element ? m_element->GetVolume() : 0);
}

/// Parent volume (envelope)
Volume PlacedVolume::motherVol() const {
  return Volume::handle_t(m_element ? m_element->GetMotherVolume() : 0);
}

/// Access to the volume IDs
const PlacedVolume::VolIDs& PlacedVolume::volIDs() const {
  return data<Object>()->volIDs;
}

/// Set the detector handle
void PlacedVolume::setDetElement(Ref_t detector)   const {
  data<Object>()->detector = detector;
}

/// Access to the corresponding detector element (maybe invalid)
Ref_t PlacedVolume::detElement() const {
  return data<Object>()->detector;
}

/// String dump
string PlacedVolume::toString() const {
  strstream s;
  Object* obj = data<Object>();
  s << m_element->GetName() << ":  vol='" << m_element->GetVolume()->GetName()
    << "' mat:'" << m_element->GetMatrix()->GetName() << "' volID[" << obj->volIDs.size() << "] ";
  for(VolIDs::const_iterator i=obj->volIDs.begin(); i!=obj->volIDs.end();++i)
    s << (*i).first << "=" << (*i).second << "  ";
  s << ends;
  return s.str();
}
