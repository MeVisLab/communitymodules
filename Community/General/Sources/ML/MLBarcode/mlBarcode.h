// Copyright (c) 2009, Fraunhofer MEVIS Institute for Medical Image Computing
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Fraunhofer MEVIS nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR THE COPYRIGHT HOLDER BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
//! Defines and implements the module Barcode.
/*!
// \file    mlBarcode.h
// \author  Dominik Boehm
// \date    8/2003
*/
//----------------------------------------------------------------------------------
#ifndef __BARCODE_H
#define __BARCODE_H

// Include system specific settings.
#include "MLBarcodeSystem.h"

// Include most ml specific things.
#include "mlOperatorIncludes.h"

// Be sure that all your modules are part of the ml. So collisions
// with names in system files or other libraries are minimized.
ML_START_NAMESPACE

  //----------------------------------------------------------------------------------
  //! Module to create barcode images.
  //----------------------------------------------------------------------------------
  class MLBARCODE_EXPORT Barcode : public BaseOp{

  public:

    //! Constructor:
    Barcode();

    //! Destructor.
    ~Barcode();

    //--------------------------------------------------------------------------------
    //@{\name Public declaration of algorithm parameters. Use fields.
    //--------------------------------------------------------------------------------
    //! Empty space at the beginning and the end of the code.
    IntField* _qFld;
    //! Width of on normal bar.
    IntField* _xFld;
    //! Ratio between small and large bars.
    FloatField* _ratioFld;
    //! Interval between two characters.
    IntField* _iFld;
    //! Y_Size of the image (X_Size will be calculated, depends on the number of characters).
    IntField* _ysizeFld;
    //! Text to be coded.
    StringField* _textFld;
     //! Field for barcode pixel value.
    IntField* _foregroundFld;
    //! Field for image background value.
    IntField* _backgroundFld;

    //! Field to manage the apply button.
    NotifyField*  _apply;

    //! Field to manage the auto apply toggle.
    BoolField*  _autoApplyFld;
    //@}

  private:

    int _lockNotification;

    MLint _xsize;

    //! Pointer to a memory buffer for a code line.
    int* _codeline;

    //! \name Helper functions.
    //@{
    //! Creates the bar coded pixel line.
    virtual void makeCodeLine();

    //! Returns the Code 3 of 9 value for a given ASCII character
    virtual char* Code39(char Asc);
    //@}

    //! Handle field changes of the field field.
    virtual void handleNotification (Field *field);

    //! \name Image processing.
    //@{
    //! Sets properties of the output image at output outIndex.
    virtual void calcOutImageProps(int outIndex);

    //! Calculates page outSubImg of output image with index outIndex by using inSubimgs.
    //! \param outSubImg The subimage of output image outIndex calculated from inSubImgs.
    //! \param outIndex  The index of the output the subimage is calculated for.
    //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
    //!                  by calcInSubImageBox. Array size is given by getInputNum().
    virtual void calcOutSubImage(SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

    //! Method template for type-specific page calculation. Called by calcOutSubImage().
    //! \param outSubImg   The typed subimage of output image outIndex loaded form file.
    //! Parameter outIndex is the index of the output the subimage is calculated for and
    //!                    is not used here.
    template <typename DATATYPE>
    void calcOutSubImage(TSubImg<DATATYPE> *outImg, int /*outIndex*/);
    //@}

    //! Implements interface for the runtime type system of the ML.
    ML_BASEOP_CLASS_HEADER(Barcode)

  };

ML_END_NAMESPACE

#endif //of __BARCODE_H





