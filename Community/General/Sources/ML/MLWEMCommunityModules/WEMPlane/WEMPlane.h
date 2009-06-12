
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

#ifndef __MLWEMPLANE_H__
#define __MLWEMPLANE_H__

#include "../MLWEMCommunityModulesSystem.h"
#include "mlOperatorIncludes.h"

#include <WEMBase/WEMModuleBase/WEMProcessor.h>

#include <mlPlaneField.h>

ML_START_NAMESPACE

////////////////////////////////////////////////////////////////////////

class MLWEMCOMMUNITYMODULES_EXPORT WEMPlane : public WEMProcessor {
public:
  //! Standard constructor
  WEMPlane(std::string type = "WEMPlane");
  //! Standard destructor
  ~WEMPlane();
  //! Called when input changes.
  virtual void handleNotification(Field *field);
  //! Initialize module after loading.
  virtual void activateAttachments();
  //! Process input WEM -> generate plane.
  virtual void _process();
  //! Finish generation of given WEMPatch
  virtual void _finish(WEMPatch *wemPatch);

private:
  //! Plane field.
  PlaneField *_planeFld;
  //! Should edges be generated?
  BoolField* _generateEdgesFld;

  //! Build plane for given patch
  void _buildPlane(WEMPatch* wemPatch);

  ML_BASEOP_CLASS_HEADER(WEMPlane)  
};

////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

#endif
