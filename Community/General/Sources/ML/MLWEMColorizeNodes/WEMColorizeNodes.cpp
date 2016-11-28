//----------------------------------------------------------------------------------
//! The ML module class WEMColorizeNodes.
/*!
// \file    WEMColorizeNodes.cpp
// \author  Axel Newe
// \date    2015-12-03
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#include "WEMColorizeNodes.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(WEMColorizeNodes, WEMProcessor);


WEMColorizeNodes::WEMColorizeNodes (std::string type) : WEMProcessor(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inLUTFld = addBase("inLUT", NULL);
  _inLUTFld->addAllowedType<LUTFunction>();

  _RGBALUT = NULL;

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


WEMColorizeNodes::~WEMColorizeNodes()
{
  delete(_RGBALUT);
  _RGBALUT = NULL;
}


void WEMColorizeNodes::handleNotification (Field* field)
{
  // ... field notification handling code

  if (field == _inLUTFld)
  {
    /*
    LUTFunction *inLUTFunction = (LUTFunction*)_inLUTFld->getBaseValue();

    // Input object is a LUT?
    if (ML_BASE_IS_A(inLUTFunction, LUTFunction))
    {
      _minIndex->setDoubleValue(lutFunction->getNativeMinIndex());
      _maxIndex->setDoubleValue(lutFunction->getNativeMaxIndex());
    }
    */
    _process();
  }


  // call parent class and handle apply/autoApply and in/outputs
  WEMProcessor::handleNotification(field);
}


void WEMColorizeNodes::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMProcessor::activateAttachments();
}


void WEMColorizeNodes::_process()
{
  // for time measurement and mouse cursor setting.
  _startProcessing();

  WEMProcessor::_process();

  // Now call the own processing routine.
  _colorizeWEMNodes();

  // stop time measurement and mouse cursor resetting.
  _finishProcessing();

  // notify registered observer modules.
  _notifyObservers();
}

void WEMColorizeNodes::_setupLUT()
{
  if (_RGBALUT)
  {
    delete(_RGBALUT);
  }

  _RGBALUT = NULL;

  LUTFunction *inLUTFunction = (LUTFunction*)_inLUTFld->getBaseValue();

  // Input object is a LUT?
  if (ML_BASE_IS_A(inLUTFunction, LUTFunction))
  {
    //_minIndex->setDoubleValue(lutFunction->getNativeMinIndex());
    //_maxIndex->setDoubleValue(lutFunction->getNativeMaxIndex());

    _RGBALUT = new ml::LUTFloatRGBA(inLUTFunction, 0, 1);

    if (_RGBALUT != NULL)
    {
      if (!_RGBALUT->isValid())
      {
        delete(_RGBALUT);
        _RGBALUT = NULL;
      }
    }

  }

}

void WEMColorizeNodes::_colorizeWEMNodes()
{
  // If no other WEM input is explicitly declared,
  // the _outWEM is the WEM to work on.
  if ( (_outWEM) && (_inWEM) )
  {
    _setupLUT();

    // Input object is a LUT?
    if (_RGBALUT)
    {
      for (unsigned int i = 0; i < _outWEM->getNumWEMPatches(); i++){

        WEMPatch* wemPatch = _outWEM->getWEMPatchAt(i);

        const unsigned int numNodesInPatch = wemPatch->getNumNodes();
        ml::WEMPrimitiveValueList* lutValueList = wemPatch->getPrimitiveValueList("LUT");;

        for (unsigned int j = 0; j < numNodesInPatch; j++){

          WEMNode* node = wemPatch->getNodeAt(j);

          const double thisNodeLUTValue = node->getPrimitiveValue(lutValueList);

          float* rgbaArray = _RGBALUT->getColorAt(thisNodeLUTValue);

          node->setColor(Vector4(rgbaArray[0], rgbaArray[1], rgbaArray[2], rgbaArray[3]));
        }
      }
    }  
  }
}

ML_END_NAMESPACE