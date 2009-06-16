//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METK2DLabelingSystem.h
// \author  Konrad Mühler
// \date    2008-04-11
*/
//----------------------------------------------------------------------------------


#ifndef __METK2DLabelingSystem_H
#define __METK2DLabelingSystem_H

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef METK2DLABELING_EXPORTS
// Use the METK2DLABELING_EXPORT macro to export classes and functions
#define METK2DLABELING_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define METK2DLABELING_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define METK2DLABELING_EXPORT
#endif


#endif // __METK2DLabelingSystem_H


