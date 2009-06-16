#ifndef _SoFileToSceneGraph_H
#define _SoFileToSceneGraph_H

#include "UMDToolsSystem.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFNode.h>

class SoFieldSensor;
class SoSearchAction;

class UMDTOOLS_EXPORT SoFileToSceneGraph : public SoSeparator
{
   typedef SoSeparator inherited;

   SO_NODE_HEADER(SoFileToSceneGraph);


public:
   //! A float field.
   SoSFNode inputObject;
   SoSFNode outConvertedScene;

   SoFileToSceneGraph();

   SoINTERNAL public:
   static void initClass();

private:

   //! the sensor for \c inputObject
   SoFieldSensor* _inputObjectSensor;
   //! called if inputObject has been changed, calls the non-static method \c inputChanged
   static void  inputChangedCB(void *userData, SoSensor*);
   //! called if the ouput should be changed, calls the non-static method \c outputChanged
   void  inputChanged();
   //! called by \c outputChangedCB

   SoSeparator* _inputSep;
   SoSearchAction* _searchAction;

protected:
   virtual ~SoFileToSceneGraph();
};

#endif  // _SoFileToSceneGraph_H
