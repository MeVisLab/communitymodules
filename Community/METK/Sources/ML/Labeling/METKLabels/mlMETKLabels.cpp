//----------------------------------------------------------------------------------
//! The ML module class METKLabels.
/*!
// \file    mlMETKLabels.cpp
// \author  Konrad Mühler
// \date    2007-07-13
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMETKLabels.h"
#include "METK.h"

#include "UMDLabelModul/SoLabelModul.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKLabels, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKLabels::METKLabels (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METKLabels::METKLabels()")

	FieldContainer *fields = getFieldContainer();

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	static const char* debugStates[3] = {"None","High","Low"};
	fldDebugState = getFieldContainer()->addEnum("debugState", debugStates, 3);
	fldDebugState->setStringValue("None");
	kDebug::setDebugLevel(fldDebugState->getStringValue());
	static const char* lineWidths[5] = {"1","2","3","4","5"};
	fldLineWidth = getFieldContainer()->addEnum("lineWidth", lineWidths, 5);
	fldLineWidth->setStringValue("1");


	myObjMgr = new ObjMgrCommunicator();

	_Refresh = fields->addNotify("Refresh");
	_ReConnectInventor = fields->addNotify("ReConnectInventor");
	_outInventor = fields->addSoNode("outInventor");
	_inInventor = fields->addSoNode("inInventor");

	_outColorBufferView = fields->addSoNode("outInventor");
	//_outMyAnnotatorOutput = fields->addSoNode("outInventor");

	_fontFile = fields->addString("fontFile");
	_fontFile->setStringValue("c:/windows/Fonts/arial.ttf");
	_fontSize = fields->addInt("fontSize");
	_fontSize->setIntValue(12);
	_fontColor = fields->addColor("fontColor");
	_fontColor->setColorValue(0,0,0);
	_lineSpacing = fields->addFloat("lineSpacing");
	_lineSpacing->setFloatValue(1.0);
	_insetSpace = fields->addInt("insetSpace");
	_insetSpace->setIntValue(5);
	_layHaloColorInvert = fields->addBool("layHaloColorInvert");
	_layHaloColorInvert->setBoolValue(true);
	_layOrthoRefLines = fields->addBool("layOrthRefLines");
	_layOrthoRefLines->setBoolValue(false);
	static const char* invisStyles[4] = {"None","Dashed Line","Solid Line","Arrow"};
	fldInvisObjStyle = getFieldContainer()->addEnum("invisObjStyle", invisStyles, 4);
	fldInvisObjStyle->setEnumValue(0);


	_arrowColor = fields->addColor("arrowColor");
	_arrowColor->setColorValue(0,0,0);
	_refLineColor = fields->addColor("refLineColor");
	_refLineColor->setColorValue(0,0,0);
	_refLineHaloColor = fields->addColor("refLineHaloColor");
	_refLineHaloColor->setColorValue(1,1,1);
	_arrowColorHalo = fields->addColor("arrowColorHalo");
	_arrowColorHalo->setColorValue(1,1,1);
	_arrowSize = fields->addFloat("arrowSize");
	_arrowSize->setFloatValue(10);
	_arrowThickness = fields->addFloat("arrowThickness");
	_arrowThickness->setFloatValue(1.5);

	_bgColor = fields->addColor("bgColor");
	_bgColor->setColorValue(1,1,1);
	_defaultLabelBgColor = fields->addColor("defaultLabelBgColor");
	_defaultLabelBgColor->setColorValue(0.8,0.8,0.8);
	_labelBGAlpha = fields->addFloat("labelBGAlpha");
	_labelBGAlpha->setFloatValue(1.0);

	_labelIfInteraction = fields->addBool("labelIfInteraction");
	_labelIfInteraction->setBoolValue(true);
	/*_labelIfInvis = fields->addBool("labelIfInvis");
	_labelIfInvis->setBoolValue(false);*/
	_labelBackground = fields->addBool("labelBackground");
	_labelBackground->setBoolValue(false);
	_motionLabeling = fields->addBool("motionLabeling");
	_motionLabeling->setBoolValue(true);
	_debug_drawOrbit = fields->addBool("debug_drawOrbit");
	_debug_drawOrbit->setBoolValue(false);
	_printLabelObjects = fields->addNotify("printLabelObjects");
	_writeBufferDFT = fields->addNotify("writeBufferDFT");
	_skipLevel = fields->addInt("skipLevel");
	_skipLevel->setIntValue(3);
	_orbitDistance = fields->addInt("orbitDistance");
	_orbitDistance->setIntValue(20);



	myLabelModul = new SoLabelModul();
	myLabelModul->ref();
	_outInventor->setSoNodeValue(myLabelModul);
	_outColorBufferView->setSoNodeValue(myLabelModul->outputColorBufferView.getValue());
	//_outMyAnnotatorOutput->setSoNodeValue(myLabelModul->outputMyAnnotatorOutput.getValue());
	myLabelModul->setMLClass(this);

	//Ein Refresh am Anfang damit man auch ohne Labels was sieht //geht nicht??
	myLabelModul->clear.touch();
	myLabelModul->start.touch();


	clearAcceptedObjectIDs();
	addAcceptedObjectID("*");
	clearAcceptedInfoLayerNames();
	addAcceptedInfoLayerName(LAY_APPEARANCE);
	addAcceptedInfoLayerName(LAY_LABEL);
	addAcceptedInfoLayerName(LAY_GLOBALEVENTS);

	timerSensor = new SoTimerSensor((SoSensorCB*)METKLabels::timerEvent, this);
	timerSensor->setInterval(SbTime(1.0/3.0));	
	timerSensor->unschedule();

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}

