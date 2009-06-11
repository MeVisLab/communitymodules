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
// \file    mlMatlabScriptWrapper.cpp
// \author  Alexander Gryanik, Markus Harz, Ola Friman 
// \date    2009-02-23
//
// Module for executing Matlab scripts in MeVisLab.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMatlabScriptWrapper.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4702 ) // VC8: unreachable code
ML_BASEOP_CLASS_SOURCE(MatlabScriptWrapper, BaseOp);
#pragma warning( pop )
#else
ML_BASEOP_CLASS_SOURCE(MatlabScriptWrapper, BaseOp);
#endif


//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
MatlabScriptWrapper::MatlabScriptWrapper (void)
: BaseOp(3, 3)
{
  ML_TRACE_IN("MatlabScriptWrapper::MatlabScriptWrapper()")

  FieldContainer *fields = getFieldContainer();
  ML_CHECK(fields);

  //! Change field values here without calling handleNotification.
  handleNotificationOff();

  (_inputXMarkerListFld = fields->addBase("inputXMarkerList"))->setBaseValue(NULL);
  _outputXMarkerListFld = fields->addBase("outputXMarkerList");
  _outputXMarkerListFld->setBaseValue(&_outputXMarkerList);

  (_matlabScriptFld = fields->addString("matlabScript"))->setStringValue("Output0=Input0 % Type your matlab script here.");

  //! Use external matlab script.
  (_useExternalScriptFld = fields->addBool("useExternalScript"))->setBoolValue(false);

  //! Where will matlab script be dumped.
  (_matlabScriptPathFld = fields->addString("matlabScriptPath"))->setStringValue("");

  //! Set input and output data names used in matlab.
  (_inDataNameFld[0] = fields->addString("inDataName0"))->setStringValue("Input0");
  (_inDataNameFld[1] = fields->addString("inDataName1"))->setStringValue("Input1");
  (_inDataNameFld[2] = fields->addString("inDataName2"))->setStringValue("Input2");
  (_outDataNameFld[0] = fields->addString("outDataName0"))->setStringValue("Output0");
  (_outDataNameFld[1] = fields->addString("outDataName1"))->setStringValue("Output1");
  (_outDataNameFld[2] = fields->addString("outDataName2"))->setStringValue("Output2");
  //! Set input and output XMarker names used in matlab.
  (_inXMarkerNameFld = fields->addString("inXMarkerName"))->setStringValue("inXMarker");
  (_outXMarkerNameFld = fields->addString("outXMarkerName"))->setStringValue("outXMarker");

  //! Create image data randomly.
  (_autoCalculationFld = fields->addBool("autoUpdate"))->setBoolValue(false);
  //! Add toggle to delete user set variables before new calculation.
  //(_deleteMatlabVarFld = fields->addBool("delMatlabVar"))->setBoolValue(false);
  //! Add update button.
  _calculateFld = fields->addNotify("update");
  //! Add restart Matlab button.
  _restartMatlabFld = fields->addNotify("restartMatlab");
  // Error message.
  (_statusFld = fields->addString("status"))->setStringValue("Ready.");

  // Set scalar name and value fields.
  (_scalarNameFld[0] = fields->addString("scalarName0"))->setStringValue("scalar0");
  (_scalarFld[0] = fields->addDouble("scalar0"))->setDoubleValue(0.0);
  (_scalarNameFld[1] = fields->addString("scalarName1"))->setStringValue("scalar1");
  (_scalarFld[1] = fields->addDouble("scalar1"))->setDoubleValue(0.0);
  (_scalarNameFld[2] = fields->addString("scalarName2"))->setStringValue("scalar2");
  (_scalarFld[2] = fields->addDouble("scalar2"))->setDoubleValue(0.0);
  (_scalarNameFld[3] = fields->addString("scalarName3"))->setStringValue("scalar3");
  (_scalarFld[3] = fields->addDouble("scalar3"))->setDoubleValue(0.0);
  (_scalarNameFld[4] = fields->addString("scalarName4"))->setStringValue("scalar4");
  (_scalarFld[4] = fields->addDouble("scalar4"))->setDoubleValue(0.0);
  (_scalarNameFld[5] = fields->addString("scalarName5"))->setStringValue("scalar5");
  (_scalarFld[5] = fields->addDouble("scalar5"))->setDoubleValue(0.0);

  //! Reactivate calls of handleNotification on field changes.
  handleNotificationOn();

  m_pEngine  = engOpen("\0");
  if ( !_checkMatlabIsStarted() )
  {
    std::cerr << "MatlabScriptWrapper::MatlabScriptWrapper():" << std::endl;
    std::cerr << "Error: Matlab Engine not found. For this module to work, a matlab installation is required." << std::endl << std::endl;
    std::cerr << "Additional Hints: " << std::endl;
    std::cerr << " (1) On Windows, the matlab COM server must be registered. Execute " << std::endl;
    std::cerr << "       >> matlab /regserver " << std::endl;
    std::cerr << "     on a command line." << std::endl;
    std::cerr << " (2) On MacOSX, it is currently required to start MeVisLab from a shell with" << std::endl;
    std::cerr << "     PATH set to " << std::endl;
    std::cerr << "     $PATH:/bin:/sbin:/usr/bin:/usr/sbin:/Applications/MATLAB_R2007b/bin" << std::endl;
    std::cerr << " (3) When starting from XCode, ensure that the executable environment contains a variable"  << std::endl;
    std::cerr << "     PATH set to the above. Edit your 'current executable' settings accordingly."  << std::endl;
    std::cerr << "     DYLD_LIBRARY_PATH has to be extended to contain" << std::endl;
    std::cerr << "     /Applications/MATLAB_R2007b/bin/maci:/Applications/MATLAB_R2007b/sys/os/maci" << std::endl;
  }
}


