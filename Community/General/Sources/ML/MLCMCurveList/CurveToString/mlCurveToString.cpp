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
//! The ML module class CurveToString.
/*!
// \file    mlCurveToString.cpp
// \author  Reinhard Hameeteman
// \date    2011-06-06
//
// Convert a curve into a string
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlCurveToString.h"

#include <boost/algorithm/string.hpp>
ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(CurveToString, Module);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CurveToString::CurveToString () : Module(0, 0)
{
  ML_TRACE_IN("CurveToString::CurveToString ()")

  handleNotificationOff();

  FieldContainer *fieldC = getFieldContainer();

  f_InCurveList = fieldC->addBase( "inCurveList" );

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


//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void CurveToString::handleNotification (Field * field)
{
  ML_TRACE_IN("CurveToString::handleNotification ()");
  if ( field == f_InCurveList ) {
    m_SingleCurveList.clear();
    m_InCurveList = NULL;
    Base* baseInput = f_InCurveList->getBaseValue();
    CurveData* inCurve = mlbase_cast<CurveData*>( baseInput );
    if (inCurve){
      m_SingleCurveList.getCurveList().push_back( inCurve );
      m_InCurveList = &m_SingleCurveList;
    } else {
      m_InCurveList = mlbase_cast<CurveList*>(baseInput);
    }
  }
  if ( m_InCurveList ){
    UpdateString();
  }
}


void CurveToString::UpdateString()
{

  std::stringstream curveString;
  std::stringstream indexString;
  std::string vSep = f_ValueSeparator->getStringValue();
  std::string cSep = f_CurveSeparator->getStringValue();
  size_t nCurveSets = m_InCurveList->getNumCurves();
  if ( nCurveSets == 0 ) {return;}

  for (int iSet = 0; iSet < nCurveSets; ++iSet ) {

    size_t nCurves = m_InCurveList->getCurveData( iSet )->getNumSeries();
    if (nCurves == 0 ) {continue;}

    for (int iCurve = 0; iCurve < nCurves; ++iCurve ) {

      CurveData *curveSet = m_InCurveList->getCurveData( iSet );

      for (int iX = 0; iX < curveSet->getPoints(); ++iX ){
        const double yValue = curveSet->getYValue( iCurve,iX );
        curveString << yValue << vSep;
        if ( iCurve == 0 ){
          const double xValue = curveSet->getXValue( iX );
          indexString << xValue << vSep;
        }
      }
      char dummy;
      curveString >> dummy;
      curveString << cSep;
    }
  }
  f_CurveString->setStringValue( curveString.str() );
  char dummy;
  indexString >> dummy;
  f_IndexString->setStringValue( indexString.str() );
}

ML_END_NAMESPACE

