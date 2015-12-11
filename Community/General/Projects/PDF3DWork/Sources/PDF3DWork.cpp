//----------------------------------------------------------------------------------
//! The ML module class PDF3DWork.
/*!
// \file    PDF3DWork.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-20
*/
//----------------------------------------------------------------------------------

// Local includes
#include "PDF3DWork.h"

// Project includes
#include <MLPDF_Tools.h>
#include <MLPDF_PDFDocumentTools.h>
#include <U3D_Constants.h>

// ThirdParty includes
#include <hpdf.h>
#include <hpdf_u3d.h>


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDF3DWork, Module);

//----------------------------------------------------------------------------------

PDF3DWork::PDF3DWork() : PDFGenerator()
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

void PDF3DWork::handleNotification(Field* field)
{
  // Call super class functionality first!
  PDFGenerator::handleNotification(field);

  // Handle changes of module parameters and input image fields here.
}

//----------------------------------------------------------------------------------

bool PDF3DWork::assemblePDFDocument()
{
  bool result = _assembleVersion3();

  return result;
}

//----------------------------------------------------------------------------------

bool PDF3DWork::_assembleVersion1()
{
  bool result = true;

  const std::string headerCitationText = "Neubert et.al. (2016). Automatic Segmentation of the Glenohumeral Cartilages from Magnetic Resonance Images.";
  const std::string headerHeadlineText = "Additional Figure 1 - Interactive 3D Figure";

  //const std::string caption = "CAPTION.";
  const std::string description = "3D visualization of the focused shape models of glenohumeral joint bone constructed from manual segmentations: the combined bone model (top); humerus (bottom left); scapula (bottom right). Each subfigure shows surfaces generated at ±3 standard deviations of the most important mode of variation. The surfaces of the models are colour coded with the manually assigned weights for constructing focused shape models in the relevant areas of the bones.";

  const std::string resourcesPath = resourcesPathFld->getStringValue();
  std::string u3dFilename = "";
  std::string posterFilename = "";
  std::string viewDefinition = "";

  //HPDF_Page pdfPage;
  float yPos = 0;
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
  pdfDoc_SetGlobalPageMarginsMM(20, 20, 20, 20);

  pdfDoc_SetCurrentFont(buildInFonts.Times, 11);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), 40, headerCitationText, mlPDF::TEXTALIGNMENT_LEFT);

  yPos += 50;
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 12);
  pdfDoc_WriteTextAt(0, yPos, headerHeadlineText);

  yPos += 35;
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAt(0, yPos, "Figure is best viewed with Adobe Reader 9 or later");

  pdfDoc_SetCurrentFont(buildInFonts.Times, 9);
  yPos += 15;
  pdfDoc_WriteTextAt(0, yPos, "Click inside image or frame to enable interactive mode.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Left-click & move mouse to rotate scene.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Right-click & move mouse to zoom.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Both-click and move mouse to pan.");

  // Add sub-figure #1
  u3dFilename = resourcesPath + "/version1/combined_combined_colorized.u3d";
  posterFilename = resourcesPath + "/version1/ombined_combined_poster.png";
  viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.000 0.779 -0.002</CamCenterOfOrbit><CamCenterToCamera>3.937 -4.304 -2.4894</CamCenterToCamera><CamRadiusOfOrbit>3.3424</CamRadiusOfOrbit><CamRollAngle>1.472</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";

  float sceneWidth = pdfDoc_GetPageMaxWidth();
  float sceneHeight = sceneWidth * 0.5; // Use fixed heigt ratio to facilitate poster image creation!

  if (_getMetaDataFromU3DFile(u3dFilename))
  {
    yPos += 40;
    _add3DFigure(0, yPos, sceneWidth, sceneHeight, true, u3dFilename, posterFilename, viewDefinition);
  }

  yPos += sceneHeight;

  // Add sub-figure #2
  u3dFilename = resourcesPath + "/version1/humerus_combined_colorized.u3d";
  posterFilename = resourcesPath + "/version1/humerus_combined_poster.png";
  viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>-0.299 0.522 -0.036</CamCenterOfOrbit><CamCenterToCamera>4.109 -0.112 -1.217</CamCenterToCamera><CamRadiusOfOrbit>4.287</CamRadiusOfOrbit><CamRollAngle>14.376</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
  sceneWidth = pdfDoc_GetPageMaxWidth() * 0.5;
  sceneHeight = sceneWidth;

  if (_getMetaDataFromU3DFile(u3dFilename))
  {
    _add3DFigure(0, yPos, sceneWidth, sceneHeight, true, u3dFilename, posterFilename, viewDefinition);
  }

  // Add sub-figure #3
  u3dFilename = resourcesPath + "/version1/scapula_combined_colorized.u3d";
  posterFilename = resourcesPath + "/version1/scapula_combined_poster.png";
  viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.215 0.797 0.310</CamCenterOfOrbit><CamCenterToCamera>3.940 -0.581 -0.752</CamCenterToCamera><CamRadiusOfOrbit>4.053</CamRadiusOfOrbit><CamRollAngle>6.585</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";

  if (_getMetaDataFromU3DFile(u3dFilename))
  {
    _add3DFigure(sceneWidth, yPos, sceneWidth, sceneHeight, true, u3dFilename, posterFilename, viewDefinition);
  }

  yPos += sceneHeight;

  //yPos += 5;
  //pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 10);
  //pdfDoc_WriteTextAt(0, yPos, caption);

  yPos += 12;
  float descriptionBoxHeight = pdfDoc_GetPageRemainingHeight(yPos);
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), descriptionBoxHeight, description, mlPDF::TEXTALIGNMENT_LEFT);

  return result;
}

