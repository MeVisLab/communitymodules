#include "vpCalcVis.h"
#include <stdio.h>
#include <fstream>
#include "vpDivSphere.h"
#include "vpAlgorithms.h"
#include "vpField.h"
#include <math.h>
#include "kDebug.h"
#include "kBasics.h"

#ifndef min
#define min(a,b) a>b?b:a
#endif
#ifndef max
#define max(a,b) a<b?b:a
#endif
#ifndef sign
#define sign(a) a<0?-1:1
#endif


CvpCalcVis::CvpCalcVis()
{
  m_flHasData=false;
  m_iCurrentId = -1;
  m_iStackSize = 0;
  m_oVisField = NULL;
  m_oImpField = NULL;
  m_oNumField = NULL;
  m_oEntField = NULL;
  m_oDisField = NULL;
  m_oCamField = NULL;
  m_oRegField = NULL;
  m_oVisStaField = NULL;
  m_oImpStaField = NULL;
  m_oSilField = NULL;
  m_oCenterField = NULL;

  m_oTempField = NULL;
  m_oTempField2 = NULL;

  m_oFieldStack = NULL;
  m_oObjectImportance = NULL;
  m_oObjectStatus = NULL;
  m_iNrOfObjects = 0;
  m_fCamX = m_fCamY = m_fCamZ = 0;
  m_fVisSta = m_fImpSta = 0;

  m_fCamRange = 1;

  m_fVisW = m_fImpW = m_fNumW = m_fEntW = m_fDisW = m_fCamW = m_fRegW = m_fVisStaW = m_fImpStaW = m_fSilW = m_fCenterW = 1.0;

  //useExternalPrefReg = false;
  prefRegionVectorX = 0; prefRegionVectorY = 0; prefRegionVectorZ = 1;
  prefRegionRange = 1;
  restrictToRegion = false;
  prefRegionType = PR_VECTOR;
  _apexAngle=20;
}


CvpCalcVis::~CvpCalcVis()
{
  if ( m_oImpField != NULL ) delete m_oImpField;
  if ( m_oVisField != NULL ) delete m_oVisField;
  if ( m_oNumField != NULL ) delete m_oNumField;
  if ( m_oEntField != NULL ) delete m_oEntField;
  if ( m_oDisField != NULL ) delete m_oDisField;
  if ( m_oCamField != NULL ) delete m_oCamField;
  if ( m_oVisStaField != NULL ) delete m_oVisStaField;
  if ( m_oImpStaField != NULL ) delete m_oImpStaField;
  if ( m_oSilField != NULL ) delete m_oSilField;
  if ( m_oCenterField != NULL ) delete m_oCenterField;

  if (m_oFieldStack != NULL) delete[] m_oFieldStack;

  if ( m_oTempField != NULL ) delete m_oTempField;
  if ( m_oTempField2 != NULL ) delete m_oTempField2;

  if ( m_oObjectStatus !=NULL ) delete m_oObjectStatus;
  if ( m_oObjectImportance != NULL ) delete m_oObjectImportance;
}


bool CvpCalcVis::setData (string cPath)
{
  string pathVis(cPath);
  pathVis.append("camera_data.dat");
  string pathIds(cPath);
  pathIds.append("camera_ids.dat");

  _loadIds(pathIds.c_str());

  FILE * pFile;
  pFile = fopen (pathVis.c_str(),"rb");
  fread(&m_iViewportX,sizeof(int),1,pFile);
  fread(&m_iViewportY,sizeof(int),1,pFile);
  fread(&m_iDiv,sizeof(int),1,pFile);
  fread(&m_iNrOfObjects,sizeof(int),1,pFile);
  fread(&m_cVersionChar[0],sizeof(char),1,pFile);
  fread(&m_cVersionChar[1],sizeof(char),1,pFile);
  fread(&m_cVersionChar[2],sizeof(char),1,pFile);

  if (m_cVersionChar[0]!='V' || m_cVersionChar[1]!='0' || m_cVersionChar[2]!='3')
  {
    cout << "wrong version of viewpoint data (without silhouette and imageSpaceCenter information) ... please re-generate viewpoint data for this dataset" << endl;
    return false;
  }

  m_oSphere = new CvpDivSphere( m_fSphereX, m_fSphereY, m_fSphereZ, m_fSphereRadius, m_iDiv, 1 );
  std::cout << "sphere data: x:" << m_fSphereX << "  y:" << m_fSphereY << "  z:" << m_fSphereZ << "  radius:" << m_fSphereRadius << "   div:" << m_iDiv << std::endl;
  QuickSort(m_oSphere->vPoints);

  m_fResultMatrix = new float**[m_oSphere->vPoints.size()];
  for ( unsigned int ri1 = 0; ri1 < m_oSphere->vPoints.size(); ri1++)
  {
    m_fResultMatrix[ri1] = new float*[m_iNrOfObjects];
    for (int ri2 = 0; ri2 < m_iNrOfObjects; ri2++)
    {
      m_fResultMatrix[ri1][ri2] = new float[m_iNrOfObjects+PARAM_COUNT];
    }
  }

  unsigned int i = 0;
  for ( i = 0; i< m_oSphere->vPoints.size(); i++)
  {
    for ( int j = 0; j< m_iNrOfObjects;j++)
    {
      fread(m_fResultMatrix[i][j],sizeof(float),m_iNrOfObjects+PARAM_COUNT,pFile);
    }
  }
  fclose (pFile);

  m_oObjectImportance = new CvpField( m_iNrOfObjects );
  m_oObjectStatus     = new CvpField( m_iNrOfObjects );
  for ( i = 0; (int)i < m_iNrOfObjects; i++ ) m_oObjectStatus->setValueAt( i, 0 );

  m_oVisField = new CvpField(m_oSphere->vPoints.size());
  m_oImpField = new CvpField(m_oSphere->vPoints.size());
  m_oNumField = new CvpField(m_oSphere->vPoints.size());
  m_oEntField = new CvpField(m_oSphere->vPoints.size());
  m_oDisField = new CvpField(m_oSphere->vPoints.size());
  m_oCamField = new CvpField(m_oSphere->vPoints.size());
  m_oVisStaField = new CvpField(m_oSphere->vPoints.size());
  m_oImpStaField = new CvpField(m_oSphere->vPoints.size());
  m_oSilField = new CvpField(m_oSphere->vPoints.size());
  m_oCenterField = new CvpField(m_oSphere->vPoints.size());
  m_oSumField = new CvpField(m_oSphere->vPoints.size());

  m_oTempField = new CvpField(m_oSphere->vPoints.size());
  m_oTempField2 = new CvpField(m_oSphere->vPoints.size());

  m_flHasData=true;
  return m_flHasData;
}


