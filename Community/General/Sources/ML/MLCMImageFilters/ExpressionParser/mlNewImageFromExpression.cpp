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
//! The ML module class NewImageFromExpression.
/*!
// \file    mlNewImageFromExpression.cpp
// \author  ISI/TVW
// \date    2007-03-06
//
// Generates new image from an expression
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlNewImageFromExpression_H
#include "mlNewImageFromExpression.h"
#endif


ML_START_NAMESPACE


#include "parseImage.h" 

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(NewImageFromExpression, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
NewImageFromExpression::NewImageFromExpression (void)
  : BaseOp(0, 1)
{
  ML_TRACE_IN("NewImageFromExpression::NewImageFromExpression()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  getOutField(0)->setName("outputImage");

  _dimFld = fields->addVector("imageDimension");
  _dimFld->setVectorValue(Vector(128,128,1,1,1,1));
  _dimFld->attachField(getOutField());
  _pageFld = fields->addVector("pageSize");
  _pageFld->setVectorValue(Vector(128,128,1,1,1,1));
  _pageFld->attachField(getOutField());

  _dataTypeFld = fields->addEnum("datatype",MLDataTypeNames(),MLNumDataTypes());
  _dataTypeFld->setEnumValue(MLuint8Type);
  _dataTypeFld->attachField(getOutField());

  _expressionFld = fields->addString("expression");
  _expressionFld->setStringValue("0");
  _expressionFld->attachField(getOutField());
  _parser = makeParser(_expressionFld->getStringValue()); 

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();

}

NewImageFromExpression::~NewImageFromExpression (void)
{
  destroyParser(_parser);
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void NewImageFromExpression::handleNotification (Field * /* field */)
{
  ML_TRACE_IN("NewImageFromExpression::handleNotification()")
  // mlDebug("handleNotification for field" << field->getName() );

}


//----------------------------------------------------------------------------------
//! Sets properties of the output image at output \c outIndex.
//----------------------------------------------------------------------------------
void NewImageFromExpression::calcOutImageProps (int outIndex)
{
  ML_TRACE_IN("NewImageFromExpression::calcOutImageProps ()")

    destroyParser(_parser);
    _parser = makeParser(_expressionFld->getStringValue()); 
    if (!_parser) 
      getOutImg(outIndex)->setOutOfDate();
    else
      {
        getOutImg(outIndex)->setImgExt(_dimFld->getVectorValue());
        getOutImg(outIndex)->setPageExt(_pageFld->getVectorValue());
        getOutImg(outIndex)->setDataType(MLfloatType);
      }

  // ... set image properties for output image outIndex
}



//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of \c calcOutSubImage to calculate page 
//! \c outSubImg of output image with index \c outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE0_CPP(NewImageFromExpression);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by 
//! CALC_OUTSUBIMAGE0_CPP(NewImageFromExpression).
//----------------------------------------------------------------------------------
template <typename T>
void NewImageFromExpression::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex)
{
  ML_TRACE_IN("template <typename T> NewImageFromExpression::calcOutSubImage ()")

  // ... compute subimage of output image outIndex from subimages of all input images
  SubImgBox outBox(outSubImg->getBox().intersect( getOutImg(outIndex)-> getBoxFromImgExt()));

  parseCoordinatesImage(outSubImg, _parser, outBox);
}



ML_END_NAMESPACE

