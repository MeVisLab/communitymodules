//----------------------------------------------------------------------------------
//! The Inventor module class SoVisibilitySolver
/*!
// \file    SoVisibilitySolver.cpp
// \author
// \date    2007-04-16
//
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoVisibilitySolver.h"
#include "SomnOffscreenRenderer.h"
#include "image_basic.h"
#include "vpBasic/vpDivSphere.h"
#include "vpBasic/vpPoint.h"
#include "vpBasic/vpTriangle.h"
#include "vpBasic/vpEdge.h"
#include "vpBasic/vpAlgorithms.h"
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <stdio.h>
#include <fstream>


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

#include <iostream>
using namespace std;

SoVisibilitySolver *globalThisPtr; // for global _insideGlFuncWrap callback

void _insideGlFuncWrap (void);

SO_NODE_SOURCE(SoVisibilitySolver)

const short SoVisibilitySolver::PARAM_VISIBLEPIXEL=0;
const short SoVisibilitySolver::PARAM_MINDIST=1;
const short SoVisibilitySolver::PARAM_MAXDIST=2;
const short SoVisibilitySolver::PARAM_SILHOUETTE=3;
const short SoVisibilitySolver::PARAM_CENTERVALUE=4;
const short SoVisibilitySolver::PARAM_COUNT=5;


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoVisibilitySolver::initClass()
{
  SO_NODE_INIT_CLASS(SoVisibilitySolver, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoVisibilitySolver::SoVisibilitySolver()
{
  SO_TRACE_IN("SoVisibilitySolver::SoVisibilitySolver")

  // Execute inventor internal stuff for node construction.
  SO_NODE_CONSTRUCTOR(SoVisibilitySolver);
  SO_NODE_ADD_FIELD(m_fldScene, (NULL));
  SO_NODE_ADD_FIELD(m_fldCompute, ());
  SO_NODE_ADD_FIELD(m_fldBufferSizeX, (0));
  SO_NODE_ADD_FIELD(m_fldBufferSizeY, (0));
  SO_NODE_ADD_FIELD(m_fldDivision, (0));
  SO_NODE_ADD_FIELD(m_fldDataPath,(""));
  SO_NODE_ADD_FIELD(m_fldSphereMode, (0));
  SO_NODE_ADD_FIELD(m_fldCurrentObj, (0));
  SO_NODE_ADD_FIELD(m_fldProgress, (0));
  SO_NODE_ADD_FIELD(m_fldEstTime, (""));

  // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
  // sensor to be sure that changes are not delayed or collected.
  _nodeSensor = new SoNodeSensor(SoVisibilitySolver::nodeChangedCB, this);
  _nodeSensor->setPriority(0);
  _nodeSensor->attach(this);
  m_soOff = new SomnOffscreenRenderer();
  m_soOff->setInsideGlFunc(_insideGlFuncWrap);
  m_soCamera = new SoOrthographicCamera();
  m_soLight = new SoDirectionalLight();
  m_soSep = new SoSeparator();
  m_soSep->addChild(m_soLight);
  m_soSep->addChild(m_soCamera);
  m_soBboxAction = new SoGetBoundingBoxAction(SbViewportRegion(1,1));
  m_strPathVis = NULL;
  m_strPathIds = NULL;
  globalThisPtr = this;
  m_flHasSphere = false;
  m_fAllFrameBuffers = NULL;
  m_fResultMatrix = NULL;

  //Timer to interupt the calculation process for GUI refreshs and text outputs
  timerComputeLoop = new SoTimerSensor((SoSensorCB*)SoVisibilitySolver::timerEventComputeLoop, this);
  timerComputeLoop->setInterval(SbTime(1.0/400.0));
  timerComputeLoop->unschedule();
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoVisibilitySolver::~SoVisibilitySolver()
{
  SO_TRACE_IN("SoVisibilitySolver::~SoVisibilitySolver")

  // Remove the node sensor.
  if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoVisibilitySolver::nodeChangedCB(void *data, SoSensor* sens)
{
  SO_TRACE_IN("SoVisibilitySolver::nodeChangedCB")

  ((SoVisibilitySolver*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoVisibilitySolver::nodeChanged(SoNodeSensor* sensor)
{
  SO_TRACE_IN("SoVisibilitySolver::nodeChanged")

  // Get the field which caused the notification.
  SoField* field = sensor->getTriggerField();
  if (field == &m_fldCompute)
  {
    computeStart();
  }

  if (field == &m_fldBufferSizeX)
  {
    m_soOff->setSizeX(m_fldBufferSizeX.getValue());
  }

  if (field == &m_fldBufferSizeY)
  {
    m_soOff->setSizeY(m_fldBufferSizeY.getValue());
  }

  if (field == &m_fldDataPath)
  {
    if (m_strPathVis != NULL) delete m_strPathVis;
    if (m_strPathIds != NULL) delete m_strPathIds;
    m_strPathVis = new string(m_fldDataPath.getValue().getString());
    m_strPathVis->append("camera_data.dat");                                                      //wenn alles fertig, hier später dat !!!!!!!!!!!!!!!!!!!!!
    m_strPathIds = new string(m_fldDataPath.getValue().getString());
    m_strPathIds->append("camera_ids.dat");                                                      //wenn alles fertig, hier später dat !!!!!!!!!!!!!!!!!!!!!
  }
}


void SoVisibilitySolver::computeStart()
{
  m_soCamera->nearDistance.setValue(0);
  m_soCamera->farDistance.setValue(m_fRadius*2);
    
  m_soSwitch = (SoSwitch*) m_fldScene.getValue();
  m_soSep->addChild(m_soSwitch);

  m_iNrOfObjects = m_soSwitch->getNumChildren()/2;

  _createSphere();
  m_flHasSphere = true;

  m_soOff->createBuffer();
  m_soOff->setSceneRoot(m_soSep);

  int ri1,ri2,ri3;
  if (m_fResultMatrix!=NULL)
  {
    for (ri1 = 0; (unsigned int)ri1<m_vpSphere->vPoints.size(); ri1++)
    {
      for (ri2 = 0; ri2< m_iNrOfObjects; ri2++)
      {
        delete[] m_fResultMatrix[ri1][ri2];
      }
      delete[] m_fResultMatrix[ri1];
    }
    delete[] m_fResultMatrix;
  }

  m_fAllFrameBuffers = new float[m_soOff->getSizeX()*m_soOff->getSizeY()*(m_iNrOfObjects+1)];
    
  m_fResultMatrix = new float**[m_vpSphere->vPoints.size()];
  for (ri1 = 0; (unsigned int)ri1<m_vpSphere->vPoints.size(); ri1++)
  {
    m_fResultMatrix[ri1] = new float*[m_iNrOfObjects];
    for (ri2 = 0; ri2 < m_iNrOfObjects; ri2++)
    {
      m_fResultMatrix[ri1][ri2] = new float[m_iNrOfObjects+PARAM_COUNT];
      for (ri3 = 0; ri3 < m_iNrOfObjects+PARAM_COUNT; ri3++)
      {
        if (ri3 == m_iNrOfObjects + PARAM_MINDIST) m_fResultMatrix[ri1][ri2][ri3] = 1.0;
        else m_fResultMatrix[ri1][ri2][ri3] = 0.0;
      }
    }
  }

  iTestCount = 0;

  m_fBufferPosition = m_fAllFrameBuffers;
  m_fldProgress.setValue(0);
  m_iCurrentViewpoint = 0;
  startTime = kBasics::getCurrentTime();
  timerComputeLoop->schedule();
}


void SoVisibilitySolver::timerEventComputeLoop(void* data, SoDataSensor* a)
{
  ((SoVisibilitySolver*)data)->computeLoop();
}


void SoVisibilitySolver::computeLoop()
{
  timerComputeLoop->unschedule();
  if ((unsigned int)m_iCurrentViewpoint<m_vpSphere->vPoints.size())
  {
    cout<<m_iCurrentViewpoint<<"/"<<m_vpSphere->vPoints.size()<< "       " << float((float(m_iCurrentViewpoint+1))/(float)m_vpSphere->vPoints.size()) << endl;
    m_fldProgress.setValue(float((float(m_iCurrentViewpoint+1))/(float)m_vpSphere->vPoints.size()));
    double timeDiff = double(kBasics::getCurrentTime()-startTime);///1000.0;
    double esttime = timeDiff/(m_iCurrentViewpoint+1)*(m_vpSphere->vPoints.size()-m_iCurrentViewpoint-1);
    
    int minutes = floor(esttime/60.0);
    int sek = floor(esttime-(minutes*60.0));

    cout << "esttime:" << esttime << "  minutes:" << minutes << "   sec:" << sek << std::endl;
    
    string esttime_str = kBasics::leadingZero(minutes,2)+":"+kBasics::leadingZero(sek,2);
    m_fldEstTime.setValue(esttime_str.c_str());

    m_soCamera->position.setValue(SbVec3f(m_vpSphere->vPoints[m_iCurrentViewpoint]->x,
                                          m_vpSphere->vPoints[m_iCurrentViewpoint]->y,
                                          m_vpSphere->vPoints[m_iCurrentViewpoint]->z));
    m_soCamera->pointAt(m_vCenter);
    m_soCamera->nearDistance.setValue(0);
    m_soCamera->farDistance.setValue(m_fRadius*2);

    m_flRenderAll = true;
    m_iSwitchCount = 1;
    cout << "->render" << endl;
    for (int j = 0; j< m_iNrOfObjects + 1; j++)     //Gesamtbuffer+alle ID-Buffer
    {
      iTestCount++;
      if (m_flRenderAll)
      {
        m_soSwitch->whichChild = -3;
        m_flRenderAll = false;
      }
      else
      {
        m_soSwitch->whichChild = m_iSwitchCount;
        m_iSwitchCount += 2;
      }
      m_soOff->render();
      m_fBufferPosition += m_soOff->getSizeX() * m_soOff->getSizeY();
    }
    cout << "->analyze" << endl;
    _analyzeCurrentViewpoint();
    m_flRenderAll = true;
    m_iSwitchCount = 1;
    m_fBufferPosition = m_fAllFrameBuffers;

    m_iCurrentViewpoint++;
    timerComputeLoop->schedule();
  }
  else
  {
    computeFinalize();
  }
}


void SoVisibilitySolver::computeFinalize ()
{
  _saveResultMatrix();

  delete[] m_fAllFrameBuffers;
  m_soSep->removeChild(2);                          //mögliche Fehlerquelle wenn nicht 0-basiert.

  logResults();

  m_soSwitch->whichChild = -3;
}


//Hier wird der Buffer gefüllt
void SoVisibilitySolver::_insideGlFunc ()
{
  glReadPixels(0,0,m_soOff->getSizeX(),m_soOff->getSizeY(),GL_DEPTH_COMPONENT,GL_FLOAT, m_fBufferPosition);
}

void _insideGlFuncWrap (void)
{
  globalThisPtr->_insideGlFunc();
}

void SoVisibilitySolver::_createSphere ( )
{
  float   fSizeX,fSizeY,fSizeZ,fBiggest;
  SbVec3f vMinPoint;

  m_soBboxAction->apply(m_soSwitch);
  m_vCenter = m_soBboxAction->getBoundingBox().getCenter();               //Kugelmittelpunkt
  m_soBboxAction->getBoundingBox().getSize(fSizeX,fSizeY,fSizeZ);

  if ((fSizeX>=fSizeY)&&(fSizeX>=fSizeZ)) fBiggest=fSizeX;
  if ((fSizeY>=fSizeX)&&(fSizeY>=fSizeZ)) fBiggest=fSizeY;
  if ((fSizeZ>=fSizeX)&&(fSizeZ>=fSizeY)) fBiggest=fSizeZ;            //längste BBox-Ausdehnung
  
  m_soCamera->height.setValue(fBiggest*1.5);

  vMinPoint = m_soBboxAction->getBoundingBox().getMin();
  m_fRadius = (vMinPoint-m_vCenter).length();                     //Kugelradius
  
  float mx,my,mz;
  m_vCenter.getValue(mx,my,mz);

  //int mix,miy,miz,m_iRadius;
  float fRound = 100;

  float newRadius, newX, newY, newZ;
  newRadius = floor(m_fRadius*fRound+0.5)/fRound;
  newX = floor(mx*fRound+0.5)/fRound;
  newY = floor(my*fRound+0.5)/fRound;
  newZ = floor(mz*fRound+0.5)/fRound;

  ofstream fout;
  fout.open(m_strPathIds->c_str(),ios::out | ios::app);
  fout<<"-"<<endl;
  fout<<newX<<" "<<newY<<" "<<newZ<<endl;
  fout<<newRadius<<endl;
  fout.close();
  m_vpSphere = new CvpDivSphere( newX, newY, newZ, newRadius, m_fldDivision.getValue(),1);

  QuickSort( m_vpSphere->vPoints );
}

void SoVisibilitySolver::_analyzeCurrentViewpoint ( )
{
  float* fTempBufferPosition;
  m_fBufferPosition = m_fAllFrameBuffers;
  vpListType1 *m_vpList = new vpListType1[m_iNrOfObjects];
  int i;
  int xSize = m_soOff->getSizeX(); int ySize= m_soOff->getSizeY();
  for (int iIter = 0; iIter < xSize*ySize; iIter++) //alle pixel durchgehen
  {
    if (m_fBufferPosition[iIter] != 1)                        //wenn Information in gemeinsamen Buffer
    {
      fTempBufferPosition = &m_fBufferPosition[iIter];
      for (i = 0; i < m_iNrOfObjects; i++)
      {
        fTempBufferPosition+=xSize*ySize;
        m_vpList[i].Id     = i;
        m_vpList[i].zValue = (*fTempBufferPosition);
      }
      QuickSort(m_vpList, m_iNrOfObjects);
      int iPos1, iPos2;
      iPos1 = 0;
      while (m_vpList[iPos1].zValue < 1)
      {
        m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1].Id][m_vpList[iPos1].Id]++;

         //Visible Pixel
        if (iPos1 == 0) m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1].Id][m_iNrOfObjects]++;
        
        //MinDistance
        if (m_vpList[iPos1].zValue < m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1].Id][m_iNrOfObjects+PARAM_MINDIST])
        {
          m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1].Id][m_iNrOfObjects+PARAM_MINDIST] = m_vpList[iPos1].zValue;
        }

        //MaxDistance
        if (m_vpList[iPos1].zValue > m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1].Id][m_iNrOfObjects+PARAM_MAXDIST])
        {
          m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1].Id][m_iNrOfObjects+PARAM_MAXDIST] = m_vpList[iPos1].zValue;
        }



        iPos2 = iPos1 + 1;
        if (iPos2 != m_iNrOfObjects)
        {
          while (m_vpList[iPos2].zValue < 1)
          {
            m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos2].Id][m_vpList[iPos1].Id]++;
            iPos2++;
            if (iPos2 == m_iNrOfObjects) break;
          }
        }
        iPos1++;
        if (iPos1 == m_iNrOfObjects) break;
      }

      //std::cout << "entropy pixel:" << m_fResultMatrix[m_iCurrentViewpoint][m_vpList[iPos1-1].Id][m_iNrOfObjects] << std::endl;

      //Sillhoutten-Check mit 4er-Nachbarschaft UND BildraumCenter
      fTempBufferPosition = &m_fBufferPosition[iIter];
      bool isSilhouettePixel = false;
      double xPos, yPos;
      double pixelValue;

      for (i = 0; i < m_iNrOfObjects; i++)
      {
        isSilhouettePixel = false;
        //liegt der erste ObjectBuffer erst an 2. stelle und kommt als 1. der gesamtbuffer?
        //^^Ja
        fTempBufferPosition+=xSize*ySize;

        if ((*(fTempBufferPosition-1)<1)) //Nur wenn Pixel überhaupt Objektpixel ist
        {
          //wenn kein linker Rand
          if (iIter%(xSize)!=0 && iIter!=0)
          {
            if (*(fTempBufferPosition-1)==1)
            {
              isSilhouettePixel = true;
            }
          }

          //wenn kein rechter Rand
          if ((iIter+1)%xSize!=0)
          {
            if (*(fTempBufferPosition+1)==1)
            {
              isSilhouettePixel = true;
            }
          }

          //wenn kein oberer Rand
          if (iIter>=xSize)
          {
            if (*(fTempBufferPosition-xSize)==1)
            {
              isSilhouettePixel = true;
            }
          }

          //wenn kein unterer Rand
          if (iIter<(xSize*(ySize-1)))
          {
            if (*(fTempBufferPosition+xSize)==1)
            {
              isSilhouettePixel = true;
            }
          }

          if (isSilhouettePixel)
          {
            m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_SILHOUETTE]++;
          }

          
          //Bildraum-Center-Wert für Pixel berechnen
          yPos = std::floor(iIter/xSize);
          xPos = iIter - (yPos*xSize);
          
          xPos = std::abs( xPos - (xSize/2) );
          yPos = std::abs( yPos - (ySize/2) );

          pixelValue = 1.0 -  ( std::sqrt(xPos*xPos+yPos*yPos) / std::sqrt(xSize*xSize/4 + ySize*ySize/4) );

          m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_CENTERVALUE]+=pixelValue;
        }
      }
    }
  }

  for (i=0; i < m_iNrOfObjects; i++)
  {
    //Normaliesieren durch Gesamtanzahl Pixel des Objektes
    if  (m_fResultMatrix[m_iCurrentViewpoint][i][i]!=0)
    {
      m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_CENTERVALUE] = m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_CENTERVALUE] / m_fResultMatrix[m_iCurrentViewpoint][i][i];
    }
        
    /*std::cout << "objID:" << i << " centerValue:" << m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_CENTERVALUE];
    std::cout << "Silh: " <<  m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_SILHOUETTE];
    std::cout << "visPix: " <<  m_fResultMatrix[m_iCurrentViewpoint][i][m_iNrOfObjects+PARAM_VISIBLEPIXEL];
    std::cout << "totalPix: " <<  m_fResultMatrix[m_iCurrentViewpoint][i][i];
    std::cout << std::endl;*/
  }
  delete[] m_vpList;
}


