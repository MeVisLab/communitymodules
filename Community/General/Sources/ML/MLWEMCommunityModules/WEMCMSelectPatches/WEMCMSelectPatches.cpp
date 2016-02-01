//----------------------------------------------------------------------------------
//! The ML module class WEMCMSelectPatches.
/*!
// \file    WEMCMSelectPatches.cpp
// \author  Coert Metz
// \date    2009-02-10
//
// Select a range of patches from a WEM
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
#include "WEMCMSelectPatches.h"

#include <algorithm>
#include <functional>
#include <fstream>

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(WEMCMSelectPatches, WEMProcessor);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
WEMCMSelectPatches::WEMCMSelectPatches (std::string type)
  : WEMProcessor(type, false)
{
  ML_TRACE_IN("WEMCMSelectPatches::WEMCMSelectPatches()")

  FieldContainer *fields = getFieldContainer();
  ML_CHECK(fields);

  // Suppress calls of hanbdleNotification on field changes.
  handleNotificationOff();

  // WEM Patch start and end index field
  _patchStartIndexFld = fields->addInt("patchStartIndex");
  _patchStartIndexFld->setIntValue(0);
  _patchEndIndexFld = fields->addInt("patchEndIndex");
  _patchEndIndexFld->setIntValue(0);

  // Bool field to enable to select only one patch based on start index value
  _onePatchFld = fields->addBool("onePatch");
  _onePatchFld->setBoolValue(false);
  
  // Set auto apply fields
  _autoApplyFld->setBoolValue(true);

  _notifyFld = fields->addNotify("notify");
 
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
WEMCMSelectPatches::~WEMCMSelectPatches()
{
  ML_TRACE_IN("WEMCMSelectPatches::~WEMCMSelectPatches()")
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void WEMCMSelectPatches::handleNotification (Field *field)
{
  ML_TRACE_IN("WEMCMSelectPatches::handleNotification()")

  if (field==_patchStartIndexFld || field==_patchEndIndexFld || field==_onePatchFld) {
    if (_autoApplyFld->getBoolValue()) {
      _process();
    }
  }

  // call parent class and handle apply/autoApply and in/outputs
  WEMProcessor::handleNotification(field);
}

//----------------------------------------------------------------------------------
//! Code below is performed after module and network initialization.
//----------------------------------------------------------------------------------
void WEMCMSelectPatches::activateAttachments()
{
    ML_TRACE_IN("WEMCMSelectPatches::activateAttachments()")

    // call parent class
    WEMProcessor::activateAttachments();
}

//----------------------------------------------------------------------------------
//! The process method is called by the parent class.
//----------------------------------------------------------------------------------
void WEMCMSelectPatches::_process()
{
    ML_TRACE_IN("WEMCMSelectPatches::process()")

    // for time measurement and mouse cursor setting.
    _startProcessing();

    WEMProcessor::_process();

    // Now call the own processing routine.
    if (_inWEM!=NULL) {
      selectPatch();
      //std::cout<<"Process"<<std::endl;
    }

    // stop time measurement and mouse cursor resetting.
    _finishProcessing();

    // notify registered observer modules.
    _notifyObservers();

    _notifyFld->notifyAttachments();
}

//----------------------------------------------------------------------------------
//! The main processing routine. Implement your algorithm here.
//----------------------------------------------------------------------------------
void WEMCMSelectPatches::selectPatch()
{
  ML_TRACE_IN("WEMCMSelectPatches::selectPatch()")

  // Get number of input WEM patches
  const int numPatches = _inWEM->getNumWEMPatches();
  
  // Get settings
  int patchStartIndex = _patchStartIndexFld->getIntValue();
  int patchEndIndex = _patchEndIndexFld->getIntValue();
  if (_onePatchFld->getBoolValue()) {
    // Select only one patch, so set end index to start index
    patchEndIndex=patchStartIndex;
  }

  if (numPatches>0) {
    // Check index numbers
    if (patchStartIndex>patchEndIndex) {
      patchEndIndex=patchStartIndex;
      _patchEndIndexFld->setIntValue(patchEndIndex);
    }

    if ( _inWEM->getWEMPatchAt(0)->getPatchType() == WEM_PATCH_TRIANGLES ) 
    {
      // Output selected WEM patches
      for (int i=patchStartIndex; i<=patchEndIndex; ++i) {
        const int index = ( i + numPatches ) % numPatches;
        
        // Get input patch
        WEMTrianglePatch * inPatch = (WEMTrianglePatch*) _inWEM->getWEMPatchAt(index % numPatches);

        // Create new output patch
        WEMTrianglePatch * outTrianglePatch = NULL;
        ML_CHECK_NEW(outTrianglePatch, WEMTrianglePatch(*inPatch));
      
        _finish(outTrianglePatch);
        _addWEMPatch(outTrianglePatch);
      }
    }
    else if ( _inWEM->getWEMPatchAt(0)->getPatchType() == WEM_PATCH_POLYGONS )
    {
      // Output selected WEM patches
      for (int i=patchStartIndex; i<=patchEndIndex; ++i) {
        const int index = ( i + numPatches ) % numPatches;

        // Get input patch
        WEMPolygonPatch * inPatch = (WEMPolygonPatch*) _inWEM->getWEMPatchAt(index % numPatches);

        // Create new output patch
        WEMPolygonPatch * outTrianglePatch = NULL;
        ML_CHECK_NEW(outTrianglePatch, WEMPolygonPatch(*inPatch));

        _finish(outTrianglePatch);
        _addWEMPatch(outTrianglePatch);
      }
    }
    else if ( _inWEM->getWEMPatchAt(0)->getPatchType() == WEM_PATCH_QUADS )
    {
      // Output selected WEM patches
      for (int i=patchStartIndex; i<=patchEndIndex; ++i) {
        const int index = ( i + numPatches ) % numPatches;

        // Get input patch
        WEMQuadPatch * inPatch = (WEMQuadPatch*) _inWEM->getWEMPatchAt(index % numPatches);

        // Create new output patch
        WEMQuadPatch * outTrianglePatch = NULL;
        ML_CHECK_NEW(outTrianglePatch, WEMQuadPatch(*inPatch));

        _finish(outTrianglePatch);
        _addWEMPatch(outTrianglePatch);
      }
    }
    else
    {
      std::cout << "Input WEMPatch type not supported!" << std::endl;
      return;
    }
  }
}

ML_END_NAMESPACE
