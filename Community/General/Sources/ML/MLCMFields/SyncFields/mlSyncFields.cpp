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
// \file    mlSyncFields.cpp
// \author  Reinhard Hameeteman
// \date    2008-11-04
//
// Sync float or vector depending on difference criterium
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlSyncFields.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(SyncFields, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
SyncFields::SyncFields ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("SyncFields::SyncFields ()");

  handleNotificationOff();
  FieldContainer *fieldC = getFieldContainer();

  const char* syncModes[] = {"Float","MultiFloat","Vector","MultiVector"};
  f_Mode = fieldC->addEnum("mode",syncModes,4);
  f_Mode->setEnumValue( FLOAT );

  f_On = fieldC->addBool("on");
  f_On->setBoolValue( true );

  f_Precision = fieldC->addDouble("precision");
  f_Precision->setDoubleValue( 1e-3f );

  for (unsigned int iFloat=0; iFloat < NFIELDS; ++iFloat ){
    std::stringstream ss;
    ss << iFloat;
    std::string currentName = "float" + ss.str();
    m_FloatsFields.push_back(fieldC->addDouble(currentName.c_str()) );
    m_FloatsFields[iFloat]->setDoubleValue( 0.0f );
  }

  for (unsigned int iVector =0; iVector < NFIELDS; ++iVector ){
    std::stringstream ss;
    ss << iVector;
    std::string currentName = "vector" + ss.str();
    m_Vec6Fields.push_back( fieldC->addVec6f(currentName.c_str()) );
    m_Vec6Fields[iVector]->setVec6fValue( vec6() );
  }

  m_Updating = false;
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void SyncFields::handleNotification (Field *field)
{
  ML_TRACE_IN("SyncFields::handleNotification ()");

  if (  m_Updating ||!f_On->getBoolValue() ) {
      return;
  }

  if ( field == f_Mode || field == f_On || field == f_Precision ) {
    switch ( f_Mode->getEnumValue() ){
      case FLOAT:
      case MULTIFLOAT:
        this->handleNotification( m_FloatsFields[0] );
        break;
      case VECTOR:
      case MULTIVECTOR:
        this->handleNotification( m_Vec6Fields[0] );
        break;
      default:
        break;
    }
  } else {

    m_Updating = true;

    mlDebug( "Handling notification from: " << field->getName() );

    unsigned int nFields = NFIELDS;

    switch ( f_Mode->getEnumValue() ){
      case FLOAT :
        nFields = 2;
      case MULTIFLOAT:
        {
          std::vector< bool > changed(nFields,false);
          const double precision = f_Precision->getDoubleValue();
          double newValue = 0.0;
          // determine the changed field
          for (unsigned int iFloat = 0; iFloat < nFields; ++iFloat){
            if ( field == m_FloatsFields[iFloat] ){
              changed[iFloat]= true;
              newValue = m_FloatsFields[iFloat]->getDoubleValue();
            } else {
              changed[iFloat] = false;
            }
          }

          // Prevent notifications while updating all values
          handleNotificationOff();
          // updated fields if necessary
          for (unsigned int iFloat = 0; iFloat < nFields; ++iFloat){
            if ( !changed[iFloat] && DiffTest(newValue, m_FloatsFields[iFloat]->getDoubleValue(), precision ) ){
              m_FloatsFields[iFloat]->setDoubleValue( newValue );
              changed[iFloat] = true;
            }
          }
          handleNotificationOn();

          for (unsigned int iFloat = 0; iFloat < nFields; ++iFloat){
            if ( changed[iFloat] ){
              m_FloatsFields[iFloat]->notifyAttachments();
            }
          }
          break;
        }
      case VECTOR:
        nFields = 2;
      case MULTIVECTOR:
        {
          std::vector< bool > changed(nFields,false);
          const double precision = f_Precision->getDoubleValue();
          vec6 newValue;
          for (unsigned int iVector = 0; iVector < nFields; ++iVector){
            if ( field == m_Vec6Fields[iVector] ){
              changed[iVector]= true;
              newValue = m_Vec6Fields[iVector]->getVec6fValue();
            } else {
              changed[iVector] = false;
            }
          }
          // Prevent notifications while updating all values
          handleNotificationOff();
          // Update fields if necessary
          for (unsigned int iVector = 0; iVector < nFields; ++iVector){
            if ( !changed[iVector] && DiffTest(newValue, m_Vec6Fields[iVector]->getVec6fValue(), precision ) ){
              m_Vec6Fields[iVector]->setVec6fValue( newValue );
              changed[ iVector ] = true;
            }
          }
          handleNotificationOn();

          for (unsigned int iVector = 0; iVector < nFields; ++iVector){
            if ( changed[iVector] ){
              m_Vec6Fields[iVector]->notifyAttachments();
            }
          }
          break;
        }
      default:
        break;
      }
  }
  m_Updating = false;
}

template< typename T >
bool SyncFields::DiffTest( const T first, const T last, const double precision ) 
{
  unsigned int numComp = sizeof(first)/sizeof(first[0]);
  for (unsigned int iComp=0; iComp < numComp; ++iComp){
    if ( MLAbs(first[iComp] - last[iComp]) > precision) {
      return true;
    }
  }
  return false;
}
bool SyncFields::DiffTest( const double first, const double last, const double precision ) 
{
  
  return (MLAbs(first - last) > precision);
}
ML_END_NAMESPACE

