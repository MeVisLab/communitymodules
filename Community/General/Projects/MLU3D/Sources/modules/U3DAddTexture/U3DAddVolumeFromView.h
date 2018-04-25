//----------------------------------------------------------------------------------
// U3DAddTextureFromImage module.
//
// \file    U3DAddTextureFromImage.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DAddVolumeFromView_H
#define _U3DAddVolumeFromView_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DProcessor.h"
#include "mlTools.h"
#include "png.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DAddVolumeFromView : public U3DProcessor
{
  //! Implements interface for the runtime type system of the ML.
	ML_MODULE_CLASS_HEADER(U3DAddVolumeFromView)

public:

  //! Constructor.
	U3DAddVolumeFromView();

protected:

  //! Destructor.
	virtual ~U3DAddVolumeFromView();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();



private:

  /* FIELDS */
	ml::InputConnectorField* _inImageFld;
	ml::PagedImage* _inImage;
	ProgressField *_progressFld;
	StringField   *_statusFld;
	StringField   *_volumeNameFld;
  

  




  /* VARIABLES */



  /* METHODS */

	//! Convert a volume into a 3D color buffer, outputs dimensions
	png_bytep** prepare_png_buffer(ml::TVirtualVolume<MLuint8> &volume, MLint &num_slices, MLint &slice_resolution_x, MLint &slice_resolution_y, MLuint8 slice_dimension);

	//! Free a 3D buffer of the given dimensions
	void free_png_buffer(png_bytep** &buffer, MLint num_slices, MLint slice_resolution_x, MLint slice_resolution_y);

	//! Write the content of a 2D Buffer into a vector 
	std::string write_slice_buffer_to_png(png_bytep* buffer, std::vector<uint8_t>* target, MLint slice_resolution_x, MLint slice_resolution_y);

	void write_view_dimension_to_output(ml::TVirtualVolume<MLuint8>& volume, std::string view_name, MLuint8 slice_dimension);


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif 