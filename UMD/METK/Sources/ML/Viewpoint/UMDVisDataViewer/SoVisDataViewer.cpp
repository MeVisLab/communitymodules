//----------------------------------------------------------------------------------
//! The Inventor module class SoVisDataViewer
/*!
// \file    SoVisDataViewer.cpp
// \author  
// \date    2007-04-26
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoVisDataViewer.h"

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
// TODO: include more inventor things here
#include "XVLeaveScope.h"

#include "vpBasic/vpPoint.h"
#include "vpBasic/vpTriangle.h"
#include "vpBasic/vpEdge.h"
#include "vpBasic/vpAlgorithms.h"
#include "vpBasic/vpField.h"
#include "vpBasic/vpDivSphere.h"

#include <Inventor/SbBox.h>

#include <fstream>


SO_NODE_SOURCE(SoVisDataViewer)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoVisDataViewer::initClass()
{
  SO_NODE_INIT_CLASS(SoVisDataViewer, SoShape, "Shape" /* SoSeparator, "Separator"*/);
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoVisDataViewer::SoVisDataViewer()
{
  SO_TRACE_IN("SoVisDataViewer::SoVisDataViewer")

  // Execute inventor internal stuff for node construction.
  SO_NODE_CONSTRUCTOR(SoVisDataViewer);




  // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
  // sensor to be sure that changes are not delayed or collected.
  _nodeSensor = new SoNodeSensor(SoVisDataViewer::nodeChangedCB, this);
  _nodeSensor->setPriority(0);
  _nodeSensor->attach(this);

   m_flHasSphere = false;
   m_flHasData = false;
   m_iSphereMode = 0;
   iInspect = -1;


}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoVisDataViewer::~SoVisDataViewer()
{
  SO_TRACE_IN("SoVisDataViewer::~SoVisDataViewer")


  // Remove the node sensor.
  if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoVisDataViewer::nodeChangedCB(void *data, SoSensor* sens)
{
  SO_TRACE_IN("SoVisDataViewer::nodeChangedCB")

  ((SoVisDataViewer*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoVisDataViewer::nodeChanged(SoNodeSensor* /*sensor*/)
{
  SO_TRACE_IN("SoVisDataViewer::nodeChanged")

  // Get the field which caused the notification.
  //SoField* field = sensor->getTriggerField();

  // Handle changed fields here
}

void SoVisDataViewer::createSphere (float fSx, float fSy, float fSz, float fRadius, int iDiv)
{
  m_oSphere = new CvpDivSphere(fSx, fSy, fSz, fRadius, iDiv, 1);
  QuickSort(m_oSphere->vPoints);
  m_flHasSphere = true;
}

void SoVisDataViewer::setDataField (CvpField *oField)
{
  m_flHasData = false;
  if ( !m_flHasSphere ) return;
  if ( oField == NULL ) return;
  if (oField->getSize() != m_oSphere->vPoints.size()) return;
  m_oField = oField;
  m_flHasData = true;
  //logResults();
}

void SoVisDataViewer::setSphereMode (unsigned int iMode)
{
  m_iSphereMode = iMode;  
  if (m_iSphereMode > 2) m_iSphereMode = 2; 
}
/*void SoVisDataViewer::drawTriangle(int i1,int i2,int i3){
	
}*/
void SoVisDataViewer::GLRender/*BelowPath*/(SoGLRenderAction *action)
{
  float fColor1=1.0,fColor2=1.0,fColor3=1.0,fColor4=1.0;
  if (m_flHasSphere && m_iSphereMode > 0)
  {
    //cout<<"render sphere"<<endl;
    glDisable(GL_LIGHTING);
    for (unsigned int i=0; i< m_oSphere->vTriangles.size(); i++)
    {
      int iWrong = m_oSphere->vTriangles[i]->getWrongEdge();
	  CvpPoint *oP1,*oP2,*oP3;
	  if(iWrong==0){
		oP1 = m_oSphere->vTriangles[i]->getEdge1()->getPoint1();
		oP2 = m_oSphere->vTriangles[i]->getEdge2()->getPoint1();
		oP3 = m_oSphere->vTriangles[i]->getEdge3()->getPoint1();
	  }else{
		if (iWrong == 1) oP1 = m_oSphere->vTriangles[i]->getEdge1()->getPoint2();
		else oP1 = m_oSphere->vTriangles[i]->getEdge1()->getPoint1();
		if (iWrong == 2) oP2 = m_oSphere->vTriangles[i]->getEdge2()->getPoint2();
		else oP2 = m_oSphere->vTriangles[i]->getEdge2()->getPoint1();
		if (iWrong == 3) oP3 = m_oSphere->vTriangles[i]->getEdge3()->getPoint2();
        else oP3 = m_oSphere->vTriangles[i]->getEdge3()->getPoint1();
	  }
      if (m_iSphereMode == 1)
      {
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
		if (m_flHasData){
			fColor1 = m_oField->getValueAt(oP1->id);
			fColor2 = m_oField->getValueAt(oP2->id);
			fColor3 = m_oField->getValueAt(oP3->id);
			fColor4 = m_oField->getValueAt(oP1->id);
		}
		glColor3f(fColor1,fColor1,fColor1);
		glVertex3f(oP1->x,oP1->y,oP1->z);
		glColor3f(fColor2,fColor2,fColor2);
		glVertex3f(oP2->x,oP2->y,oP2->z);
		glColor3f(fColor3,fColor3,fColor3);
		glVertex3f(oP3->x,oP3->y,oP3->z);
		glColor3f(fColor4,fColor4,fColor4);
		glVertex3f(oP1->x,oP1->y,oP1->z);      
		glEnd();
        glLineWidth(1.0);
      }
      if (m_iSphereMode == 2)
      {
        glBegin(GL_POLYGON);
		if (m_flHasData){
			fColor1 = m_oField->getValueAt(oP1->id);
			fColor2 = m_oField->getValueAt(oP2->id);
			fColor3 = m_oField->getValueAt(oP3->id);
		}
		glColor3f(fColor1,fColor1,fColor1);
		glVertex3f(oP1->x,oP1->y,oP1->z);
		glColor3f(fColor2,fColor2,fColor2);
		glVertex3f(oP2->x,oP2->y,oP2->z);
		glColor3f(fColor3,fColor3,fColor3);
		glVertex3f(oP3->x,oP3->y,oP3->z);      
        glEnd();
      }

      if (oP1->id == iInspect)
      {
      float x,y,z,id;
      glPointSize(5.0);
      glBegin(GL_POINTS);
      glColor3f(1,1,0);
      glVertex3f(oP1->x, oP1->y, oP1->z);
      if (m_flHasData) cout<<"PointValue: "<<m_oField->getValueAt(oP1->id)<<endl;
      cout<<"------"<<endl;
      for (int j=0; j<oP1->getNrOfNeighbours(); j++)
      {
        if (j==0) glColor3f(1,0,0);
        else if (j==1) glColor3f(0,1,0);
        else if (j==2) glColor3f(0,0,1);
        else if (j==3) glColor3f(.5,0,0);
        else if (j==4) glColor3f(0,.5,0);
        else if (j==5) glColor3f(0,0,.5);
        x = oP1->getNeighbour(j)->x;
        y = oP1->getNeighbour(j)->y;
        z = oP1->getNeighbour(j)->z;
        id = oP1->getNeighbour(j)->id;
        if (m_flHasData) cout<<j<<": "<<m_oField->getValueAt(id)<<endl;
        glVertex3f(x,y,z);
      }
      cout<<"------"<<endl;
      glEnd();
      glPointSize(1.0);
      }

    }
    
    glEnable(GL_LIGHTING);
  }
}

void SoVisDataViewer::computeBBox(SoAction * action, SbBox3f & box, SbVec3f & center)
{
  if (!m_flHasSphere)
  {
    center.setValue(0.0f, 0.0f, 0.0f);
    box.setBounds(0, 0, 0, 0, 0, 0);
  }
  else
  {
    float fX,fY,fZ,fR;
    fX = m_oSphere->fMx;
    fY = m_oSphere->fMy;
    fZ = m_oSphere->fMz;
    fR = m_oSphere->fRadius;
    center.setValue(fX,fY,fZ);
    //cout<<fX<<","<<fY<<","<<fZ<<","<<fR<<endl;
    box.setBounds(fX-fR,fY-fR,fZ-fR,fX+fR,fY+fR,fZ+fR);
  }
}


void SoVisDataViewer::generatePrimitives(SoAction *action)
{
  SO_TRACE_IN("SoTest::generatePrimitives")

  // TODO
}

void SoVisDataViewer::logResults (void)
{
  unsigned int i;
  ofstream fout;
  fout.open("c:/viewer.txt",ios::out);
  fout<<"Spherepoints:"<<endl;
  fout<<"-------------"<<endl;
  
  for (i = 0; i<m_oSphere->vPoints.size(); i++)
  {
    fout<<i<<": ("<<m_oSphere->vPoints[i]->x<<","<<m_oSphere->vPoints[i]->y<<","<<m_oSphere->vPoints[i]->z<<") : "<<m_oSphere->vPoints[i]->id<<endl;
  }
  fout<<"ResultMatrix:"<<endl;
  fout<<"-------------"<<endl;
  
  fout<<"Visibility for Object 0"<<endl;
  fout<<"-----------------------"<<endl;
  for (i = 0; i<m_oSphere->vPoints.size(); i++)
  {
    fout<<i<<": "<<m_oField->getValueAt(i)<<endl;
  }
  fout.close();
}

void SoVisDataViewer::inspectPoint(int iIndex)
{
  iInspect = iIndex;
}




