//----------------------------------------------------------------------------------
//! The ML module class METK2DLabeling.
/*!
// \file    mlMETK2DLabeling.cpp
// \author  Konrad Mühler
// \date    2008-04-11
//
// Module to label MCSM in 2d
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlMETK2DLabeling.h"
#include "../METK.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METK2DLabeling, ObjMgrClient);

const char* METK2DLabeling::debugOutputStrings[METK2DLabeling::NUM_DEBUGOUTPUT] = { "None", "Voxel values", "Component IDs", "Global Distance Field", "Anchor Point Distance Field", "Text Distance Horizontal 1", "Text Distance Horizontal 2", "Free Space Field", "Label Boxes"};
const char* METK2DLabeling::debugStateStrings[METK2DLabeling::NUM_DEBUGSTATEOUTPUT] = { "None", "All"};
const int METK2DLabeling::LT_CONNECTIONLINE = 1;
const int METK2DLabeling::LT_TEXTBB = 2;
const int METK2DLabeling::DIRTYPE_UPRIGHT = 1;
const int METK2DLabeling::DIRTYPE_DOWNLEFT = 2;
const int METK2DLabeling::DIRTYPE_UPLEFT = 3;
const int METK2DLabeling::DIRTYPE_DOWNRIGHT = 4;
const int METK2DLabeling::DIRTYPE_UP = 5;
const int METK2DLabeling::DIRTYPE_DOWN = 6;
const int METK2DLabeling::DIRTYPE_LEFT = 7;
const int METK2DLabeling::DIRTYPE_RIGHT = 8;
const int METK2DLabeling::IT_CL_CL = 1;
const int METK2DLabeling::IT_CL_TEXT = 2;
const int METK2DLabeling::IT_TEXT_TEXT = 3;



//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METK2DLabeling::METK2DLabeling () : inherited(2,1,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METK2DLabeling::METK2DLabeling ()");

	// Get reference to the container for parameters/fields.
	FieldContainer &fields = *getFieldContainer();

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_optFactor = 2.0;

	// Add fields to the module and set their values.
	// Also attach them to the output images to notify connected modules about changes.
	_sliceFld = fields.addInt("slice");
	_sliceFld->setIntValue(0);
	_sliceFld->attachField(getOutField(0));
	_enableLabelingFld = fields.addBool("enableLabeling");
	_enableLabelingFld->setBoolValue(true);
	//_enableLabelingFld->attachField(getOutField(0));
	_maxConflictIterationsFld = fields.addInt("maxConflictIterations");
	_maxConflictIterationsFld->setIntValue(20);
	_maxConflictIterationsFld->attachField(getOutField(0));	
	_calculateFld = fields.addNotify("calculate");
	//_calculateFld->attachField(getOutField(0));
	_initPlacementFld = fields.addNotify("initPlacement");
	//_initPlacementFld->attachField(getOutField(0));
	_refineFld = fields.addNotify("refine");
	//_refineFld->attachField(getOutField(0));
	_refineSingleStepFld = fields.addNotify("refineSingleStep");
	//_refineSingleStepFld->attachField(getOutField(0));

	(_optFactorFld = fields.addInt("optFactor"))->setIntValue(2);

	_debugOutputFld = fields.addEnum("debugOutput", debugOutputStrings, NUM_DEBUGOUTPUT);
	_debugOutputFld->setEnumValue(2);
	//_debugOutputFld->attachField(getOutField(0));

	_debugStateFld = fields.addEnum("debugState", debugStateStrings, NUM_DEBUGSTATEOUTPUT);
	_debugStateFld->setEnumValue(1);

	(_anchorPointInspectionFld = fields.addInt("anchorPointInspection"))->setIntValue(1);
	//_anchorPointInspectionFld->attachField(getOutField(0));

	(_borderDistanceFld = fields.addInt("borderDistance"))->setIntValue(7);
	//_borderDistanceFld->attachField(getOutField(0));

	(_minCompSizeFld = fields.addInt("minCompSize"))->setIntValue(2);
	//_minCompSizeFld->attachField(getOutField(0));

	(_maxGroupDistanceFld = fields.addInt("maxGroupDistance"))->setIntValue(50);
	//_maxGroupDistanceFld->attachField(getOutField(0));

	(_transpThresholdFld = fields.addDouble("transpThreshold"))->setDoubleValue(0.8);

	(_encircleSizeFld = fields.addInt("encircleSize"))->setIntValue(5);

	(_textColorFld = fields.addColor("textColor"))->setColorValue(0,0,0);
	(_boxColorFld = fields.addColor("boxColor"))->setColorValue(1,1,1);
	(_boxTransparencyFld = fields.addDouble("boxTransparency"))->setDoubleValue(0.3);
	(_borderColorFld = fields.addColor("borderColor"))->setColorValue(0,0,0);
	(_lineColorFld = fields.addColor("lineColor"))->setColorValue(1,1,0);
	(_autoBoxColorFld = fields.addBool("autoBoxColor"))->setBoolValue(false);
	(_innerBorderFld = fields.addInt("innerBorder"))->setIntValue(2);

	(_useAdditionalMaskFld = fields.addBool("useAdditionalMask"))->setBoolValue(false);

	(_sendCSOFld = fields.addBool("sendCSO"))->setBoolValue(true);
	(_CSOFld = fields.addString("CSO"))->setStringValue("");

	_2dLabelsNode = new SoDraw2DLabels();
	_2dLabelsNode->ref();
	_outInventor = getFieldContainer()->addSoNode("outInventor");	
	_outInventor->setSoNodeValue(_2dLabelsNode);

	_2dLabelsNode->setFontSize(12);
	_2dLabelsNode->setSendCSO(_sendCSOFld->getBoolValue());

	_voxelValues = NULL;
	_additionalMask = NULL;
	_componentIDs = NULL;
	_globalDistanceField = NULL;
	_textFreeSpaceHorizontalField.resize(2);
	_textFreeSpaceHorizontalField[0] = NULL;
	_textFreeSpaceHorizontalField[1] = NULL;
	_tempFieldForLabelBoxes = NULL;

	myObjMgr = new ObjMgrCommunicator();
	myObjIterator = new ObjConstrainedIterator();

	timerSensor = new SoTimerSensor((SoSensorCB*)METK2DLabeling::timerEvent, this);
	timerSensor->setInterval(SbTime(1.0/4.0));
	timerSensor->unschedule();

	//Der FieldSensor ist nötig, um auf Ereignisse von Inventormodulen (Bspw. Ready-Trigger) von internen Inventormodulen zu hören
	_fieldSensor = new SoFieldSensor(METK2DLabeling::fieldChangedCB, this);
	_fieldSensor->setPriority(0);
	_fieldSensor->attach(_2dLabelsNode->getField("_CSOFld"));


	// Connect input fields with output field to update connected
	// modules if input images change.
	for (int inIdx=0; inIdx < 1; ++inIdx){
		for (int outIdx=0; outIdx < 1; ++outIdx){
			getInField(inIdx)->attachField(getOutField(outIdx));
		}
	}

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();

}

METK2DLabeling::~METK2DLabeling()
{
	if (_voxelValues) delete _voxelValues;
	_voxelValues = NULL;
	if (_additionalMask) delete _additionalMask;
	_additionalMask = NULL;
	if (_componentIDs) delete _componentIDs;
	_componentIDs = NULL;
	if (_globalDistanceField) delete _globalDistanceField;
	_globalDistanceField = NULL;
	if (_textFreeSpaceHorizontalField[0]) delete _textFreeSpaceHorizontalField[0];
	_textFreeSpaceHorizontalField[0] = NULL;
	if (_textFreeSpaceHorizontalField[1]) delete _textFreeSpaceHorizontalField[1];
	_textFreeSpaceHorizontalField[1] = NULL;
	if (_tempFieldForLabelBoxes) delete _tempFieldForLabelBoxes;
	_tempFieldForLabelBoxes = NULL;

	if (myObjMgr) delete myObjMgr;
	myObjMgr = NULL;
	if (myObjIterator) delete myObjIterator;
	myObjIterator = NULL;

	if (timerSensor) delete timerSensor;
	timerSensor = NULL;
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void METK2DLabeling::handleNotification (Field *field)
{
	ML_TRACE_IN("METK2DLabeling::handleNotification ()");

	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjIterator->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}
	else if (field == _initPlacementFld)
	{
		initialPlacement();
		transferLabelsToInventor();
	}

	else if (field== _refineSingleStepFld)
	{
		resolveConflicts();
		transferLabelsToInventor();
	}

	else if (field==_refineFld)
	{
		refineAll();
		transferLabelsToInventor();
	}

	else if (field==_calculateFld)
	{
		initialPlacement();
		refineAll();
		transferLabelsToInventor();
	}

	else if (field==_sliceFld)
	{
		_2dLabelsNode->clearLabels();
		if (_enableLabelingFld->getBoolValue())
		{
			_currentSlice = _sliceFld->getIntValue();
			timerSensor->schedule();
			//initialPlacement();
			//refineAll();
		}
	}

	else if (field==_optFactorFld)
	{
		_optFactor = _optFactorFld->getIntValue();
	}

	else if (field==_sendCSOFld)
	{
		_2dLabelsNode->setSendCSO(_sendCSOFld->getBoolValue());
	}
}

//----------------------------------------------------------------------------------
//! Update internal module state after load or clone and enable notification handling again.
//----------------------------------------------------------------------------------
void METK2DLabeling::activateAttachments ()
{
	ML_TRACE_IN("METK2DLabeling::activateAttachments ()");

	clearAcceptedObjectIDs();
	addAcceptedObjectID("*");
	clearAcceptedInfoLayerNames();
	//addAcceptedInfoLayerName(LAY_APPEARANCE);
	addAcceptedInfoLayerName("*");

	sendNotification();

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}




void METK2DLabeling::handleObjMgrNotification() {
	ML_TRACE_IN("METK2DLabeling::handleObjMgrNotification()");

	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);

		//Status has changed
		//if (myEvent.infoID == INF_SURFACEDIST || myEvent.infoID == INF_VISIBLE || myEvent.infoID == INF_IVPOINTER) {
	}
	clearEventContainer();
}


//----------------------------------------------------------------------------------
//! Configures (in)validation handling of inputs which are not connected or up to date.
//----------------------------------------------------------------------------------
BaseOp::INPUT_HANDLE METK2DLabeling::handleInput(int /*inIndex*/, INPUT_STATE /*state*/) const
{
	ML_TRACE_IN("METK2DLabeling::handleInput ()");

	return INVALIDATE; 
}

