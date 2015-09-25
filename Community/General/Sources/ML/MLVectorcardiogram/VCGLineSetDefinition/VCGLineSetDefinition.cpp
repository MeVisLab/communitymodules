//----------------------------------------------------------------------------------
//! The ML module class VCGLineSetDefinition.
/*!
// \file    VCGLineSetDefinition.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "VCGLineSetDefinition.h"
#include "shared/MLVectorcardiogram_Tools.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(VCGLineSetDefinition, Module);

//----------------------------------------------------------------------------------

VCGLineSetDefinition::VCGLineSetDefinition() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  static const char* axesTypeOptions[2] = {"Axes",  "Grid"};

  // Add input/output fields
  (_inLinePositionsFld    = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);

  // Add UI fields
   _singleLoopFld               = addBool("singleLoop", false);
   _linesetDefinitionFld        = addString("linesetDefinition", "");
   _linesetStartColorFld        = addColor("linesetStartColor", Vector3(1,1,0));
   _linesetEndColorFld          = addColor("linesetEndColor", Vector3(1,0.5,0));

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

VCGLineSetDefinition::~VCGLineSetDefinition()
{

}

//----------------------------------------------------------------------------------

void VCGLineSetDefinition::handleNotification(Field* field)
{
  // Always immediately update output
  if (field)
  {  
    _updateOutput();
  }
}

//----------------------------------------------------------------------------------

void VCGLineSetDefinition::_updateOutput()
{
  _inLinePositions.clear();
  std::string linesetDefinition = "";

  XMarkerList* inList = static_cast<XMarkerList*>(_inLinePositionsFld->getBaseValue());

  if (inList)
  {
    _inLinePositions = *inList;
     
    size_t numLinePositions = _inLinePositions.size();

    if (numLinePositions > 0)
    {

      int lastPositionType = _inLinePositions[numLinePositions-1].type;

      if (_singleLoopFld->getBoolValue())
      {
        std::string positionTypes = "";

        for (int i = 1; i <= lastPositionType; i++)
        {
          if (i != 1)
          {
            positionTypes += ",";
          }
          positionTypes += intToString(i);
        }

        linesetDefinition += "<LineSet>\n";
        linesetDefinition += "<ObjectName>All Beats</ObjectName>\n";
        linesetDefinition += "<PositionTypes>" + positionTypes + "</PositionTypes>\n";
        linesetDefinition += "<ConnectionTypes>simple</ConnectionTypes>\n";
        linesetDefinition += "<Color>" + _linesetStartColorFld->getStringValue() + "</Color>\n";
        linesetDefinition += "\n";
      }
      else
      {
        Vector3 startColorVec  = _linesetStartColorFld->getVector3Value();
        Vector3 endColorVec    = _linesetEndColorFld->getVector3Value();

        int colorSteps = (int)(lastPositionType - 1);

        if (0 == colorSteps)
        {
          colorSteps = 1;
        }

        double colorStepR = (endColorVec.x - startColorVec.x) / colorSteps;
        double colorStepG = (endColorVec.y - startColorVec.y) / colorSteps;
        double colorStepB = (endColorVec.z - startColorVec.z) / colorSteps;

        for (int i = 1; i <= lastPositionType; i++)
        {
          double colorR = startColorVec.x + (i-1) * colorStepR;
          double colorG = startColorVec.y + (i-1) * colorStepG;
          double colorB = startColorVec.z + (i-1) * colorStepB;

          std::string colorString = floatToString((float)colorR) + " " + floatToString((float)colorG) + " " + floatToString((float)colorB);

          linesetDefinition += "<LineSet>\n";
          linesetDefinition += "<ObjectName>Beat " + intToString(i) + "</ObjectName>\n";
          linesetDefinition += "<PositionTypes>" + intToString(i) + "</PositionTypes>\n";
          linesetDefinition += "<ConnectionTypes>simple</ConnectionTypes>\n";
          linesetDefinition += "<Color>" + colorString + "</Color>\n";
          linesetDefinition += "\n";
        }

      } // if (_singleLoopFld->getBoolValue())

    } // if (numLinePositions > 0)

  }

  _linesetDefinitionFld->setStringValue(linesetDefinition);
}
 
//----------------------------------------------------------------------------------


ML_END_NAMESPACE