void SoVisibilitySolver::_saveResultMatrix (void)
{
  FILE * pFile;
  int iX = m_soOff->getSizeX();
  int iY = m_soOff->getSizeY();
  int iDiv = m_fldDivision.getValue();
  char iVersion1 = 'V'; char iVersion2 = '0'; char iVersion3 = '3';
  pFile = fopen (m_strPathVis->c_str(),"wb");
  fwrite(&iX,sizeof(int),1,pFile);
  fwrite(&iY,sizeof(int),1,pFile);
  fwrite(&iDiv,sizeof(int),1,pFile);
  fwrite(&m_iNrOfObjects,sizeof(int),1,pFile);
  
  //write the Version; Unterscheidung zu alten Version ob da oder nicht
  fwrite(&iVersion1,sizeof(char),1,pFile);
  fwrite(&iVersion2,sizeof(char),1,pFile);
  fwrite(&iVersion3,sizeof(char),1,pFile);
  
  for (unsigned int i = 0; i< m_vpSphere->vPoints.size(); i++)
  {
    for (int j = 0; j< m_iNrOfObjects;j++)
    {
      fwrite(m_fResultMatrix[i][j],sizeof(float),m_iNrOfObjects+PARAM_COUNT,pFile);
    }
  }
  fclose (pFile);
}

