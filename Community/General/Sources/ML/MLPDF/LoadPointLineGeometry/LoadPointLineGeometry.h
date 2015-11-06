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
#include "shared/MLBaseListExtensions_DataTypes.h"


// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <IndexPairList.h>
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


  // UI fields
  StringField* _filenameFld;
  StringField* _filterFld;

  StringField* _numberDelimiterFld;
  BoolField*   _numberDelimiterSpaceFld;
  StringField* _decimalSeparatorFld;

  NotifyField* _loadFld;
  NotifyField* _unloadFld;
  BoolField*   _autoLoadFld;

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // Load data
  void _loadData();

  // Unload data
  void _unloadData();

  // Analyze input data
  void _analyzeInputData();

  // Update output data
  void _updateOutputData();

  // Create fibers
  void _createFibers();

  // Crop the delimiter to the first character
  void _cropNumberDelimiter();

  // Crop the decimal separator to the first character
  void _cropDecimalSeparator();

  // Globals
  XMarkerList                   _outPositionsList;
  IndexPairList                 _outConnectionsList;
  FiberSetContainer             _outFiberSetContainer;

  StringVector                  _inputFileLinesVector;
  int                           _dataStartIndex;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(LoadPointLineGeometry)
};


ML_END_NAMESPACE

#endif // __LoadPointLineGeometry_H