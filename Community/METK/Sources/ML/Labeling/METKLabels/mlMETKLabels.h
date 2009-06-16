//----------------------------------------------------------------------------------
//! The ML module class METKLabels.
/*!
// \file    mlMETKLabels.h
// \author  Konrad Mühler
// \date    2007-07-13
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETKLabels_H
#define __mlMETKLabels_H

// Local includes
#include "LabelingSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>
#include "ObjMgrCommunicator.h"
#include "kDebug.h"

class SoLabelModul;

ML_START_NAMESPACE

//!
class LABELING_EXPORT METKLabels : public ObjMgrClient
{
public:

	//! Constructor.
	METKLabels (void);
	~METKLabels();

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);
	virtual void handleObjMgrNotification();
	virtual void activateAttachments();

	ObjMgrCommunicator* getObjMgr();

private:

	typedef ObjMgrClient inherited;

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	//!
	NotifyField* _Refresh;
	NotifyField* _ReConnectInventor;
	SoNodeField* _outInventor;
	SoNodeField* _inInventor;

	SoNodeField* _outColorBufferView;
	//SoNodeField* _outMyAnnotatorOutput;

	StringField* _fontFile;
	IntField* _fontSize;
	ColorField* _fontColor;
	FloatField* _lineSpacing;
	IntField* _insetSpace;
	BoolField* _layHaloColorInvert;
	BoolField* _layOrthoRefLines;

	ColorField* _arrowColor;
	ColorField* _arrowColorHalo;
	ColorField* _refLineColor;
	ColorField* _refLineHaloColor;
	FloatField* _arrowSize;
	FloatField* _arrowThickness;

	ColorField* _bgColor;
	ColorField* _defaultLabelBgColor;
	FloatField* _labelBGAlpha;

	BoolField* _labelIfInteraction;
	BoolField* _labelBackground;
	BoolField* _motionLabeling;

	BoolField* _debug_drawOrbit;
	NotifyField* _printLabelObjects;
	NotifyField* _writeBufferDFT;
	IntField* _skipLevel;
	IntField* _orbitDistance;



	EnumField* fldDebugState;
	EnumField* fldLineWidth;

	EnumField * fldInvisObjStyle;

	//@}

	ObjMgrCommunicator* myObjMgr;
	SoLabelModul* myLabelModul;

	SoTimerSensor* timerSensor; //!< Delays the reconnecting of inventor scene after visibility changes
	static void timerEvent(void* data, SoDataSensor* a);
	void refreshAfterVisibility(SoDataSensor* sensor);

	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(METKLabels)

};

ML_END_NAMESPACE

#endif // __mlMETKLabels_H