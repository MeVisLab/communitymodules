//----------------------------------------------------------------------------------
//! The ML module class WEMNodesToFile.
/*!
// \file    WEMNodesToFile.cpp
// \author  Coert Metz
// \date    2009-02-09
//
// Output WEM node coordinates to a textfile
*/
//----------------------------------------------------------------------------------

/* =================================================================================
   Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR),
   Departments of Radiology and Medical Informatics, Erasmus MC. All
   rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of BIGR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =================================================================================*/

// Local includes
#include "WEMNodesToFile.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(WEMNodesToFile, WEMInspector);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
WEMNodesToFile::WEMNodesToFile (std::string type)
  : WEMInspector(type)
{
  ML_TRACE_IN("WEMNodesToFile::WEMNodesToFile()")

  FieldContainer *fields = getFieldContainer();
  ML_CHECK(fields);

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Add filename field
  _filenameFld = fields->addString("filename");
  _filenameFld->setStringValue("");

  // Add transformix option field
  _transformixCompatibleFld = fields->addBool("transformixCompatible");
  _transformixCompatibleFld->setBoolValue(false);

  // Add save button
  _saveFld = fields->addNotify("save");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
WEMNodesToFile::~WEMNodesToFile()
{
    ML_TRACE_IN("WEMNodesToFile::~WEMNodesToFile()")
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void WEMNodesToFile::handleNotification (Field *field)
{
  ML_TRACE_IN("WEMNodesToFile::handleNotification()")

  WEMInspector::handleNotification(field);
  if (field == _saveFld) {
    OutputNodes();
  }
}

//----------------------------------------------------------------------------------
//! Code below is performed after module and network initialization.
//----------------------------------------------------------------------------------
void WEMNodesToFile::activateAttachments()
{
    ML_TRACE_IN("WEMNodesToFile::activateAttachments()")

    // call parent class
    WEMInspector::activateAttachments();
}


//----------------------------------------------------------------------------------
//! The process method is called by the parent class.
//----------------------------------------------------------------------------------
void WEMNodesToFile::_process()
{
    ML_TRACE_IN("WEMNodesToFile::process()")

    // for time measurement and mouse cursor setting.
    _startProcessing();

    WEMInspector::_process();

    // stop time measurement and mouse cursor resetting.
    _finishProcessing();
}

//----------------------------------------------------------------------------------
//! The main processing routine. Implement your algorithm here.
//----------------------------------------------------------------------------------
void WEMNodesToFile::OutputNodes()
{
  ML_TRACE_IN("WEMNodesToFile::OutputNodes()")

  // Get and check filename
  std::string filename = _filenameFld->getStringValue();
  if (filename=="") {
    std::cout << "You should provide a filename for the output!" << std::endl;
    return;
  }

  // Open file
  std::fstream file_op(filename.c_str(), std::ios::out);
  if (!file_op.is_open()) {
    std::cout << "Cannot write output file!" << std::endl;
    return;
  }

  if (_inWEM != NULL){
    // Output transformix specific line
    if (_transformixCompatibleFld->getBoolValue()) file_op << "point" << std::endl;
    
    std::stringstream points;
    int num=0;
    // Add all node coordinates to point stringstream
    for (unsigned int i = 0; i < _inWEM->getNumWEMPatches(); i++){
      WEMPatch* wemPatch = _inWEM->getWEMPatchAt(i);
      const unsigned int numNodesInPatch = wemPatch->getNumNodes();
      for (unsigned int j = 0; j < numNodesInPatch; j++){
        WEMNode* node = wemPatch->getNodeAt(j);
        float x, y, z;
        node->getPosition(x, y, z);
        points << x << " " << y << " " << z << std::endl;
        ++num;
      }
    }
    // Output transformix specific line (number of points)
    if (_transformixCompatibleFld->getBoolValue()) file_op << num << std::endl;
    // Output coordinates
    file_op << points.str();
  }
  file_op.close();
}


ML_END_NAMESPACE