void CvpCalcVis::_loadIds(const char* cPath)
{
  ifstream fin;
  char cTemp[255];
  fin.open(cPath, ios::in);
  bool end=false;
  m_vStructures.clear();
  while(!end)
  {
    fin>>cTemp;
    if (cTemp[0] == '-') end=true;
    else
    {
      fin>>cTemp;
      m_vStructures.push_back(new string(cTemp));
    }
  }
  fin>>m_fSphereX>>m_fSphereY>>m_fSphereZ>>m_fSphereRadius;
  fin.close();
}


int CvpCalcVis::getSphereValues (float &fX,float &fY,float &fZ,float &fR,int &iDiv) const
{
  fX = m_fSphereX;
  fY = m_fSphereY;
  fZ = m_fSphereZ;
  fR = m_fSphereRadius;
  iDiv = m_iDiv;
  return 1;
}


int CvpCalcVis::findObjectId (string sStruct)
{
  for ( unsigned int i=0; i<m_vStructures.size(); i++)
  {
    if ((*m_vStructures[i]).compare(sStruct) == 0) return i;
  }
  return -1;
}

string CvpCalcVis::getObjectName (int iID)
{
  if (iID>0 && (unsigned int)iID<m_vStructures.size())
    return (*m_vStructures[iID]);
  return "";
}

string CvpCalcVis::getFocusObject(){
  return getObjectName(m_iCurrentId);
}


int CvpCalcVis::setFocusObject (string cName)
{
  m_iCurrentId = findObjectId(cName);
  if (m_iCurrentId == -1) return 0;
  return 1;
}

int CvpCalcVis::setCamPos ( float fX, float fY, float fZ )
{
  m_fCamX = fX;
  m_fCamY = fY;
  m_fCamZ = fZ;
  return 1;
}

int CvpCalcVis::setCamRange ( float fR )
{
  m_fCamRange = fR;
  return 1;
}

int CvpCalcVis::setVisStability ( float fVisSta )
{
  m_fVisSta = fVisSta;
  return 1;
}

int CvpCalcVis::setImpStability ( float fImpSta )
{
  m_fImpSta = fImpSta;
  return 1;
}

void CvpCalcVis::_calcVisField (void)
{
  if (!m_flHasData) return;
  for (int i=0; i<m_oVisField->getSize(); i++)
  {
    m_oVisField->setValueAt(i, m_fResultMatrix[i][m_iCurrentId][m_iNrOfObjects] /
                               m_fResultMatrix[i][m_iCurrentId][m_iCurrentId]);
  }
}

void CvpCalcVis::_calcImpField (void)
{
  if (!m_flHasData) return;
  float fSum;
  for (int i=0; i<m_oImpField->getSize(); i++)
  {
    fSum = 0.0;
    for (int j=0; j<m_iNrOfObjects; j++)
    {
      fSum+=m_oObjectStatus->getValueAt(j) *
            m_oObjectImportance->getValueAt(j)*( m_fResultMatrix[i][m_iCurrentId][j] /
                                                 m_fResultMatrix[i][m_iCurrentId][m_iCurrentId] );
    }
    m_oImpField->setValueAt( i, fSum );
  }
  m_oImpField->normalize();
  m_oImpField->invert();
}

void CvpCalcVis::_calcNumField (void)
{
  if (!m_flHasData) return;
  int iSum;
  float fVisibility;
  for (int i=0; i<m_oNumField->getSize(); i++)
  {
    iSum = 0;
    for (int j=0; j<m_iNrOfObjects; j++)
    {
      if ( m_fResultMatrix[i][m_iCurrentId][j] != 0 && j != m_iCurrentId && m_oObjectStatus->getValueAt(j) != 0 ) iSum++;
    }
    fVisibility = 1 - (m_fResultMatrix[i][m_iCurrentId][m_iNrOfObjects] /
                       m_fResultMatrix[i][m_iCurrentId][m_iCurrentId] );
    m_oNumField->setValueAt( i, (float) iSum );
  }
  m_oNumField->normalize();
  m_oNumField->invert();
}

