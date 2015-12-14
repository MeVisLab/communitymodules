//----------------------------------------------------------------------------------
//! Defines for PDF file creation.
/*!
// \file    MLPDF_Defines.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_Defines_H
#define _PDF_Defines_H 

// ML includes
#include <mlModuleIncludes.h>

// Thirdparty includes
#include <hpdf.h>


ML_START_NAMESPACE

namespace mlPDF 
{
  typedef HPDF_Page       PAGE;
  typedef HPDF_Image      IMAGE;
  typedef HPDF_U3D        MODEL3D;
  typedef HPDF_Dict       VIEW3D;
  typedef HPDF_Annotation SCENE3D;

  static bool IGNORE_MARGINS = true;

  static bool YAXIS_REFERENCE_DEFAULT  = true;
  static bool YAXIS_REFERENCE_TOP      = true;
  static bool YAXIS_REFERENCE_BOTTOM   = false;
  static bool YAXIS_REFERENCE_NATIVE   = false;

  static std::string LOREM_IPSUM = "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua.\nUt enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\nExcepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n";

  static unsigned int COMPRESS_NOTHING   = HPDF_COMP_NONE;
  static unsigned int COMPRESS_TEXT      = HPDF_COMP_TEXT;
  static unsigned int COMPRESS_IMAGES    = HPDF_COMP_IMAGE;
  static unsigned int COMPRESS_METADATA  = HPDF_COMP_METADATA;
  static unsigned int COMPRESS_ALL       = HPDF_COMP_ALL;

  static unsigned int PERMIT_PRINT_LOW_QUALITY         = 0x0004;  // Bit 3:  Print the document (possibly not at the highest quality level, depending on whether bit 12 is also set).
  static unsigned int PERMIT_MODIFY                    = 0x0008;  // Bit 4:  Modify the contents of the document by operations other than those controlled by bits 6, 9, and 11.
  static unsigned int PERMIT_EXTACT_TEXT_AND_GRAPHICS  = 0x0010;  // Bit 5:  Copy or otherwise extract text and graphics from the document by operations other than that controlled by bit 10.
  static unsigned int PERMIT_MODIFY_ANNOTATIONS        = 0x0020;  // Bit 6:  Add or modify text annotations, fill in interactive form fields, and, if bit 4 is also set, create or modify interactive form fields (including signature fields).
  static unsigned int PERMIT_FORM_FIELDS               = 0x0100;  // Bit 9:  Fill in existing interactive form fields (including signature fields), even if bit 6 is clear.
  static unsigned int PERMIT_EXTRACT_FOR_ACCESSIBILITY = 0x0200;  // Bit 10: Extract text and graphics (in support of accessibility to users with disabilities or for other purposes).
  static unsigned int PERMIT_DOCUMENT_ASSEMBLY         = 0x0400;  // Bit 11: Assemble the document (insert, rotate, or delete pages and create bookmarks or thumbnail images), even if bit 4 is clear.
  static unsigned int PERMIT_PRINT_HIGH_QUALITY        = 0x0804;  // Bit 12: Print the document to a representation from which a faithful digital copy of the PDF content could be generated. When this bit is clear (and bit 3 is set), printing is limited to a low-level representation of the appearance, possibly of degraded quality.
  static unsigned int PERMIT_ALL                       = 0x0FFF;  // Enable all permissions (default)

  enum PAGE_SIZES {
    PAGESIZE_LETTER       = HPDF_PAGE_SIZE_LETTER,       // 612     x 792       pixels
    PAGESIZE_LEGAL        = HPDF_PAGE_SIZE_LEGAL,        // 612     x 1008      pixels
    PAGESIZE_A3           = HPDF_PAGE_SIZE_A3,           // 841.89  x 1199.551  pixels
    PAGESIZE_A4           = HPDF_PAGE_SIZE_A4,           // 595.276 x 841.89    pixels
    PAGESIZE_A5           = HPDF_PAGE_SIZE_A5,           // 419.528 x 595.276   pixels
    PAGESIZE_B4           = HPDF_PAGE_SIZE_B4,           // 708.661 x 1000.63   pixels
    PAGESIZE_B5           = HPDF_PAGE_SIZE_B5,           // 498.898 x 708.661   pixels
    PAGESIZE_EXECUTIVE    = HPDF_PAGE_SIZE_EXECUTIVE,    // 522     x 756       pixels
    PAGESIZE_US4x6        = HPDF_PAGE_SIZE_US4x6,        // 288     x 432       pixels
    PAGESIZE_US4x8        = HPDF_PAGE_SIZE_US4x8,        // 288     x 576       pixels
    PAGESIZE_US5x7        = HPDF_PAGE_SIZE_US5x7,        // 360     x 504       pixels
    PAGESIZE_COMM10       = HPDF_PAGE_SIZE_COMM10,       // 297     x 684       pixels

    NUM_PAGESIZES         = 12
  };

  const char* const PageSizeStrings[NUM_PAGESIZES] = {
    "PAGESIZE_A4",
    "PAGESIZE_LETTER",
    "PAGESIZE_LEGAL",
    "PAGESIZE_A3",
    "PAGESIZE_A5",
    "PAGESIZE_B4",
    "PAGESIZE_B5",
    "PAGESIZE_EXECUTIVE",
    "PAGESIZE_US4x6",
    "PAGESIZE_US4x8",
    "PAGESIZE_US5x7",
    "PAGESIZE_COMM10"
  };

  struct PageSizeInfoStruct
  {
    _HPDF_PageSizes PageSize;
    HPDF_REAL       Width;
    HPDF_REAL       Height;
    HPDF_REAL       MarginX;
    HPDF_REAL       MarginY;
  };

  enum PAGE_DIRECTIONS {
    PAGEDIRECTION_PORTRAIT  = HPDF_PAGE_PORTRAIT,
    PAGEDIRECTION_LANDSCAPE = HPDF_PAGE_LANDSCAPE,

    NUM_PAGEDIRECTIONS      = 3
  };

  const char* const PageDirectionStrings[NUM_PAGEDIRECTIONS] = {
    "PAGEDIRECTION_PORTRAIT",
    "PAGEDIRECTION_LANDSCAPE"
  };

  struct BuiltInFontsStruct
  {
    HPDF_Font Times;
    HPDF_Font TimesBold;
    HPDF_Font TimesItalic;
    HPDF_Font TimesBoldItalic;
    HPDF_Font Courier;
    HPDF_Font CourierBold;
    HPDF_Font CourierOblique;
    HPDF_Font CourierBoldOblique;
    HPDF_Font Helvetica;
    HPDF_Font HelveticaBold;
    HPDF_Font HelveticaOblique;
    HPDF_Font HelveticaBoldOblique;
    HPDF_Font Symbol;
    HPDF_Font ZapfDingbats;
  };


  enum ACTIVATION_MODES {
    ACTIVATION_MODE_EXPLICIT_ACTIVATE = 0,
    ACTIVATION_MODE_PAGE_OPEN         = 1,
    ACTIVATION_MODE_PAGE_VISIBLE      = 2,
    NUM_ACTIVATIONMODES               = 3
  };

  const char* const ActivationModeStrings[NUM_ACTIVATIONMODES] = {
    "ExplicitActivate",
    "PageOpen",
    "PageVisible"
  };

  enum DEACTIVATION_MODES {
    DEACTIVATION_MODE_EXPLICIT_DEACTIVATE = 0,
    DEACTIVATION_MODE_PAGE_CLOSED         = 1,
    DEACTIVATION_MODE_PAGE_INVISIBLE      = 2,
    NUM_DEACTIVATIONMODES                 = 3
  };

  const char* const DeactivationModeStrings[NUM_DEACTIVATIONMODES] = {
    "ExplicitDeactivate",
    "PageClosed",
    "PageInvisible"
  };

  enum LIGHTING_SCHEMES {
    LIGHTING_SCHEME_NONE       = 0,
    LIGHTING_SCHEME_WHITE,
    LIGHTING_SCHEME_DAY,
    LIGHTING_SCHEME_NIGHT,
    LIGHTING_SCHEME_HARD,
    LIGHTING_SCHEME_PRIMARY,
    LIGHTING_SCHEME_BLUE,
    LIGHTING_SCHEME_RED,
    LIGHTING_SCHEME_CUBE,
    LIGHTING_SCHEME_CAD,
    LIGHTING_SCHEME_HEADLAMP,

    NUM_LIGHTING_SCHEMES
  };

  const char* const LightingSchemeStrings[NUM_LIGHTING_SCHEMES] = {
    "None", 
    "White", 
    "Day", 
    "Night", 
    "Hard", 
    "Primary", 
    "Blue", 
    "Red", 
    "Cube", 
    "CAD", 
    "HeadLamp"
  };

  enum MODEL_RENDERMODES {
    MODEL_RENDERMODE_SOLID = 0,
    MODEL_RENDERMODE_SOLIDWIREFRAME,
    MODEL_RENDERMODE_TRANSPARENT,
    MODEL_RENDERMODE_TRANSPARENTWIREFRAME,
    MODEL_RENDERMODE_BOUNDINGBOX,
    MODEL_RENDERMODE_TRANSPARENTBOUNDINGBOX,
    MODEL_RENDERMODE_TRANSPARENTBOUNDINGBOXOUTLINE,
    MODEL_RENDERMODE_WIREFRAME,
    MODEL_RENDERMODE_SHADEDWIREFRAME,
    MODEL_RENDERMODE_HIDDENWIREFRAME,
    MODEL_RENDERMODE_VERTICES,
    MODEL_RENDERMODE_SHADEDVERTICES,
    MODEL_RENDERMODE_ILLUSTRATION,
    MODEL_RENDERMODE_SOLIDOUTLINE,
    MODEL_RENDERMODE_SHADEDILLUSTRATION,

    NUM_MODEL_RENDERMODES
  };

  const char* const ModelRenderModeStrings[NUM_MODEL_RENDERMODES] = {
    "Solid",
    "SolidWireframe",
    "Transparent",
    "TransparentWireframe",
    "BoundingBox",
    "TransparentBoundingBox",
    "TransparentBoundingBoxOutline",
    "Wireframe",
    "ShadedWireframe",
    "HiddenWireframe",
    "Vertices",
    "ShadedVertices",
    "Illustration",
    "SolidOutline",
    "ShadedIllustration"
  };

  enum TEXT_ALIGNMENTS {
    TEXTALIGNMENT_LEFT = HPDF_TALIGN_LEFT,
    TEXTALIGNMENT_RIGHT = HPDF_TALIGN_RIGHT,
    TEXTALIGNMENT_CENTER = HPDF_TALIGN_CENTER,
    TEXTALIGNMENT_JUSTIFY = HPDF_TALIGN_JUSTIFY,

    NUM_TEXTALIGNMENTS
  };

  const char* const TextAlignmentStrings[NUM_TEXTALIGNMENTS] = {
    "Left",
    "Right",
    "Center",
    "Justify"
  };

  enum TEXT_RENDERMODES {
    TEXTRENDERMODE_FILL                 = HPDF_FILL,
    TEXTRENDERMODE_STROKE               = HPDF_STROKE,
    TEXTRENDERMODE_FILL_STROKE          = HPDF_FILL_THEN_STROKE,
    TEXTRENDERMODE_INVISIBLE            = HPDF_INVISIBLE,
    TEXTRENDERMODE_FILL_CLIPPING        = HPDF_FILL_CLIPPING,
    TEXTRENDERMODE_STROKE_CLIPPING      = HPDF_STROKE_CLIPPING,
    TEXTRENDERMODE_FILL_STROKE_CLIPPING = HPDF_FILL_STROKE_CLIPPING,
    TEXTRENDERMODE_CLIPPING             = HPDF_CLIPPING,

    NUM_TEXTRENDERMODES
  };

  const char* const TextRenderModeStrings[NUM_TEXTRENDERMODES] = {
    "TEXTRENDERMODE_FILL",
    "TEXTRENDERMODE_STROKE",
    "TEXTRENDERMODE_FILL_STROKE",
    "TEXTRENDERMODE_INVISIBLE",
    "TEXTRENDERMODE_FILL_CLIPPING",
    "TEXTRENDERMODE_STROKE_CLIPPING",
    "TEXTRENDERMODE_FILL_STROKE_CLIPPING",
    "TEXTRENDERMODE_CLIPPING"
  };

  static std::string USEVERTEXCOLORS = "{VertexColors}";

} // namespace mlPDF 

ML_END_NAMESPACE

#endif // _PDF_Defines_H