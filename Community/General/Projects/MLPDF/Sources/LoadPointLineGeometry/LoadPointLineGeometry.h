//----------------------------------------------------------------------------------
//! The ML module class LoadPointLineGeometry.
/*!
// \file    LoadPointLineGeometry.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
// 
*/
//----------------------------------------------------------------------------------


#ifndef __LoadPointLineGeometry_H
#define __LoadPointLineGeometry_H


// Local includes
#include "MLPDFSystem.h"
#include "../shared/MLPDF_Tools.h"

// Project includes
#include <IndexPairList.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <mlFiberSet.h>


ML_START_NAMESPACE

//! 
class MLPDF_EXPORT LoadPointLineGeometry : public Module
{
public:

  //! Constructor.
  LoadPointLineGeometry();

  //! Destructor.
  ~LoadPointLineGeometry();

  // Output fields
  BaseField*   _outPositionsListFld;
  BaseField*   _outConnectionsListFld;
  BaseField*   _outFibersFld;
  BaseField*   _outCachedPositionsListFld;
  BaseField*   _outCachedConnectionsListFld;

  // UI fields
  StringField* _filenameFld;
  StringField* _filterFld;

  StringField* _numberDelimiterFld;
  BoolField*   _numberDelimiterSpaceFld;
  StringField* _decimalSeparatorFld;

  NotifyField* _loadFld;
  NotifyField* _unloadFld;
  BoolField*   _autoLoadFld;

  BoolField*   _positionsLoadedFld;
  BoolField*   _connectionsLoadedFld;

  NotifyField* _addToCacheFld;
  NotifyField* _clearCacheFld;
  BoolField*   _autoAddToCacheFld;

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // Globals
  XMarkerList           _outPositionsList;
  IndexPairList         _outConnectionsList;
  FiberSetContainer     _outFiberSetContainer;
  XMarkerList           _outCachedPositionsList;
  IndexPairList         _outCachedConnectionsList;

  StringVector          _inputFileLinesVector;
  int                   _dataStartIndex;


  // Load data
  void _loadData();

  // Unload data
  void _unloadData();

  // Clear cached data
  void _clearCache();

  // Add loaded data to cache
  void _addToCache();

  // Analyze input data
  void _analyzeInputData();

  // Update output data
  void _updateOutputData();

  // Crop the delimiter to the first character
  void _cropNumberDelimiter();

  // Crop the decimal separator to the first character
  void _cropDecimalSeparator();

  // Create fiberset container
  void _createFibers();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(LoadPointLineGeometry)
};


ML_END_NAMESPACE

#endif // __LoadPointLineGeometry_H