//----------------------------------------------------------------------------------

bool PDF3DWork::_assembleVersion2()
{
  bool result = true;

  const std::string headerCitationText = "Neubert et.al. (2016). Automatic Segmentation of the Glenohumeral Cartilages from Magnetic Resonance Images.";
  const std::string headerHeadlineText = "Additional Figure 1 - Interactive 3D Figure";

  //const std::string caption = "CAPTION.";
  const std::string description = "3D visualization of the focused shape models of glenohumeral joint bone constructed from manual segmentations (the combined bone model). Each sub-model shows surfaces generated at ±3 standard deviations of the most important mode of variation. The surfaces of the models are colour coded with the manually assigned weights for constructing focused shape models in the relevant areas of the bones.";

  const std::string resourcesPath = resourcesPathFld->getStringValue();
  std::string u3dFilename = "";
  std::string posterFilename = "";
  std::string viewDefinition = "";

  //HPDF_Page pdfPage;
  float yPos = 0;
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
  pdfDoc_SetGlobalPageMarginsMM(20, 20, 20, 20);

  pdfDoc_SetCurrentFont(buildInFonts.Times, 11);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), 40, headerCitationText, mlPDF::TEXTALIGNMENT_LEFT);

  yPos += 50;
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 12);
  pdfDoc_WriteTextAt(0, yPos, headerHeadlineText);

  yPos += 35;
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAt(0, yPos, "Figure is best viewed with Adobe Reader 9 or later");

  pdfDoc_SetCurrentFont(buildInFonts.Times, 9);
  yPos += 15;
  pdfDoc_WriteTextAt(0, yPos, "Click inside image or frame to enable interactive mode.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Left-click & move mouse to rotate scene.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Right-click & move mouse to zoom.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Both-click and move mouse to pan.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Use the model tree on the left to select which sub-models are to be displayed.");

  // Add sub-figure #1
  u3dFilename = resourcesPath + "/version2/combined_combined_blend_colorized.u3d";
  posterFilename = resourcesPath + "/version2/combined_combined_blend_poster.png";
  viewDefinition = "<View><DisplayName>Default View 2</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.001 0.000 -0.002</CamCenterOfOrbit><CamCenterToCamera>2.072 -1.456 -0.991</CamCenterToCamera><CamRadiusOfOrbit>3</CamRadiusOfOrbit><CamRollAngle>17.8844</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";

  float sceneWidth = pdfDoc_GetPageMaxWidth();
  float sceneHeight = sceneWidth; 

  if (_getMetaDataFromU3DFile(u3dFilename))
  {
    yPos += 40;
    _add3DFigure(0, yPos, sceneWidth, sceneHeight, true, u3dFilename, posterFilename, viewDefinition);
  }

  yPos += sceneHeight;

  //yPos += 5;
  //pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 10);
  //pdfDoc_WriteTextAt(0, yPos, caption);

  yPos += 12;
  float descriptionBoxHeight = pdfDoc_GetPageRemainingHeight(yPos);
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), descriptionBoxHeight, description, mlPDF::TEXTALIGNMENT_LEFT);

  return result;
}

