//----------------------------------------------------------------------------------
//! The ML module class XMarkerListToFile.
/*!
// \file    mlXMarkerListToFile.cpp
// \author  Coert Metz
// \date    2007-07-06
//
// Save XMarkers to a text file
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
#include "mlXMarkerListToFile.h"
#include <fstream>

ML_START_NAMESPACE

#include <set>

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(XMarkerListToFile, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
XMarkerListToFile::XMarkerListToFile (void)
  : BaseOp(1, 0)
{
  ML_TRACE_IN("XMarkerListToFile::XMarkerListToFile()")

  // Coordinate system and separator options
  const char *_outputCoordinateSystemFldValues[] = { "world", "voxel","pos:voxel, vec:world"};
  const char *_coordinateSeparatorFldValues[] = { "space", "newline" };
  
  // Get field container
  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Add filename field
  _filenameFld = fields->addString("filename");
  _filenameFld->setStringValue("");
  
  // Add enum to choose output coordinate system
  _outputCoordinateSystemFld = fields->addEnum("outputCoordinateSystem", _outputCoordinateSystemFldValues, 3);
  _outputCoordinateSystemFld->setEnumValue(1);

  // Add enum to choose coordinate separator
  _coordinateSeparatorFld = fields->addEnum("coordinateSeparator", _coordinateSeparatorFldValues, 2);
  _coordinateSeparatorFld->setEnumValue(0);

  // Add input XMarkerList field containing the markers to write to file
  _inputXMarkerListFld = fields->addBase("inputXMarkerList");
  _inputXMarkerListFld->addAllowedType<XMarkerList>();

  // Add bools to select elements for the export
  _positionXFld = fields->addBool("exportPositionX");
  _positionXFld->setBoolValue(true);
  _positionYFld = fields->addBool("exportPositionY");
  _positionYFld->setBoolValue(true);
  _positionZFld = fields->addBool("exportPositionZ");
  _positionZFld->setBoolValue(true);
  _positionTFld = fields->addBool("exportPositionT");
  _positionTFld->setBoolValue(false);
  _vectorXFld = fields->addBool("exportVectorX");
  _vectorXFld->setBoolValue(false);
  _vectorYFld = fields->addBool("exportVectorY");
  _vectorYFld->setBoolValue(false);
  _vectorZFld = fields->addBool("exportVectorZ");
  _vectorZFld->setBoolValue(false);
  _typeFld = fields->addBool("exportType");
  _typeFld->setBoolValue(false);
  _nameFld = fields->addBool("exportName");
  _nameFld->setBoolValue(false);

  // Add field to enable/disable output of only one marker per voxel
  _maxOneMarkerPerVoxelFld = fields->addBool("maxOneMarkerPerVoxel");
  _maxOneMarkerPerVoxelFld->setBoolValue(false);

  // Enable/disable elastix (transformix) format
  _transformixFormatFld = fields->addBool("elastixFormat");
  _transformixFormatFld->setBoolValue(false);

  // Add save button
  _saveFld = fields->addNotify("save");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void XMarkerListToFile::handleNotification (Field *field)
{
  ML_TRACE_IN("XMarkerListToFile::handleNotification()")

  if (field==_saveFld) {
    // When voxel is selected as output coordinate system, an image should be
    // connected for the worldToVoxel matrix
    if (_outputCoordinateSystemFld->getEnumValue() && !getUpdatedInImg(0)) {
      std::cout << "When you select voxel as output coordinate system you should provide an input image!" << std::endl;
      return;
    }

    // Get coordinate separator
    std::stringstream coordinateSeparator;
    if (_coordinateSeparatorFld->getEnumValue()) {
      coordinateSeparator << std::endl;
    } else {
      coordinateSeparator << " ";
    }

    // Check XMarkerList input
    Base *baseInput = _inputXMarkerListFld->getBaseValue();
    if (baseInput && BASE_IS_A(baseInput, XMarkerList)) {
      // Get XMarkerList
      XMarkerList markers = * (XMarkerList*) baseInput;
      // Get and check filename
      std::string filename = _filenameFld->getStringValue();
      if (filename=="") {
        std::cout << "You should provide a filename for the output!" << std::endl;
        return;
      }
      // Open file
      std::fstream file_op(filename.c_str(), std::ios::out);
      // Check if file is open
      if (!file_op.is_open()) {
        std::cout << "Cannot write output file!" << std::endl;
        return;
      }

      // transformix format?
      if (_transformixFormatFld->getBoolValue()) {
        if (!_outputCoordinateSystemFld->getEnumValue()) {
          file_op << "point" << std::endl;
        } else {
          file_op << "index" << std::endl;
        }
        file_op << markers.size() << std::endl;
      }

      // Check which elements to export
      const bool posX = _positionXFld->getBoolValue();
      const bool posY = _positionYFld->getBoolValue();
      const bool posZ = _positionZFld->getBoolValue();
      const bool posT = _positionTFld->getBoolValue();
      const bool vecX = _vectorXFld->getBoolValue();
      const bool vecY = _vectorYFld->getBoolValue();
      const bool vecZ = _vectorZFld->getBoolValue();
      const bool type = _typeFld->getBoolValue();
	    const bool name = _nameFld->getBoolValue();

      // Vector storing occupied voxel position
      std::set<vec3> voxelsOccupied;
      const bool onePerVoxel = _maxOneMarkerPerVoxelFld->getBoolValue();
      
      // Write markers to file
      for (XMarkerList::iterator it = markers.begin(); it != markers.end(); ++it) {
        // Get marker
        XMarker marker = *it;
        // Get world coordinates of marker
        const vec3 world = marker.pos.getVec3();
        vec3 voxel = world;
        vec3 vec = marker.vec;
        if (_outputCoordinateSystemFld->getEnumValue() == 1) {
          // Output position and vector in voxel coordinates
          getUpdatedInImg(0)->transformToVoxelCoord(world, voxel);
          getUpdatedInImg(0)->transformToVoxelCoord(world+vec, vec);
          vec = vec - voxel;
          // Floor voxel coordinates
          voxel[0] = int (voxel[0]);
          voxel[1] = int (voxel[1]);
          voxel[2] = int (voxel[2]);
        } else {
          // Output position in voxel coordinates and vector in world coordinates
          if (_outputCoordinateSystemFld->getEnumValue() == 2) {
            getUpdatedInImg(0)->transformToVoxelCoord(world, voxel);
            // Floor voxel coordinates
            voxel[0] = int (voxel[0]);
            voxel[1] = int (voxel[1]);
            voxel[2] = int (voxel[2]);
          }
        }

        // Check if voxel position already ocupied
        bool allowInsert = true;
        if(onePerVoxel) {
          if(voxelsOccupied.find(voxel) != voxelsOccupied.end())
            allowInsert = false;
          else
            voxelsOccupied.insert(voxel);
        }

        // Output marker
        if(allowInsert) {
          if (posX) {
            file_op << voxel[0];
            if (posT || posY || posZ || vecX || vecY || vecZ || type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (posY) {
            file_op << voxel[1];
            if (posT || posZ || vecX || vecY || vecZ || type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (posZ) {
            file_op << voxel[2];
            if (posT || vecX || vecY || vecZ || type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (posT) {
            file_op << marker.pos[4];
            if (vecX || vecY || vecZ || type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (vecX) {
            file_op << vec[0];
            if (vecY || vecZ || type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (vecY) {
            file_op << vec[1];
            if (vecZ || type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (vecZ) {
            file_op << vec[2];
            if (type || name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (type) {
            file_op << marker.type << " ";
            if (name) {
              file_op << coordinateSeparator.str();
            }
          }
          if (name) {
			      file_op << marker.name();
		      }
          file_op << std::endl;
        }
      }
    } else {
      std::cout << "Input is not a valid XMarkerList!" << std::endl;
      return;
    }
  }
}

ML_END_NAMESPACE
