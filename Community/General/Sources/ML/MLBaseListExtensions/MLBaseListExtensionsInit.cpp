//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLBaseListExtensionsInit.cpp 
// \author  Axel Newe
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLBaseListExtensionsSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "IndexPairList/IndexPairList.h"
#include "ColoredMarkerList/ColoredMarkerList.h"
#include "ColoredMarkerList/XMarkerToColoredMarker.h"
#include "ColoredMarkerList/ColoredMarkerToXMarker.h"
#include "ListTools/ListInfo.h"
#include "ListTools/ListFilter.h"
#include "ListTools/ListModifyProperties.h"
#include "MarkerListImport/MarkerListImport.h"

ML_START_NAMESPACE

  // Initialization of the run type system

  int MLBaseListExtensionsInit()
  {
    ML_TRACE_IN("MLBaseListExtensionsInit()")

    IndexPair                  ::initClass();
    IndexPairList              ::initClass();
    IndexPairListContainer     ::initClass();

    ColoredMarker              ::initClass();
    ColoredMarkerList          ::initClass();
    ColoredMarkerListContainer ::initClass();

    XMarkerToColoredMarker     ::initClass();
    ColoredMarkerToXMarker     ::initClass();

    ListInfo                   ::initClass();
    ListFilter                 ::initClass();
    ListModifyProperties       ::initClass();

    MarkerListImport           ::initClass();

    return 1;
  }

ML_END_NAMESPACE


// Call init method on load of shared library
ML_INIT_LIBRARY(MLBaseListExtensionsInit)








