//----------------------------------------------------------------------------------
//! The ML module class CSOPointsToXMarkers.
/*!
// \file    mlCSOPointsToXMarkers.cpp
// \author  Coert Metz
// \date    2007-07-11
//
// Convert CSO points to an XMarkerList. Additionaly CSO normals can be computed
// and exported.
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
#include "mlCSOPointsToXMarkers.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(CSOPointsToXMarkers, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CSOPointsToXMarkers::CSOPointsToXMarkers (void)
  : BaseOp(0, 0), _inputCSOList(NULL)
{
  ML_TRACE_IN("CSOPointsToXMarkers::CSOPointsToXMarkers()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Input CSO List field
  _inputCSOListFld = fields->addBase("inputCSOList");
  _inputCSOListFld->setBaseValue(NULL);

  // Output XMarkerList field
  _outputXMarkerListFld = fields->addBase("outputXMarkerList");
  _outputXMarkerListFld->setBaseValue(&_outputXMarkerList);
  
  // List index field
  _listIndexFld = fields->addInt("listIndexFld");
  _listIndexFld->setIntValue(0);

  // Bool to enable/disable to also export the path points
  _convertPathPoints=fields->addBool("convertPathPoints");

  // Bool to enable/disable to export CSO normals
  _outputCSONormals = fields->addBool("outputCSONormals");
  _outputCSONormals->setBoolValue(false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
CSOPointsToXMarkers::~CSOPointsToXMarkers (void) 
{
  if (_inputCSOList != NULL) {
    _inputCSOList->removeNotificationObserver(CSOListNotifyObserverCB, this);
  }
}

//----------------------------------------------------------------------------------
//! Handle notifications of CSOList
//----------------------------------------------------------------------------------
void CSOPointsToXMarkers::CSOListNotifyObserverCB(void* userData, int notificationFlag)
{
  ML_TRACE_IN("CSOPointsToXMarkers::_csoListNotifyObserverCB");

  CSOPointsToXMarkers * thisPointer = static_cast<CSOPointsToXMarkers*>(userData);
  thisPointer->convertCSOToXMarkerList();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void CSOPointsToXMarkers::handleNotification (Field * field)
{
  ML_TRACE_IN("CSOPointsToXMarkers::handleNotification()")

  // Get CSO list if changed
  if (field==_inputCSOListFld) {
    if (_inputCSOList!=NULL) {
      // Remove old notification observer
      _inputCSOList->removeNotificationObserver(CSOListNotifyObserverCB, this);
      _inputCSOList=NULL;
    }

    // Get input CSO list
    Base *baseValue = _inputCSOListFld->getBaseValue();
    if (baseValue && BASE_IS_A(baseValue,CSOList)) {
      _inputCSOList = (CSOList *) baseValue;
      // Add new notification observer
      _inputCSOList->addNotificationObserver(CSOListNotifyObserverCB, this);
    }
  }

  convertCSOToXMarkerList();
}

//----------------------------------------------------------------------------------
//! Convert a CSO to an XMarkerList
//----------------------------------------------------------------------------------
void CSOPointsToXMarkers::convertCSOToXMarkerList()
{
  // Empty output XMarkerList
  _outputXMarkerList.clear();

  if (_inputCSOList!=NULL) {
    // Get list index value
    int listIndex = mlMax (static_cast<MLint>(-1), _listIndexFld->getIntValue());

    // Get output options
    const bool outputNormals = _outputCSONormals->getBoolValue();
    const bool outputPathPoints = _convertPathPoints->getBoolValue();

    // Add all markers with specified type to output markerlist
    if (listIndex > int (_inputCSOList->numCSO()-1)) {
      std::cout << "The CSO with index " << listIndex << " does not exist in the CSOList" << std::endl;
    } 
    else 
    {
      // If listIndex = -1, all CSOs in the list are exported;
      // determine start index and number of CSOs to export.
      int numOfLoops;
      int currentIndex;
      if (listIndex == -1) {
        numOfLoops = _inputCSOList->numCSO();
        currentIndex = 0;
      } else {
        numOfLoops = 1;
        currentIndex = listIndex;
      }
      for (int j=currentIndex; j<currentIndex + numOfLoops; j++) {
        // Get CSO at listIndex
        CSO * cso = _inputCSOList->getCSOAt(j);

        // Compute plane normal
        cso->computePlaneNormal();
        const bool inPlane = cso->isInPlane();
        const vec3 planeNormal = cso->getPlaneNormal();

        if (outputPathPoints) {
          std::list<vec3> points;
          // First collect all pathpoints
          for (int i=0; i<cso->numPathPointLists(); ++i) {
            CSOPathPoints* pathList=cso->getPathPointsAt(i);
            int numOfPathPoints = pathList->numPathPoints();
            for (int k=0; k<numOfPathPoints; k++)
            {
              vec3 posPathPoint = pathList->getPosAt(k);
              if (points.back()!=posPathPoint) points.push_back(posPathPoint);
            }
          }

          // Second: compute normals and output points
          for (std::list<vec3>::const_iterator it = points.begin(); it != points.end(); ++it) {
            // Create marker
            XMarker markerPath(*it);
            markerPath.type = j;
            // Compute normal
            if (outputNormals && inPlane) {
              // Determine prev and next it
              std::list<vec3>::const_iterator itPrev = it;
              itPrev--;
              std::list<vec3>::const_iterator itNext = it;
              itNext++;
              if (it == points.begin()) {
                if (cso->isClosed()) {
                  itPrev = points.end();
                  itPrev--;
                } else {
                  itPrev = it;
                }
              }
              if (itNext == points.end()) {
                if (cso->isClosed()) {
                  itNext = points.begin();
                } else {
                  itNext = it;
                }
              }

              // Get vector from previous to next point
              const vec3 posPrevPathPoint = *itPrev;
              const vec3 posNextPathPoint = *itNext;
              const vec3 rc = posNextPathPoint - posPrevPathPoint;

              // Compute normal
              vec3 pointNormal = rc.cross(planeNormal);
              pointNormal.normalize();

              // Set normal
              markerPath.vec = pointNormal;
            }

            // Add marker to output list
            _outputXMarkerList.appendItem(markerPath);
          }
        } else{
          for (int i=0; i<cso->numSeedPoints(); ++i) {
            CSOSeedPoint * seedpoint = cso->getSeedPointAt(i);
            vec3 pos = seedpoint->worldPosition;
            XMarker marker (pos);
            marker.type = j;

            if (inPlane && outputNormals) {
              // Determine previous and next marker
              int iPrev = i-1;
              int iNext = i+1;
              if (iPrev<0) {
                if (cso->isClosed()) {
                  iPrev=cso->numSeedPoints()-1;
                } else {
                  iPrev=i;
                }
              }
              if (iNext>cso->numSeedPoints()-1) {
                if (cso->isClosed()) {
                  iNext=0;
                } else {
                  iNext=i;
                }
              }

              // Get vector from previous to next point              
              const vec3 posPrevSeedPoint = cso->getSeedPointAt(iPrev)->worldPosition;
              const vec3 posNextSeedPoint = cso->getSeedPointAt(iNext)->worldPosition;
              const vec3 rc = posNextSeedPoint - posPrevSeedPoint;

              // Compute normal
              vec3 pointNormal = rc.cross(planeNormal);
              pointNormal.normalize();

              // Set normal
              marker.vec = pointNormal;
            }

            // Add marker to output list
            _outputXMarkerList.appendItem(marker);
          }
        }

        // Notify attached modules
        _outputXMarkerListFld->notifyAttachments();
      }
    }
  }
}

ML_END_NAMESPACE
