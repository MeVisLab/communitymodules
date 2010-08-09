//----------------------------------------------------------------------------------
//! The ML module class METKCreateIntraOPDummy.
/*!
// \file    mlMETKCreateIntraOPDummy.cpp
// \author  Konrad Mühler
// \date    2007-07-26
//
// This module creates dummy tumors for a new intra OP viewpoint pre-calculation
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMETKCreateIntraOPDummy.h"

#include "METK.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKCreateIntraOPDummy, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKCreateIntraOPDummy::METKCreateIntraOPDummy (void) : inherited(1,1,ObjMgrClient::EVENTS_SELECTED)  
{
	ML_TRACE_IN("METKCreateIntraOPDummy::METKCreateIntraOPDummy()")

	FieldContainer *fields = getFieldContainer();
	
	myObjMgr = new ObjMgrCommunicator();

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_CreateDummy = fields->addNotify("CreateDummy");
	_MinVoxelCount = fields->addInt("MinVoxelCount");
	_MinVoxelCount->setIntValue(1250);
	_SubDivSize = fields->addInt("SubDivSize");
	_SubDivSize->setIntValue(4);

	isCalculating = false;
	validBoundingBox = false;

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKCreateIntraOPDummy::handleNotification (Field *field)
{
	ML_TRACE_IN("METKCreateIntraOPDummy::handleNotification()")

	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-) (aus METKObjContainer geklaut)			
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);			
		}	
	}

	else if (field == _CreateDummy)
	{
		usedValues = ";";
		isCalculating = true;
		voxelsPerValue.getTable()->clear();
		processAllPages(-1);
		std::cout << usedValues << std::endl;
		createMETKObjects();
		isCalculating = false;
	}

	else if (field == getFieldContainer()->getField("input0"))
	{
		calcBoundingBox();
	}
}


//----------------------------------------------------------------------------------
//! Sets properties of the output image at output \c outIndex.
//----------------------------------------------------------------------------------
void METKCreateIntraOPDummy::calcOutImageProps (int /*outIndex*/)
{
  ML_TRACE_IN("METKCreateIntraOPDummy::calcOutImageProps ()")

  // ... set image properties for output image outIndex

}


//----------------------------------------------------------------------------------
//! Returns the input image region required to calculate a region of an output image.
//----------------------------------------------------------------------------------
SubImgBox METKCreateIntraOPDummy::calcInSubImageBox (int /*inIndex*/, const SubImgBox &outSubImgBox, int /*outIndex*/)
{
  ML_TRACE_IN("METKCreateIntraOPDummy::calcInSubImageBox ()")

  // ... return region of input image inIndex needed to compute region outSubImgBox of
  //     output image outIndex

  // replace this with your own SubImgBox:
  return outSubImgBox;
}



