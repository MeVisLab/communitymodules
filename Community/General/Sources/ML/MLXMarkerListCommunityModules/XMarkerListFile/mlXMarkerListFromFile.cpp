////----------------------------------------------------------------------------------
//! The ML module class XMarkerListFromFile.
/*!
// \file    mlXMarkerListFromFile.cpp
// \author  Coert Metz
// \date    2007-06-22
//
// Read XMarkers from a text file
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
#include "mlXMarkerListFromFile.h"
#include <fstream>

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(XMarkerListFromFile, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
XMarkerListFromFile::XMarkerListFromFile (void)
  : BaseOp(1, 0)
{
  ML_TRACE_IN("XMarkerListFromFile::XMarkerListFromFile()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Add filename field
  _filenameFld = fields->addString("filename");
  _filenameFld->setStringValue("");
  
  // Add bool fields to select elements to import
  _positionXFld = fields->addBool("importPositionX");
  _positionXFld->setBoolValue(true);
  _positionYFld = fields->addBool("importPositionY");
  _positionYFld->setBoolValue(true);
  _positionZFld = fields->addBool("importPositionZ");
  _positionZFld->setBoolValue(true);
  _positionTFld = fields->addBool("importPositionT");
  _positionTFld->setBoolValue(false);
  _vectorXFld = fields->addBool("importVectorX");
  _vectorXFld->setBoolValue(false);
  _vectorYFld = fields->addBool("importVectorY");
  _vectorYFld->setBoolValue(false);
  _vectorZFld = fields->addBool("importVectorZ");
  _vectorZFld->setBoolValue(false);
  _typeFld = fields->addBool("importType");
  _typeFld->setBoolValue(false);
  
  // Add input coordinate system field
  const char *_inputCoordinateSystemFldValues[] = { "world", "voxel" };
  _inputCoordinateSystemFld = fields->addEnum("inputCoordinateSystem", _inputCoordinateSystemFldValues, 2);
  _inputCoordinateSystemFld->setEnumValue(1);
  
  // Add field to let the module skip some element before reading the markers from file
  _skipElementsFld = fields->addInt("numElementsToSkip");
  _skipElementsFld->setIntValue(0);

  // Output XMarkerList
  _outputXMarkerListFld = fields->addBase("outputXMarkerList");
  _outputXMarkerListFld->setBaseValue(&_outputXMarkerList);

  // Update fields
  _updateFld = fields->addNotify("update");
  _autoUpdateFld = fields->addBool("autoUpdate");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void XMarkerListFromFile::handleNotification (Field *field)
{
  ML_TRACE_IN("XMarkerListFromFile::handleNotification()")

  if ( field==_updateFld || ( _autoUpdateFld->getBoolValue() && field != _outputXMarkerListFld ) ) {
    _outputXMarkerList.clear();

    // Check if an input image is connected when coordinate system is set to voxel
    if (_inputCoordinateSystemFld->getEnumValue()==1 && !getUpdatedInImg(0)) {
      std::cout << "When you select voxel as input coordinate system you should provide an input image!" << std::endl;
      return;
    }

    // Check filename
    std::string filename = _filenameFld->getStringValue();
    if (filename != "") {
      // Open file
      std::fstream file_op(filename.c_str(), std::ios::in);
      // Check if file is open
      if (file_op.is_open()) {
        mat4 toWorld = mat4::getIdentity();
        // Skip number of lines
        for (int i=0; i<_skipElementsFld->getIntValue(); ++i) {
          std::string dummy;
          file_op >> dummy;
        }
        
        // Read XMarkers from text file
        while (true) {
          XMarker marker;
          std::vector<std::string> tokens (8, "0");
          if (_positionXFld->getBoolValue()) if ( !(file_op >> tokens[0]) ) break;
          if (_positionYFld->getBoolValue()) if ( !(file_op >> tokens[1]) ) break;
          if (_positionZFld->getBoolValue()) if ( !(file_op >> tokens[2]) ) break;
          if (_positionTFld->getBoolValue()) if ( !(file_op >> tokens[3]) ) break;
          if (_vectorXFld->getBoolValue()) if ( !(file_op >> tokens[4]) ) break;
          if (_vectorYFld->getBoolValue()) if ( !(file_op >> tokens[5]) ) break;
          if (_vectorZFld->getBoolValue()) if ( !(file_op >> tokens[6]) ) break;
          if (_typeFld->getBoolValue()) if ( !(file_op >> tokens[7]) ) break;
          vec3 voxel (atof (tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str()));
          // When coordinates in file are in world coordinates, we are done
          vec3 world = voxel;
          vec3 vec;
          vec[0] = atof (tokens[4].c_str());
          vec[1] = atof (tokens[5].c_str());
          vec[2] = atof (tokens[6].c_str());
          int type = atoi (tokens[7].c_str());
          // When coordinates in file are in voxel coordinates, we need to convert both the
          // position and the vector to world coordinates
          if (_inputCoordinateSystemFld->getEnumValue()) {
            getUpdatedInImg(0)->transformToWorldCoord(voxel+vec3(0.5, 0.5, 0.5), world);
            vec3 vecWOrld;
            getUpdatedInImg(0)->transformToWorldCoord(voxel+vec+vec3(0.5, 0.5, 0.5), vecWOrld);
            vec = vecWOrld - world;
          }
          marker.pos[0] = world[0];
          marker.pos[1] = world[1];
          marker.pos[2] = world[2];
          marker.pos[4] = atof(tokens[3].c_str());
          marker.vec = vec;
          marker.type = type;
          // Add marker to output list
          _outputXMarkerList.appendItem(marker);
        }
      } else {
        std::cout << "Cannot open file (" << filename << ")!" << std::endl;
      }
    }

    _outputXMarkerListFld->notifyAttachments();
  }
}

ML_END_NAMESPACE
