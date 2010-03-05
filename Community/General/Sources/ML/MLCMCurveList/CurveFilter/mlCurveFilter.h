//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
//! The ML module class CurveFilter.
/*!
// \file    mlCurveFilter.h
// \author  Reinhard Hameeteman
// \date    2008-03-03
//
// Select a curve from the curveList or curveSet
*/
//----------------------------------------------------------------------------------


#ifndef __mlCurveFilter_H
#define __mlCurveFilter_H


// Local includes
#include "../MLCMCurveListSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <mlDiagramData.h> 

ML_START_NAMESPACE


//! Select a curve from the curveList or curveSet
class MLCMCURVELIST_EXPORT CurveFilter : public BaseOp
{
public:

  //! Constructor.
  CurveFilter (void);

  //! Destructor
  ~CurveFilter(void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  void SetOutputCurve();


private:


  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(CurveFilter);

  //! Remove any existing output curves
  void ResetOutput();

  //! Set the maximum values allowed for curves and series.
  void SetMaxValues();

  //! Input curve data
  CurveList *m_InCurveList;
  BaseField *f_InCurveList;
  CurveList  m_SingleCurveList;

  //! Output curve data
  CurveList *m_OutCurveList;
  BaseField *f_OutCurveList;

  //! Update modes
  NotifyField *f_Update;
  BoolField   *f_AutoUpdate;

  //! Curve set range
  IntField  *f_MinCurveSet;
  IntField  *f_MaxCurveSet;

  //! Curve series range
  IntField  *f_MinCurve;
  IntField  *f_MaxCurve;

  //! Number of curves present in input
  IntField  *f_NumberOfCurveSets;

  //! Number of series present in input
  IntField  *f_NumberOfCurves;

  //! Crop the curve?
  BoolField  *f_CropCurve;
  FloatField *f_XStart;
  FloatField *f_XEnd;

};


ML_END_NAMESPACE

#endif // __mlCurveFilter_H


