// $Id: BindType2ClonerDef.h,v 1.3 2010-08-11 13:37:52 jpalac Exp $
#ifndef MICRODST_BINDTYPE2CLONERDEF_H 
#define MICRODST_BINDTYPE2CLONERDEF_H 1

// Include files

/** @class BindType2Cloner BindType2ClonerDef.h MicroDST/BindType2ClonerDef.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-09-01
 */
template <typename T> struct BindType2Cloner 
{
  typedef T Type;
  typedef MicroDST::BasicCopy<T> Cloner;
};

#endif // MICRODST_BINDTYPE2CLONERDEF_H
