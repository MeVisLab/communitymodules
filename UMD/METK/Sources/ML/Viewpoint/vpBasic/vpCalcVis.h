#ifndef VP_CALC_VIS_H
#define VP_CALC_VIS_H

#ifdef WIN32
  #ifndef VPBASIC_EXPORTS
    #define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
  #else
    #define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
  #endif
#else
  #define __VPBASIC_IMPORTEXPORT
#endif

#pragma warning(disable:4786)
#pragma warning(disable: 4251)

#define VIS_FIELD 0
#define IMP_FIELD 1
#define NUM_FIELD 2
#define ENT_FIELD 3
#define DIS_FIELD 4
#define CAM_FIELD 5
#define REG_FIELD 6
#define STA_VIS_FIELD 7
#define STA_IMP_FIELD 8
#define SIL_FIELD 9
#define CENTER_FIELD 10
#define SUM_FIELD 11

//Nochmalige Definition der Parameter aus SoVisibilitySolver zwecks Unabhängigkeit
#define PARAM_VISIBLEPIXEL 0
#define PARAM_MINDIST 1
#define PARAM_MAXDIST 2
#define PARAM_SILHOUETTE 3
#define PARAM_CENTERVALUE 4
#define PARAM_COUNT 5

#include <vector>
#include <string>
#include <math.h>
#include "kCamera.h"

using namespace std;


class CvpDivSphere;
class CvpField;

class __VPBASIC_IMPORTEXPORT CvpCalcVis
{
  public:
    CvpCalcVis ();
    ~CvpCalcVis();
    bool setData (string cPath);
    int getSphereValues (float &fX,float &fY,float &fZ,float &fR,int &iDiv) const;
	int getFieldPos(const float &fX, const float &fY, const float &fZ);
	void getSpherePos(int index, float &fX, float &fY, float &fZ);
	int findNeighborPointDirection(int index,SbVec3f dir, const std::list<int>& visited, SbVec3f newPosition);
    int setFocusObject (string cName);
	string getFocusObject();
    CvpField* getField ( int iFieldType );
    void setStackSize (unsigned int iSize);
    int copyFieldToStack ( int iFieldType, unsigned int iStackPos );
    int copyStackFieldToStack ( unsigned int iFromStackPos, unsigned int iToStackPos );
    int multiplyStackFields ( unsigned int iStackPos1, unsigned int iStackPos2, unsigned int iStackPosResult );
	int multiplyStackField ( unsigned int iStackPos1, double factor );
    int addStackFields ( unsigned int iStackPos1, unsigned int iStackPos2, unsigned int iStackPosResult );
    CvpField* getStackField ( unsigned int iStackPos );
	bool hasData ( void );
    int getNrOfObjects ( void );
    vector<string*> getStructureNames ( void );
    int setImportance ( string sStruct, float fImportance );
    int setStatus ( string sStruct, bool flEnabled );
    int setCamPos ( float fX, float fY, float fZ );
    int setCamRange ( float fR );
    int setStackFieldAsRegionField ( int iStackPos );
    int addPointRegionToStackField ( int iStackPos, float fX, float fY, float fZ, float fRange );
    int addVectorRegionToStackField ( int iStackPos, float fX, float fY, float fZ, float fRange );
	int clearStack( int iStackPos );
    int setVisStability ( float fVisSta );
    int setImpStability ( float fImpSta );
    int setWeight ( int iField, float fValue );
    float getWeight ( int iField );
	//void setUseExternalPrefRegion ( bool value );
	void setPrefRegionVector ( float fX, float fY, float fZ );
	void setPrefRegionRange ( double value );
	void setRestrictToRegion ( bool value );
	enum PrefRegionTypeEnum { PR_VECTOR = 0, PR_POINT = 1 };
	void setPrefRegionType ( PrefRegionTypeEnum value );

    int getFieldMaxPos ( int iFieldType, float &fX, float &fY, float &fZ );
    int getStackMaxPos ( int iStackPos, float &fX, float &fY, float &fZ );
	int getMatrixValues ( int iCamID, string sStruct, std::vector<float>* vValues);
	int findObjectId (string sStruct);
	string getObjectName (int iID);
	int calc (void);

    void logResults (void);
	void setApexAngle(int angle);

	int getDataViewportX(void);
	int getDataViewportY(void);
	int getDataNrOfObjects(void);
	int getDataDiv(void);
	int getDataCamCount(void);
	string getDataVersion(void);
    
  private:
    int m_iViewportX, m_iViewportY, m_iNrOfObjects, m_iDiv;
	char m_cVersionChar[3];
    float m_fSphereX,m_fSphereY,m_fSphereZ, m_fSphereRadius;
    float *** m_fResultMatrix;
    float m_fCamX, m_fCamY, m_fCamZ;
    float m_fCamRange;
    float m_fVisSta, m_fImpSta;

    CvpDivSphere* m_oSphere;
    vector<string*> m_vStructures;
    bool m_flHasData;
    bool m_flHasImportance;
    bool m_flHasStack;
    int m_iCurrentId;

	//bool useExternalPrefReg;
	float prefRegionVectorX,prefRegionVectorY,prefRegionVectorZ;
	double prefRegionRange;
	bool restrictToRegion;	
	PrefRegionTypeEnum prefRegionType;

	int _apexAngle;

    unsigned int m_iStackSize;

    CvpField *m_oFieldStack;

    CvpField *m_oVisField;
    CvpField *m_oImpField;
    CvpField *m_oNumField;
    CvpField *m_oEntField;
    CvpField *m_oDisField;
    CvpField *m_oCamField;
    CvpField *m_oRegField;
    CvpField *m_oVisStaField;
    CvpField *m_oImpStaField;
	CvpField *m_oSilField;
	CvpField *m_oCenterField;
    CvpField *m_oSumField;

    CvpField *m_oTempField;
    CvpField *m_oTempField2;

    float m_fVisW,m_fImpW,m_fNumW,m_fEntW,m_fDisW,m_fCamW,m_fRegW,m_fVisStaW,m_fImpStaW,m_fSilW,m_fCenterW;

    CvpField *m_oObjectImportance;
    CvpField *m_oObjectStatus;

    void _loadIds(const char* cPath);    
    void _scaleNormalizedField (CvpField *oField, float fRange);
    void _calcPointRegion ( CvpField *oField, float fX, float fY, float fZ, float fRange );
    void _calcVecRegion ( CvpField *oField, float fX, float fY, float fZ, float fRange );

    void _calcVisField (void);
    void _calcImpField (void);
    void _calcNumField (void);
    void _calcEntField (void);
    void _calcDisField (void);
    void _calcCamField (void);
    void _calcStaField (int iField);
	void _calcPrefRegionField (void);
	void _calcSilField (void);
	void _calcCenterField (void);
    void _calcSum (void);
};

#endif