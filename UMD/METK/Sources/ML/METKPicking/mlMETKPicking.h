
//! ML ObjMgrClient module METKPicking.
//! \file mlMETKPicking.h

#ifndef __MLMETKPICKING_H
#define __MLMETKPICKING_H


// Local includes
#include "METKPickingSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>
#include <kBasics.h>
#include <vector>

class SoSeparator;
class SbVec3f;
class SbBox3f;
class SoPickObject;

ML_START_NAMESPACE

//! ML ObjMgrClient module METKPicking.
class METKPICKING_EXPORT METKPicking : public ObjMgrClient
{
public:

   //! Constructor.
   METKPicking();
   ~METKPicking (void);

   //! Handle field changes of the field \c field.
   virtual void handleNotification (Field *field);

   virtual void activateAttachments();

   const float getTransparency(const SoNode* node);
   const float getImportance(const SoNode* node);
   const bool getPickable(const SoNode* node);
   void setPickedShape(SoNode* node, SbVec3f* selection, std::vector<SoNode*>* allObjInRay);
   void setMouseOverShape(SoNode* node, std::vector<SoNode*>* allObjInRay);
   const bool containsNode(const SoNode* node);

private:

   typedef ObjMgrClient inherited;

   // ----------------------------------------------------------
   //@{ \name Module field declarations
   // ----------------------------------------------------------

   FloatField* _scrapLight;
   FloatField* _importanceWeighting;
   FloatField* _boundingBoxSizeWeighting;
   //BoolField*  _mustBePickable;
   IntField*   _pickPixelTolerance;
   BoolField* _showBoundingBoxes;
   BoolField* _enableMouseOver;


   SoNodeField* _outPickedObject;
   SoNodeField* _outScene;
   StringField* _objName;
   StringField* _objList;
   StringField* _mouseOverName;
   StringField* _mouseOverList;
   Vec3fField*  _selection;
   //@}

   SoPickObject* _pickObject;

   bool _pickedObjectFound;
   omObjectContainer::const_iterator _pickedObjectIter;

   //! Implements interface for the runtime type system of the ML.
   ML_CLASS_HEADER(METKPicking)
};


ML_END_NAMESPACE

#endif // __MLMETKPICKING_H