void CvpCalcVis::_calcEntField (void)
{
  if (!m_flHasData) return;
  for (int i=0; i<m_oEntField->getSize(); i++)
  {
    m_oEntField->setValueAt( i, m_fResultMatrix[i][m_iCurrentId][m_iCurrentId] );
  }
  m_oEntField->normalize();
}

void CvpCalcVis::_calcDisField (void)
{
  if (!m_flHasData) return;
  for (int i=0; i<m_oDisField->getSize(); i++)
  {
    m_oDisField->setValueAt( i, m_fResultMatrix[i][m_iCurrentId][m_iNrOfObjects+PARAM_MINDIST] );
  }
  m_oDisField->normalize();
  m_oDisField->invert();
}

void CvpCalcVis::_calcCamField (void)
{
  if (!m_flHasData) return;
  _calcPointRegion( m_oCamField, m_fCamX, m_fCamY, m_fCamZ, m_fCamRange );
}


void CvpCalcVis::_calcPrefRegionField (void)
{
    if (!m_flHasData) return;
  m_oRegField->fillWith(0);
  if (prefRegionType==PR_VECTOR)
  {
    //Vector-Region
    addVectorRegionToStackField(0,prefRegionVectorX,prefRegionVectorY,prefRegionVectorZ,(float)prefRegionRange);
  }
  else
  {
    addPointRegionToStackField(0,
                   (prefRegionVectorX*m_oSphere->fRadius) + m_oSphere->fMx,
                   (prefRegionVectorY*m_oSphere->fRadius) + m_oSphere->fMy,
                   (prefRegionVectorZ*m_oSphere->fRadius) + m_oSphere->fMz,
                   (float)prefRegionRange);
  }
  setStackFieldAsRegionField(0);
  m_oRegField->normalize();
}

void CvpCalcVis::_calcSilField (void)
{
  if (!m_flHasData) return;
  for (int i=0; i<m_oSilField->getSize(); i++)
  {
    m_oSilField->setValueAt(i, m_fResultMatrix[i][m_iCurrentId][m_iNrOfObjects+PARAM_SILHOUETTE]);
  }
  m_oSilField->normalize();
}

void CvpCalcVis::_calcCenterField (void)
{
  if (!m_flHasData) return;
  for (int i=0; i<m_oCenterField->getSize(); i++)
  {
    m_oCenterField->setValueAt(i, m_fResultMatrix[i][m_iCurrentId][m_iNrOfObjects+PARAM_CENTERVALUE]);
  }
  m_oCenterField->normalize();
}


void CvpCalcVis::_calcSum (void)
{
  if (!m_flHasData) return;
  int i;
  m_oSumField->fillWith(0.0);
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oVisField->getValueAt(i) * m_fVisW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oImpField->getValueAt(i) * m_fImpW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oNumField->getValueAt(i) * m_fNumW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oEntField->getValueAt(i) * m_fEntW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oDisField->getValueAt(i) * m_fDisW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oCamField->getValueAt(i) * m_fCamW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oVisStaField->getValueAt(i) * m_fVisStaW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oImpStaField->getValueAt(i) * m_fImpStaW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oSilField->getValueAt(i) * m_fSilW );
  m_oSumField->add((*m_oTempField));
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oCenterField->getValueAt(i) * m_fCenterW );
  m_oSumField->add((*m_oTempField));

  //PrefReg für normale Wichtung
  for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oRegField->getValueAt(i) * m_fRegW );
  m_oSumField->add((*m_oTempField));

  //PrefReg für -zusätzlich- Restrict
  if (restrictToRegion)
  {
    for (i=0; i<m_oTempField->getSize(); i++) m_oTempField->setValueAt( i, m_oRegField->getValueAt(i));
    m_oTempField->binarize(0.1f);
    m_oSumField->multiply((*m_oTempField));
  }
  kDebug::Debug("min1: "+kBasics::FloatToString(m_oSumField->getMinValue()) ,kDebug::DL_HIGH);
  kDebug::Debug("max1: "+kBasics::FloatToString(m_oSumField->getMaxValue()) ,kDebug::DL_HIGH);
  m_oSumField->normalize();
  kDebug::Debug("min2: "+kBasics::FloatToString(m_oSumField->getMinValue()) ,kDebug::DL_HIGH);
  kDebug::Debug("max2: "+kBasics::FloatToString(m_oSumField->getMaxValue()) ,kDebug::DL_HIGH);
}


CvpField* CvpCalcVis::getField (int iField)
{
  if (iField == VIS_FIELD) return m_oVisField;
  if (iField == IMP_FIELD) return m_oImpField;
  if (iField == NUM_FIELD) return m_oNumField;
  if (iField == ENT_FIELD) return m_oEntField;
  if (iField == DIS_FIELD) return m_oDisField;
  if (iField == CAM_FIELD) return m_oCamField;
  if (iField == REG_FIELD) return m_oRegField;
  if (iField == STA_VIS_FIELD) return m_oVisStaField;
  if (iField == STA_IMP_FIELD) return m_oImpStaField;
  if (iField == SIL_FIELD) return m_oSilField;
  if (iField == CENTER_FIELD) return m_oCenterField;
  if (iField == SUM_FIELD) return m_oSumField;
  return NULL;
}


