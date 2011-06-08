//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCSOToRTStructureSystem.h
// \author  linal
// \date    2010-01-20
*/
//----------------------------------------------------------------------------------


#ifndef __MLCSOToRTStructureSystem_H
#define __MLCSOToRTStructureSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLCSOTORTSTRUCTURE_EXPORTS
    // Export library symbols.
    #define MLCSOTORTSTRUCTURE_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLCSOTORTSTRUCTURE_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLCSOTORTSTRUCTURE_EXPORTS
  // Use the MLCSOTORTSTRUCTURE_EXPORT macro to export classes and functions
  #define MLCSOTORTSTRUCTURE_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLCSOTORTSTRUCTURE_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLCSOTORTSTRUCTURE_EXPORT
  #endif
#endif

#endif // __MLCSOToRTStructureSystem_H


