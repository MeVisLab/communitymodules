//----------------------------------------------------------------------------------
//! The ML module class MarkerListImport.
/*!
// \file    MarkerListImport.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-01-15
// 
*/
//----------------------------------------------------------------------------------


#ifndef __MarkerListImport_H
#define __MarkerListImport_H


// Local includes
#include "MLBaseListExtensionsSystem.h"
#include "shared/MLBaseListExtensions_DataTypes.h"


// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>

ML_START_NAMESPACE

//! 
class MLBASELISTEXTENSIONS_EXPORT MarkerListImport : public Module
{
public:

  //! Constructor.
  MarkerListImport();

  //! Destructor.
  ~MarkerListImport();

  // Output fields
  BaseField*   _outMarkerListFld;

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

  // Crop the delimiter to the first character
  void _cropNumberDelimiter();

  // Crop the decimal separator to the first character
  void _cropDecimalSeparator();

  // Globals
  XMarkerList                   _outMarkerList;
  StringVector                  _inputFileLinesVector;
  int                           _dataStartIndex;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(MarkerListImport)
};


ML_END_NAMESPACE

#endif // __MarkerListImport_H