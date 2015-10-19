//----------------------------------------------------------------------------------
//! This ML ObjMgrClient module creates dummy tumors for a new intra OP viewpoint pre-calculation.
/*!
// \file    mlMETKCreateIntraOPDummy.h
// \author  Konrad Mühler
// \date    2007-07-26
//
// This module creates dummy tumors for a new intra OP viewpoint pre-calculation
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETKCreateIntraOPDummy_H
#define __mlMETKCreateIntraOPDummy_H


// Local includes
#include "ViewpointSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "kBasics.h"
#include "HashTable.h"
#include "ObjMgrCommunicator.h"

ML_START_NAMESPACE


//! This ML ObjMgrClient module creates dummy tumors for a new intra OP viewpoint pre-calculation.
class VIEWPOINT_EXPORT METKCreateIntraOPDummy : public ObjMgrClient
{
public:

  //! Constructor.
  METKCreateIntraOPDummy (void);

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
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

  //! Method template for type-specific page calculation. Called by \c calcOutSubImage().
  //! \param outSubImg The typed subimage of output image \c outIndex calculated from \c inSubImg?.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImg1 Temporary subimage of input 1.
  template <typename DATATYPE>
  void calcOutSubImage (TSubImg<DATATYPE> *outSubImg, int outIndex, TSubImg<DATATYPE> *inSubImg1);

  //@}


   virtual void handleObjMgrNotification();

   virtual void activateAttachments();


private:

  typedef ObjMgrClient inherited;

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //!
  NotifyField *_CreateDummy;

  IntField* _MinVoxelCount;

  IntField* _SubDivSize;

  //@}


  std::string usedValues;
  bool isCalculating;

  ObjMgrCommunicator* myObjMgr;

  void createMETKObjects();
  void calcBoundingBox();

  bool validBoundingBox;

  struct BoundingBox
  {
    vec3 min;
    vec3 max;
    double xWidth;
    double yWidth;
    double zWidth;
  };

  BoundingBox myBoundingBox;

  HashTable<long> voxelsPerValue;

  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(METKCreateIntraOPDummy)

};


ML_END_NAMESPACE

#endif // __mlMETKCreateIntraOPDummy_H


