//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_Utils.
/*!
// \file    PDF3DFigurePage_Utils.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDF3DFigurePage_Utils.h"

// Project includes
#include <MLPDF_Defines.h>
#include <MLPDF_SpecificationGenerator.h>


ML_START_NAMESPACE

using namespace mlPDF;

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_process()
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

void PDF3DFigurePage_Utils::_inWEMChanged()
{
  WEMProcessor::_inWEMChanged();

  handleNotificationOff();

  _updateAvailableWEMPatchesFld(_inWEM);

  handleNotificationOn();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateAvailableWEMPatchesFld(WEMPtr wem, std::string defaultEntry)
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
        patchDescription = NOLABELSPECIFIED;
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

void PDF3DFigurePage_Utils::_selectedWEMPatchChanged(WEMPtr wem)
{
  int patchId = _getModelIDFromString(_selectedWEMPatchFld->getStringValue());

  if (patchId != ML_INT16_MIN)
  {
    _selectedWEMPatchIdFld->setIntValue(patchId);
  }

  _selectedWEMPatchIdChanged(wem);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_selectedWEMPatchIdChanged(WEMPtr wem)
{
  std::string newLabel = "";
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

        groupPath = PDFTools::getSpecificParameterFromWEMDescription(description, "GroupPath"); 
        modelColor = PDFTools::getSpecificParameterFromWEMDescription(description, "Color");
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

    Vector4 colorVector = PDFTools::getColorVec4FromString(modelColor, Vector4(0));

    _selectedWEMPatchColorFld->setColorValue(colorVector[0], colorVector[1], colorVector[2]);
    _selectedWEMPatchColorAlphaFld->setFloatValue(colorVector[3]);
  }

}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateSelectedWEMPatchLabel()
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

void PDF3DFigurePage_Utils::_updateSelectedWEMPatchDescription()
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
          modelColor = PDFTools::FormatColorString(_selectedWEMPatchColorFld->getColorValue(), _selectedWEMPatchColorAlphaFld->getFloatValue());
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

void PDF3DFigurePage_Utils::_updateWEMPatchNodesColor()
{
  if (_outWEM) 
  {
    const int patchId = _selectedWEMPatchIdFld->getIntValue();

    if (patchId > -1)
    {
      WEMPatch* outWEMPatch = _outWEM->getWEMPatchById(patchId);

      if (outWEMPatch)
      {
        Vector4 newColor(0, 0, 0, 1);

        if (!_selectedWEMPatchUseDefaultColorFld->getBoolValue())
        {
          newColor = Vector4(_selectedWEMPatchColorFld->getColorValue(), _selectedWEMPatchColorAlphaFld->getFloatValue());
        }

        const int numNodes = outWEMPatch->getNumNodes();

        for (int i = 0; i < numNodes; i++)
        {
          outWEMPatch->getNodeAt(i)->setColor(newColor);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_processPatch(unsigned int patchIndex)
{
  WEMPatch* inWEMPatch = _inWEM->getWEMPatchAt(patchIndex);
  WEMPatch* outWEMPatch = _outWEM->getWEMPatchAt(patchIndex);

  if (inWEMPatch && outWEMPatch)
  {
    outWEMPatch->setId(inWEMPatch->getId());
    const int numNodes = outWEMPatch->getNumNodes();

    std::string label       = inWEMPatch->getLabel();
    std::string description = inWEMPatch->getDescription();

    std::string modelName = "ModelName=" + label +";";
    std::string groupPath = mlPDF::PDFTools::getSpecificParameterFromWEMDescription(description, "GroupPath"); 
    std::string modelColor = mlPDF::PDFTools::getSpecificParameterFromWEMDescription(description, "Color");

    if (groupPath != "")
    {
      groupPath = "GroupPath=" + groupPath + ";";
    }

    if (modelColor == "")
    {
      // Set default color to black (normally it is white...)
      for (int n = 0; n < numNodes; n++)
      {
        outWEMPatch->getNodeAt(n)->setColor(Vector4(0, 0, 0, 1));
      }
    }
    else
    {
      Vector4 colorVector = PDFTools::getColorVec4FromString(modelColor, Vector4(0));

      modelColor = "Color=" + modelColor + ";";

      for (int n = 0; n < numNodes; n++)
      {
        outWEMPatch->getNodeAt(n)->setColor(colorVector);
      }
    }

    //modelColor = "Color=" + mlPDF::USEVERTEXCOLORS + ";";

    outWEMPatch->setDescription(modelName + groupPath + modelColor);

  }
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE