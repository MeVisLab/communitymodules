//----------------------------------------------------------------------------------
//! The ML module class CalcCodedSegmentation.
/*!
// \file    mlCalcCodedSegmentation.cpp
// \author  Konrad Mühler
// \date    2007-01-16
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlCalcCodedSegmentation_H
#include "mlCalcCodedSegmentation.h"
#endif

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(CalcCodedSegmentation, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CalcCodedSegmentation::CalcCodedSegmentation (void) : BaseOp(1, 1)
{
  ML_TRACE_IN("CalcCodedSegmentation::CalcCodedSegmentation()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  MAX_SIZE = SHRT_MAX; //32767

  _fld_Add = fields->addNotify("Add");  
  (_fld_NameToAdd = fields->addString("NameToAdd"))->setStringValue("");
  (_fld_addMinValue = fields->addInt("addMinValue"))->setIntValue(1);
  (_fld_addMaxValue = fields->addInt("addMaxValue"))->setIntValue(ML_INT_MAX);
  (_fld_addAllExceptNull = fields->addBool("addAllExceptNull"))->setBoolValue(false);
  _fld_Reset = fields->addNotify("Reset");
  _fld_Purge = fields->addNotify("Purge");
  _fld_Finish = fields->addNotify("Finish");
  (_fld_ImageValues = fields->addString("ImageValues"))->setStringValue("");
  (_fld_ObjectValues = fields->addString("ObjectValues"))->setStringValue("");

  //All pointer values need to be set NULL here, before reset, otherwise, default random values of pointe can be interpreted as real values and a non existing pointer is tried to delete ... uuhhhh
  _virtualVolume = NULL;
  replaceValues = NULL;
  objectValues = NULL;
  valuesForObjects = NULL;
  objectNames = NULL;

  reset();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


void CalcCodedSegmentation::reset()
{
	nextFreeValue = 1;
	nextStart = 1;
	replaceValues_MaxIndex = 0;

	/*if (_virtualVolume) 
	{ 
		delete _virtualVolume; 
		_virtualVolume = NULL;
	}*/

	if (replaceValues)
	{
		delete replaceValues;
		replaceValues = NULL;
	}
	replaceValues = new vector<MLint64>;

	if (objectValues)
	{
		delete objectValues;
		objectValues = NULL;
	}
	objectValues = new vector<string>;
	objectValues->resize(MAX_SIZE,"");

	if (valuesForObjects)
	{
		delete valuesForObjects;
		valuesForObjects = NULL;
	}
	valuesForObjects = new HashTable<string>;

	if (objectNames)
	{
		delete objectNames;
		objectNames = NULL;
	}
	objectNames = new vector<string>;
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void CalcCodedSegmentation::handleNotification (Field *field)
{
	ML_TRACE_IN("CalcCodedSegmentation::handleNotification()")
	
    if (field == _fld_Add)
	{
		replaceValues->clear();
		replaceValues_MaxIndex = 0;		
		replaceValues->resize(MAX_SIZE,0);		
		addImage();		
		renewObjectValues();		
		int error = processAllPages(0);
		if (error!=0) std::cout << "processAllPages Error (add) = " << MLGetErrorCodeDescription(error) << std::endl;
    } 
	else if (field == _fld_Reset)
	{
		//std::cout << "_fld_Reset" << std::endl;
		calcOutImageProps(0);		
		reset();		
		clearOutputImage();	
		getOutImg(0)->setOutOfDate();
		
		int error = processAllPages(0);
		if (error!=0) std::cout << "processAllPages Error (reset) = " << MLGetErrorCodeDescription(error) << std::endl; //Das mit dem invalidieren per setOutOfDate geht nicht
		
		_fld_ObjectValues->setStringValue("");
		_fld_ImageValues->setStringValue("");		
	}
	else if (field == _fld_Purge)
	{
		purgeValues();
	}
}


