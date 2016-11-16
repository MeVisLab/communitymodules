//----------------------------------------------------------------------------------
//! The ML module class ${vars.moduleName}.
/*!
// \file    ${__TARGETFILE__}
// \author  ${vars.author}
// \date    ${__DATE__}
//
// ${vars.comment}
*/
//----------------------------------------------------------------------------------

// Local includes
#include "${vars.moduleName}.h"

// Project includes
#include "MLPDF_Tools.h"
#include "MLPDF_Defines.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(${vars.moduleName}, PDFGenerator);

//----------------------------------------------------------------------------------

${vars.moduleName}::${vars.moduleName}() : PDFGenerator()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

%if fields:
  // Add automatically generated fields and set their values.
 %for f in fields:
  %if f.fieldType.upper() == "ENUM":
  static const char * const ${f.fieldName}Values[] = { ${f.enumValuesInitializer} };
  _${f.fieldName}Fld = addEnum("${f.fieldName}", ${f.fieldName}Values, ${f.enumNumValues});
  _${f.fieldName}Fld->setEnumValue(${f.fieldCValue});
  %elif f.fieldType.upper() == "NOTIFY":
  _${f.fieldName}Fld = addNotify("${f.fieldName}");
  %elif f.fieldType.upper() == "ROTATION":
  _${f.fieldName}Fld = addRotation("${f.fieldName}");
  _${f.fieldName}Fld->setStringValue("${f.fieldValue}");
  %else:
  _${f.fieldName}Fld = add${f.fieldType}("${f.fieldName}", ${f.fieldCValue});
  %endif
 %endfor
%endif

%if vars.includeDemoCode:
  // Add fields for demo code
  _imageFilenameFld  = addString("imageFilename",  "PDFDemoData_Image.png");
  _modelFilenameFld  = addString("modelFilename",  "PDFDemoData_Model.u3d");
  _posterFilenameFld = addString("posterFilename", "PDFDemoData_ModelPoster.png");
%endif

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

${vars.moduleName}::~${vars.moduleName}()
{
  // Destroy own dynamic data structures here!


  // Super class destructor is called automatically!
}

//----------------------------------------------------------------------------------

void ${vars.moduleName}::handleNotification(Field* field)
{
  // Call super class functionality first!
  PDFGenerator::handleNotification(field);

  // Handle changes of module parameters and input image fields here.
}

//----------------------------------------------------------------------------------

