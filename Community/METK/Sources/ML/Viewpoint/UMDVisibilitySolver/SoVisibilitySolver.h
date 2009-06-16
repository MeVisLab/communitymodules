//----------------------------------------------------------------------------------
//! The Inventor module class SoVisibilitySolver derived from SoSeparator
/*!
// \file    SoVisibilitySolver.h
// \author  
// \date    2007-04-16
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __SOVISIBILITYSOLVER_H__
#define __SOVISIBILITYSOLVER_H__


// Local includes
#include "UMDVisibilitySolverSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFString.h>
#include "XVLeaveScope.h"

#include "kBasics.h"

class SoOrthographicCamera;
class SomnOffscreenRenderer;
class SoSeparator;
class SoDirectionalLight;
class SoSwitch;
class SoGetBoundingBoxAction;
class CvpDivSphere;

struct vpListType1;

using namespace std;


//! 
class UMDVISIBILITYSOLVER_EXPORT SoVisibilitySolver : public SoSeparator
{
	//! Implements the runtime type system interface of this new node.
	SO_NODE_HEADER(SoVisibilitySolver);

public:
	//! Constructor
	SoVisibilitySolver();

	//! Initializes this class (called on dll initialization).
	static void  initClass();

	SoSFNode    m_fldScene;
	SoSFTrigger m_fldCompute;
	SoSFShort   m_fldBufferSizeX, m_fldBufferSizeY, m_fldDivision, m_fldSphereMode, m_fldCurrentObj;
	SoSFString  m_fldDataPath;
	SoSFFloat   m_fldProgress;
	SoSFString  m_fldEstTime;

	void _insideGlFunc (void);
	void GLRenderBelowPath(SoGLRenderAction *action);

protected:
	//! Protected destructor
	virtual            ~SoVisibilitySolver();

	//! Sensor to detect changes in node's fields.
	SoNodeSensor      *_nodeSensor;

	//! Called by \c _nodeSensor when any field in node changes.
	//! Only redirects the call of this static function to the
	//! class method \c nodeChanged.
	static void       nodeChangedCB(void *data, SoSensor* sens);

	//! Called by the static function \c _nodeChangedCB to notify node changes.
	void              nodeChanged(SoNodeSensor* sensor);  

	void computeStart();
	void computeLoop();
	void computeFinalize();


private:
  
	void _createSphere (void);
	void _analyzeCurrentViewpoint (void);
	void _saveResultMatrix (void);
	void _paintAllWhiteAneOneRed (int iObjNr);

	void logResults (void);

	SoOrthographicCamera    *m_soCamera;
	SomnOffscreenRenderer   *m_soOff;
	SoSeparator             *m_soSep;
	SoDirectionalLight      *m_soLight;
	SoSwitch                *m_soSwitch;
	SoGetBoundingBoxAction  *m_soBboxAction;
	CvpDivSphere            *m_vpSphere;

	static const short PARAM_VISIBLEPIXEL;
	static const short PARAM_MINDIST;
	static const short PARAM_MAXDIST;
	static const short PARAM_SILHOUETTE;
	static const short PARAM_CENTERVALUE;
	static const short PARAM_COUNT;


	SbVec3f m_vCenter;
	float   m_fRadius;
	string* m_strPathVis;
	string* m_strPathIds;

	int m_iNrOfObjects;
	int m_iCurrentViewpoint;
	float *m_fAllFrameBuffers;
	float *m_fBufferPosition;
	float ***m_fResultMatrix;
	int m_iSwitchCount;
	bool m_flRenderAll;
	vpListType1 *m_vpList;

	bool m_flHasSphere;

	int iTestCount;

	double startTime;

	SoTimerSensor* timerComputeLoop;									//!< Timer um zwischen den verschiedenen CamPos eine keine Pause einzubauen damit Anzeigen und Fortschrittsbalken geupdatet werden können.
	static void timerEventComputeLoop(void* data, SoDataSensor* a);	//!< Sends the Update-Event by calling sendNotifyUpdateMLOutput	

};

#endif // __SOVISIBILITYSOLVER_H