METKLabels::~METKLabels(){}

void METKLabels::activateAttachments(){
	ML_TRACE_IN("METKLabels::activateAttachments()");

	getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),true);
	//std::cout << "1 - set ->input to " << _inInventor->getSoNodeValue() << std::endl;
	myLabelModul->input = _inInventor->getSoNodeValue();

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKLabels::handleNotification (Field *field)
{
	ML_TRACE_IN("METKLabels::handleNotification()")
	// ... field notification handling code
	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-) (aus METKObjContainer geklaut)			
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);			
		}	
	}
	else if (field == getFieldContainer()->getField("inInventor"))
	{		
		myLabelModul->input = _inInventor->getSoNodeValue();
	}
	else if (field == _Refresh)
	{
		//std::cout << "Refresh" << std::endl;
		myLabelModul->clear.touch();
		myLabelModul->start.touch();

		//frühzeitiges return damit am ende nicht wieder _Refresh->notify() aufgerufen wird
		inherited::handleNotification(field);
		return;
	}

	else if (field == _ReConnectInventor)
	{
		std::cout << "ReConncetInventor" << std::endl;
		_outColorBufferView->setSoNodeValue(myLabelModul->outputColorBufferView.getValue());
		//_outMyAnnotatorOutput->setSoNodeValue(myLabelModul->outputMyAnnotatorOutput.getValue());
	}

	else if (field == fldDebugState)
	{		
		kDebug::setDebugLevel(fldDebugState->getStringValue());
	}

	else if (field == fldLineWidth){
		myLabelModul->refLineWidth.setValue(kBasics::StringToFloat(fldLineWidth->getStringValue()));
	}

	else if (field == _fontFile) myLabelModul->fontFile.setValue(_fontFile->getStringValue().c_str());
	else if (field == _fontSize) myLabelModul->fontSize.setValue(_fontSize->getIntValue());
	else if (field == _fontColor) myLabelModul->fontColor.setValue(SbColor(_fontColor->getVec3fValue()[0],_fontColor->getVec3fValue()[1],_fontColor->getVec3fValue()[2]));
	else if (field == _lineSpacing) myLabelModul->linespacing.setValue(_lineSpacing->getFloatValue());
	else if (field == _insetSpace) myLabelModul->insetspace.setValue(_insetSpace->getIntValue());
	else if (field == _layHaloColorInvert) myLabelModul->haloColorInvert.setValue(_layHaloColorInvert->getBoolValue());
	else if (field == _layOrthoRefLines) myLabelModul->orthoRefLines.setValue(_layOrthoRefLines->getBoolValue());
	else if (field == fldInvisObjStyle) myLabelModul->invisObjStyle.setValue(fldInvisObjStyle->getEnumValue());
	else if (field == _arrowColor) myLabelModul->arrowColor.setValue(SbColor(_arrowColor->getVec3fValue()[0],_arrowColor->getVec3fValue()[1],_arrowColor->getVec3fValue()[2]));
	else if (field == _refLineColor) myLabelModul->refLineColor.setValue(SbColor(_refLineColor->getVec3fValue()[0],_refLineColor->getVec3fValue()[1],_refLineColor->getVec3fValue()[2]));
	else if (field == _refLineHaloColor) myLabelModul->refLineHaloColor.setValue(SbColor(_refLineHaloColor->getVec3fValue()[0],_refLineHaloColor->getVec3fValue()[1],_refLineHaloColor->getVec3fValue()[2]));
	else if (field == _arrowColorHalo) myLabelModul->arrowColorHalo.setValue(SbColor(_arrowColorHalo->getVec3fValue()[0],_arrowColorHalo->getVec3fValue()[1],_arrowColorHalo->getVec3fValue()[2]));
	else if (field == _arrowSize) myLabelModul->arrowSize.setValue(_arrowSize->getFloatValue());
	else if (field == _arrowThickness) myLabelModul->arrowThickness.setValue(_arrowThickness->getFloatValue());
	else if (field == _bgColor) myLabelModul->bgColor.setValue(SbColor(_bgColor->getVec3fValue()[0],_bgColor->getVec3fValue()[1],_bgColor->getVec3fValue()[2]));
	else if (field == _defaultLabelBgColor) myLabelModul->defaultLabelBgColor.setValue(SbColor(_defaultLabelBgColor->getVec3fValue()[0],_defaultLabelBgColor->getVec3fValue()[1],_defaultLabelBgColor->getVec3fValue()[2]));
	else if (field == _labelBGAlpha) myLabelModul->labelBGAlpha.setValue(_labelBGAlpha->getFloatValue());
	else if (field == _labelIfInteraction) myLabelModul->labelIfInteraction.setValue(_labelIfInteraction->getBoolValue());
	else if (field == _labelBackground) myLabelModul->labelBackground.setValue(_labelBackground->getBoolValue());
	else if (field == _motionLabeling) myLabelModul->motionLabeling.setValue(_motionLabeling->getBoolValue());
	else if (field == _debug_drawOrbit) myLabelModul->debug_drawOrbit.setValue(_debug_drawOrbit->getBoolValue());
	else if (field == _printLabelObjects) myLabelModul->printLabelObjects.touch();
	else if (field == _writeBufferDFT) myLabelModul->myAnnotator->saveBufferAsBMP();
	else if (field == _skipLevel) myLabelModul->skipLevel.setValue(_skipLevel->getIntValue());
	else if (field == _orbitDistance) myLabelModul->orbitDistance.setValue(_orbitDistance->getIntValue());
	//sehr wichtig, weil sonst die ObjMgr-Ereignisse dieses Modul nie erreichen
	inherited::handleNotification(field);
	_Refresh->notify();
}

