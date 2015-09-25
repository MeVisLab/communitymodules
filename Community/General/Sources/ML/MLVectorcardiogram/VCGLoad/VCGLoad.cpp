//----------------------------------------------------------------------------------
//! The ML module class VCGLoad.
/*!
// \file    VCGLoad.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "VCGLoad.h"
#include "shared/MLVectorcardiogram_DataTypes.h"
#include "shared/MLVectorcardiogram_Tools.h"

#include <mlUnicode.h>

#include <iostream>
#include <fstream>

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(VCGLoad, Module);

//----------------------------------------------------------------------------------

VCGLoad::VCGLoad() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_outFiberSetFld  = addBase("outFiberSetFld"))->setBaseValueAndAddAllowedType(&_outFiberSet);
  (_outPointSetFld  = addBase("outPointSetFld"))->setBaseValueAndAddAllowedType(&_outPointSet);
  
   _filenameFld = addString("filename","");

   _loadFld     = addNotify("load");
   _unloadFld   = addNotify("unload");
   _autoLoadFld = addBool("autoLoad", false);

   _outputValidFld = addBool("outputValid", false);

  _numBeatsFld         = addInt("numBeats", 1);
  _numBeatsFromFileFld = addBool("numBeatsFromFile", false);

  _numDataLinesFld                = addInt("numDataLines", 0);
  _numDataLinesPerBeatFld         = addInt("numDataLinesPerBeat", 0);
  _numDataLinesMatchesNumBeatsFld = addBool("numDataLinesMatchesNumBeats", false);


  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();

}

//----------------------------------------------------------------------------------

VCGLoad::~VCGLoad()
{

}

//----------------------------------------------------------------------------------

void VCGLoad::handleNotification(Field* field)
{
  if (field == _unloadFld)
  {
    _unloadData();
  }
  else if (field == _loadFld)
  {
     _loadData();
  }
  else if ( (_autoLoadFld->getBoolValue()) && ( (field == _filenameFld) || (field == _autoLoadFld) ) )
  {
     _loadData();
  }
  else if (field == _numBeatsFld)
  {
     _updateOutputData();
  }

}

//----------------------------------------------------------------------------------

void VCGLoad::_loadData()
{
  std::string filename = _filenameFld->getStringValue();

  if (filename == "") 
  {
    return;
  }

  std::ifstream inputStream;

#ifdef WIN32
#if (_MSC_VER >= 1310)
  MLuint16* filename16 = static_cast<MLuint16*>(MLConvertUTF8ToUTF16(filename.c_str()));
#else
  const char* filename16 = filename.c_str();
#endif
  if (filename16 != NULL) 
  {
    inputStream.open( filename16 );
#if (_MSC_VER >= 1310)
  MLFree(filename16);
#endif
  } 
  else 
  {
    std::cerr << "Could not convert filename.\n";
  }
#else
  inputStream.open( filename.c_str() );
#endif

  std::string line;
  _inputFileLinesVector.clear();

  if (inputStream.is_open())
  {
    while ( getline (inputStream, line) )
    {
      std::replace(line.begin(), line.end(), ',', '.');
      std::replace(line.begin(), line.end(), ';', ' ');

      _inputFileLinesVector.push_back(line);
    }
    inputStream.close();
  }
  else
  {
    std::cerr << "Unable to open file.\n"; 
  }

  _analyzeInputData();
  _updateOutputData();

}

//----------------------------------------------------------------------------------

void VCGLoad::_analyzeInputData()
{
  size_t numDataLines = 0;
  int  numBeats = 1;
  bool numBeatsFromFile = false;

  if (_inputFileLinesVector.size() > 0)
  {
    _dataStartIndex = 1;

    if (_inputFileLinesVector[0] == "VCG")
    {
      StringVector stringComponents = stringSplit(_inputFileLinesVector[1], " ", false);
      
      if (2 == stringComponents.size()) 
      {
        std::string beatsString = stringComponents[0];
        stringToLower(beatsString);

        if (beatsString == "beats")
        {
          numBeats = stringToInt(stringComponents[1]);
          _dataStartIndex++;
          
          if (numBeats == 0)
          {
            numBeats = 1;
          }
          else
          {
            numBeatsFromFile = true;
          }
        }

      }

      numDataLines = _inputFileLinesVector.size() - _dataStartIndex; 

      _numBeatsFld->setIntValue(numBeats);
      _numBeatsFromFileFld->setBoolValue(numBeatsFromFile);
      _numDataLinesFld->setIntValue((MLint)numDataLines);


    } // if (_inputFileLinesVector[0] == "VCG")

  } // if (_inputFileLinesVector.size() > 0)

  _numBeatsFld->setIntValue(numBeats);
  _numBeatsFromFileFld->setBoolValue(numBeatsFromFile);
  _numDataLinesFld->setIntValue((MLint)numDataLines);

}
 
//----------------------------------------------------------------------------------

void VCGLoad::_updateOutputData()
{
  MLint numDataLines = _numDataLinesFld->getIntValue();
  MLint numDataLinesPerBeat = 0;  
  bool numDataLinesMatchesNumBeats = false;
  bool outputValid = false;

  _outFiberSet.deleteAllFibers();
  _outPointSet.clear();

  if (numDataLines > 0)
  {

    MLint numBeats = _numBeatsFld->getIntValue();

    if (numBeats < 1) // Should not happen, but let's be careful...
    {
      numBeats = 1; 
    }

    numDataLinesPerBeat = numDataLines / numBeats;  // Integer division is intentional.
    numDataLinesMatchesNumBeats = (numDataLines % numBeats == 0);
    int currentDataIndex = _dataStartIndex;

    for (int thisBeat = 0; thisBeat < numBeats; thisBeat++)
    {
      ml::FiberSetContainer::FiberSet::Fiber *newFiber = _outFiberSet.createFiber();  // Automatically appends the fiber to the fiber set

      int beatLabelInt = thisBeat + 1;
      std::string beatLabel = "Beat " + intToString(beatLabelInt);

      newFiber->setLabel((float)(beatLabelInt));

      for (int thisDataLine = 0; thisDataLine < numDataLinesPerBeat; thisDataLine++)
      {
        if (currentDataIndex < _inputFileLinesVector.size())
        {
          StringVector thisDataLineComponents = stringSplit(_inputFileLinesVector[currentDataIndex], " ", true);

          double x = stringToDouble(thisDataLineComponents[0]);
          double y = stringToDouble(thisDataLineComponents[1]);
          double z = stringToDouble(thisDataLineComponents[2]);

          // Add fiber
          ml::FiberSetContainer::FiberSet::Fiber::FiberPoint newFiberPoint(x,y,z,0,0,0,0,0);

          newFiber->appendPoint(newFiberPoint);

          if (0 == thisDataLine)
          {
            newFiber->setInitialFiberPoint(newFiberPoint);
          }

          newFiber->markEndPoint();

          // Add point
          Vector6 newPositionVector(x,y,z,0,0,0);
          XMarker newPosition(newPositionVector, beatLabelInt, beatLabel.c_str());
          _outPointSet.appendItem(newPosition);
        }
        else
        { 
          break;
        }

        currentDataIndex++;

      }

    }

    outputValid = true;
  }

  _numDataLinesPerBeatFld->setIntValue(numDataLinesPerBeat);
  _numDataLinesMatchesNumBeatsFld->setBoolValue(numDataLinesMatchesNumBeats);
  _outputValidFld->setBoolValue(outputValid);
  _outFiberSetFld->touch();
  _outPointSetFld->touch();
}

//----------------------------------------------------------------------------------

void VCGLoad::_unloadData()
{
  _inputFileLinesVector.clear();
  _analyzeInputData();
  _updateOutputData();
}
 
ML_END_NAMESPACE