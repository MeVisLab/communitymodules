//----------------------------------------------------------------------------------
//! Functions to distribute points on a sphere surface equally
/*!
// \file    kSphere.h
// \author  Konrad Mühler
// \date    2006-11-16
//
*/
//----------------------------------------------------------------------------------


# ifndef __KSPHERE_HEADER__
# define __KSPHERE_HEADER__


#pragma warning(disable:4251)
#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport)
#endif
#else
#define __COMMON_IMPORTEXPORT
#endif



#include <string>
#include <list>
#include "kBasics.h"
#include <Inventor/SbLinear.h>


namespace kSphere
{
  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------
  __COMMON_IMPORTEXPORT void subdivide(int rek, SbVec3f* pa, SbVec3f* pb, SbVec3f* pc, SbVec3f* center, float radius, list<SbVec3f>* points);
  __COMMON_IMPORTEXPORT void getPoints(SbVec3f center, const double radius, list<SbVec3f>* points, int recursiondeep);
  __COMMON_IMPORTEXPORT list<SbVec3f>* clearDoubles(list<SbVec3f>* points);

}

#endif