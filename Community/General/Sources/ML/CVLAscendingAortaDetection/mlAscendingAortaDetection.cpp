//----------------------------------------------------------------------------------
//! The ML module class AscendingAortaDetection.
/*!
// \file    mlAscendingAortaDetection.h
// \author  Stefan Saur
// \date    2007-03-24
//
// as published and described in
// "Automatic Ascending Aorta Detection in CTA Datasets"
// ftp://ftp.vision.ee.ethz.ch/publications/proceedings/eth_biwi_00536.pdf
//
// @InProceedings{eth_biwi_00536,
//   author = {Stefan C. Saur and Caroline K\"uhnel and Tobias Boskamp and Gábor Székely and Philippe Cattin},
//   title = {Automatic Ascending Aorta Detection in CTA Datasets},
//   booktitle = {Bildverarbeitung für der Medizin 2008},
//   year = {2008},
//   month = {April},
//   pages = {323-327}}
//
// This code was created by support of the research project Co-Me (http://co-me.ch) 
// at the Computer Vision Laboratory of ETH Zurich (http://www.vision.ee.ethz.ch)
//
// YOU MUST CITE THIS PAPER WHEN USING THE ALGORITHM IN YOUR PUBLICATION
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlAscendingAortaDetection_H
#include "mlAscendingAortaDetection.h"
#endif

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(AscendingAortaDetection, BaseOp);


bool sortClusterPoints(XMarker elem1, XMarker elem2)
{
	return (elem1.z() < elem2.z());
}


//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
AscendingAortaDetection::AscendingAortaDetection (void)
  : BaseOp(1, 0) // in, out
{
  ML_TRACE_IN("AscendingAortaDetection::AscendingAortaDetection()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  // input parameters
  // primary threshold th_p in gray values
  (_primaryThresholdFld = fields->addInt("primaryThreshold"))->setIntValue(1204);
  // secondary threshold th_s in gray values
  (_secondaryThresholdFld = fields->addInt("secondaryThreshold"))->setIntValue(1124);
  // minimum diameter d_min in mm
  (_minDiameterFld = fields->addInt("minDiameter"))->setIntValue(20);
  // maximum diameter d_max in mm
  (_maxDiameterFld = fields->addInt("maxDiameter"))->setIntValue(70);
  // increment of search ray delta_x, delta_y, delta_z
  (_incrementFld = fields->addVec3f("increment"))->setVec3fValue(vec3(1.0, 5.0, 1.0));
  // search range VOI_x, VOI_y, VOI_z
  (_startSearchAreaFld = fields->addVec3f("startSearchArea"))->setVec3fValue(vec3(0.0, 0.25, 0.5));
  (_endSearchAreaFld = fields->addVec3f("endSearchArea"))->setVec3fValue(vec3(0.75, 0.70, 1.0));
  // ratio a
  (_ratioLimitFld = fields->addDouble("ratioLimit"))->setDoubleValue(0.25);
  (_useRatioLimitFld = fields->addBool("useRatioLimit"))->setBoolValue(true);
  // distance limit for clustering d_clust
  (_distanceClusterFld = fields->addVec3f("distanceCluster"))->setVec3fValue(vec3(5.0, 5.0, 5.0));

  // apply
  (_applyFld = fields->addNotify("apply"));
  (_autoApplyFld = fields->addBool("autoApply"))->setBoolValue(false);
  
  // output
  (_outXMarkerListFld = fields->addBase("outXMarkerList"))->setBaseValue(&_outXMarkerList);
  (_outXMarkerListLargestClusterFld = fields->addBase("outXMarkerListLargestCluster"))->setBaseValue(&_outLargestCluster);
  (_outXMarkerListSecondLargestClusterFld = fields->addBase("outXMarkerListSecondLargestCluster"))->setBaseValue(&_outSecondLargestCluster);

  // seedpoint for subsequent aorta segmentation as described in paper
  (_seedPointFld = fields->addVec3f("seedPoint"))->setVec3fValue(vec3(0, 0, 0));
  
  // primary / largest cluster
  // size of cluster i.e. number of candidate points
  (_priClusterSizeFld = fields->addInt("priClusterSize"))->setIntValue(0);
  // mean intensity within bounding box
  (_priMeanIntensityFld = fields->addDouble("priMeanIntensity"))->setDoubleValue(0.0);
  // mean intensity within bounding box
  (_priStdIntensityFld = fields->addDouble("priStdIntensity"))->setDoubleValue(0.0);
  // barycenter of cluster
  (_priBaryCenterFld = fields->addVec3f("priBaryCenter"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  // center point at lower extend (z smallest) of cluster
  (_priLowerCenterFld = fields->addVec3f("priLowerCenter"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  // center point at upper extend (z largest) of cluster
  (_priUpperCenterFld = fields->addVec3f("priUpperCenter"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  // radius at bary center of cluster
  (_priRadiusBCFld = fields->addVec2f("priRadiusBC"))->setVec2fValue(vec2(0.0, 0.0));
  // bounding box for cluster (see computation on how it is defined)
  (_priBBLowerFld = fields->addVec3f("priBBLower"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  (_priBBUpperFld = fields->addVec3f("priBBUpper"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  // exentsion in z direction in mm
  (_extensionLargestClusterFld = fields->addDouble("extensionLargestCluster"))->setDoubleValue(0.0);
  // mean radius in mm of candidates points within largest cluster
  (_meanRadiusLargestClusterFld = fields->addDouble("meanRadiusLargestCluster"))->setDoubleValue(0.0);
  // std of radius in mm of candidates points within largest cluster
  (_stdRadiusLargestClusterFld = fields->addDouble("stdRadiusLargestCluster"))->setDoubleValue(0.0);
  

  // secondary / second largest cluster
  // description equivalent to largest cluster
  (_secClusterSizeFld = fields->addInt("secClusterSize"))->setIntValue(0);
  (_secMeanIntensityFld = fields->addDouble("secMeanIntensity"))->setDoubleValue(0.0);
  (_secStdIntensityFld = fields->addDouble("secStdIntensity"))->setDoubleValue(0.0);
  (_secBaryCenterFld = fields->addVec3f("secBaryCenter"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  (_secLowerCenterFld = fields->addVec3f("secLowerCenter"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  (_secUpperCenterFld = fields->addVec3f("secUpperCenter"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  (_secRadiusBCFld = fields->addVec2f("secRadiusBC"))->setVec2fValue(vec2(0.0, 0.0));
  (_secBBLowerFld = fields->addVec3f("secBBLower"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  (_secBBUpperFld = fields->addVec3f("secBBUpper"))->setVec3fValue(vec3(0.0, 0.0, 0.0));
  (_extensionSecondLargestClusterFld = fields->addDouble("extensionSecondLargestCluster"))->setDoubleValue(0.0);
  (_meanRadiusSecondLargestClusterFld = fields->addDouble("meanRadiusSecondLargestCluster"))->setDoubleValue(0.0);
  (_stdRadiusSecondLargestClusterFld = fields->addDouble("stdRadiusSecondLargestCluster"))->setDoubleValue(0.0);

  // info
  (_infoFld = fields->addString("info"))->setStringValue("");

  // statistic
  (_runTimeFld = fields->addDouble("runTime"))->setDoubleValue(0.0);
  
  _applyFld->attachField(_outXMarkerListFld);
  _applyFld->attachField(_outXMarkerListLargestClusterFld);
  _applyFld->attachField(_outXMarkerListSecondLargestClusterFld);
 
  _virtInVol = NULL;
  _outXMarkerList.clear();
  _outLargestCluster.clear();
  _outSecondLargestCluster.clear();

  
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Destructor
//----------------------------------------------------------------------------------
AscendingAortaDetection::~AscendingAortaDetection(void)
{
	ML_TRACE_IN("AscendingAortaDetection::~AscendingAortaDetection()")
	ML_DELETE(_virtInVol);
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void AscendingAortaDetection::handleNotification (Field *field)
{
  ML_TRACE_IN("AscendingAortaDetection::handleNotification()")
  	  
  if ( (field == _applyFld) && (getUpdatedInImg(0)) ) {
	_process();
  }
  if ((field == getInField(0)) && getUpdatedInImg(0) && _autoApplyFld->getBoolValue() ) {
	_process();
  }
}


//----------------------------------------------------------------------------------
//! Main routine for the detection of the ascending aorta.
//----------------------------------------------------------------------------------
void AscendingAortaDetection::_process()
{
	ML_TRACE_IN("AscendingAortaDetection::_process()")
	_ss.str("");

	// ++++++++++++++ TIME MEASUREMENT START ++++++++++++++ //	
    TimeCounter timeWhole;

	// any problems creating the virtual volume from the input volume?
    if (!_initVirtualVolumes()) {
		return;
	}

  
	// now we can process
	int i, j; // loop variables
	
	// virtual volume is casted to MLuint16    
    TVirtualVolume<MLuint16> vInVol(*_virtInVol);

	_outXMarkerList.clear();
	
	Vector imgExt  = getInImg(0)->getImgExt();
	vec3   voxSize = getInImg(0)->getVoxelSize();
	
	vec3   startArea = _startSearchAreaFld->getVec3fValue(); // parameter where to start looking for aorta
	vec3   endArea   = _endSearchAreaFld->getVec3fValue();   // parameter where to stop looking for aorta

    TimeCounter timeCandidates;

	// read thresholds
    unsigned primeThreshold = _primaryThresholdFld->getIntValue();
	unsigned secondThreshold = _secondaryThresholdFld->getIntValue();

  
    // determine search line distance
	vec3 increment = _incrementFld->getVec3fValue();
	// convert mm to voxcel unit
	int incX = increment[0] / voxSize[0];
	int incY = increment[1] / voxSize[1];
	int incZ = increment[2] / voxSize[2];

	// minimum step size is 1
	if (incX < 1) {
		incX = 1;
	}

	if (incY < 1) {
		incY = 1;
	}
	
	if (incZ < 1) {
		incZ = 1;
	}

	_ss << "search ray increment " << incX << " | " << incY << " | " << incZ << " voxels" << std::endl;
	
	// search range in z direction
	for (int z = imgExt[2]*startArea[2]; z < imgExt[2]*endArea[2]; z+=incZ)
	{
		// search range in y direction
		for (int y = imgExt[1]*startArea[1]; y < imgExt[1]*endArea[1]; y+=incY)
		{
			int startX = 0;
			int stopX = 0;
			bool isPrimaryThreshold = false;
			
			// search range in x direction
			for (int x = imgExt[0]*startArea[0]; x < imgExt[0]*endArea[0]; x+=incX)
			{
				int value = vInVol.getVoxel(Vector(x, y, z, 0, 0, 0));
				
				// primary threshold is obtained for the first time for a object
				if ( (!isPrimaryThreshold) && (value > _primaryThresholdFld->getIntValue()) ) {
					isPrimaryThreshold = true;
					startX = x;
					stopX = x;
				}
				// candidate mode, primary threshold was triggered, looking for transition to secondary threshold
				else if ( (isPrimaryThreshold) && (value > _primaryThresholdFld->getIntValue()) ) {
					stopX = x;
				}
				// transition under secondary threshold
				else if ( (isPrimaryThreshold) && (value < _secondaryThresholdFld->getIntValue()) ) {
					double diameterX = (stopX - startX) * voxSize[0];
					// check if diameter is within allowed range for aorta 
					if ( (diameterX > _minDiameterFld->getIntValue()) && (diameterX < _maxDiameterFld->getIntValue()) ) {
						int centerX = (stopX+startX)/2;
						
						// now look in y direction for this point
						int startY = y;
						int stopY = y;
						int loopY = y; // just a loop variable

						int value = vInVol.getVoxel(Vector(centerX, loopY, z, 0, 0, 0));
						// first look for transition to the front
						while ( (loopY>0) && (value > _secondaryThresholdFld->getIntValue()) )
						{
							if (value > _primaryThresholdFld->getIntValue()) {
								startY--;
							}
							loopY--;
							value = vInVol.getVoxel(Vector(centerX, loopY, z, 0, 0, 0));
						}

						// now look for transition to the back
						loopY = y;
						value = vInVol.getVoxel(Vector(centerX, loopY, z, 0, 0, 0));
						while ( (loopY<imgExt[1]-1) && (value > _secondaryThresholdFld->getIntValue()) )
						{
							if (value > _primaryThresholdFld->getIntValue()) {
								stopY++;
							}
							loopY++;
							value = vInVol.getVoxel(Vector(centerX, loopY, z, 0, 0, 0));
						}

						double diameterY = (stopY - startY) * voxSize[1];

						// check if diameter is within allowed range for aorta 
						if ( (diameterY > _minDiameterFld->getIntValue()) && (diameterY < _maxDiameterFld->getIntValue()) )
						{
							bool isRatioLimitOk = true;
							
							// should ratio limit be considered
							if (_useRatioLimitFld->isOn()) {
								isRatioLimitOk = (fabs(diameterY / diameterX - 1.0) < _ratioLimitFld->getDoubleValue() );
							}

							if (isRatioLimitOk) {
								int centerY = (stopY+startY)/2;
								vec3 voxPos = vec3(centerX, centerY, z);
								
								double diameterDiag1 = _singleRayBothDirections(voxPos, vec3(1, -1, 0), imgExt, primeThreshold, secondThreshold, voxSize);
								double diameterDiag2 = _singleRayBothDirections(voxPos, vec3(1, 1, 0), imgExt, primeThreshold, secondThreshold, voxSize);

								if ( (diameterDiag1 > _minDiameterFld->getIntValue()) && (diameterDiag1 < _maxDiameterFld->getIntValue())
									&& (diameterDiag2 > _minDiameterFld->getIntValue()) && (diameterDiag2 < _maxDiameterFld->getIntValue())) {
									vec3 worldPos;
									getInImg(0)->transformToWorldCoord(voxPos, worldPos);
									XMarker tmpMarker = XMarker(worldPos);
									tmpMarker.vec[0] = diameterX / 2.0;
									tmpMarker.vec[1] = diameterY / 2.0;
									tmpMarker.type = 0;
									_outXMarkerList.push_back(tmpMarker);							
								}
							}
						}					
					}
					
					isPrimaryThreshold = false;
				}
			}
		}
	}

  	_ss << "TIME AscendingAortaDetection.GetCandidates : " << timeCandidates.getRunningTimeInSeconds() << std::endl;
 
  
    TimeCounter timeClustering;

	
	// make clusters (assume a maximum number of 512 clusters)
	int counter[512];
	for (j=0; j<512; j++) {
		counter[j] = 0;
	}

	int currClusterLabel = 1;					
			
	for (i=0; i<_outXMarkerList.getSize(); i++)
	{
		XMarker *currXMarker = (XMarker*)_outXMarkerList.getItemAt(i);
		int tmpClusterLabel;
		// XMarker already belongs to cluster
		if (currXMarker->type > 0) { 
			tmpClusterLabel = currXMarker->type;
		}
		// new cluster
		else {
			currXMarker->type = currClusterLabel;
			tmpClusterLabel = currClusterLabel;
			currClusterLabel++;
		}

		// merge clusters if distance is within an ellipsoid
		// 
		// ellipsoid equation
		// x^2/a^2 + y^2/b^2 + z^2/c^2 = 1

		vec3 distanceCluster = _distanceClusterFld->getVec3fValue();
		double a = distanceCluster[0];
		double b = distanceCluster[1];
		double c = distanceCluster[2];

		for (j=0; j<_outXMarkerList.getSize(); j++)
		{
			if (j!=i) {
				XMarker *tmpXMarker = (XMarker*)_outXMarkerList.getItemAt(j);
				// compute distance between two XMarkers
				// XMarker positions are stored in world coordinates
				double distanceSquare = (currXMarker->x()-tmpXMarker->x()) * (currXMarker->x()-tmpXMarker->x()) / (a*a)   + 
										  (currXMarker->y()-tmpXMarker->y()) * (currXMarker->y()-tmpXMarker->y()) / (b*b) +
										  (currXMarker->z()-tmpXMarker->z()) * (currXMarker->z()-tmpXMarker->z()) / (c*c);

				// if distance smaller than threshold
				if (distanceSquare < 1) {
					if (tmpXMarker->type == 0) {
						tmpXMarker->type = tmpClusterLabel;
					}
					else {
						_mergeClusters(tmpXMarker->type, tmpClusterLabel);
					}
				}
			}
		}
	}

	for (i=0; i<_outXMarkerList.getSize(); i++)
	{
		counter[((XMarker*)_outXMarkerList.getItemAt(i))->type]++;
	}
					
	
	int maxNumber = 0;
	int maxLabel = 0;
	int secMaxNumber = 0;
	int secMaxLabel = 0;


	// determine largest and second largest cluster
	for (i=0; i<currClusterLabel; i++)
	{
		_ss << "cluster " << i+1 << " - size: " << counter[i] << std::endl;

		if (counter[i] > secMaxNumber) {
			secMaxNumber = counter[i];
			secMaxLabel = i;
		}

		if (counter[i] > maxNumber) {
			secMaxNumber = maxNumber;
			secMaxLabel = maxLabel;
			maxNumber = counter[i];
			maxLabel = i;
		}
	}

	_ss << "-------------------------------------" << std::endl;
	_ss << "largest cluster : " << maxLabel << " ( " << maxNumber << " elements) " << std::endl;
	_ss << "second largest cluster : " << secMaxLabel << " ( " << secMaxNumber << " elements) " << std::endl;
	
	
	_outLargestCluster.clear();
	_outSecondLargestCluster.clear();
	
	// fill up largest cluster and second largest cluster list
	for (i=0; i<_outXMarkerList.getSize(); i++)
	{
		XMarker *tmpMarker = (XMarker*)_outXMarkerList.getItemAt(i);
		if (tmpMarker->type == maxLabel) {
			_outLargestCluster.push_back(*tmpMarker);
		}
		if (tmpMarker->type == secMaxLabel) {
			_outSecondLargestCluster.push_back(*tmpMarker);
		}
	}



	// get 90% quantile
	std::sort(_outLargestCluster.begin(), _outLargestCluster.end(), sortClusterPoints);
	
	int pIndex = (_outLargestCluster.size() * 0.9);
	
	if (_outLargestCluster.size()) {
		XMarker pMarker = _outLargestCluster.at(pIndex);
		_seedPointFld->setVec3fValue(vec3(pMarker.x(), pMarker.y(), pMarker.z()));
	} 
	else {
		_seedPointFld->setVec3fValue(vec3(-1.0, -1.0, -1.0));
	}
	_seedPointFld->notifyAttachments();

	
	// ++++++++++++++ TIME MEASUREMENT END ++++++++++++++ //
	_runTimeFld->setDoubleValue(timeWhole.getRunningTimeInSeconds());
	_ss << "TIME AscendingAortaDetection.Clustering : " << timeClustering.getRunningTimeInSeconds() << std::endl;
	_ss << "TIME AscendingAortaDetection : " << timeWhole.getRunningTimeInSeconds() << std::endl;

_infoFld->setStringValue(_ss.str());
	// compute statistics about largest and second largest cluster
	// pri = primary -> largestCluster
	// sec = secondoary -> secondLargestCluster

	vec3 priBaryCenter;
	vec3 priLowerCenter;
	vec3 priUpperCenter;
	double priMean;
	double priStd;
	vec2 priRadiusBC;
	vec3 priBBLower;
	vec3 priBBUpper;

	_computeBB(_outLargestCluster, priBaryCenter, priLowerCenter, priUpperCenter, priMean, priStd, priRadiusBC, priBBLower, priBBUpper);
	_priClusterSizeFld->setIntValue(_outLargestCluster.getSize());
	_priMeanIntensityFld->setDoubleValue(priMean);
	_priStdIntensityFld->setDoubleValue(priStd);
	_priBaryCenterFld->setVec3fValue(priBaryCenter);
	_priLowerCenterFld->setVec3fValue(priLowerCenter);
	_priUpperCenterFld->setVec3fValue(priUpperCenter);
	_priRadiusBCFld->setVec2fValue(priRadiusBC);
	_priBBLowerFld->setVec3fValue(priBBLower);
	_priBBUpperFld->setVec3fValue(priBBUpper);

	vec3 secBaryCenter;
	vec3 secLowerCenter;
	vec3 secUpperCenter;
	double secMean;
	double secStd;
	vec2 secRadiusBC;
	vec3 secBBLower;
	vec3 secBBUpper;

	_computeBB(_outSecondLargestCluster, secBaryCenter, secLowerCenter, secUpperCenter, secMean, secStd, secRadiusBC, secBBLower, secBBUpper);
	_secClusterSizeFld->setIntValue(_outSecondLargestCluster.getSize());
	_secMeanIntensityFld->setDoubleValue(secMean);
	_secStdIntensityFld->setDoubleValue(secStd);
	_secBaryCenterFld->setVec3fValue(secBaryCenter);
	_secLowerCenterFld->setVec3fValue(secLowerCenter);
	_secUpperCenterFld->setVec3fValue(secUpperCenter);
	_secRadiusBCFld->setVec2fValue(secRadiusBC);
	_secBBLowerFld->setVec3fValue(secBBLower);
	_secBBUpperFld->setVec3fValue(secBBUpper);

	double extMinZ = DBL_MAX;
	double extMaxZ = DBL_MIN;

	for (int i=0; i<_outLargestCluster.getSize(); i++)
	{
		XMarker *tmpMarker = (XMarker*)_outLargestCluster.getItemAt(i);
		
		if (tmpMarker->z() < extMinZ) {
			extMinZ = tmpMarker->z();
		}

		if (tmpMarker->z() > extMaxZ) {
			extMaxZ = tmpMarker->z();
		}
	}
	_extensionLargestClusterFld->setDoubleValue(extMaxZ-extMinZ);
	
	extMinZ = DBL_MAX;
	extMaxZ = DBL_MIN;
	double minY = 0.0;
	double minX = 0.0;

	for (int i=0; i<_outSecondLargestCluster.getSize(); i++)
	{
		XMarker *tmpMarker = (XMarker*)_outSecondLargestCluster.getItemAt(i);
		
		if (tmpMarker->z() < extMinZ) {
			extMinZ = tmpMarker->z();
			minX = tmpMarker->x();
			minY = tmpMarker->y();
		}

		if (tmpMarker->z() > extMaxZ) {
			extMaxZ = tmpMarker->z();
		}

	}
	
	_secLowerCenterFld->setVec3fValue(vec3(minX, minY, extMinZ));
	_extensionSecondLargestClusterFld->setDoubleValue(extMaxZ-extMinZ);
	
	_statistics();
	_infoFld->setStringValue(_ss.str());

	_ss << "TIME AscendingAortaDetection.statistics: " << timeWhole.getRunningTimeInSeconds() << std::endl;
	
}

//----------------------------------------------------------------------------------
//! Compute bounding box and some other statistics of given cluster.
//----------------------------------------------------------------------------------
void AscendingAortaDetection::_computeBB(XMarkerList &_list, vec3 &baryCenter, vec3 &lowerCenter, vec3 &upperCenter, double &mean, double &std, vec2 &radiusBC, vec3 &bbLower, vec3 &bbUpper)
{
	TVirtualVolume<MLuint16> vInVol(*_virtInVol);
	
	if (_list.getSize() == 0) {
		return;
	}

	baryCenter = vec3(0, 0, 0);
	double minZCoord = DBL_MAX;
	double maxZCoord = -DBL_MAX;
	for (int i=0; i<_list.getSize(); i++)
	{
		XMarker *tmpMarker = (XMarker*)_list.getItemAt(i);
		baryCenter[0] += tmpMarker->x();
		baryCenter[1] += tmpMarker->y();
		baryCenter[2] += tmpMarker->z();

		if (tmpMarker->z() < minZCoord) {
			minZCoord = tmpMarker->z();
			lowerCenter = vec3(tmpMarker->x(), tmpMarker->y(), tmpMarker->z());
		}

		if (tmpMarker->z() > maxZCoord) {
			maxZCoord = tmpMarker->z();
			upperCenter = vec3(tmpMarker->x(), tmpMarker->y(), tmpMarker->z());
		}
	}
	baryCenter[0] /= (double)_list.getSize();
	baryCenter[1] /= (double)_list.getSize();
	baryCenter[2] /= (double)_list.getSize();

	// get radius of aorta at baryCenter
	vec3 baryCenterVox ;  // baryCenter in voxel coordinates
	getInImg(0)->transformToVoxelCoord(baryCenter, baryCenterVox);
	
	// radius at barycenter
	radiusBC[0] = 0.5 * _singleRayBothDirections(baryCenterVox, vec3(1, 0, 0), getInImg(0)->getImgExt(), _primaryThresholdFld->getIntValue(), _secondaryThresholdFld->getIntValue(), getInImg(0)->getVoxelSize());
	radiusBC[1] = 0.5 * _singleRayBothDirections(baryCenterVox, vec3(0, 1, 0), getInImg(0)->getImgExt(), _primaryThresholdFld->getIntValue(), _secondaryThresholdFld->getIntValue(), getInImg(0)->getVoxelSize());
	
	// extension of bounding box in xy-plane
	double bbExtension = _minDiameterFld->getIntValue()*0.5*0.6; // 0.5 to get the radius, 0.6 -> 60% of radius

	bool ok = false;

	while (!ok)
	{
		Vector corner[8];
		vec3 cornerWorld;
		vec3 cornerVoxel;

		cornerWorld = vec3(baryCenter[0]-bbExtension, baryCenter[1]-bbExtension, baryCenter[2]);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[0] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]+bbExtension, baryCenter[1]-bbExtension, baryCenter[2]);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[1] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]-bbExtension, baryCenter[1]+bbExtension, baryCenter[2]);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[2] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]+bbExtension, baryCenter[1]+bbExtension, baryCenter[2]);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[3] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]-bbExtension, baryCenter[1]-bbExtension, (baryCenter[2]+minZCoord)/2.0);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[4] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]+bbExtension, baryCenter[1]-bbExtension, (baryCenter[2]+minZCoord)/2.0);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[5] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]-bbExtension, baryCenter[1]+bbExtension, (baryCenter[2]+minZCoord)/2.0);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[6] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		cornerWorld = vec3(baryCenter[0]+bbExtension, baryCenter[1]+bbExtension, (baryCenter[2]+minZCoord)/2.0);
		getInImg(0)->transformToVoxelCoord(cornerWorld, cornerVoxel);
		corner[7] = Vector(cornerVoxel[0], cornerVoxel[1], cornerVoxel[2], 0, 0, 0);

		
		// check the 8 corners of the bounding box if their intensity is greater than the primary threshold
		if ( (vInVol.getVoxel(corner[0]) > _primaryThresholdFld->getIntValue()) 
			&& (vInVol.getVoxel(corner[1]) > _primaryThresholdFld->getIntValue())
			&& (vInVol.getVoxel(corner[2]) > _primaryThresholdFld->getIntValue())
			&& (vInVol.getVoxel(corner[3]) > _primaryThresholdFld->getIntValue())
			&& (vInVol.getVoxel(corner[4]) > _primaryThresholdFld->getIntValue())
			&& (vInVol.getVoxel(corner[5]) > _primaryThresholdFld->getIntValue())
			&& (vInVol.getVoxel(corner[6]) > _primaryThresholdFld->getIntValue())
			&& (vInVol.getVoxel(corner[7]) > _primaryThresholdFld->getIntValue()) ) {
			ok = true;
		}
		else {
			bbExtension -= 1.0;
		}
		if (bbExtension < 0) {
			ok = true;
			bbExtension = 0.0;
		}

	}

	bbLower = vec3(baryCenter[0]-bbExtension, baryCenter[1]-bbExtension, baryCenter[2]);
	bbUpper = vec3(baryCenter[0]+bbExtension, baryCenter[1]+bbExtension, (baryCenter[2]+minZCoord)/2.0);

	// compute statistics
	vec3 bbLowerVox;
	vec3 bbUpperVox;
	getInImg(0)->transformToVoxelCoord(bbLower, bbLowerVox);
	getInImg(0)->transformToVoxelCoord(bbUpper, bbUpperVox);

	bbLowerVox[0] = (int)bbLowerVox[0];
	bbLowerVox[1] = (int)bbLowerVox[1];
	bbLowerVox[2] = (int)bbLowerVox[2];
	bbUpperVox[0] = (int)bbUpperVox[0];
	bbUpperVox[1] = (int)bbUpperVox[1];
	bbUpperVox[2] = (int)bbUpperVox[2];

	for (int i=0; i<3; i++)
	{
		if (bbUpperVox[i] < bbLowerVox[i]) {
			int tmp = bbLowerVox[i];
			bbLowerVox[i] = bbUpperVox[i];
			bbUpperVox[i] = tmp;
		}
	}

	// mean
	mean = 0.0;
	unsigned meanCounter = 0;
	for (int z = bbLowerVox[2]; z <= bbUpperVox[2]; z++)
	{
		for (int y = bbLowerVox[1]; y <= bbUpperVox[1]; y++)
		{
			for (int x = bbLowerVox[0]; x <= bbLowerVox[0]; x++)
			{
				unsigned value = vInVol.getVoxel(Vector(x, y, z, 0, 0, 0));
				meanCounter++;
				mean += value;
			}
		}
	}
	mean /= (double)meanCounter;

	// standard deviation
	std = 0.0;
	for (int z = bbLowerVox[2]; z <= bbUpperVox[2]; z++)
	{
		for (int y = bbLowerVox[1]; y <= bbUpperVox[1]; y++)
		{
			for (int x = bbLowerVox[0]; x <= bbLowerVox[0]; x++)
			{
				unsigned value = vInVol.getVoxel(Vector(x, y, z, 0, 0, 0));
				std += (value - mean) * (value - mean);
			}
		}
	}
	std /= (double)meanCounter;
	std = sqrt(std);
}

//----------------------------------------------------------------------------------
//! Compute some additional statistics.
//----------------------------------------------------------------------------------
void AscendingAortaDetection::_statistics()
{
	// mean radius largest cluster
	double meanRadiusLargestCluster = 0.0;
	for (int i=0; i<_outLargestCluster.getSize(); i++) {
		XMarker *tmpMarker = (XMarker*)_outLargestCluster.getItemAt(i);
		meanRadiusLargestCluster += tmpMarker->vec[0];
		meanRadiusLargestCluster += tmpMarker->vec[1];
	}
	meanRadiusLargestCluster /= (2.0 * _outLargestCluster.getSize());
	_ss << "meanRadiusLargestCluster " << meanRadiusLargestCluster << std::endl;

	// mean radius second largest cluster
	double meanRadiusSecondLargestCluster = 0.0;
	for (int i=0; i<_outSecondLargestCluster.getSize(); i++) {
		XMarker *tmpMarker = (XMarker*)_outSecondLargestCluster.getItemAt(i);
		meanRadiusSecondLargestCluster += tmpMarker->vec[0];
		meanRadiusSecondLargestCluster += tmpMarker->vec[1];
	}
	meanRadiusSecondLargestCluster /= (2.0 * _outSecondLargestCluster.getSize());
	_ss << "meanRadiusSecondLargestCluster " << meanRadiusSecondLargestCluster << std::endl;
	
	// std radius largest cluster
	double stdRadiusLargestCluster = 0.0;
	for (int i=0; i<_outLargestCluster.getSize(); i++)
	{
		XMarker *tmpMarker = (XMarker*)_outLargestCluster.getItemAt(i);
		stdRadiusLargestCluster += (tmpMarker->vec[0]-meanRadiusLargestCluster) * (tmpMarker->vec[0]-meanRadiusLargestCluster);
		stdRadiusLargestCluster += (tmpMarker->vec[1]-meanRadiusLargestCluster) * (tmpMarker->vec[1]-meanRadiusLargestCluster);
	}
	stdRadiusLargestCluster /= (2.0 * _outLargestCluster.getSize());
	stdRadiusLargestCluster = sqrt(stdRadiusLargestCluster);
	_ss << "stdRadiusLargestCluster " << stdRadiusLargestCluster << std::endl;

	// std radius second largest cluster
	double stdRadiusSecondLargestCluster = 0.0;
	for (int i=0; i<_outSecondLargestCluster.getSize(); i++)
	{
		XMarker *tmpMarker = (XMarker*)_outSecondLargestCluster.getItemAt(i);
		stdRadiusSecondLargestCluster += (tmpMarker->vec[0]-meanRadiusSecondLargestCluster) * (tmpMarker->vec[0]-meanRadiusSecondLargestCluster);
		stdRadiusSecondLargestCluster += (tmpMarker->vec[1]-meanRadiusSecondLargestCluster) * (tmpMarker->vec[1]-meanRadiusSecondLargestCluster);
	}
	stdRadiusSecondLargestCluster /= (2.0 * _outSecondLargestCluster.getSize());
	stdRadiusSecondLargestCluster = sqrt(stdRadiusSecondLargestCluster);
	_ss << "stdRadiusSecondLargestCluster " << stdRadiusSecondLargestCluster << std::endl;

	_meanRadiusLargestClusterFld->setDoubleValue(meanRadiusLargestCluster);
	_stdRadiusLargestClusterFld->setDoubleValue(stdRadiusLargestCluster);
	_meanRadiusSecondLargestClusterFld->setDoubleValue(meanRadiusSecondLargestCluster);
	_stdRadiusSecondLargestClusterFld->setDoubleValue(stdRadiusSecondLargestCluster);
}

//----------------------------------------------------------------------------------
//! Merge two clusters in _outXMarkerList as given by their cluster label.
//----------------------------------------------------------------------------------
void AscendingAortaDetection::_mergeClusters(int from, int to)
{
	for (int i=0; i<_outXMarkerList.getSize(); i++)
	{
		XMarker *tmpXMarker = (XMarker*)_outXMarkerList.getItemAt(i);
		if (tmpXMarker->type == from)
		{
			tmpXMarker->type = to;
		}
	}
}

//----------------------------------------------------------------------------------
//! _initVirtualVolumes
//! return value 1:  no problems while creating virtual volume
//! return value -1: error while creating virtual volume
//!
//----------------------------------------------------------------------------------
int AscendingAortaDetection::_initVirtualVolumes()
{
  
  if (_virtInVol) {
	  ML_DELETE(_virtInVol);
  }

  if (getUpdatedInImg(0)) {
	 //ML_CHECK_NEW(_virtInVol, VirtualVolume(this, 0, getInImg(0)->getDataType()));
	  // explicitely cast to MLuint16Type
	  ML_CHECK_NEW(_virtInVol, VirtualVolume(this, 0, MLuint16Type));
  }
  else {
	  _virtInVol = NULL;
  }
  
  if (CVL_IS_VIRTUALVOLUME_NOT_OK(_virtInVol)) {
	  return -1;
  }
  return 1;
}

//----------------------------------------------------------------------------------
//! Propagate ray in two directions.
//----------------------------------------------------------------------------------
double AscendingAortaDetection::_singleRayBothDirections(vec3 start, vec3 direction, Vector imgExt, unsigned primeThreshold, unsigned secondThreshold, vec3 voxSize)
{
	double result = 0.0;

	vec3 resDir1 = _singleRay(start, direction, imgExt, primeThreshold, secondThreshold);
	vec3 resDir2 = _singleRay(start, -direction, imgExt, primeThreshold, secondThreshold);

	result = sqrt((resDir1[0]-resDir2[0])*(resDir1[0]-resDir2[0])*voxSize[0]*voxSize[0] +
		    	  (resDir1[1]-resDir2[1])*(resDir1[1]-resDir2[1])*voxSize[1]*voxSize[1] +
				  (resDir1[2]-resDir2[2])*(resDir1[2]-resDir2[2])*voxSize[2]*voxSize[2]);

	return result;
}

//----------------------------------------------------------------------------------
//! Propagate ray in one direction as described in the paper.
//----------------------------------------------------------------------------------
vec3 AscendingAortaDetection::_singleRay(vec3 start, vec3 direction, Vector imgExt, unsigned primeThreshold, unsigned secondThreshold)
{
	TVirtualVolume<MLuint16> vInVol(*_virtInVol);
	vec3 result = start;

	unsigned value = vInVol.getVoxel(CVL_VEC3_TO_VECTOR(start));

	// as long as intensity greater than second threshold
	while (value > secondThreshold)
	{
		start = start + direction;
		if (CVL_IS_IN_VOLUME(start, imgExt)) {
			value = vInVol.getVoxel(CVL_VEC3_TO_VECTOR(start));
			// the result is the last voxel with an intensity greater than
			// the primary threshold
			if (value > primeThreshold) {
				result = start;
			}
		}
		else {
			// this will cause the while loop to end
			value = 0;
		}
	}
	return result;
}

ML_END_NAMESPACE