//----------------------------------------------------------------------------------
//! Sets properties of the output image at output outIndex.
//----------------------------------------------------------------------------------
void METK2DLabeling::calcOutImageProps (int /*outIndex*/)
{
	ML_TRACE_IN("METK2DLabeling::calcOutImageProps ()");

	//std::cout << "METK2DLabeling::calcOutImageProps ()" << std::endl;
	// Change properties of output image outIndex here whose 
	// defaults are inherited from the input image (if there is one).

	getOutImg()->setImgExt  ( getInImg(0)->getImgExt() );
	getOutImg()->setPageExt ( getInImg(0)->getPageExt() );

	// Determine new min/max range.
	long double min = -1000;
	long double max = 1000;
	getOutImg()->setMinVoxelValue( min );
	getOutImg()->setMaxVoxelValue( max );

	// Get data type which can hold the new data range correctly. Use signed types for ambiguous cases
	// where both the unsigned and signed data types could hold the data range. E.g. range is 13 - 97 where
	// unsigned and signed 8 bit integer both are well suited for that range.
	MLDataType dt = MLGetDataTypeForRange(&min, &max, false);

	// Set found data type. If no one is found then invalidate output image.
	if (MLIsValidType(dt)){ getOutImg()->setDataType(dt);         }
	else                  { getOutImg()->setOutOfDate(); return;  }

}

//----------------------------------------------------------------------------------
//! Returns the input image region required to calculate a region of an output image.
//----------------------------------------------------------------------------------
SubImgBox METK2DLabeling::calcInSubImageBox (int /*inIndex*/, const SubImgBox &outSubImgBox, int /*outIndex*/)
{
	ML_TRACE_IN("METK2DLabeling::calcInSubImageBox ()");

	// Return region of input image inIndex needed to compute region 
	// outSubImgBox of output image outIndex.
	return outSubImgBox;
}

//----------------------------------------------------------------------------------
//! Called to determine properties props of input image region (tile) of input inIndex 
//! required to calculate an output image region (page) of output outIndex.
//----------------------------------------------------------------------------------
void METK2DLabeling::calcInSubImageProps(int /*inIndex*/,
										 InSubImageProps& /*props*/,
										 int /*outIndex*/)
{
	ML_TRACE_IN("METK2DLabeling::calcInSubImageProps ()");
}

//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of calcOutSubImage to calculate page 
//! outSubImg of output image with index outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE1_CPP(METK2DLabeling);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by 
//! CALC_OUTSUBIMAGE1_CPP(METK2DLabeling).
//----------------------------------------------------------------------------------
template <typename T>
void METK2DLabeling::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex
									  , TSubImg<T> *inSubImg0
									  )
{
	ML_TRACE_IN("template <typename T> METK2DLabeling::calcOutSubImage ()");
	if (!getUpdatedInImg(0)) return;

	if (_debugOutputFld->getEnumValue()==DOUT_NONE ||
		!_voxelValues ||
		!_componentIDs ||
		!_globalDistanceField ||
		!_textFreeSpaceHorizontalField[0] ||
		!_textFreeSpaceHorizontalField[1] ||
		!_tempFieldForLabelBoxes ||
		_debugStateFld->getEnumValue()==DSTATE_NONE) return;

	// Compute subimage of output image outIndex from input subimages.

	// Clamp box of output image against image extent to avoid that unused areas are processed.
	const SubImgBox validOutBox(outSubImg->getBox().intersect(getOutImg(outIndex)->getBoxFromImgExt()));

	_sliceSizeOrg = getUpdatedInImg(0)->getImgExt();

	switch (_debugOutputFld->getEnumValue())
	{
	case DOUT_VOXEL:
		getOutImg()->setMaxVoxelValue( _voxelValues->getMaxValue() );
		getOutImg()->setMinVoxelValue( _voxelValues->getMinValue() );
		break;
	case DOUT_COMP:
		getOutImg()->setMaxVoxelValue( _componentIDs->getMaxValue() );
		getOutImg()->setMinVoxelValue( _componentIDs->getMinValue() );
		break;
	case DOUT_DISTANCE:
		getOutImg()->setMaxVoxelValue( _globalDistanceField->getMaxValue() );
		getOutImg()->setMinVoxelValue( _globalDistanceField->getMinValue() );
		break;
	case DOUT_TEXTH1:
		getOutImg()->setMaxVoxelValue( _textFreeSpaceHorizontalField[0]->getMaxValue() );
		getOutImg()->setMinVoxelValue( _textFreeSpaceHorizontalField[0]->getMinValue() );
		break;
	case DOUT_TEXTH2:
		getOutImg()->setMaxVoxelValue( _textFreeSpaceHorizontalField[1]->getMaxValue() );
		getOutImg()->setMinVoxelValue( _textFreeSpaceHorizontalField[1]->getMinValue() );
		break;
	case DOUT_FREESPACE:
		getOutImg()->setMaxVoxelValue( 1 );
		getOutImg()->setMinVoxelValue( 0 );
		break;
	case DOUT_LABELBOXES:
		getOutImg()->setMaxVoxelValue( _tempFieldForLabelBoxes->getMaxValue() );
		getOutImg()->setMinVoxelValue( _tempFieldForLabelBoxes->getMinValue() );
		break;
	}

	// Process all voxels of the valid region of the output page.
	Vector p;
	long int valuePos;
	int anchorNr;
	for (p.u=validOutBox.v1.u;  p.u<=validOutBox.v2.u;  ++p.u){
		for (p.t=validOutBox.v1.t;  p.t<=validOutBox.v2.t;  ++p.t){
			for (p.c=validOutBox.v1.c;  p.c<=validOutBox.v2.c;  ++p.c){
				for (p.z=validOutBox.v1.z;  p.z<=validOutBox.v2.z;  ++p.z){
					for (p.y=validOutBox.v1.y;  p.y<=validOutBox.v2.y;  ++p.y){

						p.x = validOutBox.v1.x;
						// Get pointers to row starts of input and output subimages.
						const T    *in0Voxel = inSubImg0->getImgPos(p);
						T          *outVoxel = outSubImg->getImgPos(p);
						const MLint rowEnd   = validOutBox.v2.x;

						// Process all row voxels.
						for (; p.x <= rowEnd;  ++p.x, ++in0Voxel, ++outVoxel){
							//*outVoxel = *in0Voxel;
							//*outVoxel = p.y*p.z+p.x;
							valuePos = (p.y/_optFactor)*(_sliceSizeOrg.x/_optFactor)+(p.x/_optFactor);
							if (valuePos<_voxelValues->getSize())
							{
								switch (_debugOutputFld->getEnumValue())
								{
								case DOUT_VOXEL:
									*outVoxel = (T)_voxelValues->getValueAt(valuePos);
									break;
								case DOUT_COMP:
									*outVoxel = (T)_componentIDs->getValueAt(valuePos);
									break;
								case DOUT_DISTANCE:
									*outVoxel = (T)_globalDistanceField->getValueAt(valuePos);
									break;
								case DOUT_ANCHORDIST:									
									anchorNr = _anchorPointInspectionFld->getIntValue();
									if (anchorNr<_components.size())
										if (_components[anchorNr].anchorDistanceField)
											*outVoxel = (T)_components[anchorNr].anchorDistanceField->getValueAt(valuePos);
									else
										*outVoxel = 0;
									break;
								case DOUT_TEXTH1:
									*outVoxel = (T)_textFreeSpaceHorizontalField[0]->getValueAt(valuePos);
									break;
								case DOUT_TEXTH2:
									*outVoxel = (T)_textFreeSpaceHorizontalField[1]->getValueAt(valuePos);
									break;
								case DOUT_FREESPACE:									
									anchorNr = _anchorPointInspectionFld->getIntValue();
									if (anchorNr<_components.size())
										//if (_components[anchorNr].maxInnerDistance<-1*(_minCompSizeFld->getIntValue()/_optFactor)) //is valid
										if (_components[anchorNr].valid) //is valid
											if (_components[anchorNr].freeSpaceField)
												*outVoxel = (T)_components[anchorNr].freeSpaceField->getValueAt(valuePos);
										else
											*outVoxel = 0;
									else
										*outVoxel = 0;
									break;
								case DOUT_LABELBOXES:
									*outVoxel = (T)_tempFieldForLabelBoxes->getValueAt(valuePos);
									break;

								default:
									*outVoxel = 0;
								}
							}
							else
								*outVoxel = 12;
						}
					}
				}
			}
		}
	}
}


void METK2DLabeling::initialPlacement()
{
	if (!getUpdatedInImg(0)) return;
	MLDataType mlDt = getUpdatedInImg(0)->getDataType();

	switch (mlDt) {
		  case MLint8Type:       calculateLabels<MLint8>(mlDt); break;
		  case MLuint8Type:      calculateLabels<MLuint8>(mlDt); break;
		  case MLint16Type:      calculateLabels<MLint16>(mlDt); break;
		  case MLuint16Type:     calculateLabels<MLuint16>(mlDt); break;
		  case MLint32Type:      calculateLabels<MLint32>(mlDt); break;
		  case MLuint32Type:     calculateLabels<MLuint32>(mlDt); break;
		  case MLint64Type:      calculateLabels<MLint64>(mlDt); break;
		  case MLfloatType:      calculateLabels<MLfloat>(mlDt); break;
		  case MLldoubleType:    calculateLabels<MLldouble>(mlDt); break;
		  default: // using double was the default case as for now, so we keep it that way:
		  case MLdoubleType:     calculateLabels<MLdouble>(mlDt); break;
	}

}

void METK2DLabeling::refineAll()
{
	//std::cout << "refineAll-start" << std::endl;
	int iteration=0;
	int conflicts = _labelConflicts.size();		
	while (iteration<_maxConflictIterationsFld->getIntValue() && conflicts!=0)
	{
		conflicts = resolveConflicts();
		iteration++;
		if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "conflicts = " << conflicts << "   iterations=" << iteration << std::endl;
	}
	//std::cout << "refineAll-stop" << std::endl;
}


