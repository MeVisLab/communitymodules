//----------------------------------------------------------------------------------
//! The Inventor module class SoDraw2DLabels
/*!
// \file    SoDraw2DLabels.cpp
// \author  Konrad Mühler
// \date    2008-05-08
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoDraw2DLabels.h"

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
// TODO: include more inventor things here
#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoDraw2DLabels)


const int SoDraw2DLabels::GROUPTYPE_NONE=0;
const int SoDraw2DLabels::GROUPTYPE_PARENT=1;
const int SoDraw2DLabels::GROUPTYPE_CHILD=2;



// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoDraw2DLabels::initClass()
{
	SO_NODE_INIT_CLASS(SoDraw2DLabels, SoView2DExtension, "SoView2DExtension");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoDraw2DLabels::SoDraw2DLabels()
{
	SO_TRACE_IN("SoDraw2DLabels::SoDraw2DLabels");

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoDraw2DLabels);

	//! Set inherited fields to a default state using the default string setting
	//! mechanism available in OpenInventor. E.g. in this case the extension
	//! shall be activated by button 1 and it must be over a valid voxel position
	//! to be activated.
	set("button1 PRESSED button2 IGNORED button3 IGNORED needsValidVoxel FALSE"); // This is the "mouse mask"

	SO_NODE_ADD_FIELD(_CSOFld, (""));

	_fontSize = 12;
	_fontValid = false;
	_font = NULL;
	_innerBorder = 3;	
	
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoDraw2DLabels::~SoDraw2DLabels()
{
	SO_TRACE_IN("SoDraw2DLabels::~SoDraw2DLabels");
}




// -------------------------------------------------
// View2DExtension virtual methods
// -------------------------------------------------
bool SoDraw2DLabels::evalEvent(SoView2D * view2d, View2DSliceList *slicelist,
							   View2DEvent* ec, View2DEventPhase phase)
{
	SO_TRACE_IN("SoDraw2DLabels::evalEvent");
	if (drawingOn.getValue() && _setCSO)
	{
		float vx, vy, vz, wx, wy, wz;		

		// Position is inside slice?
		if (slicelist->mapEventToVoxel(ec, vx, vy, vz))
		{
			// Convert voxel to world coordinates and print coordinates to console.
			slicelist->mapVoxelToWorld(vx, vy, vz, wx, wy, wz);
			//std::cout << "Voxel position: " << vx << " " << vy << " " << vz << std::endl;
			//std::cout << "World position: " << wx << " " << wy << " " << wz << std::endl;

			// Evaluate event phases. On activation and motion draw cross
			// and clean it on release.
			switch (phase)
			{
				case EVENT_START:
				case EVENT_MOTION:
					// Store current selected voxel position in _lastSelectedPosition.
					std::cout << "Voxel position: " << vx << " " << vy << " " << vz << std::endl;
					//Search for matching label
					for (unsigned int i=0; i<_labels.size(); i++)
					{
						if (kBasics::pointInBox(kBasics::POINTF(vx,vy),_labels[i].labelPoint,kBasics::POINT(_labels[i].labelPoint.x+_labels[i].labelSize.x,_labels[i].labelPoint.y+_labels[i].labelSize.y)))
						{
							std::cout << "Hit label " << i << "   Text: " << _labels[i].objID << std::endl;
							_CSOFld.setValue(_labels[i].objID.c_str());
							break;
						}
					}
					break;	
				default:
				break;
			}
		}		
		touch(); // Force redraw.
	}

	return FALSE;
}

void SoDraw2DLabels::draw(View2DSliceList *dsl, View2DSlice *dslice, int slice)
{
	SO_TRACE_IN("SoDraw2DLabels::draw");

	_dsl = dsl;
	_dslice = dslice;

	// Drawing field enabled, i.e. are we supposed to draw?
	if (drawingOn.getValue())
	{
		// Yes, get size of connected image from the slice list. Note that this is the
		// list of slices connected to the SoView2D. Note that there is usually no need
		// for an additional image connected to this SoView2DExtension since that one of
		// the SoView2D accessable by dsl->getInputImage() is the relevant one.
		int x, y, z, c, t;
		dsl->getInputImage()->getSize(x, y, z, c, t);

		//! Get the 2d devices (pixel) coordinates of the origin and the opposite corner of the
		//! 2D region in which the slice is drawn. Note that these coordinates have nothing to do
		//! with voxel coordinates. These coordinates are directly drawable device coordinates.
		float dx1, dy1;
		float dx2, dy2;
		dslice->getVisibleDeviceRect(dx1, dy1, dx2, dy2);


		//Draw all labels
		//float anchorX, anchorY;
		//float lineEndX, lineEndY;
		//float labelX, labelY;
		_font = SoView2D::globalFont(dsl->getCurrentCacheContext());
		_fontValid = true;
		

		// Antialiasing
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

		//1. the lines
		for (unsigned int i=0; i<_labels.size(); i++)
		{
			//Re-Calc label-point (next corner to anchor-point)			
			_font->getTextSize(_fontSize, _labels[i].labelText.c_str(), _labels[i].labelSize.x, _labels[i].labelSize.y);

			dslice->mapVoxelToDevice(_labels[i].anchorPoint.x, _labels[i].anchorPoint.y, slice, _labels[i].anchorPointDev.x, _labels[i].anchorPointDev.y);
			dslice->mapVoxelToDevice(_labels[i].labelPoint.x, _labels[i].labelPoint.y, slice, _labels[i].labelPointDev.x, _labels[i].labelPointDev.y);
			dslice->mapVoxelToDevice(_labels[i].lineEndPoint.x, _labels[i].lineEndPoint.y, slice, _labels[i].lineEndPointDev.x, _labels[i].lineEndPointDev.y);

			kBasics::POINTF anchorPoint = _labels[i].anchorPointDev;
			//kBasics::POINTF labelPoint = _labels[i].labelPointDev;
			kBasics::POINTF lineEndPoint = _labels[i].lineEndPointDev;

			//Correct the anchorPoint if a circle is drawn
			if (_labels[i].encircle)
			{
				//Halo
				glColor3f(0,0,0);
				_circle(anchorPoint.x,anchorPoint.y,_labels[i].encircleSize+1);
				//Normal
				glColor3f(_labels[i].lineColor[0],_labels[i].lineColor[1],_labels[i].lineColor[2]);
				_circle(anchorPoint.x,anchorPoint.y,_labels[i].encircleSize);

				float lineLength = sqrt((anchorPoint.x-lineEndPoint.x)*(anchorPoint.x-lineEndPoint.x)+(anchorPoint.y-lineEndPoint.y)*(anchorPoint.y-lineEndPoint.y));
				kBasics::POINTF tempVec = anchorPoint;
				tempVec.x-=lineEndPoint.x; tempVec.y-=lineEndPoint.y;
				tempVec.x = _labels[i].encircleSize*tempVec.x/lineLength; tempVec.y = _labels[i].encircleSize*tempVec.y/lineLength;
				anchorPoint.x-=tempVec.x; anchorPoint.y-=tempVec.y;				
			}
			else
			{
				//Halo circle
				glColor3f(0,0,0);
				_circleSolid(anchorPoint.x,anchorPoint.y,3);
				//Normal circle
				glColor3f(_labels[i].lineColor[0],_labels[i].lineColor[1],_labels[i].lineColor[2]);
				_circleSolid(anchorPoint.x,anchorPoint.y,2);
			}

			//Halo line
			glColor3f(0,0,0); 
			glBegin(GL_LINES);
			glVertex2f(anchorPoint.x+1, anchorPoint.y+1);
			glVertex2f(lineEndPoint.x+1, lineEndPoint.y+1);
			glEnd();

			//Normal line
			glColor3f(_labels[i].lineColor[0],_labels[i].lineColor[1],_labels[i].lineColor[2]);
			glBegin(GL_LINES);
			glVertex2f(anchorPoint.x, anchorPoint.y);			
			glVertex2f(lineEndPoint.x, lineEndPoint.y);
			glEnd();
		}	


		//2. the label type and boxes
		for (unsigned int i=0; i<_labels.size(); i++)
		{
			//Re-Calc label-point (next corner to anchor-point)			
			kBasics::POINTF labelPoint = _labels[i].labelPointDev;
			kBasics::POINT labelSize = _labels[i].labelSize;

			if (_labels[i].groupType!=GROUPTYPE_CHILD)
			{
				//Draw Box				
				glBegin(GL_POLYGON);			
				glColor4f(_labels[i].boxColor[0],_labels[i].boxColor[1],_labels[i].boxColor[2],_labels[i].boxTransparency);
				glVertex2f(labelPoint.x, labelPoint.y);
				glVertex2f(labelPoint.x+labelSize.x+2*_innerBorder+1, labelPoint.y);
				glVertex2f(labelPoint.x+labelSize.x+2*_innerBorder+1, labelPoint.y+labelSize.y+2*_innerBorder+1);
				glVertex2f(labelPoint.x, labelPoint.y+labelSize.y+2*_innerBorder+1);
				glVertex2f(labelPoint.x, labelPoint.y);
				glEnd();
				glColor3f(_labels[i].borderColor[0],_labels[i].borderColor[1],_labels[i].borderColor[2]); 
				glBegin(GL_LINES);
				glVertex2f(labelPoint.x, labelPoint.y);
				glVertex2f(labelPoint.x+labelSize.x+2*_innerBorder+1, labelPoint.y);
				glEnd();
				glBegin(GL_LINES);
				glVertex2f(labelPoint.x+labelSize.x+2*_innerBorder+1, labelPoint.y);
				glVertex2f(labelPoint.x+labelSize.x+2*_innerBorder+1, labelPoint.y+labelSize.y+2*_innerBorder+1);
				glEnd();
				glBegin(GL_LINES);
				glVertex2f(labelPoint.x+labelSize.x+2*_innerBorder+1, labelPoint.y+labelSize.y+2*_innerBorder+1);
				glVertex2f(labelPoint.x, labelPoint.y+labelSize.y+2*_innerBorder+1);
				glEnd();
				glBegin(GL_LINES);
				glVertex2f(labelPoint.x, labelPoint.y+labelSize.y+2*_innerBorder+1);
				glVertex2f(labelPoint.x, labelPoint.y);
				glEnd();

				labelPoint.x+=_innerBorder; labelPoint.y+=_innerBorder;

				float fcolor[4] = {_labels[i].textColor[0],_labels[i].textColor[1],_labels[i].textColor[2],1};
				_font->drawText(labelPoint.x, labelPoint.y, 
 					_fontSize, 
					fcolor, 
					_labels[i].labelText.c_str(), 
					View2DFont::ALIGN_LEFT,
					false, //shadow
					0 ); //line spacing
			}
		}
	}

}




int SoDraw2DLabels::addLabel(kBasics::POINT &anchorPoint, kBasics::POINT &labelPoint, kBasics::POINT &lineEndPoint, int groupType, string &labelText, int borderDistance, int optFactor, bool encircle, float encircleSize, SbVec3f textColor, SbVec3f boxColor, float boxTransparency, SbVec3f borderColor, SbVec3f lineColor, string &objID)
{
	SO_TRACE_IN("SoDraw2DLabels::addLabel");

	LABEL newLabel;
	newLabel.anchorPoint.x = (anchorPoint.x+1)*optFactor; //irgendwie sind die Anchor-Positions in X um eins geshiftet ... vielleicht täusch ich mich ja auch ... aber für's erste hier mal dieser einfache Hack
	newLabel.anchorPoint.y = anchorPoint.y*optFactor;
	newLabel.labelPoint.x = labelPoint.x*optFactor;
	newLabel.labelPoint.y = labelPoint.y*optFactor;
	newLabel.labelPoint.x+= borderDistance;
	newLabel.labelPoint.y+= borderDistance;
	newLabel.lineEndPoint.x = lineEndPoint.x*optFactor;
	newLabel.lineEndPoint.y = lineEndPoint.y*optFactor;
	newLabel.labelText = labelText;
	newLabel.groupType = groupType;
	newLabel.encircle = encircle;
	newLabel.encircleSize = encircleSize;
	newLabel.textColor = textColor;
	newLabel.boxColor = boxColor;
	newLabel.boxTransparency = boxTransparency;
	newLabel.borderColor = borderColor;
	newLabel.lineColor = lineColor;
	newLabel.objID = objID;
	_labels.push_back(newLabel);
	return 0;
}

void SoDraw2DLabels::clearLabels()
{
	SO_TRACE_IN("SoDraw2DLabels::clearLabels");	
	_labels.clear();
}


void SoDraw2DLabels::_circle(float x, float y, const float r)
{
	/* draw a circle from a bunch of short lines */
	float vectorY1=y+r;
	float vectorX1=x;
	float vectorX, vectorY;
	float angle;
	glBegin(GL_LINE_STRIP);			
	for(int i=0;i<=360;i+=2)
	{
		angle=(float)(((double)i)/57.29577957795135);
		vectorX=x+(r*(float)sin((double)angle));
		vectorY=y+(r*(float)cos((double)angle));		
		glVertex2d(vectorX1,vectorY1);
		vectorY1=vectorY;
		vectorX1=vectorX;			
	}
	glEnd();
}



