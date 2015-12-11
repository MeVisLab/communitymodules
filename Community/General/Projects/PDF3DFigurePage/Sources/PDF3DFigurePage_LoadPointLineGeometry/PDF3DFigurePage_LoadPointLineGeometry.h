//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_LoadPointLineGeometry.
/*!
// \file    PDF3DFigurePage_LoadPointLineGeometry.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
// 
*/
//----------------------------------------------------------------------------------


#ifndef __LoadPointLineGeometry_H
#define __LoadPointLineGeometry_H


// Local includes
#include "PDF3DFigurePageSystem.h"

// Project includes
#include <MLPDF_Tools.h>
#include <IndexPairList.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <mlFiberSet.h>


ML_START_NAMESPACE

//! 
class PDF3DFIGUREPAGE_EXPORT PDF3DFigurePage_LoadPointLineGeometry : public Module
{
public:

  //! Constructor.
  PDF3DFigurePage_LoadPointLineGeometry();

  //! Destructor.
  ~PDF3DFigurePage_LoadPointLineGeometry();

  // Output fields
  BaseField*   _outPositionsListFld;
  //BaseField*   _outConnectionsListFld;
  BaseField*   _outFibersFld;
  BaseField*   _outCachedPointPositionsListFld;
  BaseField*   _outCachedLinePositionsListFld;
  BaseField*   _outCachedLineConnectionsListFld;

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

  NotifyField* _addToPointCacheFld;
  NotifyField* _addToLineCacheFld;
  NotifyField* _clearPointCacheFld;
  NotifyField* _clearLineCacheFld;

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // Globals
  XMarkerList           _outPositionsList;
  IndexPairList         _outConnectionsList;
  FiberSetContainer     _outFiberSetContainer;
  XMarkerList           _outCachedPointPositionsList;
  XMarkerList           _outCachedLinePositionsList;
  IndexPairList         _outCachedLineConnectionsList;

  StringVector          _inputFileLinesVector;
  int                   _dataStartIndex;


  // Load data
  void _loadData();

  // Unload data
  void _unloadData();

  // Clear cached data
  void _clearPointCache();
  void _clearLineCache();

  // Add loaded data to cache
  void _addToPointCache();
  void _addToLineCache();

  // Update output data
  void _updateOutputData();

  // Crop the delimiter to the first character
  void _cropNumberDelimiter();

  // Crop the decimal separator to the first character
  void _cropDecimalSeparator();

  // Create fiberset container
  void _createFibers();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDF3DFigurePage_LoadPointLineGeometry)
};


ML_END_NAMESPACE

#endif // __LoadPointLineGeometry_H