//----------------------------------------------------------------------------------
//! The ML module class LoadPointLineGeometry.
/*!
// \file    LoadPointLineGeometry.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "LoadPointLineGeometry.h"
#include "shared/MLBaseListExtensions_DataTypes.h"
#include "shared/MLBaseListExtensions_Tools.h"

#include <mlUnicode.h>

#include <iostream>
#include <fstream>

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(LoadPointLineGeometry, Module);

//----------------------------------------------------------------------------------

LoadPointLineGeometry::LoadPointLineGeometry() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add output field
  (_outPositionsListFld   = addBase("outPositionsList"))->setBaseValueAndAddAllowedType(&_outPositionsList);
  (_outConnectionsListFld = addBase("outConnectionsList"))->setBaseValueAndAddAllowedType(&_outConnectionsList);
  (_outFibersFld          = addBase("outFibers"))->setBaseValueAndAddAllowedType(&_outFiberSetContainer);

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

LoadPointLineGeometry::~LoadPointLineGeometry()
{
  _outPositionsList.clearList();
  _outConnectionsList.clearList();
  _outFiberSetContainer.deleteAllFiberSets();
}

//----------------------------------------------------------------------------------

void LoadPointLineGeometry::handleNotification(Field* field)
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

void LoadPointLineGeometry::_cropNumberDelimiter()
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

void LoadPointLineGeometry::_cropDecimalSeparator()
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

void LoadPointLineGeometry::_loadData()
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

void LoadPointLineGeometry::_analyzeInputData()
{
  // Add any code that is needed to analyze input data here.
}
 
//----------------------------------------------------------------------------------

void LoadPointLineGeometry::_updateOutputData()
{
  _outPositionsList.clearList();
  _outConnectionsList.clearList();
  _outFiberSetContainer.deleteAllFiberSets();

  std::string newSetFilter = "";
  int newSetType = 0;
  std::string newSetName = "";

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

    StringVector thisDataLineComponents = stringSplit(thisLine, " ", false);
    size_t thisDataLineNumComponents = thisDataLineComponents.size();

    if (thisDataLineNumComponents > 0)
    {
      // Check if new set definition is started
      if (thisDataLineNumComponents > 1)  // Must be at least a a <Tag> <Type> {<Name>} pair/triple
      {
        std::string firstComponent = thisDataLineComponents[0];

        if (isalpha(firstComponent[0]))
        {
          newSetFilter = firstComponent;
          newSetType = stringToInt(thisDataLineComponents[1]);

          if (thisDataLineNumComponents > 2)
          {
            // Append all remaining fragments to the name
            for (size_t i = 2; i < thisDataLineNumComponents; i++)
            {
              if (newSetName != "")
              {
                newSetName += " ";
              }

              newSetName += thisDataLineComponents[i];
            }
          }
          else
          {
            newSetName = "";
          }

          continue;
        }
      }

      if ((filterString == "") || (filterString == newSetFilter))
      {
        // If it is a list of connections
        if (thisDataLineNumComponents == 2)
        {
          // Add point (only the first 3 components)         
          int start = stringToInt(thisDataLineComponents[0]);
          int end = stringToInt(thisDataLineComponents[1]);

          IndexPair newConnection(start, end, newSetType, newSetName.c_str());
          _outConnectionsList.appendItem(newConnection);
        }
        // If it is a list of positions
        else if (thisDataLineNumComponents > 2)
        {
          // Add point (only the first 3 components)         
          double x = stringToDouble(thisDataLineComponents[0]);
          double y = stringToDouble(thisDataLineComponents[1]);
          double z = stringToDouble(thisDataLineComponents[2]);

          XMarker newPosition(Vector6(x, y, z, 0, 0, 0), newSetType, newSetName.c_str());
          _outPositionsList.appendItem(newPosition);
        }

      }

    } // if (thisDataLineNumComponents > 0)

  } // for(StringVector::iterator it

  // Deselect items
  _outPositionsList.selectItemAt(-1);
  _outConnectionsList.selectItemAt(-1);

  // Create fibers
  _createFibers();

  // Update output
  _outPositionsListFld->touch();
  _outConnectionsListFld->touch();
  _outFibersFld->touch();
}

//----------------------------------------------------------------------------------

void LoadPointLineGeometry::_createFibers()
{
  std::set<int> typeIDsSet;

  // Step 1: Scan all positions and get all type ID from them
  for (XMarkerList::const_iterator it = _outPositionsList.cbegin(); it != _outPositionsList.cend(); ++it)
  {
    XMarker thisMarker = *it;
    typeIDsSet.insert(thisMarker.type);
  }

  // Step 2: Now create fibers
  for (std::set<int>::const_iterator typeIdIterator = typeIDsSet.cbegin(); typeIdIterator != typeIDsSet.cend(); ++typeIdIterator)
  {
    int thisTypeID = *typeIdIterator;

    FiberSetContainer::FiberSet* newFiberSet = _outFiberSetContainer.createFiberSet("Key");
    newFiberSet->setColor(Vector3(1, 0, 0));
    newFiberSet->setLabel("FiberSet Label");

    // Add all connections to the temporary connections list if type ID matches
    XMarkerList   workPositions;
    for (XMarkerList::const_iterator outPositionsIterator = _outPositionsList.cbegin(); outPositionsIterator != _outPositionsList.cend(); ++outPositionsIterator)
    {
      XMarker thisMarker = *outPositionsIterator;

      if (thisMarker.type == thisTypeID)
      {
        workPositions.push_back(thisMarker);
      }
    }
    MLint workPositionsSize = (MLint)workPositions.size();

    // Add all connections to the temporary connections list if type ID matches
    IndexPairList workConnections;
    for (IndexPairList::const_iterator outConnectionsIterator = _outConnectionsList.cbegin(); outConnectionsIterator != _outConnectionsList.cend(); ++outConnectionsIterator)
    {
      IndexPair thisPair = *outConnectionsIterator;

      if (thisPair.type == thisTypeID)
      {
        workConnections.push_back(thisPair);
      }
    }

    // If temporary connections list is still empty at this point: create default list
    if (workConnections.size() == 0)
    {



    }

    // Now finally create fibers from connections
    for (IndexPairList::const_iterator workListIterator = workConnections.cbegin(); workListIterator != workConnections.cend(); ++workListIterator)
    {
      IndexPair thisWorkPair = *workListIterator;

      MLint startIndex = thisWorkPair.index1;
      MLint endIndex = thisWorkPair.index2;

      if ((startIndex < workPositionsSize) && (endIndex < workPositionsSize))
      {
        XMarker startMarker = workPositions[startIndex];
        XMarker endMarker = workPositions[endIndex];

        FiberSetContainer::FiberSet::Fiber* newFiber = newFiberSet->createFiber();

        FiberSetContainer::FiberSet::Fiber::FiberPoint startPoint;
        startPoint.setCoordinates(startMarker.x(), startMarker.y(), startMarker.z());
        FiberSetContainer::FiberSet::Fiber::FiberPoint endPoint;
        endPoint.setCoordinates(endMarker.x(), endMarker.y(), endMarker.z());

        newFiber->appendPoint(startPoint);
        newFiber->appendPoint(endPoint);
        newFiber->setLabel(1.0);

        newFiberSet->appendFiber(newFiber);
      }
    }

    _outFiberSetContainer.appendFiberSet(newFiberSet);

    workPositions.clearList();
    workConnections.clearList();
  }

}

//----------------------------------------------------------------------------------

void LoadPointLineGeometry::_unloadData()
{
  _inputFileLinesVector.clear();
  _analyzeInputData();
  _updateOutputData();
}
 
ML_END_NAMESPACE