//----------------------------------------------------------------------------------

bool PDF3DWork::_assembleVersion3()
{
  bool result = true;

  const std::string headerCitationText = "Neubert et.al. (2016). Automatic Segmentation of the Glenohumeral Cartilages from Magnetic Resonance Images.";
  const std::string headerHeadlineText = "Additional Figure 1 - Interactive 3D Figure";

  //const std::string caption = "CAPTION.";
  const std::string description = "3D visualization of the focused shape models of glenohumeral joint bone constructed from manual segmentations: the combined bone model (top); humerus (bottom left); scapula (bottom right). Each subfigure shows surfaces generated at ±3 standard deviations of the most important mode of variation. The surfaces of the models are colour coded with the manually assigned weights for constructing focused shape models in the relevant areas of the bones.";

  const std::string resourcesPath = resourcesPathFld->getStringValue();
  std::string u3dFilename = "";
  std::string posterFilename = "";
  std::string viewDefinition = "";

  //HPDF_Page pdfPage;
  float yPos = 0;
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
  pdfDoc_SetGlobalPageMarginsMM(20, 20, 20, 20);

  pdfDoc_SetCurrentFont(buildInFonts.Times, 11);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), 40, headerCitationText, mlPDF::TEXTALIGNMENT_LEFT);

  yPos += 50;
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 12);
  pdfDoc_WriteTextAt(0, yPos, headerHeadlineText);

  yPos += 35;
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAt(0, yPos, "Figure is best viewed with Adobe Reader 9 or later");

  pdfDoc_SetCurrentFont(buildInFonts.Times, 9);
  yPos += 15;
  pdfDoc_WriteTextAt(0, yPos, "Click inside image or frame to enable interactive mode.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Left-click & move mouse to rotate scene.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Right-click & move mouse to zoom.");
  yPos += 10;
  pdfDoc_WriteTextAt(0, yPos, "- Both-click and move mouse to pan.");

  // Add sub-figure #1
  float pageWidth = pdfDoc_GetPageMaxWidth();
  float sceneWidth = pageWidth * 0.5;
  float sceneHeight = sceneWidth;

    yPos += 40;
    pdfDoc_AddOutlineRectangle(0, yPos, sceneWidth*2, sceneHeight, 1);

    u3dFilename = resourcesPath + "/version3/combined_minus3_colorized.u3d";
    posterFilename = resourcesPath + "/version3/combined_minus3_poster.png";
    viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.001 0.001 -0.002</CamCenterOfOrbit><CamCenterToCamera>0.622 -0.684 -0.381</CamCenterToCamera><CamRadiusOfOrbit>3</CamRadiusOfOrbit><CamRollAngle>10.8656</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
    _getMetaDataFromU3DFile(u3dFilename);
    _add3DFigure(0, yPos, sceneWidth, sceneHeight, false, u3dFilename, posterFilename, viewDefinition);

    u3dFilename = resourcesPath + "/version3/combined_plus3_colorized.u3d";
    posterFilename = resourcesPath + "/version3/combined_plus3_poster.png";
    //viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.000 0.779 -0.002</CamCenterOfOrbit><CamCenterToCamera>3.937 -4.304 -2.4894</CamCenterToCamera><CamRadiusOfOrbit>3.3424</CamRadiusOfOrbit><CamRollAngle>1.472</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
    _getMetaDataFromU3DFile(u3dFilename);
    _add3DFigure(sceneWidth, yPos, sceneWidth, sceneHeight, false, u3dFilename, posterFilename, viewDefinition);

  yPos += sceneHeight;

  // Add sub-figure #2
  sceneWidth = pageWidth * 0.25;
  sceneHeight = sceneWidth * 2;

    pdfDoc_AddOutlineRectangle(0, yPos, sceneWidth*2, sceneHeight, 1);

    u3dFilename = resourcesPath + "/version3/humerus_minus3_colorized.u3d";
    posterFilename = resourcesPath + "/version3/humerus_minus3_poster.png";
    viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>-0.313 -0.256 -0.196</CamCenterOfOrbit><CamCenterToCamera>0.631 -0.694 -0.346</CamCenterToCamera><CamRadiusOfOrbit>2</CamRadiusOfOrbit><CamRollAngle>10.9752</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
    _getMetaDataFromU3DFile(u3dFilename);
    _add3DFigure(0, yPos, sceneWidth, sceneHeight, false, u3dFilename, posterFilename, viewDefinition);

    u3dFilename = resourcesPath + "/version3/humerus_plus3_colorized.u3d";
    posterFilename = resourcesPath + "/version3/humerus_plus3_poster.png";
    //viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>-0.299 0.522 -0.036</CamCenterOfOrbit><CamCenterToCamera>4.109 -0.112 -1.217</CamCenterToCamera><CamRadiusOfOrbit>4.287</CamRadiusOfOrbit><CamRollAngle>14.376</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
    _getMetaDataFromU3DFile(u3dFilename);
    _add3DFigure(sceneWidth, yPos, sceneWidth, sceneHeight, false, u3dFilename, posterFilename, viewDefinition);

  // Add sub-figure #3
    pdfDoc_AddOutlineRectangle(sceneWidth*2, yPos, sceneWidth*2, sceneHeight, 1);

    u3dFilename = resourcesPath + "/version3/scapula_minus3_colorized.u3d";
    posterFilename = resourcesPath + "/version3/scapula_minus3_poster.png";
    viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.215 0.018 0.211</CamCenterOfOrbit><CamCenterToCamera>0.640 -0.711 -0.291</CamCenterToCamera><CamRadiusOfOrbit>2</CamRadiusOfOrbit><CamRollAngle>11.2132</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
    _getMetaDataFromU3DFile(u3dFilename);
    _add3DFigure(sceneWidth*2, yPos, sceneWidth, sceneHeight, false, u3dFilename, posterFilename, viewDefinition);

    u3dFilename = resourcesPath + "/version3/scapula_plus3_colorized.u3d";
    posterFilename = resourcesPath + "/version3/scapula_plus3_poster.png";
    //viewDefinition = "<View><DisplayName>Default View</DisplayName><BackgroundColor>1.000 1.000 1.000</BackgroundColor><LightingScheme>10</LightingScheme><RenderMode>1</RenderMode><CamCenterOfOrbit>0.215 0.797 0.310</CamCenterOfOrbit><CamCenterToCamera>3.940 -0.581 -0.752</CamCenterToCamera><CamRadiusOfOrbit>4.053</CamRadiusOfOrbit><CamRollAngle>6.585</CamRollAngle><CamFOVAngle>45</CamFOVAngle></View>";
    _getMetaDataFromU3DFile(u3dFilename);
    _add3DFigure(sceneWidth*3, yPos, sceneWidth, sceneHeight, false, u3dFilename, posterFilename, viewDefinition);

  yPos += sceneHeight;

  //yPos += 5;
  //pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 10);
  //pdfDoc_WriteTextAt(0, yPos, caption);

  yPos += 12;
  float descriptionBoxHeight = pdfDoc_GetPageRemainingHeight(yPos);
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), descriptionBoxHeight, description, mlPDF::TEXTALIGNMENT_LEFT);

  return result;
}

