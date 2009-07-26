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


#ifndef __mlAscendingAortaDetection_H
#define __mlAscendingAortaDetection_H


// Local includes
#ifndef __CVLAscendingAortaDetectionSystem_H
#include "CVLAscendingAortaDetectionSystem.h"
#endif

// ML includes
#include "mlOperatorIncludes.h"
#include "mlVersion.h"

//! Include Virtual volume classes from MLTools project.
#include "mlTVirtualVolume.h"

//! Include XMarker stuff
#include "mlXMarkerList.h"
#include "mlTimeCounter.h"

#include <sstream>

ML_START_NAMESPACE

// checks if a given virtual volume is valid
#define CVL_IS_VIRTUALVOLUME_NOT_OK(vv) (!vv || (vv && !vv->isValid()))
// converts a vec3 to a vector
#define CVL_VEC3_TO_VECTOR(input) Vector(input[0], input[1], input[2], 0, 0, 0)
// check if given point is within volume
#define CVL_IS_IN_VOLUME(point, extension) (point[0] >= 0) && (point[1] >= 0) && (point[2] >= 0) && (point[0]<extension[0]) && (point[1]<extension[1]) && (point[2]<extension[2])


//! 
class CVLASCENDINGAORTADETECTION_EXPORT AscendingAortaDetection : public BaseOp
{
public:

  //! Constructor.
  AscendingAortaDetection (void);
  //! Destructor.
  ~AscendingAortaDetection (void);
  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);


private:

  
  //! Propagate ray in one direction, returns points which meets threshold conditions.
  vec3 _singleRay(vec3 start, vec3 direction, Vector imgExt, unsigned primeThreshold, unsigned secondThreshold);
  //! Propagate ray in two directions, returns diameter.
  double _singleRayBothDirections(vec3 start, vec3 direction, Vector imgExt, unsigned primeThreshold, unsigned secondThreshold, vec3 voxSize);
  //! Initialize virtual volume.
  int _initVirtualVolumes();
  //! Main routine.
  void _process();
  //! Merge two clusters in _outXMarkerList as given by their cluster label.
  void _mergeClusters(int from, int to);
  //! Compute some additional statistics.
  void _statistics();
  //! Compute bounding box and some other statistics of given cluster.
  void _computeBB(XMarkerList &_list, vec3 &baryCenter, vec3 &lowerCenter, vec3 &upperCenter, double &mean, double &std, vec2 &radiusBC, vec3 &bbLower, vec3 &bbUpper);
  
  std::stringstream _ss;
    
  //! MarkerList with all clusters.
  XMarkerList _outXMarkerList;
  //! MarkerList with largest cluster.
  XMarkerList _outLargestCluster;
  //! MarkerList with second largest cluster.
  XMarkerList _outSecondLargestCluster;

  //! Virtual volume of input image.
  VirtualVolume *_virtInVol;
   
  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //! Output field for MarkerList with all clusters.
  BaseField *_outXMarkerListFld;
  //! Output field for MarkerList with largest cluster.
  BaseField *_outXMarkerListLargestClusterFld;
  //! Output field for MarkerList with second largest cluster.
  BaseField *_outXMarkerListSecondLargestClusterFld;
  //! Primary threshold th_p in gray values.
  IntField *_primaryThresholdFld;
  //! Secondary threshold th_s in gray values.
  IntField *_secondaryThresholdFld;
  //! Minimum diameter d_min in mm
  IntField *_minDiameterFld;
  //! Maximum diameter d_max in mm
  IntField *_maxDiameterFld;
  //! Distance limit for clustering d_clust.
  Vec3fField *_distanceClusterFld;
  //! Ratio a.
  DoubleField *_ratioLimitFld;
  //! Apply ratio limit.
  BoolField *_useRatioLimitFld;
  //! Output during computation.
  StringField *_infoFld;
  //! increment for search rays in mm
  Vec3fField *_incrementFld;
  //! percentage in respect to the image extension where the algorithm starts looking for the aorta  
  Vec3fField *_startSearchAreaFld;
  //! percentage in respect to the image extension where the algorithm stops  looking for the aorta
  Vec3fField *_endSearchAreaFld;
  //! Apply field.
  NotifyField *_applyFld;
  //! Is auto apply.
  BoolField *_autoApplyFld;
  //! Runtime of algorithm.
  DoubleField *_runTimeFld;
  //! Selected seed point for segmentation.
  Vec3fField *_seedPointFld;

  // fields for largest / primary cluster
  
  //! Size of cluster i.e. number of candidate points. 
  IntField *_priClusterSizeFld;
  //! Mean intensity within bounding box.
  DoubleField *_priMeanIntensityFld;
  //! Std intensity within bounding box.
  DoubleField *_priStdIntensityFld;
  //! Computed barycenter of the candidate points (in world coords).
  Vec3fField *_priBaryCenterFld;
  //! Candidate point with lowest voxel-z-coordinate (in world coords).
  Vec3fField *_priLowerCenterFld;
  //! Candidate point with highest voxel-z-coordinate (in world coords).
  Vec3fField *_priUpperCenterFld;
  //! Radius at bary center of cluster.
  Vec2fField *_priRadiusBCFld;
  //! Bounding box for cluster (see computation on how it is defined).
  Vec3fField *_priBBLowerFld;
  //! Bounding box for cluster (see computation on how it is defined).
  Vec3fField *_priBBUpperFld;
  //! Exentsion in z direction in mm.
  DoubleField *_extensionLargestClusterFld;
  //! Mean radius in mm of candidates points within cluster.
  DoubleField *_meanRadiusLargestClusterFld;
  //! Std radius in mm of candidates points within cluster.
  DoubleField *_stdRadiusLargestClusterFld;

  // fields for second largest / secondary cluster

  //! Size of cluster i.e. number of candidate points. 
  IntField *_secClusterSizeFld;
  //! Mean intensity within bounding box.
  DoubleField *_secMeanIntensityFld;
  //! Std intensity within bounding box.
  DoubleField *_secStdIntensityFld;
  //! Computed barycenter of the candidate points (in world coords).
  Vec3fField *_secBaryCenterFld;
  //! Candidate point with lowest voxel-z-coordinate (in world coords).
  Vec3fField *_secLowerCenterFld;
  //! Candidate point with highest voxel-z-coordinate (in world coords).
  Vec3fField *_secUpperCenterFld;
  //! Radius at bary center of cluster.
  Vec2fField *_secRadiusBCFld;
  //! Bounding box for cluster (see computation on how it is defined).
  Vec3fField *_secBBLowerFld;
  //! Bounding box for cluster (see computation on how it is defined).
  Vec3fField *_secBBUpperFld;
  //! Exentsion in z direction in mm.
  DoubleField *_extensionSecondLargestClusterFld;
  //! Mean radius in mm of candidates points within cluster.
  DoubleField *_meanRadiusSecondLargestClusterFld;
  //! Std radius in mm of candidates points within cluster.
  DoubleField *_stdRadiusSecondLargestClusterFld;

  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(AscendingAortaDetection)

};


ML_END_NAMESPACE

#endif // __mlAscendingAortaDetection_H
