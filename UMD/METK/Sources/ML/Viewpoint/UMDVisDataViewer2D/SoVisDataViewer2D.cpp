//----------------------------------------------------------------------------------
//! The Inventor module class SoVisDataViewer2D
/*!
// \file    SoVisDataViewer2D.cpp
// \author
// \date    2007-04-26
//
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoVisDataViewer2D.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/actions/SoGLRenderAction.h>
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


SO_NODE_SOURCE(SoVisDataViewer2D)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoVisDataViewer2D::initClass()
{
	SO_NODE_INIT_CLASS(SoVisDataViewer2D, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoVisDataViewer2D::SoVisDataViewer2D()
{
	SO_TRACE_IN("SoVisDataViewer2D::SoVisDataViewer2D");

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoVisDataViewer2D);

	// Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
	// sensor to be sure that changes are not delayed or collected.
	_nodeSensor = new SoNodeSensor(SoVisDataViewer2D::nodeChangedCB, this);
	_nodeSensor->setPriority(0);
	_nodeSensor->attach(this);

	m_flHasSphere = false;
	m_flHasData = false;
	cout<<"start"<<endl;
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoVisDataViewer2D::~SoVisDataViewer2D()
{
	SO_TRACE_IN("SoVisDataViewer2D::~SoVisDataViewer2D");


	// Remove the node sensor.
	if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoVisDataViewer2D::nodeChangedCB(void *data, SoSensor* sens)
{
	SO_TRACE_IN("SoVisDataViewer2D::nodeChangedCB");

	((SoVisDataViewer2D*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoVisDataViewer2D::nodeChanged(SoNodeSensor* /*sensor*/)
{
	SO_TRACE_IN("SoVisDataViewer2D::nodeChanged");

	// Get the field which caused the notification.
	//SoField* field = sensor->getTriggerField();

	// Handle changed fields here
}

void SoVisDataViewer2D::createSphere (float fSx, float fSy, float fSz, float fRadius, int iDiv)
{
	m_oSphere = new CvpDivSphere(fSx, fSy, fSz, fRadius, iDiv, 1);
	QuickSort(m_oSphere->vPoints);
	m_flHasSphere = true;
}

void SoVisDataViewer2D::setDataField (CvpField *oField)
{
	m_flHasData = false;
	if ( !m_flHasSphere ) return;
	if ( oField == NULL ) return;
	if (oField->getSize() != m_oSphere->vPoints.size()) return;
	m_oField = oField;
	m_flHasData = true;
	//logResults();
}


void SoVisDataViewer2D::GLRenderBelowPath(SoGLRenderAction *action)
{
	float fColor1=1.0,fColor2=1.0,fColor3=1.0;
	cout<<"ping"<<endl;


	// Save the current transformation and attribute state of OpenGL.
	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	SoState * state = action->getState();
	const SbViewportRegion & vp = SoViewportRegionElement::get(state);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();                                           //Einheitsmatrix
	glMatrixMode(GL_PROJECTION);                                //"2D"-Modus
	glLoadIdentity();                                           //Einheitsmatrix



	/*std::cout << "window: " << vp.getWindowSize()[0] << "," << vp.getWindowSize()[1] << std::endl;
	glBegin(GL_POLYGON);
	glColor3f(1.0,0.0,0.0);
	glVertex2f(0, 0);

	glColor3f(1.0,1.0,0.0);
	glVertex2f(-1, -1 );

	glColor3f(1.0,0.0,1.0);
	glVertex2f(0,-1);

	glColor3f(1.0,0.0,1.0);
	glVertex2f(0,0);

	glEnd();*/

	float maxalpha, minalpha, maxbeta, minbeta;
	const float PI = 3.14159;
	maxalpha=0;
	maxbeta=0;
	minalpha=100;
	minbeta=100;

	if (m_flHasSphere)
	{
		cout<<"render sphere"<<endl;

		//Spehere Center
		float centerX = m_oSphere->fMx;
		float centerY = m_oSphere->fMy;
		float centerZ = m_oSphere->fMz;

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

			float alpha1, beta1;			
			getAlphaBeta(SbVec3f(oP1->x-centerX,oP1->y-centerY,oP1->z-centerZ),alpha1,beta1);	
			if (alpha1<minalpha) minalpha=alpha1;
			if (beta1<minbeta) minbeta=beta1;
			if (alpha1>maxalpha) maxalpha=alpha1;
			if (beta1>maxbeta) maxbeta=beta1;
			float alpha2, beta2;
			getAlphaBeta(SbVec3f(oP2->x-centerX,oP2->y-centerY,oP2->z-centerZ),alpha2,beta2);
			float alpha3, beta3;
			getAlphaBeta(SbVec3f(oP3->x-centerX,oP3->y-centerY,oP3->z-centerZ),alpha3,beta3);


			//Normierung der Winkel auf x,y [-1,1] des Viewports
			alpha1=(alpha1/PI)*2.0-1.0;
			alpha2=(alpha2/PI)*2.0-1.0;
			alpha3=(alpha3/PI)*2.0-1.0;
			beta1=(beta1/PI)*2.0-1.0;
			beta2=(beta2/PI)*2.0-1.0;
			beta3=(beta3/PI)*2.0-1.0;

			//std::cout << "alpha1=" << alpha1 << "   beta1=" << beta1 << std::endl;

			if (m_flHasData){
				fColor1 = m_oField->getValueAt(oP1->id);
				fColor2 = m_oField->getValueAt(oP2->id);
				fColor3 = m_oField->getValueAt(oP3->id);				
			}

			glBegin(GL_POLYGON);
			glColor3f(fColor1,fColor1,fColor1);
			glVertex2f(alpha1,beta1);
			glColor3f(fColor2,fColor2,fColor2);
			glVertex2f(alpha2,beta2);
			glColor3f(fColor3,fColor3,fColor3);
			glVertex2f(alpha3,beta3);
			glColor3f(fColor1,fColor1,fColor1);
			glVertex2f(alpha1,beta1);
			glEnd();

			/*glPointSize(20.0);
			glBegin(GL_POINTS);
			glColor3f(fColor1,fColor1,fColor1);
			glVertex2f(alpha1,beta1);
			glColor3f(fColor2,fColor2,fColor2);
			glVertex2f(alpha2,beta2);
			glColor3f(fColor3,fColor3,fColor3);
			glVertex2f(alpha3,beta3);
			glColor3f(fColor1,fColor1,fColor1);
			glVertex2f(alpha1,beta1);
			glEnd();*/
		}
	}

	//std::cout << "maxalpha=" << maxalpha << "   maxbeta=" << maxbeta << std::endl;
	//std::cout << "minalpha=" << minalpha << "   minbeta=" << minbeta << std::endl;

	glEnable(GL_LIGHTING);

	// Restore the transformation and attribute state of OpenGL.
	glPopAttrib();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode (GL_PROJECTION);
	glPopMatrix();
	glMatrixMode (GL_MODELVIEW);
}

void SoVisDataViewer2D::getAlphaBeta(SbVec3f vec1,float& alpha, float& beta)
{
	alpha = acos(SbVec3f(0,1,0).dot(vec1)/vec1.length());
	beta = acos(SbVec3f(0,0,1).dot(vec1)/vec1.length());
}



/*void SoVisDataViewer2D::computeBBox(SoAction * action, SbBox3f & box, SbVec3f & center)
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
}*/


/*void SoVisDataViewer2D::generatePrimitives(SoAction *action)
{
	SO_TRACE_IN("SoTest::generatePrimitives");

		// TODO
}*/

void SoVisDataViewer2D::logResults (void)
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


