//----------------------------------------------------------------------------------
/*!
// \file    kCamera.h
// \author  Konrad Mühler
// \date    2005-07-06
//
// 
*/
//----------------------------------------------------------------------------------


#include "kCamera.h"


//! Constructor
kCamera::kCamera()
{
	epsilon = 0.0005; //eigentl mind. 0.001 ... aber bei 0.0005 fällt das nachruckeln/überspringen nach Notiz Maja20050621 weg
}



//! Sets lookAt vector
/*! \param value vector */
void kCamera::setLookAt(SbVec3f value)
{
	currentLookAt = value;
}



//! Return lookAt vector
/*! \return lookAt vector */
SbVec3f kCamera::getLookAt()
{
	return currentLookAt;
}



//! Sets look direction
/*! \param value vector */
void kCamera::setLookDir(SbVec3f value)
{
	currentLookDir = value;
}



//! Returns look direction
/*! \return look direction */
SbVec3f kCamera::getLookDir()
{
	return currentLookDir;
}



//! Sets upVector
/*! \param value upVector */
void kCamera::setUpVec(SbVec3f value)
{
	currentUpVec = value;
}



//! Return upVector
/*! \return upVector */
SbVec3f kCamera::getUpVec()
{
	return currentUpVec;
}



//! Set upVecAngle
/*! \param value angle (bogenmaß) */
void kCamera::setUpVecAngle(double value)
{
	currentUpVecAngle = value;
}



//! Returns angle between normal upVector and current upVector
/*! \return angle between normal upVector and current upVector */
double kCamera::getUpVecAngle()
{
	return currentUpVecAngle;
}



//! Set NormPlump (i.e. z-axis)
/*! \param value vector */
void kCamera::setNormPlump(SbVec3f value)
{
	NormPlump = value;
}



//! Returns NormPlump
/*! \return vector */
SbVec3f kCamera::getNormPlump()
{
	return NormPlump;
}



//! Sets Position
/*! \param value position */
void kCamera::setPosition(SbVec3f value)
{
	currentPosition = value;
}



//! Return Position
/*! \return position */
SbVec3f kCamera::getPosition()
{
	return currentPosition;
}



//! Sets orientation
/*! \param value orientation */
void kCamera::setOrientation(SbRotation value)
{
	currentOrientation = value;
	splitOrientation(currentOrientation,currentUpVec,currentLookDir,currentUpVecAngle);
}



//! Returns orientation
/*! \return orientation
 */
SbRotation kCamera::getOrientation()
{
	return currentOrientation;
}



//! Sets height
/*! \param value height */
void kCamera::setHeight(double value)
{
	currentHeight = value;
}



//! Returns height
/*! \return height */
double kCamera::getHeight()
{
	return currentHeight;
}




void kCamera::setFocalDistance(double value)
{
	currentFocalDistance = value;
	currentLookAt = currentPosition + currentLookDir*currentFocalDistance;
}



double kCamera::getFocalDistance()
{
	return currentFocalDistance;
}


//! Erxtract upVector, lookDirection and angle between upVector and perfectUpVector from orientation
/*! \param orientation
 *  \param[out] upVec
 *  \param[out] lookDir
 *  \param[out] upVecAngle
 */
