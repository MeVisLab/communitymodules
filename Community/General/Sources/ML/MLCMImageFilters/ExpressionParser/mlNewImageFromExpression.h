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
// \file    mlNewImageFromExpression.h
// \author  ISI/TVW
// \date    2007-03-06
//
// Generates new image from an expression
*/
//----------------------------------------------------------------------------------


#ifndef __mlNewImageFromExpression_H
#define __mlNewImageFromExpression_H


// Local includes
#ifndef __MLCMImageFiltersSystem_H
#include "MLCMImageFiltersSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include "parser.h"


ML_START_NAMESPACE


//! Generates new image from an expression
class MLCMIMAGEFILTERS_EXPORT NewImageFromExpression : public BaseOp
{
public:

  //! Constructor.
  NewImageFromExpression (void);

  ~NewImageFromExpression (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  // ----------------------------------------------------------
  //@{ \name Image processing methods.
  // ----------------------------------------------------------

  //! Sets properties of the output image at output \c outIndex.
  virtual void calcOutImageProps (int outIndex);


  //! Calculates page \c outSubImg of output image with index \c outIndex by using \c inSubimgs.
  //! \param outSubImg The subimage of output image \c outIndex calculated from \c inSubImgs.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
  //!                  by \c calcInSubImageBox. Array size is given by \c getInputNum().
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

  //! Method template for type-specific page calculation. Called by \c calcOutSubImage().
  //! \param outSubImg The typed subimage of output image \c outIndex calculated from \c inSubImg?.
  //! \param outIndex  The index of the output the subimage is calculated for.
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex);

  //@}

private:


  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(NewImageFromExpression)

    Parser *_parser;

  VectorField *_dimFld;

  VectorField *_pageFld;

  EnumField *_dataTypeFld;

  StringField *_expressionFld;

};


ML_END_NAMESPACE

#endif // __mlNewImageFromExpression_H