bool ${vars.moduleName}::assemblePDFDocument()
{
  // *************************************************************************
  // This is the main method for all modules derived from PDFCreatorBase.
  // Add all code that assembles the actual PDF document here!
%if vars.includeDemoCode:
  //
  // The following code is a simple example that demonstrates the usage and
  // built-in methods to add content to the PDF.
%endif
  // *************************************************************************

%if vars.includeDemoCode:
  // Predefined helper variable to store current y position for drawing
  // Attention! pdfDoc_CurrentYPos (and pdfDoc_CurrentXPos, respectively)
  // are automatically set to zero at each time a new page is started. 
  pdfDoc_CurrentYPos = 0;

  // Enable compression for images only (Default is mlPDF::COMPRESS_ALL).
  // Writing the actual PDF file to disk is considerably speeded up if compression
  // is disbled. However, file size can considerably increase!
  pdfDoc_SetCompressionMode(mlPDF::COMPRESS_IMAGES);

  // Set the passwords
  // The first password protects the document from unauthorized modifications.
  // The second password protects the document from unautorized access and can be empty.
  pdfDoc_SetPasswords("ModifyMe", "");

  // Set permissions
  // Attention: A password must be set PRIOR to setting permissions.
  // If no password has been set when pdfDoc_SetPermissions() is invoked,
  // a random password of 64 characters (512 bit) length is set!
  pdfDoc_SetPermissions(mlPDF::PERMIT_PRINT_HIGH_QUALITY | mlPDF::PERMIT_EXTRACT_FOR_ACCESSIBILITY);

  // Open document with thumbnail preview.
  pdfDoc_SetDocumentOpenMode(mlPDF::DOCUMENTOPENMODE_SHOW_THUMBS);
  
  // Display pages with "Single Page" layout.
  pdfDoc_SetInitialPageDisplayLayout(mlPDF::PAGELAYOUT_SINGLEPAGE);

  // Set y axis reference to mlPDF::YAXIS_REFERENCE_TOP
  // This will result in all y coordinates being interpreted with the upper page border as zero
  // This is also the default setting and actually does not need to be called here... 
  // However, the availability of this method shall be demonstrated. ;-)
  pdfDoc_SetYAxisReference(mlPDF::YAXIS_REFERENCE_TOP);

  // Add a page and set size & direction
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);

  // Set page margins (i.e., the space that shall not be written at
  pdfDoc_SetGlobalPageMarginsMM(20, 20, 20, 15);

  // Set text font to Times Bold, 14 pt.
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 14);

  // Write text at specified position
  pdfDoc_WriteTextAt(0, pdfDoc_CurrentYPos, "PDF Creation Example");

  pdfDoc_CurrentYPos += 25;

  // Set font to Times, 12 pt.
  pdfDoc_SetCurrentFont(buildInFonts.Times, 12);

  // Write text at specified position
  pdfDoc_WriteTextAt(10, pdfDoc_CurrentYPos, "Created by '${vars.moduleName}' module.");

  pdfDoc_CurrentYPos += 35;

  // Adjust font size without changing the font type.
  pdfDoc_SetCurrentFontSize(10);

  // Write lorem ipsum text into specified area
  pdfDoc_WriteTextAreaAt(0, pdfDoc_CurrentYPos, pdfDoc_GetPageMaxWidth(), 50, mlPDF::LOREM_IPSUM, mlPDF::TEXTALIGNMENT_JUSTIFY);

  // Draw a rectangle around the recently added text
  pdfDoc_AddOutlineRectangle(0, pdfDoc_CurrentYPos, pdfDoc_GetPageMaxWidth(), 50, 1);

  // Set font to Courier Oblique, keep current font size.
  pdfDoc_SetCurrentFont(buildInFonts.CourierOblique);

  // Backup current y-axis reference (which is mlPDF::YAXIS_REFERENCE_TOP by default)
  pdfDoc_StoreYAxisReference();

  // Now set the y-axis reference to mlPDF::YAXIS_REFERENCE_BOTTOM, 
  // which will result in all y coordinates being interpreted with the lower page border as zero
  pdfDoc_SetYAxisReference(mlPDF::YAXIS_REFERENCE_BOTTOM); 

  // Write text into specified area at lower right corner of the page (note that the preset page margins shall be ignored)
  float textAreaWidth = pdfDoc_GetPageMaxWidth(mlPDF::IGNORE_MARGINS) / 2;
  pdfDoc_WriteTextAreaAt(pdfDoc_GetPageMaxX(mlPDF::IGNORE_MARGINS) - textAreaWidth, 0, textAreaWidth, 25, "A text in the lower right corner of the page.\nPage margins are ignored!", mlPDF::TEXTALIGNMENT_RIGHT, mlPDF::IGNORE_MARGINS);

  // Now restore the previous y-axis reference
  // pdfDoc_RestoreDefaultYAxisReference() would have worked here as well.
  pdfDoc_RestoreYAxisReference();

  // Go back to Times font.
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 10);

  // Write headline for the following images
  pdfDoc_CurrentYPos += 80;
  pdfDoc_WriteTextAt(0, pdfDoc_CurrentYPos, "Two Images Sharing the Same Resource");
 
  // Load image into image resource pool (n.b.: only PNG anf JPG images are allowed!)
  std::string resourcesPath = resourcesPathFld->getStringValue() + "/";
  std::string exampleImagePath = resourcesPath + _imageFilenameFld->getStringValue();

  if (mlPDF::fileExists(exampleImagePath))
  {
    mlPDF::IMAGE exampleImage = pdfDoc_LoadImageFromFile(exampleImagePath);

    // Add image by direct reference
    pdfDoc_CurrentYPos += 15;
    pdfDoc_AddImage(0, pdfDoc_CurrentYPos, 100, 100, exampleImage);

    // Add image once more through image pool
    if (pdfDocImages.size() > 0)
    {
      pdfDoc_AddImage(110, pdfDoc_CurrentYPos, 100, 100, pdfDocImages[pdfDocImages.size() - 1]);
    }
  }
  else
  {
    // Image could not be found. -> Cancel assembly, set error message
    assemblyErrorMessage = "Image file not found.";
    return false;
  }

  // Write headline for the interactive 3D scene
  pdfDoc_CurrentYPos += 120;
  pdfDoc_WriteTextAt(0, pdfDoc_CurrentYPos, "Interactive 3D Scene with Poster Image");

  // Now add 3D scene.
  // This needs a little more than just one command.
  // Therefore it has been outsourced to a separate method...
  pdfDoc_CurrentYPos += 15;
  _add3DFigure(0, pdfDoc_CurrentYPos, 210, 210);

  // Now let's add some graphics...

  // First change fill color from default black to green.
  // Attention! This applies for text as well!
  pdfDoc_SetFillColor(0, 1, 0);

  // Add a green circle with black outline (black is the default stroke color)
  pdfDoc_AddFilledOutlineCircle(pdfDoc_GetPageCenterX()+80, pdfDoc_GetPageCenterY(), 50, 2);

  // And some text to show that the fill color also applies for text
  pdfDoc_WriteTextAt(pdfDoc_GetPageCenterX() + 140, pdfDoc_GetPageCenterY(), "This text is now green!");

  // Add a filled blue arc without outline
  pdfDoc_SetFillColor(0, 0, 1);
  pdfDoc_AddFilledPieDegrees(pdfDoc_GetPageCenterX() + 80, pdfDoc_GetPageCenterY() + 100, 50, 60, -40);

  // And a yellow outline ellipse
  pdfDoc_SetStrokeColor(1, 1, 0);
  pdfDoc_AddOutlineEllipse(pdfDoc_GetPageCenterX() + 80, pdfDoc_GetPageCenterY() + 200, 50, 25, 1);

  // Finally draw a fat red line from the lower left corner to the upper right corner! :-)
  pdfDoc_SetYAxisReference(mlPDF::YAXIS_REFERENCE_BOTTOM);
  pdfDoc_SetStrokeColor(1, 0, 0);
  pdfDoc_AddLine(0, 0, pdfDoc_GetPageMaxX(true), pdfDoc_GetPageMaxY(true), 5, true);

  //
  // For more methods to add and manipulate contents, see MLPDF_PDFCreatorBase.h file.
  //
