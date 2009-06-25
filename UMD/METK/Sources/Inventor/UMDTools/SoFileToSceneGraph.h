//! Inventor node SoFileToSceneGraph derived from SoSeparator creating scene graphs from scenes loaded with SoFile.
//! \file SoFileToSceneGraph.h

#ifndef _SoFileToSceneGraph_H
#define _SoFileToSceneGraph_H

#include "UMDToolsSystem.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFNode.h>

class SoFieldSensor;
class SoSearchAction;

//! Inventor node SoFileToSceneGraph derived from SoSeparator creating scene graphs from scenes loaded with SoFile.
class UMDTOOLS_EXPORT SoFileToSceneGraph : public SoSeparator
{
   typedef SoSeparator inherited;

   SO_NODE_HEADER(SoFileToSceneGraph);


public:
   SoSFNode inputObject;
   SoSFNode outConvertedScene;

   SoFileToSceneGraph();

   SoINTERNAL public:
   static void initClass();

private:

   //! the sensor for inputObject.
   SoFieldSensor* _inputObjectSensor;
   //! called if inputObject has been changed, calls the non-static method inputChanged.
   static void  inputChangedCB(void *userData, SoSensor*);
   //! called if the ouput should be changed, calls the non-static method outputChanged.
   void  inputChanged();
   //! called by outputChangedCB

   SoSeparator* _inputSep;
   SoSearchAction* _searchAction;

protected:
   virtual ~SoFileToSceneGraph();
};

#endif  // _SoFileToSceneGraph_H
