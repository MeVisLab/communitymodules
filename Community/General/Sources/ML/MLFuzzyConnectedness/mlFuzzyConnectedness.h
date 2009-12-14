//----------------------------------------------------------------------------------
//! The ML module class FuzzyConnectedness.
/*!
// \file    mlMLFuzzyConnectedness.h
// \author  Markus Hüllebrand
// \date    2009-06-12
// 
// This code implements a Fuzzy Connectedness algorithm in the software platform
// MeVisLab. The code consists of two parts, the Fuzzy Connectedness algorithm and
// and a C++ wrapper that fits the MeVisLab ML interface. The algorithm is found
// in the files CMIVFuzzyConnectedness.(h,cpp).

This file is part of the FuzzyConnectedness module for MeVisLab.

Copyright (c) 2001-2009, Fraunhofer MEVIS, Bremen, Germany
ALL RIGHTS RESERVED

The FuzzyConnectedness module for MeVisLab is free software:
you can redistribute it and/or modify it under the terms of the 
GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The FuzzyConnectedness module for MeVisLab is distributed in 
the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
//----------------------------------------------------------------------------------


#ifndef __mlMLFuzzyConnectedness_H
#define __mlMLFuzzyConnectedness_H

// Local includes
#include "MLFuzzyConnectednessSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#include "CMIVFuzzyConnectedness.h"
ML_START_NAMESPACE

//! 
class MLFUZZYCONNECTEDNESS_EXPORT FuzzyConnectedness : public BaseOp
{
public:

  //! Constructor.
  FuzzyConnectedness ();
  //! Destructor
  virtual ~FuzzyConnectedness();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

  //! Sets properties of the output image at output outIndex.
  //! \param outIndex The index of the output the subimage is calculated for.
  //! \param outImg The output image at index outIndex
  virtual void calcOutImageProps (int outIndex);

  //! Calculates page outSubImg of output image with index outIndex by using inSubimgs.
  //! \param outSubImg The subimage of output image outIndex calculated from inSubImgs.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
  //!                  by calcInSubImageBox. Array size is given by getInputNum().
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

  //! Method template for type-specific page calculation. Called by calcOutSubImage().
  //! \param outSubImg The typed subimage of output image outIndex calculated from inSubImg.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImg0 Temporary subimage of input 0.
  //! \param inSubImg1 Temporary subimage of input 1.
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex
                        , TSubImg<T> *inSubImg0,TSubImg<T> *inSubImg1
                       );
  //@}

private:
  //! Copies the data from the given input images to subimages, allocates additionally required memory
  //! and performs calculation. The additional memory will be freed afterwards.
  //! \param inputData Input data. MLfloat required.
  //! \param inputMarker Image containing different markers. MLuint8 required.
  //! \return Either error code or ML_RESULT_OK is returned.
  MLErrorCode calculate(PagedImg* inputData, PagedImg* inputMarker);

  //! Method to clean up the memory.
  void clearData();
  //! Notification field that triggers the calculation.
  NotifyField   *_startButtonFld;
  //! Field to allow an auto update functionality.
  BoolField *_autoUpdateFld;
  //! Core class performing the calculation.
  CMIVFuzzyConnectedness _segmentCore;
  //! Connectivity map image.
  TSubImg<MLfloat> _connectivityMapOutImg; 
  //! Label map image.
  TSubImg<MLuint8> _labelMapOutImg; 
  //! Temporary input image.
  TSubImg<MLfloat> _inputTmpImg; 
  //! Temporary pointer image.
  TSubImg<MLuint8> _pointerTmpImg; 
  MLErrorCode _state;
  //! Minimum intensity value of input image.
  MLfloat _minValue;
  //! Maximum intensity value of input image.
  MLfloat _maxValue;

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(FuzzyConnectedness)
};


ML_END_NAMESPACE

#endif // __mlMLFuzzyConnectedness_H

