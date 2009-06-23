//----------------------------------------------------------------------------------
//! The Inventor module class SoPickInfo
/*!
// \file    SoPickInfo.cpp
// \author  Stefan Hiller
// \date    2007-09-11
//
// Get picking information.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoPickInfo.h"

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
#include "XVLeaveScope.h"

// Resolve platform dependent itoa function.
#ifdef WIN32
  #define myIToA(A, B, C) _itoa(A, B, C)
#else
  #define myIToA(A, B, C)  itoa(A, B, C)
#endif

#include <string>		// for concatenation, also uses non-standard function itoa

SO_NODE_SOURCE(SoPickInfo)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoPickInfo::initClass()
{
	SO_NODE_INIT_CLASS(SoPickInfo, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoPickInfo::SoPickInfo()
{
	SO_TRACE_IN("SoPickInfo::SoPickInfo")

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoPickInfo);

    SO_NODE_ADD_FIELD(infoHitCoordinate,		(0.0f, 0.0f, 0.0f));
	SO_NODE_ADD_FIELD(infoObjHitCoordinate,		(0.0f, 0.0f, 0.0f));
	SO_NODE_ADD_FIELD(infoHitNormal,			(0.0f, 0.0f, 0.0f));
	SO_NODE_ADD_FIELD(infoObjHitNormal,			(0.0f, 0.0f, 0.0f));
	SO_NODE_ADD_FIELD(infoStatus,				(""));
	SO_NODE_ADD_FIELD(infoTypeName,				(""));
	SO_NODE_ADD_FIELD(infoPickPath,				(""));
	SO_NODE_ADD_FIELD(infoDetail,				(""));

	_leftMouseButtonDown = false;

}

void SoPickInfo::handleEvent(SoHandleEventAction *action)
{
	SO_TRACE_IN("SoWEMFaceInspector::handleEvent")

	const SoMouseButtonEvent* event = (SoMouseButtonEvent*)action->getEvent();

	/////////////////////////////////////////////
	//////// LEFT MOUSE BUTTON PRESSED //////////
	/////////////////////////////////////////////
	if (event->isButtonPressEvent(event, SoMouseButtonEvent::BUTTON1))
	{
		_leftMouseButtonDown = true;
	}
	//////////////////////////////////////////////
	//////// LEFT MOUSE BUTTON RELEASED //////////
	//////////////////////////////////////////////
	else if (event->isButtonReleaseEvent(event, SoMouseButtonEvent::BUTTON1))
	{
		_leftMouseButtonDown = false;
	}
	/////////////////////////////////////////////
	//////// LEFT MOUSE BUTTON HELD /////////////
	/////////////////////////////////////////////
	if (_leftMouseButtonDown)
	{
		const SoPickedPoint* pPoint = action->getPickedPoint();
		if (!pPoint)
		{
			_resetFields();
			return; 
		}
		_tempPickPoint = pPoint->getPoint();
		infoHitCoordinate	.setValue(_tempPickPoint);
		infoObjHitCoordinate.setValue(pPoint->getObjectPoint());
		infoHitNormal		.setValue(pPoint->getNormal());
		infoObjHitNormal	.setValue(pPoint->getObjectNormal());
		infoStatus			.setValue("Picked something.");

		const SoDetail *pickDetail = pPoint->getDetail();

		if (pickDetail)
		{
			SoType detailTypeId = pickDetail->getTypeId();
			infoTypeName.setValue(detailTypeId.getName().getString());

			////////////////////////////////////
			//////// SoPointDetail /////////////
			////////////////////////////////////
			if (detailTypeId == SoPointDetail::getClassTypeId())
			{
				SoPointDetail *pointDetail = (SoPointDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoPointDetail</td> </tr> ");
				char iA[33];	// buffer for 32-bit int + 0

				myIToA(pointDetail->getCoordinateIndex(), iA, 10);
				details += "<tr> <td>Coordinate Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(pointDetail->getMaterialIndex(), iA, 10);
				details += "<tr> <td>Material Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(pointDetail->getNormalIndex(), iA, 10);
				details += "<tr> <td>Normal Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(pointDetail->getTextureCoordIndex(), iA, 10);
				details += "<tr> <td>Texture Coordinate Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				details += "</table>";

				infoDetail.setValue(details.c_str());
			}
			////////////////////////////////////
			//////// SoLineDetail //////////////
			////////////////////////////////////
			else if (detailTypeId == SoLineDetail::getClassTypeId())
			{
				SoLineDetail *lineDetail = (SoLineDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoLineDetail</td> </tr> ");
				char iA[33];	// buffer for 32-bit int + 0

				myIToA(lineDetail->getPoint0()->getCoordinateIndex(), iA, 10);
				details += "<tr> <td>Point0 Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(lineDetail->getPoint1()->getCoordinateIndex(), iA, 10);
				details += "<tr> <td>Point1 Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(lineDetail->getLineIndex(), iA, 10);
				details += "<tr> <td>Line Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(lineDetail->getPartIndex(), iA, 10);
				details += "<tr> <td>Part Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				details += "</table>";

				infoDetail.setValue(details.c_str());
			}
			////////////////////////////////////
			//////// SoFaceDetail //////////////
			////////////////////////////////////
			else if (detailTypeId == SoFaceDetail::getClassTypeId())
			{
				SoFaceDetail *faceDetail = (SoFaceDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoFaceDetail</td> </tr> ");
				char iA[33];	// buffer for 32-bit int + 0

				for (int i = 0; i < faceDetail->getNumPoints(); i++)
				{
					myIToA(i, iA, 10);
					details += "<tr> <td>Point " + std::string(iA);
					myIToA(faceDetail->getPoint(i)->getCoordinateIndex(), iA, 10);
					details += " Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				}
				myIToA(faceDetail->getFaceIndex(), iA, 10);
				details += "<tr> <td>Face Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(faceDetail->getPartIndex(), iA, 10);
				details += "<tr> <td>Part Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				details += "</table>";

				infoDetail.setValue(details.c_str());
			}
			////////////////////////////////////
			//////// SoTextDetail //////////////
			////////////////////////////////////
			else if (detailTypeId == SoTextDetail::getClassTypeId())
			{
				SoTextDetail *textDetail = (SoTextDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoTextDetail</td> </tr> ");
				char iA[33];	// buffer for 32-bit int + 0

				myIToA(textDetail->getStringIndex(), iA, 10);
				details += "<tr> <td>String Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				myIToA(textDetail->getCharacterIndex(), iA, 10);
				details += "<tr> <td>Character Index</td> <td>" + std::string(iA) + "</td> </tr> ";
				details += "</table>";

				infoDetail.setValue(details.c_str());
			}
			///////////////////////////////////////
			//////// SoCylinderDetail /////////////
			///////////////////////////////////////
			else if (detailTypeId == SoCylinderDetail::getClassTypeId())
			{
				SoCylinderDetail *cylDetail = (SoCylinderDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoCylinderDetail</td> </tr> ");

				details += "<tr> <td>Part Hit</td> <td>";
				switch(cylDetail->getPart())
				{
				case SoCylinder::SIDES:
					details += "SIDES";
					break;
				case SoCylinder::TOP:
					details += "TOP";
					break;
				case SoCylinder::BOTTOM:
					details += "BOTTOM";
					break;
				default:
					details += "ALL";
				}
				details += "</td> </tr> </table>";

				infoDetail.setValue(details.c_str());
			}
			////////////////////////////////////
			//////// SoCubeDetail //////////////
			////////////////////////////////////
			else if (detailTypeId == SoCubeDetail::getClassTypeId())
			{
				SoCubeDetail *cubeDetail = (SoCubeDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoCubeDetail</td> </tr> ");

				details += "<tr> <td>Part Hit</td> <td>";
				switch(cubeDetail->getPart())
				{
				case 0:
					details += "Front";
					break;
				case 1:
					details += "Back";
					break;
				case 2:
					details += "Left";
					break;
				case 3:
					details += "Right";
					break;
				case 4:
					details += "Top";
					break;
				case 5:
					details += "Bottom";
					break;
				default:
					details += "Error";
				}
				details += "</td> </tr> </table>";

				infoDetail.setValue(details.c_str());
			}
			////////////////////////////////////
			//////// SoConeDetail //////////////
			////////////////////////////////////
			else if (detailTypeId == SoConeDetail::getClassTypeId())
			{
				SoConeDetail *coneDetail = (SoConeDetail*) pickDetail;
				std::string details("<table border=1 cellpadding=4> <tr> <td colspan=2>SoConeDetail</td> </tr> ");

				details += "<tr> <td>Part Hit</td> <td>";
				switch(coneDetail->getPart())
				{
				case SoCone::SIDES:
					details += "SIDES";
					break;
				case SoCone::BOTTOM:
					details += "BOTTOM";
					break;
				default:
					details += "ALL";
				}
				details += "</td> </tr> </table>";

				infoDetail.setValue(details.c_str());
			}
			else { infoDetail.setValue("Unknown Detail Type"); }
		}
		else
		{ 
			infoTypeName.setValue("-");
			infoDetail.setValue("-");
		}

		SoPath *pickPath = pPoint->getPath();
		if (pickPath)
		{
			std::string path("<table border=1 cellpadding=4> <tr> <th>i</th> <th>Node</th> <th>Name</th> </tr> ");
			char iA[33];	// buffer for 32-bit int + 0

			for (int i = 0; i < pickPath->getLength(); i++)
			{
				myIToA(i, iA, 10);
				path += "<tr> <td>" + std::string(iA) + "</td> <td>" 
					+ pickPath->getNode(i)->getTypeId().getName().getString() + "</td> <td>"
					+ pickPath->getNode(i)->getName().getString() + "</td> </tr> ";
			}
			path += "</table>";
			infoPickPath.setValue(path.c_str());
		}
		else { infoPickPath.setValue("-"); }
	}
}
// --------------------------------------------------------------------------
void SoPickInfo::_resetFields()
{
	SO_TRACE_IN("SoPickInfo::_resetFields")

	infoHitCoordinate	.setValue(0.0f, 0.0f, 0.0f);
	infoObjHitCoordinate.setValue(0.0f, 0.0f, 0.0f);
	infoHitNormal		.setValue(0.0f, 0.0f, 0.0f);
	infoObjHitNormal	.setValue(0.0f, 0.0f, 0.0f);
	infoStatus			.setValue("Picked nothing.");
	infoTypeName		.setValue("-");
	infoPickPath		.setValue("-");
	infoDetail			.setValue("-");
}
// --------------------------------------------------------------------------
//! Protected destructor to clean up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoPickInfo::~SoPickInfo()
{
	SO_TRACE_IN("SoPickInfo::~SoPickInfo")
}


