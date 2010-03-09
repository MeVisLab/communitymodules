// Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR),
// Departments of Radiology and Medical Informatics, Erasmus MC. All
// rights reserved.
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
//! The ML module class ExpressionParser1.
/*!
// \file    mlExpressionParser1.cpp
// \author  ISI, Theo van Walsum
// \date    2007-03-06
//
// Parses expression on input images
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlExpressionParser1_H
#include "mlExpressionParser1.h"
#endif

ML_START_NAMESPACE

#include "parseImage.h"

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(ExpressionParser1, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
ExpressionParser1::ExpressionParser1 (void)
  : BaseOp(1, 1)
{
  ML_TRACE_IN("ExpressionParser1::ExpressionParser1()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  (_expressionFld = fields->addString("expression"))->setStringValue("1.0*x+0.0");
  _expressionFld->attachField(getOutField());
  getInField(0)->attachField(getOutField());
  _parser = makeParser(_expressionFld->getStringValue()); 

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

ExpressionParser1::~ExpressionParser1 (void)
{
  destroyParser(_parser);
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void ExpressionParser1::handleNotification (Field *field)
{
  ML_TRACE_IN("ExpressionParser1::handleNotification()")

  // ... field notification handling code
    if (field == _expressionFld)
      {
        destroyParser(_parser);
        _parser = makeParser(_expressionFld->getStringValue()); 
      }
}

void ExpressionParser1::activateAttachments(){
  this->handleNotification( _expressionFld );
  BaseOp::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Sets properties of the output image at output \c outIndex.
//----------------------------------------------------------------------------------
void ExpressionParser1::calcOutImageProps (int outIndex)
{
  ML_TRACE_IN("ExpressionParser1::calcOutImageProps ()")

  // ... set image properties for output image outIndex
    if (!_parser) 
      getOutImg(outIndex)->setOutOfDate();
}


//----------------------------------------------------------------------------------
//! Returns the input image region required to calculate a region of an output image.
//----------------------------------------------------------------------------------
SubImgBox ExpressionParser1::calcInSubImageBox (int /* inIndex */, const SubImgBox &outSubImgBox, int /* outIndex */)
{
  ML_TRACE_IN("ExpressionParser1::calcInSubImageBox ()")

  // ... return region of input image inIndex needed to compute region outSubImgBox of
  //     output image outIndex

  // replace this with your own SubImgBox:
  return outSubImgBox;
}



//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of \c calcOutSubImage to calculate page 
//! \c outSubImg of output image with index \c outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE1_CPP(ExpressionParser1);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by 
//! CALC_OUTSUBIMAGE1_CPP(ExpressionParser1).
//----------------------------------------------------------------------------------
template <typename T>
void ExpressionParser1::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex,
                                     TSubImg<T> *inSubImg1)
{
  ML_TRACE_IN("template <typename T> ExpressionParser1::calcOutSubImage ()")

  // ... compute subimage of output image outIndex from subimages of all input images
  SubImgBox outBox(outSubImg->getBox().intersect( getOutImg(outIndex)-> getBoxFromImgExt()));

  parseSubImage(inSubImg1, outSubImg, _parser, outBox);

}



ML_END_NAMESPACE

