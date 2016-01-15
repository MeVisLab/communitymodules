//----------------------------------------------------------------------------------
//! The ML module class SegmentationEvaluationMetric.
/*!
// \file    mlSegmentationEvaluationMetric.h
// \author  Stefan Saur
// \date    2009-07-21
//
// Validatation metric for accuracy assessment in medical image segmentation as described in
// 
// "Statistical validation metric for accuracy assessment in medical image segmentation"
// Aleksandra Popovic, Matias de la Fuente, Martin Engelhardt, Klaus Radermacher
// Int J CARS, 2007(2):169-181
// DOI 10.1007/s11548-007-0125-1
*/
//----------------------------------------------------------------------------------


#ifndef __mlSegmentationEvaluationMetric_H
#define __mlSegmentationEvaluationMetric_H


// Local includes
#include "CVLSegmentationEvaluationMetricSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE


//! 
class CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT SegmentationEvaluationMetric : public BaseOp
{
public:

  //! Constructor.
  SegmentationEvaluationMetric ();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

  //! Update internal module state after load or clone and enable notification handling again.
  virtual void activateAttachments ();

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
  //! \param inSubImg2 Temporary subimage of input 2.
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex,
                        TSubImg<T> *inSubImg1,
                        TSubImg<T> *inSubImg2);


private:

  //! Main routine.
  void _process();

  //! reset parameters on empty input
  void _reset();

  //! Number of true positives.
  unsigned int _numTruePositive;
  //! Number of true negatives.
  unsigned int _numTrueNegative;
  //! Number of false positives.  
  unsigned int _numFalsePositive;
  //! Number of false negatives.
  unsigned int _numFalseNegative;
  

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! Threshold for segmentation image.
  IntField *_segmentationThresholdFld;
  //! Threshold for reference image.
  IntField *_referenceThresholdFld;
  //! True positive.
  IntField *_truePositiveFld;
  //! True negative.
  IntField *_trueNegativeFld;
  //! False positive.
  IntField *_falsePositiveFld;
  //! False negative.
  IntField *_falseNegativeFld;
  //! Sensitivity.
  DoubleField *_sensitivityFld;
  //! Specificity.
  DoubleField *_specificityFld;
  //! Prevalence.
  DoubleField *_prevalenceFld;
  //! Level of Test.
  DoubleField *_levelOfTestFld;
  //! Dice Similarity Coefficient
  DoubleField *_diceSimilarityCoefficientFld;
  //! c-Factor.
  DoubleField *_cFactorFld;
  
  //! Apply.
  NotifyField *_applyFld;
  //! is auto apply.
  BoolField *_isAutoApplyFld;


  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(SegmentationEvaluationMetric)
};


ML_END_NAMESPACE

#endif // __mlSegmentationEvaluationMetric_H