template <typename DT>
void METK2DLabeling::calculateLabels(MLDataType ML_DT)
{
	ML_TRACE_IN("void METK2DLabeling::calculateLabels()");
	if (!getUpdatedInImg(0)) return;

	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "calculateLabels start" << std::endl;

	//std::cout << "01 start" << std::endl;

	_sliceSizeOrg = getUpdatedInImg(0)->getImgExt();
	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "x=" << _sliceSizeOrg.x << "  y=" << _sliceSizeOrg.y << "  z=" << _sliceSizeOrg.z << std::endl;	
	_sliceSizeOpt.x = _sliceSizeOrg.x/_optFactor; _sliceSizeOpt.y = _sliceSizeOrg.y/_optFactor; //_sliceSizeOpt.z = _sliceSizeOrg.z/_optFactor;
	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "optimized x=" << _sliceSizeOpt.x << "  y=" << _sliceSizeOpt.y << "  z=" << _sliceSizeOpt.z << std::endl;

	//Create Fields
	//EVTL EIN DELETE NOCH VORHER!!!!!
	if (_voxelValues) delete _voxelValues;
	_voxelValues = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);
	if (_additionalMask) delete _additionalMask;
	if (_useAdditionalMaskFld->getBoolValue())
		_additionalMask = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);
	if (_componentIDs) delete _componentIDs;
	_componentIDs = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);
	if (_globalDistanceField) delete _globalDistanceField;
	_globalDistanceField = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);
	if (_textFreeSpaceHorizontalField[0]) delete _textFreeSpaceHorizontalField[0];
	_textFreeSpaceHorizontalField[0] = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);
	if (_textFreeSpaceHorizontalField[1]) delete _textFreeSpaceHorizontalField[1];
	_textFreeSpaceHorizontalField[1] = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);
	if (_tempFieldForLabelBoxes) delete _tempFieldForLabelBoxes;
	_tempFieldForLabelBoxes = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);

	_componentIDs->fillWith(-1);
	_globalDistanceField->fillWith(32000);
	_textFreeSpaceHorizontalField[0]->fillWith(0);
	_textFreeSpaceHorizontalField[1]->fillWith(0);
	_tempFieldForLabelBoxes->fillWith(0);

	_labelLines.clear();
	_labelBoxes.clear();
	_labelConflicts.clear();
	_components.clear();

	//std::cout << "02 start ObjMgr" << std::endl;
	buildObjIDList();
	//std::cout << "03 stop ObjMgr" << std::endl;

	//Fill _voxelValues
	SubImg mySubImg;
	mySubImg.setBox(Vector(0,0,_sliceFld->getIntValue(),_sliceSizeOrg.x,_sliceSizeOrg.y,_sliceFld->getIntValue()));
	mySubImg.setDataType(getInImg(0)->getDataType());	
	int error = getUpdatedInImg(0)->getTile(mySubImg);	
	if (error!=0)
	{
		std::cout << "getUpdatedInImg(0)->getTile Error = " << MLGetErrorCodeDescription(error) << std::endl;
		return;
	}

	// Init the slice data pointer here since we want to reuse the allocated slice memory later.
	void *sliceData = NULL;

	MLErrorCode err = getTile(getInOp(0),
		getInOpIndex(0),
		SubImgBox(Vector(   0,    0, _sliceFld->getIntValue(), 0 /*c*/, 0 /*t*/ , 0 /*u*/),
		Vector(_sliceSizeOrg.x-1, _sliceSizeOrg.y-1, _sliceFld->getIntValue(), 0 /*c*/, 0 /*t*/ , 0 /*u*/)),
		ML_DT,
		&sliceData);

	if (err!=0)
	{
		std::cout << "getTile Error = " << MLGetErrorCodeDescription(err) << std::endl;
		return;
	}


	// Scan the slice value by value.
	// get correctly typed pointer:
	DT* slice = (DT*) sliceData;
	for (MLint y = 0; y < _sliceSizeOrg.y-_optFactor; /*y++*/y=y+_optFactor)
	{						
		const MLint rowStart = _sliceSizeOrg.x * y;
		DT* row = &slice[rowStart];
		for (MLint x = 0; x < _sliceSizeOrg.x-_optFactor; /*x++*/x=x+_optFactor)
		{
			// Get value from image.	
			row = row+_optFactor;
			const DT voxelValue = *row;
			_voxelValues->setValueAt(x/_optFactor,y/_optFactor,voxelValue);
		}              
	}


	// Release the allocated memory.
	if (sliceData){
		freeTile(sliceData);
		sliceData = NULL;
	}


	//Fill _additionalMask
	if (_useAdditionalMaskFld->getBoolValue() &&
		getUpdatedInImg(1))
	{
		if (getUpdatedInImg(0)->getDataType()==getUpdatedInImg(1)->getDataType() &&
			getUpdatedInImg(0)->getImgExt()==getUpdatedInImg(1)->getImgExt())
		{			
			mySubImg.setBox(Vector(0,0,_sliceFld->getIntValue(),_sliceSizeOrg.x,_sliceSizeOrg.y,_sliceFld->getIntValue()));
			mySubImg.setDataType(getInImg(1)->getDataType());	
			error = getUpdatedInImg(1)->getTile(mySubImg);	
			if (error!=0)
			{
				std::cout << "getUpdatedInImg(1)->getTile Error = " << MLGetErrorCodeDescription(error) << std::endl;
				return;
			}

			// Init the slice data pointer here since we want to reuse the allocated slice memory later.
			sliceData = NULL;

			err = getTile(getInOp(1),
				getInOpIndex(1),
				SubImgBox(Vector(   0,    0, _sliceFld->getIntValue(), 0 /*c*/, 0 /*t*/ , 0 /*u*/),
				Vector(_sliceSizeOrg.x-1, _sliceSizeOrg.y-1, _sliceFld->getIntValue(), 0 /*c*/, 0 /*t*/ , 0 /*u*/)),
				ML_DT,
				&sliceData);

			if (err!=0)
			{
				std::cout << "getTile Error = " << MLGetErrorCodeDescription(err) << std::endl;
				return;
			}


			// Scan the slice value by value.
			// get correctly typed pointer:
			slice = (DT*) sliceData;
			for (MLint y = 0; y < _sliceSizeOrg.y-_optFactor; /*y++*/y=y+_optFactor)
			{						
				const MLint rowStart = _sliceSizeOrg.x * y;
				DT* row = &slice[rowStart];
				for (MLint x = 0; x < _sliceSizeOrg.x-_optFactor; /*x++*/x=x+_optFactor)
				{
					// Get value from image.	
					row = row+_optFactor;
					const DT voxelValue = *row;
					_additionalMask->setValueAt(x/_optFactor,y/_optFactor,voxelValue);
				}              
			}

			// Release the allocated memory.
			if (sliceData){
				freeTile(sliceData);
				sliceData = NULL;
			}
		}
		else
		{
			std::cout << "Labeling error: Additional mask has not the same data type as MCSM or not the same image dimensions!" << std::endl;
			delete _additionalMask;
			_additionalMask = NULL;
		}
	}
	else
	{
		delete _additionalMask;
		_additionalMask = NULL;
	}
	

	int compCount;
	//std::cout << "04 start calcConnectedComp" << std::endl;
	compCount = calcConnectedComp();
	//std::cout << "05 stop calcConnectedComp" << std::endl;

	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "compCount=" << compCount << std::endl;
	for (MLint i=0; i<_components.size(); i++)
	{
		_components[i].maxInnerDistance=1;
		//_components[i].labelText = "Component "+kBasics::IntToString(i)+"\n Subtitle abc";
		if (_components[i].valid)
		{
			myObjMgr->getObjAttributeString(_components[i].objectID,LAY_LABEL,INF_LABELTEXT,_components[i].labelText);
			_2dLabelsNode->getTextDimension(_components[i].labelText,_components[i].textSize.x,_components[i].textSize.y,_optFactor);
			_components[i].totalSize.x = _components[i].textSize.x+2*(_borderDistanceFld->getIntValue()/_optFactor);
			_components[i].totalSize.y = _components[i].textSize.y+2*(_borderDistanceFld->getIntValue()/_optFactor);
			_components[i].anchorDistanceField = new labField(_sliceSizeOpt.x,_sliceSizeOpt.y);		
		}
		else
		{
			_components[i].labelText="";
			_components[i].anchorDistanceField = NULL;
		}
		
		_components[i].freeSpaceField = NULL;		
		_components[i].parentID=-1;
		_components[i].groupID=-1;		
		//_components[i].objectID=1; //LATER: real Object ID for grouping
	}


	//!!ACHTUNG
	//_globalDistanceField wird derzeit nirgendwo genutzt!!!
	//Aber weitere Berechnungen in dieser Funktion schon
	calcGlobalDistanceField();

	calcGroups();

	calcAnchorPointDistances();

	//std::cout << "xxx" << std::endl;
	calcFreeSpace();

	_labelConflicts.clear();

	//std::cout << "yyy" << std::endl;
	calcTextPos(); //Dauert wohl sehr lange

	//transferLabelsToInventor();

	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "calculateLabels end" << std::endl;
	//std::cout << "05 stop calcLabels" << std::endl;
}



