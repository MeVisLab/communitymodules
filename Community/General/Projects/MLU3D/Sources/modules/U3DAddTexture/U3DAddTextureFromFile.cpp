//----------------------------------------------------------------------------------
// U3DAddTextureFromFile module.
//
// \file    U3DAddTextureFromFile.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------



// Local includes
#include "U3DAddTextureFromFile.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_Constants.h"
#include <boost/filesystem.hpp>
#define PNG_DEBUG 3
#include <png.h>


ML_START_NAMESPACE

using namespace mlU3D;




// Variables representing the currently processed image
static int x, y;
static int width, height;
static png_byte color_type;
static png_byte bit_depth;
static png_structp png_ptr;
static png_infop info_ptr;
static int number_of_passes;
static png_bytep * row_pointers;

// Read PNG from file System
static std::string read_png_file(const char* file_name)
{
	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp){
		  return "File could not be opened for reading";
	}

	/* initialize struct */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr){
		return "internal error";
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		return "internal error";
	}

	#pragma warning(push)
	#pragma warning(disable:4611)
	//the local code is basically C
	if (setjmp(png_jmpbuf(png_ptr))){
		return "internal error";
	}

	png_init_io(png_ptr, fp);

	png_read_info(png_ptr, info_ptr);

	/* set helper variables */
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);

	/* conversion to rgba */
	if (color_type == PNG_COLOR_TYPE_RGB){
		png_set_filler(png_ptr, 0xffff, PNG_FILLER_AFTER);
		color_type = PNG_COLOR_TYPE_RGBA;
	}

	/* read file */
	png_read_update_info(png_ptr, info_ptr);


	if (setjmp(png_jmpbuf(png_ptr))){
		return "unable to read Image";
	}
	#pragma warning(pop)
	row_pointers = new png_bytep[height];
	for (y = 0; y<height; y++)
		row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr,info_ptr)];

	png_read_image(png_ptr, row_pointers);

	fclose(fp);
	return "";
}

// callback function for png_set_write_fn 
static void png_write_to_vector(png_structp png_ptr, png_bytep data, png_size_t length){
	std::vector<uint8_t> *p = (std::vector<uint8_t>*)png_get_io_ptr(png_ptr);
	p->insert(p->end(), data, data + length);
}

// writes current png to a vector and deletes it from libpng
static std::string write_png_file(std::vector<uint8_t>* out)
{

	/* create structs */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr){
		return "internal error";
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		return "internal error";
	}

	/* set callback */
	png_set_write_fn(png_ptr,out,png_write_to_vector,NULL);
	

	#pragma warning(push)
	#pragma warning(disable:4611)

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr))){
		return "unable to write PNG header";
	}

	png_set_IHDR(png_ptr, info_ptr, width, height,
		bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))){
		return "error during PNG creation";
	}

	png_write_image(png_ptr, row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr))){
		return "error during PNG creation";
	}
	#pragma warning(pop)

	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (y = 0; y<height; y++)
		delete[] row_pointers[y];
 	delete[] row_pointers;
	return "";

}




//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddTextureFromFile, U3DProcessor);


//***********************************************************************************