//----------------------------------------------------------------------------------
//! Sets properties of the output image at output \c outIndex.
//----------------------------------------------------------------------------------
void CalcCodedSegmentation::calcOutImageProps (int outIndex)
{
	ML_TRACE_IN("CalcCodedSegmentation::calcOutImageProps ()")

	// ... set image properties for output image outIndex

	if (!getUpdatedInImg(0))
	{
		std::cout << "calcOutImageProps ... getUpdatedInImg(0)=NULL ... return" << std::endl;
		return;
	}

	if (getUpdatedInImg(0)==NULL)
	{
		std::cout << "calcOutImageProps ... getUpdatedInImg(0)=NULL ... return" << std::endl;	
		return;
	}

	//getOutImg(0)->setImgProps(*getUpdatedInImg(0));
	//getOutImg(0)->setDataType(getUpdatedInImg(0)->getDataType());
	getOutImg(0)->setDataType(MLint32Type);
	getOutImg(0)->setImgExt(getUpdatedInImg(0)->getImgExt());
	getOutImg(0)->setPageExt(getUpdatedInImg(0)->getPageExt());	


	if (getUpdatedInImg(0))
	{
		if (_virtualVolume) 
		{ 
			//delete _virtualVolume; 
			//std::cout << "destroyTypedVirtualVolume" << std::endl;
			VirtualVolume::destroyTypedVirtualVolume(_virtualVolume,MLint64Type);
			_virtualVolume = NULL;
		}

		//_virtualVolume = new VirtualVolume(getUpdatedInImg(0)->getImgExt(), 0, getUpdatedInImg(0)->getDataType(), -1, true); //evtl. auch false als letzter parameter???
		
		//! Als Datentyp wird das Größte genommen, da es vorgekommen ist, dass nicht alle Masken zu einer ROI den gleichen Datentyp wie die erste Maske hatten und dann kracht's
		std::cout << "Create new VirtualVolume for CalcCodedSegmentation" << std::endl;
		_virtualVolume = new VirtualVolume(getUpdatedInImg(0)->getImgExt(), 0, MLint64Type, -1, true); //evtl. auch false als letzter parameter???		
	}
	else
	{
		_virtualVolume = NULL;
	}

	if (!_virtualVolume || (_virtualVolume && !_virtualVolume->isValid()))
	{
		getOutImg(outIndex)->setOutOfDate();
		return;
	}
}


//----------------------------------------------------------------------------------
//! Returns the input image region required to calculate a region of an output image.
//----------------------------------------------------------------------------------
SubImgBox CalcCodedSegmentation::calcInSubImageBox (int inIndex, const SubImgBox &outSubImgBox, int outIndex)
{
  ML_TRACE_IN("CalcCodedSegmentation::calcInSubImageBox ()")

  // ... return region of input image inIndex needed to compute region outSubImgBox of
  //     output image outIndex

  // replace this with your own SubImgBox:
  return outSubImgBox;
}



//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of \c calcOutSubImage to calculate page 
//! \c outSubImg of output image with index \c outSubImg.
//----------------------------------------------------------------------------------
//CALC_OUTSUBIMAGE_CPP(CalcCodedSegmentation);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by 
//! CALC_OUTSUBIMAGE2_CPP(CalcCodedSegmentation).
//----------------------------------------------------------------------------------
void CalcCodedSegmentation::calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs)
{	
	ML_TRACE_IN("template <typename T> CalcCodedSegmentation::calcOutSubImage ()")

	// ... compute subimage of output image outIndex from subimages of all input images	

	if (!_virtualVolume)
	{
		std::cout << "calcOutSubImage ... _virtualVolume = NULL" << std::endl;
		return;
	}
	
	//TVirtualVolume<T> vVol(*_virtualVolume);  // Create wrapper for typed voxel access.
	_virtualVolume->copyToSubImg(*outSubImg);
}