int METK2DLabeling::calcConnectedComp()
{
	ML_TRACE_IN("void METK2DLabeling::calcConnectedComp()");

	//Calculate the connected components
	multimap<int,int> voxelToWatch;
	unsigned long int pos;
	multimap<int,int>::iterator tempVoxel;
	int currentCompID=0;
	int tempX,tempY;
	string currentObjID;
	for (unsigned int i=0; i<_sliceSizeOpt.x; i++)
	{
		for (unsigned int j=0; j<_sliceSizeOpt.y; j++)
		{
			pos = j*_sliceSizeOpt.x+i;
			if (_componentIDs->getValueAt(pos)==-1.0)
			{				
				currentCompID++;
				_components.resize(currentCompID+1);
				//currentObjID=_voxelValues->getValueAt(pos);
				currentObjID = _voxelValObjID[_voxelValues->getValueAt(pos)];
				_components[currentCompID].objectID=currentObjID;
				_components[currentCompID].voxelCount=0;				
				double* tempTransp = _htObjIDTransp.find(currentObjID);
				if (tempTransp)
				{
					if (*tempTransp<_transpThresholdFld->getDoubleValue())
					{
						_components[currentCompID].importanceLayer=0;
					}
					else _components[currentCompID].importanceLayer=1;
				}
				else _components[currentCompID].importanceLayer=0;

				//Find neigbourhs
				voxelToWatch.clear();
				voxelToWatch.insert(pair<int,int>(i,j));
				while (voxelToWatch.size()>0)
				{
					//pop first voxel
					tempVoxel = voxelToWatch.begin();
					tempX=(int)tempVoxel->first;
					tempY=(int)tempVoxel->second;
					pos = tempY*_sliceSizeOpt.x+tempX;
					voxelToWatch.erase(voxelToWatch.begin());

					if (_componentIDs->getValueAt(pos)==-1.0)
					{
						if (tempX<_sliceSizeOpt.x-1)						
							//if (_voxelValues->getValueAt(pos+1)==currentObjID)
							if (_voxelValObjID[_voxelValues->getValueAt(pos+1)]==currentObjID
								&& _componentIDs->getValueAt(pos+1)==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX+1,tempY));												

						if (tempX>0)
							//if (_voxelValues->getValueAt(pos-1)==currentObjID)
							if (_voxelValObjID[_voxelValues->getValueAt(pos-1)]==currentObjID
								&& _componentIDs->getValueAt(pos-1)==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX-1,tempY));

						if (tempY<_sliceSizeOpt.y-1)
							//if (_voxelValues->getValueAt(pos+_sliceSizeOpt.x)==currentObjID)
							if (_voxelValObjID[_voxelValues->getValueAt(pos+_sliceSizeOpt.x)]==currentObjID 
								&& _componentIDs->getValueAt(pos+_sliceSizeOpt.x)==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX,tempY+1));

						if (tempY>0)
							//if (_voxelValues->getValueAt(pos-_sliceSizeOpt.x)==currentObjID)
							if (_voxelValObjID[_voxelValues->getValueAt(pos-_sliceSizeOpt.x)]==currentObjID
								&& _componentIDs->getValueAt(pos-_sliceSizeOpt.x)==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX,tempY-1));

					}
					/* alte bones test variante
					if (_voxelValues->getValueAt(pos)==0)
					{
					_componentIDs->setValueAt(pos,0);
					}
					else
					{
					_componentIDs->setValueAt(pos,currentCompID);
					}
					*/
					
					if (currentObjID=="")
					{
						_componentIDs->setValueAt(pos,0);
					}
					else
					{
						int oldValue = _componentIDs->getValueAt(pos);
						if (oldValue!=currentCompID)
						{
							_componentIDs->setValueAt(pos,currentCompID);
							_components[currentCompID].voxelCount++;
							if (oldValue>=0) _components[oldValue].voxelCount--;
						}
					}
				}
			}
		}
	}

	//Set valid or not
	for (unsigned int i=0; i<_components.size(); i++)
	{
		if (_components[i].voxelCount>0 &&
			_components[i].voxelCount*_optFactor*_optFactor>=_minCompSizeFld->getIntValue() && 
			_components[i].objectID!="")
		{
			bool labelOn;
			myObjMgr->getObjAttributeBool(_components[i].objectID,LAY_LABEL,INF_LABELON,labelOn);
			if (labelOn)
				_components[i].valid=true;
			else
				_components[i].valid=false;
		}
		else _components[i].valid=false;
	}

	return currentCompID;
}



void METK2DLabeling::calcGlobalDistanceField()
{
	ML_TRACE_IN("void METK2DLabeling::calcGlobalDistanceField()");

	//Create distance field
	//First only for background or not
	unsigned long int pos;
	int a,b,c,d;
	int currentDistance = 0;
	int currentComponentID;
	for (unsigned int i=1; i<_sliceSizeOpt.x-1; i++) //Rand lassen
	{
		for (unsigned int j=1; j<_sliceSizeOpt.y-1; j++) //Rand lassen
		{			
			pos = j*_sliceSizeOpt.x+i;
			currentDistance = _globalDistanceField->getValueAt(pos);
			a=_globalDistanceField->getValueAt(pos-1);
			b=_globalDistanceField->getValueAt(pos+1);
			c=_globalDistanceField->getValueAt(pos-_sliceSizeOpt.x);
			d=_globalDistanceField->getValueAt(pos+_sliceSizeOpt.x);
			currentComponentID = _componentIDs->getValueAt(i,j);

			if (currentComponentID==0.0) //is pixel a background pixel
			{			
				//////currentDistance = min(currentDistance,min(a+1,min(b+1,min(c+1,d+1))));
				//We calculate no distances for background pixels
				currentDistance=1;
			}
			else
			{
				//Is there a background pixel in the 4-nei. then its a border pixel with distance=0
				//better: pixel that not belongs to component
				if (_componentIDs->getValueAt(i-1,j)!=currentComponentID ||
				_componentIDs->getValueAt(i+1,j)!=currentComponentID ||
				_componentIDs->getValueAt(i,j-1)!=currentComponentID ||
				_componentIDs->getValueAt(i,j+1)!=currentComponentID)
				{
					currentDistance = 0;
				}
				else
				{
					if (currentDistance>0) currentDistance = -30000;
					if (a>0) a=-30000; if (b>0) b=-30000; if (c>0) c=-30000; if (d>0) d=-30000; //Only neccery for the first pass; in the second pass, all 32.000 values are overwritten
					currentDistance = max(currentDistance,max(a-1,max(b-1,max(c-1,d-1))));
				}
			}
			_globalDistanceField->setValueAt(pos,currentDistance);
		}
	}

	//And reverse
	for (unsigned int i=_sliceSizeOpt.x-2; i>0; i--) //Rand lassen
	{
		for (MLint j=_sliceSizeOpt.y-2; j>0; j--) //Rand lassen
		{			
			pos = j*_sliceSizeOpt.x+i;
			currentDistance = _globalDistanceField->getValueAt(pos);
			a=_globalDistanceField->getValueAt(pos-1);
			b=_globalDistanceField->getValueAt(pos+1);
			c=_globalDistanceField->getValueAt(pos-_sliceSizeOpt.x);
			d=_globalDistanceField->getValueAt(pos+_sliceSizeOpt.x);
			currentComponentID = _componentIDs->getValueAt(i,j);

			if (currentComponentID==0.0) //is pixel a background pixel
			{			
				///////currentDistance = min(currentDistance,min(a+1,min(b+1,min(c+1,d+1))));
				//We calculate no distances for background pixels
				currentDistance=1;
			}
			else
			{
				//Is there a background pixel in the 4-nei. then its a border pixel with distance=0
				if (_componentIDs->getValueAt(i-1,j)!=currentComponentID ||
					_componentIDs->getValueAt(i+1,j)!=currentComponentID  ||
					_componentIDs->getValueAt(i,j-1)!=currentComponentID  ||
					_componentIDs->getValueAt(i,j+1)!=currentComponentID)
				{
					currentDistance = 0;
				}
				else
				{
					if (currentDistance>0) currentDistance = -32000;
					currentDistance = max(currentDistance,max(a-1,max(b-1,max(c-1,d-1))));
				}

				if (_components[currentComponentID].maxInnerDistance>currentDistance)
				{
					_components[currentComponentID].maxInnerDistance=currentDistance;
					_components[currentComponentID].anchorPoint.x=i;
					_components[currentComponentID].anchorPoint.y=j;
				}

			}
			_globalDistanceField->setValueAt(pos,currentDistance);
		}
	}
}


//Group components
void METK2DLabeling::calcGroups()
{
	int nextFreeGroupID=1;
	for (MLint i=1; i<_components.size(); i++)
	{
		//check distance to all other components
		for (MLint j=i+1; j<_components.size(); j++)
		{
			if (_components[i].objectID==_components[j].objectID && 
				//_components[i].maxInnerDistance<-1*(_minCompSizeFld->getIntValue()/_optFactor) && 
				//_components[j].maxInnerDistance<-1*(_minCompSizeFld->getIntValue()/_optFactor))
				_components[i].valid &&
				_components[j].valid)

			{
				//Calc distance
				int tempDist = abs(_components[i].anchorPoint.x-_components[j].anchorPoint.x)+
					abs(_components[i].anchorPoint.y-_components[j].anchorPoint.y);
				if (tempDist<=_maxGroupDistanceFld->getIntValue()/_optFactor)
				{
					if (_components[j].groupID==-1)
					{
						if (_components[i].groupID==-1)
						{
							_components[i].groupID=nextFreeGroupID;
							nextFreeGroupID++;
							_components[i].parentID=i; //itself
						}
						_components[j].groupID=_components[i].groupID;
						_components[j].parentID=_components[i].parentID;
					}
					else //Object has still group ID
					{
						if (_components[i].groupID==-1)
						{
							_components[i].groupID=_components[j].groupID;
							_components[i].parentID=_components[j].parentID;
						}
						else if (_components[i].groupID!=_components[j].groupID) // hier wirds böse
						{
							//Merging to groups
							if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "Merging group" << _components[i].groupID << " with group " << _components[j].groupID << std::endl;
							//All from the j group get values from the i group
							int newParent=_components.size();
							for (MLint k=0; k<_components.size(); k++)
							{								
								if (_components[k].groupID==_components[j].groupID || _components[k].groupID==_components[i].groupID)
								{
									if (k<newParent) newParent=k;
									_components[k].groupID=_components[i].groupID;
									_components[k].parentID=newParent;
								}
							}
						}
					}
				}
			}
		}
	}

	//Debug output
	if (_debugStateFld->getEnumValue()==DSTATE_ALL)
	{
		for (MLint i=1; i<_components.size(); i++)
		{
			if (_components[i].groupID!=-1)
				std::cout << "Component " << i << "  Group=" << _components[i].groupID << "  ParentID=" << _components[i].parentID << std::endl;
		}
	}
}

//Calculate distance fields for every anchor point
void METK2DLabeling::calcAnchorPointDistances()
{
	for (MLint i=0; i<_components.size(); i++)
	{
		//if (_components[i].maxInnerDistance<-1*(_minCompSizeFld->getIntValue()/_optFactor))
		if (_components[i].valid)
		{
			//_components[i].anchorDistanceField->fillManhattan(_components[i].anchorPoint.x,_components[i].anchorPoint.y);
			_components[i].anchorDistanceField->fillManhattanTextBoxCorrected(_components[i].anchorPoint.x,_components[i].anchorPoint.y,_components[i].textSize.x,_components[i].textSize.y);
		}		
	}
}