//----------------------------------------------------------------------------------

void PDF3DWork::_add3DFigure(float x, float y, float width, float height, bool addOutline, std::string u3dFilename, std::string posterFilename, std::string viewDefinition)
{
  bool sceneAdded = false;

  if (addOutline)
  {
    pdfDoc_AddOutlineRectangle(x, y, width, height, 1);
  }

  if (mlPDF::fileExists(u3dFilename))
  {
    std::string viewsSpecification = _getDefaultViewSpecificationFromU3DMetaData();

    if (viewDefinition != "")
    {
      viewsSpecification = viewDefinition;
    }

    HPDF_U3D u3dModel = pdfDoc_Load3DModelDataFromFile(u3dFilename);

    if (u3dModel)
    {

      pdfDoc_3DModel_AddAllViewsFromSpecificationString(u3dModel, viewsSpecification);

      // Load poster image
      bool posterExists = false;
      mlPDF::IMAGE posterImage = NULL;

      if (mlPDF::fileExists(posterFilename))
      {
        posterImage = pdfDoc_LoadImageFromFile(posterFilename);

        if (posterImage)
        {
          posterExists = true;
        }
      }

      if (!posterExists)
      {
        pdfDoc_WriteTextAreaAt(x, y + height / 2.0, width, 50, "Click here to display interactive 3D figure.", mlPDF::TEXTALIGNMENT_CENTER);
      }

      mlPDF::SCENE3D u3dScene = pdfDoc_Add3DScene(x + 1, y + 1, width - 2, height - 2, u3dModel, posterImage);

      if (u3dScene)
      {
        pdfDoc_3DScene_SetActivationProperties(u3dScene, mlPDF::ActivationModeStrings[mlPDF::ACTIVATION_MODE_EXPLICIT_ACTIVATE], mlPDF::DeactivationModeStrings[mlPDF::DEACTIVATION_MODE_EXPLICIT_DEACTIVATE], true);
        sceneAdded = true;
      }

    } // if (u3dModel)

  } // if (mlPDF::fileExists(u3dFilename))

  if (!sceneAdded)
  {
    pdfDoc_WriteTextAreaAt(x, y + height / 2.0, width, 50, "FAILED TO IMPORT 3D FIGURE!", mlPDF::TEXTALIGNMENT_CENTER);
  }
}