void CalcCodedSegmentation::addImage()
{
	if (!_virtualVolume)
	{
		std::cout << "_virtualVolume==NULL" << std::endl;
		return;
	}

	MLint64 xSize, ySize, zSize;

	if (getUpdatedInImg(0)==NULL)
	{
		std::cout << "getUpdatedInImg(0)==NULL ... return" << std::endl;
		return;
	}
		
	//SubImgBox box = getUpdatedInImg(0)->getBoxFromImgExt();
	xSize = getUpdatedInImg(0)->getBoxFromImgExt().getExt()[0]; ySize = getUpdatedInImg(0)->getBoxFromImgExt().getExt()[1]; zSize = getUpdatedInImg(0)->getBoxFromImgExt().getExt()[2];

	//if (!_computeBoundingBox())
	//{
		b1x=b1y=b1z=0;
		b2x=xSize;
		b2y=ySize;
		b2z=zSize;
	//}

	//std::cout << "b1x: " << b1x << " b1y: " << b1y << " b1z: " << b1z << " b2x: " << b2x << " b2y: " << b2y << " b2z: " << b2z << std::endl;
		
	MLDataType type = getUpdatedInImg(0)->getDataType();

	//PagedImg toAdd = *getInImg(1);		
	SubImg mySubImg;
	mySubImg.setBox(getUpdatedInImg(0)->getBoxFromImgExt());
	mySubImg.setDataType(type);	
	int error = getUpdatedInImg(0)->getTile(mySubImg);	
	if (error!=0)
	{
		std::cout << "getUpdatedInImg(0)->getTile Error (addImage) = " << MLGetErrorCodeDescription(error) << std::endl;
		return;
	}
	
	TVirtualVolume<MLint64> vVol(*_virtualVolume);  // Create wrapper for typed voxel access.
	MLint64 tempINVoxelValue;
	MLint64 tempOUTVoxelValue;

	nextStart = nextFreeValue; //für das nächste Ändern der objectValues später dann müssen erst Werte ab dem aktuellen nextFreeValue angefast werden. Davor finden keine Veränderungen statt. Es können höchstens bei einer Struktur neue Werte hinzukommen.
	objectNames->push_back(_fld_NameToAdd->getStringValue());	

	void* voidValue;
	for (MLint64 i=b1z; i<b2z; i++)
	{
		for (MLint64 j=b1y; j<b2y; j++)
		{
			for (MLint64 k=b1x; k<b2x; k++)
			{
				//tempINVoxelValue = 0;
				//try {
					voidValue = mySubImg.getVoidSubImgPos(k,j,i);
				//}
				//catch( ... )
				//{
				//	voidValue = NULL;
				//	std::cout << "EXCEPTION-B   k/x=" << k << " j/y=" << j << " i/z=" << i << std::endl;
				//}

				//if (mySubImg.getVoidSubImgPos(k,j,i)!=NULL)
				if (voidValue!=NULL)
				{
					switch (type)
					{
						case MLint8Type: 
							tempINVoxelValue = *(MLint8*)(voidValue);
							break;
						case MLuint8Type:
							tempINVoxelValue = *(MLuint8*)(voidValue);
							break;
						case MLint16Type:
							tempINVoxelValue = *(MLint16*)(voidValue);					
							break;
						case MLuint16Type:									
							tempINVoxelValue = *(MLuint16*)(voidValue);						
							break;
						case MLint32Type:
							tempINVoxelValue = *(MLint32*)(voidValue);
							break;
						case MLuint32Type:
							tempINVoxelValue = *(MLuint32*)(voidValue);
							break;
						case MLfloatType:
							tempINVoxelValue = *(MLfloat*)(voidValue);
							break;
						case MLdoubleType:
							tempINVoxelValue = *(MLdouble*)(voidValue);
							break;
						case MLldoubleType:
							tempINVoxelValue = *(MLldouble*)(voidValue);
							break;
						case MLint64Type:
							tempINVoxelValue = *(MLint64*)(voidValue);
							break;
						default:
							tempINVoxelValue = 0;
							std::cout << "Unsupported type of input image to add." << std::endl;						
					}
				}
				else
				{
					tempINVoxelValue = 0;
					std::cout << "mySubImg.getVoidSubImgPos(k,j,i)=NULL" << std::endl;
				}
								
				if (tempINVoxelValue!=0)				
				{										
					if (   (!_fld_addAllExceptNull->getBoolValue() && tempINVoxelValue>=_fld_addMinValue->getIntValue() && tempINVoxelValue<=_fld_addMaxValue->getIntValue())
						|| (_fld_addAllExceptNull->getBoolValue() && tempINVoxelValue>0) )
					{	
						try {
							tempOUTVoxelValue = vVol.getVoxel(Vector(k, j, i, 0, 0, 0));
						}
						catch ( ... )
						{						
							std::cout << "EXCEPTION-C   k/x=" << k << " j/y=" << j << " i/z=" << i << std::endl;
						}
						if (tempOUTVoxelValue<(MLint64)replaceValues->size())
						{
							MLint64 tempIndex = tempOUTVoxelValue;						
							if ((*replaceValues)[tempIndex]==0)
							{					
								(*replaceValues)[tempIndex]=nextFreeValue;
								nextFreeValue++;
								//std::cout << "nextFreeValue++  = " << nextFreeValue  << "    tempIndex=" << tempIndex << std::endl;
								if (tempIndex>replaceValues_MaxIndex) replaceValues_MaxIndex=tempIndex;							
							}						
							vVol.setVoxel(Vector(k, j, i, 0, 0, 0), (*replaceValues)[tempIndex]);						
						}
						else
						{							
							std::cout << "VoxelValue larger than vector.size()" << std::endl;
						}
					}	
				}
			}
		}
	}	
	//!!
	//Diese Zeile ist lebenswichtig, weil er sonst nach mehreren Bildern mit einem MemoryAllocationFailed abstürzt
	getUpdatedInImg(0)->freeTile(mySubImg.getData());

	//Set new Max Value for OutputImage:
	getOutImg(0)->setMaxVoxelValue(nextFreeValue-1);
}


