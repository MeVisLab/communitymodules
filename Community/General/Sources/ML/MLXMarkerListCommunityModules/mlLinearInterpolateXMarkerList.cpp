//----------------------------------------------------------------------------------
//! The ML module class LinearInterpolateXMarkerList.
/*!
// \file    mlLinearInterpolateXMarkerList.cpp
// \author  Coert Metz
// \date    2009-09-09
//
// Linearly interpolate an XMarkerList
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
#include "mlLinearInterpolateXMarkerList.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(LinearInterpolateXMarkerList, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
LinearInterpolateXMarkerList::LinearInterpolateXMarkerList ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("LinearInterpolateXMarkerList::LinearInterpolateXMarkerList ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();

  // Add fields to the module and set their values.
  _inputXMarkerListFld = fields.addBase("inputXMarkerList");
  _inputXMarkerListFld->setBaseValue(NULL);
  _outputXMarkerListFld = fields.addBase("outputXMarkerList");
  _outputXMarkerListFld->setBaseValue(&_outputXMarkerList);
  
  // Sample distance for output XMarkerList
  _sampleDistanceFld = fields.addFloat("sampleDistance");
  _sampleDistanceFld->setFloatValue(0.5f);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void LinearInterpolateXMarkerList::handleNotification (Field *field)
{
  ML_TRACE_IN("LinearInterpolateXMarkerList::handleNotification ()");

  // Clear output XMarkerList
  _outputXMarkerList.clear();

  Base * baseValue = _inputXMarkerListFld->getBaseValue();
  if (baseValue && BASE_IS_A(baseValue, XMarkerList)) {
    XMarkerList markers = *(XMarkerList*)baseValue;
    
    if (markers.size()>1 && markers.isModified()) {
      // Compute length XMarkerList
      double length = 0.0;
      for (size_t i=0; i<markers.size()-1; ++i) {
        length += (markers[i].pos-markers[i+1].pos).length();
      }

      // Check sample distance setting
      if (_sampleDistanceFld->getFloatValue()<=0.0f) {
        _sampleDistanceFld->setFloatValue(1.0f);
      }
      // Get sample distance setting
      const float sampleDistance = _sampleDistanceFld->getFloatValue();

      // Add first point
      _outputXMarkerList.appendItem(markers[0]);

      // The stepsize is the sampledistance
      double stepSize = sampleDistance;
      // Centerline length already processed
      double curLength = 0.0;
      // Current line segment position
      unsigned int segmentPos = 0;

      // Get first position
      vec3 currentPos = markers[0].pos.getVec3();
      while (curLength < length && segmentPos<markers.size()-1) {
        // Determine direction
        vec3 direction = (vec6(markers[segmentPos+1].pos-markers[segmentPos].pos)).getVec3();
        direction.normalize();
        // Determine segment length
        double segmentLength=(markers[segmentPos+1].pos-markers[segmentPos].pos).length();
        double curSegmentLength=0.0;
        // Step in direction until next input point
        while (curSegmentLength+stepSize<segmentLength) {
          currentPos = currentPos + stepSize * direction;
          curSegmentLength += stepSize;
          curLength += stepSize;
          _outputXMarkerList.appendItem(XMarker (currentPos));
          stepSize = sampleDistance;
        }
        // Determine rest length
        stepSize = segmentLength-curSegmentLength;
        // Update currentpos
        currentPos = currentPos + stepSize * direction;
        curLength+=stepSize;
        stepSize = sampleDistance - stepSize;

        // Increment current line segment position
        ++segmentPos;
      }
    }
  }

  // Notify attached modules
  _outputXMarkerListFld->notifyAttachments();
}

ML_END_NAMESPACE

