//----------------------------------------------------------------------------------
//! The Inventor module class SoVisLogo
/*!
// \file    SoVisLogo.cpp
// \author  Konrad Mühler
// \date    2007-06-12
//
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoVisLogo.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif
#include <Inventor/actions/SoGLRenderAction.h>
// TODO: include more inventor things here
#include "XVLeaveScope.h"

#include <GL/glew.h>
#pragma warning( disable : 4611 )
#include "png.h"
#include <malloc.h>

SO_NODE_SOURCE(SoVisLogo)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoVisLogo::initClass()
{
	SO_NODE_INIT_CLASS(SoVisLogo, SoShape, "Shape");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoVisLogo::SoVisLogo()
{
	SO_TRACE_IN("SoVisLogo::SoVisLogo")

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoVisLogo);

	SO_NODE_ADD_FIELD(fldLocalPath, (""));

	SO_NODE_ADD_FIELD(fldFilename, (vislogo));
	SO_NODE_DEFINE_ENUM_VALUE(logoNames, vislogo);
	SO_NODE_DEFINE_ENUM_VALUE(logoNames, otto);
	SO_NODE_DEFINE_ENUM_VALUE(logoNames, otto2);
	SO_NODE_DEFINE_ENUM_VALUE(logoNames, lst);
	SO_NODE_DEFINE_ENUM_VALUE(logoNames, neck);
	SO_NODE_SET_SF_ENUM_TYPE(fldFilename, logoNames);

	SO_NODE_ADD_FIELD(fldCorner, (topright));
	SO_NODE_DEFINE_ENUM_VALUE(corners, topright);
	SO_NODE_DEFINE_ENUM_VALUE(corners, topleft);
	SO_NODE_DEFINE_ENUM_VALUE(corners, bottomright);
	SO_NODE_DEFINE_ENUM_VALUE(corners, bottomleft);
	SO_NODE_SET_SF_ENUM_TYPE(fldCorner, corners);


	// Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
	// sensor to be sure that changes are not delayed or collected.
	_nodeSensor = new SoNodeSensor(SoVisLogo::nodeChangedCB, this);
	_nodeSensor->setPriority(0);
	_nodeSensor->attach(this);

	filenameSensor = new SoFieldSensor(filenameChangedCB, this);
	filenameSensor->setPriority(0);
    filenameSensor->attach(&fldLocalPath);
	localPathSensor = new SoFieldSensor(filenameChangedCB, this);
	localPathSensor->setPriority(0);
	localPathSensor->attach(&fldFilename);
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoVisLogo::~SoVisLogo()
{
	SO_TRACE_IN("SoVisLogo::~SoVisLogo")
	// Remove the node sensor.
	if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoVisLogo::nodeChangedCB(void *data, SoSensor* sens)
{
	SO_TRACE_IN("SoVisLogo::nodeChangedCB")

	((SoVisLogo*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoVisLogo::nodeChanged(SoNodeSensor* /*sensor*/)
{
	SO_TRACE_IN("SoVisLogo::nodeChanged")

	// Get the field which caused the notification.
	//SoField* field = sensor->getTriggerField();

	// Handle changed fields here
}