//Calculate Horizontal Free Space for later label placement
void METK2DLabeling::calcFreeSpace()
{
	ML_TRACE_IN("void METK2DLabeling::calcFreeSpace()");

	int currentValue[2];
	currentValue[0]=0; currentValue[1]=0;
	float tempValue, tempValueAddMask;

	//Horizontal text space
	for (MLint j=0; j<_sliceSizeOpt.y; j++) //each row
	{
		currentValue[0]=0; currentValue[1]=0;
		for (MLint i=_sliceSizeOpt.x-1; i>=0; i--) //from right to left
		{
			tempValue=_componentIDs->getValueAt(i,j);
			tempValueAddMask=0;
			if (_additionalMask)
			{
				tempValueAddMask=_additionalMask->getValueAt(i,j);
			}

			if (tempValue>0 || tempValueAddMask!=0)
			{
				if (_components[tempValue].importanceLayer==0 || tempValueAddMask!=0)
				{
					currentValue[0] = 0; //reset distance to 0
					currentValue[1] = 0;
					_textFreeSpaceHorizontalField[0]->setValueAt(i,j,0);
					_textFreeSpaceHorizontalField[1]->setValueAt(i,j,0);
				}
				else
				{
					_textFreeSpaceHorizontalField[0]->setValueAt(i,j,currentValue[0]++);
					currentValue[1] = 0;
					_textFreeSpaceHorizontalField[1]->setValueAt(i,j,0);
				}
			}
			else
			{				
				_textFreeSpaceHorizontalField[0]->setValueAt(i,j,currentValue[0]++);
				_textFreeSpaceHorizontalField[1]->setValueAt(i,j,currentValue[1]++);
			}
		}
	}
}



//Calculate posible text position from _textFreeSpaceHorizontalField
void METK2DLabeling::calcTextPos()
{
	ML_TRACE_IN("void METK2DLabeling::calcTextPos()");
	labField* tempField = new labField(_textFreeSpaceHorizontalField[0]->getSizeX(),_textFreeSpaceHorizontalField[0]->getSizeY());

	if (_tempFieldForLabelBoxes)
	{
		delete _tempFieldForLabelBoxes;
		_tempFieldForLabelBoxes = NULL;
	}
	_tempFieldForLabelBoxes = new labField(_textFreeSpaceHorizontalField[0]->getSizeX(),_textFreeSpaceHorizontalField[0]->getSizeY());

	for (MLint i=0; i<_components.size(); i++)
	{
		if (_components[i].parentID>i)
		{
			std::cout << "Boeser Fehler. ParentID > id !!! Duerfte eigentlich nie auftreten." << std::endl; 
		}
		if (_components[i].groupID!=-1 && _components[i].parentID!=i)
		{
			int parentID = _components[i].parentID;
			//Component has parent and receive its positions
			//We hope, the parent id < i so the position is alread calculated
			_components[i].lineEndPoint = _components[parentID].lineEndPoint;
			_components[i].labelPoint = _components[parentID].labelPoint;
		}
		//else if (_components[i].maxInnerDistance<-1*(_minCompSizeFld->getIntValue()/_optFactor))  //has anchorpoint ???
		else if (_components[i].valid)
		{
			//Create field where 1=enough space for label and 0=not
			if (_components[i].freeSpaceField)
			{
				delete _components[i].freeSpaceField;
				_components[i].freeSpaceField = NULL;
			}

			int importanceLayer = _components[i].importanceLayer;
			_components[i].freeSpaceField = new labField(_textFreeSpaceHorizontalField[importanceLayer]->getSizeX(),_textFreeSpaceHorizontalField[importanceLayer]->getSizeY());
			_components[i].freeSpaceField->set(_textFreeSpaceHorizontalField[importanceLayer]);
			_components[i].freeSpaceField->binarize(_components[i].totalSize.x);

			//Search in binarized Horiz. Field for vertical lines of label-hight and set all other to 0
			for (MLint ix=0; ix<_components[i].freeSpaceField->getSizeX(); ix++)
			{
				for (MLint iy=0; iy<_components[i].freeSpaceField->getSizeY()-_components[i].totalSize.y; iy++)
				{
					//Check voxel below
					if (_components[i].freeSpaceField->getValueAt(ix,iy)!=0)
					{
						for (MLint ih=0; ih<_components[i].totalSize.y; ih++)
						{
							if (_components[i].freeSpaceField->getValueAt(ix,iy+ih)==0)
							{
								_components[i].freeSpaceField->setValueAt(ix,iy,0);
								break;
							}
						}
					}
				}
				if (_components[i].freeSpaceField->getSizeY()-_components[i].totalSize.y>0)
				{
					for (MLint iy=_components[i].freeSpaceField->getSizeY()-_components[i].totalSize.y; iy<_components[i].freeSpaceField->getSizeY(); iy++)
					{
						_components[i].freeSpaceField->setValueAt(ix,iy,0);
					}
				}
			}

			/////////////_components[i].freeSpaceField->multiply(tempField);


			//Find best position
			if (_components[i].groupID==-1)
			{
				tempField->set(_components[i].anchorDistanceField);
			}
			else
			{
				kBasics::POINT corner1, corner2, tempPoint;
				corner1.x=_sliceSizeOpt.x+1;corner1.y=_sliceSizeOpt.y+1;
				corner2.x=-1; corner2.y=-1;
				//Sum up all group partner's fields
				tempField->set(_components[i].anchorDistanceField);
				for (MLint k=0; k<_components.size(); k++)
				{
					if (_components[k].groupID==_components[i].groupID)
					{
						if (k!=i) tempField->add(_components[k].anchorDistanceField);
						tempPoint = _components[k].anchorPoint;
						if (tempPoint.x<corner1.x) corner1.x=tempPoint.x;
						if (tempPoint.x>corner2.x) corner2.x=tempPoint.x;
						if (tempPoint.y<corner1.y) corner1.y=tempPoint.y;
						if (tempPoint.y>corner2.y) corner2.y=tempPoint.y;						
					}
				}

				//Disable positions in the inner of the bounding box of all anchor points (easy version of convex hul)
				for (MLint ix=max(0,corner1.x-_components[i].textSize.x); ix<=corner2.x; ix++)
				{
					for (MLint iy=max(0,corner1.y-_components[i].textSize.y); iy<=corner2.y; iy++)
					{
						tempField->setValueAt(ix,iy,0);
					}
				}

				//tempField->normalize();
			}


			tempField->multiply(_components[i].freeSpaceField);

			int minPos=0;
			float minValue;
			bool posFound = false;
			int iterations=0;

			while (minPos!=-1 && !posFound && iterations<5)
			{
				tempField->scanMinThreshold(1,minPos,minValue);
				iterations++;
				if (minPos!=-1)
				{
					_components[i].labelPoint.y = floor(minPos / tempField->getSizeX());
					_components[i].labelPoint.x = minPos - (_components[i].labelPoint.y*tempField->getSizeX());

					kBasics::POINT labelPoint = _components[i].labelPoint;
					labelPoint.x+=_borderDistanceFld->getIntValue()/_optFactor;
					labelPoint.y+=_borderDistanceFld->getIntValue()/_optFactor;										

					calcLabelLine(_components[i].anchorPoint, labelPoint, _components[i].textSize, _components[i].lineEndPoint);

					posFound=true;
				}
			}
			if (iterations>=5) posFound=true;

			if (posFound)
			{
				//disableLabelPosition(i);
				if (_debugStateFld->getEnumValue()==DSTATE_ALL) debugLabelsIn2D(i);

				if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "label pos of component " << i << " = " << _components[i].labelPoint.x << "," << _components[i].labelPoint.y << std::endl;

				addLabelLinesToCache(i);

			}
			else
			{
				if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "No Position found for component " << i << std::endl;
			}
		}		
	}

	_tempFieldForLabelBoxes->add(_componentIDs);	

	delete tempField;
}


void METK2DLabeling::transferLabelsToInventor()
{
	//std::cout << "transferLabelsToInventor-start" << std::endl;
	_2dLabelsNode->clearLabels();
	_2dLabelsNode->setInnerBorder(_innerBorderFld->getIntValue());
	int tempGroupType;
	SbVec3f tempTextColor;
	_textColorFld->getColorValue(tempTextColor[0],tempTextColor[1],tempTextColor[2]);
	SbVec3f tempBorderColor;
	_borderColorFld->getColorValue(tempBorderColor[0],tempBorderColor[1],tempBorderColor[2]);
	SbVec3f tempLineColor;
	_lineColorFld->getColorValue(tempLineColor[0],tempLineColor[1],tempLineColor[2]);


	for (MLint i=0; i<_components.size(); i++)
	{
		//if (_components[i].maxInnerDistance<-1*(_minCompSizeFld->getIntValue()/_optFactor))
		if (_components[i].valid)
		{
			//_components[i].labelPoint.x = _components[i].labelPoint.x + _borderDistanceFld->getIntValue();
			//_components[i].labelPoint.y = _components[i].labelPoint.y + _borderDistanceFld->getIntValue();
			if (_components[i].groupID==-1)
			{
				tempGroupType=SoDraw2DLabels::GROUPTYPE_NONE;
			}
			else if (_components[i].parentID==i)
			{
				tempGroupType=SoDraw2DLabels::GROUPTYPE_PARENT;
			}
			else
			{
				tempGroupType=SoDraw2DLabels::GROUPTYPE_CHILD;
			}
			SbVec3f tempBoxColor;
			if (_autoBoxColorFld->getBoolValue())
			{
				myObjMgr->getObjAttributeVec3f(_components[i].objectID,LAY_APPEARANCE,INF_COLOR,tempBoxColor);
			}
			else _boxColorFld->getColorValue(tempBoxColor[0],tempBoxColor[1],tempBoxColor[2]);
			
			_2dLabelsNode->addLabel(_components[i].anchorPoint, _components[i].labelPoint, _components[i].lineEndPoint, tempGroupType, _components[i].labelText, _borderDistanceFld->getIntValue(), _optFactor, _components[i].voxelCount*_optFactor*_optFactor<=_encircleSizeFld->getIntValue(),2*sqrt(_components[i].voxelCount*_optFactor*_optFactor),tempTextColor, tempBoxColor, _boxTransparencyFld->getDoubleValue(), tempBorderColor, tempLineColor, _components[i].objectID);
		}		
	}
	_2dLabelsNode->touch();
	//std::cout << "transferLabelsToInventor-end" << std::endl;
}