//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
MatlabScriptWrapper::~MatlabScriptWrapper()
{
  ML_TRACE_IN("MatlabScriptWrapper::~MatlabScriptWrapper()");

  if (!_checkMatlabIsStarted()) {
    engClose(m_pEngine);
  }
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::handleNotification (Field* field)
{
  ML_TRACE_IN("MatlabScriptWrapper::handleNotification()");

  if(field == _restartMatlabFld)
  {
    if(!_checkMatlabIsStarted()) {
      // Start Matlab if it's not started.
      m_pEngine  = engOpen("\0");
    }
    else{
      std::cout<<"Matlab is already started";
    }
  }

  // Update output only if autoapply is enabled.
  if ( ((field == getInField(0))||(field == getInField(1))||(field == getInField(2))) && (_autoCalculationFld->isOn()) 
    || (field == _calculateFld) )
  {
    // Check if Matlab is started.
    if (!_checkMatlabIsStarted()) {
      _statusFld->setStringValue("Cannot finding matlab engine!");
      return;
    }

    // Get script to evaluate
    std::string evaluateString = "";
    bool validScriptString = true;
    if(_useExternalScriptFld->isOn()) { // Get script from .m-file
      validScriptString = _loadMatlabScriptFromFile(evaluateString);
    }
    else {
      evaluateString = _matlabScriptFld->getStringValue();
    }

    // Execute Matlab script only when the string is valid
    if(validScriptString) {
      // Copy input XMarker to matlab.
      _copyInputXMarkerToMatlab();
      // Copy input image data to matlab.
      _copyInputImageDataToMatlab();
      // Copy scalar values to matlab.
      _copyInputScalarsToMatlab();


      // Insert at the end of the script variable to proof execution status
      // and run the script in Matlab
      evaluateString += "\nmevmatscr=1";
      _statusFld->setStringValue("Matlab script is executing....");
      engEvalString(m_pEngine, evaluateString.c_str());

      // If variable mevmatscr exist it means the whole matlab script was executed.
      mxArray *mtmp = engGetVariable(m_pEngine,"mevmatscr");
      if(mtmp!=NULL) {
        _statusFld->setStringValue("Execution successful!");
        engEvalString(m_pEngine, "clear mevmatscr");
      }
      else {
        _statusFld->setStringValue("Matlab script contains errors!");
        _clearAllVariables();
      }
    }
    // If the script string was not valid, clear all data so that
    // the user notes this.
    else {
      _clearAllVariables();
    }

    // Get XMarkerList from Matlab and copy results into output XMarkerList
    _getXMarkerBackFromMatlab();

    // Get scalars back from matlab. First store the current scalars so that
    // we can check if they change. A notification is only sent upon change.
    double tmpScalars[6];
    for(int k=0; k<6; ++k) {
	   tmpScalars[k] = _scalarFld[k]->getDoubleValue();
	 }
    _getScalarsBackFromMatlab();
    for(int k=0; k<6; ++k) {
      if(tmpScalars[k] == _scalarFld[k]->getDoubleValue()) {
        _scalarFld[k]->notifyAttachments();
      }
    }

    // Notify image attachments that are new images calculated so that they
    // update themselves and call the calcOutSubImage()
    getOutField(0)->notifyAttachments();
    getOutField(1)->notifyAttachments();
    getOutField(2)->notifyAttachments();

    // Notify the XMarkerList output
    _outputXMarkerListFld->notifyAttachments();
  }
}

//----------------------------------------------------------------------------------
//! Configures (in)validation handling of inputs which are not connected or up to date.
//----------------------------------------------------------------------------------
BaseOp::INPUT_HANDLE MatlabScriptWrapper::handleInput(int inIndex, INPUT_STATE /*state*/) const
{
  ML_TRACE_IN("MatlabScriptWrapper::handleInput ()");

  // If one of inputs is open we close it artificially by setting the input to a dummy input.
  if((inIndex == 0)||(inIndex == 1)||(inIndex == 2)) {
    return BaseOp::REDIRECT_TO_DUMMY_OP;
  }
  // If any other input is not present or invalid, we cannot do any processing.
  else {
    return BaseOp::INVALIDATE;
  }
}

//----------------------------------------------------------------------------------
//! Sets properties of the output image at output \c outIndex.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::calcOutImageProps(int outIndex)
{
  ML_TRACE_IN("MatlabScriptWrapper::calcOutImageProps ()");

  // Proof if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "calcOutImageProps(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }

  // Get variable name in the matlab workspace of the ouput image 
  std::string outname = _outDataNameFld[outIndex]->getStringValue();

  // Get the size of the image via the matlab workspace
  std::ostringstream dim;
  dim << "mevtmpdim=size(" << outname << ")";
  engEvalString(m_pEngine, dim.str().c_str());
  mxArray *m_Y = engGetVariable(m_pEngine, "mevtmpdim");

  // If we could find the variable and calculate its size, go on.
  if(m_Y != NULL)
  {
    const mwSize m_numDims = mxGetN(m_Y);
    double *ext = static_cast<double*>(mxGetPr(m_Y));
    Vector outExt = Vector(1,1,1,1,1,1);
    for (size_t i=0; i<m_numDims; i++)
    {
      outExt[i] = static_cast<MLint>(ext[i]);
    }
    // Set page size.
    getOutImg(outIndex)->setPageExt(outExt);
    // Set output image size.
    getOutImg(outIndex)->setImgExt(outExt);
    // Set output image datatype.
    getOutImg(outIndex)->setDataType(MLdoubleType);
    mxDestroyArray(m_Y);
    m_Y = NULL;
    // Delete temp variable.
    engEvalString(m_pEngine, "clear mevtmpdim");  

    // Get min and max values in matlab workspace and set them in MeVisLab
    std::ostringstream minmaxCommand, maxCommand;
    minmaxCommand << "mevtmpminval = min(" << outname << "(:));" << "mevtmpmaxval = max(" << outname << "(:));";
    engEvalString(m_pEngine, minmaxCommand.str().c_str());
    mxArray *minVal = engGetVariable(m_pEngine, "mevtmpminval");
    mxArray *maxVal = engGetVariable(m_pEngine, "mevtmpmaxval");
    getOutImg(outIndex)->setMinVoxelValue(*static_cast<double*>(mxGetPr(minVal)));
    getOutImg(outIndex)->setMaxVoxelValue(*static_cast<double*>(mxGetPr(maxVal)));
    mxDestroyArray(minVal); minVal = NULL;
    mxDestroyArray(maxVal); maxVal = NULL;
    engEvalString(m_pEngine, "clear mevtmpminval mevtmpmaxval");
  }
  else
  {
    getOutImg(outIndex)->setOutOfDate();
    getOutImg(outIndex)->setStateInfo("Cannot set output size, because variable could not be found in Matlab workspace.",ML_BAD_DATA_TYPE);
  }
}

//----------------------------------------------------------------------------------
//! Request input image in fixed datatype.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::calcInSubImageProps(int /*inIndex*/, InSubImageProps &props, int /*outIndex*/)
{
  // Request input image in double type.
  props.inSubImgDType = MLdoubleType;
}

//----------------------------------------------------------------------------------
//! Returns the input image region required to calculate a region of an output image.
//----------------------------------------------------------------------------------
SubImgBox MatlabScriptWrapper::calcInSubImageBox (int /*inIndex*/, const SubImgBox &outSubImgBox, int /*outIndex*/)
{
  ML_TRACE_IN("MatlabScriptWrapper::calcInSubImageBox ()");

  return outSubImgBox;
}

//----------------------------------------------------------------------------------
//! Type specific page calculation.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::calcOutSubImage (SubImg *outSubImg,
                                           int outIndex,
                                           SubImg* /*inSubImgs*/)
{
  ML_TRACE_IN("MatlabScriptWrapper::calcOutSubImage ()");

  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "calcOutImageProps(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }

  // Get matlab image data.
  mxArray *m_Y  = engGetVariable(m_pEngine, (_outDataNameFld[outIndex]->getStringValue()).c_str());

  if ( (m_Y != NULL) )
  {
    // Copy images back from matlab.
    TSubImg<MLdouble> subImgBuf(outSubImg->getBox(), MLdoubleType, mxGetPr(m_Y));
    outSubImg->copySubImage(subImgBuf);

    mxDestroyArray(m_Y);
    m_Y = NULL;
  }
  else
  {
    // Throw error, if no data available.
    ML_PRINT_ERROR("MatlabScriptWrapper::calcOutSubImage()", ML_BAD_INPUT_IMAGE_POINTER, "Cannot copy from matlab data.");
  }
}

