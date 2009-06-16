//----------------------------------------------------------------------------------
//! The Inventor module class SoDraw2DSilhouette
/*!
// \file    SoDraw2DSilhouette.cpp
// \author  Björn Meyer
// \date    2004-12-17
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDSoView2DExtensionsSystem.h"
#include "SoDraw2DSilhouette.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
// TODO: include more inventor things here
#include "XVLeaveScope.h"


SO_NODE_SOURCE(SoDraw2DSilhouette)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoDraw2DSilhouette::initClass()
{
  SO_NODE_INIT_CLASS(SoDraw2DSilhouette, SoView2DExtension, "SoView2DExtension");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoDraw2DSilhouette::SoDraw2DSilhouette()
{
  // Execute inventor internal stuff for node construction.
  SO_NODE_CONSTRUCTOR(SoDraw2DSilhouette);

  // node connector for the SoIndexedLineSet containing the silhouette information
  SO_NODE_ADD_FIELD(inLineSet, (NULL));
	SO_NODE_ADD_FIELD(barycentre, (0,0,0));
	SO_NODE_ADD_FIELD(stippleFactor, (0));

  //! Set inherited fields to a default state using the default string setting
  //! mechanism available in OpenInventor. E.g. in this case the extension
  //! shall be activated by button 1 and it must be over a valid voxel position
  //! to be activated.
  set("button1 IGNORED button2 PRESSED button3 IGNORED needsValidVoxel TRUE");


  // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
  // sensor to be sure that changes are not delayed or collected.
  _nodeSensor = new SoNodeSensor(SoDraw2DSilhouette::nodeChangedCB, this);
  _nodeSensor->setPriority(0);
  _nodeSensor->attach(this);


  _lineSetDetected = false;
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoDraw2DSilhouette::~SoDraw2DSilhouette()
{

  // Remove the node sensor.
  if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoDraw2DSilhouette::nodeChangedCB(void *data, SoSensor* sens)
{
  ((SoDraw2DSilhouette*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//**************************************
// check the input connector for new data -> a new SoIndexedLineSet
//**************************************
void SoDraw2DSilhouette::nodeChanged(SoNodeSensor* sensor)
{
  // Get the field which caused the notification.
  SoField* field = sensor->getTriggerField();

  // Handle changed fields here

  // if inconnector inLineSet changes
  if (field==&inLineSet){

	  // check if pointing to a node
	  if (inLineSet.getValue())
	  {
		  if (!(inLineSet.getValue()->getTypeId().isBad()) && inLineSet.getValue()->getTypeId().isDerivedFrom(SoIndexedLineSet::getClassTypeId()))
		  {
			  if ( ((SoIndexedLineSet*)inLineSet.getValue())->vertexProperty.getValue() != 0)
			  {
				  // cast delivered pointer as SoIndexedLineSet
				  _lineSet = (SoIndexedLineSet*)inLineSet.getValue();
				  _vertexProp = (SoVertexProperty*)(_lineSet->vertexProperty.getValue());
				  _lineSetDetected = true;
				  std::cout << "SoIndexedLineSet detected !" << std::endl;
			  }
			  else std::cout << " No VertexProperty detected !" << std::endl;
		  }
		  else if (!(inLineSet.getValue()->getTypeId().isBad()) && inLineSet.getValue()->getTypeId().isDerivedFrom(SoGroup::getClassTypeId()))
		  {
			  //std::cout << "SoGroup detected !" << std::endl;
			  int childIdx = ((SoGroup*)inLineSet.getValue() )->getNumChildren();
			  if ( childIdx > 0)
			  {
				  //std::cout << "Anzahl der gefundenen Children: " << childIdx << std::endl;
				  int childCnt = 0;
				  while (childCnt < childIdx)
				  {
					  if( ((SoGroup*)inLineSet.getValue())->getChild(childCnt)->isOfType(SoMaterial::getClassTypeId()) )
					  {
						  ((SoMaterial*)((SoGroup*)inLineSet.getValue())->getChild(childCnt))->ambientColor[0].getValue(_colour[0], _colour[1], _colour[2]);
					  }
					  else if ( ((SoGroup*)inLineSet.getValue())->getChild(childCnt)->isOfType(SoIndexedLineSet::getClassTypeId()) )
					  {
						  // cast delivered pointer as SoIndexedLineSet
						  _lineSet = (SoIndexedLineSet*)((SoGroup*)inLineSet.getValue())->getChild(childCnt);
						  _vertexProp = (SoVertexProperty*)(_lineSet->vertexProperty.getValue());
						  _lineSetDetected = true;
						  //_drawSilhouette(_activeView2D->getSliceList());
						  //_drawSilhouette();
						  //std::cout << "child nummer " << childIdx << " ist SoIndexedLineSet" << std::endl;
					  }
					  childCnt++;
				  }
			  }
		  }

		  else
		  {
			  std::cout << " No LineSet detected !" << std::endl;
			  _lineSet = NULL;
			  _lineSetDetected = false;
		  }
	  }
	  else
	  {
		  std::cout << " No valid pointer detected !" << std::endl;
		  _lineSet = NULL;
		  _lineSetDetected = false;
	  }
  }
}


// -------------------------------------------------
// View2DExtension virtual methods
// -------------------------------------------------
bool SoDraw2DSilhouette::evalEvent(SoView2D * view2d, View2DSliceList *slicelist,
                                View2DEvent* ec, View2DEventPhase phase)
{
// in comments, because problem solved inside the MevisLab Network via field-to-field connections
/*
	if (isEditingOn())
	{
		float z, wx, wy, wz;
		
		switch (phase)
		{
			case EVENT_MOTION:
			case EVENT_MOUSEWHEEL:
				// if scrolling through slices: get slice z value
				z = view2d->startSlice.getValue();// + view2d->sliceStep.getValue();
				// update current Voxel Z field
				zEbeneVoxel.setValue(z);
				// convert voxel to world z
				slicelist->mapVoxelToWorld(0, 0, z, wx, wy, wz);
				// update current World Z field
				zEbeneWorld.setValue(wz);
				break;

			default:
				break;
      }
	}

 */
	return FALSE;
}


