//----------------------------------------------------------------------------------
//! The ML module class OsiriXImporter.
/*!
// \file    mlOsiriXImporter.h
// \author  Chunliang Wang
// \date    2009-07-07
//
// import image from OsiriX via shared memory
*/
//----------------------------------------------------------------------------------


#ifndef __mlOsiriXImporter_H
#define __mlOsiriXImporter_H

#import <Cocoa/Cocoa.h>
// Local includes
#include "MLOsiriXImporterSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#define id Id
#include "mlXMarkerList.h"
#undef id

@class MeVisLab2OsiriXTBridge;

ML_START_NAMESPACE
#define OUTPUTPORTNUMBER 4


//! import image from OsiriX via shared memory
class MLOSIRIXIMPORTER_EXPORT OsiriXImporter : public BaseOp
{
public:

  //! Constructor.
  OsiriXImporter ();
  ~OsiriXImporter ();

	
	//MeVisLab2OsiriXTBridge
	MeVisLab2OsiriXTBridge* bridgeToOsiriX;
	void notifyInputImagesUpdated();
	void notifyXMarkerListUpdated();
	void updateParameters(NSDictionary* parameters);
 
  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);
	
	//! Update internal module state after load or clone and enable notification handling again.
	virtual void activateAttachments ();

  // ----------------------------------------------------------
  //! \name Image processing methods.
  //@{
  // ----------------------------------------------------------

  //! Sets properties of the output image at output outIndex.
  virtual void calcOutImageProps (int outIndex);

  //! Calculates page outSubImg of output image with index outIndex by using inSubimgs.
  //! \param outSubImg The subimage of output image outIndex calculated from inSubImgs.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
  //!                  by calcInSubImageBox. Array size is given by getInputNum().
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

  //! Method template for type-specific page calculation. Called by calcOutSubImage().
  //! \param outSubImg The typed subimage of output image outIndex calculated from inSubImg.
  //! \param outIndex  The index of the output the subimage is calculated for.
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex);
  //@}

private:
  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

    //! The name of this operation (should be unique on the system)
	StringField *_OperationNameFld;


  //! if it is true, the caching data should be recaculated.
  BoolField *_ModifiedFld;

  //! When triggered, OsiriXImporter should free all memory. And set export port to empty.
  NotifyField *_ReleaseFld;
  //! Marker input from OsiriX	
  BaseField *_OutputXMarkerListField;
	XMarkerList _outputXMarkerList;

    //! If set true, MeVisLab will be activated from background
	BoolField *_ShowMeVisLabWindowFld;
	
	//! The name of parameter0
	StringField *_ParaName0Fld;
	
	//! The value of parameter0 if it is integer
//	IntField *_Parameter0IntValueFld;
	
	//! The value of parameter0 if it is double
	DoubleField *_Parameter0DoubleValueFld;
	
	//! The value of parameter0 if it is string
//	StringField *_Parameter0StringValueFld;
	
	//! The name of parameter1
	StringField *_ParaName1Fld;
	
	//! The value of parameter1 if it is integer
//	IntField *_Parameter1IntValueFld;
	
	//! The value of parameter1 if it is double
	DoubleField *_Parameter1DoubleValueFld;
	
	//! The value of parameter1 if it is string
//	StringField *_Parameter1StringValueFld;
	
	//! The name of parameter2
	StringField *_ParaName2Fld;
	
	//! The value of parameter2 if it is integer
//	IntField *_Parameter2IntValueFld;
	
	//! The value of parameter2 if it is double
	DoubleField *_Parameter2DoubleValueFld;
	
	//! The value of parameter2 if it is string
//	StringField *_Parameter2StringValueFld;
	
	//! The name of parameter3
	StringField *_ParaName3Fld;
	
	//! The value of parameter3 if it is integer
//	IntField *_Parameter3IntValueFld;
	
	//! The value of parameter3 if it is double
	DoubleField *_Parameter3DoubleValueFld;
	
	//! The value of parameter3 if it is string
//	StringField *_Parameter3StringValueFld;
	
   //......
	StringField *_ParaName4Fld;
	DoubleField *_Parameter4DoubleValueFld;
	StringField *_ParaName5Fld;
	DoubleField *_Parameter5DoubleValueFld;
	
	//! The Series UID of the images on which the operation is performing. This can be used as a uniqe filename for saving intermediate result in MeVisLab
	StringField *_DICOM_Series_UIDFld;
	  
 
	
  //@}
	void* InputImagePtr[OUTPUTPORTNUMBER];
	int InputImageType[OUTPUTPORTNUMBER];
  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(OsiriXImporter)
};


ML_END_NAMESPACE

#endif // __mlOsiriXImporter_H

