//----------------------------------------------------------------------------------
//! The ML module class TransformXMarkerList.
/*!
// \file    mlTransformXMarkerList.cpp
// \author  Coert Metz
// \date    2006-04-20
//
// Transform all XMarkers of an XMarkerList with the given matrix or elementary 
// transforms
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
#ifndef __mlTransformXMarkerList_H
#include "mlTransformXMarkerList.h"
#endif

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(TransformXMarkerList, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
TransformXMarkerList::TransformXMarkerList (void)
  : BaseOp(0, 0)
{
  ML_TRACE_IN("TransformXMarkerList::TransformXMarkerList()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Input and output XMarkerList
  _inputXMarkerList = fields->addBase("inputXMarkerList");
  _outputXMarkerList = fields->addBase("outputXMarkerList");
  _outputXMarkerList->setBaseValue(&_outXMarkerList);
  
  // Bool field to enable/disable transformation
  _transformEnabled = fields->addBool("transformEnabled");
  _transformEnabled->setBoolValue(true);

  // Fields for elementary transforms
  _center = fields->addVec3f("center");
  _center->setVec3fValue(vec3(0));
  _scaleOrientation = new RotationField("scaleOrientation");
  _scaleOrientation->setRotationValue(vec3(0, 0, 1), 0);
  fields->addField(_scaleOrientation);
  _scale = fields->addVec3f("scale");
  _scale->setVec3fValue(vec3(1));
  _rotation = new RotationField("rotation");
  _rotation->setRotationValue(vec3(0, 0, 1), 0);
  fields->addField(_rotation);
  _translation = fields->addVec3f("translation");
  _translation->setVec3fValue(vec3(0));
  _elTrans = fields->addBool("useElementaryTransforms" );
  _elTrans->setBoolValue (true);
  
  // Transformation matrix field
  _matrix = fields->addMatrix("matrix");
  
  // Option for homogenous coordinates
  _homogeneousFld = fields->addBool("divideByHomogeneousCoordinate");
  _homogeneousFld->setBoolValue(false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void TransformXMarkerList::handleNotification (Field * /*field*/)
{
  ML_TRACE_IN("TransformXMarkerList::handleNotification()");
  
  // Compose transformation matrix
  if (_elTrans->getBoolValue()) {
    mat4 matrix = identity3D<mat4::ComponentType>();    
    vec3 center = _center->getVec3fValue();
    Rotation scaleRot = _scaleOrientation->getRotationValue();
    vec3 scale = _scale->getVec3fValue();
    Rotation rot = _rotation->getRotationValue();
    vec3 trans = _translation->getVec3fValue();
    Rotation invScaleRot = scaleRot;
    invScaleRot.conjugate();

    matrix = translation3D<mat4::ComponentType>(trans+center)*
      rot.getMatrix()*
      invScaleRot.getMatrix()*
      scaling3D<mat4::ComponentType>(scale)*
      scaleRot.getMatrix()*
      translation3D<mat4::ComponentType>(-center);
    
    _matrix->setMatrixValue (matrix);
  }

  // Do not transform if not enabled ...
  mat4 matrix = _matrix->getMatrixValue();
  if (!_transformEnabled->getBoolValue())
    matrix = mat4::getIdentity();

  // Transform XMarkers
  Base *baseValue = _inputXMarkerList->getBaseValue();
  if (baseValue && BASE_IS_A(baseValue,XMarkerList)) {
    XMarkerList *inXMarkerList = (XMarkerList *) baseValue; 
    // Clear output list
    _outXMarkerList.clearList();
    for (int i=0; i<int(inXMarkerList->getSize()); ++i) {
      // Get marker position and transform it
      XMarker marker = inXMarkerList->at (i);
      vec4 vMarker = vec4 (marker.x(), marker.y(), marker.z(), 1.0);
      vMarker = matrix * vMarker;
      // If homogeneous coordinates divide by last element
      if (_homogeneousFld->getBoolValue() && vMarker[3] != 0.0) {
        marker.x() = vMarker[0]/vMarker[3];
        marker.y() = vMarker[1]/vMarker[3];
        marker.z() = vMarker[2]/vMarker[3];
      } else {
        marker.x() = vMarker[0];
        marker.y() = vMarker[1];
        marker.z() = vMarker[2];
      }
      // Add marker to output list
      _outXMarkerList.appendItem (marker);
    }
    // Select XMarker which is selected in input list and notify attachments
    _outXMarkerList.doSelectItem(inXMarkerList->getCurrentIndex());
    _outputXMarkerList->notifyAttachments();
  }
}

ML_END_NAMESPACE
