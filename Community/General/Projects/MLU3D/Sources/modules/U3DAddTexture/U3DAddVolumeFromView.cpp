//----------------------------------------------------------------------------------
// U3DAddTextureFromImage module.
//
// \file    U3DAddTextureFromImage.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAddVolumeFromView.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_Constants.h"





ML_START_NAMESPACE

using namespace mlU3D;

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddVolumeFromView, U3DProcessor);

// allocates and fills a libpng buffer from a MeVisLab volume
png_bytep** U3DAddVolumeFromView::prepare_png_buffer(ml::TVirtualVolume<MLuint8> &volume, MLint &num_slices, MLint &slice_resolution_x, MLint &slice_resolution_y, MLuint8 slice_dimension){
	
	/* get dimensions */
	ml::SubImageBox imageBox = volume.getVirtualVolume().getBox();
	MLint res_x = imageBox.getExtent()[0];
	MLint res_y = imageBox.getExtent()[1];
	MLint res_z = imageBox.getExtent()[2];
	if (slice_dimension == 0){
		slice_resolution_x = res_z;
		slice_resolution_y = res_y;
		num_slices = res_x;
	}
	else if (slice_dimension == 1){
		slice_resolution_x = res_z;
		slice_resolution_y = res_x;
		num_slices = res_y;
	}
	else {
		slice_resolution_x = res_y;
		slice_resolution_y = res_x;
		num_slices = res_z;
	}

	/* allocate buffer */
	png_bytep** png_data_buffer = new png_bytep*[num_slices];
	for (MLint i = 0; i < num_slices; i++){
		png_data_buffer[i] = new png_bytep[slice_resolution_y];
		for (MLint j = 0; j < slice_resolution_y; j++){
			png_data_buffer[i][j] = new png_byte[slice_resolution_x * 4];
		}
	}

	/* loop over volume and fill buffer */
	for (MLint i = 0; i < num_slices; i++){
		_progressFld->setProgressValue(0.05 + 0.9 * (((float)(i)) / ((float)(num_slices))));
		for (MLint j = 0; j < slice_resolution_y; j++){
			for (MLint k = 0; k < slice_resolution_x; k++){
				ml::ImageVector access_position = { 0, 0, 0, 0, 0, 0 };
				if (slice_dimension == 0){
					access_position[0] = i;
					access_position[1] = j;
					access_position[2] = k;
				}
				else if (slice_dimension == 1){
					access_position[0] = j;
					access_position[1] = i;
					access_position[2] = k;
				}
				else {
					access_position[0] = j;
					access_position[1] = k;
					access_position[2] = i;
				}
				MLuint8 current_value = volume.getValue(access_position);
				

				png_data_buffer[i][j][k * 4 + 0] = current_value; //r
				png_data_buffer[i][j][k * 4 + 1] = current_value; //g
				png_data_buffer[i][j][k * 4 + 2] = current_value; //b
				png_data_buffer[i][j][k * 4 + 3] = current_value; //a
			}
		}
	}
	return png_data_buffer;
}

//free buffer structure created by prepare_png_buffer
void U3DAddVolumeFromView::free_png_buffer(png_bytep** &buffer, MLint num_slices, MLint slice_resolution_x, MLint slice_resolution_y){
	for (MLint i = 0; i < num_slices; i++){
		for (MLint j = 0; j < slice_resolution_y; j++){
			delete[] buffer[i][j];
		}
		delete[] buffer[i];
	}
	delete[] buffer;
	buffer = nullptr;
}

// callback function for png_set_write_fn 
static void png_write_to_vector_callback(png_structp png_ptr, png_bytep data, png_size_t length){
	std::vector<uint8_t> *p = (std::vector<uint8_t>*)png_get_io_ptr(png_ptr);
	p->insert(p->end(), data, data + length);
}

// writes current png to a vector
std::string U3DAddVolumeFromView::write_slice_buffer_to_png(png_bytep* buffer, std::vector<uint8_t>* target, MLint slice_resolution_x, MLint slice_resolution_y){
	
	/* create structs */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr){
		return "internal error";
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		return "internal error";
	}


	png_set_write_fn(png_ptr, target, png_write_to_vector_callback, NULL);

	/* write header */
	#pragma warning(push)
	#pragma warning(disable:4611)
	if (setjmp(png_jmpbuf(png_ptr))){
		return "unable to write PNG header";
	}

	png_set_IHDR(png_ptr, info_ptr, slice_resolution_x, slice_resolution_y,
		8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))){
		return "error during PNG creation";
	}

	png_write_image(png_ptr, buffer);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr))){
		return "error during PNG creation";
	}
	#pragma warning(pop)

	png_write_end(png_ptr, NULL);

	return "";
}