//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of \c calcOutSubImage to calculate page 
//! \c outSubImg of output image with index \c outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE1_CPP(METKCreateIntraOPDummy);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by 
//! CALC_OUTSUBIMAGE1_CPP(METKCreateIntraOPDummy).
//----------------------------------------------------------------------------------
template <typename DATATYPE>
void METKCreateIntraOPDummy::calcOutSubImage (TSubImg<DATATYPE> *outSubImg, int outIndex, TSubImg<DATATYPE> *inSubImg1)
{
	ML_TRACE_IN("template <typename T> METKCreateIntraOPDummy::calcOutSubImage ()")

	if (!validBoundingBox)
	{
		calcBoundingBox();
	}

	// Get extent of output image and clamp the extent of the box of outSubImg against
	// it to be sure that no voxels outside the image are processed.
	PagedImg *outImg = getOutImg(0);
	SubImgBox box = outSubImg->getBox();
	box = SubImgBox::intersect(box, outImg->getBoxFromImgExt());
	// Iterate over all valid voxels of inSubImg and outSubImg.
	Vector p = box.v1;
	double pointNr;
	int xSize = getInImg(0)->getBoxFromImgExt().getExt()[0];
	int xRows,yRows,zRows;	
	int numberOfSquares = _SubDivSize->getIntValue();//int numberOfSquaresX = floor(xSize/SubDivSize)+1;
	if (numberOfSquares<1) numberOfSquares=1;
	double SubDivSizeX = myBoundingBox.xWidth/numberOfSquares+1; //+1 um Artefakte durch Rundungsfehler zu vermeiden
	//double SubDivSizeY = myBoundingBox.yWidth/numberOfSquares;
	double SubDivSizeZ = myBoundingBox.zWidth/numberOfSquares+1;
	
	//std::cout << "SubDivSizeX:" << SubDivSizeX << "  SubDivSizeY:" << SubDivSizeY << "  SubDivSizeZ:" << SubDivSizeZ << std::endl;
	//int numberOfSquaresY = floor(ySize/SubDivSize)+1;
	int voxelValue;
	std::string voxelValueString;	
	long* valueFromHT;
	for (p.u = box.v1.u; p.u <= box.v2.u; ++p.u)
	{
		for (p.t = box.v1.t; p.t <= box.v2.t; ++p.t)
		{
			for (p.c = box.v1.c; p.c <= box.v2.c; ++p.c)
			{
				for (p.z = box.v1.z; p.z <= box.v2.z; ++p.z)
				{
					zRows = floor( (p.z-myBoundingBox.min[2]) / SubDivSizeZ );
					for (p.y = box.v1.y; p.y <= box.v2.y; ++p.y)
					{
						// Get/Set position of row starts as pointers to memory positions
						// in inSubImg and outSubImg buffers.
						p.x = box.v1.x;
						DATATYPE* iP = inSubImg1 ->getImgPos(p);
						DATATYPE* oP = outSubImg->getImgPos(p);
						// Process all voxels in row with pointers. Be sure to include last
						// voxel in row with "<= box.v2.x", because v2 is still part of box region.
						for (; p.x <= box.v2.x; ++p.x)
						{					
							if (*iP != 0)
							{
								pointNr = (p.x-myBoundingBox.min[0]) + ( (p.y-myBoundingBox.min[1]) *xSize);
								xRows = floor(( pointNr - xSize*floor(pointNr/(double)xSize) )/SubDivSizeX);
								yRows = floor( pointNr/((double)xSize*SubDivSizeX) );

								voxelValue = 10 + xRows+(yRows*numberOfSquares) + zRows*numberOfSquares*numberOfSquares;
								if (isCalculating)
								{
									voxelValueString = kBasics::IntToString(voxelValue);
									if (usedValues.find(";"+voxelValueString+";", 0)== std::string::npos)
									{
										usedValues += voxelValueString+";";
									}		
									valueFromHT = voxelsPerValue.find(voxelValueString);
									if (valueFromHT!=NULL)
									{
										*valueFromHT = *valueFromHT+1;
									}
									else voxelsPerValue.insert(voxelValueString,1);
								}
								if (outIndex!=-1) //outIndex==-1, wenn kein Wert rausgeschrieben werden soll, sondern nur berechnet werden soll (siehe Hinweise in mlProcessAllPagesExample.cpp)
								{
									*oP = (DATATYPE)( voxelValue ); // Calculate voxel from position & input
								}
							}
							else
							{
								if (outIndex!=-1) //outIndex==-1, wenn kein Wert rausgeschrieben werden soll, sondern nur berechnet werden soll (siehe Hinweise in mlProcessAllPagesExample.cpp)
								{
									*oP = (DATATYPE) (0);
								}
							}

							++iP; ++oP; // Move input and output pointer forward
						}
					}
				}
			}
		}
	}
}



void METKCreateIntraOPDummy::handleObjMgrNotification()
{
	ML_TRACE_IN("METKCreateIntraOPDummy::handleObjMgrNotification()");
	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);            
		//if (myEvent.infoID == INF_SURFACEDIST || myEvent.infoID == INF_VISIBLE || myEvent.infoID == INF_IVPOINTER) {
	}
	clearEventContainer();
}

void METKCreateIntraOPDummy::activateAttachments()
{
	ML_TRACE_IN("METKCreateIntraOPDummy::activateAttachments()");

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}