//*************************************
// Draw the SoIndexedLineSet
//*************************************
void SoDraw2DSilhouette::draw(View2DSliceList *dsl, View2DSlice *dslice, int slice)
{
  // Drawing field enabled and lineSet existent, i.e. are we supposed to draw?
  //if (drawingOn.getValue() && (_lineSet->getClassTypeId()==SoIndexedLineSet::getClassTypeId()) && (_lineSet->vertexProperty.getClassTypeId() == SoSFNode::getClassTypeId()))
  if (drawingOn.getValue() && _lineSetDetected)
  {
	  _drawSilhouette(dslice);
	  //_drawSilhouette();
  } //end if
}


void SoDraw2DSilhouette::_drawSilhouette(View2DSlice *slice)
//void SoDraw2DSilhouette::_drawSilhouette()
{
	int n = 0;
	float x, y, bx, by, bz;
	bx = by = bz = 0;
	const SbVec3f *vector;

	if (stippleFactor.getValue() == 0) glDisable(GL_LINE_STIPPLE);
	else{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(abs(stippleFactor.getValue()), 0x0101);
	}

	// get vertices of the silhouette and draw them as GL_LINE_STRIP
	while (n < _lineSet->coordIndex.getNum())
	{

	  glBegin(GL_LINE_STRIP);

	  // change OpenGL colour to values stored in SoMaterial node
		if (stippleFactor.getValue() > 0)
		{
			glColor3f(_colour[0] + 0.3, _colour[1] + 0.3, _colour[2] + 0.3);
		}
		else 
		{
			glColor3f(_colour[0], _colour[1], _colour[2]);
		}
	  // draw one separated lineStrip
	  while ( (_lineSet->coordIndex[n] != -1) && (n <= _lineSet->coordIndex.getNum() - 1) )
	  {
		  // get current vertex/vector from lineSet
		  vector = _vertexProp->vertex.getValues(_lineSet->coordIndex[n]);
		  // map coordinates to window
		  slice->mapWorldToDevice(*vector, x, y);
		  // draw a new vertex/line-segment
		  glVertex2f(x,y);
		  n++;

			// add up coordinates

			//barycentre.getValue()+=(*vector);

			bx += (*vector)[0];
			by += (*vector)[1];
			bz += (*vector)[2];

	  } // end while

	  glEnd();

		n++;
	} //end while

	// set barycentre
	barycentre.setValue(bx/(n-1), by/(n-1), bz/(n-1));

	glDisable(GL_LINE_STIPPLE);
}
