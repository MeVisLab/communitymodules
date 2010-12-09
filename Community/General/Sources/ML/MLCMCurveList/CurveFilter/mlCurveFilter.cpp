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
// \file    mlCurveFilter.cpp
// \author  Reinhard Hameeteman
// \date    2008-03-03
//
// Select a curve from the curveList or curveSet
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlCurveFilter.h"
ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(CurveFilter, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CurveFilter::CurveFilter (void)
  : BaseOp(0, 0), m_InCurveList(0)
{
  ML_TRACE_IN("CurveFilter::CurveFilter()");

  handleNotificationOff();

  FieldContainer *fields = getFieldContainer();

  m_InCurveList = NULL;
  f_InCurveList = fields->addBase( "inCurveList" );
  f_InCurveList->setBaseValue( m_InCurveList );

  m_OutCurveList = new CurveList;
  f_OutCurveList = fields->addBase( "outCurveList" );
  f_OutCurveList->setBaseValue( m_OutCurveList );

  f_Update = fields->addNotify( "update" );
  f_AutoUpdate = fields->addBool( "autoUpdate" );
  f_AutoUpdate->setBoolValue( false );

  f_MinCurveSet = fields->addInt( "minCurveSet" );
  f_MinCurveSet->setIntValue( 0 );

  f_MaxCurveSet = fields->addInt( "maxCurveSet" );
  f_MaxCurveSet->setIntValue( 0 );

  f_NumberOfCurveSets = fields->addInt( "numberOfCurveSets" );
  f_NumberOfCurveSets->setIntValue( 0 );

  f_MinCurve = fields->addInt("minCurve");
  f_MinCurve->setIntValue( 0 );

  f_MaxCurve = fields->addInt("maxCurve");
  f_MaxCurve->setIntValue( 0 );

  f_NumberOfCurves = fields->addInt( "numberOfCurves" );
  f_NumberOfCurves->setIntValue( 0 );

  f_CropCurve = fields->addBool( "cropCurve" );
  f_CropCurve->setBoolValue( false );

  f_XStart = fields->addFloat( "start" );
  f_XStart->setFloatValue( 0.0f );

  f_XEnd = fields->addFloat( "end" );
  f_XEnd->setFloatValue( 0.0f );

  handleNotificationOn();
}


CurveFilter::~CurveFilter(void)
{
  ResetOutput();
  delete m_OutCurveList;
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void CurveFilter::handleNotification (Field *field)
{
  ML_TRACE_IN("CurveFilter::handleNotification()");

  if ( field == f_InCurveList ) {
    m_SingleCurveList.clear();
    m_InCurveList = NULL;
    Base* baseInput = f_InCurveList->getBaseValue();
    m_InCurveList = mlbase_cast<CurveList*>(baseInput);
    CurveData* inCurve = mlbase_cast<CurveData*>( baseInput );
    if (inCurve){
      m_SingleCurveList.getCurveList().push_back( static_cast<CurveData*>( baseInput ) );
      m_InCurveList = &m_SingleCurveList;
    }

    if ( m_InCurveList != NULL ) {
      if ( f_AutoUpdate->getBoolValue() ){
        SetMaxValues();
        SetOutputCurve(); 
        f_OutCurveList->notifyAttachments();
      }
    } else {
      ResetOutput();
      f_OutCurveList->notifyAttachments();
    }

  } else 
    if (  field == f_Update       || 
         (f_AutoUpdate->getBoolValue()  &&
         (field == f_AutoUpdate   || 
          field == f_MinCurveSet  || 
          field == f_MaxCurveSet  ||
          field == f_MinCurve     || 
          field == f_MaxCurve     || 
          field == f_CropCurve    ||
          (f_CropCurve->getBoolValue() && ( field == f_XStart || field == f_XEnd ))
          ))){
      if ( m_InCurveList != NULL ){
        if ( m_InCurveList->getNumCurves() > 0 ){
          SetOutputCurve();
          f_OutCurveList->notifyAttachments();
        }
      }
    }
}


void CurveFilter::ResetOutput()
{
  while ( !m_OutCurveList->getCurveList().empty() ) {
    delete m_OutCurveList->getCurveList().back();
    m_OutCurveList->getCurveList().pop_back();
  }
}


void CurveFilter::SetMaxValues()
{
  int nCurveSets = 0;
  int nCurves = 0;
  if ( m_InCurveList != NULL ){
    nCurveSets = m_InCurveList->getNumCurves();
    for (int iSet = 0; iSet < nCurveSets; ++iSet ) {
      nCurves = mlMax( static_cast<MLssize_t>(nCurves), m_InCurveList->getCurveData( iSet )->getNumSeries() );
    }
  }
  f_NumberOfCurveSets->setIntValue( nCurveSets-1 );
  f_NumberOfCurves->setIntValue( nCurves-1 );
}


void CurveFilter::SetOutputCurve(){

  ResetOutput();

  // Check for valid field values
  // Curves are numbered from 0
  int nCurveSets = m_InCurveList->getNumCurves();
  if ( nCurveSets == 0 ) {return;}
  int minSet = mlMax( static_cast<MLint>(0), f_MinCurveSet->getIntValue() );
  int maxSet = mlMax( static_cast<MLint>(minSet), f_MaxCurveSet->getIntValue() );
  maxSet = mlMin( maxSet, nCurveSets-1 );
  for (int iSet = minSet; iSet <= maxSet; ++iSet ) {

    int nCurves = m_InCurveList->getCurveData( iSet )->getNumSeries();
    if (nCurves == 0 ) {continue;}
    int minCurve = mlMax(static_cast<MLint>(0),f_MinCurve->getIntValue() );
    int maxCurve = mlMax(static_cast<MLint>(minCurve), f_MaxCurve->getIntValue() );
    maxCurve = mlMin(maxCurve, nCurves-1 );
    for (int iCurve = minCurve; iCurve <= maxCurve; ++iCurve ) {
      
      CurveData *curveSet = m_InCurveList->getCurveData( iSet );
      CurveData *outputCurve = new CurveData( *curveSet );
      outputCurve->clearData();
      std::vector< float > xSeries;
      std::vector< float > ySeries;

      // Copy/Crop curve values
      bool crop = f_CropCurve->getBoolValue();
      float minX = f_XStart->getFloatValue();
      float maxX = f_XEnd->getFloatValue();
      minX = mlMin(minX,maxX);
      for (int iX = 0; iX < curveSet->getPoints(); ++iX ){
        float xValue = curveSet->getXValue( iX );
        if ( !crop || ( (minX<=xValue) && (xValue <= maxX)) ) {
          ySeries.push_back( curveSet->getYValue( minCurve, iX) );
          xSeries.push_back( xValue );
        }
      }
      outputCurve->setX( xSeries.size(),&xSeries[0], 1 );
      outputCurve->setY( ySeries.size(),&ySeries[0], 1 );

      m_OutCurveList->getCurveList().push_back( outputCurve );
    }
  }
}


ML_END_NAMESPACE