//Disable label position for later labels by correcting the horizontal distance fields
void METK2DLabeling::disableLabelPosition(const int i)
{	
	//Correct Horizontal
	for (MLint jy=_components[i].labelPoint.y; jy<_components[i].labelPoint.y+_components[i].totalSize.y; jy++)
	{
		for (MLint jx=_components[i].labelPoint.x+_components[i].totalSize.x; jx>=0; jx--)
		{
			if (jx>=_components[i].labelPoint.x)
			{
				_textFreeSpaceHorizontalField[0]->setValueAt(jx,jy,0);
			}
			else
			{
				if (_textFreeSpaceHorizontalField[0]->getValueAt(jx,jy)!=0)
				{
					_textFreeSpaceHorizontalField[0]->setValueAt(jx,jy,_components[i].labelPoint.x-jx);
				}
				else
				{
					//break
					jx=-1;
				}
			}
		}
	}
}



void METK2DLabeling::debugLabelsIn2D(const int i)
{
	//For debug purposes only; visualize the labelboxes in a 2d field
	int tempValue;
	for (MLint ix=_components[i].labelPoint.x; ix<_components[i].labelPoint.x+_components[i].totalSize.x; ix++)
	{
		for (MLint iy=_components[i].labelPoint.y; iy<_components[i].labelPoint.y+_components[i].totalSize.y; iy++)
		{
			if (iy>_components[i].labelPoint.y+(_borderDistanceFld->getIntValue()/_optFactor) &&
				iy<_components[i].labelPoint.y+_components[i].totalSize.y-(_borderDistanceFld->getIntValue()/_optFactor))
			{
				if (ix>_components[i].labelPoint.x+(_borderDistanceFld->getIntValue()/_optFactor) &&
					ix<_components[i].labelPoint.x+_components[i].totalSize.x-(_borderDistanceFld->getIntValue()/_optFactor))
				{
					tempValue=0;
				}
				else
				{
					tempValue=i;
				}
			}
			else
			{
				tempValue=i;
			}

			_tempFieldForLabelBoxes->setValueAt(ix,iy,tempValue); //sets the component ID as labelbox value
		}
	}
}





int METK2DLabeling::resolveConflicts()
{
	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "resolving conflicts" << std::endl;

	//int iterations=0;

	//transfer conflict list to a new one
	std::vector<LABELCONFLICT> tempConflictList;
	for (unsigned int i=0; i<_labelConflicts.size(); i++)
	{
		tempConflictList.push_back(_labelConflicts[i]);
	}
	_labelConflicts.clear();

	for (unsigned int i=0; i<tempConflictList.size(); i++)
	{
		int id1 = tempConflictList[i].componentID1;
		int id2 = tempConflictList[i].componentID2;

		bool intersect=false;
		kBasics::POINT intersectionPoint;
		kBasics::BOX intersectionBox;
		kBasics::POINT newStartPoint1,newStartPoint2;
		int dirType1,dirType2;
		int minDist;
		//first check, if intersection still exists
		if (tempConflictList[i].intersectionType==IT_CL_CL && _components[id1].groupID==-1 && _components[id2].groupID==-1)
		{			
			intersect=kBasics::lineIntersection(_components[id1].lineEndPoint,_components[id1].anchorPoint,
				_components[id2].lineEndPoint,_components[id2].anchorPoint,intersectionPoint);

			if (intersect)
			{
				//find new posision for label 1
				//calc direction of search
				int xDiff = _components[id2].lineEndPoint.x - _components[id1].lineEndPoint.x;
				int yDiff = _components[id2].lineEndPoint.y - _components[id1].lineEndPoint.y;

				if (yDiff<=0 && xDiff>=0) dirType1 = DIRTYPE_UPRIGHT;
				else if (yDiff>=0 && xDiff<=0) dirType1 = DIRTYPE_DOWNLEFT;
				else if (yDiff<=0 && xDiff<=0) dirType1 = DIRTYPE_UPLEFT;
				else if (yDiff>=0 && xDiff>=0) dirType1 = DIRTYPE_DOWNRIGHT;

				minDist = 1;
				newStartPoint1 = _components[id2].labelPoint;


				//find new posision for label 2
				xDiff = _components[id1].lineEndPoint.x - _components[id2].lineEndPoint.x;
				yDiff = _components[id1].lineEndPoint.y - _components[id2].lineEndPoint.y;		
				if (yDiff<=0 && xDiff>=0) dirType2 = DIRTYPE_UPRIGHT;
				else if (yDiff>=0 && xDiff<=0) dirType2 = DIRTYPE_DOWNLEFT;
				else if (yDiff<=0 && xDiff<=0) dirType2 = DIRTYPE_UPLEFT;
				else if (yDiff>=0 && xDiff>=0) dirType2 = DIRTYPE_DOWNRIGHT;

				minDist = 1;
				newStartPoint2 = _components[id1].labelPoint;
			}
		}
		else if (tempConflictList[i].intersectionType==IT_TEXT_TEXT)
		{
			intersect=kBasics::boxIntersection(
				kBasics::POINT(_components[id1].labelPoint.x-_borderDistanceFld->getIntValue()/_optFactor,_components[id1].labelPoint.y-_borderDistanceFld->getIntValue()/_optFactor),
				kBasics::POINT(_components[id1].labelPoint.x+_components[id1].textSize.x+_borderDistanceFld->getIntValue()/_optFactor,_components[id1].labelPoint.y+_components[id1].textSize.y+_borderDistanceFld->getIntValue()/_optFactor),
				kBasics::POINT(_components[id2].labelPoint.x-_borderDistanceFld->getIntValue()/_optFactor,_components[id2].labelPoint.y-_borderDistanceFld->getIntValue()/_optFactor),
				kBasics::POINT(_components[id2].labelPoint.x+_components[id2].textSize.x+_borderDistanceFld->getIntValue()/_optFactor,_components[id2].labelPoint.y+_components[id2].textSize.y+_borderDistanceFld->getIntValue()/_optFactor),
				intersectionBox);

			if (intersect)
			{
				//find new posision for label 1
				//calc direction of search
				int boxSizeX = intersectionBox.corner2.x-intersectionBox.corner1.x;
				int boxSizeY = intersectionBox.corner4.y-intersectionBox.corner1.y;

				if (boxSizeX<boxSizeY && _components[id1].labelPoint.x<=_components[id2].labelPoint.x)
				{
					dirType1 = DIRTYPE_LEFT;
					dirType2 = DIRTYPE_RIGHT;
					//minDist = boxSizeX/2+1;
					minDist=1;
					newStartPoint1 = _components[id1].labelPoint;
					newStartPoint1.x-=(boxSizeX/2+1); //verschiebe Startpunkt um Mindestverschiebung
					newStartPoint2 = _components[id2].labelPoint;
					newStartPoint2.x+=(boxSizeX/2+1); //verschiebe Startpunkt um Mindestverschiebung
				}
				else if (boxSizeX<boxSizeY && _components[id2].labelPoint.x<=_components[id1].labelPoint.x)
				{
					dirType2 = DIRTYPE_LEFT;
					dirType1 = DIRTYPE_RIGHT;
					//minDist = boxSizeX/2+1;
					minDist=1;
					newStartPoint1 = _components[id1].labelPoint;
					newStartPoint1.x+=(boxSizeX/2+1); //verschiebe Startpunkt um Mindestverschiebung
					newStartPoint2 = _components[id2].labelPoint;
					newStartPoint2.x-=(boxSizeX/2+1); //verschiebe Startpunkt um Mindestverschiebung
				}
				else if (boxSizeX>=boxSizeY && _components[id1].labelPoint.y<=_components[id2].labelPoint.y)
				{
					dirType1 = DIRTYPE_UP;
					dirType2 = DIRTYPE_DOWN;
					//minDist = boxSizeY/2+1;
					minDist=1;
					newStartPoint1 = _components[id1].labelPoint;
					newStartPoint1.y-=(boxSizeY/2+1); //verschiebe Startpunkt um Mindestverschiebung
					newStartPoint2 = _components[id2].labelPoint;
					newStartPoint2.y+=(boxSizeY/2+1); //verschiebe Startpunkt um Mindestverschiebung
				}
				else if (boxSizeX>=boxSizeY && _components[id2].labelPoint.y<=_components[id1].labelPoint.y)
				{
					dirType2 = DIRTYPE_UP;
					dirType1 = DIRTYPE_DOWN;
					//minDist = boxSizeY/2+1;
					minDist=1;
					newStartPoint1 = _components[id1].labelPoint;
					newStartPoint1.y+=(boxSizeY/2+1); //verschiebe Startpunkt um Mindestverschiebung
					newStartPoint2 = _components[id2].labelPoint;
					newStartPoint2.y-=(boxSizeY/2+1); //verschiebe Startpunkt um Mindestverschiebung
				}				
			}
		}


		if (intersect)
		{
			bool newPosFound;
			if (_components[id1].groupID==-1)
			{
				newPosFound = findNewPosition(id1, //label id
					newStartPoint1, //new start point
					dirType1,
					minDist); //min distance from start point

				if (newPosFound)
				{
					//Calc new connection line
					kBasics::POINT labelPoint = _components[id1].labelPoint;
					labelPoint.x+=_borderDistanceFld->getIntValue()/_optFactor;
					labelPoint.y+=_borderDistanceFld->getIntValue()/_optFactor;
					calcLabelLine(_components[id1].anchorPoint, labelPoint, _components[id1].textSize, _components[id1].lineEndPoint);
				}
				//Delete old line from cache
				deleteLinesFromCache(id1);
				//Add new lines to cache
				addLabelLinesToCache(id1);
			}

			if (_components[id2].groupID==-1)
			{
				newPosFound = findNewPosition(id2, //label id
					newStartPoint2, //new start point
					dirType2,
					minDist); //min distance from start point

				if (newPosFound)
				{
					//Calc new connection line
					kBasics::POINT labelPoint = _components[id2].labelPoint;
					labelPoint.x+=_borderDistanceFld->getIntValue()/_optFactor;
					labelPoint.y+=_borderDistanceFld->getIntValue()/_optFactor;
					calcLabelLine(_components[id2].anchorPoint, labelPoint, _components[id2].textSize, _components[id2].lineEndPoint);
				}
				//Delete old line from cache
				deleteLinesFromCache(id2);
				//Add new lines to cache
				addLabelLinesToCache(id2);
			}
		}
		//iterations++;
		//if (iterations>50) break;
		//std::cout << "iterations=" << iterations << std::endl;
	}	
	return _labelConflicts.size();
}