int CvpCalcVis::calc (void)
{
  if ( !m_flHasData || m_iCurrentId == -1 ) return 0;

  _calcVisField();
  _calcImpField();
  _calcNumField();
  _calcEntField();
  _calcDisField();
  _calcCamField();
  _calcStaField(VIS_FIELD);
  _calcStaField(IMP_FIELD);
  _calcSilField();
  _calcCenterField();
  _calcPrefRegionField();
  _calcSum();
  return 1;
}

void CvpCalcVis::setStackSize (unsigned int iSize)
{
  if (m_oFieldStack != NULL) delete[] m_oFieldStack;
  m_iStackSize = iSize;
  m_oFieldStack = new CvpField[iSize];
}

int CvpCalcVis::copyFieldToStack (int iFieldType, unsigned int iStackPos)
{
  if ( iFieldType == VIS_FIELD && m_oVisField == NULL ) return 0;
  if ( iFieldType == IMP_FIELD && m_oImpField == NULL ) return 0;
  if ( iFieldType == DIS_FIELD && m_oDisField == NULL ) return 0;
  if ( iFieldType == CAM_FIELD && m_oCamField == NULL ) return 0;
  if ( iFieldType == ENT_FIELD && m_oEntField == NULL ) return 0;
  if ( iFieldType == NUM_FIELD && m_oNumField == NULL ) return 0;
  if ( iFieldType == REG_FIELD && m_oRegField == NULL ) return 0;
  if ( iFieldType == STA_VIS_FIELD && m_oVisStaField == NULL ) return 0;
  if ( iFieldType == STA_IMP_FIELD && m_oImpStaField == NULL ) return 0;
  if ( iFieldType == SIL_FIELD && m_oSilField == NULL ) return 0;
  if ( iFieldType == CENTER_FIELD && m_oCenterField == NULL ) return 0;
  if ( iFieldType == SUM_FIELD && m_oSumField == NULL ) return 0;

  if ( !m_flHasData ) return 0;
  if ( iStackPos >= m_iStackSize ) return 0;
  if ( !m_oFieldStack[iStackPos].isValid() ) m_oFieldStack[iStackPos].setSize( m_oSphere->vPoints.size() );
  for ( unsigned int i = 0; i < m_oSphere->vPoints.size(); i++ )
  {
    if ( iFieldType == VIS_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oVisField->getValueAt( i ) );
    if ( iFieldType == IMP_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oImpField->getValueAt( i ) );
    if ( iFieldType == DIS_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oDisField->getValueAt( i ) );
    if ( iFieldType == CAM_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oCamField->getValueAt( i ) );
    if ( iFieldType == ENT_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oEntField->getValueAt( i ) );
    if ( iFieldType == NUM_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oNumField->getValueAt( i ) );
    if ( iFieldType == REG_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oNumField->getValueAt( i ) );
    if ( iFieldType == STA_VIS_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oVisStaField->getValueAt( i ) );
    if ( iFieldType == STA_IMP_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oImpStaField->getValueAt( i ) );
  if ( iFieldType == SIL_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oSilField->getValueAt( i ) );
  if ( iFieldType == CENTER_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oCenterField->getValueAt( i ) );
    if ( iFieldType == SUM_FIELD ) m_oFieldStack[iStackPos].setValueAt (i, m_oSumField->getValueAt( i ) );
  }
  return 1;
}


int CvpCalcVis::copyStackFieldToStack ( unsigned int iFromStackPos, unsigned int iToStackPos )
{
  if ( !m_oFieldStack[iFromStackPos].isValid() ) return 0;
  if ( !m_flHasData ) return 0;
  if ( iFromStackPos >= m_iStackSize || iToStackPos >= m_iStackSize ) return 0;
  if ( !m_oFieldStack[iToStackPos].isValid() ) m_oFieldStack[iToStackPos].setSize( m_oSphere->vPoints.size() );
  for ( unsigned int i = 0; i < m_oSphere->vPoints.size(); i++ )
  {
    m_oFieldStack[iToStackPos].setValueAt (i, m_oFieldStack[iFromStackPos].getValueAt( i ) );
  }
  return 1;
}

int CvpCalcVis::multiplyStackFields ( unsigned int iStackPos1, unsigned int iStackPos2, unsigned int iStackPosResult )
{
  if ( iStackPos1 == iStackPos2 && iStackPos2 == iStackPosResult ) return 0;
  if ( !m_oFieldStack[iStackPos1].isValid() || !m_oFieldStack[iStackPos2].isValid() ) return 0;
  if ( !m_flHasData ) return 0;
  if ( iStackPos1 >= m_iStackSize || iStackPos2 >= m_iStackSize || iStackPosResult >= m_iStackSize  ) return 0;
  if ( !m_oFieldStack[iStackPosResult].isValid() ) m_oFieldStack[iStackPosResult].setSize( m_oSphere->vPoints.size() );
  if ( iStackPos1 != iStackPosResult && iStackPos2 != iStackPosResult ) copyStackFieldToStack( iStackPos1, iStackPosResult );
        if ( iStackPos1 == iStackPosResult ) m_oFieldStack[iStackPos1].multiply( m_oFieldStack[iStackPos2] );
  else  if ( iStackPos2 == iStackPosResult ) m_oFieldStack[iStackPos2].multiply( m_oFieldStack[iStackPos1] );
  else                                       m_oFieldStack[iStackPosResult].multiply ( m_oFieldStack[iStackPos2] );
  return 1;
}