void kCamera::splitOrientation(const SbRotation orientation, SbVec3f& upVec, SbVec3f& lookDir, double& upVecAngle)
{
	// Extrahieren der lookDir aus der aktuellen orientation
	lookDir.setValue(0.0, 0.0, -1.0); //! init to default lookat direction (DIRECTION!)
    orientation.multVec(lookDir, lookDir); 
	lookDir.normalize();	

	// Extrahieren des upVectors aus der aktuellen orientation
	upVec.setValue(0.0, 1.0, 0.0); // init to default up vector direction 
	orientation.multVec(upVec, upVec);
	upVec.normalize();

	// Ermitteln des perfekten upVectors (upVector ohne zusätzliche Drehung um die Sichtachse)
	SbVec3f perfectUpVec;		
	if (fabs(lookDir.dot(NormPlump))>(1.0-epsilon)) //! wenn lookDir und perfectUpVec parallel, dann gleich setzen (Vermeidung von Berechnungsfehlern
		perfectUpVec = upVec;
	else
		perfectUpVec = calcPerfectUpVector(lookDir,NormPlump);

	perfectUpVec.normalize();

	// a dot b = |a|*|b|+cos(a,b)	
	double tempDot = upVec.dot(perfectUpVec); //! Es gab Fälle, in denen war .dot minimal größer als 1.0 -> acos = 1.#IND
	if (tempDot>1.0) tempDot = 1.0;	if (tempDot<-1.0) tempDot = -1.0;
	upVecAngle = acos(tempDot); //! 1.0 = Produkt der beiden Längen ... sind aber normalisiert


	// Ermittlung der Drehrichtung des Winkels und ggf. Erhöhung um 180Grad
	// Im R3 gibt es zuerst einmal keine positiven und negativen drehwinkel.
	// Erst wenn man die Ebene, die die beiden Vektoren definieren, orientiert, geht das.
	// Man kann also festlegen, dass der Winkel positiv ist, wenn Hesse-Normalenvektor der Ebene und Kreuzprodukt in dieselbe Richtung zeigen. 
	if (getUpVecAngleDir(lookDir,upVec)>epsilon && (upVecAngle<(kBasics::PI-epsilon)))
		upVecAngle = kBasics::PI + (kBasics::PI - upVecAngle);

	// Vermeidung von Winkeln > 2*PI
	if (upVecAngle>(2*(kBasics::PI-epsilon)))
		upVecAngle = upVecAngle - 2*kBasics::PI;

	// Sehr kleine Winkel werden auf 0.0 gesetzt
	if (fabs(upVecAngle)<epsilon)
		upVecAngle = 0.0; //! sonst kommt es zu Dingen wie 1.#IND
}



//! Calc the upVector from lookDirection and a given plump (standard 0,0,1).
/*! Rotates the upVector by the current upVector angle
 * \param lookDir
 * \param plump
 */
SbVec3f kCamera::calcUpVector(const SbVec3f lookDir, const SbVec3f plump)
{
	SbVec3f upVec;	
	upVec = calcPerfectUpVector(lookDir, plump);	
	upVec.normalize();
	rotateVector(upVec,lookDir,currentUpVecAngle);
	upVec.normalize();
	return upVec;
}



//! Calc the perfect upVector using lookDirection and a plump
/*! \param lookDir
 *  \param plump
 *  \return an upVector
 */
SbVec3f kCamera::calcPerfectUpVector(const SbVec3f lookDir, const SbVec3f plump)
{
	return lookDir.cross(plump.cross(lookDir));	
}



//! Gets the orientation (right, left) of angle between upVector and perfectUpVector.
/*! Returns the dot-product (1 = parallel, 0 = right angle, -1 = opposite)
 *  \param lookDir
 *  \param upVec
 *  \return dotproduct ( +1 = parallel, 0 = orthogonal, -1 = 180 deg )
 */
float kCamera::getUpVecAngleDir(const SbVec3f lookDir, const SbVec3f upVec)
{
	SbVec3f perfectUpVec = calcPerfectUpVector(lookDir,NormPlump);
	perfectUpVec.normalize();

	SbVec3f crossproduct;
	crossproduct = upVec.cross(perfectUpVec);
	crossproduct.normalize();

	float dotproduct = crossproduct.dot(lookDir);
	
	//SoDebugError::postInfo("_____________________     MASTER-WINKEL: ",kBasics::FloatToString(angleDiff).c_str());

	return dotproduct;
}



//! Rotates a vector
/*! \param[in,out] vect vector to rotate
 *  \param axis
 *  \param angle
 */
void kCamera::rotateVector(SbVec3f& vect, const SbVec3f axis, const double angle)
{
	SbRotation vecRotation;
	vecRotation.setValue(axis,angle);
	vecRotation.multVec(vect,vect);
	vect.normalize();
}



//! Calc the orientation using upVector and lookDirection
/*! \param upVec
 *  \param dir
 *  \return orientation
 */
SbRotation kCamera::calcOrientation(const SbVec3f upVec, const SbVec3f dir)
{
	// from comp.graphics.api.inventor ... "Setting SoCamera orientation"
	SbVec3f z = -dir; 
	SbVec3f y = upVec; 
	z.normalize(); 
	y.normalize(); 
	SbVec3f x = y.cross(z); 

	// recompute y to create a valid coordinate system 
	y	= z.cross(x); 

	// create a rotation matrix 
	SbMatrix rot = SbMatrix::identity(); 
	rot[0][0] = x[0]; 
	rot[0][1] = x[1]; 
	rot[0][2] = x[2]; 

	rot[1][0] = y[0]; 
	rot[1][1] = y[1]; 
	rot[1][2] = y[2]; 

	rot[2][0] = z[0]; 
	rot[2][1] = z[1]; 
	rot[2][2] = z[2]; 

	// convert matrix into rotation 
	return SbRotation(rot); 
}



