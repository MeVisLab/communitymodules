//----------------------------------------------------------------------------------
//! The ML module class OsiriXExporter.
/*!
// \file    mlOsiriXExporter.cpp
// \author  Chunliang Wang
// \date    2009-08-04
//
// Export image to OsiriX
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlOsiriXExporter.h"
#import "MeVisLab2OsiriXTBridge.h"
ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(OsiriXExporter, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
OsiriXExporter::OsiriXExporter ()
  : BaseOp(2, 0)
{
  ML_TRACE_IN("OsiriXExporter::OsiriXExporter ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();

  // Add fields to the module and set their values.
  // Also attach them to the output images to notify connected modules about changes.
  _OperationNameFld = fields.addString("OperationName");
  _OperationNameFld->setStringValue("Operation1");
  _SendBackButtonFld = fields.addNotify("SendBackButton");

  // Connect input field(s) with output field(s) to notify
  // connected modules if input image(s) change.
  for (int inIdx=0; inIdx < 2; ++inIdx){
    for (int outIdx=0; outIdx < 0; ++outIdx){
      getInField(inIdx)->attachField(getOutField(outIdx));
    }
  }
  //CSO based Marker export
  _InputCSOListFld=NULL;
  _InputCSOListFld  = fields.addBase ("inputCSOList");
  _InputCSOListFld->setBaseValue(_InputCSOList);
  
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
  NSString* uniqueName=[NSString stringWithFormat:@"OsiriXExporter:%f",uniqueNumberForNow];
  bridgeToOsiriX=[[MeVisLab2OsiriXTBridge alloc] initWithIncommingConnectionName:uniqueName];
  [bridgeToOsiriX setExporterML:this];
  
}
OsiriXExporter::~OsiriXExporter()
{
  if(bridgeToOsiriX)
  {
    [bridgeToOsiriX prepareToDealloc];
    [bridgeToOsiriX release];
  }
  
}
BOOL OsiriXExporter::calcInputImageProps(NSMutableDictionary* anImage,int inIndex)
{

  if(!getInImg(inIndex))
    return NO;
  int dimension[4]={0,0,0,0};
  float spacing[4]={1.0,1.0,1.0,1.0};
  MLfloat transformmatrix[16]={1,0,0,0,
    0,1,0,0,
    0,0,1,0,
  0,0,0,1};
  float maxvalue=1, minvalue=0;
  if(inIndex>=2)
  {
    //for CSO objects we do not get Image info
    prepareCSOForOsiriX(anImage);
  } 
  else
  {
    if(getUpdatedInImg(inIndex))
    {
      Vector dim=getInImg(inIndex)->getImgExt();
      dimension[0]=dim.x;
      dimension[1]=dim.y;
      dimension[2]=dim.z;
      dimension[3]=dim.c;
      getInImg(inIndex)->getVoxelSize(&(spacing[0]),&(spacing[1]),&(spacing[3]));
      minvalue=getInImg(inIndex)->getMinVoxelValue();
      maxvalue=getInImg(inIndex)->getMaxVoxelValue();
      getInImg(inIndex)->getToWorldMatrix(transformmatrix);
    }
    
    
    if(anImage)
    {
      
      int i;
      NSMutableArray* dimensionarray=[NSMutableArray arrayWithCapacity:0];
      for(i=0;i<4;i++)
        [dimensionarray addObject:[NSNumber numberWithInt:dimension[i]]];
      [anImage setObject:dimensionarray forKey:@"Dimension"];
      
      
      NSMutableArray* spacingarray=[NSMutableArray arrayWithCapacity:0];
      for(i=0;i<4;i++)
        [spacingarray addObject:[NSNumber numberWithFloat:spacing[i]]];
      [anImage setObject:spacingarray forKey:@"Spacing"];
      
      NSMutableArray* matrix=[NSMutableArray arrayWithCapacity:0];
      for(i=0;i<16;i++)
        [matrix addObject:[NSNumber numberWithFloat:transformmatrix[i]]];
      [anImage setObject:matrix forKey:@"MatrixToPatientCo"];
      [anImage setObject:[NSNumber numberWithFloat:maxvalue] forKey:@"Maximum"];
      [anImage setObject:[NSNumber numberWithFloat:minvalue] forKey:@"Minimum"];
      
    }
  }
  
  
  
  return YES;
}

BOOL OsiriXExporter::calcInSubImage(NSMutableDictionary* anImage, int inIndex )
{
  if(inIndex>=2)
  {
    //for CSO objects we do not get Images
    return NO;
  } 
  else
  {
    if(!getInImg(inIndex))
      return NO;
    NSLog(@"MeVisLab: MeVisLab is computing");
    SubImgBox inImageBox = getInImg(inIndex)->getBoxFromImgExt();
    
    // get according sub image memory
    int imagetype=MLfloatType;
    if([[anImage objectForKey:@"ImageType"] isEqualToString:@"float"])
      imagetype=MLfloatType;
    else if([[anImage objectForKey:@"ImageType"] isEqualToString:@"char"])
      imagetype=MLuint8Type;
    void* memPtr=(void*)[[anImage objectForKey:@"Data"] bytes];

    
    MLErrorCode memOK = getTile(getInOp(inIndex), getInOpIndex(inIndex), inImageBox, imagetype, &memPtr);
    if (memOK != ML_RESULT_OK)
    {
      std::cout << "Failed to caculate the result for OsiriX Exporter " <<  std::endl;
      return NO;

    } 
    else
      std::cout << "Succeed to caculate the result for OsiriX Exporter " <<  std::endl;
    NSLog(@"MeVisLab: MeVisLab finished computing");
  } 
  return YES;
}
//prepare CSO for OsiriX
BOOL OsiriXExporter::prepareCSOForOsiriX(NSMutableDictionary* anImage)
{
  if(anImage)
  {

    NSMutableArray* overlayObjects=[NSMutableArray arrayWithCapacity:0];
    _InputCSOList=static_cast<CSOList*>(_InputCSOListFld->getBaseValue());
    if(!_InputCSOList)
      return NO;
    int numberofCSO=_InputCSOList->numCSO();
    int i;
    for(i=0;i<numberofCSO;i++)
    {
      CSO* aCSO=_InputCSOList->getCSOAt(i);
      std::string csoname= aCSO->getGroupAt(0)->getLabel();
      NSMutableDictionary* anOverlayObject=[NSMutableDictionary dictionaryWithCapacity:0];
      [anOverlayObject setObject:[NSString stringWithUTF8String:csoname.c_str() forKey:@"Name"]];
      NSMutableArray* points3D=[NSMutableArray arrayWithCapacity:0];
      int numberofPathLists=aCSO->numPathPointLists();
      int j;
      for(j=0;j<numberofPathLists;j++)
      {
        CSOPathPoints* apath=aCSO->getPathPointsAt(j);
        int numberofPoints=apath->numPathPoints();
        int k;
        for(k=0;k<numberofPoints;k++)
        {
          vec3 aCSOPos;
          aCSOPos=apath->getPosAt(k);
          float x,y,z;
          x=aCSOPos[0];
          y=aCSOPos[1];
          z=aCSOPos[2];
          NSMutableDictionary* a3Dpoint=[NSMutableDictionary dictionaryWithCapacity:3];
          [a3Dpoint setObject:[NSNumber numberWithFloat:x] forKey:@"x"];
          [a3Dpoint setObject:[NSNumber numberWithFloat:y] forKey:@"y"];
          [a3Dpoint setObject:[NSNumber numberWithFloat:z] forKey:@"z"];
          [points3D addObject:a3Dpoint];
          
        }
      }
      if([points3D count]>2)
      {
        [anOverlayObject setObject:points3D forKey:@"Points"];
        //OsiriX handle polygon and spline as same object, and change a global switch to turn on or off the "smooth" function
        //so here we just define any CSO as a polygon because here we send all the path points not seed points to OsiriX so
        //OsiriX will not need to do the interpolation.
        [anOverlayObject setObject:[NSString stringWithString:@"polygon" forKey:@"OverlayType"]];
        [anOverlayObject setObject:[NSNumber numberWithBool:aCSO->isClosed()] forKey:@"IsClosed"];
        [overlayObjects addObject:anOverlayObject];
      }
      
    }
    //check if CSO exists
    [anImage setObject:[NSString stringWithString:@"overlay"] forKey:@"ImageType"];
    if([overlayObjects count])
    {
      [anImage setObject:overlayObjects forKey:@"OverlayObjects"];
    }
        
  }
  return YES;
}
//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void OsiriXExporter::handleNotification (Field *field)
{
  ML_TRACE_IN("OsiriXExporter::handleNotification ()");

  // Handle changes of module parameters and connectors here.
  std::string fieldname = field->getName();

    
  if(fieldname=="OperationName")
  {
    std::string operatinName=_OperationNameFld->getStringValue();
    NSString* newRegisterName=[NSString stringWithString:@"OsiriXExporter:"];
    newRegisterName=[newRegisterName stringByAppendingString:[NSString stringWithCString:operatinName.c_str()]];
  [bridgeToOsiriX registerIncomingConnectionWithName:newRegisterName];  
  }
  else if(fieldname=="SendBackButton")
  {
    NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:1];
    [parameters setObject:[NSNumber numberWithBool:YES] forKey:@"NeedUpdateImage"];
    [parameters setObject:[NSNumber numberWithBool:YES] forKey:@"NeewShowSelfWindow"];
    [bridgeToOsiriX passingOnNotificationsToUpperBridge:parameters];
    
  }
  else
  {
    NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:1];
    [parameters setObject:[NSNumber numberWithBool:YES] forKey:@"NeedUpdateImage"];
    [parameters setObject:[NSNumber numberWithBool:NO]  forKey:@"NeewShowSelfWindow"];
    [bridgeToOsiriX passingOnNotificationsToUpperBridge:parameters];
    
  }
}

//----------------------------------------------------------------------------------
//! Update internal module state after load or clone and enable notification handling again.
//----------------------------------------------------------------------------------
void OsiriXExporter::activateAttachments ()
{
  ML_TRACE_IN("OsiriXExporter::activateAttachments ()");

  // Update members to new field state here.
  std::string operatinName=_OperationNameFld->getStringValue();
  NSString* newRegisterName=[NSString stringWithString:@"OsiriXExporter:"];
  newRegisterName=[newRegisterName stringByAppendingString:[NSString stringWithCString:operatinName.c_str()]];
  [bridgeToOsiriX registerIncomingConnectionWithName:newRegisterName];
  // Call super class functionality to enable notification handling again.
  BaseOp::activateAttachments ();
}

//----------------------------------------------------------------------------------
//! Configures (in)validation handling of inputs which are not connected or up to date.
//----------------------------------------------------------------------------------
BaseOp::INPUT_HANDLE OsiriXExporter::handleInput(int /*inIndex*/, INPUT_STATE /*state*/) const
{
  ML_TRACE_IN("OsiriXExporter::handleInput ()");

  return INVALIDATE;
}

ML_END_NAMESPACE