int CvpCalcVis::multiplyStackField ( unsigned int iStackPos1, double factor )
{
  if ( !m_oFieldStack[iStackPos1].isValid() ) return 0;
  if ( !m_flHasData ) return 0;
  if ( iStackPos1 >= m_iStackSize ) return 0;

  m_oFieldStack[iStackPos1].multiply( (float)factor );

  return 1;
}


int CvpCalcVis::addStackFields ( unsigned int iStackPos1, unsigned int iStackPos2, unsigned int iStackPosResult )
{
  if ( iStackPos1 == iStackPos2 && iStackPos2 == iStackPosResult ) return 0;
  if ( !m_oFieldStack[iStackPos1].isValid() || !m_oFieldStack[iStackPos2].isValid() ) return 0;
  if ( !m_flHasData ) return 0;
  if ( iStackPos1 >= m_iStackSize || iStackPos2 >= m_iStackSize || iStackPosResult >= m_iStackSize  ) return 0;
  if ( !m_oFieldStack[iStackPosResult].isValid() ) m_oFieldStack[iStackPosResult].setSize( m_oSphere->vPoints.size() );
  if ( iStackPos1 != iStackPosResult && iStackPos2 != iStackPosResult ) copyStackFieldToStack( iStackPos1, iStackPosResult );
  if ( iStackPos1 == iStackPosResult ) m_oFieldStack[iStackPos1].add( m_oFieldStack[iStackPos2] );
  else  if ( iStackPos2 == iStackPosResult ) m_oFieldStack[iStackPos2].add( m_oFieldStack[iStackPos1] );
  else                                       m_oFieldStack[iStackPosResult].add( m_oFieldStack[iStackPos2] );
  return 1;
}

CvpField* CvpCalcVis::getStackField ( unsigned int iStackPos )
{
  if ( iStackPos >= m_iStackSize ) return NULL;
  return &m_oFieldStack[iStackPos];
}

int CvpCalcVis::setStackFieldAsRegionField ( int iStackPos )
{
  if ( !m_flHasData ) return 0;
  m_oRegField = getStackField ( iStackPos );
  return 1;
}


int CvpCalcVis::addPointRegionToStackField ( int iStackPos, float fX, float fY, float fZ, float fRange )
{
  if ( !m_flHasData ) return 0;
  if ( ( unsigned int ) iStackPos >= m_iStackSize ) return 0;
  if ( !m_oFieldStack[iStackPos].isValid() ) m_oFieldStack[iStackPos].setSize( m_oSphere->vPoints.size() );
  _calcPointRegion( m_oTempField, fX, fY, fZ, fRange );
  m_oFieldStack[iStackPos].merge( (*m_oTempField) );
  return 1;
}

int CvpCalcVis::addVectorRegionToStackField ( int iStackPos, float fX, float fY, float fZ, float fRange )
{
  if ( !m_flHasData ) return 0;
  if ( ( unsigned int ) iStackPos >= m_iStackSize ) return 0;
  if ( !m_oFieldStack[iStackPos].isValid() ) m_oFieldStack[iStackPos].setSize( m_oSphere->vPoints.size() );
  _calcVecRegion( m_oTempField, fX, fY, fZ, fRange );
  m_oFieldStack[iStackPos].merge( (*m_oTempField) );
  return 1;

}

int CvpCalcVis::clearStack( int iStackPos )
{
  if ( !m_flHasData ) return 0;
  if ( ( unsigned int ) iStackPos >= m_iStackSize ) return 0;
  if ( !m_oFieldStack[iStackPos].isValid() ) m_oFieldStack[iStackPos].setSize( m_oSphere->vPoints.size() );
  m_oFieldStack[iStackPos].fillWith(0.0);
  return 1;
}


bool CvpCalcVis::hasData ( void )
{
  return m_flHasData;
}

int CvpCalcVis::getNrOfObjects ( void )
{
  return m_iNrOfObjects;
}

vector<string*> CvpCalcVis::getStructureNames ( void )
{
  return m_vStructures;
}


int CvpCalcVis::setImportance ( string sStruct, float fImportance )
{
  if ( !m_flHasData ) return 0;
  int iObjID = findObjectId (sStruct);
  if ( iObjID < 0 ) return 0;
  m_oObjectImportance->setValueAt( iObjID, fImportance );
  return 1;
}

int CvpCalcVis::setStatus ( string sStruct, bool flEnabled )
{
  if ( !m_flHasData ) return 0;
  int iObjID = findObjectId (sStruct);
  if ( iObjID < 0 ) return 0;
  m_oObjectStatus->setValueAt( iObjID, (float) flEnabled );
  return 1;

}

