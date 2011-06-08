
//! ML ObjMgrClient module METKSurfaceDistance3D.
//! \file mlMETKSurfaceDistance3D.h

#ifndef __MLMETKSurfaceDistance3D_H
#define __MLMETKSurfaceDistance3D_H


// Local includes
#ifndef __METKSURFACEDISTANCE_H
#include "METKSurfaceDistanceSystem.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>

class SoSeparator;
class SbVec3f;
class SbBox3f;
class SoColorShape;

ML_START_NAMESPACE

typedef std::vector<SoColorShape*> T_ColorShapeVec;
typedef std::vector<SoColorShape*>::iterator T_ColorShapeIterator;
typedef std::vector<SoColorShape*>::const_iterator T_ConstColorShapeIterator;

class BoundingBox;

//! ML ObjMgrClient module METKSurfaceDistance3D.
class METKSURFACEDISTANCE_EXPORT METKSurfaceDistance3D : public ObjMgrClient
{
public:

   //! Constructor.
   METKSurfaceDistance3D();
   ~METKSurfaceDistance3D (void);

   //! Handle field changes of the field \c field.
   virtual void handleNotification (Field *field);

   virtual void handleObjMgrNotification();

   virtual void activateAttachments();

   //! Read out image data at the nodes' positions.
   void setBoundingBox(SbBox3f* bbox);
   float getImageValue(const SbVec3f* vertex);

private:

   typedef ObjMgrClient inherited;

   // ----------------------------------------------------------
   //@{ \name Module field declarations
   // ----------------------------------------------------------

   SoNodeField* _outInventor;

   FloatField* _nearDistance;
   ColorField* _nearDistanceColor;

   FloatField* _farDistance;
   ColorField* _farDistanceColor;
   //@}

   SoSeparator* _outGroup;
   void addStructure(SoNode* node);
   void removeStructure(SoNode* node);
   T_ColorShapeVec _colorShapeVec;

   //! Do we have a valid input image?
   bool _inputImageIsValid;

   //! A pointer to the input image.
   PagedImg* _image;

   //! Memory containing the sub image
   void* _memSlice;
   float* _subImageMemory;
   SubImgBox _intersectBB;

   // Extension of the sub image
   int _yStride;
   int _zStride;

   //! Image properties: Image extend
   //BoundingBox* _boundingBox;
   SubImgBox* _inImageBox;

   //! Implements interface for the runtime type system of the ML.
   ML_CLASS_HEADER(METKSurfaceDistance3D)
};


ML_END_NAMESPACE

#endif // __MLMETKSurfaceDistance3D_H


