//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_LoadPointLineGeometry.
/*!
// \file    PDF3DFigurePage_LoadPointLineGeometry.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "PDF3DFigurePage_LoadPointLineGeometry.h"
#include "../shared/MLPDF_MarkerListTools.h"

#include <mlUnicode.h>

#include <iostream>
#include <fstream>

ML_START_NAMESPACE

using namespace mlPDF;

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDF3DFigurePage_LoadPointLineGeometry, Module);

//----------------------------------------------------------------------------------

PDF3DFigurePage_LoadPointLineGeometry::PDF3DFigurePage_LoadPointLineGeometry() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add output field
  (_outCachedPointPositionsListFld  = addBase("outCachedPointPositionsList"))->setBaseValueAndAddAllowedType(&_outCachedPointPositionsList);
  (_outCachedLinePositionsListFld   = addBase("outCachedLinePositionsList"))->setBaseValueAndAddAllowedType(&_outCachedLinePositionsList);
  (_outCachedLineConnectionsListFld = addBase("outCachedLineConnectionsList"))->setBaseValueAndAddAllowedType(&_outCachedLineConnectionsList);
  (_outPositionsListFld = addBase("outPositionsList"))->setBaseValueAndAddAllowedType(&_outPositionsList);
  //(_outConnectionsListFld           = addBase("outConnectionsList"))->setBaseValueAndAddAllowedType(&_outConnectionsList);
  (_outFibersFld = addBase("outFibers"))->setBaseValueAndAddAllowedType(&_outFiberSetContainer);

  // Add parameter fields
   _filenameFld = addString("filename","");
   _filterFld   = addString("filter","");

   _numberDelimiterFld      = addString("numberDelimiter",  ";");
   _numberDelimiterSpaceFld = addBool("numberDelimiterSpace",  true);
   _decimalSeparatorFld     = addString("decimalSeparator", ".");

   _loadFld     = addNotify("load");
   _unloadFld   = addNotify("unload");
   _autoLoadFld = addBool("autoLoad", false);

   _positionsLoadedFld   = addBool("positionsLoaded", false);
   _connectionsLoadedFld = addBool("connectionsLoaded", false);

   _addToPointCacheFld = addNotify("addToPointCache");
   _addToLineCacheFld  = addNotify("addToLineCache");
   _clearPointCacheFld = addNotify("clearPointCache");
   _clearLineCacheFld  = addNotify("clearLineCache");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDF3DFigurePage_LoadPointLineGeometry::~PDF3DFigurePage_LoadPointLineGeometry()
{
  _outPositionsList.clearList();
  _outConnectionsList.clearList();
  _outFiberSetContainer.deleteAllFiberSets();
  _outCachedPointPositionsList.clearList();
  _outCachedLinePositionsList.clearList();
  _outCachedLineConnectionsList.clearList();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::handleNotification(Field* field)
{
  if (field == _clearPointCacheFld)
  {
    _clearPointCache();
    return;
  }

  if (field == _clearLineCacheFld)
  {
    _clearLineCache();
    return;
  }

  if (field == _addToPointCacheFld)
  {
    _addToPointCache();
    return;
  }

  if (field == _addToLineCacheFld)
  {
    _addToLineCache();
    return;
  }

  if (field == _numberDelimiterFld)
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

void PDF3DFigurePage_LoadPointLineGeometry::_cropNumberDelimiter()
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

void PDF3DFigurePage_LoadPointLineGeometry::_cropDecimalSeparator()
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

void PDF3DFigurePage_LoadPointLineGeometry::_loadData()
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

  _updateOutputData();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_updateOutputData()
{
  _outPositionsList.clearList();
  _outConnectionsList.clearList();

  std::string newSetFilter = "";
  std::string newSetName = "";
  int newSetType = 0;

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

    StringVector thisDataLineComponents = PDFTools::stringSplit(thisLine, " ", false);
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
            newSetName = "";

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

  _createFibers();

  // Set validation fields
  _positionsLoadedFld->setBoolValue(_outPositionsList.size() > 0);
  _connectionsLoadedFld->setBoolValue(_outConnectionsList.size() > 0);

  // Update output
  _outPositionsListFld->touch();
  //_outConnectionsListFld->touch();
  _outFibersFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_unloadData()
{
  _inputFileLinesVector.clear();
  _updateOutputData();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_createFibers()
{
  PDFMarkerListTools::fillFiberSetContainerFromPositionsAndConnections(_outFiberSetContainer, _outPositionsList, _outConnectionsList);

  _outFibersFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_clearPointCache()
{
  _outCachedPointPositionsList.clearList();

  _outCachedPointPositionsListFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_clearLineCache()
{
  _outCachedLinePositionsList.clearList();
  _outCachedLineConnectionsList.clearList();

  _outCachedLinePositionsListFld->touch();
  _outCachedLineConnectionsListFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_addToPointCache()
{
  // Get highest type ID from positions
  int highestTypeID = 0;
  std::map<int, int> typeIDMap;

  // Scan all positions and get all type ID from them
  for (XMarkerList::const_iterator it = _outCachedPointPositionsList.cbegin(); it != _outCachedPointPositionsList.cend(); ++it)
  {
    XMarker thisMarker = *it;

    if (thisMarker.type > highestTypeID)
    {
      highestTypeID = thisMarker.type;
    }
  }

  // Add positions 
  for (XMarkerList::const_iterator it = _outPositionsList.cbegin(); it != _outPositionsList.cend(); ++it)
  {
    XMarker inMarker = *it;
    int inMarkerType = inMarker.type;
    int newMarkerType = inMarker.type;

    // Check if type ID must be modified
    if (inMarkerType <= highestTypeID)
    {
      std::map<int, int>::iterator findIt = typeIDMap.find(inMarkerType);

      if (findIt == typeIDMap.end())
      {
        // inMarkerType has not yet been added to map, so add it now
        newMarkerType = highestTypeID + 1;
        typeIDMap[inMarkerType] = newMarkerType;
        highestTypeID++;
      }
      else
      {
        newMarkerType = typeIDMap[inMarkerType];
      }
    }

    XMarker newCachedMarker(Vector6(inMarker.x(), inMarker.y(), inMarker.z(), inMarker.c(), inMarker.t(), inMarker.u()), newMarkerType, inMarker.name());

    _outCachedPointPositionsList.appendItem(newCachedMarker);
  }

  // Deselect items
  _outCachedPointPositionsList.selectItemAt(-1);

  // Notify observers
  _outCachedPointPositionsListFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_LoadPointLineGeometry::_addToLineCache()
{
  // Get highest type ID from positions and connections
  int highestTypeID = 0;
  std::map<int, int> typeIDMap;

  // Scan all positions and get all type ID from them
  for (XMarkerList::const_iterator it = _outCachedLinePositionsList.cbegin(); it != _outCachedLinePositionsList.cend(); ++it)
  {
    XMarker thisMarker = *it;

    if (thisMarker.type > highestTypeID)
    {
      highestTypeID = thisMarker.type;
    }
  }

  // Scan all connections and get all type ID from them
  for (IndexPairList::const_iterator it = _outCachedLineConnectionsList.cbegin(); it != _outCachedLineConnectionsList.cend(); ++it)
  {
    IndexPair thisPair = *it;

    if (thisPair.type > highestTypeID)
    {
      highestTypeID = thisPair.type;
    }
  }

  // Add positions 
  for (XMarkerList::const_iterator it = _outPositionsList.cbegin(); it != _outPositionsList.cend(); ++it)
  {
    XMarker inMarker  = *it;
    int inMarkerType  = inMarker.type;
    int newMarkerType = inMarker.type;

    // Check if type ID must be modified
    if (inMarkerType <= highestTypeID)
    {
      std::map<int, int>::iterator findIt = typeIDMap.find(inMarkerType);

      if (findIt == typeIDMap.end())
      {
        // inMarkerType has not yet been added to map, so add it now
        newMarkerType = highestTypeID + 1;
        typeIDMap[inMarkerType] = newMarkerType;
        highestTypeID++;
      }
      else
      {
        newMarkerType = typeIDMap[inMarkerType];
      }
    }
    
    XMarker newCachedMarker(Vector6(inMarker.x(), inMarker.y(), inMarker.z(), inMarker.c(), inMarker.t(), inMarker.u()), newMarkerType, inMarker.name());

    _outCachedLinePositionsList.appendItem(newCachedMarker);
  }

  // Add connections
  for (IndexPairList::const_iterator it = _outConnectionsList.cbegin(); it != _outConnectionsList.cend(); ++it)
  {
    IndexPair inPair = *it;
    int inPairType = inPair.type;
    int newPairType = inPair.type;

    // Check if type ID must be modified
    if (inPairType <= highestTypeID)
    {
      std::map<int, int>::iterator findIt = typeIDMap.find(inPairType);

      if (findIt == typeIDMap.end())
      {
        // newPairType has not yet been added to map -> skip this connection!
        inPairType = ML_INT16_MIN;
      }
      else
      {
        newPairType = typeIDMap[inPairType];
      }
    }

    if (inPairType > ML_INT16_MIN)
    {
      IndexPair newCachedPair(inPair.index1, inPair.index2, newPairType, inPair.name());

      _outCachedLineConnectionsList.appendItem(newCachedPair);
    }
  }

  // Deselect items
  _outCachedLinePositionsList.selectItemAt(-1);
  _outCachedLineConnectionsList.selectItemAt(-1);

  // Notify observers
  _outCachedLinePositionsListFld->touch();
  _outCachedLineConnectionsListFld->touch();
}

//----------------------------------------------------------------------------------



ML_END_NAMESPACE