//----------------------------------------------------------------------------------
//! The ML module class WEMColorizeNodes.
/*!
// \file    WEMColorizeNodes.h
// \author  Axel Newe
// \date    2015-12-03
//
// 
*/
//----------------------------------------------------------------------------------


#pragma once


#include "mlLUTFunction.h"
#include "mlLUTFloatRGBA.h"

// Local includes
#include "MLWEMColorizeNodesSystem.h"

#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMProcessor.h>

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLWEMCOLORIZENODES_EXPORT WEMColorizeNodes : public WEMProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(WEMColorizeNodes)

public:

  //! Constructor.
  WEMColorizeNodes (std::string type="WEMColorizeNodes");

protected:

  //! Destructor.
  virtual ~WEMColorizeNodes();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! _process()-routine for correct processing.
  virtual void _process();

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! 
  BaseField *_inLUTFld;

  LUTFloatRGBA* _RGBALUT;
  LUTFunction* _inLUTFunction;

  //@}

  //! The main processing routine. Here, the own mesh algorithm can be implemented.
  void _colorizeWEMNodes();

  void _setupLUT();
};

ML_END_NAMESPACE