void CalcCodedSegmentation::clearOutputImage()
{
	//std::cout << "clearOutputImage" << std::endl;
	if (!_virtualVolume)
	{
		std::cout << "clearOutputImage  ... _virtualVolume=NULL" << std::endl;
		return;
	}
	int xSize, ySize, zSize;
	
	SubImgBox box = _virtualVolume->getBox();
	xSize = box.getExt()[0]; ySize = box.getExt()[1]; zSize = box.getExt()[2];

	TVirtualVolume<MLint64> vVol(*_virtualVolume);  // Create wrapper for typed voxel access.
	short value = 0;
	for(int i=0; i<zSize; i++)
	{		
		for(int j=0; j<ySize; j++)
		{
			for(int k=0; k<xSize; k++)
			{
				Vector pos(k, j, i, 0, 0, 0);
				vVol.setVoxel(pos, value);
			}
		}
	}
}



void CalcCodedSegmentation::renewObjectValues()
{
	MLint64 objectValues_MaxIndex = 0;
	int i=0;

	for (i=0; i<=replaceValues_MaxIndex; i++)
	{
		if ((*replaceValues)[i]!=0 && (*replaceValues)[i]<(MLint64)objectValues->size() && i<(int)objectValues->size())
		{			
			(*objectValues)[(*replaceValues)[i]] = _fld_NameToAdd->getStringValue();
			if ((*objectValues)[i]!="") (*objectValues)[(*replaceValues)[i]]=(*objectValues)[(*replaceValues)[i]] + ","+(*objectValues)[i];
			if ((*replaceValues)[i]>objectValues_MaxIndex) objectValues_MaxIndex=(*replaceValues)[i];
		}
	}

	vector<string> curObjects;
	for (i=nextStart; i<=objectValues_MaxIndex; i++)
	{
		if (_fld_ImageValues->getStringValue()!="") _fld_ImageValues->setStringValue(_fld_ImageValues->getStringValue()+";");
		_fld_ImageValues->setStringValue(_fld_ImageValues->getStringValue()+kBasics::IntToString(i)+":"+(*objectValues)[i]);
		kBasics::split((*objectValues)[i], ',', -1, &curObjects);
		for (int j=0; j<(int)curObjects.size(); j++)
		{			
			if (curObjects[j]!="")
			{
				if (valuesForObjects->find(curObjects[j]))
				{					
					*(valuesForObjects->find(curObjects[j])) = *(valuesForObjects->find(curObjects[j]))+","+kBasics::IntToString(i);					
				}
				else
				{					
					valuesForObjects->insert(curObjects[j],kBasics::IntToString(i));				
				}										
			}	
		}
	}
	buildString();
}



void CalcCodedSegmentation::buildString()
{
	string finalString = "";
	for (int i=0; i<(int)objectNames->size(); i++)
	{
		finalString = finalString + (*objectNames)[i] + ":";
		if (valuesForObjects->find((*objectNames)[i]))
		{
			finalString = finalString + *(valuesForObjects->find((*objectNames)[i]));
		}		
		finalString = finalString + ";";
	}
	_fld_ObjectValues->setStringValue(finalString);
}