//! called whenever the scene is rendered
void SoVisLogo::GLRender(SoGLRenderAction *action)
{
	SO_TRACE_IN("SoVisLogo::GLRender")

	// Save the current transformation and attribute state of OpenGL.
	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	SoState * state = action->getState();
	const SbViewportRegion & vp = SoViewportRegionElement::get(state);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();                                           //Einheitsmatrix
	glMatrixMode(GL_PROJECTION);                                //"2D"-Modus
    glLoadIdentity();                                           //Einheitsmatrix

	if (load_result!=NULL && png_image!=NULL)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		GLuint textureID;
		glGenTextures( 1, &textureID );
		glBindTexture( GL_TEXTURE_2D, textureID );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png_image);

		glBegin(GL_POLYGON);
		float border = 10.0;
		float scale = 1.0;
		if (fldCorner.getValue()==bottomleft)
		{
			glTexCoord2f(0.0, 0.0);glVertex2f(-1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[0], -1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 0.0);glVertex2f(-1.0f + 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], -1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 1.0);glVertex2f(-1.0f + 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], -1.0f+2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(0.0, 1.0);glVertex2f(-1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[0],-1.0f + 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
		}
		else if (fldCorner.getValue()==bottomright)
		{
			glTexCoord2f(0.0, 0.0);glVertex2f(1.0f - 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], -1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 0.0);glVertex2f(1.0f - (float)(border)/(float)vp.getWindowSize()[0], -1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 1.0);glVertex2f(1.0f - (float)(border)/(float)vp.getWindowSize()[0], -1.0f + 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(0.0, 1.0);glVertex2f(1.0f - 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0],-1.0f + 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
		}
		else if (fldCorner.getValue()==topright)
		{
			glTexCoord2f(0.0, 0.0);glVertex2f(1.0f - 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 0.0);glVertex2f(1.0f - (float)(border)/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 1.0);glVertex2f(1.0f - (float)(border)/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)(border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(0.0, 1.0);glVertex2f(1.0f - 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)border/(float)vp.getWindowSize()[1]);
		}
		else if (fldCorner.getValue()==topleft)
		{
			glTexCoord2f(0.0, 0.0);glVertex2f(-1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 0.0);glVertex2f(-1.0f + 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)(img_height*scale+border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(1.0, 1.0);glVertex2f(-1.0f + 2.0f*(float)(img_width*scale+border)/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)(border)/(float)vp.getWindowSize()[1]);
			glTexCoord2f(0.0, 1.0);glVertex2f(-1.0f + 2.0f*(float)border/(float)vp.getWindowSize()[0], 1.0f - 2.0f*(float)border/(float)vp.getWindowSize()[1]);
		}
		//std::cout << "img_w:" << img_width << "   windowSize[0]" << vp.getWindowSize()[0] << std::endl;
		//std::cout << "x:" << -1.0f+2.0f*(float)img_width/(float)vp.getWindowSize()[0] << "  y:" << -1.0f+2.0f*(float)img_height/(float)vp.getWindowSize()[1] << std::endl;
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
	}
	//else std::cout << "result!=NULL oder image!=NULL" << std::endl;

	glFlush();

	// Restore the transformation and attribute state of OpenGL.
	glPopAttrib();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode (GL_PROJECTION);
	glPopMatrix();
	glMatrixMode (GL_MODELVIEW);
}

//! called when an SoEvent is passed through the scene
void SoVisLogo::handleEvent(SoHandleEventAction* /*action*/)
{
	SO_TRACE_IN("SoVisLogo::handleEvent")
	// TODO
}

//! called when a scene bounding box is computed
void SoVisLogo::computeBBox(SoAction* /*action*/, SbBox3f& /*box*/, SbVec3f& /*center*/)
{
  SO_TRACE_IN("SoVisLogo::computeBBox")
  // TODO
}

//! called by the generate primitives action
void SoVisLogo::generatePrimitives(SoAction* /*action*/)
{
  SO_TRACE_IN("SoVisLogo::generatePrimitives")
  // TODO
}

//! called by the ray pick action
void SoVisLogo::rayPick(SoRayPickAction* /*action*/)
{
  SO_TRACE_IN("SoVisLogo::rayPick")
  // TODO
}



