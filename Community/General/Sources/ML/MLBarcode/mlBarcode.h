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

// Include dll-specific settings.
#include "MLBarcodeSystem.h"

// Include most ml specific things.
#include "mlOperatorIncludes.h"

// Be sure that all your modules are part of the ml. So collisions
// with names in system files or other libraries are minimized.
namespace ml{

  //----------------------------------------------------------------------------------
  /*! This module demonstrates

  */
  //----------------------------------------------------------------------------------
  class MLBARCODE_EXPORT Barcode : public BaseOp{

  public:

    //--------------------------------------------------------------------------------
    //! Constructor:
    //! -Calls base class constructor to set right number of inputs and outputs
    //! -Initializes fields for input and output images and algorithm parameters.
    //! -Connects input and parameter fields with output field(s) to propagate
    //!  input changes to output(s) so that appended modules are notified on changes.
    //!  Note that input and output image fields are created automatically when
    //!  the base class constuctor is called.
    //!  Normal algorithm parameters must be created/initialized 'by hand' in
    //!  the constructor.
    //! -Sets algorithm parameters like memory management (inplace calulation) or
    //!  multithreading support (paralallization).
    //--------------------------------------------------------------------------------
    Barcode();
    ~Barcode();

    //--------------------------------------------------------------------------------
    //@{\name Public declaration of algorithm parameters. Use fields.
    //--------------------------------------------------------------------------------
    
    //! Empty space at the beginning and the end of the code 
     IntField* _qFld;
     //! Width of on normal bar
     IntField* _xFld;
     //! Ratio between small and large bars
     FloatField* _ratioFld;
     //! Intervall between two characters
     IntField* _iFld;
     //! Y_Size of the image (X_Size will be calculated, depends on the number of characters)
     IntField* _ysizeFld;
     //! Text to be coded
     StringField* _textFld;
     //! Statusline (not used anymore)
     //  StringField* _statusFld;
      //! Field for foreground value
     IntField* _foregroundFld;
     //! Field for background value
     IntField* _backgroundFld;
    
     //! Apply new code or changes
     NotifyField*  _apply;  /*Apply Button*/
     ToggleField*  _autoApplyFld;  /*AutoApply*/
     
    //@}

  private:
     //! Pixelvalue for background
     MLint _background;
     //! Pixelvalue for foreground
     MLint _foreground;
     
     int _lockNotification;

     MLint _q, _x, _i, _ysize, _xsize;
     float _ratio;

     int* _codeline;

     virtual void makeCodeLine();
     virtual char* Code39(char Asc);

    //---------------------------------------------------------------------------
    //@{\name  Image processing
    //---------------------------------------------------------------------------


    virtual void handleNotification (Field *field);

    //--------------------------------------------------------------------------------
    //! Sets properties of the output image dependent on the properties
    //! of input image(s).
    //! So for each property of the output image \c getOutImg(outIndex)
    //! the corresponding properties of input image \c getInImg(0) and
    //! \c getInImg(1) are merged and set.
    //!
    //! Accessfunctions to the input and output images are defined
    //! in the ml class \c BaseOp:\c getOutImg und \c getInImg.
    //! Within the \c calcOutImageProps methods the properties and
    //! operators are always valid and need not to be checked for validity
    //! (otherwise \c calcOutImageProps wouldn't be called).
    //! Accessfunctions to the properties of an image are defined
    //! in the classes \c ImageProperties, \c MedicalImageProperties and \c PagedImg:
    //! -\c getImgExt,
    //! -\c getBoxFromImgExt,
    //! -\c getPageExt and \c setPageExt,
    //! -\c getDataType and \c setDataType,
    //! -\c getMinVoxelValue and -\c setMinVoxelValue,
    //! -\c getMaxVoxelValue and -\c getMaxVoxelValue.
    //!
    //! If not implemented the default implementation copies the properties
    //! of \c getInImg(0) to the output image(s).
    //--------------------------------------------------------------------------------
    virtual void calcOutImageProps(int outIndex);

    //--------------------------------------------------------------------------------
    //! This function analyses the data type of the image and calls the right
    //! typed (template) function to calculate the output image.
    //! Usually this function is implemented in the .cpp file by using the
    //! macro \c CALC_OUTSUBIMAGE2_CPP in dyadic modules or \c CALC_OUTSUBIMAGE1 in
    //! monadic modules (see mlOperatorMacros.h).
    //! Similar macros for other header implementations and other input/output
    //! configurations are also available.
    //! Please do not implement this call yourself but use use macros from
    //! mlOperatorsMacros.h if possible to make it easier to add new data types
    //! in future releases.
    //--------------------------------------------------------------------------------
    virtual void calcOutSubImage(SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

    //--------------------------------------------------------------------------------
    //! In this method the algorithm must calculate the output page \c outImg from
    //! the input page(s) \c inImg1 and \c inImg2. For each datatype
    //! this method is instanciated. Note that \c calcOutSubImage calls this
    //! function by searching the correct data type and calling the correctly typed
    //! template version. This is implemented automatically by the macro
    //! CALC_OUTSUBIMAGE2 in the .cpp file. Note that this template function should
    //! not be used outside this class since there could appear linker problems,
    //! especially on dll-interfaces. Ask experienced template programmers before
    //! doing that. Do that also before you use a debugger or performance profiler
    //! on template code.
    //--------------------------------------------------------------------------------
    template <typename DATATYPE>
    void calcOutSubImage(TSubImg<DATATYPE> *outImg, int /*outIndex*/);
    //@}

    //--------------------------------------------------------------------------------
    //! This macro declares some automatically generated functions and methods
    //! for the runtime system and for the initialization of this class.
    //! They are needed to ask this module for its name, type and so on
    //! (see mlRuntimeSubClass.h).
    //! IMPORTANT:
    //! -To make this class usable it is neccessary to call its
    //!  static \c init function declared in this macro. It must be called in the
    //!  dll initialization function \c mlInitDllExampleOps.h.
    //! - Be sure that the class name is written correctly since the compiler
    //!   cannot check wrong names here.
    //--------------------------------------------------------------------------------
    ML_BASEOP_CLASS_HEADER(Barcode)


  };

} //end of namespace ml
#endif //of __BARCODE_H





