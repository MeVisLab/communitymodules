// Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
// Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of BIGR nor the names of its contributors 
//       may be used to endorse or promote products derived from this software 
//       without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------------
//! The ML module class SyncFields.
/*!
// \file    mlSyncFields.h
// \author  Reinhard Hameeteman
// \date    2008-11-04
//
// Sync float or vector depending on difference criterium
*/
//----------------------------------------------------------------------------------


#ifndef __mlSyncFields_H
#define __mlSyncFields_H


// Local includes
#include "../MLCMFieldsSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE

#define NFIELDS 10

//! Sync float or vector depending on difference criterium
class MLCMFIELDS_EXPORT SyncFields : public BaseOp
{
public:

  //! Constructor.
  SyncFields ();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

  DoubleField *f_Precision;
  EnumField  *f_Mode;
  BoolField  *f_On;
  std::vector< DoubleField* > m_FloatsFields;
  std::vector< Vec6fField* > m_Vec6Fields;


private:

  template< typename T > bool DiffTest( const T first, const T last, const double precision );
  bool DiffTest( const double first, const double last, const double precision );
  bool m_Updating;

  enum {FLOAT=0, MULTIFLOAT, VECTOR, MULTIVECTOR };
  
  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(SyncFields)
};


ML_END_NAMESPACE

#endif // __mlSyncFields_H

