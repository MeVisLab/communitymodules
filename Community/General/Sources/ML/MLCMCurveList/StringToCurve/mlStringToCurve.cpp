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
// \file    mlStringToCurve.cpp
// \author  Reinhard Hameeteman
// \date    2009-03-18
//
// Convert a string into a curve
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlStringToCurve.h"

#include <boost/algorithm/string.hpp>
ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(StringToCurve, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
StringToCurve::StringToCurve () : BaseOp(0, 0)
{
  ML_TRACE_IN("StringToCurve::StringToCurve ()")

  handleNotificationOff();

  FieldContainer *fieldC = getFieldContainer();

  m_OutCurveList = new CurveList;
  f_OutCurveList = fieldC->addBase( "outCurveList" );
  f_OutCurveList->setBaseValue( m_OutCurveList );

  f_CurveString = fieldC->addString("curveString");
  f_CurveString->setStringValue("");

  f_IndexString = fieldC->addString("indexString");
  f_IndexString->setStringValue("");

  f_ValueSeparator = fieldC->addString("valueSeparator");
  f_ValueSeparator->setStringValue(" ");

  f_CurveSeparator = fieldC->addString( "curveSeparator" );
  f_CurveSeparator->setStringValue(";");

  handleNotificationOn();
}


StringToCurve::~StringToCurve()
{
  // Remove any existing curves
  while ( !m_OutCurveList->getCurveList().empty() ) {
    delete m_OutCurveList->getCurveList().back();
    m_OutCurveList->getCurveList().pop_back();
  }
  delete m_OutCurveList;
}


void StringToCurve::activateAttachments()
{
  UpdateCurve();
  f_OutCurveList->notifyAttachments();
  BaseOp::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void StringToCurve::handleNotification (Field * /*field*/)
{
  ML_TRACE_IN("StringToCurve::handleNotification ()");

  UpdateCurve();
  f_OutCurveList->notifyAttachments();
}


void StringToCurve::UpdateCurve()
{
  // Remove any existing curves
  while ( !m_OutCurveList->getCurveList().empty() ) {
    delete m_OutCurveList->getCurveList().back();
    m_OutCurveList->getCurveList().pop_back();
  }

  std::string dataString = f_CurveString->getStringValue();
  dataString.append( f_CurveSeparator->getStringValue() );

  std::string indexString = f_IndexString->getStringValue();
  std::vector< float > indexValues;
  if (indexString.length() != 0 ){
    StringToVector(indexString, indexValues );
  }

  if ( dataString.length() > 0 ){
    // Split string into curves
    std::vector<std::string> curves;
    boost::split(curves, dataString, boost::is_any_of(f_CurveSeparator->getStringValue() ));

    // Iterate over curves
    std::vector<std::string>::iterator stringIt = curves.begin();
    for (;stringIt < curves.end(); ++stringIt ){
      std::string curCurve = *stringIt;
      if (curCurve.length() == 0) {continue;}
      // Remove leading and trailing spaces
      boost::trim( curCurve );
      std::vector< MLfloat > curveValues;
      StringToVector( curCurve, curveValues );
      CurveData *outputCurve = new CurveData;
      outputCurve->setY( curveValues.size(),&curveValues[0], 1 );
      if ( f_IndexString->getStringValue().length() != 0 ){
        outputCurve->setX( indexValues.size(),&indexValues[0], 1 );
      }
      m_OutCurveList->getCurveList().push_back( outputCurve );
    }
  }
}


void StringToCurve::StringToVector(std::string valueString, std::vector< MLfloat > &valueVector )
{
  if (valueString.length() > 0){
    // Split string into values
    std::vector<std::string> values;
    boost::split(values, valueString, boost::is_any_of(f_ValueSeparator->getStringValue() ));

    // Iterate over the values
    std::vector<std::string>::iterator valueIt = values.begin();
    for (;valueIt < values.end(); ++valueIt ){
      MLfloat curValue;
      std::stringstream strm(*valueIt);
      strm >> curValue;
      valueVector.push_back( curValue );
    }
  }
}
ML_END_NAMESPACE

