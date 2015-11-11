//----------------------------------------------------------------------------------
//! The ML module class PDFUtils.
/*!
// \file    PDFUtils_Meshes.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Utilities for PDF creation.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFUtils.h"
#include "../shared/SpecificationGenerator/MLPDF_SpecificationGenerator.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFUtils::_process()
{
  // Start time measurement and mouse cursor setting.
  _startProcessing();

  WEMProcessor::_process();

  if (_inWEM && _outWEM)
  {
    for (unsigned int i = 0; i < _inWEM->getNumWEMPatches(); i++)
    {
      _processPatch(i);
    }    
  }

  // Stop time measurement and mouse cursor resetting.
  // Use WEMModule version to avoid overwriting of label/description!!
  WEMModule::_finishProcessing();

  // Notify registered observer modules.
  _notifyObservers();
}

//----------------------------------------------------------------------------------

void PDFUtils::_inWEMChanged()
{
  WEMProcessor::_inWEMChanged();

  handleNotificationOff();

  _updateAvailableWEMPatchesFld(_inWEM);

  handleNotificationOn();
}

//----------------------------------------------------------------------------------

void PDFUtils::_updateAvailableWEMPatchesFld(WEMPtr wem, std::string defaultEntry)
{
  if (wem)
  {
    std::string availableWEMs = "";
    WEMPatch* wemPatch = NULL;

    for (unsigned int i = 0; i < wem->getNumWEMPatches(); i++)
    {
      wemPatch = wem->getWEMPatchAt(i);

      if (!wemPatch)
      {
        continue;
      }

      if (i > 0)
      {
        availableWEMs.append("|");
      }

      std::string patchLabel = wemPatch->getLabel();
      std::string patchDescription = patchLabel;

      if (patchDescription == "")
      {
        patchDescription = "[no label specified]";
      }

      patchDescription.append(" {ID=");
      patchDescription.append(intToString(wemPatch->getId()));
      patchDescription.append("}");

      availableWEMs.append(patchDescription);

      if (defaultEntry == patchLabel)
      {
        defaultEntry = patchDescription;
      }

      if ((i == 0) && (defaultEntry == ""))
      {
        _selectedWEMPatchFld->setStringValue(patchDescription);
        _selectedWEMPatchChanged(wem);
      }
    }

    _availableWEMPatchesFld->setStringValue(availableWEMs);

    if (defaultEntry != "")
    {
      _selectedWEMPatchFld->setStringValue(defaultEntry);
      _selectedWEMPatchChanged(wem);
    }
  }
  else
  {
    _selectedWEMPatchFld->setStringValue("");
    _availableWEMPatchesFld->setStringValue("");
  }
}

//----------------------------------------------------------------------------------

void PDFUtils::_selectedWEMPatchChanged(WEMPtr wem)
{
  std::string selectedWEMPatchString = _selectedWEMPatchFld->getStringValue();

  if (selectedWEMPatchString != "")
  {
    // Get patch ID from string
    size_t startIDPosition = selectedWEMPatchString.find("{ID=");

    if (startIDPosition != std::string::npos)
    {
      std::string idSectionPlusRest = selectedWEMPatchString.substr(startIDPosition);
      size_t endIDPosition = idSectionPlusRest.find("}");

      if (endIDPosition != std::string::npos)
      {
        std::string idSection = idSectionPlusRest.substr(4, endIDPosition - 4);

        int patchId = stringToInt(idSection);

        _selectedWEMPatchIdFld->setIntValue(patchId);

      }

    }

  } // if (selectedWEMPatchString != "")

  _selectedWEMPatchIdChanged(wem);
}

//----------------------------------------------------------------------------------

void PDFUtils::_selectedWEMPatchIdChanged(WEMPtr wem)
{
  std::string newLabel = "[undefined]";
  std::string groupPath = ""; 
  std::string modelColor = "";

  if (wem)
  {
    const int patchId = _selectedWEMPatchIdFld->getIntValue();

    if (patchId >= 0)
    {
      WEMPatch* patch = wem->getWEMPatchById(patchId);

      if (patch)
      {
        newLabel = patch->getLabel();

        std::string description = patch->getDescription();

        groupPath = getSpecificParameterFromWEMDescription(description, "GroupPath"); 
        modelColor = getSpecificParameterFromWEMDescription(description, "Color");
      }

    }

  }

  _selectedWEMPatchNewLabelFld->setStringValue(newLabel);
  _selectedWEMPatchGroupPathFld->setStringValue(groupPath);

  if (modelColor == "")
  {
    _selectedWEMPatchUseDefaultColorFld->setBoolValue(true);
  }
  else
  {
    _selectedWEMPatchUseDefaultColorFld->setBoolValue(false);

    Vector4 colorVector = getColorVec4(modelColor, Vector4(0));

    _selectedWEMPatchColorFld->setColorValue(colorVector[0], colorVector[1], colorVector[2]);
    _selectedWEMPatchColorAlphaFld->setFloatValue(colorVector[3]);
  }

}

//----------------------------------------------------------------------------------

void PDFUtils::_updateSelectedWEMPatchLabel()
{
  const std::string newLabel = _selectedWEMPatchNewLabelFld->getStringValue();

  if (_outWEM) 
  {
    const int patchId = _selectedWEMPatchIdFld->getIntValue();

    if (patchId > -1)
    {
      WEMPatch* outWEMPatch = _outWEM->getWEMPatchById(patchId);

      if (outWEMPatch)
      {
        outWEMPatch->setLabel(newLabel);
        _updateAvailableWEMPatchesFld(_outWEM, newLabel);
        _notifyObservers();
      }
    }
  }
}

//----------------------------------------------------------------------------------

void PDFUtils::_updateSelectedWEMPatchDescription()
{

  if (_outWEM) 
  {
    const int patchId = _selectedWEMPatchIdFld->getIntValue();

    if (patchId > -1)
    {
      WEMPatch* outWEMPatch = _outWEM->getWEMPatchById(patchId);

      if (outWEMPatch)
      {
        std::string modelName = "ModelName=" + outWEMPatch->getLabel()+";";
        std::string groupPath = _selectedWEMPatchGroupPathFld->getStringValue(); 
        std::string modelColor = "";

        if (!_selectedWEMPatchUseDefaultColorFld->getBoolValue())
        {
          modelColor = mlPDF::SpecificationGenerator::FormatColorString(_selectedWEMPatchColorFld->getColorValue(), _selectedWEMPatchColorAlphaFld->getFloatValue());
        }

        if (groupPath != "")
        {
          groupPath = "GroupPath=" + groupPath + ";";
        }

        if (modelColor != "")
        {
          modelColor = "Color=" + modelColor + ";";
        }

        outWEMPatch->setDescription(modelName + groupPath + modelColor);
        _notifyObservers();
      }
    }
  }

}

//----------------------------------------------------------------------------------

void PDFUtils::_updateWEMPatchNodesColor()
{
  if (_outWEM) 
  {
    const int patchId = _selectedWEMPatchIdFld->getIntValue();

    if (patchId > -1)
    {
      WEMPatch* outWEMPatch = _outWEM->getWEMPatchById(patchId);

      if (outWEMPatch)
      {
        // ToDo: change patch nodes color!
      }
    }
  }
}

//----------------------------------------------------------------------------------

void PDFUtils::_processPatch(unsigned int patchIndex)
{
  WEMPatch* inWEMPatch = _inWEM->getWEMPatchAt(patchIndex);
  WEMPatch* outWEMPatch = _outWEM->getWEMPatchAt(patchIndex);

  if (inWEMPatch && outWEMPatch)
  {
    outWEMPatch->setId(inWEMPatch->getId());
  }
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE