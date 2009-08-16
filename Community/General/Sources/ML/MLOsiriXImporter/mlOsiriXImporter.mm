//----------------------------------------------------------------------------------
//! The ML module class OsiriXImporter.
/*!
// \file    mlOsiriXImporter.cpp
// \author  Chunliang Wang
// \date    2009-07-07
//
// import image from OsiriX via shared memory
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlOsiriXImporter.h"
#import "MeVisLab2OsiriXTBridge.h"
ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(OsiriXImporter, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
OsiriXImporter::OsiriXImporter ()
  : BaseOp(0, OUTPUTPORTNUMBER)
{
  ML_TRACE_IN("OsiriXImporter::OsiriXImporter ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();

  // Add fields to the module and set their values.
  // Also attach them to the output images to notify connected modules about changes.

	_OperationNameFld = fields.addString("OperationName");
	_OperationNameFld->setStringValue("Operation1");
	
  _ModifiedFld = fields.addBool("Modified");
  _ModifiedFld->setBoolValue(0);
	int i;
	for(i=0;i<OUTPUTPORTNUMBER;i++)
		_ModifiedFld->attachField(getOutField(i));
 
  _ReleaseFld = fields.addNotify("Release");
	for(i=0;i<OUTPUTPORTNUMBER;i++)
		_ReleaseFld->attachField(getOutField(i));

	_ShowMeVisLabWindowFld = fields.addBool("ShowMeVisLabWindow");
	_ShowMeVisLabWindowFld->setBoolValue(0);
	_ParaName0Fld = fields.addString("ParaName0");
	_ParaName0Fld->setStringValue("Parameter0");
//	_Parameter0IntValueFld = fields.addInt("Parameter0IntValue");
//	_Parameter0IntValueFld->setIntValue(0);
	_Parameter0DoubleValueFld = fields.addDouble("Parameter0Value");
	_Parameter0DoubleValueFld->setDoubleValue(0);
//	_Parameter0StringValueFld = fields.addString("Parameter0StringValue");
//	_Parameter0StringValueFld->setStringValue("none");
	_ParaName1Fld = fields.addString("ParaName1");
	_ParaName1Fld->setStringValue("Paramete1");
//	_Parameter1IntValueFld = fields.addInt("Parameter1IntValue");
//	_Parameter1IntValueFld->setIntValue(0);
	_Parameter1DoubleValueFld = fields.addDouble("Parameter1Value");
	_Parameter1DoubleValueFld->setDoubleValue(0);
//	_Parameter1StringValueFld = fields.addString("Parameter1StringValue");
//	_Parameter1StringValueFld->setStringValue("none");
	_ParaName2Fld = fields.addString("ParaName2");
	_ParaName2Fld->setStringValue("Paramete2");
//	_Parameter2IntValueFld = fields.addInt("Parameter2IntValue");
//	_Parameter2IntValueFld->setIntValue(0);
	_Parameter2DoubleValueFld = fields.addDouble("Parameter2Value");
	_Parameter2DoubleValueFld->setDoubleValue(0);
//	_Parameter2StringValueFld = fields.addString("Parameter2StringValue");
//	_Parameter2StringValueFld->setStringValue("none");
	_ParaName3Fld = fields.addString("ParaName3");
	_ParaName3Fld->setStringValue("Paramete3");
//	_Parameter3IntValueFld = fields.addInt("Parameter3IntValue");
//	_Parameter3IntValueFld->setIntValue(0);
	_Parameter3DoubleValueFld = fields.addDouble("Parameter3Value");
	_Parameter3DoubleValueFld->setDoubleValue(0);
//	_Parameter3StringValueFld = fields.addString("Parameter3StringValue");
//	_Parameter3StringValueFld->setStringValue("none");
	
	_ParaName4Fld = fields.addString("ParaName4");
	_ParaName4Fld->setStringValue("Paramete4");
	_Parameter4DoubleValueFld = fields.addDouble("Parameter4Value");
	_Parameter4DoubleValueFld->setDoubleValue(0);
	_ParaName5Fld = fields.addString("ParaName5");
	_ParaName5Fld->setStringValue("Paramete5");
	_Parameter5DoubleValueFld = fields.addDouble("Parameter5Value");
	_Parameter5DoubleValueFld->setDoubleValue(0);
	
	_DICOM_Series_UIDFld = fields.addString("DICOM_Series_UID");
	_DICOM_Series_UIDFld->setStringValue("none");
	
 
  // Connect input field(s) with output field(s) to notify
  // connected modules if input image(s) change.
  for (int inIdx=0; inIdx < 0; ++inIdx){
    for (int outIdx=0; outIdx < OUTPUTPORTNUMBER; ++outIdx){
      getInField(inIdx)->attachField(getOutField(outIdx));
    }
  }

  //Marker export
	_OutputXMarkerListField = fields.addBase("outputXMarkerList");
	_OutputXMarkerListField->setBaseValue(&_outputXMarkerList);
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();



  // Activate inplace data buffers for output outIndex and input inIndex.
  // setOutImgInplace(outIndex, inIndex);

  // Activate page data bypass from input inIdx to output outIdx.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outIndex, inIndex);

  // Activate parallel execution of calcOutSubImage.
  // setThreadSupport(supportMode);
  // with supportMode =
  //   NO_THREAD_SUPPORT                 //! The module is not thread safe at all.
  //   CALC_OUTSUBIMAGE_ON_STD_TYPES     //! calcOutSubImage can be called in parallel for scalar voxel types.
  //   CALC_OUTSUBIMAGE_ON_CARRIER_TYPES //! calcOutSubImage can be called in parallel for carrier voxel types.
  //   CALC_OUTSUBIMAGE_ON_ALL_TYPES     //! calcOutSubImage can be called in parallel for all voxel types.
  // Warning: You should be familiar with multithreading before activating this feature.

  // Specify whether the module can only process standard scalar voxel types or
  // also registered voxel types (vec2, mat2, complexf, Vector, etc.)
  // setVoxelDataTypeSupport(permittedTypes);
  // with permittedTypes =
  //   ONLY_STANDARD_TYPES               //! Only standard scalar voxels are supported.
  //   FULLY_OPERATIONAL                 //! Scalar and registered voxels types are supported.
  //   MINIMUM_OPERATIONAL               //! Scalar and registered voxel types are supported.
  //                                     //! Voxel operators are not used by algorithm.
  //
  // See ML Programming Guide, "Configuring Image Processing Behaviour of the BaseOp"
  // for further details.
	
	//initialize the OsiriX bridge
	NSDate* fixData=[NSDate dateWithString:@"2009-01-01 01:00:00 +0100"];
	NSTimeInterval uniqueNumberForNow=[fixData timeIntervalSinceNow];
	NSString* uniqueName=[NSString stringWithFormat:@"OsiriXImporter:%f",uniqueNumberForNow];
	bridgeToOsiriX=[[MeVisLab2OsiriXTBridge alloc] initWithIncommingConnectionName:uniqueName OutgoingConnection:@"MeVisHub:Exporter"];
	[bridgeToOsiriX setImporterML:this];
}
OsiriXImporter::~OsiriXImporter()
{
	if(bridgeToOsiriX)
	{
		[bridgeToOsiriX prepareToDealloc];
		[bridgeToOsiriX release];
	}
	
}
//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void OsiriXImporter::handleNotification (Field *field)
{
  ML_TRACE_IN("OsiriXImporter::handleNotification ()");

  // Handle changes of module parameters and connectors here.
	std::string fieldname = field->getName();

	if(fieldname=="OperationName")
	{
		std::string operatinName=_OperationNameFld->getStringValue();
		NSString* newRegisterName=[NSString stringWithString:@"OsiriXImporter:"];
		newRegisterName=[newRegisterName stringByAppendingString:[NSString stringWithCString:operatinName.c_str()]];
		[bridgeToOsiriX registerIncomingConnectionWithName:newRegisterName];
	}
	
}
//----------------------------------------------------------------------------------
//! Update internal module state after load or clone and enable notification handling again.
//----------------------------------------------------------------------------------
void OsiriXImporter::activateAttachments ()
{
	ML_TRACE_IN("OsiriXExporter::activateAttachments ()");
	
	// Update members to new field state here.
	std::string operatinName=_OperationNameFld->getStringValue();
	NSString* newRegisterName=[NSString stringWithString:@"OsiriXImporter:"];
	newRegisterName=[newRegisterName stringByAppendingString:[NSString stringWithCString:operatinName.c_str()]];
	[bridgeToOsiriX registerIncomingConnectionWithName:newRegisterName];
	// Call super class functionality to enable notification handling again.
	BaseOp::activateAttachments ();
}

void OsiriXImporter::notifyInputImagesUpdated()
{
	int i;
	for(i=0;i<OUTPUTPORTNUMBER;i++)
		getOutField(i)->notifyAttachments();
}
void OsiriXImporter::notifyXMarkerListUpdated()
{
	NSString* description=[NSString stringWithString:@"InputImage4"];
	NSDictionary* anImage=[bridgeToOsiriX getImageFromLowerBridge:description];
	if(!anImage)
		return;
	// Clear _outputXMarkerList.
	if([[anImage objectForKey:@"OverlayType"] isEqualToString:@"points"])
	{
		NSArray* points=[anImage objectForKey:@"Points"];
		_outputXMarkerList.clearList();
		unsigned i;
		for(i=0; i<[points count]; i++)
		{
			// Fill marker with zeros.
			XMarker outMarker(vec6(0),vec3(0),0);
			NSDictionary* apoint=[points objectAtIndex:i];
			outMarker.pos[0]=[[apoint objectForKey:@"x"] intValue];
			outMarker.pos[1]=[[apoint objectForKey:@"y"] intValue];
			outMarker.pos[2]=[[apoint objectForKey:@"z"] intValue];
			outMarker.type = [[apoint objectForKey:@"value"] intValue];
					
			// Append XMarker to XMarkerList.
			_outputXMarkerList.push_back(outMarker);
		}
		
	}
	// Update local XMarkerList and set BaseValue of the output
	(XMarkerList*)_OutputXMarkerListField->getBaseValue();
	// Notify the XMarkerList output
    _OutputXMarkerListField->notifyAttachments();

}
void OsiriXImporter::updateParameters(NSDictionary* parameters)
{
	NSArray* allKeys=[parameters allKeys];
	for(NSString* akey in allKeys)
	{
		if([akey isEqualToString:@"ImageUpdated"])
		{
			_ModifiedFld->setBoolValue(1);
			//notifyInputImagesUpdated();
		}
		else if([akey isEqualToString:@"SeriesUID"])
			_DICOM_Series_UIDFld->setStringValue([[parameters valueForKey:akey] UTF8String]);
		else if([akey isEqualToString:@"MarkerUpdated"])
			    notifyXMarkerListUpdated();
		else if([akey isEqualToString:@"ShowMeVisLab"])
		{
			_ShowMeVisLabWindowFld->setBoolValue(1);
			[[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
		}
		else if(_ParaName0Fld->getStringValue()==[akey UTF8String])
			_Parameter0DoubleValueFld->setDoubleValue([[parameters valueForKey:akey] doubleValue]);
		else if(_ParaName1Fld->getStringValue()==[akey UTF8String])
			_Parameter1DoubleValueFld->setDoubleValue([[parameters valueForKey:akey] doubleValue]);
		else if(_ParaName2Fld->getStringValue()==[akey UTF8String])
			_Parameter2DoubleValueFld->setDoubleValue([[parameters valueForKey:akey] doubleValue]);
		else if(_ParaName3Fld->getStringValue()==[akey UTF8String])
			_Parameter3DoubleValueFld->setDoubleValue([[parameters valueForKey:akey] doubleValue]);
		else if(_ParaName4Fld->getStringValue()==[akey UTF8String])
			_Parameter4DoubleValueFld->setDoubleValue([[parameters valueForKey:akey] doubleValue]);
		else if(_ParaName5Fld->getStringValue()==[akey UTF8String])
			_Parameter5DoubleValueFld->setDoubleValue([[parameters valueForKey:akey] doubleValue]);
	}
}
//----------------------------------------------------------------------------------
//! Sets properties of the output image at output outIndex.
//----------------------------------------------------------------------------------
void OsiriXImporter::calcOutImageProps (int outIndex)
{
  ML_TRACE_IN("OsiriXImporter::calcOutImageProps ()");

  // Change properties of output image outIndex here whose
  // defaults are inherited from the input image (if there is one).

  // There are no inputs from which image properties are inherited.
  // At least the image extent has to be specified.
	NSString* description=[NSString stringWithFormat:@"InputImage%d",outIndex];
	NSDictionary* anImage=[bridgeToOsiriX getImageFromLowerBridge:description];
	
	int imagetype=MLfloatType;
	int dimension[4]={0,0,0,0};
	float spacing[4]={1.0,1.0,1.0,1.0};
	float transformmatrix[16]={1,0,0,0,
		                       0,1,0,0,
		                       0,0,1,0,
	                           0,0,0,1};
	float maxvalue=1, minvalue=0;
	
		
	if(anImage)
	{
		NSString* imgtypestr=[anImage objectForKey:@"ImageType"];
		if([imgtypestr isEqualToString:@"float"]||[imgtypestr isEqualToString:@"char"])
		{
			
					int i;
					NSArray* dimensionarray=[anImage objectForKey:@"Dimension"];
					for(i=0;i<4;i++)
						dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
					
					
					NSArray* spacingarray=[anImage objectForKey:@"Spacing"];
					for(i=0;i<4;i++)
						spacing[i]=[[spacingarray objectAtIndex:i] floatValue];
					
					NSArray* matrix=[anImage objectForKey:@"MatrixToPatientCo"];
					for(i=0;i<16;i++)
						transformmatrix[i]=[[matrix objectAtIndex:i] floatValue];
					maxvalue=[[anImage objectForKey:@"Maximum"] floatValue];
					minvalue=[[anImage objectForKey:@"Minimum"] floatValue];

					if([imgtypestr isEqualToString:@"float"])
						imagetype=MLfloatType;
					else if([imgtypestr isEqualToString:@"char"])
						imagetype=MLuint8Type;
			InputImagePtr[outIndex]=(void*)[[anImage objectForKey:@"Data"] bytes];
			InputImageType[outIndex]=imagetype;
		}
	}

  getOutImg(outIndex)->setImgExt(Vector(dimension[0],dimension[1],dimension[2],dimension[3],1,1));
  getOutImg(outIndex)->setMinVoxelValue(minvalue);
  getOutImg(outIndex)->setMaxVoxelValue(maxvalue);
  getOutImg(outIndex)->setVoxelSize(spacing[0],spacing[1],spacing[2]);
  getOutImg(outIndex)->setToWorldMatrix(transformmatrix);
  getOutImg(outIndex)->setDataType(imagetype);
}

//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of calcOutSubImage to calculate page
//! outSubImg of output image with index outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE0_CPP(OsiriXImporter);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by
//! CALC_OUTSUBIMAGE0_CPP(OsiriXImporter).
//----------------------------------------------------------------------------------
template <typename T>
void OsiriXImporter::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex
                                    )
{
  ML_TRACE_IN("template <typename T> OsiriXImporter::calcOutSubImage ()");

  // Compute subimage of output image outIndex.
	float* floatImagePtr=(float*)InputImagePtr[outIndex];
	unsigned char* bitImagePtr=(unsigned char*)InputImagePtr[outIndex];
	Vector dimension=getOutImg(outIndex)->getImgExt();
	
  // Clamp box of output image against image extent to avoid that unused areas are processed.
  const SubImgBox validOutBox(outSubImg->getBox().intersect(getOutImg(outIndex)->getBoxFromImgExt()));

  // Process all voxels of the valid region of the output page.
  Vector p;
  for (p.u=validOutBox.v1.u;  p.u<=validOutBox.v2.u;  ++p.u) {
    for (p.t=validOutBox.v1.t;  p.t<=validOutBox.v2.t;  ++p.t) {
      for (p.c=validOutBox.v1.c;  p.c<=validOutBox.v2.c;  ++p.c) {
        for (p.z=validOutBox.v1.z;  p.z<=validOutBox.v2.z;  ++p.z) {
          for (p.y=validOutBox.v1.y;  p.y<=validOutBox.v2.y;  ++p.y) {

            p.x = validOutBox.v1.x;
            // Get pointer to the row start of the output subimage.
            T          *outVoxel = outSubImg->getImgPos(p);
            const MLint rowEnd   = validOutBox.v2.x;

            // Process all row voxels.
			  switch (InputImageType[outIndex]) {
				  case MLfloatType:
					  for (; p.x <= rowEnd;  ++p.x, ++outVoxel)
						  *outVoxel =(T)( *(floatImagePtr+p.c*dimension.x*dimension.y*dimension.z+p.z*dimension.x*dimension.y+p.y*dimension.x+p.x));;
					  break;
				  case MLuint8Type:
					  for (; p.x <= rowEnd;  ++p.x, ++outVoxel)
						  *outVoxel =(T)( *(bitImagePtr+p.c*dimension.x*dimension.y*dimension.z+p.z*dimension.x*dimension.y+p.y*dimension.x+p.x));;
					  break;
					  
				  default:
					  for (; p.x <= rowEnd;  ++p.x, ++outVoxel) {
						  *outVoxel = 1;
					  }
					  
					  break;
			  }
		  }
        }
      }
    }
  }
}

ML_END_NAMESPACE

