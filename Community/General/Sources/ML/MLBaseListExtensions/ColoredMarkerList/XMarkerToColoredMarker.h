//----------------------------------------------------------------------------------
//! The ML module class XMarkerToColoredMarker.
/*!
// \file    XMarkerToColoredMarker.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-02
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __XMarkerToColoredMarker_H
#define __XMarkerToColoredMarker_H

#include <mlXMarkerList.h>
#include "mlLUTFunction.h"
#include "mlStylePalette.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLBASELISTEXTENSIONS_EXPORT XMarkerToColoredMarker : public Module
{
public:

  //! Constructor.
  XMarkerToColoredMarker();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! Input LUT field
  BaseField *_inLUTFld;

  //! Input style palette field
  BaseField* _inStylePaletteFld;

  // Input XMarkerList field
  BaseField *_inXMarkerListFld;

  // Output ColoredMarkerList field
  BaseField *_outColoredMarkerListFld;

  // Select if default color values shall be written
  BoolField *_writeDefaultColorFld;

  // The field to select a default color
  ColorField *_defaultColorFld;

  // The field to select a default alpha
  FloatField *_defaultAlphaFld;

  //@}

  //! The input style palette
  //StylePalette _inStylePalette;

  //! The XMarkerList input for point positions of point clouds
	ml::XMarkerList _inXMarkerList;  

  //! The XMarkerList input for node positions of line sets
  ml::ColoredMarkerList _outColoredMarkerList;

  //! Stores the minimum and maximum type ID # of the input XMarkerList
  int _inListTypeMinID, _inListTypeMaxID;
  
  //! Convert the input list
  void _convertXMarkersToColoredMarkers();

  //! Scan input XMarkerList and get minimum and maximum type ID #
  void _GetMinMaxTypesIDsFromInXmarkerList();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(XMarkerToColoredMarker)
};


ML_END_NAMESPACE

#endif // __XMarkerToColoredMarker_H