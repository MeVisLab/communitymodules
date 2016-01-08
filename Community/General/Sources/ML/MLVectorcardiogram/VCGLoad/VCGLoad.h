//----------------------------------------------------------------------------------
//! The ML module class VCGLoad.
/*!
// \file    VCGLoad.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __VCGLoad_H
#define __VCGLoad_H


// Local includes
#include "MLVectorcardiogramSystem.h"
#include "shared/MLVectorcardiogram_DataTypes.h"


// ML includes
#include <mlModuleIncludes.h>
#include <mlFiberSet.h>
#include <mlXMarkerList.h>

ML_START_NAMESPACE

//! 
class MLVECTORCARDIOGRAM_EXPORT VCGLoad : public Module
{
public:

  //! Constructor.
  VCGLoad();

  //! Destructor.
  ~VCGLoad();

  // Output fields
  BaseField*   _outFiberSetFld;
  BaseField*   _outPointSetFld;

  // UI fields
  StringField* _filenameFld;

  NotifyField* _loadFld;
  NotifyField* _unloadFld;
  BoolField*   _autoLoadFld;

  BoolField*   _outputValidFld;

  IntField*    _numBeatsFld;
  BoolField*   _numBeatsFromFileFld;

  IntField*    _numDataLinesFld;
  IntField*    _numDataLinesPerBeatFld;
  BoolField*   _numDataLinesMatchesNumBeatsFld;


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


  // Globals
  FiberSetContainer::FiberSet   _outFiberSet;
  XMarkerList                   _outPointSet;
  StringVector                  _inputFileLinesVector;
  int                           _dataStartIndex;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(VCGLoad)
};


ML_END_NAMESPACE

#endif // __VCGLoad_H