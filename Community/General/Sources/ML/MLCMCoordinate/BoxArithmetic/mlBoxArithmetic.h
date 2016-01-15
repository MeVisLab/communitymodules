//-----------------------------------------------------------------------
//
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
//
//-----------------------------------------------------------------------
//! The ML module class BoxArithmetic.
/*!
// \file    mlBoxArithmetic.h
// \author  Reinhard Hameeteman
// \date    2008-10-28
//
// Perform simple calculations on a (image) box
*/
//----------------------------------------------------------------------------------


#ifndef __mlBoxArithmetic_H
#define __mlBoxArithmetic_H


// Local includes
#include "../MLCMCoordinateSystem.h"
// ML includes
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE


//! Perform simple calculations on a (image) box
class MLCMCOORDINATE_EXPORT BoxArithmetic : public BaseOp
{
public:

  //! Constructor.
  BoxArithmetic ();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field * field);

private:

  bool             m_LockUpdate;
  SubImgBoxfField *f_InBox0;
  SubImgBoxfField *f_InBox1;
  BoolField       *f_CorrectInput0;
  BoolField       *f_CorrectInput1;
  BoolField       *f_V2WInput0;
  BoolField       *f_V2WInput1;
  BoolField       *f_W2VInput0;
  BoolField       *f_W2VInput1;
  BoolField       *f_UseVoxelCenter;
  SubImgBoxfField *f_OutBox;
  SubImgBoxfField *f_OutBox0;
  SubImgBoxfField *f_OutBox1;
  BoolField       *f_Correct;
  Vec6fField      *f_OutExtent;
  Vec6fField      *f_OutV1;
  Vec6fField      *f_OutV2;
  FloatField      *f_Constant;
  Vec6fField      *f_V1;
  Vec6fField      *f_V2;
  FloatField      *f_BoxV0x,*f_BoxV0y,*f_BoxV0z,*f_BoxV0c,*f_BoxV0t,*f_BoxV0u;
  FloatField      *f_BoxV1x,*f_BoxV1y,*f_BoxV1z,*f_BoxV1c,*f_BoxV1t,*f_BoxV1u;
  EnumField       *f_OperationMode;

  enum {ExpandScalar=0, ExpandVector, AddScalar, AddVector, Intersect, Unify, LastMode };
  enum {Box0=0, Input0, Scalars0, Vectors0};
  enum {Box1=0, Input1, Scalars1, Vectors1};
  EnumField       *f_InputMode0;
  EnumField       *f_InputMode1;

  SubImgBoxf AddConstant();
  SubImgBoxf ComposeFromVectors();
  SubImgBoxf ComposeFromScalars();
  SubImgBoxf InterSectWithBox();
  SubImgBoxf UnifyWithBox();
  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(BoxArithmetic)
};


ML_END_NAMESPACE

#endif // __mlBoxArithmetic_H