void CvpCalcVis::_scaleNormalizedField (CvpField *oField, float fRange)
{
  for (int i=0; i<oField->getSize(); i++)
  {
      oField->setValueAt( i, max( oField->getValueAt(i) - ( 1 - fRange ), 0.0f ) );
  }
  if ( fRange != 1 ) oField->normalize();
}

void CvpCalcVis::setApexAngle(int angle){
  _apexAngle=angle;
}

int CvpCalcVis::findNeighborPointDirection(int index,SbVec3f dir, const std::list<int>& visited, SbVec3f newPosition){
  float x,y,z;
  getSpherePos(index,x,y,z);
  SbVec3f actPos(x,y,z);
  _calcPointRegion(m_oTempField,x,y,z,1.0);
  _calcPointRegion(m_oTempField2,x,y,z,1.0);
  m_oTempField2->binarize(0.95);
  //double minang=180.0;
  double maxValue=0.0;
  int ret=-1;
  SbVec3f centerOfSphere(m_fSphereX,m_fSphereY,m_fSphereZ);
  for(int i=0;i<m_oTempField->getSize();i++){
    if(std::find(visited.begin(),visited.end(),i)!=visited.end()){
      continue;
    }
    if(m_oTempField2->getValueAt(i)<=0.0){
      m_oTempField->setValueAt(i,0.0);
    }else{
      if(dir.length()>0){
        SbVec3f actPoint;
        getSpherePos(i,actPoint[0],actPoint[1],actPoint[2]);
        SbVec3f actDir=actPoint - actPos;
        if(actDir.length()>0){
          double percentage=actDir.length()/dir.length();
          SbVec3f newDir=actPos+percentage*dir;
          SbVec3f centerToPoint=newDir-centerOfSphere;
          percentage=m_fSphereRadius/centerToPoint.length();
          SbVec3f pointOnSphere=centerOfSphere+percentage*centerToPoint;
          SbVec3f diffDir=pointOnSphere-actPos;
          double skalprod=(actDir[0]*diffDir[0]+actDir[1]*diffDir[1]+actDir[2]*diffDir[2])/(actDir.length()*diffDir.length());
          double ang=0.0;
          ang=acos(skalprod)/kBasics::PI*180;
          if(skalprod==0.0){
            ang=90.0;
          }
          if(ang<=_apexAngle){
            getStackField(4)->setValueAt(i,0.5);
            if(getStackField(1)->getValueAt(i)>maxValue){
            //if(getField(VIS_FIELD)->getValueAt(i)>maxValue){
          //    std::cout << "(" << dir[0] << ";" << dir[1] << ";" << dir[2] << ")" << std::endl;
          //    std::cout << "(" << actDir[0] << ";" << actDir[1] << ";" << actDir[2] << ")" << std::endl;
          //    std::cout << skalprod << "::" << ang << std::endl;
              maxValue=getStackField(1)->getValueAt(i);
              //maxValue=getField(VIS_FIELD)->getValueAt(i);
              ret=i;
            }
          }
        }
      }
    }
  }
  if(ret>=0){
    return ret;
  }
  m_oTempField->setValueAt(index,0.0);
  ret=m_oTempField->getMaxPos();
  if(ret==index){
    //std::cout << m_oTempField->getValueAt(ret);
    //std::cout << m_oTempField->toString() << std::endl;
  }
  return ret;
}

//Does this function get the nearest point on the bounding sphere related to a given point in space?
int CvpCalcVis::getFieldPos(const float &fX, const float &fY, const float &fZ){
  _calcPointRegion(m_oTempField,fX,fY,fZ,1.0);
  return m_oTempField->getMaxPos();
}

void CvpCalcVis::getSpherePos(int index, float &fX, float &fY, float &fZ){
  if(index<(int)m_oSphere->vPoints.size()){
    fX = m_oSphere->vPoints[index]->x;
      fY = m_oSphere->vPoints[index]->y;
    fZ = m_oSphere->vPoints[index]->z;
  }
}
void CvpCalcVis::_calcPointRegion ( CvpField* oField, float fX, float fY, float fZ, float fRange )
{
  float fEukDist,x,y,z;
  for (int i=0; i<oField->getSize(); i++)
  {
    x = m_oSphere->vPoints[i]->x;
    y = m_oSphere->vPoints[i]->y;
    z = m_oSphere->vPoints[i]->z;
    fEukDist = ( x - fX ) * ( x - fX ) +
               ( y - fY ) * ( y - fY ) +
               ( z - fZ ) * ( z - fZ );
    oField->setValueAt( i, fEukDist );
  }
  oField->normalize();
  oField->invert();
  if ( fRange != 1 ) _scaleNormalizedField( oField, fRange );
}

void CvpCalcVis::_calcVecRegion ( CvpField *oField, float fX, float fY, float fZ, float fRange )
{
  float fScalar,x,y,z;
  for (int i=0; i<oField->getSize(); i++)
  {
    x = m_oSphere->vPoints[i]->x - m_oSphere->fMx;
    y = m_oSphere->vPoints[i]->y - m_oSphere->fMy;
    z = m_oSphere->vPoints[i]->z - m_oSphere->fMz;
    fScalar = ( x*fX + y*fY + z*fZ ) / sqrt(x*x + y*y + z*z);
    oField->setValueAt( i, fabs(fScalar) );
  }
  oField->normalize();
  oField->invert();
  if ( fRange != 1 ) _scaleNormalizedField( oField, fRange );
}

