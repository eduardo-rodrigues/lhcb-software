// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiNamespace.h,v 1.7 2002-03-14 09:29:30 mato Exp $
#ifndef DADINAMESPACE_H 
#define DADINAMESPACE_H 1

// Include files
#include "DaDiClass.h"
#include "DaDiEnum.h"

#include "dom/DOMString.hpp"

#include <list>
#include <string>

/** @class DaDiNamespace DaDiNamespace.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */

class DaDiNamespace {
public:
	/// Standard constructor
	DaDiNamespace() {}; 

	virtual ~DaDiNamespace() {}; ///< Standard destructor

	DOMString name();
	void setName(DOMString value);

	DOMString author();
	void setAuthor(DOMString value);

	DOMString desc();
	void setDesc(DOMString value);
  
	DOMString longDesc();
	void setLongDesc(DOMString value);
  
	std::string popImpSoftList();
  std::list<std::string> impSoftList();
	void pushImpSoftList(std::string value);
	int sizeImpSoftList();
  void remDblImpSoftList();
  
	std::string popImpStdList();
  std::list<std::string> impStdList();
	void pushImpStdList(std::string value);
	int sizeImpStdList();
  void remDblImpStdList();
  
	std::string popImportList();
  std::list<std::string> importList();
	void pushImportList(std::string value);
	int sizeImportList();
	void remDblImportList();

	std::string popNoImports();
  std::list<std::string> noImports();
	void pushNoImports(std::string value);
	int sizeNoImports();

	DaDiClass* popDaDiClass();
	void pushDaDiClass(DaDiClass* value);
	int sizeDaDiClass();
  
	DaDiEnum* popDaDiEnum();
	void pushDaDiEnum(DaDiEnum* value);
	int sizeDaDiEnum();
  
  DaDiAttribute* popDaDiAttribute();
	void pushDaDiAttribute(DaDiAttribute* value);
	int sizeDaDiAttribute();
  
  DaDiMethod* popDaDiMethod();
	void pushDaDiMethod(DaDiMethod* value);
	int sizeDaDiMethod();
  
  DaDiTypeDef* popDaDiTypeDef();
	void pushDaDiTypeDef(DaDiTypeDef* value);
	int sizeDaDiTypeDef();
  
protected:

private:

	DOMString                 m_name,
                            m_author,
                            m_desc,
                            m_longDesc;
	std::list<std::string>    m_impSoftList,
                            m_impStdList,
                            m_importList,
                            m_noImports;
	std::list<DaDiEnum*>      m_daDiEnum;
	std::list<DaDiClass*>     m_daDiClass;
	std::list<DaDiAttribute*> m_daDiAttribute;
	std::list<DaDiMethod*>    m_daDiMethod;
	std::list<DaDiTypeDef*>   m_daDiTypeDef;
};


inline DOMString DaDiNamespace::name()
{
	return m_name;
}

inline void DaDiNamespace::setName(DOMString value)
{
	m_name = value;
}

inline DOMString DaDiNamespace::author()
{
	return m_author;
}

inline void DaDiNamespace::setAuthor(DOMString value)
{
	m_author = value;
}

inline DOMString DaDiNamespace::desc()
{
	return m_desc;
}

inline void DaDiNamespace::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiNamespace::longDesc()
{
	return m_longDesc;
}

inline void DaDiNamespace::setLongDesc(DOMString value)
{
  m_longDesc = value;
}

inline std::string DaDiNamespace::popImpSoftList()
{
	std::string pt = m_impSoftList.front();
	m_impSoftList.push_back(pt);
	m_impSoftList.pop_front();
	return pt;
}

inline std::list<std::string> DaDiNamespace::impSoftList()
{
  return m_impSoftList;
}

inline void DaDiNamespace::pushImpSoftList(std::string value)
{
  int i;
  unsigned long l;
  bool inList = false;
  for (i=0; i<sizeImportList(); ++i)
  {
    if (value == popImportList())
    {
      inList = true;
    }
  }
  for (l=0; l<m_impSoftList.size(); ++l)
  {
    if (value == popImpSoftList())
    {
      inList = true;
    }
  }
  if (!inList)
  {
	  m_impSoftList.push_back(value);
  }
}

inline int DaDiNamespace::sizeImpSoftList()
{
	return m_impSoftList.size();
}

inline void DaDiNamespace::remDblImpSoftList()
{
	m_impSoftList.sort();
	m_impSoftList.unique();
}

inline std::string DaDiNamespace::popImpStdList()
{
	std::string pt = m_impStdList.front();
	m_impStdList.push_back(pt);
	m_impStdList.pop_front();
	return pt;
}

inline std::list<std::string> DaDiNamespace::impStdList()
{
  return m_impStdList;
}

inline void DaDiNamespace::pushImpStdList(std::string value)
{
  bool inList = false;
  for(unsigned long i=0; i<m_impStdList.size(); ++i)
  {
    if (value == popImpStdList())
    {
      inList = true;
    }
  }
  if (!inList)
  {
	  m_impStdList.push_back(value);
  }
}

