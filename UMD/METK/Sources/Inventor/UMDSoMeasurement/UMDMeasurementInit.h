/*
* Copyright 2001, MeVis gGmbH
* ALL RIGHTS RESERVED
*
* THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
* INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
* DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
* PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
*
*/
//======================================================
//! This header file includes all important header files (and is therefor included by all other (header)files in the project)
//! Also here is an export symbol defined to let other applications use the DLL code of the package
/*!
// \file    MeasurementInit.h
// \author  Olaf Konrad, last changed by $Author: wolf $
// \date    20.02.2002
*/
// $Id: MeasurementInit.h,v 1.5 2004/04/28 08:05:22 wolf Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/MeasurementInit.h,v $
//======================================================

#ifndef __UMDSO_MEASUREMENT_INIT__
#define __UMDSO_MEASUREMENT_INIT__

#include <GL/glew.h>
#include <mlOpenGL.h>


#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDSOMEASUREMENT_EXPORTS
    // Export library symbols.
    #define SO_MEASUREMENT_CLASS_SPEC   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define SO_MEASUREMENT_CLASS_SPEC   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  //! Define a class export specifier needed to make the class
  //! exportable in window dll's.
  #ifdef WIN32
    #ifdef UMDSOMEASUREMENT_EXPORTS
    #define SO_MEASUREMENT_CLASS_SPEC __declspec(dllexport)
    #else
    #define SO_MEASUREMENT_CLASS_SPEC __declspec(dllimport)
    #endif
  #else
    #define SO_MEASUREMENT_CLASS_SPEC
  #endif
#endif

#ifdef WIN32

//Unterdr"uckt die Warnung warning C4275: class 'SoCoordinate4' ist keine DLL-Schnittstelle
//und wurde als Basisklasse fuer die DLL-Schnittstelle class '...' verwendet
#pragma warning (disable : 4275)

// Disable "class 'SoType' benoetigt eine DLL-Schnittstelle, die von Clients von class 'SoVesselVis2DArea' verwendet wird"
#pragma warning (disable : 4251)

// Disable warning C4514: '/=' : Nichtreferenzierte Inline-Funktion wurde entfernt
#pragma warning (disable : 4514)

// Disable warning C4244: 'argument' : Konvertierung von 'int' in 'short', moeglicher Datenverlust (at inventor node initialization)
#pragma warning (disable : 4244)


// Disable warning for release mode: Warning C4710: '__thiscall XXX::YYY(ZZZ)' ist keine Inline-Funktion
#pragma warning(disable : 4710 )

// disable warning: Keine Definitionen fuer die Inline-Funktion
#pragma warning (disable : 4506)

// disable warning:std::reverse_iterator<void (__cdecl**)(void *,char const *,ml::ErrorOutputInfos const &),void (__cdecl*)(void *,char const *,ml::ErrorOutputInfos const &),void (__cdecl*&)(void *,char const *,ml::ErrorOutputInfos const &),void (__cdecl**)(void *,char const *,ml::ErrorOutputInfos const &),int>' : Bezeichner wurde auf '255' Zeichen in den Debug-Informationen reduziert
#pragma warning (disable : 4786)

// Include often used libraries automatically and disable its warnings
// to be able to compile the ml in highest warning level.

#pragma warning (disable: 4018)

#pragma warning( push, 3 )

#include <XVEnterScope.h>
#include "XVInventorWrapper.h"
#include "XVBasics.h"
#include <XVLeaveScope.h>

//typedef __int32 int32_t;
typedef __int64 int64_t;

#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
//#include <iostream.h> //störte beim einbinden des headers in andere projekte
#include <iostream>
#include <memory.h>
#include <vector>
#include <math.h>
#include <FLOAT.H>
#include <assert.h>
#pragma warning( pop )

#elif linux
#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <assert.h>

#else

#include <mstring.h>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <memory.h>
#include <vector>
#include <math.h>

#endif

#ifdef WIN32
#pragma warning ( push, 3 )
#endif

#include <XVEnterScope.h>
#include "OIVConfig.h"

//  --- basic stuff --
#include <Inventor/SbLinear.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>

// --- actions ---
#include <Inventor/actions/SoCallbackAction.h>

// --- draggers ---
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>

// --- engines ---
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/engines/SoCompose.h>

// --- events ---
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

// --- fields ---
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFMatrix.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Inventor/fields/SoMFVec3f.h>

// --- nodes ---
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoVertexProperty.h>

// --- sensors ---
#include <Inventor/sensors/SoFieldSensor.h>


#include <XVLeaveScope.h>

#ifdef WIN32
#pragma warning ( pop )
#endif

#endif