void CvpCalcVis::_calcStaField (int iField)
{
  CvpField *oInField;
  float fThresh;
  int iNrNeigh;
  int iIndex1, iIndex2, iIndexStart;
  bool flDiffers;
  float fStartColor;

  if (iField == VIS_FIELD) { oInField = m_oVisField; fThresh = m_fVisSta; }
  if (iField == IMP_FIELD) { oInField = m_oImpField; fThresh = m_fImpSta; }

  m_oTempField2->fillWith(0);
   unsigned int i=0;
   for (i=0; (int)i < oInField->getSize(); i++)
  {
    m_oTempField->setValueAt( i, oInField->getValueAt( i ) );
  }
  m_oTempField->binarize(fThresh);
  for ( i=0; i < m_oSphere->vPoints.size(); i++)
  {
    iIndex1 = m_oSphere->vPoints[i]->id;
    if ( m_oTempField->getValueAt(iIndex1) == 0.0 )
    {
      iNrNeigh = m_oSphere->vPoints[i]->getNrOfNeighbours();
      iIndexStart = m_oSphere->vPoints[i]->getNeighbour(0)->id;
      fStartColor = m_oTempField->getValueAt(iIndexStart);
      flDiffers = false;
      for (int j=1; j < iNrNeigh; j++)
      {
        iIndex2 = m_oSphere->vPoints[i]->getNeighbour(j)->id;
        if ( m_oTempField->getValueAt(iIndex2) != fStartColor ) { flDiffers = true; break; }
      }
      if (flDiffers)
      {
        m_oTempField2->setValueAt(iIndex1,-1.0);
      }
    }
  }

  bool flEnd = false;
  float fVal = 1.0;
  int k,id;

  while (!flEnd)
  {
    flEnd=true;
    for (k = 0; k < m_oTempField2->getSize(); k++)
    {
      if (m_oTempField2->getValueAt(k) == -fVal)
      {
        flEnd = false;
        m_oTempField2->setValueAt(k,fVal-1.0f);
        iNrNeigh = m_oSphere->vPoints[k]->getNrOfNeighbours();
        for (int j=0; j<iNrNeigh;j++)
        {
          id = m_oSphere->vPoints[k]->getNeighbour(j)->id;
          if (m_oTempField->getValueAt(id) == 1.0f && m_oTempField2->getValueAt(id) == 0.0f) m_oTempField2->setValueAt(id,-(fVal+1.0f));
        }
      }
    }
    fVal+=1.0;
  }
  m_oTempField2->normalize();


  for ( i=0; (int)i < oInField->getSize(); i++)
  {
    if (iField == VIS_FIELD) m_oVisStaField->setValueAt( i, m_oTempField2->getValueAt( i ) );
    if (iField == IMP_FIELD) m_oImpStaField->setValueAt( i, m_oTempField2->getValueAt( i ) );

  }
}


int CvpCalcVis::setWeight ( int iFieldType, float fValue )
{
   if ( !m_flHasData ) return 0;
   if ( iFieldType == VIS_FIELD ) m_fVisW = fValue;
   if ( iFieldType == IMP_FIELD ) m_fImpW = fValue;
   if ( iFieldType == NUM_FIELD ) m_fNumW = fValue;
   if ( iFieldType == ENT_FIELD ) m_fEntW = fValue;
   if ( iFieldType == DIS_FIELD ) m_fDisW = fValue;
   if ( iFieldType == CAM_FIELD ) m_fCamW = fValue;
   if ( iFieldType == REG_FIELD ) m_fRegW = fValue;
   if ( iFieldType == STA_VIS_FIELD ) m_fVisStaW = fValue;
   if ( iFieldType == STA_IMP_FIELD ) m_fImpStaW = fValue;
   if ( iFieldType == SIL_FIELD ) m_fSilW = fValue;
   if ( iFieldType == CENTER_FIELD )  { m_fCenterW = fValue; std::cout << "m_fCenterW=" << m_fCenterW <<std::endl; }
   return 1;
}

float CvpCalcVis::getWeight ( int iFieldType )
{
   if ( !m_flHasData ) return 0;
   if ( iFieldType == VIS_FIELD ) return m_fVisW;
   if ( iFieldType == IMP_FIELD ) return m_fImpW;
   if ( iFieldType == NUM_FIELD ) return m_fNumW;
   if ( iFieldType == ENT_FIELD ) return m_fEntW;
   if ( iFieldType == DIS_FIELD ) return m_fDisW;
   if ( iFieldType == CAM_FIELD ) return m_fCamW;
   if ( iFieldType == REG_FIELD ) return m_fRegW;
   if ( iFieldType == STA_VIS_FIELD ) return m_fVisStaW;
   if ( iFieldType == STA_IMP_FIELD ) return m_fImpStaW;
   if ( iFieldType == SIL_FIELD ) return m_fSilW;
   if ( iFieldType == CENTER_FIELD ) return m_fCenterW;
   return 0;
}


void CvpCalcVis::setPrefRegionVector ( float fX, float fY, float fZ )
{
  prefRegionVectorX = fX;
  prefRegionVectorY = fY;
  prefRegionVectorZ = fZ;
  _calcPrefRegionField();
}

