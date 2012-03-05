//----------------------------------------------------------------------------------
//! The ML module class SegmentationEvaluationMetric.
/*!
// \file    mlSegmentationEvaluationMetric.cpp
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

// Local includes
#include "mlSegmentationEvaluationMetric.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(SegmentationEvaluationMetric, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
SegmentationEvaluationMetric::SegmentationEvaluationMetric ()
  : BaseOp(2, 0)
{
  ML_TRACE_IN("SegmentationEvaluationMetric::SegmentationEvaluationMetric ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();

  // Add fields to the module and set their values.
  // Also attach them to the output images to notify connected modules about changes.

  (_segmentationThresholdFld = fields.addInt("segmentationThreshold"))->setIntValue(0);
  (_referenceThresholdFld = fields.addInt("referenceThreshold"))->setIntValue(0);
  
  (_truePositiveFld = fields.addInt("truePositive"))->setIntValue(0);
  (_trueNegativeFld = fields.addInt("trueNegative"))->setIntValue(0);
  (_falsePositiveFld = fields.addInt("falsePositive"))->setIntValue(0);
  (_falseNegativeFld = fields.addInt("falseNegative"))->setIntValue(0);
  (_sensitivityFld = fields.addDouble("sensitivity"))->setDoubleValue(0.0);
  (_specificityFld = fields.addDouble("specificity"))->setDoubleValue(0.0);
  (_prevalenceFld = fields.addDouble("prevalence"))->setDoubleValue(0.0);
  (_levelOfTestFld = fields.addDouble("levelOfTest"))->setDoubleValue(0.0);
  (_diceSimilarityCoefficientFld = fields.addDouble("diceSimilarityCoefficient"))->setDoubleValue(0.0);
  (_cFactorFld = fields.addDouble("cFactor"))->setDoubleValue(0.0);

  (_isAutoApplyFld = fields.addBool("isAutoApply"))->setBoolValue(false);
  _applyFld = fields.addNotify("apply");

  // Connect input field(s) with output field(s) to notify
  // connected modules if input image(s) change.
  for (int inIdx=0; inIdx < 2; ++inIdx){
    for (int outIdx=0; outIdx < 0; ++outIdx){
      getInField(inIdx)->attachField(getOutField(outIdx));
    }
  }

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
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void SegmentationEvaluationMetric::handleNotification (Field *field)
{
  ML_TRACE_IN("SegmentationEvaluationMetric::handleNotification ()");

  if (_isAutoApplyFld->isOn() || (field == _applyFld)) {
    if (getUpdatedInImg(0) && getUpdatedInImg(1) ) {
	    _process();
    } else {
      _reset();
    }
  }
}

//----------------------------------------------------------------------------------
//! Update internal module state after load or clone and enable notification handling again.
//----------------------------------------------------------------------------------
void SegmentationEvaluationMetric::activateAttachments ()
{
  ML_TRACE_IN("SegmentationEvaluationMetric::activateAttachments ()");

  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  handleNotification(NULL);
  BaseOp::activateAttachments ();
}

//----------------------------------------------------------------------------------
//! Calls correctly typed (template) version of \c calcOutSubImage to calculate page 
//! \c outSubImg of output image with index \c outSubImg.
//----------------------------------------------------------------------------------
CALC_OUTSUBIMAGE2_CPP(SegmentationEvaluationMetric);

//----------------------------------------------------------------------------------
//! Template for type specific page calculation. Called by 
//! CALC_OUTSUBIMAGE2_CPP(SegmentationEvaluationMetric).
//----------------------------------------------------------------------------------
template <typename T>
void SegmentationEvaluationMetric::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex,
                                     TSubImg<T> *inSubImg1,
                                     TSubImg<T> *inSubImg2)
{
  ML_TRACE_IN("template <typename T> SegmentationEvaluationMetric::calcOutSubImage ()")
  
  outSubImg;
  int segThreshold = _segmentationThresholdFld->getIntValue();
  int refThreshold = _referenceThresholdFld->getIntValue();

  // called by processAllPages for the virtual output image with the index -1
  if (outIndex == -1)  {
	  // Clamp page box to valid image regions since pages can reach outside the image.
	  SubImgBox box = SubImgBox::intersect(inSubImg1->getBox(), getInImg(0)->getBoxFromImgExt());

	  // Scan all voxels in both input subimages which have identical
      // extents (as specified in calcInSubImageProps).
	  Vector p = box.v1;
	  for (p.u = box.v1.u;   p.u <= box.v2.u;  p.u++)
	  {
		for (p.t = box.v1.t;   p.t <= box.v2.t;  p.t++)
		{
			for (p.c = box.v1.c;   p.c <= box.v2.c;  p.c++)
			{
				for (p.z = box.v1.z;  p.z <= box.v2.z;  p.z++)
				{
					for (p.y = box.v1.y;  p.y <= box.v2.y;  p.y++)
					{

					  // Calculate first voxel position in x row.
					  p.x = box.v1.x;

					  // Get pointers to first voxels in x-rows of all input subimages.
					  T* i1P = inSubImg1->getImgPos(p); 
					  T* i2P = inSubImg2->getImgPos(p); 

					  // Iterate over all voxels in input lines.
					  for (;  p.x <= box.v2.x;  p.x++)
					  {
						T val1 = (*i1P); // segmentation
						T val2 = (*i2P); // reference (ground truth) segmentation

						// true positive
						if ( (val1 > segThreshold) && (val2 > refThreshold) ) {
							_numTruePositive++;
						}
						// true negative
						if ( (val1 <= segThreshold) && (val2 <= refThreshold) ) {
							_numTrueNegative++;
						}
						// false positive
						if ( (val1 > segThreshold) && (val2 <= refThreshold) ) {
							_numFalsePositive++;
						}
						// false negative
						if ( (val1 <= segThreshold) && (val2 > refThreshold) ) {
							_numFalseNegative++;
						}
						// Move pointers forward in all input images.
						++i1P;
						++i2P;
					  }
					}
				}
			}
		}
	  }
  }
}


//----------------------------------------------------------------------------------
//! Main routine.
//----------------------------------------------------------------------------------
void SegmentationEvaluationMetric::_process()
{
	ML_TRACE_IN("SegmentationEvaluationMetric::_process ()");

	_numTruePositive = 0;
	_numTrueNegative = 0;
	_numFalsePositive = 0;
	_numFalseNegative = 0;
	double sensitivity = 0.0;
	double specificity = 0.0;
	unsigned int numVoxels = 0; 
	double prevalence = 0.0;
	double levelOfTest = 0.0;
	double dsc = 0.0;
	double cFactor = 0.0;
	
		


	// process all pages
    MLErrorCode err = processAllPages(-1);
  
	if (err==ML_RESULT_OK) {
		
		// sensitivity
		if ( (_numTruePositive + _numFalseNegative) > 0) {
			sensitivity = _numTruePositive / (double)(_numTruePositive+_numFalseNegative);
		}

		// specificity
		if ( (_numTrueNegative + _numFalsePositive) > 0) {
			specificity = _numTrueNegative / (double) (_numTrueNegative+_numFalsePositive);
		}

		// total number of voxels
		numVoxels = _numTruePositive + _numFalsePositive + _numFalseNegative + _numTrueNegative;
		
		// prevalence
		if (numVoxels > 0) {
			prevalence = (_numTruePositive + _numFalseNegative) / (double)numVoxels;
		}

		// level of test
		if (numVoxels > 0) {
			levelOfTest = (_numTruePositive + _numFalsePositive) / (double)numVoxels;
		}
		
		// Dice similarity coefficient
		if ( 2*_numTruePositive + _numFalsePositive + _numFalseNegative > 0) {
			dsc = 2.0 * _numTruePositive / (double)(2.0*_numTruePositive + _numFalsePositive + _numFalseNegative);
		}
		
		// C-Factor
		double p = sensitivity;
		double q = specificity;
		double d = (2.0 * p * (1 - q)) / (p + (1-q)) + (2 * (1-p) * q) / ((1-p) + p);
		
		if ( (p >= q) && (p > 1-q) ) {
			cFactor = d;
		}
		else if ( (p < q) && (p > 1-q) ) {
			cFactor = -d;
		}
		else if ( (p <= 1-q) ) {
			// undefined
		}
    }
    else {
        // Set error status.
		std::cerr << MLGetErrorCodeDescription(err);
    }

	// set output
	_truePositiveFld->setIntValue(_numTruePositive);
	_trueNegativeFld->setIntValue(_numTrueNegative);
	_falsePositiveFld->setIntValue(_numFalsePositive);
	_falseNegativeFld->setIntValue(_numFalseNegative);
	_sensitivityFld->setDoubleValue(sensitivity);
	_specificityFld->setDoubleValue(specificity);
	_prevalenceFld->setDoubleValue(prevalence);
	_levelOfTestFld->setDoubleValue(levelOfTest);
	_diceSimilarityCoefficientFld->setDoubleValue(dsc);
	_cFactorFld->setDoubleValue(cFactor);
}

//----------------------------------------------------------------------------------
//! Reset parameters
//----------------------------------------------------------------------------------
void SegmentationEvaluationMetric::_reset()
{
  ML_TRACE_IN("SegmentationEvaluationMetric::_reset()");

  _truePositiveFld->setIntValue(0);
  _trueNegativeFld->setIntValue(0);
  _falsePositiveFld->setIntValue(0);
  _falseNegativeFld->setIntValue(0);
  _sensitivityFld->setDoubleValue(0.0);
  _specificityFld->setDoubleValue(0.0);
  _prevalenceFld->setDoubleValue(0.0);
  _levelOfTestFld->setDoubleValue(0.0);
  _diceSimilarityCoefficientFld->setDoubleValue(0.0);
  _cFactorFld->setDoubleValue(0.0);
}

ML_END_NAMESPACE

