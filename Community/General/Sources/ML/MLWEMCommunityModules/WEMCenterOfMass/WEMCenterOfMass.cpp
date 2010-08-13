//----------------------------------------------------------------------------------
//! The ML module class WEMCenterOfMass.
/*!
// \file    WEMCenterOfMass.cpp
// \author  Coert Metz
// \date    2008-11-27
//
// Computes the center of mass of a WEM object
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
#include "WEMCenterOfMass.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(WEMCenterOfMass, WEMInspector);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
WEMCenterOfMass::WEMCenterOfMass (std::string type)
  : WEMInspector(type)
{
  ML_TRACE_IN("WEMCenterOfMass::WEMCenterOfMass()")

  FieldContainer *fieldC = getFieldContainer();
  ML_CHECK(fieldC);

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Add output fields
  _centerOfMass = fieldC->addVec3f("centerOfMass");
  _centerOfMass->setVec3fValue(vec3(0.0,0.0,0.0));
  _inverseCenterOfMass = fieldC->addVec3f("inverseCenterOfMass");
  _inverseCenterOfMass->setVec3fValue(vec3(0.0,0.0,0.0));

  _useSurface = fieldC->addBool("useSurface");
  _useSurface->setBoolValue( false );

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
WEMCenterOfMass::~WEMCenterOfMass()
{
    ML_TRACE_IN("WEMCenterOfMass::~WEMCenterOfMass()")

    // destroy own dynamic data structures here
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void WEMCenterOfMass::handleNotification (Field *field)
{
  ML_TRACE_IN("WEMCenterOfMass::handleNotification()");

  if ( field == _useSurface ) { this->_process(); }
  // call parent class and handle apply/autoApply and in/outputs
  WEMInspector::handleNotification(field);
}

//----------------------------------------------------------------------------------
//! Code below is performed after module and network initialization.
//----------------------------------------------------------------------------------
void WEMCenterOfMass::activateAttachments()
{
    ML_TRACE_IN("WEMCenterOfMass::activateAttachments()")

    // call parent class
    WEMInspector::activateAttachments();
}


//----------------------------------------------------------------------------------
//! The process method is called by the parent class.
//----------------------------------------------------------------------------------
void WEMCenterOfMass::_process()
{
    ML_TRACE_IN("WEMCenterOfMass::process()")

    // for time measurement and mouse cursor setting.
    _startProcessing();

    WEMInspector::_process();

    // Compute centroid of WEM
    ComputeCentroid();

    // stop time measurement and mouse cursor resetting.
    _finishProcessing();

    // notify registered observer modules.
    //_notifyObservers();
}

//----------------------------------------------------------------------------------
//! The main processing routine. Implement your algorithm here.
//----------------------------------------------------------------------------------
void WEMCenterOfMass::ComputeCentroid()
{
    ML_TRACE_IN("WEMCenterOfMass::ComputeCentroid()")

    if (_inWEM != NULL){
      double sumX=0.0, sumY=0.0, sumZ=0.0;
      int num=0;
      Vector3 centroid;
      double totalArea=0.0;
      // Iterate over all nodes and compute sum of x, y and z-position
      for (unsigned int i = 0; i < _inWEM->getNumWEMPatches(); i++){
        WEMPatch* wemPatch = _inWEM->getWEMPatchAt(i);
        const unsigned int numNodesInPatch = wemPatch->getNumNodes();
        if ( _useSurface->getBoolValue() ){
          WEMTrianglePatch * trianglePatch;
          bool isTrangle = true;
          if (_inWEM->getWEMPatchAt(i)->getPatchType() != WEM_PATCH_TRIANGLES ){
            trianglePatch = new WEMTrianglePatch;
            isTrangle = false;
            wemPatch->triangulate(  trianglePatch, WEM_TRIANGULATION_CENTER );
          } else {
            trianglePatch = (WEMTrianglePatch*)_inWEM->getWEMPatchAt(i);
          }
          unsigned int nFaces = trianglePatch->getNumFaces();
          for (unsigned int j = 0; j < nFaces; j++){
            WEMFace* face = trianglePatch->getFaceAt(j);
            double area = face->getArea();
            Vector3 centroid = face->getCentroid();
            sumX+=centroid[0]*area;
            sumY+=centroid[1]*area;
            sumZ+=centroid[2]*area;
            totalArea += area;
            ++num;
          }
          if (!isTrangle){
            delete trianglePatch;
          }
        } else {
          for (unsigned int j = 0; j < numNodesInPatch; j++){
            WEMNode* node = wemPatch->getNodeAt(j);
            float x, y, z;
            node->getPosition(x, y, z);
            sumX+=x;
            sumY+=y;
            sumZ+=z;
            ++num;
          }
        }
      }
      // Set output fields
      if ( _useSurface->getBoolValue() ){
        _centerOfMass->setVec3fValue( vec3(sumX/totalArea, sumY/totalArea, sumZ/totalArea) );
        _inverseCenterOfMass->setVec3fValue( -vec3(sumX/totalArea, sumY/totalArea, sumZ/totalArea) );
      } else {
        _centerOfMass->setVec3fValue(vec3(sumX/double(num), sumY/double(num), sumZ/double(num)));
        _inverseCenterOfMass->setVec3fValue(-vec3(sumX/double(num), sumY/double(num), sumZ/double(num)));
      }
    } else {
      // Reset output fields
      _centerOfMass->setVec3fValue(vec3());
      _inverseCenterOfMass->setVec3fValue(vec3());
    }
}


ML_END_NAMESPACE

