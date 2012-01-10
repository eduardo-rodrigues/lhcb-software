#ifndef DETDESC_ELEMENTINTERNALS_H
#define DETDESC_ELEMENTINTERNALS_H
#include <TNamed.h>
#include <algorithm>

namespace DetDesc  {
  class IDDescriptor;
  namespace Geometry  {
    typedef DetDesc::IDDescriptor IDDescriptor;
    template<typename Q, class P> struct Value : public Q, public P  {};
    template <class T, class Q> typename T* first_value(const Q& e)  {
      return e.handle()._ptr<Value<T,Q::Object> >();
    }
    template <typename T, typename Q> typename Q::Object* second_value(const Q& e)  {
      return e.handle()._ptr<Value<T,Q::Object> >();
    }
    template <typename T, typename Q> typename T* value(const Q& e)  {
      return e.handle()._ptr<T>();
    }
  }
}
#define getAttr(y)  m_element._ptr<Value<TNamed,Object> >()->y
#define setAttr(y,value) getAttr(y) = value

// Conversion factor from radians to degree: 360/(2*PI)
#define RAD_2_DEGREE 57.295779513082320876798154814105

#endif // DETDESC_ELEMENTINTERNALS_H