//***********************************************************************************

U3DAddVolumeFromView::U3DAddVolumeFromView() : U3DProcessor(1, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  _inImageFld = getInputImageField(0);
  _inImageFld->setName("Image Data");
  _inImage = getInputImage(0);
  (_progressFld = addProgress("progress"))->setFloatValue(0.0f);
  (_statusFld = addString("status"))->setStringValue("Idle.");
  (_volumeNameFld = addString("volumeName"))->setStringValue("");
 

  // Add and initialize parameter fields

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddVolumeFromView::~U3DAddVolumeFromView()
{
  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddVolumeFromView::handleNotification(Field* field)
{
   
	_updateModeFld->setEnumValue(UPDATE_MODE_CLEAR);
  if (field == getInputImageField(0)){
	  _inImage = getInputImage(0);
  }


  if (_autoApplyFld->getBoolValue())
  {
    process();
  }

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddVolumeFromView::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}



//***********************************************************************************

void U3DAddVolumeFromView::process()
{
  if (_inImage == nullptr) return;
  if (_inU3DValid)
  {
	
	  /* get name for volume */
	  std::string view_name = _volumeNameFld->getStringValue();
	  if (view_name == ""){
		  _statusFld->setStringValue("enter a valid name");
		  return;
	  }

	  /*create group*/
	  _outU3DObject->groupNodes.push_back(GroupNode());
	  auto volume_group = _outU3DObject->groupNodes.end() - 1;
	  volume_group->displayName = view_name;
	  volume_group->parentName = "";
	  volume_group->nodeName = view_name;

	  /* write the three perspectives to the internal format*/
	  ml::TVirtualVolume<MLuint8> in_volume = ml::TVirtualVolume<MLuint8>(_inImage);
	  write_view_dimension_to_output(in_volume, view_name, 0);
	  write_view_dimension_to_output(in_volume, view_name, 1);
	  write_view_dimension_to_output(in_volume, view_name, 2);
  }
  else
  {
    _statusFld->setStringValue("U3D Input invalid");
  }

  _outU3DObjectFld->touch();
}


//***********************************************************************************

// writes data from volume sliced along slice_dimension into the given view
void U3DAddVolumeFromView::write_view_dimension_to_output(ml::TVirtualVolume<MLuint8>& volume, std::string view_name, MLuint8 slice_dimension){
	MLint num_slices, slice_resolution_x, slice_resolution_y;


	std::string dimension_name = "invalid dimension";
	if (slice_dimension == 0){
		dimension_name = "x";
	}
	else if (slice_dimension == 1){
		dimension_name = "y";
	}
	else {
		dimension_name = "z";
	}

	/* create subgroup */
	std::string group_name = "dimension_" + dimension_name;
	std::string group_path = view_name + "/" + group_name;
	_outU3DObject->groupNodes.push_back(GroupNode());
	auto volume_group = _outU3DObject->groupNodes.end() - 1;
	volume_group->displayName = group_name;
	volume_group->parentName = view_name;
	volume_group->nodeName = group_name;

	/* import image into libpng */
	_statusFld->setStringValue("Reading Image.");
	png_bytep** buffer = prepare_png_buffer(volume, num_slices, slice_resolution_x, slice_resolution_y, slice_dimension);

	/* uvs for slices */
	std::vector<std::pair<MLfloat, MLfloat>> texture_map;
	texture_map.push_back(std::pair<MLfloat, MLfloat>(1, 0));
	texture_map.push_back(std::pair<MLfloat, MLfloat>(0, 0));
	texture_map.push_back(std::pair<MLfloat, MLfloat>(0, 1));
	texture_map.push_back(std::pair<MLfloat, MLfloat>(0, 1));
	texture_map.push_back(std::pair<MLfloat, MLfloat>(1, 1));
	texture_map.push_back(std::pair<MLfloat, MLfloat>(1, 0));

	/* precalculate silce positions depending on slice_dimension */
	MLfloat voxel_width;
	Vector3 base_corner_a, base_corner_b, base_corner_c, base_corner_d, slice_offset;
	if (slice_dimension == 0){
		MLfloat resx = (MLfloat)slice_resolution_x;
		MLfloat resy = (MLfloat)slice_resolution_y;
		voxel_width = 1 / ((MLfloat)num_slices);
		slice_offset = Vector3(1, 0, 0) * voxel_width;
		base_corner_d = Vector3(0, -0.5 * voxel_width * (resx - 1), 0.5 * voxel_width * (resy - 1));
		base_corner_a = Vector3(0, -0.5 * voxel_width * (resx - 1), -0.5 * voxel_width * resy);
		base_corner_b = Vector3(0, 0.5 * voxel_width * resx, -0.5 * voxel_width * resy);
		base_corner_c = Vector3(0, 0.5 * voxel_width * resx, 0.5 * voxel_width * (resy - 1));
	}
	else if (slice_dimension == 1){
		voxel_width = 1 / ((MLfloat)slice_resolution_y);
		slice_offset = Vector3(0, 0, -1)*voxel_width;
		base_corner_a = Vector3(1 - voxel_width, -0.5 * ((MLfloat)slice_resolution_x - 2) * voxel_width, 0.5 * ((MLfloat)num_slices) * voxel_width);
		base_corner_b = Vector3(1 - voxel_width, 0.5 * ((MLfloat)slice_resolution_x) * voxel_width, 0.5 * ((MLfloat)num_slices) * voxel_width);
		base_corner_c = Vector3(0, 0.5 * ((MLfloat)slice_resolution_x) * voxel_width, 0.5 * ((MLfloat)num_slices) * voxel_width);
		base_corner_d = Vector3(0, -0.5 * ((MLfloat)slice_resolution_x - 2) * voxel_width, 0.5 * ((MLfloat)num_slices) * voxel_width);
	}
	else if (slice_dimension == 2){
		voxel_width = 1 / ((MLfloat)slice_resolution_y);
		slice_offset = Vector3(0, -1, 0)*voxel_width;
		base_corner_a = Vector3(1 - voxel_width, 0.5 * ((MLfloat)num_slices) * voxel_width, -0.5 * ((MLfloat)slice_resolution_x - 2) * voxel_width);
		base_corner_b = Vector3(1 - voxel_width, 0.5 * ((MLfloat)num_slices) * voxel_width, 0.5 * ((MLfloat)slice_resolution_x) * voxel_width);
		base_corner_c = Vector3(0, 0.5 * ((MLfloat)num_slices) * voxel_width, 0.5 * ((MLfloat)slice_resolution_x) * voxel_width);
		base_corner_d = Vector3(0, 0.5 * ((MLfloat)num_slices) * voxel_width, -0.5 * ((MLfloat)slice_resolution_x - 2) * voxel_width);
	}

	/* iterate over slices */
	for (MLint slice = 0; slice < num_slices; slice++){
		/* generate names for internal format */
		std::string slice_name = view_name + "_" + dimension_name + "_slice" + std::to_string(slice);
		std::string texture_name = "texture_for_"+slice_name;
		std::string wem_name = "wem_for_"+slice_name;
		std::string node_name = slice_name;

		/* create slice mesh */
		WEM* geometry = U3DTools::constructQuadWem(
			base_corner_a + slice_offset * ((MLfloat)slice),
			base_corner_b + slice_offset * ((MLfloat)slice),
			base_corner_c + slice_offset * ((MLfloat)slice),
			base_corner_d + slice_offset * ((MLfloat)slice)
			);
		geometry->setName(wem_name);
		geometry->getWEMPatchAt(0)->setLabel(wem_name);

		/* add mesh to internal format */
		std::string spec;
		spec += "<WEMLabel>" + wem_name + "</WEMLabel>";
		spec += "<ObjectName>" + node_name + "</ObjectName>";
		spec += "<GroupPath>/" + group_path + "/</GroupPath>";
		spec += "<Color>.1.000 1.000 1.000 0.2</Color>";
		spec += "<SpecularColor>1.000 1.000 1.000</SpecularColor>";
		spec += "<ModelVisibility>3</ModelVisibility>";
		spec += "</Mesh>";
		U3DTools::addMeshModelAndGeometry({ spec }, _outU3DObject, geometry, node_name + " ");

		/* create and add slice texture */
		_outU3DObject->textureResources.push_back(TextureResource());
		auto texture_iterator = _outU3DObject->textureResources.end() - 1;
		texture_iterator->resourceName = texture_name;
		texture_iterator->width = slice_resolution_x;
		texture_iterator->height = slice_resolution_y;
		write_slice_buffer_to_png(buffer[slice], &(texture_iterator->pictureData), slice_resolution_x, slice_resolution_y);

		/* apply slice texture */
		U3DTools::setNodeShaderToTexture(_outU3DObject, node_name, texture_name, texture_map);
	}

	free_png_buffer(buffer, num_slices, slice_resolution_x, slice_resolution_y);

	_progressFld->setProgressValue(1);
	_statusFld->setStringValue("Done.");
}


ML_END_NAMESPACE