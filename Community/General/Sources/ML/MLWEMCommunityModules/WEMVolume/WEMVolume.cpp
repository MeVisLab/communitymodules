//----------------------------------------------------------------------------------
//! The ML module class WEMVolume.
/*!
// \file    WEMVolume.cpp
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
#include "WEMVolume.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(WEMVolume, WEMInspector);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
WEMVolume::WEMVolume (std::string type)
  : WEMInspector(type)
{
  ML_TRACE_IN("WEMVolume::WEMVolume()")

  FieldContainer *fields = getFieldContainer();
  ML_CHECK(fields);

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // Add min, max volume, and corresponding index fields
  _minVolumeFld = fields->addFloat("minVolume");
  _minVolumeFld->setFloatValue(0.0f);
  _maxVolumeFld = fields->addFloat("maxVolume");
  _maxVolumeFld->setFloatValue(0.0f);
  _minVolumeIndexFld = fields->addInt("minVolumePatchIndex");
  _minVolumeIndexFld->setIntValue(0);
  _maxVolumeIndexFld = fields->addInt("maxVolumePatchIndex");
  _maxVolumeIndexFld->setIntValue(0);
  
  // Add output curve field
  _outputVolumeCurveFld = fields->addBase("outputVolumeCurve");
  _outputVolumeCurveFld->setBaseValue(&_outputVolumeCurve);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
WEMVolume::~WEMVolume()
{
  ML_TRACE_IN("WEMVolume::~WEMVolume()")
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void WEMVolume::handleNotification (Field *field)
{
  ML_TRACE_IN("WEMVolume::handleNotification()")

  // call parent class and handle apply/autoApply and in/outputs
  WEMInspector::handleNotification(field);
}

//----------------------------------------------------------------------------------
//! Code below is performed after module and network initialization.
//----------------------------------------------------------------------------------
void WEMVolume::activateAttachments()
{
    ML_TRACE_IN("WEMVolume::activateAttachments()")

    // call parent class
    WEMInspector::activateAttachments();
}


//----------------------------------------------------------------------------------
//! The process method is called by the parent class.
//----------------------------------------------------------------------------------
void WEMVolume::_process()
{
    ML_TRACE_IN("WEMVolume::process()")

    // for time measurement and mouse cursor setting.
    _startProcessing();

    WEMInspector::_process();

    // Now call the own processing routine.
    ComputeVolumes();

    // stop time measurement and mouse cursor resetting.
    _finishProcessing();
}

//----------------------------------------------------------------------------------
//! The main processing routine. Implement your algorithm here.
//----------------------------------------------------------------------------------
void WEMVolume::ComputeVolumes()
{
  ML_TRACE_IN("WEMVolume::ComputeVolumes()")

  // Clear output curve
  _outputVolumeCurve.clear();

  // Vectors to store the x and y values of the curve
  std::vector<float> indices;
  std::vector<float> volumes;

  if (_inWEM != NULL){
    float minVol = ML_FLOAT_MAX;
    int minVolIndex = 0;
    float maxVol = 0.0f;
    int maxVolIndex = 0;
    
    // Iterate over all patches and compute volume of patch
    for (unsigned int i = 0; i < _inWEM->getNumWEMPatches(); i++){
      WEMPatch* wemPatch = _inWEM->getWEMPatchAt(i);
      const float volume = wemPatch->getVolume();
      // Check for min and max volume
      if (volume<minVol) {
        minVol=volume;
        minVolIndex=i;
      }
      if (volume>maxVol) {
        maxVol=volume;
        maxVolIndex=i;
      }
      // Add values to curve
      volumes.push_back(volume);
      indices.push_back(static_cast<float>(i));
    }
    _minVolumeFld->setFloatValue(minVol);
    _minVolumeIndexFld->setIntValue(minVolIndex);
    _maxVolumeFld->setFloatValue(maxVol);
    _maxVolumeIndexFld->setIntValue(maxVolIndex);
  } else {
    // Default values
    _minVolumeFld->setFloatValue(0.0f);
    _minVolumeIndexFld->setIntValue(0);
    _maxVolumeFld->setFloatValue(0.0f);
    _maxVolumeIndexFld->setIntValue(0);
  }

  // Output curve
  _outputVolumeCurveData.setTitle("Volumes");
  _outputVolumeCurveData.setXRange(0, volumes.size()-1);
  _outputVolumeCurveData.setX(volumes.size(), &indices[0]);
  _outputVolumeCurveData.setY(0, volumes.size(), &volumes[0]);  
  _outputVolumeCurve.getCurveList().push_back(&_outputVolumeCurveData);
  _outputVolumeCurveFld->notifyAttachments();
}

ML_END_NAMESPACE
