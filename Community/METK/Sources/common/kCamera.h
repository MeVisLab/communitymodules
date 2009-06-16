//----------------------------------------------------------------------------------
/*!
// \file    kCamera.h
// \author  Konrad Mühler
// \date    2005-07-06
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __kCamera2_H
#define __kCamera2_H


#pragma warning(disable:4251)
#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport) 
#endif
#else
#define MLIMAGEUTILITIES_EXPORT
#endif

#include <iostream>
#include <iomanip> 
#include <string>
#include <sstream> 
#pragma warning(disable:4786)
#include <list>
#include <vector>
#include <algorithm> //for string-replace
#include <limits>
#include <math.h>

#include "XVEnterScope.h"
#include <Inventor/SbLinear.h>
#include "XVLeaveScope.h"

#include "kBasics.h"

#pragma warning(disable:4251)


//! Class for simulate a camera with position, orientation etc.
class __COMMON_IMPORTEXPORT kCamera
{
public:

	//--------------------------------------------------------
	//Functions
	//--------------------------------------------------------

	kCamera();

	void setLookAt(SbVec3f value);
	SbVec3f getLookAt();
	void setLookDir(SbVec3f value);
	SbVec3f getLookDir();
	void setUpVec(SbVec3f value);
	SbVec3f getUpVec();
	void setUpVecAngle(double value);
	double getUpVecAngle();
	void setNormPlump(SbVec3f value);
	SbVec3f getNormPlump();
	void setPosition(SbVec3f value);
	SbVec3f getPosition();
	void setOrientation(SbRotation value);
	SbRotation getOrientation();
	void setHeight(double value);
	double getHeight();	
	void setFocalDistance(double value);
	double getFocalDistance();	


	SbVec3f calcUpVector(const SbVec3f lookDir, const SbVec3f normPlump);
	SbVec3f calcPerfectUpVector(const SbVec3f lookDir, const SbVec3f plump);
	float getUpVecAngleDir(const SbVec3f lookDir, const SbVec3f upVec);
	void rotateVector(SbVec3f& vect, const SbVec3f axis, const double angle);

	SbRotation calcOrientation(const SbVec3f upVec, const SbVec3f dir);

	void setCamPosition(const SbVec3f& position, const SbVec3f& lookAt);
	void rotatePosition(SbVec3f rotAxis, double rotAngle, SbVec3f axisPoint);
	void rotateCam(double angle);
	
	//bool setNormPlump(const SbVec3f lookDir);

private:
	
	double epsilon; //!< error size
	
	SbVec3f currentLookAt; //!< point to look at
	SbVec3f currentLookDir; //!< look direction
	SbVec3f currentUpVec; //!< up vector
	SbVec3f currentPosition; //!< position
	SbRotation currentOrientation; //!< orientation
	double currentUpVecAngle; //!< angle between normal up vector (given by direction to NormPlump) an current up vector
	SbVec3f NormPlump; //!< dircetion for the top of scene for up vector (i.e. z-axis)
	double currentHeight; //!< height of view volume for orthogonal camera
	double currentFocalDistance; //!< focal distance

	//--------------------------------------------------------
	//Functions
	//--------------------------------------------------------

	void splitOrientation(const SbRotation orientation, SbVec3f& upVec, SbVec3f& lookAt, double& upVecAngle);
};

#endif // __kCamera_H