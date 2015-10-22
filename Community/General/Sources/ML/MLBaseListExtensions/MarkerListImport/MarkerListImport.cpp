//----------------------------------------------------------------------------------
//! The ML module class MarkerListImport.
/*!
// \file    MarkerListImport.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-01-15
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "MarkerListImport.h"
#include "shared/MLBaseListExtensions_DataTypes.h"
#include "shared/MLBaseListExtensions_Tools.h"

#include <mlUnicode.h>

#include <iostream>
#include <fstream>

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(MarkerListImport, Module);

//----------------------------------------------------------------------------------

MarkerListImport::MarkerListImport() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add output field
  (_outMarkerListFld = addBase("outMarkerList"))->setBaseValueAndAddAllowedType(&_outMarkerList);
  
  // Add parameter fields
   _filenameFld = addString("filename","");
   _filterFld   = addString("filter","");

   _numberDelimiterFld      = addString("numberDelimiter",  ";");
   _numberDelimiterSpaceFld = addBool("numberDelimiterSpace",  true);
   _decimalSeparatorFld     = addString("decimalSeparator", ".");

   _loadFld     = addNotify("load");
   _unloadFld   = addNotify("unload");
   _autoLoadFld = addBool("autoLoad", false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

MarkerListImport::~MarkerListImport()
{

}

//----------------------------------------------------------------------------------

void MarkerListImport::handleNotification(Field* field)
{
  if  (field == _numberDelimiterFld) 
  {
    _cropNumberDelimiter();
  }

  if  (field == _decimalSeparatorFld) 
  {
    _cropDecimalSeparator();
  }

  if  (field == _unloadFld) 
  {
    _unloadData();
  }
  else if (field == _loadFld)
  {
     _loadData();
  }
  else if ( (_autoLoadFld->getBoolValue()) && ( (field == _filenameFld) || (field == _autoLoadFld) || (field == _filterFld) || (field == _decimalSeparatorFld) || (field == _numberDelimiterFld) || (field == _numberDelimiterSpaceFld) ) )
  {
     _loadData();
  }
  else  // Any other parameter changed...
  {
    _unloadData();
  }
}

//----------------------------------------------------------------------------------

void MarkerListImport::_cropNumberDelimiter()
{
  std::string delimiter = _numberDelimiterFld->getStringValue();

  if (delimiter.length() == 0)
  {
    delimiter = " ";
  }
  else 
  {
    delimiter = delimiter[0];
  }

  _numberDelimiterFld->setStringValue(delimiter);

  if (delimiter == ",")
  {
    _decimalSeparatorFld->setStringValue(".");
  }
}

//----------------------------------------------------------------------------------

void MarkerListImport::_cropDecimalSeparator()
{
  std::string separator = _decimalSeparatorFld->getStringValue();

  if (separator.length() == 0)
  {
    separator = ".";
  }
  else 
  {
    separator = separator[0];
  }

  _decimalSeparatorFld->setStringValue(separator);
}

//----------------------------------------------------------------------------------

void MarkerListImport::_loadData()
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

  std::string numberDelimiter = _numberDelimiterFld->getStringValue();

  if (_numberDelimiterSpaceFld->getBoolValue())
  {
    numberDelimiter = " ";
  }

  std::string decimalSeparator = _decimalSeparatorFld->getStringValue();

  if (inputStream.is_open())
  {
    while ( getline (inputStream, line) )
    {
      std::replace(line.begin(), line.end(), numberDelimiter[0], ' ');
      std::replace(line.begin(), line.end(), decimalSeparator[0], '.');

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

void MarkerListImport::_analyzeInputData()
{
  // Add any code that is needed to analyze input data here.
}
 
//----------------------------------------------------------------------------------

void MarkerListImport::_updateOutputData()
{
  _outMarkerList.clearList();

  std::string newSetType = "PointSet";
  int newPositionType = 0;

  std::string filterString = _filterFld->getStringValue();

  std::string numberDelimiter = _numberDelimiterFld->getStringValue();

  if (_numberDelimiterSpaceFld->getBoolValue())
  {
    numberDelimiter = " ";
  }

  std::string decimalSeparator = _decimalSeparatorFld->getStringValue();

  for(StringVector::iterator it = _inputFileLinesVector.begin(); it != _inputFileLinesVector.end(); ++it) 
  {
      std::string thisLine = *it;

      StringVector thisDataLineComponents = stringSplit(thisLine, " ", true);

      if (thisDataLineComponents.size() == 2)  // Must be a <Tag> <Type> pair
      {
        newSetType = thisDataLineComponents[0];

        if ((newSetType != "PointSet") && (newSetType != "LineSet"))
        {
          newSetType = "PointSet";
        }

        newPositionType = stringToInt(thisDataLineComponents[1]);

      }

      if (thisDataLineComponents.size() == 3)  // Must be the point coordinates
      {
        double x = stringToDouble(thisDataLineComponents[0]);
        double y = stringToDouble(thisDataLineComponents[1]);
        double z = stringToDouble(thisDataLineComponents[2]);

        // Add point
        Vector6 newPositionVector(x,y,z,0,0,0);
        XMarker newPosition(newPositionVector, newPositionType);

        if ( (filterString == "") || (filterString == newSetType) )          
        {
          _outMarkerList.appendItem(newPosition);
        }

      }

  }

  // Deselect items
  _outMarkerList.selectItemAt(-1);

  // Update output
  _outMarkerListFld->touch();
}

//----------------------------------------------------------------------------------

void MarkerListImport::_unloadData()
{
  _inputFileLinesVector.clear();
  _analyzeInputData();
  _updateOutputData();
}
 
ML_END_NAMESPACE