bool METK2DLabeling::findNewPosition(int compID, kBasics::POINT searchPoint, int dirType, int minDist)
{
	if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "new position for " << compID << "  dirType=" << dirType << std::endl;

	int searchDist=minDist; //Mindestens eine andere Position nicht die gleiche sonst kann ewig hin und her geswitcht werden
	int maxSearchDist=512;

	if (dirType==DIRTYPE_UPRIGHT)
	{		
		while ((searchPoint.x+searchDist<_sliceSizeOpt.x || searchPoint.y-searchDist>0) && searchDist<maxSearchDist)
		{
			//horizontal
			if (searchPoint.y-searchDist>0)
			{
				for (int i=searchPoint.x; i<min(_sliceSizeOpt.x,searchPoint.x+searchDist); i++) //to the right
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y-searchDist)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y-searchDist;
						return true;
					}
				}
			}

			//vertical
			if (searchPoint.x+searchDist<_sliceSizeOpt.x)
			{
				for (int i=searchPoint.y; i>max(0,searchPoint.y-searchDist); i--) //to the top
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x+searchDist,i)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=searchPoint.x+searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}
			searchDist++;
		} //while
	}


	else if (dirType==DIRTYPE_UPLEFT)
	{		
		while ((searchPoint.x-searchDist>0 || searchPoint.y-searchDist>0) && searchDist<maxSearchDist)
		{
			//horizontal
			if (searchPoint.y-searchDist>0)
			{
				for (int i=searchPoint.x; i>max(0,searchPoint.x-searchDist); i--) //to the left
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y-searchDist)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y-searchDist;
						return true;
					}
				}
			}

			//vertical
			if (searchPoint.x-searchDist>0)
			{
				for (int i=searchPoint.y; i>max(0,searchPoint.y-searchDist); i--) //to the top
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x-searchDist,i)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=searchPoint.x-searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}
			searchDist++;
		} //while
	}


	else if (dirType==DIRTYPE_DOWNRIGHT)
	{		
		while ((searchPoint.x+searchDist<_sliceSizeOpt.x || searchPoint.y+searchDist<_sliceSizeOpt.y) && searchDist<maxSearchDist)
		{
			//horizontal
			if (searchPoint.y+searchDist<_sliceSizeOpt.y)
			{
				for (int i=searchPoint.x; i<min(_sliceSizeOpt.x,searchPoint.x+searchDist); i++) //to the right
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y+searchDist)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y+searchDist;
						return true;
					}
				}
			}

			//vertical
			if (searchPoint.x+searchDist<_sliceSizeOpt.x)
			{
				for (int i=searchPoint.y; i<min(_sliceSizeOpt.y,searchPoint.y+searchDist); i++) //to the bottom
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x+searchDist,i)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=searchPoint.x+searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}
			searchDist++;
		} //while
	}



	else if (dirType==DIRTYPE_DOWNLEFT)
	{
		while ((searchPoint.x-searchDist>0 || searchPoint.y+searchDist<_sliceSizeOpt.y) && searchDist<maxSearchDist)
		{
			//horizontal
			if (searchPoint.y+searchDist<_sliceSizeOpt.y)
			{
				for (int i=searchPoint.x; i>max(0,searchPoint.x-searchDist); i--) //to the left
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y+searchDist)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y+searchDist;
						return true;
					}
				}
			}

			//vertical
			if (searchPoint.x-searchDist>0)
			{
				for (int i=searchPoint.y; i<min(_sliceSizeOpt.y,searchPoint.y+searchDist); i++) //to the bottom
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x-searchDist,i)==1)
					{
						//stop=true;
						_components[compID].labelPoint.x=searchPoint.x-searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}
			searchDist++;
		}  //while
	}


	else if (dirType==DIRTYPE_UP)
	{		
		while ((searchPoint.x+searchDist<_sliceSizeOpt.x || searchPoint.x-searchDist>0 || searchPoint.y-searchDist>=0) && searchDist<maxSearchDist)
		{
			//horizontal
			if (searchPoint.y-searchDist>0)
			{
				for (int i=max(1,searchPoint.x-searchDist); i<min(_sliceSizeOpt.x,searchPoint.x+searchDist); i++) //to the right
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y-searchDist)==1)
					{						
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y-searchDist;
						return true;
					}
				}
			}

			//right vertical
			if (searchPoint.x+searchDist<_sliceSizeOpt.x)
			{
				for (int i=searchPoint.y; i>max(0,searchPoint.y-searchDist); i--) //to the top
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x+searchDist,i)==1)
					{
						_components[compID].labelPoint.x=searchPoint.x+searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}

			//left vertical
			if (searchPoint.x-searchDist>0)
			{
				for (int i=searchPoint.y; i>max(0,searchPoint.y-searchDist); i--) //to the top
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x-searchDist,i)==1)
					{
						_components[compID].labelPoint.x=searchPoint.x-searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}

			searchDist++;
		} //while
	}
	else if (dirType==DIRTYPE_DOWN)
	{		
		while ((searchPoint.x+searchDist<_sliceSizeOpt.x || searchPoint.x-searchDist>0 || searchPoint.y+searchDist<_sliceSizeOpt.y) && searchDist<maxSearchDist)
		{
			//horizontal
			if (searchPoint.y+searchDist<_sliceSizeOpt.y)
			{
				for (int i=max(1,searchPoint.x-searchDist); i<min(_sliceSizeOpt.x,searchPoint.x+searchDist); i++) //to the right
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y+searchDist)==1)
					{						
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y+searchDist;
						return true;
					}
				}
			}

			//right vertical
			if (searchPoint.x+searchDist<_sliceSizeOpt.x)
			{
				for (int i=searchPoint.y; i<min(searchPoint.y+searchDist,_sliceSizeOpt.y); i++) //to the bottom
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x+searchDist,i)==1)
					{
						_components[compID].labelPoint.x=searchPoint.x+searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}

			//left vertical
			if (searchPoint.x-searchDist>0)
			{
				for (int i=searchPoint.y; i<min(searchPoint.y+searchDist,_sliceSizeOpt.y); i++) //to the bottom
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x-searchDist,i)==1)
					{
						_components[compID].labelPoint.x=searchPoint.x-searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}

			searchDist++;
		} //while
	}

	else if (dirType==DIRTYPE_LEFT)
	{
		while ((searchPoint.x-searchDist>0 || searchPoint.y-searchDist>0 || searchPoint.y+searchDist<_sliceSizeOpt.y) && searchDist<maxSearchDist)
		{
			//first horizontal
			if (searchPoint.y+searchDist<_sliceSizeOpt.y)
			{
				for (int i=searchPoint.x; i>max(0,searchPoint.x-searchDist); i--) //to the left
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y+searchDist)==1)
					{
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y+searchDist;
						return true;
					}
				}
			}

			//second horizontal
			if (searchPoint.y-searchDist>0)
			{
				for (int i=searchPoint.x; i>max(0,searchPoint.x-searchDist); i--) //to the left
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y-searchDist)==1)
					{
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y-searchDist;
						return true;
					}
				}
			}

			//vertical
			if (searchPoint.x-searchDist>0)
			{
				for (int i=max(searchPoint.y-searchDist,1); i<min(_sliceSizeOpt.y,searchPoint.y+searchDist); i++) //to the bottom
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x-searchDist,i)==1)
					{
						_components[compID].labelPoint.x=searchPoint.x-searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}
			searchDist++;
		}  //while
	}

	else if (dirType==DIRTYPE_RIGHT)
	{
		while ((searchPoint.x+searchDist<_sliceSizeOpt.x || searchPoint.y-searchDist>0 || searchPoint.y+searchDist<_sliceSizeOpt.y) && searchDist<maxSearchDist)
		{
			//first horizontal
			if (searchPoint.y+searchDist<_sliceSizeOpt.y)
			{
				for (int i=searchPoint.x; i<min(searchPoint.x+searchDist,_sliceSizeOpt.x); i++) //to the right
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y+searchDist)==1)
					{
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y+searchDist;
						return true;
					}
				}
			}

			//second horizontal
			if (searchPoint.y-searchDist>0)
			{
				for (int i=searchPoint.x; i<min(searchPoint.x+searchDist,_sliceSizeOpt.x); i++) //to the right
				{
					if (_components[compID].freeSpaceField->getValueAt(i,searchPoint.y-searchDist)==1)
					{
						_components[compID].labelPoint.x=i;
						_components[compID].labelPoint.y=searchPoint.y-searchDist;
						return true;
					}
				}
			}

			//vertical
			if (searchPoint.x+searchDist<_sliceSizeOpt.x)
			{
				for (int i=max(searchPoint.y-searchDist,1); i<min(_sliceSizeOpt.y,searchPoint.y+searchDist); i++) //to the bottom
				{
					if (_components[compID].freeSpaceField->getValueAt(searchPoint.x+searchDist,i)==1)
					{
						_components[compID].labelPoint.x=searchPoint.x+searchDist;
						_components[compID].labelPoint.y=i;
						return true;
					}
				}
			}
			searchDist++;
		}  //while
	}
	return false;
}


//Calc line end point
void METK2DLabeling::calcLabelLine(kBasics::POINT anchorPoint, kBasics::POINT labelPoint, kBasics::POINT labelSize, kBasics::POINT &lineEndPoint)
{
	int tempDist, minDist;
	lineEndPoint = labelPoint;

	minDist = abs(anchorPoint.x - labelPoint.x)+
		abs(anchorPoint.y - labelPoint.y);

	tempDist = abs(anchorPoint.x - (labelPoint.x+labelSize.x))+
		abs(anchorPoint.y - labelPoint.y);
	if (tempDist<minDist)
	{
		minDist=tempDist;
		lineEndPoint.x = labelPoint.x+labelSize.x;
	}

	tempDist = abs(anchorPoint.x - labelPoint.x)+
		abs(anchorPoint.y - (labelPoint.y+labelSize.y));
	if (tempDist<minDist)
	{
		minDist=tempDist;
		lineEndPoint.y = labelPoint.y+labelSize.y;
	}

	tempDist = abs(anchorPoint.x - (labelPoint.x+labelSize.x))+
		abs(anchorPoint.y - (labelPoint.y+labelSize.y));
	if (tempDist<minDist)
	{
		minDist=tempDist;
		lineEndPoint.x = labelPoint.x+labelSize.x;
		lineEndPoint.y = labelPoint.y+labelSize.y;
	}
}