void SoVisibilitySolver::GLRenderBelowPath(SoGLRenderAction *action)
{
  if ((m_flHasSphere)&&(m_fResultMatrix!=NULL))
  {
    int iObj = m_fldCurrentObj.getValue();
    if ( iObj<0 )               iObj = 0;
    if ( iObj>=m_iNrOfObjects ) iObj = m_iNrOfObjects - 1;
    _paintAllWhiteAneOneRed(iObj);
    float fColor;
    glDisable(GL_LIGHTING);
    for (int i=0; (unsigned int)i< m_vpSphere->vTriangles.size(); i++)
    {
      int iWrong = m_vpSphere->vTriangles[i]->getWrongEdge();
      CvpPoint *oP1,*oP2,*oP3;
      if (iWrong == 1) oP1 = m_vpSphere->vTriangles[i]->getEdge1()->getPoint2();
      else oP1 = m_vpSphere->vTriangles[i]->getEdge1()->getPoint1();
      if (iWrong == 2) oP2 = m_vpSphere->vTriangles[i]->getEdge2()->getPoint2();
      else oP2 = m_vpSphere->vTriangles[i]->getEdge2()->getPoint1();
      if (iWrong == 3) oP3 = m_vpSphere->vTriangles[i]->getEdge3()->getPoint2();
      else oP3 = m_vpSphere->vTriangles[i]->getEdge3()->getPoint1();
      if (m_fldSphereMode.getValue() == 0)
      {
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);
        fColor = m_fResultMatrix[oP1->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
            m_fResultMatrix[oP1->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP1->x,oP1->y,oP1->z);
        fColor = m_fResultMatrix[oP2->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
            m_fResultMatrix[oP2->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP2->x,oP2->y,oP2->z);
        fColor = m_fResultMatrix[oP3->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
              m_fResultMatrix[oP3->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP3->x,oP3->y,oP3->z);
        fColor = m_fResultMatrix[oP1->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
            m_fResultMatrix[oP1->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP1->x,oP1->y,oP1->z);
        glEnd();
        glLineWidth(1.0);
      }
      if (m_fldSphereMode.getValue() == 1)
      {
        glBegin(GL_POLYGON);
        fColor = m_fResultMatrix[oP1->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
            m_fResultMatrix[oP1->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP1->x,oP1->y,oP1->z);
        fColor = m_fResultMatrix[oP2->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
            m_fResultMatrix[oP2->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP2->x,oP2->y,oP2->z);
        fColor = m_fResultMatrix[oP3->id][m_fldCurrentObj.getValue()][m_iNrOfObjects] /
              m_fResultMatrix[oP3->id][m_fldCurrentObj.getValue()][m_fldCurrentObj.getValue()];
        glColor3f(fColor,fColor,fColor);
        glVertex3f(oP3->x,oP3->y,oP3->z);
        glEnd();
      }
    }
    glEnable(GL_LIGHTING);
  }
  SoSeparator::GLRenderBelowPath(action);
}


void SoVisibilitySolver::_paintAllWhiteAneOneRed (int iObjNr)
{
  SoMaterial* soMat;
  for (int i = 0; i < m_iNrOfObjects; i++)
  {
    soMat = (SoMaterial*)m_soSwitch->getChild(i*2);
    if (i==iObjNr) soMat->diffuseColor.setValue(1,0,0); else soMat->diffuseColor.setValue(1,1,1);
  }
}

void SoVisibilitySolver::logResults (void)
{
  int i,j,k;
  ofstream fout;
  fout.open("c:/result1.txt",ios::out);
  fout<<"Spherepoints:"<<endl;
  fout<<"-------------"<<endl;

  for (i = 0; (unsigned int)i<m_vpSphere->vPoints.size(); i++)
  {
    fout<<i<<": ("<<m_vpSphere->vPoints[i]->x<<","<<m_vpSphere->vPoints[i]->y<<","<<m_vpSphere->vPoints[i]->z<<") : "<<m_vpSphere->vPoints[i]->id<<endl;
  }
  fout<<"ResultMatrix:"<<endl;
  fout<<"-------------"<<endl;

  for (i = 0; (unsigned int)i<m_vpSphere->vPoints.size(); i++)
  {
    fout<<"Viewpoint: "<<i<<endl;
    fout<<".......... "<<endl;
    for (j = 0; j<m_iNrOfObjects; j++)
    {
      for (k = 0; k<m_iNrOfObjects + PARAM_COUNT; k++)
      {
        fout<<m_fResultMatrix[i][j][k]<<"_";
      }
      fout<<endl;
    }
  }
  fout<<"Visibility/Visible Pixel/ Total Pixel/Silhouette Pixel for Object 0"<<endl;
  fout<<"-----------------------"<<endl;
  for (i = 0; (unsigned int)i<m_vpSphere->vPoints.size(); i++)
  {
    fout<<i<<": "<<   m_fResultMatrix[i][0][m_iNrOfObjects] /  m_fResultMatrix[i][0][0] << " / "
           << m_fResultMatrix[i][0][m_iNrOfObjects] << " / "
           << m_fResultMatrix[i][0][0] << " / "
           << m_fResultMatrix[i][0][m_iNrOfObjects+PARAM_SILHOUETTE] << endl;
  }
  fout.close();
}