%endif

  return true;
}

%if vars.includeDemoCode:
//----------------------------------------------------------------------------------

void ${vars.moduleName}::_add3DFigure(float x, float y, float width, float height)
{
  std::string resourcesPath = resourcesPathFld->getStringValue() + "/";

  // Load model into model resource pool (n.b.: only U3D models are allowed!)
  HPDF_U3D u3dModel = pdfDoc_Load3DModelDataFromFile(resourcesPath + _modelFilenameFld->getStringValue());

  if (u3dModel)
  {
    // Create a view for the model
    mlPDF::VIEW3D anteriorView = pdfDoc_3DModel_CreateView("Anterior View", mlPDF::LIGHTING_SCHEME_HEADLAMP, 0.75f, 0.75f, 0.75f);

    // Set camera properties for this view 
    // The values are pre-calculated for the example model.
    // Alternatively they can be taken from the camera* fields.
    Vector3 centerOfOrbit(92.983320219395, -144.044277472549, 1118.7488450232);
    float   radiusOfOrbit = 605.31464f;
    Vector3 centerToCamera(107.696596162441, -587.894809441514, 95.8489332971112);
    float   fovAngle = 42.787704f;
    float   rollAngle = 351.67535f;
    pdfDoc_3DView_SetPerspectiveCamera(anteriorView, centerOfOrbit, radiusOfOrbit, centerToCamera, fovAngle, rollAngle);
    pdfDoc_3DModel_AddView(u3dModel, anteriorView);

    // Create another view
    mlPDF::VIEW3D lateralView = pdfDoc_3DModel_CreateView("Lateral View", mlPDF::LIGHTING_SCHEME_CAD, 0.75f, 0.75f, 0.75f);
    centerOfOrbit = Vector3(92.983320219395, -144.044277472549, 1118.7488450232);
    radiusOfOrbit = 605.31464f;
    centerToCamera = Vector3(-598.932991211859, -56.9541532176117, 66.6430675379161);
    fovAngle = 42.787704f;
    rollAngle = 8.9973822f;
    pdfDoc_3DView_SetPerspectiveCamera(lateralView, centerOfOrbit, radiusOfOrbit, centerToCamera, fovAngle, rollAngle);
    pdfDoc_3DModel_AddView(u3dModel, lateralView);

    // Set default view
    pdfDoc_3DModel_SetDefaultView(u3dModel, anteriorView);

    // Load poster image and add it to the image resource pool
    mlPDF::IMAGE posterImage = pdfDoc_LoadImageFromFile(resourcesPath + _posterFilenameFld->getStringValue());

    // Now actually create the 3D scene
    mlPDF::SCENE3D u3dScene = pdfDoc_Add3DScene(x, y, width, height, u3dModel, posterImage);

    // And finally set the scene properties.
    std::string activationMode = mlPDF::ActivationModeStrings[mlPDF::ACTIVATION_MODE_EXPLICIT_ACTIVATE];
    std::string deactivationMode = mlPDF::DeactivationModeStrings[mlPDF::DEACTIVATION_MODE_EXPLICIT_DEACTIVATE];
    bool toolbarEnabled = true;
    bool navigationInterfaceEnabled = false;
    bool animationAutoStart = false;
    pdfDoc_3DScene_SetActivationProperties(u3dScene, activationMode, deactivationMode, toolbarEnabled, navigationInterfaceEnabled, animationAutoStart);
  }
}
%endif

//----------------------------------------------------------------------------------

ML_END_NAMESPACE