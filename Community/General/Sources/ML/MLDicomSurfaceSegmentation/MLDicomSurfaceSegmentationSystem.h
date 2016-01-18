//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLDicomSurfaceSegmentationSystem.h
// \author  Axel Newe
// \date    2015-09-25
*/
//----------------------------------------------------------------------------------

#ifndef __MLDicomSurfaceSegmentationSystem_H
#define __MLDicomSurfaceSegmentationSystem_H

//DLL export macro definition.
#ifdef MLDICOMSURFACESEGMENTATION_EXPORTS
  // Use the MLLOADDICOMSEGMENTATION_EXPORT macro to export classes and functions.
  #define MLDICOMSURFACESEGMENTATION_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define MLDICOMSURFACESEGMENTATION_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif

#endif // __MLDicomSurfaceSegmentationSystem_H
