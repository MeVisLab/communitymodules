//----------------------------------------------------------------------------------
//! The Inventor module class SoStraightLine
/*!
// \file    SoStraightLine.cpp
// \author  kmuehler
// \date    2008-08-06
//
// A single line with a start and an endpoint
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoStraightLine.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoCube.h>
// TODO: include more inventor things here
#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoStraightLine)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoStraightLine::initClass()
{
	SO_NODE_INIT_CLASS(SoStraightLine, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoStraightLine::SoStraightLine()
{
	SO_TRACE_IN("SoStraightLine::SoStraightLine");

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoStraightLine);


	SO_NODE_ADD_FIELD(startPoint, (0,0,0));
	SO_NODE_ADD_FIELD(endPoint, (1,1,1));
	SO_NODE_ADD_FIELD(color, (SbColor(1,1,1)));
	SO_NODE_ADD_FIELD(thickness, (1));
	SO_NODE_ADD_FIELD(transparency, (0));

	_startPointSensor = new SoFieldSensor(SoStraightLine::startPointChangedCB, this);
	_startPointSensor->attach(&startPoint);
	_endPointSensor = new SoFieldSensor(SoStraightLine::endPointChangedCB, this);
	_endPointSensor->attach(&endPoint);
	_colorSensor = new SoFieldSensor(SoStraightLine::colorChangedCB, this);
	_colorSensor->attach(&color);
	_thicknessSensor = new SoFieldSensor(SoStraightLine::thicknessChangedCB, this);
	_thicknessSensor->attach(&thickness);
	_transparencySensor = new SoFieldSensor(SoStraightLine::transparencyChangedCB, this);
	_transparencySensor->attach(&transparency);

	_translation2 = new SoTranslation();
	addChild(_translation2);
	_rotation1 = new SoRotation();
	addChild(_rotation1);
	_translation1 = new SoTranslation();
	addChild(_translation1);
	_material = new SoMaterial();
	addChild(_material);
	_cylinder = new SoCylinder();
	addChild(_cylinder);
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoStraightLine::~SoStraightLine()
{
	SO_TRACE_IN("SoStraightLine::~SoStraightLine");

	delete _startPointSensor;
	delete _endPointSensor;
	delete _colorSensor;
	delete _thicknessSensor;
	delete _transparencySensor;
}


void SoStraightLine::startPointChangedCB(void *data, SoSensor* /*sens*/)
{
	SO_TRACE_IN("SoStraightLine::startPointChangedCB");

	//((SoStraightLine*) data)->startPointChanged((SoFieldSensor*)sens);
	((SoStraightLine*) data)->calculate();
}


void SoStraightLine::endPointChangedCB(void *data, SoSensor* /*sens*/)
{
	SO_TRACE_IN("SoStraightLine::endPointChangedCB");

	((SoStraightLine*) data)->calculate();
}

void SoStraightLine::colorChangedCB(void *data, SoSensor* /*sens*/)
{
	SO_TRACE_IN("SoStraightLine::colorChangedCB");
	((SoStraightLine*) data)->_material->diffuseColor.setValue(((SoStraightLine*) data)->color.getValue());
}

void SoStraightLine::thicknessChangedCB(void *data, SoSensor* /*sens*/)
{
	SO_TRACE_IN("SoStraightLine::thicknessChangedCB");

	((SoStraightLine*) data)->_cylinder->radius.setValue(((SoStraightLine*) data)->thickness.getValue());
}

void SoStraightLine::transparencyChangedCB(void *data, SoSensor* /*sens*/)
{
	SO_TRACE_IN("SoStraightLine::transparencyChangedCB");

	((SoStraightLine*) data)->_material->transparency.setValue(((SoStraightLine*) data)->transparency.getValue());
}

void SoStraightLine::calculate()
{
	SbVec3f rotAxis;
	_dirVec = endPoint.getValue()-startPoint.getValue();
	_cylinder->height.setValue(_dirVec.length());

	if (_dirVec.length()!=0)
	{		
		_translation1->translation.setValue(0,_dirVec.length()/2,0);
		rotAxis.setValue(0,1,0); //yAxis
		rotAxis = rotAxis.cross(_dirVec);

		//alpha=arccos(a*b/|a|*|b|)
		float alpha;
		alpha = acos(SbVec3f(0,1,0).dot(_dirVec)/_dirVec.length());
		_rotation1->rotation.setValue(rotAxis,alpha);
		
		//rotAxis = 0,0,0 in kleinsten y-Wert verschieben; sonst ist es egal
		if (rotAxis[0]==0 && rotAxis[1]==0 && rotAxis[2]==0 && endPoint.getValue()[1]<startPoint.getValue()[1])
		{			
			_translation2->translation.setValue(endPoint.getValue());
		}
		else
			_translation2->translation.setValue(startPoint.getValue());
	}
}