void CvpCalcVis::setPrefRegionRange ( double value )
{
  prefRegionRange = value;
  _calcPrefRegionField();
}

void CvpCalcVis::setRestrictToRegion ( bool value )
{
  restrictToRegion = value;
}

void CvpCalcVis::setPrefRegionType ( PrefRegionTypeEnum value )
{
  prefRegionType = value;
  _calcPrefRegionField();
}


//return cam id (iMax)
int CvpCalcVis::getFieldMaxPos ( int iFieldType, float &fX, float &fY, float &fZ )
{
  if ( !m_flHasData ) return -1;
  CvpField *oField;
  if ( iFieldType == VIS_FIELD ) oField = m_oVisField;
  if ( iFieldType == IMP_FIELD ) oField = m_oImpField;
  if ( iFieldType == NUM_FIELD ) oField = m_oNumField;
  if ( iFieldType == ENT_FIELD ) oField = m_oEntField;
  if ( iFieldType == DIS_FIELD ) oField = m_oDisField;
  if ( iFieldType == CAM_FIELD ) oField = m_oCamField;
  if ( iFieldType == REG_FIELD ) oField = m_oRegField;
  if ( iFieldType == STA_VIS_FIELD ) oField = m_oVisStaField;
  if ( iFieldType == STA_IMP_FIELD ) oField = m_oImpStaField;
  if ( iFieldType == SIL_FIELD ) oField = m_oSilField;
  if ( iFieldType == CENTER_FIELD ) oField = m_oCenterField;
  if ( iFieldType == SUM_FIELD ) oField = m_oSumField;
  if (oField == NULL) return -1;
  int iMax = oField->getMaxPos();
  fX = m_oSphere->vPoints[iMax]->x;
  fY = m_oSphere->vPoints[iMax]->y;
  fZ = m_oSphere->vPoints[iMax]->z;
  return iMax;
}

int CvpCalcVis::getStackMaxPos ( int iStackPos, float &fX, float &fY, float &fZ )
{
  if ( !m_flHasData ) return -1;
  if ( (unsigned int)iStackPos >= m_iStackSize ) return -1;
  if ( !m_oFieldStack[iStackPos].isValid() ) return -1;
  int iMax = m_oFieldStack[iStackPos].getMaxPos();
  fX = m_oSphere->vPoints[iMax]->x;
  fY = m_oSphere->vPoints[iMax]->y;
  fZ = m_oSphere->vPoints[iMax]->z;
  return iMax;
}


int CvpCalcVis::getMatrixValues ( int iCamID, string sStruct, std::vector<float>* vValues)
{
  if ( !m_flHasData ) return -1;

  vValues->clear();
  if (iCamID<0 || (unsigned int)iCamID>=m_oSphere->vPoints.size()) return -1;
  int iObjID = findObjectId (sStruct);
  if (iObjID==-1) return -1;
  for (int k = 0; k<m_iNrOfObjects + 3; k++)
  {
    vValues->push_back(float(m_fResultMatrix[iCamID][iObjID][k]));
  }
  return 0;
}


















void CvpCalcVis::logResults (void)
{
  int i,j,k;
  ofstream fout;
  fout.open("c:/calcVis.txt",ios::out);
  fout<<"Spherepoints:"<<endl;
  fout<<"-------------"<<endl;

  for (i = 0; (unsigned int)i<m_oSphere->vPoints.size(); i++)
  {
    fout<<i<<": ("<<m_oSphere->vPoints[i]->x<<","<<m_oSphere->vPoints[i]->y<<","<<m_oSphere->vPoints[i]->z<<") : "<<m_oSphere->vPoints[i]->id<<endl;
  }
  fout<<"ResultMatrix:"<<endl;
  fout<<"-------------"<<endl;

  for (i = 0; (unsigned int)i<m_oSphere->vPoints.size(); i++)
  {
    fout<<"Viewpoint: "<<i<<endl;
    fout<<".......... "<<endl;
    for (j = 0; j<m_iNrOfObjects; j++)
    {
      for (k = 0; k<m_iNrOfObjects + 3; k++)
      {
        fout<<m_fResultMatrix[i][j][k]<<"_";
      }
      fout<<endl;
    }
  }
  fout<<"Visibility for Object 0"<<endl;
  fout<<"-----------------------"<<endl;
  for (i = 0; (unsigned int)i<m_oSphere->vPoints.size(); i++)
  {
    fout<<i<<": "<<  m_oVisField->getValueAt(i) <<endl;
  }
  fout.close();
}


int CvpCalcVis::getDataViewportX(void)
{
  return m_iViewportX;
}


int CvpCalcVis::getDataViewportY(void)
{
  return m_iViewportY;
}

int CvpCalcVis::getDataNrOfObjects(void)
{
  return m_iNrOfObjects;
}


int CvpCalcVis::getDataDiv(void)
{
  return m_iDiv;
}

int CvpCalcVis::getDataCamCount(void)
{
  if (m_oSphere)
    return m_oSphere->vPoints.size();
  return -1;
}

string CvpCalcVis::getDataVersion(void)
{
  string s;
  s+=m_cVersionChar[0];
  s+=m_cVersionChar[1];
  s+=m_cVersionChar[2];
  return s;
}