//////////////////////////////////////////////////////////////////////
// Internal (private) methods.
//////////////////////////////////////////////////////////////////////

//! Copy input image data to matlab.
void MatlabScriptWrapper::_copyInputImageDataToMatlab()
{
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputImageDataToMatlab(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }

  for(int i=0; i<3; i++)
  {
    // Get a valid input if possible. Dummy input is considered invalid.
    PagedImg *inImg = getUpdatedInImg(i);

    if(!(inImg == DummyOp::getGlobalInstance().getOutImg(i)))
    {
      // If we get an invalid pointer to the image we abort.
      if (!inImg) {
        ML_PRINT_ERROR("MatlabScriptWrapper::_copyInputImageDataToMatlab()", ML_BAD_INPUT_IMAGE_POINTER, "Could not find input image. Aborting.");
        return;
      }

      // Get input image size.
      Vector imgSize = inImg->getImgExt();

      // Declare data pointer here and set it to null. Data will then only
      // be allocated once for the first slice in the getTile() function below.
      void *data = NULL;

      // Get whole image.
      MLErrorCode localErr =
        getTile(getInOp(i),getInOpIndex(i),
        SubImgBox(Vector(0, 0, 0, 0, 0, 0),  Vector(imgSize.x-1, imgSize.y-1, imgSize.z-1, imgSize.c-1, imgSize.t-1, imgSize.u-1)),
        MLdoubleType,
        &data,
        ScaleShiftData(1,0));

      // Check and save error code if necessary.
      if (localErr != ML_RESULT_OK) {
        ML_PRINT_ERROR("MatlabScriptWrapper::copyInputImageDataToMatlab()", localErr, "Could not get input image tile. Aborting.");
        return;
      }

      // Need also to have storage for complete output image.
      //MLuint32 inDataSize = inImg->getBoxFromImgExt().getExt().getStrides().u;
      const MLuint32 inDataSize = imgSize.x*imgSize.y*imgSize.z*imgSize.c*imgSize.t*imgSize.u;

      // Set matlab image extent.
      const mwSize insizesArray[6] = {imgSize.x, imgSize.y, imgSize.z, imgSize.c, imgSize.t, imgSize.u};
      // Create numeric array
      mxArray *m_X = mxCreateNumericArray(6, insizesArray, mxDOUBLE_CLASS, mxREAL);

      // Copy data to matlab array.
      memcpy((void*)mxGetPr(m_X), data, inDataSize*sizeof(double));

      // Get input names from gui.
      std::string inputName = _inDataNameFld[i]->getStringValue();
      // Write data to matlab.
      engPutVariable(m_pEngine, inputName.c_str(), m_X);

      mxDestroyArray(m_X);

      // Free allocated memory for holding a slice.
      freeTile(data);
      data = NULL;
    }
  }
}

