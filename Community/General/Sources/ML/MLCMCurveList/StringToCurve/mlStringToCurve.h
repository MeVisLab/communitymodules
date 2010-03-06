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
//! The ML module class StringToCurve.
/*!
// \file    mlStringToCurve.h
// \author  Reinhard Hameeteman
// \date    2009-03-18
//
// Convert a string into a curve
*/
//----------------------------------------------------------------------------------


#ifndef __mlStringToCurve_H
#define __mlStringToCurve_H


// Local includes
#include "../MLCMCurveListSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#include <mlDiagramData.h> 

ML_START_NAMESPACE


//! Convert a string into a curve
class MLCMCURVELIST_EXPORT  StringToCurve : public BaseOp
{
public:

  //! Constructor.
  StringToCurve ();

  ~StringToCurve();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field * /*field*/);
  
  //! Ensure the output is up to date on copying the module
  virtual void activateAttachments();

  //! Curve data
  BaseField *f_OutCurveList;

  //! String containing all curves
  StringField *f_CurveString;

  //! String with index values that are used for all curves
  StringField *f_IndexString;

  //! Characters that separate the curves in the curvesString
  StringField *f_ValueSeparator;

  //! Characters that separate the values in the curvesString
  StringField *f_CurveSeparator;

private:

  //! CurveList containing the output curves
  CurveList *m_OutCurveList;

  //! Actual conversion method
  void UpdateCurve();

  //! Convert a string to a vector of floats
  void StringToVector(std::string valueString, std::vector< MLfloat > &valueVector );

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(StringToCurve)
};


ML_END_NAMESPACE

#endif // __mlStringToCurve_H