U3DAddTextureFromFile::U3DAddTextureFromFile() : U3DProcessor()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.

  (_fileNameFld = addString("filename"))->setStringValue("");
  (_statusFld = addString("status"))->setStringValue("Idle.");
  (_textureSpecifikationFld = addString("spec"))->setStringValue("");


  // Add and initialize parameter fields


  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddTextureFromFile::~U3DAddTextureFromFile()
{
  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddTextureFromFile::handleNotification(Field* field)
{
  /*
  if (field == _myField)
  {


  }
  */

  if (_autoApplyFld->getBoolValue() && _fileNameFld->getStringValue() != "")
  {
    process();
  }


  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddTextureFromFile::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}

//helper struct for parsed specifications
struct texture_specifications{
	std::string mesh_name;
	std::vector < std::pair<MLfloat, MLfloat>> uvs;
};

//parses specification string to struct vector
static std::vector<texture_specifications> parseSpecification(std::string spec){
	/* seperate object specifications */
	StringVector elements = U3DTools::stringSplit(spec, "<Texture>", false);
	std::vector<struct texture_specifications> ret;

	/* iterate over object specifications */
	for (unsigned int i = 0; i < elements.size(); i++){
		texture_specifications cur;

		/* check for valid ending and cut it */
		if (elements[i].substr(elements[i].size() - 10, 10) != "</Texture>"){
			return std::vector<texture_specifications>();
		}
		elements[i] = elements[i].substr(0, elements[i].size() - 10);

		/* get ObjectName and cut it*/
		size_t end = elements[i].find("</ObjectName>");
		if (elements[i].substr(0, 12) != "<ObjectName>" || end == std::string::npos){
			return std::vector<texture_specifications>();
		}
		cur.mesh_name = elements[i].substr(12,end-12);
		elements[i] = elements[i].substr(end + 13, elements[i].size() - end - 13);

		/* seperate UV Coordinates and iterate */
		StringVector uvs = U3DTools::stringSplit(elements[i],"<UV>",false);
		for (int j = 0; j < uvs.size(); j++){

			/* check for valid ending and cut it */
			if (uvs[j].substr(uvs[j].size() - 5, 5) != "</UV>"){
				return std::vector<texture_specifications>();
			}

			/* parse UVs to float */
			uvs[j] = uvs[j].substr(0, uvs[j].size() - 5);
			MLfloat u, v;
			std::string::size_type sz;
			try{
				u = std::stof(uvs[j], &sz);
				v = std::stof(uvs[j].substr(sz+2), &sz);
				cur.uvs.push_back(std::pair<MLfloat, MLfloat>(u, v));
			}
			catch (std::exception){
				return std::vector<texture_specifications>();
			}
		}

		/* add parsed object to return value*/
		ret.push_back(cur);
	}
	return ret;
}


//***********************************************************************************



void U3DAddTextureFromFile::process()
{

	/* Dont start without a file */
	if (_fileNameFld->getStringValue() == ""){
		return;
	}

  if (_inU3DValid)
  {
	  _statusFld->setStringValue("adding texture");

	  /* parse specification */
	  std::string currentTextureName = "texture" + std::to_string(_outU3DObject->textureResources.size() + 1);
	  std::vector<texture_specifications> specs = parseSpecification(_textureSpecifikationFld->getStringValue());
	  if (specs.size() == 0){
		  _statusFld->setStringValue("invalid specification");
		  return;
	  }

	  /* create and add texture resource */
	  _outU3DObject->textureResources.push_back(TextureResource());
	  /* get current resource */
	  auto res = _outU3DObject->textureResources.end() - 1;
	  /* read image */
	  std::string s = read_png_file(_fileNameFld->getStringValue().c_str());
	  if (s != ""){
		  _statusFld->setStringValue(s);
		  _outU3DObject->textureResources.erase(res);
		  return;
	  }
	  /* write image to resource */
	  s = write_png_file(&res->pictureData);
	  if (s != ""){
		  _statusFld->setStringValue(s);
		  _outU3DObject->textureResources.erase(res);
		  return;
	  }
	  res->height = height;
	  res->width = width;
	  res->resourceName = currentTextureName;

	  /* apply texture to specified objects */
	  for (int ispecs = 0; ispecs < specs.size(); ispecs++){
		  U3DTools::setNodeShaderToTexture(_outU3DObject,specs[ispecs].mesh_name,currentTextureName,specs[ispecs].uvs);
	  }


	  _statusFld->setStringValue("texture added");
  }
  else
  {
    _statusFld->setStringValue("U3D Input invalid");
  }

  _outU3DObjectFld->touch();
}

//***********************************************************************************


ML_END_NAMESPACE