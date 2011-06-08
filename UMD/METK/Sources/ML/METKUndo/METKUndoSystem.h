//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METKUndoSystem.h
// \author  kmuehler
// \date    2008-06-19
*/
//----------------------------------------------------------------------------------


#ifndef __METKUndoSystem_H
#define __METKUndoSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef METKUNDO_EXPORTS
    // Export library symbols.
    #define METKUNDO_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define METKUNDO_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef METKUNDO_EXPORTS
  // Use the METKUndo_EXPORT macro to export classes and functions
  #define METKUNDO_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define METKUNDO_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define METKUNDO_EXPORT
  #endif
#endif

#endif // __METKUndoSystem_H