void SoDraw2DLabels::_circleSolid(float x, float y, const float r)
{
	/* draw a solid disc from a bunch of triangles */
	float vectorY1=y;
	float vectorX1=x;
	float vectorX, vectorY;
	float angle;
	glBegin(GL_TRIANGLES);	
	for(int i=0;i<=360;i+=2)
	{
		angle=(float)(((double)i)/57.29577957795135);
		vectorX=x+(r*(float)sin((double)angle));
		vectorY=y+(r*(float)cos((double)angle));		
		glVertex2d(x,y);
		glVertex2d(vectorX1,vectorY1);
		glVertex2d(vectorX,vectorY);
		vectorY1=vectorY;
		vectorX1=vectorX;	
	}
	glEnd();
}


void SoDraw2DLabels::setFontSize(const int iSize)
{
	_fontSize = iSize;
}

void SoDraw2DLabels::setInnerBorder(const int iValue)
{
	_innerBorder = iValue;
}

void SoDraw2DLabels::setSendCSO(const bool bValue)
{
	_setCSO = bValue;
}


void SoDraw2DLabels::getTextDimension(const string sText, int &width, int &height, int optFactor)
{
	if (_fontValid && _font)
	{
		_font->getTextSize(_fontSize, sText.c_str(), width, height);
		if (_dsl)
		{
			int xSize,ySize,xSizeImg,ySizeImg,zSizeImg;
			_dsl->getWindowSize(xSize,ySize);
			_dsl->getImageSize(xSizeImg,ySizeImg,zSizeImg);
			float factor = float(max(xSizeImg,ySizeImg))/float(min(xSize,ySize));
			width = (width+_innerBorder*2) * factor / optFactor;
			height = (height+_innerBorder*2) * factor / optFactor;
		}
	}
	else
	{
		std::cout << "font not initialized  ... set default values for width and height" << std::endl;
		height=10/optFactor;
		width=100/optFactor;
	}	
}