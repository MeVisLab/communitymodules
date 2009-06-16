//----------------------------------------------------------------------------------
//! The ML module class CalcCodedSegmentation.
/*!
// \file    mlCalcCodedSegmentation.h
// \author  Konrad Mühler
// \date    2007-01-16
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __mlCalcCodedSegmentation_H
#define __mlCalcCodedSegmentation_H


// Local includes
#ifndef __UMDCodedSegmentationSystem_H
#include "UMDCodedSegmentationSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif
#ifndef __mlTools_H
#include <mlTools.h>
#endif

//#pragma warning(disable:4786)
#include <vector>
#include "kBasics.h"
#include "HashTable.h"
//#pragma warning(disable:4251)

ML_START_NAMESPACE


//! 
class UMDCODEDSEGMENTATION_EXPORT CalcCodedSegmentation : public BaseOp
{
public:

  //! Constructor.
  CalcCodedSegmentation (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  // ----------------------------------------------------------
  //@{ \name Image processing methods.
  // ----------------------------------------------------------

  //! Sets properties of the output image at output \c outIndex.
  virtual void calcOutImageProps (int outIndex);

  //! Returns the input image region required to calculate a region of an output image.
  //! \param inIndex        The input of which the regions shall be calculated.
  //! \param outSubImageBox The region of the output image for which the required input region
  //!                       shall be calculated.
  //! \param outIndex       The index of the output image for which the required input region
  //!                       shall be calculated.
  //! \return Region of input image needed to compute the region \c outSubImgBox on output \c outIndex.
  virtual SubImgBox calcInSubImageBox (int inIndex, const SubImgBox &outSubImgBox, int outIndex);

  //! Calculates page \c outSubImg of output image with index \c outIndex by using \c inSubimgs.
  //! \param outSubImg The subimage of output image \c outIndex calculated from \c inSubImgs.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
  //!                  by \c calcInSubImageBox. Array size is given by \c getInputNum().
  //virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

  //! Method template for type-specific page calculation. Called by \c calcOutSubImage().
  //! \param outSubImg The typed subimage of output image \c outIndex calculated from \c inSubImg?.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImg1 Temporary subimage of input 1.    
  void calcOutSubImage (SubImg *outSubImg, int outIndex,SubImg *inSubImgs);
  //@}


private:

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	//! 
	NotifyField *_fld_Add;

	//! 
	StringField *_fld_NameToAdd;

	//!
	BoolField *_fld_addAllExceptNull;
	IntField *_fld_addMinValue;
	IntField *_fld_addMaxValue;

	//! 
	NotifyField *_fld_Reset;

	//! 
	NotifyField *_fld_Purge;


	//! 
	NotifyField *_fld_Finish;


	//! 
	StringField *_fld_ImageValues;
	StringField *_fld_ObjectValues;
	

	//@}


	int nextFreeValue;
	vector<unsigned long>::size_type MAX_SIZE;
	MLint64 replaceValues_MaxIndex;  
	MLint64 nextStart;  

	vector<MLint64>* replaceValues;
	vector<string>* objectValues;
	HashTable<string>* valuesForObjects;
	vector<string>* objectNames;

	VirtualVolume* _virtualVolume;

	void reset();

	void addImage();
	//template <typename T>
	//void addImageTemplated(T xxx);

	void clearOutputImage();
	//template <typename T>
	//void clearOutputImageTemplated(T x);

	void renewObjectValues();

	void buildString();

	void purgeValues();
	template <typename T>
	void purgeValuesTemplated(T x);


	//! Compute the statistics of the data set.
	bool  _computeBoundingBox();

	//! Template helper function used by _computeBoundingBox to calculate type specific, it
	//! computes the bounding box of an object within the input image object.
	template <typename DT>
	bool _computeBoundingBoxT(MLDataType ML_DT);

    MLint b1x, b1y, b1z, b1c, b1t, b1u;
    MLint b2x, b2y, b2z, b2c, b2t, b2u;


  
    
  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(CalcCodedSegmentation)

};


ML_END_NAMESPACE

#endif // __mlCalcCodedSegmentation_H


