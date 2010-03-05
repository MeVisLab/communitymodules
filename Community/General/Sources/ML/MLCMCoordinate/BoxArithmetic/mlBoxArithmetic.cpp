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
// \file    mlBoxArithmetic.cpp
// \author  Reinhard Hameeteman
// \date    2008-10-28
//
// Perform simple calculations on a (image) box
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlBoxArithmetic.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(BoxArithmetic, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
BoxArithmetic::BoxArithmetic ()
  : BaseOp(2, 0)
{
  ML_TRACE_IN("BoxArithmetic::BoxArithmetic ()");

  handleNotificationOff();

  FieldContainer *fieldC = getFieldContainer();

  f_InBox0 = fieldC->addSubImgBoxf("inputBox0");
  f_InBox0->setSubImgBoxfValue( SubImgBoxf() );

  f_InBox1 = fieldC->addSubImgBoxf("inputBox1");
  f_InBox1->setSubImgBoxfValue( SubImgBoxf() );

  f_CorrectInput0 = fieldC->addBool( "correctInput0" );
  f_CorrectInput0->setBoolValue( false );

  f_CorrectInput1 = fieldC->addBool( "correctInput1" );
  f_CorrectInput1->setBoolValue( false );

  f_UseVoxelCenter = fieldC->addBool( "useVoxelCenter" );
  f_UseVoxelCenter->setBoolValue( false );

  f_OutBox0 = fieldC->addSubImgBoxf("outputBox0");
  f_OutBox0->setSubImgBoxfValue( SubImgBoxf() );

  f_OutBox1 = fieldC->addSubImgBoxf("outputBox1");
  f_OutBox1->setSubImgBoxfValue( SubImgBoxf() );

  f_OutBox = fieldC->addSubImgBoxf("outputBox");
  f_OutBox->setSubImgBoxfValue( SubImgBoxf() );

  f_Correct = fieldC->addBool("correctBox");
  f_Correct->setBoolValue( false );

  f_OutExtent = fieldC->addVec6f("outputExtent");
  f_OutExtent->setVec6fValue( vec6() );

  f_OutV1 = fieldC->addVec6f("outV1");
  f_OutV1->setVec6fValue( vec6() );
  
  f_OutV2 = fieldC->addVec6f("outV2");
  f_OutV2->setVec6fValue( vec6(-1) );

  f_Constant = fieldC->addFloat("constant");
  f_Constant->setFloatValue( 0.0f );

  f_V1 = fieldC->addVec6f("v1");
  f_V1->setVec6fValue( vec6() );
  f_V2 = fieldC->addVec6f("v2");
  f_V2->setVec6fValue( vec6(-1) );

  (f_BoxV0x = fieldC->addFloat("x1"))->setFloatValue( 0.0f );
  (f_BoxV0y = fieldC->addFloat("y1"))->setFloatValue( 0.0f );
  (f_BoxV0z = fieldC->addFloat("z1"))->setFloatValue( 0.0f );
  (f_BoxV0c = fieldC->addFloat("c1"))->setFloatValue( 0.0f );
  (f_BoxV0t = fieldC->addFloat("t1"))->setFloatValue( 0.0f );
  (f_BoxV0u = fieldC->addFloat("u1"))->setFloatValue( 0.0f );
  (f_BoxV1x = fieldC->addFloat("x2"))->setFloatValue( -1.0f );
  (f_BoxV1y = fieldC->addFloat("y2"))->setFloatValue( -1.0f );
  (f_BoxV1z = fieldC->addFloat("z2"))->setFloatValue( -1.0f );
  (f_BoxV1c = fieldC->addFloat("c2"))->setFloatValue( -1.0f );
  (f_BoxV1t = fieldC->addFloat("t2"))->setFloatValue( -1.0f );
  (f_BoxV1u = fieldC->addFloat("u2"))->setFloatValue( -1.0f );

  const char* modes[LastMode] = {"ExpandScalar","ExpandVector", "AddScalar", "AddVector", "Intersect", "Unify" };
  f_OperationMode = fieldC->addEnum("operationMode", modes, LastMode);
  f_OperationMode->setEnumValue( Intersect );

  const char* inputModes0[4] = {"Box", "Input0", "Scalars", "Vectors"};
  f_InputMode0 = fieldC->addEnum("inputMode0", inputModes0, 4);

  const char* inputModes1[4] = {"Box", "Input1", "Scalars", "Vectors"};
  f_InputMode1 = fieldC->addEnum("inputMode1", inputModes1, 4);

  m_LockUpdate = false;
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void BoxArithmetic::handleNotification (Field *field)
{
  ML_TRACE_IN("BoxArithmetic::handleNotification ()");

  if (m_LockUpdate) {return;}
  m_LockUpdate = true;

  SubImgBoxf newBox = SubImgBoxf();
  SubImgBoxf box0 = SubImgBoxf();
  SubImgBoxf box1 = SubImgBoxf();

  double center = 0.0;
  if ( f_UseVoxelCenter->getBoolValue() ) {center = 0.5;}

  //Box0
  switch ( f_InputMode0->getEnumValue() ){
    case Box0:
      {
        box0 = f_InBox0->getSubImgBoxfValue();
      }
      break;
    case Input0:
      {
        PagedImg *inImage = getNonDummyUpdatedInImg( 0 );
        if ( inImage != NULL ) {
          const Vector size = inImage->getImgExt();
          const vec3 firstVoxel(center,center,center);
          const vec3 lastVoxel(size.x-1+center,size.y-1+center,size.z-1+center);
          vec3 imageOrigin;
          vec3 imageExtent;
          inImage->transformToWorldCoord( firstVoxel,imageOrigin );
          inImage->transformToWorldCoord( lastVoxel, imageExtent );
          box0 = SubImgBoxf(vec6(imageOrigin,0.0,0.0,0.0), vec6(imageExtent,0.0,0.0,0.0));
        } else {
          box0.makeEmpty();
        }
      }
      break;
    case Scalars0:
      {
        vec6 v1 = vec6( f_BoxV0x->getFloatValue(),f_BoxV0y->getFloatValue(),f_BoxV0z->getFloatValue(),
                        f_BoxV0c->getFloatValue(),f_BoxV0t->getFloatValue(),f_BoxV0u->getFloatValue());
        vec6 v2 = vec6( f_BoxV1x->getFloatValue(),f_BoxV1y->getFloatValue(),f_BoxV1z->getFloatValue(),
                        f_BoxV1c->getFloatValue(),f_BoxV1t->getFloatValue(),f_BoxV1u->getFloatValue());
        box0 = SubImgBoxf( v1,v2 );
      }
      break;
    case Vectors0:
      {
        box0 = SubImgBoxf( f_V1->getVec6fValue(), f_V2->getVec6fValue() );
      }
      break;
    default:
      break;
  }
  if ( f_CorrectInput0->getBoolValue() ){
    box0.correct();
  }
  f_OutBox0->setSubImgBoxfValue( box0 );

  //Box1
  switch ( f_InputMode1->getEnumValue() ){
    case Box1:
      {
        box1 = f_InBox1->getSubImgBoxfValue();
      }
      break;
    case Input1:
      {
        PagedImg *inImage = getNonDummyUpdatedInImg( 1 );
        if ( inImage != NULL ) {
          const Vector size = inImage->getImgExt();
          const vec3 firstVoxel(center,center,center);
          const vec3 lastVoxel(size.x-1+center,size.y-1+center,size.z-1+center);
          vec3 imageOrigin;
          vec3 imageExtent;
          inImage->transformToWorldCoord( firstVoxel,imageOrigin );
          inImage->transformToWorldCoord( lastVoxel, imageExtent );
          box1 = SubImgBoxf(vec6(imageOrigin,0.0,0.0,0.0), vec6(imageExtent,0.0,0.0,0.0));
        } else {
          box1.makeEmpty();
        }
        if ( f_CorrectInput1->getBoolValue() ){
          box1.correct();
        }
      }
      break;
    case Scalars1:
      {
        vec6 v1 = vec6( f_BoxV0x->getFloatValue(),f_BoxV0y->getFloatValue(),f_BoxV0z->getFloatValue(),
                        f_BoxV0c->getFloatValue(),f_BoxV0t->getFloatValue(),f_BoxV0u->getFloatValue());
        vec6 v2 = vec6( f_BoxV1x->getFloatValue(),f_BoxV1y->getFloatValue(),f_BoxV1z->getFloatValue(),
                        f_BoxV1c->getFloatValue(),f_BoxV1t->getFloatValue(),f_BoxV1u->getFloatValue());
        box1 = SubImgBoxf( v1,v2 );
        if ( f_CorrectInput1->getBoolValue() ){
          box1.correct();
        }
      }
      break;
    case Vectors1:
      {
        box1 = SubImgBoxf( f_V1->getVec6fValue(), f_V2->getVec6fValue() );
      }
    default:
      break;
  }
  if ( f_CorrectInput1->getBoolValue() ){
    box1.correct();
  }
  f_OutBox1->setSubImgBoxfValue( box1 );

  switch ( f_OperationMode->getEnumValue() ) {
    case ExpandScalar:
      {
        vec6 v1 = box0.v1;
        vec6 v2 = box0.v2;
        const double constant = (double)f_Constant->getFloatValue();
        v1[0] -= constant;
        v1[1] -= constant;
        v1[2] -= constant;
        v2[0] += constant;
        v2[1] += constant;
        v2[2] += constant;
        newBox.v1 = v1;
        newBox.v2 = v2;
        break;
      }
    case ExpandVector:
      {
        const vec6 constant = f_V1->getVec6fValue();
        newBox.v1 = box0.v1 - constant;
        newBox.v2 = box0.v2 + constant;
        break;
      }
    case AddScalar:
      {
        vec6 v1 = box0.v1;
        vec6 v2 = box0.v2;
        const double constant = (double)f_Constant->getFloatValue();
        v1[0] += constant;
        v1[1] += constant;
        v1[2] += constant;
        v2[0] += constant;
        v2[1] += constant;
        v2[2] += constant;
        newBox.v1 = v1;
        newBox.v2 = v2;
        break;
      }
    case AddVector:
      {
        const vec6 constant = f_V1->getVec6fValue();
        newBox.v1 = box0.v1 + constant;
        newBox.v2 = box0.v2 + constant;
        break;
      }
    case Intersect:
      {
        if ( !box0.isEmpty() &&
             !box1.isEmpty() ){
          newBox = SubImgBoxf::intersect( box0, box1 );
        } else {
          newBox = SubImgBoxf(); 
        }
        break;
      }
    case Unify:
      {
        if ( !box0.isEmpty() &&
             !box1.isEmpty() ){
          for (int index = 0; index < 6; ++index ){
            newBox.v1[index] = mlMin( box0.v1[index],
                                      box1.v1[index]);
          }
          for (int index = 0; index < 6; ++index ){
            newBox.v2[index] = mlMax( box0.v2[index],
                                      box1.v2[index]);
          }
        } else {
          if ( box0.isEmpty() ){
            newBox = box1;
          }
          if ( box1.isEmpty() ){
            newBox = box0;
          }
        }
        break;
      }
  }

  if ( f_Correct->getBoolValue() ){ newBox.correct();}
  f_OutBox->setSubImgBoxfValue( newBox );
  f_OutExtent->setVec6fValue( newBox.getExt() );
  f_OutV1->setVec6fValue( newBox.v1 );
  f_OutV2->setVec6fValue( newBox.v2 );
  m_LockUpdate = false;
}

ML_END_NAMESPACE