//! Copy input XMarkerList to matlab.
void MatlabScriptWrapper::_copyInputXMarkerToMatlab()
{
  // Proof if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputXMarkerToMatlab(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }

  // Internal loop.
  size_t i = 0;
  // Get names form gui.
  std::ostringstream inXMarkerStr, outXMarkerStr;
  inXMarkerStr << _inXMarkerNameFld->getStringValue(); 
  outXMarkerStr << _outXMarkerNameFld->getStringValue();

  // Combine string.
  std::ostringstream setInXMarkerStr, setOutXMarkerStr;
  setInXMarkerStr << inXMarkerStr.str() << "=struct('pos',[],'vec',[],'type',[])";
  setOutXMarkerStr << outXMarkerStr.str() << "=struct('pos',[],'vec',[],'type',[])";

  // Input XMarkerList transfered to matlab structure.
  engEvalString(m_pEngine, setInXMarkerStr.str().c_str());
  engEvalString(m_pEngine, setOutXMarkerStr.str().c_str());

  // Check if a valid XMarkerList is attached to the input.
  if( _inputXMarkerListFld->isValidValue() && ML_BASE_IS_A(_inputXMarkerListFld->getBaseValue(), XMarkerList)) 
  {
    // Get input list.
    XMarkerList inputXMarkerList = *((XMarkerList*)_inputXMarkerListFld->getBaseValue());

    // Strings to evaluate.
    std::ostringstream setPos, setVec, setType;
    setPos << inXMarkerStr.str() <<".pos=[";
    setVec << inXMarkerStr.str() << ".vec=[";
    setType << inXMarkerStr.str() << ".type=[";

    // Get XMarkerList size and go through all list step by step.
    const size_t listSize = inputXMarkerList.size();
    for(i = 0; i < listSize; i++) 
    {
      XMarker marker = inputXMarkerList[i];

      // Write pos, vec and type to strings.
      setPos<<std::dec<<marker.x()<<","<<std::dec<<marker.y()<<","<<std::dec<<marker.z()
        <<","<<std::dec<<marker.c()<<","<<std::dec<<marker.t()<<","<<std::dec<<marker.u()<<";";
      setVec<<std::dec<<marker.vx()<<","<<std::dec<<marker.vy()<<","<<std::dec<<marker.vz()<<";";
      setType<<std::dec<<marker.type<<";";
    }

    setPos<<"]";
    setVec<<"]";
    setType<<"]";
    // Put XMarkerList into matlab structure.
    engEvalString(m_pEngine, setPos.str().c_str());
    engEvalString(m_pEngine, setVec.str().c_str());
    engEvalString(m_pEngine, setType.str().c_str());
  }
}