//----------------------------------------------------------------------------------

bool PDF3DWork::_getMetaDataFromU3DFile(std::string u3dFilename)
{
  bool result = false;

  _u3dMetaData.clear();

  if (mlPDF::fileExists(u3dFilename))
  {
    std::ifstream u3dFile;
    u3dFile.open(u3dFilename, std::ios::out | std::ios::binary);

    if (u3dFile.is_open())
    {
      U3DDataBlockFundamental blockType = _readU32(u3dFile);

      if (blockType == U3D_BLOCKTYPE_FILEHEADER)
      {
        // Get data block size
        U3DDataBlockFundamental blockDataSize = _readU32(u3dFile);

        // Get meta data block size
        /*U3DDataBlockFundamental blockMetaDataSize = */_readU32(u3dFile);

        // Calc meta data start position
        MLuint32 dataStartPos = (MLuint32)u3dFile.tellg();
        MLuint32 metaDataStartPos = dataStartPos + blockDataSize;

        if (blockDataSize % 4 != 0)
        {
          metaDataStartPos += (4 - (blockDataSize % 4));
        }

        // Go to meta data start position
        u3dFile.seekg(metaDataStartPos, std::ios::beg);

        // Get Key/Value Pair Count (9.2.6.1)
        U3DDataBlockFundamental keyValuePairCount = _readU32(u3dFile);

        for (int thisKeyValuePair = 0; thisKeyValuePair < (int)keyValuePairCount; thisKeyValuePair++)
        {
          // Get Key/Value Pair Attributes (9.2.6.2)
          /*U3DDataBlockFundamental keyValuePairAttributes = */_readU32(u3dFile);

          std::string keyString = _readString(u3dFile);
          std::string valueString = _readString(u3dFile);

          MetaDataStruct newMetaDataSet;
          newMetaDataSet.key = keyString;
          newMetaDataSet.value = valueString;

          _u3dMetaData.push_back(newMetaDataSet);
        }

        result = true;

      } // if (blockType == U3D_BLOCKTYPE_FILEHEADER)

      u3dFile.close();

    } // if (u3dFile.is_open())

  } // if (mlPDF::fileExists(u3dFilename))

  return result;
}

//----------------------------------------------------------------------------------

