//----------------------------------------------------------------------------------
//! The ML module class PlaneClip.
/*!
// \file    mlPlaneClip.cpp
// \author  Bart De Dobbelaer, Hugo Kuijf
// \date    2012-07-11
//
// 
*/
//----------------------------------------------------------------------------------
/*
        This program is written by: 
        
            Bart De Dobbelaer
            
            Laboratory for Medical Imaging Research
            Radiologie/ESAT PSI
            Universitair Ziekenhuis Gasthuisberg
            Herestraat 49
            B-3000 Leuven
            Belgium
            
        Email: Bart.DeDobbelaer@uz.kuleuven.ac.be
*/

// Local includes
#include "PlaneClip.h"

#include <mlTypedHandlers.h>

ML_START_NAMESPACE

  //! The PlaneClipHandler handles the page calculation of an output image of PlaneClip.
  //! It should copy all required state from PlaneClip or the output image in its
  //! constructor and should not keep a reference to the output image nor the module itself.
  //! This facilitates to implement a thread-safe handler, which does not access non-threadsafe shared data.
class PlaneClipHandler : public TypedCalculateOutputImageHandler<PlaneClipHandler,
  /* numInputs = */ 1,
  /* MLVariableType0 = */ ScalarTypes>
{

public:
  //! Template configuration of the output/input sub images.
  enum {
    OutputSubImage_Type = MLVariableType0,
    InputSubImage0_Type = MLVariableType0,
    InputSubImage0_ReadOnly = true
  };

private:
  // Handler state:
  const MLint _outputIndex;
  const Plane _plane;
  const Matrix4& _voxelToWorldMatrix;
  const MLint _intersectionMode;
  const MLdouble _volumeThreshold;
  const bool _useGlobalSubsample;
  const MLuint _globalSubsample;
  const MLuint _xSubsample;
  const MLuint _ySubsample;
  const MLuint _zSubsample;

public:
  //----------------------------------------------------------------------------------
  //! Construct a new handler for given \p outputImage and given module state.
  //! Do not keep references to the outputImage or the module, copy the needed state
  //! instead.
  //----------------------------------------------------------------------------------
  PlaneClipHandler(const PagedImage& outputImage, const PlaneClip& module) 
    : _outputIndex(outputImage.getOutputIndex()), 
      _plane(module._planeFld->getPlaneValue()), 
      _voxelToWorldMatrix(outputImage.getVoxelToWorldMatrix()),
      _intersectionMode(module._intersectionModeFld->getEnumValue()),
      _volumeThreshold(module._volumeThresholdFld->getDoubleValue()),
      _useGlobalSubsample(module._useGlobalSubsampleFld->getBoolValue()),
      _globalSubsample(module._globalSubsampleFld->getIntValue()),
      _xSubsample(module._xSubsampleFld->getIntValue()),
      _ySubsample(module._ySubsampleFld->getIntValue()),
      _zSubsample(module._zSubsampleFld->getIntValue())
  { }

  //----------------------------------------------------------------------------------
  //! Templated output page calculation.
  //----------------------------------------------------------------------------------
  template <typename OUTTYPE>
  void typedCalculateOutputSubImage(TSubImage<OUTTYPE>& outputSubImage,
    const TSubImage<OUTTYPE>& inputSubImage0,
    UserThreadData* /*userThreadData*/)
  {
    // Compute outSubImage from inSubImage[0-N].

    // Clamp box of output image against image extent to avoid that unused areas are processed.
    const SubImageBox validOutBox = outputSubImage.getValidRegion();

    // Const variables used for volume or anti-aliasing.
    const MLuint nx(_useGlobalSubsample ? _globalSubsample : _xSubsample);
    const MLuint ny(_useGlobalSubsample ? _globalSubsample : _ySubsample);
    const MLuint nz(_useGlobalSubsample ? _globalSubsample : _zSubsample);
    const MLdouble subsampleSize(MLdouble(nx*ny*nz));
    const MLdouble xStep(1. / (2. * nx));
    const MLdouble yStep(1. / (2. * ny));
    const MLdouble zStep(1. / (2. * nz));

    // Process all voxels of the valid region of the output page.
    ImageVector p;
    for (p.u=validOutBox.v1.u;  p.u<=validOutBox.v2.u;  ++p.u) {
      for (p.t=validOutBox.v1.t;  p.t<=validOutBox.v2.t;  ++p.t) {
        for (p.c=validOutBox.v1.c;  p.c<=validOutBox.v2.c;  ++p.c) {
          for (p.z=validOutBox.v1.z;  p.z<=validOutBox.v2.z;  ++p.z) {
            for (p.y=validOutBox.v1.y;  p.y<=validOutBox.v2.y;  ++p.y) {

              p.x = validOutBox.v1.x;
              // Get pointers to row starts of input and output subimages.
              const OUTTYPE* inVoxel0 = inputSubImage0.getImagePointer(p);

              OUTTYPE*  outVoxel = outputSubImage.getImagePointer(p);

              const MLint rowEnd   = validOutBox.v2.x;

              // Process all row voxels.
              for (; p.x <= rowEnd;  ++p.x, ++outVoxel, ++inVoxel0)
              {
                // For the anti-aliasing computation, it only makes sense if the current voxel is close to the plane. If the
                // distance is too large, the plane does not intersect the voxel. Then, no computation is needed anyway and
                // we fall back to midpoint (default).
                const Vector3 voxelCenter(_voxelToWorldMatrix.transformPoint(Vector3(MLdouble(p.x) + 0.5, MLdouble(p.y) + 0.5, MLdouble(p.z) + 0.5)));

                MLint currentIntersectionMode = _intersectionMode;
                if (_intersectionMode == 1 || _intersectionMode == 2) { // Volume or midpoint
                  const MLdouble distance = mlAbs(_plane.getDistance(voxelCenter));

                  // Check if the plane intersects the current voxel. If the distance from the voxelCenter to the plane is
                  // smaller than the distance from voxelOrigin to voxelCenter, the plane is in the voxel.
                  const Vector3 voxelOrigin(_voxelToWorldMatrix.transformPoint(Vector3(MLdouble(p.x), MLdouble(p.y), MLdouble(p.z))));
                  if (distance > voxelOrigin.distance(voxelCenter)) { 
                    // No intersection, fall back to midpoint
                    currentIntersectionMode = 0;
                  }
                }

                switch (currentIntersectionMode) {
                case 0: // Midpoint
                  if (_plane.isInHalfSpace(voxelCenter)) {
                    *outVoxel = (_outputIndex == 0) ? *inVoxel0 : OUTTYPE(0);
                  } else {
                    *outVoxel = (_outputIndex == 0) ? OUTTYPE(0) : *inVoxel0;
                  } 
                  break;
                case 1:
                case 2: 
                  {
                    // Divide the voxel in n x n x n boxes and test each component individually against the plane.
                    MLuint count(0);

                    for (MLuint z(0); z < nz; ++z) {
                      for (MLuint y(0); y < ny; ++y) {
                        for (MLuint x(0); x < nx; ++x) {
                          const MLdouble newX = MLdouble(p.x) + MLdouble(x) / nx + xStep;
                          const MLdouble newY = MLdouble(p.y) + MLdouble(y) / ny + yStep;
                          const MLdouble newZ = MLdouble(p.z) + MLdouble(z) / nz + zStep;

                          if (_plane.isInHalfSpace(_voxelToWorldMatrix.transformPoint(Vector3(newX, newY, newZ)))) {
                            count += (_outputIndex == 0) ? 1 : 0;
                          } else {
                            count += (_outputIndex == 0) ? 0 : 1;
                          }
                        }
                      }
                    }

                    const MLdouble percentage(MLdouble(count) / subsampleSize); // [0..1]

                    if (currentIntersectionMode == 1) { // Volume
                      *outVoxel = (percentage >= _volumeThreshold) ? *inVoxel0 : 0;
                    }
                    if (currentIntersectionMode == 2) { // AntiAliasing
                      *outVoxel = OUTTYPE(*inVoxel0 * percentage);
                    }
                  }
                  break;
                default:
                  // This should never occur
                  mlFatalError("PlaneClipHandler::typedCalculateOutputSubImage", ML_PROGRAMMING_ERROR);
                }             
              }
            }
          }
        }
      }
    }
  }

  private:
    PlaneClipHandler& operator= (const PlaneClipHandler&) { }
};


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PlaneClip, Module);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
PlaneClip::PlaneClip() : Module(1, 2)
{
  // See section 'Implementing the Constructor()' in ML Programming Guide for details.
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _planeFld = addPlane("plane", Plane());
  _autoApplyFld = addBool("autoApply", false);
  _applyFld = addNotify("apply");

  std::vector<std::string> intersectionMode;
  intersectionMode.push_back("IntersectionModeMidpoint");
  intersectionMode.push_back("IntersectionModeVolume");
  intersectionMode.push_back("IntersectionModeAntiAliasing");

  _intersectionModeFld = addEnum("intersectionMode", intersectionMode);

  _volumeThresholdFld = addDouble("volumeThreshold", 0.5);

  _useGlobalSubsampleFld = addBool("useGlobalSubsample", true);
  _globalSubsampleFld = addInt("globalSubsample", 4);
  _xSubsampleFld = addInt("xSubsample", 4);
  _ySubsampleFld = addInt("ySubsample", 4);
  _zSubsampleFld = addInt("zSubsample", 4);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void PlaneClip::handleNotification(Field *field)
{  
  bool touchOutputs = false;

  if (field == _applyFld) {
    touchOutputs = true;
  } if (_autoApplyFld->getBoolValue()) {
    touchOutputs = true;
  } if (isInputImageField(field) && getUpdatedInputImage(0) == NULL) {
    touchOutputs = true;
  }

  if (touchOutputs) {
    // Touch all output image fields to notify connected modules.
    touchOutputImageFields();
  }
}

//----------------------------------------------------------------------------------
//! Sets properties of the output image at output outputIndex.
//----------------------------------------------------------------------------------
void PlaneClip::calculateOutputImageProperties(int /*outputIndex*/, PagedImage* outputImage)
{
  // Setup datatypes and read-only flags of output image and input subimages.
  PlaneClipHandler::setupKnownProperties(outputImage);
  
  // Verify if the input/output datatypes are supported by our handler.
  // This will invalidate the output image if the type combination is not supported by the handler.
  PlaneClipHandler::verifyProperties(outputImage);
}

//----------------------------------------------------------------------------------
//! Creates the PlaneClipHandler for the given output image whenever calculateOutputImageProperties()
//! is called and the outputImage is valid.
//----------------------------------------------------------------------------------
CalculateOutputImageHandler* PlaneClip::createCalculateOutputImageHandler(PagedImage* outputImage)
{
  return new PlaneClipHandler(*outputImage, *this);
}

ML_END_NAMESPACE