void METKCreateIntraOPDummy::createMETKObjects()
{
	ML_TRACE_IN("METKCreateIntraOPDummy::createMEKTObjects()");

	std::vector<string> splittedValues;
	kBasics::split(usedValues, ';',1000,&splittedValues);
	string objID;
	long* tempVoxelCount;
	for (unsigned int i=0; i<splittedValues.size(); i++)
	{
		if (splittedValues[i]!="")
		{
			tempVoxelCount = voxelsPerValue.find(splittedValues[i]);
			if (tempVoxelCount!=NULL)
			{
				if (*tempVoxelCount>=_MinVoxelCount->getIntValue())
				{
					objID = "IntraOPCamDummy_"+kBasics::leadingZero(i,6);
					myObjMgr->setObjAttribute(objID,LAY_DESCRIPTION,INF_NAME,new string("Name_"+objID),omINFOTYPE_STRING,true,true);
					myObjMgr->setObjAttribute(objID,LAY_DESCRIPTION,INF_RESULTTYPE,new string("IntraOPCamDummy"),omINFOTYPE_STRING,true,true);
					myObjMgr->setObjAttribute(objID,LAY_DESCRIPTION,INF_STRUCTUREGROUP,new string("TumorDummy"),omINFOTYPE_STRING,true,true);
					myObjMgr->setObjAttribute(objID,LAY_DESCRIPTION,INF_STRUCTURE,new string("IntraOPTumorDummy"),omINFOTYPE_STRING,true,true);
					myObjMgr->setObjAttribute(objID,LAY_APPEARANCE,INF_COLOR,new vec3(0.2,1,0),omINFOTYPE_VEC3,true,true);
					myObjMgr->setObjAttribute(objID,LAY_APPEARANCE,INF_TRANSPARENCY,new double(0.6),omINFOTYPE_DOUBLE,true,true);
					myObjMgr->setObjAttribute(objID,LAY_IMAGE,INF_FILENAME,new string("test.dcm"),omINFOTYPE_STRING,true,true);
					myObjMgr->setObjAttribute(objID,LAY_IMAGE,INF_OBJVALUE,new int(kBasics::StringToInt(splittedValues[i])),omINFOTYPE_INT32,true,true);
					myObjMgr->setObjAttribute(objID,LAY_IMAGE,INF_IMAGETYPE,new string("Segmentation"),omINFOTYPE_STRING,true,true);
					myObjMgr->setObjAttribute(objID,LAY_GLOBAL,INF_OBJTYPE,new string("Result"),omINFOTYPE_STRING,true,true);
				}
			}
		}
	}
}


void METKCreateIntraOPDummy::calcBoundingBox()
{
	std::cout << "calcBoundingBox" << std::endl;
	validBoundingBox = true;

	MLint64 xSize, ySize, zSize;

	if (getInImg(0)==NULL)
	{
		std::cout << "getInImg(0)==NULL ... return" << std::endl;
		return;
	}
			
	xSize = getInImg(0)->getBoxFromImgExt().getExt()[0]; ySize = getInImg(0)->getBoxFromImgExt().getExt()[1]; zSize = getInImg(0)->getBoxFromImgExt().getExt()[2];
	myBoundingBox.max = vec3(0,0,0);
	myBoundingBox.min = vec3(xSize,ySize,zSize);
	
	MLDataType type = getInImg(0)->getDataType();
	
	SubImg mySubImg;
	mySubImg.setBox(getInImg(0)->getBoxFromImgExt());
	mySubImg.setDataType(type);	
	int error = getInImg(0)->getTile(mySubImg);	
	if (error!=0)
	{
		std::cout << "getInImg(0)->getTile Error (addImage) = " << MLGetErrorCodeDescription(error) << std::endl;
		return;
	}
		
	MLint64 tempINVoxelValue;			

	void* voidValue;
	for (MLint64 z=0; z<zSize; z++)
	{
		for (MLint64 y=0; y<ySize; y++)
		{
			for (MLint64 x=0; x<xSize; x++)
			{
				voidValue = mySubImg.getVoidSubImgPos(x,y,z);
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
					std::cout << "mySubImg.getVoidSubImgPos(x,y,z)=NULL" << std::endl;
				}
				
				if (tempINVoxelValue!=0)				
				{
					if (x<myBoundingBox.min[0]) myBoundingBox.min[0]=x;
					if (x>myBoundingBox.max[0]) myBoundingBox.max[0]=x;
					if (y<myBoundingBox.min[1]) myBoundingBox.min[1]=y;
					if (y>myBoundingBox.max[1]) myBoundingBox.max[1]=y;
					if (z<myBoundingBox.min[2]) myBoundingBox.min[2]=z;
					if (z>myBoundingBox.max[2]) myBoundingBox.max[2]=z;
				}
			}
		}
	}	
	//!!
	//Diese Zeile ist lebenswichtig, weil er sonst nach mehreren Bildern mit einem MemoryAllocationFailed abstürzt
	getInImg(0)->freeTile(mySubImg.getData());

	myBoundingBox.xWidth = myBoundingBox.max[0]-myBoundingBox.min[0];
	myBoundingBox.yWidth = myBoundingBox.max[1]-myBoundingBox.min[1];
	myBoundingBox.zWidth = myBoundingBox.max[2]-myBoundingBox.min[2];	
}


ML_END_NAMESPACE