void CalcCodedSegmentation::purgeValues()
{
	int xSize, ySize, zSize;
	if (_virtualVolume==NULL) return;
	if (getUpdatedInImg(0)==NULL) return;

	TVirtualVolume<MLint64> vVol(*_virtualVolume);  // Create wrapper for typed voxel access.
	SubImgBox box = getUpdatedInImg(0)->getBoxFromImgExt();
	xSize = box.getExt()[0]; ySize = box.getExt()[1]; zSize = box.getExt()[2];
		
	MLint64 voxelValue;
	vector<bool>* existingValues;
	existingValues = new vector<bool>;
	existingValues->resize(MAX_SIZE,false);
	MLint64 maxValue = 0;
	int i=0;
	
	for (i = 0; i<zSize; i++)
	{
		for (int j=0; j<ySize; j++)
		{
			for (int k=0; k<xSize; k++)
			{
				Vector pos(k, j, i, 0, 0, 0);
				voxelValue = vVol.getVoxel(pos);
				(*existingValues)[voxelValue] = true;
				if (voxelValue>maxValue) maxValue = voxelValue;
			}
		}
	}	

	vector<string> singleElements;
	string outputStr;
	for (i=0; i<(int)objectNames->size(); i++)
	{		
		if (valuesForObjects->find((*objectNames)[i]))
		{
			outputStr = "";
			kBasics::split(*(valuesForObjects->find((*objectNames)[i])), ',', -1, &singleElements);
			for (int j=0; j<(int)singleElements.size(); j++)
			{
				if (singleElements[j]!="")
				{
					if ((*existingValues)[kBasics::StringToInt(singleElements[j])])
					{
						if (outputStr!="") outputStr = outputStr + ",";
						outputStr = outputStr + singleElements[j];
					}
				}
			}
			*(valuesForObjects->find((*objectNames)[i])) = outputStr;
		}
	}


	kBasics::split(_fld_ImageValues->getStringValue(), ';', -1, &singleElements);
	vector<string> singleElements2;
	outputStr = "";
	for (i=0; i<(int)singleElements.size(); i++)
	{
		if (singleElements[i]!="")
		{
			kBasics::split(singleElements[i], ':', 2, &singleElements2);
			if (singleElements2[0]!="")
			{
				if ((*existingValues)[kBasics::StringToInt(singleElements2[0])])
				{
					outputStr = outputStr + singleElements[i] + ";";	
				}
			}
		}
	}
	_fld_ImageValues->setStringValue(outputStr);
	buildString();
}



