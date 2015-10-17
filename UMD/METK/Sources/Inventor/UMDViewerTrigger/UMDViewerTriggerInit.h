//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDViewerTriggerInit.h
// \author  Konrad Mühler
// \date    2005-04-23
//
*/
//----------------------------------------------------------------------------------

#ifndef __UMDVIEWERTRIGGERINIT_H__
#define __UMDVIEWERTRIGGERINIT_H__

void UMDViewerTriggerInit();






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


#pragma warning( push, 3 )

#include <XVEnterScope.h>
#include "XVInventorWrapper.h"
#include "XVBasics.h"
#include <XVLeaveScope.h>

#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <FLOAT.H>
#pragma warning( pop )


//! Define a class export specifier needed to make the class
//! exportable in window dll's.
#ifdef SO_MOVIESCRIPTER_EXPORTS
#define SO_MOVIESCRIPTER_CLASS_SPEC __declspec(dllexport)
#else
#define SO_MOVIESCRIPTER_CLASS_SPEC __declspec(dllimport)
#endif

#elif linux

#include <string>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>
#include <vector>
#include <time.h>
#include <math.h>

#define SO_MOVIESCRIPTER_CLASS_SPEC 
#define SO_MOVIESCRIPTER_CLASS_SPEC 

#else

// For example MacOS:
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>
#include <vector> 
#include <math.h>

#define SO_MOVIESCRIPTER_CLASS_SPEC 
#define SO_MOVIESCRIPTER_CLASS_SPEC 

#endif

#ifdef WIN32
#pragma warning ( push, 3 )
#endif

#include <XVEnterScope.h>

#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoFocalDistanceElement.h>

#include <Inventor/engines/SoTimeCounter.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFPlane.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoMFFloat.h>

#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include <XVLeaveScope.h>


#ifdef WIN32
#pragma warning ( pop )
#endif 




#endif
// __UMDVIEWERTRIGGERINIT_H__