void METKLabels::handleObjMgrNotification()
{
	ML_TRACE_IN("METKLabels::handleObjMgrNotification()");
	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);	    
		if (myEvent.layerID == LAY_APPEARANCE && myEvent.infoID == INF_VISIBLE)
		{				
			timerSensor->unschedule();
			timerSensor->schedule();
			//Start per timer:
			//_outColorBufferView->setSoNodeValue(myLabelModul->outputColorBufferView.getValue());
			//_ReConnectInventor->notify();
			//_Refresh->notify();
		}
		else if (myEvent.layerID == LAY_LABEL)
		{	
			_Refresh->notify();
		}
		else if (myEvent.layerID == LAY_GLOBALEVENTS && myEvent.infoID == INF_CASELOADED)
		{
			_Refresh->notify();
		}

	}
	clearEventContainer();
}

ObjMgrCommunicator* METKLabels::getObjMgr()
{
	return myObjMgr;
}



//! Called by timerSensor;
void METKLabels::timerEvent(void* data, SoDataSensor* a)
{	
	((METKLabels*)data)->refreshAfterVisibility(a);
}



//! Reset the Status to Ready if rendering process is ready
void METKLabels::refreshAfterVisibility(SoDataSensor* /*sensor*/)
{
	timerSensor->unschedule();
	_outColorBufferView->setSoNodeValue(myLabelModul->outputColorBufferView.getValue());
	_ReConnectInventor->notify();
	_Refresh->notify();
}

ML_END_NAMESPACE