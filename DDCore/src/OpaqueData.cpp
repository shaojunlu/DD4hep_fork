// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/OpaqueData.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/OpaqueData_inl.h"

// C/C++ header files
#include <cstring>

using namespace std;
using namespace DD4hep;

/// Standard initializing constructor
OpaqueData::OpaqueData() : grammar(0), pointer(0)   {
}

/// Copy constructor
OpaqueData::OpaqueData(const OpaqueData& c) : grammar(c.grammar), pointer(c.pointer) {
}

/// Standard Destructor
OpaqueData::~OpaqueData()  {
}

/// Assignment operator
OpaqueData& OpaqueData::operator=(const OpaqueData& c) {
  if ( &c != this )  {
    grammar = c.grammar;
    pointer = c.pointer;
  }
  return *this;
}

/// Create data block from string representation
bool OpaqueData::fromString(const string& rep)   {
  if ( pointer && grammar )  {
    return grammar->fromString(pointer,rep);
  }
  throw runtime_error("Opaque data block is unbound. Cannot parse string representation.");
}

/// Create string representation of the data block
string OpaqueData::str()  const  {
  if ( pointer && grammar )  {
    return grammar->str(pointer);
  }
  throw runtime_error("Opaque data block is unbound. Cannot create string representation.");
}

/// Access type id of the condition
const type_info& OpaqueData::typeInfo() const  {
  if ( pointer && grammar ) {
    return grammar->type();
  }
  throw runtime_error("Opaque data block is unbound. Cannot determine type information!");
}

/// Access type name of the condition data block
const string& OpaqueData::dataType() const   {
  if ( pointer && grammar ) {
    return grammar->type_name();
  }
  throw runtime_error("Opaque data block is unbound. Cannot determine type information!"); 
}

/// Standard initializing constructor
OpaqueDataBlock::OpaqueDataBlock() : OpaqueData(), destruct(0), copy(0), type(0)   {
  InstanceCount::increment(this);
}

/// Copy constructor
OpaqueDataBlock::OpaqueDataBlock(const OpaqueDataBlock& c) 
  : OpaqueData(c), destruct(c.destruct), copy(c.copy), type(c.type)   {
  grammar = 0;
  pointer = 0;
  this->bind(c.grammar,c.copy,c.destruct);
  this->copy(pointer,c.pointer);
  InstanceCount::increment(this);
}

/// Standard Destructor
OpaqueDataBlock::~OpaqueDataBlock()   {
  if ( destruct )  {
    (*destruct)(pointer);
    if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
  }
  pointer = 0;
  grammar = 0;
  InstanceCount::decrement(this);
}

/// Move the data content: 'from' will be reset to NULL
bool OpaqueDataBlock::move(OpaqueDataBlock& from)   {
  pointer = from.pointer;
  grammar = from.grammar;
  ::memcpy(data,from.data,sizeof(data));
  destruct = from.destruct;
  copy = from.copy;
  type = from.type;
  ::memset(from.data,0,sizeof(data));
  from.type = PLAIN_DATA;
  from.destruct = 0;
  from.copy = 0;
  from.pointer = 0;
  from.grammar = 0;
  return true;
}

/// Copy constructor
OpaqueDataBlock& OpaqueDataBlock::operator=(const OpaqueDataBlock& c)   {
  if ( this != &c )  {
    if ( this->grammar == c.grammar )   {
      if ( destruct )  {
        (*destruct)(pointer);
        if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
      }
      pointer = 0;
      grammar = 0;
    }
    if ( this->grammar == 0 )  {
      this->OpaqueData::operator=(c);
      this->destruct = c.destruct;
      this->copy = c.copy;
      this->type = c.type;
      this->grammar = 0;
      this->bind(c.grammar,c.copy,c.destruct);
      this->copy(pointer,c.pointer);
      return *this;
    }
    except("OpaqueData","You may not bind opaque data multiple times!");
  }
  return *this;
}

/// Set data value
bool OpaqueDataBlock::bind(const BasicGrammar* g, void (*ctor)(void*,const void*), void (*dtor)(void*))   {
  if ( !grammar )  {
    size_t len = g->sizeOf();
    grammar  = g;
    destruct = dtor;
    copy     = ctor;
    (len > sizeof(data))
      ? (pointer=::operator new(len),type=ALLOC_DATA)
      : (pointer=data,type=PLAIN_DATA);
    return true;
  }
  else if ( grammar == g )  {
    // We cannot ingore secondary requests for data bindings.
    // This leads to memory leaks in the caller!
    except("OpaqueData","You may not bind opaque multiple times!");
  }
  typeinfoCheck(grammar->type(),g->type(),"Opaque data blocks may not be assigned.");
  return false;
}

/// Set data value
void OpaqueDataBlock::assign(const void* ptr, const type_info& typ)  {
  if ( !grammar )   {
    except("OpaqueData","Opaque data block is unbound. Cannot copy data.");
  }
  else if ( grammar->type() != typ )  {
    except("OpaqueData","Bad data binding binding");
  }
  (*copy)(pointer,ptr);
}