std::string PDF3DWork::_getDefaultViewSpecificationFromU3DMetaData()
{
  std::string result = "";
  std::string boundingBoxCenterString = "";
  std::string boundingBoxRadiusString = "";

  const int keyValuePairCount = (int)_u3dMetaData.size();

  for (int thisKeyValuePairIndex = 0; thisKeyValuePairIndex < keyValuePairCount; thisKeyValuePairIndex++)
  {
    MetaDataStruct thisKeyValuePair = _u3dMetaData[thisKeyValuePairIndex];

    if (thisKeyValuePair.key == "BoundingBoxCenter")
    {
      boundingBoxCenterString = thisKeyValuePair.value;
    }

    if (thisKeyValuePair.key == "BoundingBoxRadius")
    {
      boundingBoxRadiusString = thisKeyValuePair.value;
    }

  }

  if ((boundingBoxCenterString != "") && (boundingBoxRadiusString != ""))
  {
    Vector3 boundingBoxCenter = mlPDF::PDFTools::getVec3FromString(boundingBoxCenterString);
    double  boundingBoxRadius = mlPDF::stringToDouble(boundingBoxRadiusString);

    Vector3 camCenterOfOrbit;
    Vector3 camCenterToCamera;
    float   camRadiusOfOrbit;
    float   camRollAngle;
    float   camFOVAngle;

    mlPDF::PDFDocumentTools::CalculateDefaultCameraProperties(boundingBoxCenter, boundingBoxRadius, camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);
    //camRadiusOfOrbit *= 0.75;

    result += "<View>";
    result += "<DisplayName>Default View</DisplayName>";
    result += "<BackgroundColor>1.000 1.000 1.000</BackgroundColor>";
    result += "<LightingScheme>10</LightingScheme>";
    result += "<RenderMode>1</RenderMode>";
    result += "<CamCenterOfOrbit>" + mlPDF::PDFTools::FormatVec3String(camCenterOfOrbit) + "</CamCenterOfOrbit>\n";
    result += "<CamCenterToCamera>" + mlPDF::PDFTools::FormatVec3String(camCenterToCamera) + "</CamCenterToCamera>\n";
    result += "<CamRadiusOfOrbit>" + mlPDF::floatToString(camRadiusOfOrbit) + "</CamRadiusOfOrbit>\n";
    result += "<CamRollAngle>" + mlPDF::floatToString(camRollAngle) + "</CamRollAngle>\n";
    result += "<CamFOVAngle>" + mlPDF::floatToString(camFOVAngle) + "</CamFOVAngle>\n";
    result += "</View>";
  }

  return result;
}

//----------------------------------------------------------------------------------

MLuint8 PDF3DWork::_readU8(std::ifstream& file)
{
  char* u8Buf = new char[1];

  file.read(u8Buf, 1);

  MLuint8 result = u8Buf[0];

  delete[] u8Buf;

  return result;
}

//----------------------------------------------------------------------------------

MLuint16 PDF3DWork::_readU16(std::ifstream& file)
{
  char* u16Buf = new char[2];

  file.read(u16Buf, 2);

  MLuint16 byte1 = u16Buf[1] << 8;
  MLuint8  byte0 = u16Buf[0];

  MLuint16 result = byte1 | byte0;

  delete[] u16Buf;

  return result;
}

//----------------------------------------------------------------------------------

MLuint32 PDF3DWork::_readU32(std::ifstream& file)
{
  char* u32Buf = new char[4];

  file.read(u32Buf, 4);

  MLuint32 byte3 = u32Buf[3] << 24;
  MLuint32 byte2 = u32Buf[2] << 16;
  MLuint32 byte1 = u32Buf[1] << 8;
  MLuint8  byte0 = u32Buf[0];

  MLuint32 result = byte3 | byte2 | byte1 | byte0;

  delete[] u32Buf;

  return result;
}

//----------------------------------------------------------------------------------

std::string PDF3DWork::_readString(std::ifstream& file)
{
  std::string result = "";

  MLuint16 stringLength = _readU16(file);

  for (int i = 0; i < stringLength; i++)
  {
    MLuint8 stringCharacter = _readU8(file);

    result += stringCharacter;
  }

  return result;

}//----------------------------------------------------------------------------------

ML_END_NAMESPACE