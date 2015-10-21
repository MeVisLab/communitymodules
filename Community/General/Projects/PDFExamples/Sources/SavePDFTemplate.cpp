//----------------------------------------------------------------------------------
//! The ML module class SavePDFTemplate.
/*!
// \file    SavePDFTemplate.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-20
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SavePDFTemplate.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(SavePDFTemplate, Module);

//----------------------------------------------------------------------------------

SavePDFTemplate::SavePDFTemplate() : PDFCreatorBase()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate page data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

}

//----------------------------------------------------------------------------------

void SavePDFTemplate::handleNotification(Field* field)
{
  // Call super class functionality first!
  PDFCreatorBase::handleNotification(field);

  // Handle changes of module parameters and input image fields here.
}

//----------------------------------------------------------------------------------

void SavePDFTemplate::assemblePDFDocument()
{
  // This is the main method for all modules derived from PDFCreatorBase.
  // Add all code that assembles the actual PDF document here!

  // The following code is a simple example that demsontrates the usage and
  // built-in methods to add content to the PDF.

  // Helper variable to store current drawing position height
  float yPos = 0;

  // Set default y axis reference to mlPDF::YAXIS_REFERENCE_TOP
  // This will result in all y coordinates being interpreted with the upper page border as zero
  // This is also the default setting and actually does not need to be called here... 
  // However, the availability of this method shall be demonstrated. ;-)
  pdfDoc_SetDefaultYAxisReference(mlPDF::YAXIS_REFERENCE_TOP);

  // Add a page and set size & direction
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);

  // Set page margins (i.e., the space that shall not be written at
  pdfDoc_SetGlobalPageMarginsMM(20, 20, 20, 15);

  // Set text font to Times Bold, 14 pt.
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 14);

  // Write text at specified position
  pdfDoc_WriteTextAt(0, yPos, "PDF Creation Example");

  yPos += 25;

  // Set font to Times, 12 pt.
  pdfDoc_SetCurrentFont(buildInFonts.Times, 12);

  // Write text at specified position
  pdfDoc_WriteTextAt(10, yPos, "Created by 'SavePDFTemplate' module.");

  yPos += 50;

  // Adjust font size without changing the font type.
  pdfDoc_SetCurrentFontSize(10);

  // Write lorem ipsum text into specified area
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), 50, mlPDF::LOREM_IPSUM);

  // Set font to Courier Oblique, keep current font size.
  pdfDoc_SetCurrentFont(buildInFonts.CourierOblique);

  // Backup current y-axis reference (which is mlPDF::YAXIS_REFERENCE_TOP by default)
  pdfDoc_StoreYAxisReference();

  // Now set the y-axis reference to mlPDF::YAXIS_REFERENCE_BOTTOM, 
  // which will result in all y coordinates being interpreted with the lower page border as zero
  pdfDoc_SetYAxisReference(mlPDF::YAXIS_REFERENCE_BOTTOM); 

  // Write lorem ipsum text into specified area (note that the preset page margins shall be ignored)
  pdfDoc_WriteTextAreaAt(0, 0, pdfDoc_GetPageMaxWidth(mlPDF::IGNORE_MARGINS)/2, 25, "A text in the lower left corner of the page.\nPage margins are ignored!", mlPDF::IGNORE_MARGINS);

  // Now restore the previous y-axis reference
  // pdfDoc_RestoreDefaultYAxisReference() would have worked here as well.
  pdfDoc_RestoreYAxisReference();

  // Example code to show images can be added. It is disabled here since the image files must exists in order to work properly

  yPos += 60;
 
  // Load image into image resource pool (n.b.: only PNG anf JPG images are allowed!)
   mlPDF::IMAGE myImage = pdfDoc_LoadImageFromFile("D:/myImage.png");

   // Add image by direct reference
   pdfDoc_AddImage(0, yPos, 100, 100, myImage);

   // Add image once more through image pool
   pdfDoc_AddImage(110, yPos, 100, 100, pdfDocImages[pdfDocImages.size()-1]);




  // For more methods to add and manipulate contents, see MLPDF_PDFCreatorBase.h file.
}

//----------------------------------------------------------------------------------

ML_END_NAMESPACE