//Delete Label line to _labelLines
void METK2DLabeling::deleteLinesFromCache(int id)
{
	std::list<LINECOMPONENT>::iterator iter=_labelLines.begin();
	while (iter!=_labelLines.end())
	{					
		if (iter->componentID==id)
		{
			_labelLines.erase(iter);
			iter=_labelLines.begin();
		}
		else
			iter++;
	}

	std::list<BOXCOMPONENT>::iterator iter2=_labelBoxes.begin();
	while (iter2!=_labelBoxes.end())
	{					
		if (iter2->componentID==id)
		{
			_labelBoxes.erase(iter2);
			iter2=_labelBoxes.begin();
		}
		else
			iter2++;
	}
}


//Add Label line to _labelLines
void METK2DLabeling::addLabelLinesToCache(int id)
{	
	LINECOMPONENT tempLine;
	tempLine.line.p1 = _components[id].lineEndPoint;
	tempLine.line.p2 = _components[id].anchorPoint;
	tempLine.lineType = LT_CONNECTIONLINE;
	tempLine.componentID = id;
	checkForConflicts(tempLine);
	_labelLines.push_back(tempLine);

	tempLine.lineType = LT_TEXTBB;
	tempLine.line.p1 = _components[id].labelPoint;
	tempLine.line.p2.x = _components[id].labelPoint.x;
	tempLine.line.p2.y = _components[id].labelPoint.y+_components[id].textSize.y;
	checkForConflicts(tempLine);
	_labelLines.push_back(tempLine);

	tempLine.line.p2.x = _components[id].labelPoint.x+_components[id].textSize.x;
	tempLine.line.p2.y = _components[id].labelPoint.y;
	checkForConflicts(tempLine);
	_labelLines.push_back(tempLine);

	tempLine.line.p1.x = _components[id].labelPoint.x+_components[id].textSize.x;
	tempLine.line.p1.y = _components[id].labelPoint.y;
	tempLine.line.p2.x = _components[id].labelPoint.x+_components[id].textSize.x;
	tempLine.line.p2.y = _components[id].labelPoint.y+_components[id].textSize.y;
	checkForConflicts(tempLine);
	_labelLines.push_back(tempLine);

	tempLine.line.p1.x = _components[id].labelPoint.x;
	tempLine.line.p1.y = _components[id].labelPoint.y+_components[id].textSize.y;
	tempLine.line.p2.x = _components[id].labelPoint.x+_components[id].textSize.x;
	tempLine.line.p2.y = _components[id].labelPoint.y+_components[id].textSize.y;
	checkForConflicts(tempLine);
	_labelLines.push_back(tempLine);

	BOXCOMPONENT tempBox;
	/*tempBox.corner1 = _components[id].labelPoint;
	tempBox.corner2.x = tempBox.corner1.x + _components[id].textSize.x;
	tempBox.corner2.y = tempBox.corner1.y + _components[id].textSize.y;*/
	tempBox.corner1.x = _components[id].labelPoint.x-_borderDistanceFld->getIntValue()/_optFactor;
	tempBox.corner1.y = _components[id].labelPoint.y-_borderDistanceFld->getIntValue()/_optFactor;
	tempBox.corner2.x = tempBox.corner1.x + _components[id].textSize.x + _borderDistanceFld->getIntValue()/_optFactor;
	tempBox.corner2.y = tempBox.corner1.y + _components[id].textSize.y + _borderDistanceFld->getIntValue()/_optFactor;
	tempBox.componentID = id;
	checkForConflicts(tempBox);
	_labelBoxes.push_back(tempBox);
}



void METK2DLabeling::checkForConflicts(LINECOMPONENT tempLine)
{
	//Check for intersections of this line
	std::list<LINECOMPONENT>::const_iterator iter=_labelLines.begin();
	kBasics::POINT intersectionPoint;
	while (iter!=_labelLines.end())
	{
		if (tempLine.componentID!=iter->componentID)
		{
			if (kBasics::lineIntersection(tempLine.line.p1,tempLine.line.p2,
				iter->line.p1,iter->line.p2,intersectionPoint))
			{				
				LABELCONFLICT tempConflict;
				tempConflict.componentID1 = tempLine.componentID;
				tempConflict.componentID2 = iter->componentID;
				if (iter->lineType==LT_CONNECTIONLINE && tempLine.lineType==LT_CONNECTIONLINE)
				{
					tempConflict.intersectionType = IT_CL_CL;
					if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "Line-Line Intersection found for component " << tempLine.componentID << " with component " << iter->componentID << "  intersectionType=" << tempConflict.intersectionType << std::endl;
					_labelConflicts.push_back(tempConflict);
				}
				else if (iter->lineType==LT_CONNECTIONLINE || tempLine.lineType==LT_CONNECTIONLINE)
				{
					tempConflict.intersectionType = IT_CL_TEXT;
					//std::cout << "Line-Box Intersection found for component " << tempLine.componentID << " with component " << iter->componentID << "  intersectionType=" << tempConflict.intersectionType << std::endl;
					_labelConflicts.push_back(tempConflict);
				}				
			}
		}
		iter++;
	}
}


void METK2DLabeling::checkForConflicts(BOXCOMPONENT tempBox)
{
	//Check for intersections and overlapping of boxes
	std::list<BOXCOMPONENT>::const_iterator iter=_labelBoxes.begin();
	while (iter!=_labelBoxes.end())
	{
		kBasics::BOX intersectionBox;
		if (tempBox.componentID!=iter->componentID)
		{
			if (kBasics::boxIntersection(tempBox.corner1,tempBox.corner2,
				iter->corner1,iter->corner2,intersectionBox))
			{
				if (_debugStateFld->getEnumValue()==DSTATE_ALL) std::cout << "Box-Box Intersection found for component " << tempBox.componentID << " with component " << iter->componentID << std::endl;
				LABELCONFLICT tempConflict;
				tempConflict.componentID1 = tempBox.componentID;
				tempConflict.componentID2 = iter->componentID;
				tempConflict.intersectionType = IT_TEXT_TEXT;
				_labelConflicts.push_back(tempConflict);
			}
		}
		iter++;
	}
}


void METK2DLabeling::timerEvent(void* data, SoDataSensor* /*a*/)
{	
	METK2DLabeling* caller = (METK2DLabeling*)data;
	if (!caller) return;
	if (!caller->timerSensor) return;
	caller->timerSensor->unschedule();
	if (caller->_currentSlice==caller->_sliceFld->getIntValue())
	{
		caller->initialPlacement();
		caller->refineAll();
		caller->transferLabelsToInventor();
	}
	else
		caller->timerSensor->schedule();
}


void METK2DLabeling::buildObjIDList()
{
	string tempIDs;
	bool tempBool;
	_htObjIDTransp.getTable()->clear();

	_voxelValObjID.clear();	
	_voxelValObjID.resize(12000); //DAS IST GANZ BÖSE! EIGENTLICH MÜSSTE MAN HIER EINEN TEST AUF DEN MAXWERT MACHEN UND DANN DAS SO RESIZEN

	myObjIterator->layerIDsFld->setStringValue(LAY_IMAGE);
	myObjIterator->infoIDsFld->setStringValue(INF_CODSEGOBJECTVALUE);
	myObjIterator->useInfoValueFld->setBoolValue(false);	

	myObjIterator->firstObjFld->notify();
	while (myObjIterator->opSuccessfulFld->getBoolValue())
	{	
		//Zusätzlich später noch:
		//if (myObjMgr->getObjAttributeBool(myObjIterator->objectIDFld->getStringValue(),LAY_APPEARANCE,INF_LABELING))
		string currentObjID = myObjIterator->objectIDFld->getStringValue();
		myObjMgr->getObjAttributeBool(currentObjID,LAY_APPEARANCE,INF_VISIBLE,tempBool);
		float currentTransp;
		myObjMgr->getObjAttributeFloat(currentObjID,LAY_APPEARANCE,INF_TRANSPARENCY,currentTransp);
		_htObjIDTransp.insert(currentObjID,currentTransp);
		if (tempBool)
		{
			myObjMgr->getObjAttributeString(myObjIterator->objectIDFld->getStringValue(),LAY_IMAGE,INF_CODSEGOBJECTVALUE,tempIDs);
			vector<string> id_split;
			kBasics::split(tempIDs,',',INT_MAX,&id_split);
			string tempID;
			for (int i=0; i<id_split.size(); i++)
			{
				if (id_split[i]!="")
				{
					tempID = _voxelValObjID[kBasics::StringToInt(id_split[i])];
					if (tempID=="")
					{
						_voxelValObjID[kBasics::StringToInt(id_split[i])]=currentObjID;
					}
					else
					{
						float tempTransp;
						myObjMgr->getObjAttributeFloat(tempID,LAY_APPEARANCE,INF_TRANSPARENCY,tempTransp);
						if (tempTransp>currentTransp)
						{
							_voxelValObjID[kBasics::StringToInt(id_split[i])]=currentObjID;
						}
					}
				}
			}
		}
		myObjIterator->nextObjFld->notify();
	}	
}



//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void METK2DLabeling::fieldChangedCB(void *data, SoSensor* sens)
{
	((METK2DLabeling*) data)->fieldChanged((SoFieldSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void METK2DLabeling::fieldChanged(SoFieldSensor* sensor)
{
	// Get the field which caused the notification.
	SoField* field = sensor->getTriggerField();

	if (field == _2dLabelsNode->getField("_CSOFld"))
	{
		cout << "CSO Changed" << endl;
		_CSOFld->setStringValue(((SoSFString*)_2dLabelsNode->getField("_CSOFld"))->getValue().getString());
		if (_sendCSOFld->getBoolValue())
		{
			//send it to ObjMgr
			cout << "send CSO to ObjMgr ..." << endl;
			myObjMgr->setObjAttribute(OBJ_CSO,LAY_SELECTEDOBJ,INF_OBJID,new string(_CSOFld->getStringValue()),omINFOTYPE_STRING, true, true);
			myObjMgr->sendNotification();
		}
	}
}


ML_END_NAMESPACE