// -- BOUNDINGBOX CODE VON WOLF SPINDLER - LEICHT ABGEÄNDERT --

  //-----------------------------------------------------------------
  //! Compute the bounding box of an object within the input image by calling the 
  //! the correct template-function _computeBoundingBoxT
  //-----------------------------------------------------------------
  bool CalcCodedSegmentation::_computeBoundingBox()
  {
    ML_TRACE_IN( "CalcCodedSegmentation::_computeBoundingBox( )" );
    ML_TRY
    {
      mlDebug("Entering _computeBoundingBox.");

      if (getUpdatedInImg(0)) {      

        MLDataType mlDt = getUpdatedInImg(0)->getDataType();

        // make sure casting the interval limits to the image datatype does not hurt:
        //intMin = ML_MIN(intMin, MLDataTypeMax(mlDt));
        //intMax = ML_MAX(intMax, MLDataTypeMin(mlDt));

        switch (mlDt) {
          case MLint8Type:       return _computeBoundingBoxT<MLint8>(mlDt); break;
          case MLuint8Type:      return _computeBoundingBoxT<MLuint8>(mlDt); break;
          case MLint16Type:      return _computeBoundingBoxT<MLint16>(mlDt); break;
          case MLuint16Type:     return _computeBoundingBoxT<MLuint16>(mlDt); break;
          case MLint32Type:      return _computeBoundingBoxT<MLint32>(mlDt); break;
          case MLuint32Type:     return _computeBoundingBoxT<MLuint32>(mlDt); break;
          case MLint64Type:      return _computeBoundingBoxT<MLint64>(mlDt); break;
          case MLfloatType:      return _computeBoundingBoxT<MLfloat>(mlDt); break;
          case MLldoubleType:    return _computeBoundingBoxT<MLldouble>(mlDt); break;
          default: // using double was the default case as for now, so we keep it that way:
          case MLdoubleType:     _computeBoundingBoxT<MLdouble>(mlDt); break;
        }

      }
    }
    ML_CATCH_RETHROW;
	return false;
  }


  //-----------------------------------------------------------------
  //! Template helper function used by _computeBoundingBox to calculate type specific, it
  //! computes the bounding box of an object within the input image object.
  //-----------------------------------------------------------------
  template <typename DT>
  bool CalcCodedSegmentation::_computeBoundingBoxT(MLDataType ML_DT)
  {
    ML_TRACE_IN( "CalcCodedSegmentation::_computeBoundingBoxT( )" );
    ML_TRY
    {
      mlDebug("Entering _computeBoundingBoxT.");

      // Filled with box extents.
      b1x=b1y=b1z=b1c=b1t=b1u=ML_INT_MAX; 
      b2x=b2y=b2z=b2c=b2t=b2u=-1; 

      const Vector iSize = getUpdatedInImg(0)->getImgExt();            
      if (!(iSize == Vector(0,0,0,0,0,0)))
      {

        // Get field settings in variables for better performance.
        // Get the size of the input image;
        const MLint xs = iSize.x;
        const MLint ys = iSize.y;

        // Initialize the voxel counter
        MLuint32 numVox = 0;

        // Init the slice data pointer here since we want to reuse the allocated slice memory later.
        void *sliceData = NULL;

        for (MLint u = 0; u < iSize.u; u++)
        {
          // When increasing the index in one dimension, we always remember the voxel count
          // before starting the examination of the new data. Hence, we know if there
          // were voxels within the specified interval by comparing the voxel count before and
          // after the inner loops. This prevents us from having to put the min/max comparison for
          // e.g. the u dimension in the inner loop.
          const MLuint32 numVoxBufU = numVox;
          for (MLint t = 0; t < iSize.t; t++)
          {
            const MLuint32 numVoxBufT = numVox;
            for (MLint c = 0; c < iSize.c; c++)
            {
              const MLuint32 numVoxBufC = numVox;
              for (MLint z = 0; z < iSize.z; z++)
              {
                // Get the current slice from the buffer.
                MLErrorCode err =
                  getTile(getInOp(0),
                          getInOpIndex(0),
                          SubImgBox(Vector(   0,    0, z, c, t, u),
                                    Vector(xs-1, ys-1, z, c, t, u)),
                          ML_DT,
                          &sliceData);
                // Stop calculation if input data is not available.
                if (!sliceData || (err != ML_RESULT_OK)){
					return false;
                }

                // Scan the slice value by value.
                const MLuint32 numVoxBufZ = numVox;
                // get correctly typed pointer:
                DT* slice = (DT*) sliceData;
                for (MLint y = 0; y < iSize.y; y++)
                {
                  const MLint rowStart = xs * y;
                  DT* row = &slice[rowStart];
                  const MLuint32 numVoxBufY = numVox;
                  for (MLint x = 0; x < iSize.x; x++)
                  {
                    // Get value from image. In interval? If yes then increase voxel counter.
                    const DT val = *row++;
                    
					if (val > 0) {
						if (   (!_fld_addAllExceptNull->getBoolValue() && val>=_fld_addMinValue->getIntValue() && val<=_fld_addMaxValue->getIntValue())
							|| (_fld_addAllExceptNull->getBoolValue() && val>0) ) {
						++numVox;
						if (x < b1x){ b1x = x; }
						if (x > b2x){ b2x = x; }
						}
					}
                  }              
                  if (numVox > numVoxBufY) {
                    if (y < b1y){ b1y = y; }
                    if (y > b2y){ b2y = y; }
                  }
                }  // for (y=0...            
                if (numVox > numVoxBufZ) {
                  if (z < b1z){ b1z = z; }
                  if (z > b2z){ b2z = z; }              
                }            
              } // for (z=0...
              if (numVox > numVoxBufC) {
                if (c < b1c){ b1c = c; }
                if (c > b2c){ b2c = c; }              
              }   
            } // for (c=0...
            if (numVox > numVoxBufT) {
              if (t < b1t){ b1t = t; }
              if (t > b2t){ b2t = t; }              
            }   
          } // for (t=0...
          if (numVox > numVoxBufU) {
            if (u < b1u){ b1u = u; }
            if (u > b2u){ b2u = u; }              
          }   
        } // for (u=0...

        // Release the allocated memory.
        if (sliceData){
          freeTile(sliceData);
          sliceData = NULL;
        }

        //-------------------------------------------
        // Update interface with statistic values.
        //-------------------------------------------

        // Statistics about all voxels.

        // failed if b2x is still on init value -1:
        bool okay = (b2x != -1);
		return okay;
      }
      mlDebug("Leaving _computeBoundingBox.");
    }
    ML_CATCH_RETHROW;
	return false;
  }

ML_END_NAMESPACE