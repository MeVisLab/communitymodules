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
// \author  Alexander Gryanik, Markus Harz, Ola Friman, Felix Ritter, Alexander Broersen, Jens Kaftan
// \date    2009-02-23
//
// Module for executing Matlab scripts in MeVisLab.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMatlabScriptWrapper.h"

// SDK includes
#if defined(MACOS)
# include <macBundle.h>
#endif

// System includes
#include <iostream>
#include <sstream>
#include <sys/stat.h>

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

  _isInWEMNotificationCB = false;
  ML_CHECK_NEW(_outWEM,WEM());
  (_inputWEMFld = fields->addBase("inputWEM"))->setBaseValue(NULL);
  (_outWEMFld = fields->addBase("outWEM0"))->setBaseValue(_outWEM);

  (_inputCurveFld = fields->addBase("inputCurve"))->setBaseValue(NULL);
  (_outputCurveListFld = fields->addBase("outputCurveList"))->setBaseValue(&_outputCurveList);

  //! Use Matlab commands in text field.
  (_matlabScriptFld = fields->addString("matlabScript"))->setStringValue("Output0=Input0 % Type your matlab script here.");

  //! Use external Matlab script.
  (_useExternalScriptFld = fields->addBool("useExternalScript"))->setBoolValue(false);
  //! Where will Matlab script be dumped.
  (_matlabScriptPathFld = fields->addString("matlabScriptPath"))->setStringValue("");
  //! Add open Matlab file trigger.
  _openMatlabFileFld = fields->addNotify("openMatlabFile");

  //! Set input and output data names used in Matlab.
  (_inDataNameFld[0] = fields->addString("inDataName0"))->setStringValue("Input0");
  (_inDataNameFld[1] = fields->addString("inDataName1"))->setStringValue("Input1");
  (_inDataNameFld[2] = fields->addString("inDataName2"))->setStringValue("Input2");
  (_outDataNameFld[0] = fields->addString("outDataName0"))->setStringValue("Output0");
  (_outDataNameFld[1] = fields->addString("outDataName1"))->setStringValue("Output1");
  (_outDataNameFld[2] = fields->addString("outDataName2"))->setStringValue("Output2");

  //! Init output subimg array.
  for(int i=0; i<3; i++) {
    _outDataArray[i] = NULL;
  }
  //! Set input and output XMarker names used in Matlab.
  (_inXMarkerNameFld = fields->addString("inXMarkerName"))->setStringValue("inXMarker");
  (_outXMarkerNameFld = fields->addString("outXMarkerName"))->setStringValue("outXMarker");
  //! Set input and output WEM names used in Matlab.
  (_inWEMNameFld = fields->addString("inWEMName"))->setStringValue("inWEM");
  (_outWEMNameFld = fields->addString("outWEMName"))->setStringValue("outWEM");
  //! Set input and output Curve names used in Matlab.
  (_inCurveNameFld = fields->addString("inCurveName"))->setStringValue("inCurve");
  (_outCurveNameFld = fields->addString("outCurveName"))->setStringValue("outCurve");

  //! Add toggle to delete user set variables before new calculation.
  //(_deleteMatlabVarFld = fields->addBool("delMatlabVar"))->setBoolValue(false);
  //! Add update button.
  _calculateFld = fields->addNotify("update");
  //! Use automatic update after change of an input.
  (_autoCalculationFld = fields->addBool("autoUpdate"))->setBoolValue(false);
  //! Use automatic apply after change of a parameter/field.
  (_autoApplyFld = fields->addBool("autoApply"))->setBoolValue(false);
  //! Share the Matlab environment.
  (_sharedUseFld = fields->addBool("sharedUse"))->setBoolValue(true);
  //! Add restart Matlab button.
  _restartMatlabFld = fields->addNotify("restartMatlab");
  //! Add status field.
  (_stdReportFld = fields->addString("status"))->setStringValue("Ready.");

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

  // Set string name and value fields.
  (_stringNameFld[0] = fields->addString("stringName0"))->setStringValue("string0");
  (_stringFld[0] = fields->addString("string0"))->setStringValue("");
  (_stringNameFld[1] = fields->addString("stringName1"))->setStringValue("string1");
  (_stringFld[1] = fields->addString("string1"))->setStringValue("");
  (_stringNameFld[2] = fields->addString("stringName2"))->setStringValue("string2");
  (_stringFld[2] = fields->addString("string2"))->setStringValue("");
  (_stringNameFld[3] = fields->addString("stringName3"))->setStringValue("string3");
  (_stringFld[3] = fields->addString("string3"))->setStringValue("");
  (_stringNameFld[4] = fields->addString("stringName4"))->setStringValue("string4");
  (_stringFld[4] = fields->addString("string4"))->setStringValue("");
  (_stringNameFld[5] = fields->addString("stringName5"))->setStringValue("string5");
  (_stringFld[5] = fields->addString("string5"))->setStringValue("");

  // Set vector name and value fields.
  (_vectorNameFld[0] = fields->addString("vectorName0"))->setStringValue("vector0");
  (_vectorFld[0] = fields->addVec4f("vector0"))->setStringValue("0 0 0 0");
  (_vectorNameFld[1] = fields->addString("vectorName1"))->setStringValue("vector1");
  (_vectorFld[1] = fields->addVec4f("vector1"))->setStringValue("0 0 0 0");
  (_vectorNameFld[2] = fields->addString("vectorName2"))->setStringValue("vector2");
  (_vectorFld[2] = fields->addVec4f("vector2"))->setStringValue("0 0 0 0");
  (_vectorNameFld[3] = fields->addString("vectorName3"))->setStringValue("vector3");
  (_vectorFld[3] = fields->addVec4f("vector3"))->setStringValue("0 0 0 0");
  (_vectorNameFld[4] = fields->addString("vectorName4"))->setStringValue("vector4");
  (_vectorFld[4] = fields->addVec4f("vector4"))->setStringValue("0 0 0 0");
  (_vectorNameFld[5] = fields->addString("vectorName5"))->setStringValue("vector5");
  (_vectorFld[5] = fields->addVec4f("vector5"))->setStringValue("0 0 0 0");

  // Set matrix name and value fields.
  (_matrixNameFld[0] = fields->addString("matrixName0"))->setStringValue("matrix0");
  (_matrixFld[0] = fields->addMatrix("matrix0"))->setStringValue("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
  (_matrixNameFld[1] = fields->addString("matrixName1"))->setStringValue("matrix1");
  (_matrixFld[1] = fields->addMatrix("matrix1"))->setStringValue("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
  (_matrixNameFld[2] = fields->addString("matrixName2"))->setStringValue("matrix2");
  (_matrixFld[2] = fields->addMatrix("matrix2"))->setStringValue("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");

  //! Add field to display Matlab output.
  (_matlabOutputBufferFld = fields->addString("matlabOutput"))->setStringValue("");

#if defined(MACOS)
  if (m_startCmd.empty()) {
    // Try to locate Matlab binary in PATH environment
    const char *pathEnv = getenv("PATH");
    if (pathEnv) {
      std::istringstream pathList(pathEnv);
      while (! pathList.eof()) {
        std::string path;
        std::getline(pathList, path, ':');
        path.append("/matlab");
        struct stat info;
        if (::stat(path.c_str(), &info) == 0) {
          m_startCmd = path;
        }
      }
    }
  }

  if (m_startCmd.empty()) {
    // Try to locate Matlab binary via its bundle id
    std::string matlabBundle = macx::Bundle::getBundleDirectory("com.mathworks.StartMATLAB");
    if (! matlabBundle.empty()) {
      std::string path = matlabBundle + "/bin/matlab";
      struct stat info;
      if (::stat(path.c_str(), &info) == 0) {
        m_startCmd = path;
      }
    }
  }

  if (! m_startCmd.empty()) {
    std::cout << "Found Matlab binary at: " << m_startCmd.c_str() << std::endl;
  }
#endif

  _startMatlab();

  if ( !_checkMatlabIsStarted() ) {
    std::cerr << "MatlabScriptWrapper::MatlabScriptWrapper():" << std::endl;
    std::cerr << "Error: MATLAB Engine not found. For this module to work, a MATLAB installation is required." << std::endl << std::endl;
    std::cerr << "Additional Hints: " << std::endl;
    std::cerr << " (1) On Windows, the MATLAB COM server must be registered. Execute " << std::endl;
    std::cerr << "       >> matlab /regserver " << std::endl;
    std::cerr << "     on a command line." << std::endl;
    std::cerr << " (2) On Mac OS X, it is currently required to set the environment variable" << std::endl;
    std::cerr << "     DYLD_LIBRARY_PATH to /Applications/MATLAB_R2010a.app/bin/maci64 before" << std::endl;
    std::cerr << "     MeVisLab is started." << std::endl;

    (_showSessionFld = fields->addBool("showSession"))->setBoolValue(false);
    _stdReportFld->setStringValue("Cannot find Matlab engine!");
  } else {
    //! Show the Matlab session window.
    bool vis;
    engGetVisible(m_pEngine, &vis);
    (_showSessionFld = fields->addBool("showSession"))->setBoolValue(vis);
  }

  //! Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
MatlabScriptWrapper::~MatlabScriptWrapper()
{
  ML_TRACE_IN("MatlabScriptWrapper::~MatlabScriptWrapper()");

  for(int i=0; i<3; i++) {
    if(_outDataArray[i] != NULL) {
        delete(_outDataArray[i]);
        _outDataArray[i] = NULL;
    }
  }

  if (m_pEngine != NULL) {
    engClose(m_pEngine);
  }

  _outWEM = NULL;
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::handleNotification (Field* field)
{
  ML_TRACE_IN("MatlabScriptWrapper::handleNotification()");

  if(field == _sharedUseFld) {
    return;
  }
  if(field == _restartMatlabFld) {
    if(_checkMatlabIsStarted() && _showSessionFld->isOn()) {
        _stdReportFld->setStringValue("Matlab is already started.");
    } else {
      // (Re)Start Matlab
      if(!_showSessionFld->isOn()) {
        // Force closing Matlab
        engEvalString(m_pEngine, "quit");
        if(engClose(m_pEngine)) {
          _stdReportFld->setStringValue("Successfully closed and");
        }
      }
      // Start Matlab
      _startMatlab();
      // If Matlab engine is started, make session window (in)visible.
      if(_checkMatlabIsStarted()) {
        engSetVisible(m_pEngine,_showSessionFld->getBoolValue());
        _stdReportFld->setStringValue(_stdReportFld->getStringValue()+" started.");
      } else {
        _stdReportFld->setStringValue("Cannot find Matlab engine!");
      }
    }
  }

  if(field == _openMatlabFileFld) {
    std::string pathString = _matlabScriptPathFld->getStringValue();
    ML_TRY {
      std::string openFileLink("opentoline('");
      if(pathString.compare(0,openFileLink.length(),openFileLink) == 0) {
        engEvalString(m_pEngine, pathString.c_str());
      } else {
        engEvalString(m_pEngine, (openFileLink+pathString+"',0,0)").c_str());
      }
    }
    ML_CATCH_RETHROW;
  }

  if( (field == _showSessionFld) && _checkMatlabIsStarted() ) {
    if(_showSessionFld->isOn()) {
      engSetVisible(m_pEngine, true);
    } else {
      engSetVisible(m_pEngine, false);
    }
  }

  // Update output on an update or if autoapply is enabled.
  if( (field == _calculateFld) ||
      (_autoCalculationFld->isOn() && ((field == getInField(0))||(field == getInField(1))||(field == getInField(2))||
                                       (field == _inputXMarkerListFld) || (field== _inputWEMFld) || (field== _inputCurveFld)) ) ||
           (_autoApplyFld->isOn()  && ((field == _scalarFld[0])||(field == _scalarFld[1])||(field == _scalarFld[2])||
                                       (field == _scalarFld[3])||(field == _scalarFld[4])||(field == _scalarFld[5])||
                                       (field == _vectorFld[0])||(field == _vectorFld[1])||(field == _vectorFld[2])||
                                       (field == _vectorFld[3])||(field == _vectorFld[4])||(field == _vectorFld[5])||
                                       (field == _matrixFld[0])||(field == _matrixFld[1])||(field == _matrixFld[2])||
                                       (field == _matrixFld[3])||(field == _matrixFld[4])||(field == _matrixFld[5])||
                                       (field == _stringFld[0])||(field == _stringFld[1])||(field == _stringFld[2])||
                                       (field == _stringFld[3])||(field == _stringFld[4])||(field == _stringFld[5])) )
    ) {
    _process();
  }
}


//----------------------------------------------------------------------------------
//! The process method is called by the parent class.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::_process()
{
  ML_TRACE_IN("MatlabScriptWrapper::process()")

  // Check if Matlab is started.
  if (!_checkMatlabIsStarted()) {
    _stdReportFld->setStringValue("Cannot find Matlab engine!");
    return;
  }

  // Get script to evaluate.
  std::string evaluateString = "";
  bool validScriptString = true;
  if(_useExternalScriptFld->isOn()) { // Get script from .m-file
    validScriptString = _loadMatlabScriptFromFile(evaluateString);
  } else {
    evaluateString = _matlabScriptFld->getStringValue();
  }

  // Execute Matlab script only when the string is valid
  if(validScriptString) {
    if (_inputCurveFld->getBaseValue() != NULL) {
      // Check if a valid CurveData or CurveList is attached to the input
      if (_inputCurveFld->isValidValue() && (ML_BASE_IS_A(_inputCurveFld->getBaseValue(),CurveData)||ML_BASE_IS_A(_inputCurveFld->getBaseValue(),CurveList)) ) {
        // Copy input CurveData or CurveList to Matlab.
        _copyInputCurveToMatlab();
      }
    }
    if( _inputXMarkerListFld->getBaseValue() != NULL ) {
      // Check if a valid XMarkerList is attached to the input.
      if( _inputXMarkerListFld->isValidValue() && ML_BASE_IS_A(_inputXMarkerListFld->getBaseValue(), XMarkerList) ) {
        // Copy input XMarkerList to Matlab.
        _copyInputXMarkerToMatlab();
      }
    }
    if (_inputWEMFld->getBaseValue() != NULL) {
      // Check if a valid WEM is attached to the input
      if (_inputWEMFld->isValidValue() && ML_BASE_IS_A(_inputWEMFld->getBaseValue(), WEM)) {
        // Copy input WEM to Matlab.
        _copyInputWEMToMatlab();
      }
    }

    // Copy input image data to matlab.
    _copyInputImageDataToMatlab();
    // Copy scalar values to matlab.
    _copyInputScalarsToMatlab();
    // Copy string values to matlab.
    _copyInputStringsToMatlab();
    // Copy vector values to matlab.
    _copyInputVectorsToMatlab();
    // Copy matrix values to matlab.
    _copyInputMatricesToMatlab();

    _stdReportFld->setStringValue("Matlab script is executing....");
    // Insert at the end of the script variable to proof execution status
    // and run the script in Matlab
    evaluateString += "\nmevmatscr=1;";	// Added ';' to prevent unnecessary output

    // Buffer to capture Matlab output
    #define BUFSIZE 5120
    char buffer[BUFSIZE+1];
    buffer[BUFSIZE] = '\0';
    // Start logging Matlab output
    engOutputBuffer(m_pEngine, buffer, BUFSIZE);

    // and run the script in Matlab
    engEvalString(m_pEngine, evaluateString.c_str());

    // Stop logging Matlab output
    engOutputBuffer(m_pEngine, NULL, 0);
    // Use rich text format in output for visibility
    std::string output("<PRE>");
    output.append(buffer);
    _matlabOutputBufferFld->setStringValue(output);

    // If variable mevmatscr exist it means the whole Matlab script was executed.
    mxArray *mtmp = engGetVariable(m_pEngine,"mevmatscr");
    if(mtmp!=NULL) {
      _stdReportFld->setStringValue("Matlab execution successful!");
      engEvalString(m_pEngine, "clear mevmatscr");
    } else {
      _stdReportFld->setStringValue("Matlab script contains errors!");
      //_clearAllVariables();
    }
    mxDestroyArray(mtmp); mtmp = NULL;
  }
  // If the script string was not valid, clear all data so that
  // the user notes this.
  else {
    _matlabOutputBufferFld->setStringValue("");
    _clearAllVariables();
  }

  // Get CurveList from Matlab and copy results into output CurveList
  _getCurveDataBackFromMatlab();

  // Get XMarkerList from Matlab and copy results into output XMarkerList
  _getXMarkerBackFromMatlab();

  // Get WEM from Matlab and copy results into output WEM
  _getWEMBackFromMatlab();

  // Get scalars back from Matlab. First store the current scalars so that
  // we can check if they change. A notification is only sent upon change.
  double tmpScalars[6];
  for(int k=0; k<6; ++k) {
    tmpScalars[k] = _scalarFld[k]->getDoubleValue();
  }
  _getScalarsBackFromMatlab();
  for(int k=0; k<6; ++k) {
    if(tmpScalars[k] != _scalarFld[k]->getDoubleValue()) {
      _scalarFld[k]->notifyAttachments();
    }
  }
  // Get strings back from Matlab. First store the current strings so that
  // we can check if they change. A notification is only sent upon change.
  std::string tmpstrings[6];
  for(int k=0; k<6; ++k) {
    tmpstrings[k] = _stringFld[k]->getStringValue();
  }
  _getStringsBackFromMatlab();
  for(int k=0; k<6; ++k) {
    if(tmpstrings[k] != _stringFld[k]->getStringValue()) {
      _stringFld[k]->notifyAttachments();
    }
  }

  // Get vectors back from Matlab. First store the current vectors so that
  // we can check if they change. A notification is only sent upon change.
  for(int k=0; k<6; ++k) {
    tmpstrings[k] = _vectorFld[k]->getStringValue();
  }
  _getVectorsBackFromMatlab();
  for(int k=0; k<6; ++k) {
    if(tmpstrings[k] != _vectorFld[k]->getStringValue()) {
      _vectorFld[k]->notifyAttachments();
    }
  }

  // Get matrices back from Matlab. First store the current matrices so that
  // we can check if they change. A notification is only sent upon change.
  for(int k=0; k<3; ++k) {
    tmpstrings[k] = _matrixFld[k]->getStringValue();
  }
  _getMatricesBackFromMatlab();
  for(int k=0; k<3; ++k) {
    if(tmpstrings[k] != _matrixFld[k]->getStringValue()) {
      _matrixFld[k]->notifyAttachments();
    }
  }

  // Notify image attachments that are new images calculated so that they
  // update themselves and call the calcOutSubImage()
  _getOutputImageDataBackFromMatlab();

  getOutField(0)->notifyAttachments();
  getOutField(1)->notifyAttachments();
  getOutField(2)->notifyAttachments();

  // Notify the XMarkerList output
  _outputXMarkerListFld->notifyAttachments();
  // Notify the CurveList output
  _outputCurveListFld->notifyAttachments();

  // Notify the WEM output
  std::vector<WEMEventContainer>ecList;
  WEMEventContainer ec; 
  ec.notificationType = WEM_NOTIFICATION_FINISHED  |
                        WEM_NOTIFICATION_SELECTION |
                        WEM_NOTIFICATION_REPAINT;
  ecList.push_back(ec);

  _outWEM->notifyObservers(ecList);
}


//----------------------------------------------------------------------------------
//! Configures (in)validation handling of inputs which are not connected or up to date.
//----------------------------------------------------------------------------------
BaseOp::INPUT_HANDLE MatlabScriptWrapper::handleInput (int inIndex, INPUT_STATE /*state*/) const
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

#if ML_MAJOR_VERSION >= 2
# define __setInSubImageDataType(__t)  for(int __i=0;__i<3;++__i) {getOutImg(outIndex)->setInSubImageDataType(__i,__t);}
#else
# define __setInSubImageDataType(__t)
#endif

//----------------------------------------------------------------------------------
//! Sets properties of the output image at output \c outIndex.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::calcOutImageProps (int outIndex)
{
  ML_TRACE_IN("MatlabScriptWrapper::calcOutImageProps ()");

  // Proof if Matlab is started.
  if(!_checkMatlabIsStarted()) {
    std::cerr << "calcOutImageProps(): Cannot find Matlab engine!" << std::endl << std::flush;
#if ML_MAJOR_VERSION >= 2
    getOutImg(outIndex)->setOutOfDate();
    getOutImg(outIndex)->setStateInfo("Cannot find Matlab engine!", ML_BAD_DATA_TYPE);
#endif
    return;
  }

  // If we can find the variable and calculate its size, go on.
  if((_outDataArray[outIndex] != NULL)) {
    Vector outExt = _outDataArray[outIndex]->GetExtend();

    // Set page size.
    getOutImg(outIndex)->setPageExt(outExt);
    // Set output image size.
    getOutImg(outIndex)->setImgExt(outExt);
    // Set output image datatype.
    getOutImg(outIndex)->setDataType( _outDataArray[outIndex]->GetDataType() ); __setInSubImageDataType(MLdoubleType);

    // Get min and max values in Matlab workspace and set them in MeVisLab
    getOutImg(outIndex)->setMinVoxelValue( _outDataArray[outIndex]->GetMinValue() );
    getOutImg(outIndex)->setMaxVoxelValue( _outDataArray[outIndex]->GetMaxValue() );
  } // if((_outDataArray[outIndex] != NULL))
  else {
    getOutImg(outIndex)->setOutOfDate();
    getOutImg(outIndex)->setStateInfo("Cannot set output size, because variable could not be found in Matlab workspace.",ML_BAD_DATA_TYPE);
  }
}

#if ML_MAJOR_VERSION < 2
//----------------------------------------------------------------------------------
//! Request input image in basic datatype.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::calcInSubImageProps (int inIndex, InSubImageProps &props, int /*outIndex*/)
{
  PagedImg *inImg = getNonDummyUpdatedInImg(inIndex);
  if(inImg != NULL) {
    props.inSubImgDType = inImg->getDataType();
  } else {
    // Request input image in double type.
    props.inSubImgDType = MLdoubleType;
  }
}
#endif

//----------------------------------------------------------------------------------
//! Returns the input image region required to calculate a region of an output image.
//----------------------------------------------------------------------------------
SubImgBox MatlabScriptWrapper::calcInSubImageBox (int /*inIndex*/, const SubImgBox &outSubImgBox, int /*outIndex*/)
{
  ML_TRACE_IN("MatlabScriptWrapper::calcInSubImageBox ()");

  return SubImgBox(Vector(0), Vector(-1));
}

//----------------------------------------------------------------------------------
//! Type specific page calculation.
//----------------------------------------------------------------------------------
void MatlabScriptWrapper::calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg* /*inSubImgs*/)
{
  ML_TRACE_IN("MatlabScriptWrapper::calcOutSubImage ()");

  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "calcOutImageProps(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  if ( (_outDataArray[outIndex] != NULL) ) {
    SubImg subImgBuf(SubImgBox(_outDataArray[outIndex]->GetExtend()), _outDataArray[outIndex]->GetDataType(), _outDataArray[outIndex]->GetData());
    outSubImg->copySubImage(subImgBuf);
  } else {
    // Throw error if the variable could not be found in the Matlab workspace.
    outSubImg->fillSubImg(0.0);
    //ML_PRINT_ERROR("MatlabScriptWrapper::calcOutSubImage()", ML_BAD_INPUT_IMAGE_POINTER, "Cannot copy from Matlab data.");
  }
}

//////////////////////////////////////////////////////////////////////
// Internal (private) methods.
//////////////////////////////////////////////////////////////////////


//! Callback the input WEM is changed.
void MatlabScriptWrapper::_wemNeedsNotificationCB( void* userData, std::vector<WEMEventContainer> ecList )
{
  MatlabScriptWrapper* thisp = static_cast<MatlabScriptWrapper*>(userData);

  thisp->_isInWEMNotificationCB = true;
  WEMEventContainer wemEC = ecList.back();
  if(wemEC.notificationType & WEM_NOTIFICATION_FINISHED) {
    if(thisp->_autoCalculationFld->isOn()) {
      thisp->handleNotificationOff();
      thisp->_process();
      thisp->handleNotificationOn();
    }
  }
  thisp->_isInWEMNotificationCB = false;
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

  ML_TRY {
    if(pathString.empty()) {
      _stdReportFld->setStringValue("Script path is empty.");
      return false;
    }
    else
    {
      // File to open.
      std::ifstream dat;
      dat.open(pathString.c_str());
      if(dat.fail()) {
        // Throw error message if file couldn't be opened.
        _stdReportFld->setStringValue("Cannot find .m-file!");
        return false;
      }

      // Read script line by line from file.
      std::string line;
      while(!dat.eof()) {
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

//! Start the Matlab COM server session.
void MatlabScriptWrapper::_startMatlab()
{
  int returnStatus = 0;
  if(_sharedUseFld->isOn()) {
    m_pEngine = engOpen(          (m_startCmd.empty()) ? NULL : m_startCmd.c_str() );
  } else {
    //  0 = success, -2 = error: second argument must be NULL, -3 = error: engOpenSingleUse failed
    m_pEngine = engOpenSingleUse( (m_startCmd.empty()) ? NULL : m_startCmd.c_str(), NULL, &returnStatus );
    if(!returnStatus) {
      _stdReportFld->setStringValue("Unshared Matlab");
    }
  }
}

//! Check if Matlab is started.
bool MatlabScriptWrapper::_checkMatlabIsStarted()
{
  // Is there an engine at all?
  if(m_pEngine == NULL) {
    return false;
  }

  // Check that it is working properly
  if(0 == engEvalString(m_pEngine, "mevTestIfMatlabIsRunning=3.14"))
  {
    engEvalString(m_pEngine, "clear mevTestIfMatlabIsRunning");
    return true;
  } else {
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

  // Clear strings
  for(int i=0; i<6; i++) {
    clearString << _stringNameFld[i]->getStringValue() << " ";
  }

  // Clear vectors
  for(int i=0; i<6; i++) {
    clearString << _vectorNameFld[i]->getStringValue() << " ";
  }

  // Clear matrices
  for(int i=0; i<3; i++) {
    clearString << _matrixNameFld[i]->getStringValue() << " ";
  }

  // Clear input images
  for(int i=0; i<3; i++) {
    clearString << _inDataNameFld[i]->getStringValue() << " ";
  }

  // Clear output images
  for(int i=0; i<3; i++) {
    clearString << _outDataNameFld[i]->getStringValue() << " ";
  }

  // Clear input XMarker data
  clearString << _inXMarkerNameFld->getStringValue() << " ";

  // Clear output XMarker data
  clearString << _outXMarkerNameFld->getStringValue() << " ";
  
  // Clear input WEM data
  clearString << _inWEMNameFld->getStringValue() << " ";
  
  // Clear output WEM data
  clearString << _outWEMNameFld->getStringValue();

  // Evaluate the string in Matlab
  engEvalString(m_pEngine, clearString.str().c_str());
}

//! Copy input image data to Matlab.
void MatlabScriptWrapper::_copyInputImageDataToMatlab()
{
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputImageDataToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  for(int i=0; i<3; i++) {
    // Get a valid input if possible. Dummy input is considered invalid.
    PagedImg *inImg = getUpdatedInImg(i);

    if(!(inImg == DummyOp::getGlobalInstance().getOutImg(0)))
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
        getTile(getInOp(i),
                getInOpIndex(i),
                SubImgBox(Vector(0, 0, 0, 0, 0, 0),
                          Vector(imgSize.x-1, imgSize.y-1, imgSize.z-1,imgSize.c-1, imgSize.t-1, imgSize.u-1)),
                inImg->getDataType(),
                &data,
                ScaleShiftData(1, 0));

      // Check and save error code if necessary.
      if (localErr != ML_RESULT_OK) {
        ML_PRINT_ERROR("MatlabScriptWrapper::copyInputImageDataToMatlab()", localErr, "Could not get input image tile. Aborting.");
        return;
      }

      // Need also to have storage for complete output image.
      const MLuint32 inDataSize = imgSize.x*imgSize.y*imgSize.z*imgSize.c*imgSize.t*imgSize.u;

      // Set Matlab image extent.
      const mwSize insizesArray[6] = {imgSize.x, imgSize.y, imgSize.z, imgSize.c, imgSize.t, imgSize.u};

      // Copy different types of images from MeVisLab.
      mxClassID inputClass;
      int elementSize;
      switch (inImg->getDataType()) {
        case MLdoubleType: inputClass = mxDOUBLE_CLASS; elementSize = sizeof(double);   break;
        case MLfloatType:  inputClass = mxSINGLE_CLASS; elementSize = sizeof(float);    break;
        case MLint8Type:   inputClass = mxINT8_CLASS;   elementSize = sizeof(int8_T);   break;
        case MLuint8Type:  inputClass = mxUINT8_CLASS;  elementSize = sizeof(uint8_T);  break;
        case MLint16Type:  inputClass = mxINT16_CLASS;  elementSize = sizeof(int16_T);  break;
        case MLuint16Type: inputClass = mxUINT16_CLASS; elementSize = sizeof(uint16_T); break;
        case MLint32Type:  inputClass = mxINT32_CLASS;  elementSize = sizeof(int32_T);  break;
        case MLuint32Type: inputClass = mxUINT32_CLASS; elementSize = sizeof(uint32_T); break;
        case MLint64Type:  inputClass = mxINT64_CLASS;  elementSize = sizeof(int64_T);  break;
        default:
          inputClass = mxDOUBLE_CLASS;
          elementSize = sizeof(double);
          std::cerr << "_copyInputImageDataToMatlab(): Output type from MeVisLab not supported" << std::endl << std::flush;
      }

      // Create numeric array
      mxArray *m_pImage = mxCreateNumericArray(6, insizesArray, inputClass, mxREAL);

      // Copy data to Matlab array.
      memcpy((void*)mxGetPr(m_pImage), data, inDataSize*elementSize);

      // Get input names from GUI.
      std::string inputName = _inDataNameFld[i]->getStringValue();
      // Write data to Matlab.
      engPutVariable(m_pEngine, inputName.c_str(), m_pImage);

      mxDestroyArray(m_pImage); m_pImage = NULL;

      // Free allocated memory for holding a slice.
      freeTile(data); data = NULL;
    }
  }
}

void MatlabScriptWrapper::_getOutputImageDataBackFromMatlab()
{
  for(int i=0; i<3; i++) {
    if(_outDataArray[i] != NULL) {
      delete(_outDataArray[i]);
      _outDataArray[i] = NULL;
    }
  }

  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getOutputImageDataBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  for(int outIndex=0; outIndex<3; outIndex++) {
    // Get Matlab image data.
    std::string outname = _outDataNameFld[outIndex]->getStringValue();
    mxArray *m_pImage = engGetVariable(m_pEngine, outname.c_str());

    if ( (m_pImage != NULL) ) {
      const mwSize m_numDims = mxGetNumberOfDimensions(m_pImage);
      if(m_numDims>6) {
        std::cerr << "_getOutputImageDataBackFromMatlab(): Too many dimensions in Matlab image!" << std::endl << std::flush;
        return;
      }
      Vector outExt = Vector(1,1,1,1,1,1);
      for (size_t i=0; i<m_numDims; i++) {
        outExt[i] = static_cast<MLint>(mxGetDimensions(m_pImage)[i]);
      }

      // Copy different types of images from Matlab.
      MLPhysicalDataType outputClass;
      switch (mxGetClassID(m_pImage)) {
        case mxDOUBLE_CLASS: outputClass = MLdoubleType; break;
        case mxSINGLE_CLASS: outputClass = MLfloatType;  break;
        case mxINT8_CLASS:   outputClass = MLint8Type;   break;
        case mxUINT8_CLASS:  outputClass = MLuint8Type;  break;
        case mxINT16_CLASS:  outputClass = MLint16Type;  break;
        case mxUINT16_CLASS: outputClass = MLuint16Type; break;
        case mxINT32_CLASS:  outputClass = MLint32Type;  break;
        case mxUINT32_CLASS: outputClass = MLuint32Type; break;
        default:
          outputClass = ML_BAD_DATA_TYPE;
          std::cerr << "_getOutputImageDataBackFromMatlab(): Output type from Matlab not supported" << std::endl << std::flush;
      }

      _outDataArray[outIndex] = new MatlabScriptWrapperOutputData();
      _outDataArray[outIndex]->SetData( outExt, outputClass, mxGetPr(m_pImage) );

      mxDestroyArray(m_pImage);
      m_pImage = NULL;

      // Get min and max values in Matlab workspace and set them in MeVisLab
      std::ostringstream minmaxCommand;
      minmaxCommand << "mevtmpminval = min(" << outname << "(:));" << "mevtmpmaxval = max(" << outname << "(:));";
      engEvalString(m_pEngine, minmaxCommand.str().c_str());
      mxArray *minVal = engGetVariable(m_pEngine, "mevtmpminval");
      mxArray *maxVal = engGetVariable(m_pEngine, "mevtmpmaxval");
      // if min and max are not defined, set default values
      if ((minVal==NULL) || (minVal==NULL)) {
        _outDataArray[outIndex]->SetMinMaxValue(0,127);
      } else {
        _outDataArray[outIndex]->SetMinMaxValue(mxGetScalar(minVal), mxGetScalar(maxVal));
      }
      mxDestroyArray(minVal); minVal = NULL;
      mxDestroyArray(maxVal); maxVal = NULL;
      engEvalString(m_pEngine, "clear mevtmpminval mevtmpmaxval");
    } // if ( (m_pImage != NULL) )
    else
    {
      // Throw error, if no data available.
      // ML_PRINT_ERROR("MatlabScriptWrapper::calcOutSubImage()", ML_BAD_INPUT_IMAGE_POINTER, "Cannot copy from Matlab data.");
    }
  } // for outindex
}

//! Copy input CurveData or CurveList to Matlab.
void MatlabScriptWrapper::_copyInputCurveToMatlab()
{

  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputCurveToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  CurveData* inputCurveData = NULL;
  CurveList* inputCurveList = NULL;
  std::size_t numCurves = 0;

  // Get input data.
  if( ML_BASE_IS_A(_inputCurveFld->getBaseValue(), CurveList) ) {
    inputCurveList = mlbase_cast<CurveList*>(_inputCurveFld->getBaseValue());
    numCurves = inputCurveList->getNumCurves();
    if(numCurves) {
      inputCurveData = inputCurveList->getCurveData(0);
    }
  } else {
    inputCurveData = mlbase_cast<CurveData*>(_inputCurveFld->getBaseValue());
    numCurves = 1;
  }

  mxArray *cellArray = mxCreateCellMatrix(1,numCurves);
  mxArray *matArray;
  for(size_t i=0; i<numCurves; i++) {
    const std::size_t numPoints = inputCurveData->getPoints();
    const std::size_t numSeries = inputCurveData->getNumSeries();
    const mwSize dims[2] = {numPoints,numSeries+1};

    // Create matrix with series and points for each curve-data in a cell
    matArray = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
    // If both arrays are created
    if(cellArray && matArray){
      double *arrayPtr = (double *)mxGetData(matArray);
      for(std::size_t k=0; k<numPoints; ++k) {
        // The X - values
        arrayPtr[k] = inputCurveData->getXValue(k);
      }
      for(std::size_t j=0; j<numSeries; ++j) {
        for(std::size_t k=0; k<numPoints; ++k) {
          // The Y - values for each series
          arrayPtr[k+numPoints*(j+1)] = inputCurveData->getYValue(j,k);
        }
      }
      // Put array in a cell
      mxSetCell(cellArray, i, mxDuplicateArray(matArray));
      if(inputCurveList) {
        inputCurveData = inputCurveList->getCurveData(i+1);
      }
    }
    mxDestroyArray(matArray); matArray = NULL;
  }

  // Write CurveData or CurveList into Matlab cell array with input name from GUI.
  engPutVariable(m_pEngine, _inCurveNameFld->getStringValue().c_str(), cellArray);

  mxDestroyArray(cellArray); cellArray = NULL;
}

//! Gets structure from Matlab and copies results into output CurveData.
void MatlabScriptWrapper::_getCurveDataBackFromMatlab()
{
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getCurveDataBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  // Clear _outputCurveData and preserve curve properties.
  _outputCurveList.clear();

  // Get name from GUI.
  std::string outCurveStr = _outCurveNameFld->getStringValue();
  // Internal temp variable with the number of Curves in Matlab.
  mxArray *m_curveList = engGetVariable(m_pEngine, outCurveStr.c_str());

  if((m_curveList!=NULL && mxGetClassID(m_curveList)==mxCELL_CLASS)) {
    CurveData* curve = NULL;
    const size_t curves = mxGetN(m_curveList);  // Number of matrices in Matlab cell
    for(size_t i=0; i<curves; i++) {
      ML_CHECK_NEW(curve,CurveData);

      mxArray *m_curve = mxGetCell(m_curveList,i);
      if(m_curve && !mxIsEmpty(m_curve) && mxGetClassID(m_curve)==mxDOUBLE_CLASS) {
        const size_t points = mxGetM(m_curve);  // Number of rows in Matlab matrix
        const size_t series = mxGetN(m_curve);  // Number of columns in Matlab matrix
        curve->resizeX(points);                 // Initialize the X - values with zeros
        // Initialize the remaining columns with zeros as series of Y - values
        for(size_t serie=0; serie<series-1; serie++) {
          curve->resizeY(serie,points);
        }
        double *dataPoints = static_cast<double*>(mxGetPr(m_curve));
        // For each point.
        for(size_t point=0; point<points; point++) {
          // Set the X - values
          curve->setXValue(dataPoints[point], point);
        }
        // For each serie
        for(size_t serie=0; serie<series-1; serie++) {
          for(size_t point=0; point<points; point++) {
            // Set the Y - values
            curve->setYValue(dataPoints[point+points*(serie+1)], serie, point);
          }
        }
      }
      // Append CurveData to CurvList.
      _outputCurveList.getCurveList().push_back(curve);
    }
  }
  mxDestroyArray(m_curveList); m_curveList = NULL;
}

//! Copy input XMarkerList to Matlab.
void MatlabScriptWrapper::_copyInputXMarkerToMatlab()
{
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputXMarkerToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  // Get input list.
  XMarkerList inputXMarkerList = *((XMarkerList*)_inputXMarkerListFld->getBaseValue());

  // Internal loop.
  size_t i = 0;

  // Get input list name from GUI.
  std::string inXMarkerStr = _inXMarkerNameFld->getStringValue();

  // Strings to evaluate.
  std::ostringstream setPos, setVec, setType;
  setPos << inXMarkerStr.c_str() <<".pos=[";
  setVec << inXMarkerStr.c_str() << ".vec=[";
  setType << inXMarkerStr.c_str() << ".type=[";

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
  
  // Put XMarkerList into Matlab structure.
  std::ostringstream all;
  all << setPos.str() << "\n" << setVec.str() << "\n" << setType.str() << "\n";

  mxArray *m_X = mxCreateString(all.str().c_str());
  if (m_X) {
    if(engPutVariable(m_pEngine, "copyInputXMarkerToMatlab", m_X) == 0) {
      engEvalString(m_pEngine, "eval(copyInputXMarkerToMatlab); clear copyInputXMarkerToMatlab;");
    }
  }
  mxDestroyArray(m_X); m_X = NULL;
}

//! Gets structure from Matlab and copies results into output XMarkerList.
void MatlabScriptWrapper::_getXMarkerBackFromMatlab()
{
  // Clear _outputXMarkerList.
  _outputXMarkerList.clearList();

  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getXMarkerBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  size_t i=0, j=0;
  // Get names from GUI.
  std::string outXMarkerStr = _outXMarkerNameFld->getStringValue();
  // Compose temp string to execute in Matlab.
  std::ostringstream executeStr;
  // Internal temp variable will be used in Matlab.
  executeStr << "tmpOutXMarkerListPos=" << outXMarkerStr << ".pos";
  engEvalString(m_pEngine, executeStr.str().c_str());
  // Get array from Matlab.
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
  executeStr.str("");

  // Get data from Matlab array in which the positions have to be present.
  if(m_pos && !mxIsEmpty(m_pos) && mxGetClassID(m_pos)==mxDOUBLE_CLASS) {
    double *dataPos = static_cast<double*>(mxGetPr(m_pos));

    // Copy Matlab data to XMarker if it's not empty.
    if(dataPos!=NULL) {
      // Get rows numbers.
      const size_t rows = mxGetM(m_pos);    // Number of markers
      const size_t cols = mxGetN(m_pos);    // Number of dimensions (max 6)

      for(i=0; i<rows; i++) {
        // Fill marker with zeros.
        XMarker outMarker(vec3(0));

        // Write Matlab points to marker and prevent writing more than 6 dimensions.
        for(j=0; j<cols && cols<=6; j++) {
          outMarker.pos[j] = dataPos[i + j*rows];
        }
        // Optionally, get vector data from Matlab array
        if(m_vec && !mxIsEmpty(m_vec) && mxGetClassID(m_vec)==mxDOUBLE_CLASS) {
          double *dataVec = static_cast<double*>(mxGetPr(m_vec));
          // Write Matlab vector to marker only if number of vectors are equal to points.
          if(dataVec!=NULL && rows==mxGetM(m_vec)) {
            // Prevent writing more than 3 dimensions.
            for(j=0; j<mxGetN(m_vec) && mxGetN(m_vec)<=3; j++) {
              outMarker.vec[j] = dataVec[i + j*rows];
            }
          }
        }
        // Optionally, get type data from Matlab array
        if(m_type && !mxIsEmpty(m_type) && mxGetClassID(m_type)==mxDOUBLE_CLASS) {
          double *dataType = static_cast<double*>(mxGetPr(m_type));
          // Write Matlab type vec to marker only if number of vectors are equal to points.
          if(dataType!=NULL && rows==mxGetM(m_type)) {
            outMarker.type = static_cast<MLint>(dataType[i]);
          }
        }

        // Append XMarker to XMarkerList with new Id.
        outMarker.setId(_outputXMarkerList.newId());
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

//! Copy input WEM to Matlab.
void MatlabScriptWrapper::_copyInputWEMToMatlab()
{
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputWEMToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

    if(!_isInWEMNotificationCB){
    WEM::removeNotificationObserverFromAllWEMs(_wemNeedsNotificationCB, this);
  }
  // Get input WEM.
  WEM *inputWEM = mlbase_cast<WEM*>(_inputWEMFld->getBaseValue());
  if(!_isInWEMNotificationCB) {
    inputWEM->addNotificationObserver(_wemNeedsNotificationCB, this);
  }

  // Internal loop.
  unsigned int i = 0, j = 0, k = 0, m = 0;
  unsigned int totalNumNodes = 0, numTriangulatedNodes = 0;
  WEMPatch *patch = NULL;
  WEMNode *node = NULL;
  WEMFace *face = NULL;
  Vector3 position = NULL_VEC;
  Vector3 normal = NULL_VEC;
  unsigned int entryNumber = 0;
  
  // Get input WEM name from GUI.
  std::string inWEMStr = _inWEMNameFld->getStringValue();
  
  // Strings to evaluate.
  std::ostringstream setNodes, setFaces, setNormals, setLUT;
  setNodes << inWEMStr.c_str() << "{1}.Vertices=[";
  setFaces << inWEMStr.c_str() << "{1}.Faces=[";
  setNormals << inWEMStr.c_str() << "{1}.VertexNormals=[";
  setLUT << inWEMStr.c_str() << "{1}.FaceVertexCData=[";
  
  // Loop over all patches -> flatten WEM
  for (i = 0; i < inputWEM->getNumWEMPatches(); i ++) {
    patch = inputWEM->getWEMPatchAt(i);

    // Loop over all nodes
    const unsigned int numNodes = patch->getNumNodes();
    for (j = 0; j < numNodes; j ++) {
      node = patch->getNodeAt(j);
      position = node->getPosition();
      setNodes << std::dec << position[0] << "," << std::dec << position[1] << "," << std::dec << position[2] << ";";
    }
    numTriangulatedNodes = 0;
    
    // Loop over all faces
    const unsigned int numFaces = patch->getNumFaces();
    for (j = 0; j < numFaces; j ++) {
      face = patch->getFaceAt(j);
      const unsigned int numFaceNodes = face->getNumNodes();
      if (numFaceNodes == 3) {
        for (k = 0; k < 3; k ++) {
          entryNumber = totalNumNodes + face->getNodeAt(k)->getEntryNumber();
          setFaces << entryNumber << ",";
        }
        setFaces << ";";
      } else {
        position = face->getCentroid();
        setNodes << std::dec << position[0] << "," << std::dec << position[1] << "," << std::dec << position[2] << ";";
        numTriangulatedNodes ++;
        
        for (k = 0; k < numFaceNodes; k ++) {
          for (m = 0; m < 2; m ++) {
            entryNumber = totalNumNodes + face->getNodeAt((m + k) % numFaceNodes)->getEntryNumber();
            setFaces << entryNumber << ",";
          }
          setFaces << totalNumNodes + node->getEntryNumber();
          setFaces << ";";
        }
        
      }
      normal = face->getNormal();
      setNormals << std::dec << normal[0] << "," << std::dec << normal[1] << "," << std::dec << normal[2] << ";";
    }
    
    // Get LUT primitive value list
    WEMPrimitiveValueList *pvl = NULL;
    pvl = patch->getPrimitiveValueList("LUT");
    if (pvl->isValid()) {
      const unsigned int numValues = pvl->getNumValues();
      for (j = 0; j < numValues; j ++) {
        setLUT << pvl->getValue(j) << ";";
      }
    }
    
    totalNumNodes += numNodes + numTriangulatedNodes;
  }
  
  setNodes << "]";
  setFaces << "]+1";
  setNormals << "]";
  setLUT << "]";
  
  std::ostringstream all;
  all << setNodes.str() << "\n" << setFaces.str() << "\n" << setNormals.str() << "\n" << setLUT.str() << "\n";
  
  mxArray *m_X = mxCreateString(all.str().c_str());
  if (m_X) {
    if (engPutVariable(m_pEngine, "copyInputWEMToMatlab", m_X) == 0) {
      engEvalString(m_pEngine, "eval(copyInputWEMToMatlab); clear copyInputWEMToMatlab;");
    }
  }
  mxDestroyArray(m_X); m_X = NULL;
}

//! Gets structure from Matlab and copies results into output WEM.
void MatlabScriptWrapper::_getWEMBackFromMatlab()
{
  // Clear _outWEM.
  _outWEM->removeAll();
  
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getWEMBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }
  
  // Get names from GUI.
  std::string outWEMStr = _outWEMNameFld->getStringValue();
  
  // Variables
  WEMTrianglePatch *triPatch = NULL;
  WEMNode *node = NULL;
  WEMTriangle *triangle = NULL;
  unsigned int i = 0;
  
  // Internal temp variable with the number of patches in Matlab.
  mxArray *m_patchList = engGetVariable(m_pEngine, outWEMStr.c_str());
  if(m_patchList!=NULL) {
    const size_t patches = mxGetN(m_patchList); // Number of patches in Matlab variable
    for(size_t j=1; j<=patches; j++) {
      // Compose temp string to execute in Matlab.
      std::ostringstream executeStr;

      // Get nodes
      executeStr << "tmpOutWEMNodes=" << outWEMStr << "{" << j << "}" << ".Vertices";
      engEvalString(m_pEngine, executeStr.str().c_str());
      // Get array from Matlab.
      mxArray *m_nodes = engGetVariable(m_pEngine, "tmpOutWEMNodes");
      // Delete temp data.
      engEvalString(m_pEngine, "clear tmpOutWEMNodes");
      // Clear temp string.
      executeStr.str("");

      // Get faces
      executeStr << "tmpOutWEMFaces=" << outWEMStr << "{" << j << "}" << ".Faces-1";
      engEvalString(m_pEngine, executeStr.str().c_str());
      // Get faces from Matlab.
      mxArray *m_faces = engGetVariable(m_pEngine, "tmpOutWEMFaces");
      // Delete temp data.
      engEvalString(m_pEngine, "clear tmpOutWEMFaces");
      // Clear temp string.
      executeStr.str("");

      // Get data from Matlab array.
      if (m_nodes && !mxIsEmpty(m_nodes) && mxGetClassID(m_nodes) == mxDOUBLE_CLASS)
      {
        double *dataNodes = static_cast<double*>(mxGetPr(m_nodes));
        
        if (dataNodes != NULL) {
          const size_t node_rows = mxGetM(m_nodes);

          ML_CHECK_NEW(triPatch, WEMTrianglePatch());
          
          for (i = 0; i < node_rows; i ++) {
            node = triPatch->addNode();
            node->setPosition(dataNodes[i], dataNodes[i + node_rows], dataNodes[i + 2 * node_rows]);
          }

          if (m_faces && !mxIsEmpty(m_faces) && mxGetClassID(m_faces) == mxDOUBLE_CLASS) {
            double *dataFaces = static_cast<double*>(mxGetPr(m_faces));

            if (dataFaces != NULL) {
              const size_t face_rows = mxGetM(m_faces);
              
              for (i = 0; i < face_rows; i ++) {
                triangle = triPatch->addTriangle();
                node = triPatch->getNodeAt(dataFaces[i]);                 triangle->setNode(0,node); node->addFace(triangle);
                node = triPatch->getNodeAt(dataFaces[i + face_rows]);     triangle->setNode(1,node); node->addFace(triangle);
                node = triPatch->getNodeAt(dataFaces[i + 2 * face_rows]); triangle->setNode(2,node); node->addFace(triangle);
              }
            }
          }

          triPatch->buildEdgeConnectivity();
          triPatch->computeNormals();

          _outWEM->addWEMPatch(triPatch);
        }
      }
      mxDestroyArray(m_nodes);
      mxDestroyArray(m_faces);
    }
  }
}

//! Copies scalar values to Matlab.
void MatlabScriptWrapper::_copyInputScalarsToMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputScalarsToMatlab(): Cannot find matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  MLint i = 0;
  // Compose string that contains input scalars.
  std::ostringstream execute;
  // Put only input scalars into Matlab.
  for(i=0; i<6; i++)
  {
    execute<<_scalarNameFld[i]->getStringValue()<<"="<<(_scalarFld[i]->getDoubleValue())<<"\n";
  }
  // Execute string and write input scalars into Matlab.
  engEvalString(m_pEngine, execute.str().c_str());
}

//! Copies scalar values from Matlab.
void MatlabScriptWrapper::_getScalarsBackFromMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getScalarsBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  // Internal loop.
  mxArray *temp = NULL;
  // Get only output scalars.
  for(MLint i=0; i<6; i++) {
    temp = engGetVariable(m_pEngine, (_scalarNameFld[i]->getStringValue()).c_str());
    if(temp!=NULL) {
      if(mxGetClassID(temp)==mxDOUBLE_CLASS) {
        double *fieldVal = static_cast<double*>(mxGetPr(temp));
        _scalarFld[i]->disableNotifications();
        _scalarFld[i]->setDoubleValue(fieldVal[0]);
        _scalarFld[i]->enableNotifications();
      } else {
        std::cerr << "_getScalarsBackFromMatlab(): Output type from Matlab not supported" << std::endl << std::flush;
      }
    }
  }
  mxDestroyArray(temp); temp = NULL;
}

//! Copies string values to Matlab.
void MatlabScriptWrapper::_copyInputStringsToMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputStringsToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  MLint i = 0;
  // Compose string that contains input scalars.
  std::ostringstream execute;
  // Put only input scalars into Matlab.
  for(i=0; i<6; i++) {
    execute<<_stringNameFld[i]->getStringValue()<<"='"<<(_stringFld[i]->getStringValue())<<"'\n";
  }
  // Execute string and write input scalars into Matlab.
  engEvalString(m_pEngine, execute.str().c_str());
}

//! Copies string values from Matlab.
void MatlabScriptWrapper::_getStringsBackFromMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getStringsBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  // Internal loop.
  mxArray *temp = NULL;
  int tempsize = 0;
  char *fieldVal = NULL;
  // Get only output scalars.
  for(MLint i=0; i<6; i++)
  {
    temp = engGetVariable(m_pEngine, (_stringNameFld[i]->getStringValue()).c_str());
    if(temp!=NULL) {
      tempsize = (int)mxGetN(temp)+1;
      ML_CHECK_NEW(fieldVal,char[tempsize]);
      mxGetString(temp,fieldVal,tempsize);
      _stringFld[i]->disableNotifications();
      _stringFld[i]->setStringValue(fieldVal);
      _stringFld[i]->enableNotifications();
      ML_DELETE(fieldVal);
    }
  }
  mxDestroyArray(temp); temp = NULL;
}

//! Copy input vectors to Matlab.
void MatlabScriptWrapper::_copyInputVectorsToMatlab()
{
  // Proof if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputVectorsToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  MLint i = 0;
  // Compose string that contains input vectors.
  std::ostringstream execute;
  // Put only input vectors into Matlab.
  for(i=0; i<6; i++)
  {
    execute<<_vectorNameFld[i]->getStringValue()<<"=[";
      
    vec4 vec = _vectorFld[i]->getVec4fValue();
    execute<<vec[0]<<","<<vec[1]<<","<<vec[2]<<","<<vec[3];
    execute<<"];";
  }
  // Execute string and write input vectors into Matlab.
  engEvalString(m_pEngine, execute.str().c_str());
}

//! Copies vector values from Matlab.
void MatlabScriptWrapper::_getVectorsBackFromMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getVectorsBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  // Internal loop.
  mxArray *temp = NULL;
  // Get only output vectors.
  for(MLint i=0; i<6; i++)
  {
    temp = engGetVariable(m_pEngine, (_vectorNameFld[i]->getStringValue()).c_str());
    if(temp!=NULL) {
      if(mxGetClassID(temp)==mxDOUBLE_CLASS) {
        double *fieldVal = static_cast<double*>(mxGetPr(temp));
        if(mxGetM(temp)==1 && mxGetN(temp)==4) {
          _vectorFld[i]->disableNotifications();
          _vectorFld[i]->setVec4fValue(vec4(fieldVal[0],fieldVal[1],fieldVal[2],fieldVal[3]));
          _vectorFld[i]->enableNotifications();
        } else {
          std::cerr << "_getVectorsBackFromMatlab(): Incorrect vector size" << std::endl << std::flush;
        }
      } else {
        std::cerr << "_getVectorsBackFromMatlab(): Output type from Matlab not supported" << std::endl << std::flush;
      }
    }
  }
  mxDestroyArray(temp); temp = NULL;
}

//! Copy input matrices to Matlab.
void MatlabScriptWrapper::_copyInputMatricesToMatlab()
{
  // Proof if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_copyInputMatricesToMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }
  // Internal loop.
  MLint i = 0;
  // Compose string that contains input matrices.
  std::ostringstream execute;
  // Put only input matrices into Matlab.
  for(i=0; i<3; i++)
  {
    execute<<_matrixNameFld[i]->getStringValue()<<"=[";

    mat4 mat = _matrixFld[i]->getMatrixValue();
    execute<<mat[0][0]<<","<<mat[0][1]<<","<<mat[0][2]<<","<<mat[0][3]<<";";
    execute<<mat[1][0]<<","<<mat[1][1]<<","<<mat[1][2]<<","<<mat[1][3]<<";";
    execute<<mat[2][0]<<","<<mat[2][1]<<","<<mat[2][2]<<","<<mat[2][3]<<";";
    execute<<mat[3][0]<<","<<mat[3][1]<<","<<mat[3][2]<<","<<mat[3][3];
    execute<<"];";
  }
  // Execute string and write input matrices into Matlab.
  engEvalString(m_pEngine, execute.str().c_str());
}

//! Copies matrix values from Matlab.
void MatlabScriptWrapper::_getMatricesBackFromMatlab()
{
  // Check if Matlab is started.
  if (!_checkMatlabIsStarted())
  {
    std::cerr << "_getMatricesBackFromMatlab(): Cannot find Matlab engine!" << std::endl << std::flush;
    return;
  }

  // Internal loop.
  mxArray *temp = NULL;
  // Get only output matrices.
  for(MLint i=0; i<3; i++) {
    temp = engGetVariable(m_pEngine, (_matrixNameFld[i]->getStringValue()).c_str());
    if(temp!=NULL) {
      if(mxGetClassID(temp)==mxDOUBLE_CLASS) {
        double *fieldVal = static_cast<double*>(mxGetPr(temp));
        if(mxGetM(temp)==4 && mxGetN(temp)==4) {
          _matrixFld[i]->disableNotifications();
          _matrixFld[i]->setMatrixValue(mat4(fieldVal[0],fieldVal[4], fieldVal[8],fieldVal[12],
                                             fieldVal[1],fieldVal[5], fieldVal[9],fieldVal[13],
                                             fieldVal[2],fieldVal[6],fieldVal[10],fieldVal[14],
                                             fieldVal[3],fieldVal[7],fieldVal[11],fieldVal[15]));
          _matrixFld[i]->enableNotifications();
        } else {
          std::cerr << "_getMatricesBackFromMatlab(): Incorrect matrix size" << std::endl << std::flush;
        }
      } else {
        std::cerr << "_getMatricesBackFromMatlab(): Output type from Matlab not supported" << std::endl << std::flush;
      }
    }
  }
  mxDestroyArray(temp); temp = NULL;
}

ML_END_NAMESPACE
