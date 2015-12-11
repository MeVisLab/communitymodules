//----------------------------------------------------------------------------------
//! The ML module class PDF3DWork.
/*!
// \file    PDF3DWork.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-20
*/
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "PDF3DWorkSystem.h"

// Project includes
#include <PDFGenerator.h>
#include <MLPDF_DataTypes.h>

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class PDF3DWORK_EXPORT PDF3DWork : public PDFGenerator
{
public:

  //! Constructor.
  PDF3DWork();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

protected:

  //! Assemble PDF document
  //! This is the main method for all derived modules.
  //! Add all code that assembles the actual PDF document here!
  //! Return true if assembly was successful, false otherwise.
  virtual bool assemblePDFDocument();

private:

  // Private vars
  std::vector<MetaDataStruct> _u3dMetaData;


  MLuint8 _readU8(std::ifstream& file);
  MLuint16 _readU16(std::ifstream& file);
  MLuint32 _readU32(std::ifstream& file);
  std::string _readString(std::ifstream& file);
  bool _getMetaDataFromU3DFile(std::string u3dFilename);
  std::string _getDefaultViewSpecificationFromU3DMetaData();
  void _add3DFigure(float x, float y, float width, float height, bool addOutline, std::string u3dFilename, std::string posterFilename, std::string viewDefinition = "");

  bool _assembleVersion1();
  bool _assembleVersion2();
  bool _assembleVersion3();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDF3DWork)
};


ML_END_NAMESPACE