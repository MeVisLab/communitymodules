//----------------------------------------------------------------------------------
//! The ML module class LoadU3D.
/*!
// \file    LoadU3D.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
//
// Loads U3D file and creates WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


#ifndef __LoadU3D_H
#define __LoadU3D_H

// Global includes
#include <time.h>
#include <sys/stat.h>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

// Local includes
#include "MLPDFSystem.h"

#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMGenerator.h>

// ML includes
#include <mlModuleIncludes.h>


using namespace std;


ML_START_NAMESPACE


//! 
class MLPDF_EXPORT LoadU3D : public WEMGenerator
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(LoadU3D)

public:

  //! Constructor.
  LoadU3D(std::string type="LoadU3D");

protected:

  //! Destructor.
  virtual ~LoadU3D();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! _process()-routine for correct processing.
  virtual void _process();

private:

  /* FIELDS */

  Vector3Field*  _boundingBoxCenterFld;
  FloatField*    _boundingBoxRadiusFld;



  void _parseU3DFile(std::string filename);
  MLuint8 _readU8(ifstream& file);
  MLuint16 _readU16(ifstream& file);
  MLuint32 _readU32(ifstream& file);
  string _readString(ifstream& file);

  //! The main processing routine. Here, the own mesh algorithm can be implemented.
  void myAlgorithm();
};

ML_END_NAMESPACE

#endif // __LoadU3D_H