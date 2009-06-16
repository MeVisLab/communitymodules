//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METKLabelsSystem.h
// \author  Konrad Mühler
// \date    2007-07-13
*/
//----------------------------------------------------------------------------------


#ifndef __LabelingSystem_H
#define __LabelingSystem_H

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef LABELING_EXPORTS
// Use the METKLABELS_EXPORT macro to export classes and functions
#define LABELING_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define LABELING_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define LABELING_EXPORT
#endif


#endif // __LabelingSystem_H