inline int DaDiNamespace::sizeImpStdList()
{
	return m_impStdList.size();
}

inline void DaDiNamespace::remDblImpStdList()
{
	m_impStdList.sort();
	m_impStdList.unique();
}

inline std::string DaDiNamespace::popImportList()
{
	std::string pt = m_importList.front();
	m_importList.push_back(pt);
	m_importList.pop_front();
	return pt;
}

inline std::list<std::string> DaDiNamespace::importList()
{
  return m_importList;
}

inline void DaDiNamespace::pushImportList(std::string value)
{
  unsigned long l;
  int i, lastspace;
	std::string import, lastword;

	while (value != "")
	{
		i = value.find_first_of(":,<>");

		if (i == -1)
		{
			import = value;
			value = "";
		}
		else
		{
			import = value.substr(0,i);
			value = value.substr(i+1,std::string::npos);
		}

    while ((import[import.size()-1] == '*') ||
      (import[import.size()-1] == '&'))
    {
      import = import.substr(0,import.size()-1);
    }

    lastspace = import.find_last_of(" ");
    lastword = import.substr(lastspace+1, import.size()-lastspace);
    if ((lastword != "bool")   && (lastword != "short")    &&
        (lastword != "long")   && (lastword != "int")      && 
        (lastword != "float")  && (lastword != "double")   &&
        (lastword != "char")   && (lastword != "unsigned") &&
        (lastword != "signed") && (lastword != "")         &&
        (lastword != "std")    && (lastword != "string"))
		{
			if ((import == "vector") || (import == "list")   ||
				(import == "deque")    || (import == "queue")  ||
				(import == "stack")    || (import == "map")    ||
				(import == "set")      || (import == "bitset") ||
        (import == "pair"))
			{
				pushImpStdList(import);
			}
			else
			{
        for (l=0; l<m_impSoftList.size(); ++l)
        {
          if (import == m_impSoftList.front())
          {
            m_impSoftList.pop_front();
          }
        }
        bool inList = false;
        for (l=0; l<m_importList.size(); ++l)
        {
          if (import == popImportList())
          {
            inList = true;
          }
        }
        if (!inList)
        {
				  m_importList.push_back(import);
        }
			}
		}
	}
}

inline int DaDiNamespace::sizeImportList()
{
	return m_importList.size();
}

inline void DaDiNamespace::remDblImportList()
{
	m_importList.sort();
	m_importList.unique();
}

inline std::string DaDiNamespace::popNoImports()
{
	std::string pt = m_noImports.front();
	m_noImports.push_back(pt);
	m_noImports.pop_front();
	return pt;
}

inline std::list<std::string> DaDiNamespace::noImports()
{
  return m_noImports;
}

inline void DaDiNamespace::pushNoImports(std::string value)
{
  m_noImports.push_back(value);
}

inline int DaDiNamespace::sizeNoImports()
{
	return m_noImports.size();
}

inline DaDiEnum* DaDiNamespace::popDaDiEnum()
{
	DaDiEnum* pt = m_daDiEnum.front();
	m_daDiEnum.push_back(pt);
	m_daDiEnum.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiEnum(DaDiEnum* value)
{
	m_daDiEnum.push_back(value);
}

inline int DaDiNamespace::sizeDaDiEnum()
{
	return m_daDiEnum.size();
}

inline DaDiClass* DaDiNamespace::popDaDiClass()
{
	DaDiClass* pt = m_daDiClass.front();
	m_daDiClass.push_back(pt);
	m_daDiClass.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiClass(DaDiClass* value)
{
	m_daDiClass.push_back(value);
}

inline int DaDiNamespace::sizeDaDiClass()
{
	return m_daDiClass.size();
}

inline DaDiAttribute* DaDiNamespace::popDaDiAttribute()
{
	DaDiAttribute* pt = m_daDiAttribute.front();
	m_daDiAttribute.push_back(pt);
	m_daDiAttribute.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiAttribute(DaDiAttribute* value)
{
	m_daDiAttribute.push_back(value);
}

inline int DaDiNamespace::sizeDaDiAttribute()
{
	return m_daDiAttribute.size();
}

inline DaDiMethod* DaDiNamespace::popDaDiMethod()
{
	DaDiMethod* pt = m_daDiMethod.front();
	m_daDiMethod.push_back(pt);
	m_daDiMethod.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiMethod(DaDiMethod* value)
{
	m_daDiMethod.push_back(value);
}

inline int DaDiNamespace::sizeDaDiMethod()
{
	return m_daDiMethod.size();
}

inline DaDiTypeDef* DaDiNamespace::popDaDiTypeDef()
{
	DaDiTypeDef* pt = m_daDiTypeDef.front();
	m_daDiTypeDef.push_back(pt);
	m_daDiTypeDef.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiTypeDef(DaDiTypeDef* value)
{
	m_daDiTypeDef.push_back(value);
}

inline int DaDiNamespace::sizeDaDiTypeDef()
{
	return m_daDiTypeDef.size();
}


#endif // DADINAMESPACE_H

