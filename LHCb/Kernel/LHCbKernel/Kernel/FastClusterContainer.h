#ifndef GAUDIROOT_FASTCLUSTERCONTAINER_H
#define GAUDIROOT_FASTCLUSTERCONTAINER_H

// Framework include files
#include "GaudiKernel/DataObject.h"
// C++ include files
#include <stdexcept>
#include <vector>
#include <algorithm>

/** @class FastClusterContainer FastClusterContainer.h <dir>/FastClusterContainer.h
  *
  * Fast containers can only be used for containees not having a virtual table.
  * Though the container behaves like a vector and allows the access of objects
  * of type VISIBLE, the internal representation is of type INTERNAL.
  * This avoids unnecessary calls to the constructor of the VISIBLE provided of 
  * course that the INTERNAL type maps ideally to a primitive.
  * The size of the INTERNAL and the VISIBLE type must be identical.
  *
  * @author  M.Frank
  * @version 1.0
  *
  */
template <typename VISIBLE, typename INTERNAL> 
class FastClusterContainer : public DataObject  {
private:
  typedef typename std::vector<VISIBLE>       VD;
public:
  typedef typename VD::value_type             value_type;
  typedef typename VD::iterator               iterator;
  typedef typename VD::const_iterator         const_iterator;
  typedef typename VD::reverse_iterator       reverse_iterator;
  typedef typename VD::const_reverse_iterator const_reverse_iterator;
  typedef typename VD::size_type              size_type;
  typedef typename VD::difference_type        difference_type;
  typedef typename VD::pointer                pointer;
  typedef typename VD::const_pointer          const_pointer;
  typedef typename VD::reference              reference;
  typedef typename VD::const_reference        const_reference;

private:
  typedef std::vector<INTERNAL> InternalData;
  /// Data holder
  InternalData m_data;
  union DataRep {
    std::vector<INTERNAL>* internal;
    std::vector<VISIBLE>*  ext;
  };
  DataRep         m_r;   //! Transient

public:
  /// Standard constructor
  FastClusterContainer()  {
    m_r.internal = &m_data;
    if ( sizeof(VISIBLE) != sizeof(INTERNAL) )   {
      throw std::runtime_error("Cannot remap data of different size!");
    }
  }

  /// Standard destructor
  virtual ~FastClusterContainer() {}
  /// return iterator for beginning of mutable sequence
  iterator begin()                           { return m_r.ext->begin();       }
  /// return iterator for end of mutable sequence
  iterator end()                             { return m_r.ext->begin();       }
  /// return iterator for beginning of nonmutable sequence
  const_iterator begin() const               { return m_r.ext->begin();       }
  /// return iterator for end of nonmutable sequence
  const_iterator end() const                 { return m_r.ext->begin();       }
  /// return iterator for beginning of reversed mutable sequence
  reverse_iterator rbegin()                  { return m_r.ext->rbegin();      }
  /// return iterator for beginning of reversed nonmutable sequence
  const_reverse_iterator rbegin() const      { return m_r.ext->rbegin();      }
  /// return iterator for end of reversed mutable sequence
  reverse_iterator rend()                    { return m_r.ext->rend();        }
  /// return iterator for end of reversed nonmutable sequence
	const_reverse_iterator rend() const        { return m_r.ext->rend();        }
  /// subscript mutable sequence
  reference operator[](size_type i)          { return m_r.ext->operator[](i); }
  /// subscript nonmutable sequence
  const_reference operator[](size_type i)const{return m_r.ext->operator[](i); }
  /// subscript mutable sequence with checking
  reference at(size_type i)                  { return m_r.ext->at(i);         }
  /// subscript nonmutable sequence with checking
  const_reference at(size_type i) const      { return m_r.ext->at(i);         }
  /// test if sequence is empty
  bool empty() const                         { return m_data.empty();         }
  /// return first element of mutable sequence
  reference front()                          { return m_r.ext->front();       }
  /// return first element of nonmutable sequence
  const_reference front() const              { return m_r.ext->front();       }
  /// return last element of mutable sequence
  reference back()                           { return m_r.ext->back();        }
  /// return last element of nonmutable sequence
  const_reference back() const               { return m_r.ext->back();        }
  /// insert element at end
  void push_back(const VISIBLE& val)         { return m_r.ext->push_back(val);}
  /// erase element at end
  void pop_back()                            { return m_data.pop_back();      }

  /// Return current length of allocated storage
  size_type size() const                     { return m_data.size();          } 
  /// Return current length of allocated storage
  size_type capacity() const                 { return m_data.capacity();      } 
  /// Clear container. Note: No drestructors may be called !
  void clear()                               { return m_data.clear();         }
  /// Reserve container space
  void reserve(size_type len)                { return m_data.reserve(len);    }
  /// Resize container entries
  void resize(size_type len )                { return m_data.resize(len);     }
  /// Resize container entries
  void resize(size_type len, const VISIBLE& def )
  {  return m_data.resize(len, *(const INTERNAL*)&def);                       }

};

#endif // GAUDIROOT_FASTCLUSTERCONTAINER_H
