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
//----------------------------------------------------------------------------------
//! The ML module class MatlabScriptWrapper.
/*!
// \file    mlMatlabScriptWrapper.h
// \author  Alexander Gryanik, Markus Harz, Ola Friman
// \date    2009-02-23
//
// Module for executing Matlab scripts in MeVisLab.
*/
//----------------------------------------------------------------------------------


#ifndef __mlMatlabScriptWrapper_H
#define __mlMatlabScriptWrapper_H


// Local includes
#include "MLMatlabScriptWrapperSystem.h"
#include "MatlabCommon/engine.h"
#include "mlXMarkerList.h"

ML_START_NAMESPACE

//! The ML module class MatlabScriptWrapper.
class MLMATLABSCRIPTWRAPPER_EXPORT MatlabScriptWrapper : public BaseOp
{
public:

  //! Constructor.
  MatlabScriptWrapper (void);
  //! Destructor.
  ~MatlabScriptWrapper ();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  //! Configures (in)validation handling of inputs which are not connected or up to date.
  virtual INPUT_HANDLE handleInput(int inIndex, INPUT_STATE state) const;

  // ----------------------------------------------------------
  //@{ \name Image processing methods.
  // ----------------------------------------------------------
  
  //! Sets properties of the output image at output \c outIndex.
  virtual void calcOutImageProps (int outIndex);
  
  //! Returns the input image region required to calculate a region of an output image.
  //! \param inIndex        The input of which the regions shall be calculated.
  //! \param outSubImageBox The region of the output image for which the required input region
  //!                       shall be calculated.
  //! \param outIndex       The index of the output image for which the required input region
  //!                       shall be calculated.
  //! \return Region of input image needed to compute the region \c outSubImgBox on output \c outIndex.
  virtual SubImgBox calcInSubImageBox (int inIndex, const SubImgBox &outSubImgBox, int outIndex);
  
  //! Request input image in basic datatypes according to user choice
  //! \param inIndex        The input of which the datatype shall be set.
  //! \param props          The properties of input image upon calling of method.
  //! \param outIndex       The index of the output image.
  virtual void calcInSubImageProps(int inIndex, InSubImageProps &/*props*/, int /*outIndex*/);
  
  //! Calculates page \c outSubImg of output image with index \c outIndex by using \c inSubimgs.
  //! \param outSubImg The subimage of output image \c outIndex calculated from \c inSubImgs.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
  //!                  by \c calcInSubImageBox. Array size is given by \c getInputNum().
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg* /*inSubImgs*/);
  
  //! Method template for type-specific page calculation. Called by \c calcOutSubImage().
  //! \param outSubImg The typed subimage of output image \c outIndex calculated from \c inSubImg?.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImg1 Temporary subimage of input 1.
  //  template <typename T>
  //  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex, TSubImg<T> *inSubImg1);
  //  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImg);
  
  //@}

private:
  // ----------------------------------------------------------
  //@{ \name Module internal methods declarations
  // ----------------------------------------------------------
  //! Loads matlab script from a file, pastes it into script field and saves user written script. 
  bool _loadMatlabScriptFromFile(std::string& evaluateString);
  //! Check if Matlab is started.
  bool _checkMatlabIsStarted();
  //! Clear all variables that have been put in the Matlab workspace.
  void _clearAllVariables();

  //! Copies input image data into Matlab.
  void _copyInputImageDataToMatlab();
  //! Copies input XMarkerList into Matlab.
  void _copyInputXMarkerToMatlab();
  //! Copies XMarkerList from Matlab and copies results into output XMarkerList.
  void _getXMarkerBackFromMatlab();
  //! Copies scalar values to matlab.
  void _copyInputScalarsToMatlab();
  //! Copies scalar values from matlab.
  void _getScalarsBackFromMatlab();
  //! Copies string values to matlab.
  void _copyInputStringsToMatlab();
  //! Copies string values from matlab.
  void _getStringsBackFromMatlab();
  //! Copies vector values to matlab.
  void _copyInputVectorsToMatlab();
  //! Copies vector values from matlab.
  void _getVectorsBackFromMatlab();
  //! Copies matrix values to matlab.
  void _copyInputMatricesToMatlab();
  //! Copies matrix values from matlab.
  void _getMatricesBackFromMatlab();

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //! The XMarkerList input field.
  BaseField *_inputXMarkerListFld;
  //! The XMarkerList output field.
  BaseField *_outputXMarkerListFld;
  //! The output list
  XMarkerList _outputXMarkerList;

  //! Type a matlab script into this field.
  StringField* _matlabScriptFld;
  //! Use external script.
  BoolField* _useExternalScriptFld;

  //! Name of file where matlab script will be dumped.
  StringField *_matlabScriptPathFld;

  //! Show the Matlab session window.
  BoolField* _showSessionWindowFld;

  //{@ Set matlab names for input and output images.
  StringField *_inDataNameFld[3];
  StringField *_outDataNameFld[3];
  //@}
  //{@ Set matlab names for input and output XMarkerList.
  StringField *_inXMarkerNameFld;
  StringField *_outXMarkerNameFld;
  //@}

  //! If true, the module updates on field changes.
  BoolField* _autoCalculationFld;
  //! Status messages.
  StringField* _statusFld;
  //! If pressed, the module updates.
  NotifyField* _calculateFld;
  //! Restart Matlab button.
  NotifyField* _restartMatlabFld;

  //{@ Scalar float values.
  DoubleField *_scalarFld[6];
  StringField *_scalarNameFld[6];
  //@}
  //{@ Vector values.
  Vec4fField *_vectorFld[6];
  StringField *_vectorNameFld[6];
  //@}
  //{@ Matrix values.
  MatrixField *_matrixFld[3];
  StringField *_matrixNameFld[3];
  //@}
  //{@ String values.
  StringField *_stringFld[6];
  StringField *_stringNameFld[6];
  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(MatlabScriptWrapper)

  //! Instance of the Matlab engine.
  ::Engine *m_pEngine;
};


ML_END_NAMESPACE

#endif // __mlMatlabScriptWrapper_H

