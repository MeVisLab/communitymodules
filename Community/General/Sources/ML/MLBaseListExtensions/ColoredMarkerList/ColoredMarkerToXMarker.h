//----------------------------------------------------------------------------------
//! The ML module class ColoredMarkerToXMarker.
/*!
// \file    ColoredMarkerToXMarker.h
// \author  Axel Newe
// \date    2014-09-02
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __ColoredMarkerToXMarker_H
#define __ColoredMarkerToXMarker_H

#include <mlXMarkerList.h>
#include "mlLUTFunction.h"
#include "mlStylePalette.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLBASELISTEXTENSIONS_EXPORT ColoredMarkerToXMarker : public Module
{
public:

  //! Constructor.
  ColoredMarkerToXMarker();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  // Output ColoredMarkerList field
  BaseField *_inColoredMarkerListFld;

  // Input XMarkerList field
  BaseField *_outXMarkerListFld;


  //@}

  //! The XMarkerList input for node positions of line sets
  ml::ColoredMarkerList _inColoredMarkerList;
  
  //! The XMarkerList input for point positions of point clouds
	ml::XMarkerList _outXMarkerList;  

  //! Convert the input list
  void _convertColoredMarkersToXMarkers();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(ColoredMarkerToXMarker)
};


ML_END_NAMESPACE

#endif // __ColoredMarkerToXMarker_H