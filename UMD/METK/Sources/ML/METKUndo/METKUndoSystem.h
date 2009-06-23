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


#endif // __METKUndoSystem_H


