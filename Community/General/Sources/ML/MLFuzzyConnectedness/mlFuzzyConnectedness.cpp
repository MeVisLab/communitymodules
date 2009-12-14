//----------------------------------------------------------------------------------
//! The ML module class FuzzyConnectedness.
/*!
// \file    mlFuzzyConnectedness.cpp
// \author  Markus Hüllebrand
// \date    2009-06-12

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

// Local includes
#include "mlFuzzyConnectedness.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(FuzzyConnectedness, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
FuzzyConnectedness::FuzzyConnectedness ()
: BaseOp(2, 2)
{
  ML_TRACE_IN("FuzzyConnectedness::FuzzyConnectedness ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Initialise fields
  _startButtonFld = getFieldContainer()->addNotify("startButton");
  _autoUpdateFld = getFieldContainer()->addBool("autoUpdate");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();

  // Activate parallel execution of calcOutSubImage.
  setThreadSupport(NO_THREAD_SUPPORT);

  // Specify whether the module can only process standard scalar voxel types or
  // also registered voxel types (vec2, mat2, complexf, Vector, etc.)
  setVoxelDataTypeSupport(ONLY_STANDARD_TYPES);
}

FuzzyConnectedness::~FuzzyConnectedness()
{
  // Clear image data.
  clearData();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void FuzzyConnectedness::handleNotification (Field *field)
{
  ML_TRACE_IN("FuzzyConnectedness::handleNotification ()");
  
  // Calculation will only be performed if either the start 
  // button is pressed or the auto update mechanism is used. 
  if ((field ==_startButtonFld) || ((field != _startButtonFld) && _autoUpdateFld->isOn()))
  {
    //´If either input image is open the calculation will be cancelled.
    if(!getUpdatedInImg(0) || !getUpdatedInImg(1))
    {
      _state = ML_DISCONNECTED_GRAPH;
      clearData();
      getOutImg(0)->setOutOfDate();
      getOutImg(1)->setOutOfDate();
      getOutField(0)->notifyAttachments();
      getOutField(1)->notifyAttachments();
    }
    else
    {
      // Same extent of image data and seed point image are required
      if ( getUpdatedInImg(0)->getImgExt() != getUpdatedInImg(1)->getImgExt() ){
        _state = ML_BAD_DIMENSION;
      }
      // Calculation will be started here
      else{
        _state = calculate(getUpdatedInImg(0),getUpdatedInImg(1));
      }
    }

    // Let connected modules know that the output has changed.
    getOutField(0)->notifyAttachments();
    getOutField(1)->notifyAttachments();
  }
}

MLErrorCode FuzzyConnectedness::calculate(PagedImg *inputData, PagedImg *inputMarker)
{
  // Clear former data.
  clearData();
  // Get the images' extents. 
  Vector extent = inputData->getImgExt();
  // Create image box for internal images.
  // The input image extents  
  SubImgBox box(Vector(-1,-1,-1,0,0,0),Vector(extent.x, extent.y, extent.z, 0, 0,0)); 
  //Set extends of internal images
  _connectivityMapOutImg.setBox(box);
  _labelMapOutImg.setBox(box);
  _inputTmpImg.setBox(box);
  _pointerTmpImg.setBox(box);
  // Copy input image
  MLErrorCode status = getTile(inputData, _inputTmpImg);
  if(status==ML_RESULT_OK)
  {
    // Copy seed points
    status = getTile(inputMarker,_labelMapOutImg);
    if(status==ML_RESULT_OK)
    {   
      // Allocate memory for the connectivity map.
      _connectivityMapOutImg.allocateAsMemoryBlockHandle();
      if(_connectivityMapOutImg.getData()!=NULL)
      {
        // Allocate memory for the temporary image used internally.
        _pointerTmpImg.allocateAsMemoryBlockHandle();
        if( _pointerTmpImg.getData() != NULL )
        {
          // Calculate the minimum and maximum intensity values in the image.
          // The original SubImgBox has to be used, as the padding voxels have not been
          // initialised. (As they are never read in the algorithm it is not necessary!!)
          _inputTmpImg.calcMinMax(_minValue,_maxValue,&(SubImgBox(extent)));
          // initialise the allocated data.
          _connectivityMapOutImg.fillSubImg(_minValue);
          _pointerTmpImg.fillSubImg(0);
          // initialise the segmentation module.
          _segmentCore.setProperty(extent.x+2,extent.y+2,extent.z+2, _minValue);
          //Start calculation. Resulting connectivity map will be stored in _outputImageData, the color map in _markerData
          _segmentCore.startShortestPathSearchAsFloat(_inputTmpImg.getData(),_connectivityMapOutImg.getData(),
            _labelMapOutImg.getData(),_pointerTmpImg.getData());

          // Free temporary data. 
          _inputTmpImg.free();
          _pointerTmpImg.free();
        }
        else
        {
          // If memory allocation failed MLErrorCode
          status=ML_NO_MEMORY;
        }
      }
      else
      {
        // If memory allocation failed MLErrorCode
        status = ML_NO_MEMORY;
      }
    }
  }
  if(status != ML_RESULT_OK)
  {
    // If an error occured clear image data.
    clearData();
  }
  return status;
}

//----------------------------------------------------------------------------------
//! Sets properties of the output image at output outIndex.
//----------------------------------------------------------------------------------
void FuzzyConnectedness::calcOutImageProps (int outIndex)
{
  ML_TRACE_IN("FuzzyConnectedness::calcOutImageProps ()");

  PagedImg *outImg = getOutImg(outIndex);

  if( ( _connectivityMapOutImg.getData() == NULL ) || ( _labelMapOutImg.getData() == NULL ) )
  {   // Error during the image processing. Data cannot be restored
    outImg->setOutOfDate();
    switch(_state)
    {
    case ML_NO_MEMORY:
      outImg->setStateInfo("Not enough memory for calculation!",_state);
      break;
    case ML_BAD_DIMENSION:
      outImg->setStateInfo("Same dimensions of input images are required!", _state);
      break;
    case ML_DISCONNECTED_GRAPH:
      outImg->setStateInfo("Unconnected/invalid image is used!",_state);
      break;
    default:
      outImg->setStateInfo("Unknown error detected!",ML_CALCULATION_ERROR);
    }
    
  }
  else
  {
    switch(outIndex)
    {
    case 0: 
      outImg->setDataType(MLfloatType); 
      outImg->setMaxVoxelValue(_maxValue);
      outImg->setMinVoxelValue(_minValue);
      // Min and max voxel values will be copied from input image
      break;
    case 1:      
      // Due to the properties of the algorithm the maximal number of different seed regions
      // limited to 63
      outImg->setDataType(MLuint8Type);
      outImg->setMaxVoxelValue(63);
      outImg->setMinVoxelValue(0);
      break;
    default:
      ML_PRINT_ERROR("FuzzyConnectedness::calcOutImageProps", 
        ML_PROGRAMMING_ERROR, 
        "Unknown output index detected.");
      outImg->setOutOfDate();
      outImg->setStateInfo("Unknown output index detected!", ML_PROGRAMMING_ERROR);
      return;
    }
  }
}

//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of calcOutSubImage to calculate page
//! outSubImg of output image with index outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE2_CPP(FuzzyConnectedness);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by
//! CALC_OUTSUBIMAGE2_CPP(FuzzyConnectedness).
//----------------------------------------------------------------------------------
template <typename T>
void FuzzyConnectedness::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex
                                          , TSubImg<T> *inSubImg0,TSubImg<T> *inSubImg1
                                          )
{
  ML_TRACE_IN("template <typename T> FuzzyConnectedness::calcOutSubImage ()");

  // Compute the requested SubImage from the previously calculated data.
  // outIndex==0 : request for connectivity map.
  // outIndex==1 : request for label map.
  // If the calculation has not terminated correctly the requested data has been deleted.
  // Therefore an image filled with zero is returned. 
  if((outIndex ==0) &&  (_connectivityMapOutImg.getData()!=NULL))
  {
    outSubImg->copySubImage(_connectivityMapOutImg);
  }
  else if((outIndex == 1) && _labelMapOutImg.getData()!=NULL)
  {
    outSubImg->copySubImage( _labelMapOutImg);
  }
  else
  {
    outSubImg->fillSubImg(0);
  }
}


void FuzzyConnectedness::clearData()
{
  // Clear image data
  _pointerTmpImg.free();
  _inputTmpImg.free();
  _labelMapOutImg.free();
  _connectivityMapOutImg.free();
}

ML_END_NAMESPACE