//! Loads matlab script from a file, pastes it into script field and saves user written script.
bool MatlabScriptWrapper::_loadMatlabScriptFromFile(std::string& evaluateString)
{
  // Clear input string
  evaluateString.clear();

  // Temporary string for reading input.
  std::ostringstream tmpString;

  // Update script window if new script chosen.
  std::string pathString = _matlabScriptPathFld->getStringValue();

  ML_TRY
  {
    if(pathString.empty()) {
      _statusFld->setStringValue("Script path is empty.");
      return false;
    }
    else
    {
      // File to open.
      std::ifstream dat;

      dat.open(pathString.c_str());
      if(dat.fail()) {
        // Throw error message if file couldn't be opened.
        _statusFld->setStringValue("Cannot find .m-file!");
        return false;
      }

      // Read script line by line from file.
      std::string line;
      while(!dat.eof())
      {
        getline(dat, line);
        tmpString << line << "\n";
      }

      // Close file
      dat.close();

      // Get string
      evaluateString = tmpString.str();
    }
  }
  ML_CATCH_RETHROW;

  return true;
}

//! Gets XMarkerList from Matlab and copies results into output XMarkerList.
void MatlabScriptWrapper::_getXMarkerBackFromMatlab()
{
  // Clear _outputXMarkerList.
  _outputXMarkerList.clearList();

  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getXMarkerBackFromMatlab(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  size_t i=0, j=0;
  // Get names form gui.
  std::string outXMarkerStr = _outXMarkerNameFld->getStringValue();
  // Compose temp string to execute in matlab.
  std::ostringstream executeStr;
  // Internal temp variable will be used in matlab.
  executeStr << "tmpOutXMarkerListPos=" << outXMarkerStr << ".pos";
  engEvalString(m_pEngine, executeStr.str().c_str());
  // Get array from matlab.
  mxArray *m_pos = engGetVariable(m_pEngine, "tmpOutXMarkerListPos");
  // Delete temp data.
  engEvalString(m_pEngine, "clear tmpOutXMarkerListPos");
  // Clear temp string.
  executeStr.str("");

  executeStr << "tmpOutXMarkerListVec=" << outXMarkerStr << ".vec";
  engEvalString(m_pEngine, executeStr.str().c_str());
  mxArray *m_vec = engGetVariable(m_pEngine, "tmpOutXMarkerListVec");
  engEvalString(m_pEngine, "clear tmpOutXMarkerListVec");
  executeStr.str("");

  executeStr << "tmpOutXMarkerListType=" << outXMarkerStr << ".type";
  engEvalString(m_pEngine, executeStr.str().c_str());
  mxArray *m_type = engGetVariable(m_pEngine, "tmpOutXMarkerListType");
  engEvalString(m_pEngine, "clear tmpOutXMarkerListType");

  // Get data from matlab array.
  if(m_pos && !mxIsEmpty(m_pos))
  {
    double *dataPos = static_cast<double*>(mxGetPr(m_pos));
    double *dataVec = static_cast<double*>(mxGetPr(m_vec));
    double *dataType = static_cast<double*>(mxGetPr(m_type));

    // Copy matlab data to xmarker if it's not empty.
    if(dataPos!=NULL)
    {
      // Get rows numbers.
      const size_t rows = mxGetM(m_pos);
      const size_t cols = mxGetN(m_pos);

      for(i=0; i<rows; i++)
      {
        // Fill marker with zeros.
        XMarker outMarker(vec6(0),vec3(0),0);

        // Write matlab points to marker.
        for(j=0; j<cols && cols<=6; j++) {
          outMarker.pos[j] = dataPos[i + j*rows];
        }

        // Write matlab vector to marker and prevent writing more than 3 dimensions
        if(dataVec!=NULL) {
          if(rows==mxGetM(m_vec)) {
            for(j=0; j<mxGetN(m_vec) && mxGetN(m_vec)<=3; j++) {
              outMarker.vec[j] = dataVec[i + j*rows];
            }
          }
        }

        if(dataType!=NULL)
        {
          // Set type only if number points and types equal otherwise zero.
          if(rows==mxGetM(m_type))
          {
            // Write type to XMarker.
            outMarker.type = static_cast<MLint>(dataType[i]);
          }
        }

        // Append XMarker to XMarkerList.
        _outputXMarkerList.push_back(outMarker);
      }

      // Update local XMarkerList and set BaseValue of the output
      (XMarkerList*)_outputXMarkerListFld->getBaseValue();
      // Destroy arrays.
      mxDestroyArray(m_pos);
      mxDestroyArray(m_vec);
      mxDestroyArray(m_type);
    }
  }
}

//! Copies scalar values to matlab.
void MatlabScriptWrapper::_copyInputScalarsToMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputScalarsToMatlab(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  MLint i = 0;
  // Compose string that contains input scalars.
  std::ostringstream execute;
  // Put only input scalars into matlab.
  for(i=0; i<6; i++)
  {
    execute<<_scalarNameFld[i]->getStringValue()<<"="<<(_scalarFld[i]->getDoubleValue())<<"\n";
  }
  // Execute string and write input scalars into matlab.
  engEvalString(m_pEngine, execute.str().c_str());
}

//! Copies scalar values from matlab.
void MatlabScriptWrapper::_getScalarsBackFromMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getScalarsBackFromMatlab(): Cannot finding matlab engine!" << std::endl << std::flush;
    return;
  }

  // Internal loop.
  mxArray *temp = NULL;
  // Get only output scalars.
  for(MLint i=0; i<6; i++)
  {
    temp = engGetVariable(m_pEngine, (_scalarNameFld[i]->getStringValue()).c_str());
    if(temp!=NULL)
    {
      double *fieldVal = static_cast<double*>(mxGetPr(temp));
      _scalarFld[i]->setDoubleValue(fieldVal[0]);
    }
  }
  mxDestroyArray(temp);
  temp = NULL;
}

//! Check if Matlab is started.
bool MatlabScriptWrapper::_checkMatlabIsStarted()
{
  if(0 == engEvalString(m_pEngine, "mevTestIfMatlabIsRunning=3.14"))
  {
    engEvalString(m_pEngine, "clear mevTestIfMatlabIsRunning");
    return true;
  }
  else
  {
    return false;
  }
}

void MatlabScriptWrapper::_clearAllVariables()
{
  std::ostringstream clearString;
  clearString << "clear ";

  // Clear scalars
  for(int i=0; i<6; i++) {
    clearString << _scalarNameFld[i]->getStringValue() << " ";
  }

  // Clear input images
  for(int i=0; i<3; i++) {
    clearString << _inDataNameFld[i]->getStringValue() << " ";
  }

  // Clear output images
  for(int i=0; i<3; i++) {
    clearString << _outDataNameFld[i]->getStringValue() << " ";
  }

  // Clear XMarker data
  clearString << _outXMarkerNameFld->getStringValue();

  // Evaluate the string in Matlab
  engEvalString(m_pEngine, clearString.str().c_str());
}

ML_END_NAMESPACE