int SoVisLogo::pngLoad(const char *file, unsigned long &pwidth, unsigned long &pheight, char* &image_data_ptr)
{
   FILE         *infile;         /* PNG file pointer */
   png_structp   png_ptr;        /* internally used by libpng */
   png_infop     info_ptr;       /* user requested transforms */

   char         *image_data;      /* raw png image data */
   char         sig[8];           /* PNG signature array */

   int           bit_depth;
   int           color_type;

   png_uint_32 width;            /* PNG image width in pixels */
   png_uint_32 height;           /* PNG image height in pixels */
   unsigned int rowbytes;         /* raw bytes at row n in image */

   image_data = NULL;
   png_bytepp row_pointers = NULL;

   /* Open the file. */
   infile = fopen(file, "rb");
   if (!infile) return 0;

   /*
    * 		13.3 readpng_init()
    */

   /* Check for the 8-byte signature */
   fread(sig, 1, 8, infile);

   if (!png_check_sig((unsigned char *) sig, 8)) {
      fclose(infile);
      return 0;
   }

   /*
    * Set up the PNG structs
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr) {
      fclose(infile);
      return 4;    /* out of memory */
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
      fclose(infile);
      return 4;    /* out of memory */
   }


  /*
   * block to handle libpng errors,
   * then check whether the PNG file had a bKGD chunk
   */
   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(infile);
      return 0;
   }

  /*
   * takes our file stream pointer (infile) and
   * stores it in the png_ptr struct for later use.
   */
   /* png_ptr->io_ptr = (png_voidp)infile;*/
   png_init_io(png_ptr, infile);

  /*
   * lets libpng know that we already checked the 8
   * signature bytes, so it should not expect to find
   * them at the current file pointer location
   */
   png_set_sig_bytes(png_ptr, 8);

   /* Read the image info.*/

  /*
   * reads and processes not only the PNG file's IHDR chunk
   * but also any other chunks up to the first IDAT
   * (i.e., everything before the image data).
   */

   /* read all the info up to the image data  */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
	&color_type, NULL, NULL, NULL);

   pwidth = width;
   pheight = height;

   /* Set up some transforms. */
   if (color_type & PNG_COLOR_MASK_ALPHA) {
//	   std::cout << "ALPHA1" << std::endl;
      png_set_strip_alpha(png_ptr);
   }
   if (bit_depth > 8) {
      png_set_strip_16(png_ptr);
//	  std::cout << "RGB>8" << std::endl;
   }
   if (color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
      png_set_gray_to_rgb(png_ptr);
//	  std::cout << "GRAY" << std::endl;
   }
   if (color_type == PNG_COLOR_TYPE_PALETTE) {
      png_set_palette_to_rgb(png_ptr);
//	  std::cout << "PALETTE" << std::endl;
   }

   /* Update the png info struct.*/
   png_read_update_info(png_ptr, info_ptr);

   /* Rowsize in bytes. */
   rowbytes = png_get_rowbytes(png_ptr, info_ptr);


   /* Allocate the image_data buffer. */
   if ((image_data = (char *) malloc(rowbytes * height))==NULL) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return 4;
    }

   if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return 4;
    }

    /* set the individual row_pointers to point at the correct offsets */
    unsigned int i;
    for (i = 0;  i < height;  ++i)
        row_pointers[height - 1 - i] = (png_byte * ) image_data + i*rowbytes;

    /* now we can go ahead and just read the whole image */
    png_read_image(png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

   /* Clean up. */
   free(row_pointers);

   /* Clean up. */
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   fclose(infile);

   image_data_ptr = image_data;

   return 1;
}


void SoVisLogo::filenameChangedCB(void *data, SoSensor* )
{
	std::string logoNamesStr[5] = {"vislogo.png", "otto.png", "otto2.png", "lst.png", "neck.png"};
	SoVisLogo *myself = (SoVisLogo *)data;
	if (myself->fldFilename.getValue()>=5) return;
	std::string filename = std::string(myself->fldLocalPath.getValue().getString())+"/"+logoNamesStr[myself->fldFilename.getValue()];
	//std::cout << filename << std::endl;
	myself->load_result = myself->pngLoad(filename.c_str(), myself->img_width, myself->img_height, myself->png_image);
}


void SoVisLogo::activateAttachments()
{
	fldFilename.touch();
}