//! Sets new position and calcs the upVector and orientation
/*! \param position
 *  \param lookAt 
 */
void kCamera::setCamPosition(const SbVec3f& position, const SbVec3f& lookAt)
{
	currentLookAt = lookAt;
	currentPosition = position;
	
	currentLookDir = currentLookAt-currentPosition;
	currentLookDir.normalize();
		
	currentUpVec = calcUpVector(currentLookDir,NormPlump); //! Neuen UpVec ausrechnen - Rotation wird schon in calcUpVector vollzogen
	currentUpVec.normalize();

	currentOrientation = calcOrientation(currentUpVec,currentLookDir); //! Berechnet neue orientation

//	writeOrientation(currentOrientation); //! Schreibt orientation in ObjMgr
//	writePosition(currentPosition); //! Schreibt position in ObjMgr	
}



//! Rotate the current position around an axis with an angle
/*! \param rotAxis
 *  \param rotAngle 
 */
void kCamera::rotatePosition(SbVec3f rotAxis, double rotAngle, SbVec3f axisPoint)
{	
	//Error20051017: Er rotierte immer nur um eine Achse, ohne diese zu verschieben (in den LookAt-Punkt).	
	//Daher zunächst eine Translation der aktuellen Position um den Rotationspunkt und anschließend wieder zurück

	SbVec3f tempPos = currentPosition - axisPoint; //Error20051017

	// Position rotieren
	SbRotation pointRotation;
	pointRotation.setValue(rotAxis,rotAngle);
	//pointRotation.multVec(currentPosition, currentPosition);
	pointRotation.multVec(tempPos, tempPos); //Error20051017
	currentPosition = tempPos + axisPoint; //Error20051017
	
	currentLookDir = currentLookAt-currentPosition;
	currentLookDir.normalize();

	currentUpVec = calcUpVector(currentLookDir,NormPlump); //! Neuen UpVec ausrechnen - Rotation wird schon in calcUpVector vollzogen
	currentUpVec.normalize();

	currentOrientation = calcOrientation(currentUpVec,currentLookDir); //! Berechnet neue orientation

//	writeOrientation(currentOrientation); //! Schreibt orientation in ObjMgr
//	writePosition(currentPosition); //! Schreibt position in ObjMgr	
}



//! Rotate the cam around the lookDirection
/*! \param angle */
void kCamera::rotateCam(double angle)
{		
	// UpVec rotieren - bisher nur um die Sichtachse, also kein Kippen gegenüber der Sichtrichtung
	// Bei anderen Rotationen müssten dann sowohl UpVec wie auch lookDir gedreht werden
	
	currentUpVecAngle = currentUpVecAngle + angle;

	SbVec3f perfectUpVec = calcPerfectUpVector(currentLookDir,NormPlump);
	perfectUpVec.normalize();
	rotateVector(perfectUpVec,currentLookDir, currentUpVecAngle);	
	currentUpVec = perfectUpVec;

	currentOrientation = calcOrientation(currentUpVec,currentLookDir); //! Berechnet neue orientation
	//writeOrientation(currentOrientation); //! Schreibt orientation in ObjMgr		
}



/*bool kCamera::setNormPlump(const SbVec3f lookDir)
{
	//positiver z-Halbraum --> look-z = neg --> NormPlump-y = pos
	//negativer z-Halbraum --> look-z = pos --> NormPlump-y = neg
	/*SoDebugError::postInfo("NNNNNNNNNNNNNNNNormPlump: ",Vec3fToString(NormPlump).c_str());
	SoDebugError::postInfo("lookDir[2]: ",kBasics::FloatToString(lookDir[2]).c_str());
	SbVec3f oldNormPump = NormPlump;
	if (lookDir[2]<0.0)
		NormPlump[1] = fabs(NormPlump[1]);
	else
		NormPlump[1] = -fabs(NormPlump[1]);
	SoDebugError::postInfo("NNNNNNNNNNNNNNNNormPlump: ",Vec3fToString(NormPlump).c_str());

	
	//if (oldNormPump[1]!=NormPlump[1]) return true;
	//SoDebugError::postInfo("return false","");
	return false;
}*/