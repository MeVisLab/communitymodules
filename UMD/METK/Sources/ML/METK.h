

//! Global definitions for METK
//! \file METK.h

//--------------------------------------------------------------------------------
//MÜSSTE DRINGEND MAL SORTIERT, NACH KONVENTIONEN BENANNT UND AUFGERÄUMT WERDEN!!!
//--------------------------------------------------------------------------------


#ifndef __mlMETK_H
#define __mlMETK_H


// fixed object IDs

#define LAY_GLOBAL        "Global"
#define INF_ID        "Id"
#define INF_OBJTYPE       "ObjectType"
#define INF_CHILDS        "ChildIds"
#define INF_PARENT        "ParentId"
#define INF_VALID         "Valid"

#define OBJ_APPLICATION   "ApplicationObject"
#define OBJ_PATIENT       "PatientObject"
#define OBJ_CASE          "CaseObject"
#define OBJ_EVENTS        "Events"
#define OBJ_COLLECTIONS   "Collections"
#define OBJ_CSO           "CSO"
#define OBJ_COMMUNICATION "Communication"


// message exchange

#define LAY_STATUSEVENTS "StatusEvents"
#define INF_MESSAGE      "Message"
#define INF_DATA         "Data"
#define INF_STATUS       "Status"
// status messages
// these messages are reserved for METKMsgSender and METKMsgReceiver
#define MSG_INIT       "initialize"
#define MSG_PROCESSING "processing"
#define MSG_READY      "ready"
#define MSG_DONE       "done"

#define LAY_GLOBALEVENTS "GlobalEvents"
#define INF_CASELOADED   "CaseLoaded"
// global messages
#define MSG_LOADED  "Loaded"            // is sent from METKManager
#define MSG_CLEANUP "Cleanup"           // sent for telling all modules to cleanup their stuff



#define LAY_APPEARANCE   "Appearance"
#define INF_COLOR        "Color"
#define INF_TRANSPARENCY "Transparency"    // 0..1
#define INF_BB           "BoundingBoxOn"   // bool
#define INF_VISIBLE      "Visible"         // bool
#define INF_PICKSTYLE    "PickStyle"       // SHAPE, BOUNDING_BOX, UNPICKABLE
#define INF_SURFACEDIST  "SurfaceDistance" // bool
#define INF_SURFACE      "Surface"         // bool
#define INF_SMOOTHVES    "SmoothVessels"   // bool

#define LAY_DESCRIPTION  "Description"
#define INF_NAME         "Name"             // String
#define INF_STRUCTURE    "Structure"        // String
#define INF_STRUCTUREGROUP "StructureGroup" // String
#define INF_COMMENT      "Comment"          // String
#define INF_SIDE         "Side"             // String  Left, Right
#define INF_IMPORTANCE   "Importance"       // Double  0..1
#define INF_RESULTTYPE   "ResultType"       // String

#define LAY_FILES        "Files"
#define INF_IVFILE       "InventorFile"
#define INF_IVPOINTER    "InventorPointer"

#define OBJ_CASE         "CaseObject"
#define LAY_CASE         "Case"
#define INF_CASEDIR      "Directory"


#define LAY_MEASURES     "Measures"
#define INF_BARYCENTER   "BaryCenter"
#define INF_MIDPOINT     "MidPoint"
#define INF_X_AXIS       "xAxis"
#define INF_Y_AXIS       "yAxis"
#define INF_Z_AXIS       "zAxis"
#define INF_X_DIAMETER   "xDiameter"
#define INF_Y_DIAMETER   "yDiameter"
#define INF_Z_DIAMETER   "zDiameter"
#define INF_MAX_DIAMETER "maxDiameter"
#define INF_BBMIN        "BoundingBox_min"          // Vec3f
#define INF_BBMAX        "BoundingBox_max"          // Vec3f
#define INF_BBCENTER     "boxCenter"                // Vec3f
#define INF_WORLDBB      "MidPoint"                 // String (2x Vec3: min, max)
#define INF_OBJCENTER    "objectCenter"             // Vec3f

#define LAY_LABEL    "Labels"
//#define INF_LABELTYPE  "LabelType"
#define INF_LABELON    "LabelOn"          // Bool
#define INF_LABELTEXT    "LabelText"        // String
#define INF_LABELBG    "LabelBG"          // SbVec3f


// everything for LAY_IMAGE
#define LAY_IMAGE        "Image"
#define INF_FILENAME     "Filename"                 // String
#define INF_IMAGETYPE    "ImageType"                // String Segmentation (segmented Structure), Original (Original Image, ROI)
#define INF_OBJVALUE     "ObjectValue"              // Int32
#define INF_WMATRIX      "ToWorldMatrix"            // Mat4
#define INF_VOXELSIZEX   "VoxelSizeX"               // Double
#define INF_VOXELSIZEY   "VoxelSizeY"               // Double
#define INF_VOXELSIZEZ   "VoxelSizeZ"               // Double
#define INF_SIZEX        "SizeX"                    // Int32
#define INF_SIZEY        "SizeY"                    // Int32
#define INF_SIZEZ        "SizeZ"                    // Int32
#define INF_CODSEGOBJECTVALUE  "CodedSegObjectValue" //String

// everything for OBJ_CSO
#define LAY_SELECTEDOBJ  "SelectedObject"
#define INF_OBJID        "ObjectID"
#define INF_SELECTION    "Selection"                // Vec3f   Selection point in world coordinates
#define INF_IGNOREPICK   "IgnorePickable"           // Bool    set to ignore LAY_APPEARANCE / INF_PICKSTYLE



#define INF_IVFULLPOINTER    "InventorFullPointer"



// VolumeRendering
#define LAY_PROPERTIES  "Properties"
#define LAY_TRANSFERFUNCTION  "Transferfunction"
#define INF_COLORPOINTS "ColorPoints"
#define INF_ALPHAPOINTS "AlphaPoints"








//FROM kObjMgrAttr.h

//  Objects (O_: Object)
#define O_VESSELTREE      "VesselTree"

//  Layers of information (L_: layer)
#define L_GLOBAL        "Global"
#define L_VESSELTREE1     "VesselTree1"
#define L_VESSELTREE2     "VesselTree2"
#define L_TRANSFORMATION    "Transformation"

//  Specific information in layer (I_: info)

// L_GLOBAL
#define I_ID          "Id"

#define LAY_GLOBAL_REFRESH            "Refresh"
#define LAY_VIEWER_CAMERA           "Camera"
#define LAY_VIEWER_PROPERTIES         "Properties"
#define INF_VIEWER_CAMERA_POSITION        "Position"
#define INF_VIEWER_CAMERA_ORIENTATION     "Orientation"
#define INF_VIEWER_CAMERA_HEIGHT        "Height"
#define INF_VIEWER_CAMERA_NEWDATA       "NewData"
#define INF_VIEWER_CAMERA_FOCALDISTANCE     "FocalDistance"
#define INF_VIEWER_PROPERTIES_FRAMERATE     "Framerate"
#define INF_VIEWER_PROPERTIES_CAPTUREFRAME    "CaptureFrame"
#define INF_VIEWER_PROPERTIES_CREATEAVI     "CreateAVI"
#define INF_VIEWER_PROPERTIES_CANCELRECORDING "CancelRecording"
#define INF_VIEWER_PROPERTIES_UPDATEMLOUTPUT  "UpdateMLOutput"
#define INF_VIEWER_PROPERTIES_VIEWERSTATUS    "ViewerStatus"
#define INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS "ProtocolViewerStatus"
#define INF_VIEWER_PROPERTIES_BGCOLOR     "BGColor"
#define INF_VIEWER_PROPERTIES_BGGREYCENTER    "BGGreyCenter"
#define INF_VIEWER_PROPERTIES_BGGREYWIDTH   "BGGreyWidth"
#define LAY_VIEWER_SCREENSHOT         "Screenshot"
#define INF_VIEWER_SCREENSHOT_FILE        "ScreenshotFilename"
#define INF_VIEWER_SCREENSHOT_FLAG        "ScreenshotFlag" //generate thumbnail from screenshot
#define INF_VIEWER_SCREENSHOT_THUMB       "ScreenshotThumb"  //True = make screenshot False = Done


#define O_ANIMATION       "Animation"
#define LAY_ANIMATION_GLOBAL  "Global"
#define INF_ANIMATION_RESET   "Reset"
#define LAY_ANIMATION_SCRIPT  "Script"
#define INF_SCRIPT_FULLSCRIPT "FullScript"
#define INF_SCRIPT_SCRIPTFILE "ScriptFile"
#define INF_SCRIPT_OBJNAMESPATH "ObjNamesPath"
#define INF_SCRIPT_STATUS   "Status"

#define O_CASEOBJECT      "CaseObject"
#define LAY_CASEOBJECT_CASE   "Case"
#define INF_CASEOBJECT_CASE_DIRECTORY     "Directory"


#define O_CLIPPING        "Clipping"
#define INF_CLIPPING_SCALE    "Scale"
#define LAY_CLIPPINGLEFT    "PlaneLeft"
#define LAY_CLIPPINGRIGHT   "PlaneRight"
#define LAY_CLIPPINGTOP     "PlaneTop"
#define LAY_CLIPPINGBOTTOM    "PlaneBottom"
#define LAY_CLIPPINGREAR    "PlaneRear"
#define LAY_CLIPPINGFRONT   "PlaneFront"
#define INF_CLIPPING_TRANSLATION "Translation"
#define INF_CLIPPING_ROTATION "Rotation"
#define INF_CLIPPING_ON     "ClippingOn"




//For each object
#define INF_CLIPPING      "Clipping"
#define INF_SILHOUETTECOLOR   "SilhouetteColor"
#define INF_SILHOUETTEWIDTH   "SilhouetteWidth"
#define INF_SILHOUETTE      "Silhouette"


#define I_ORIENTATION     "Orientation"
#define I_POSITION        "Position"
#define I_CAMHEIGHT       "CamHeight"
#define I_CAPTUREFRAME      "CaptureFrame"
#define I_CREATEAVI       "CreateAVISwitch"
#define I_CANCELRECORDING   "CancelRecordingSwitch"
#define I_UPDATEMLOUTPUT    "UpdateMLOutput"

#define I_STATUS        "Status"
#define I_ENABLERENDERING   "EnableRendering"

#define I_BACKGROUNDCOLOR   "BackgroundColor"
#define I_BACKGROUNDGREYCENTER  "BackgroundGreyCenter"
#define I_BACKGROUNDGREYWIDTH "BackgroundGreyWidth"

#define I_FRAMERATE       "Framerate"

#define I_REFRESH       "Refresh"

#define I_SHIFTX        "ShiftX"
#define I_SHIFTY        "ShiftY"
#define I_SHIFTZ        "ShiftZ"


//L_VESSELSTREE#
#define I_VESSELTREECOLOR   "Color"

#define INF_MAXDIAMETER     "maxDiameter"
#define INF_XDIAMETER     "xDiameter"
#define INF_YDIAMETER     "yDiameter"
#define INF_ZDIAMETER     "zDiameter"
#define INF_MIDPOINT      "MidPoint"
#define INF_OBJECTCENTER    "objectCenter"
#define INF_XAXIS       "xAxis"
#define INF_YAXIS       "yAxis"
#define INF_ZAXIS       "zAxis"


//SELECTION
#define O_CSO         "CSO"
#define LAY_SELECTEDOBJECT    "SelectedObject"
#define INF_OBJECTID      "ObjectID"


//DESCRIPTION
#define LAY_DESCRIPTION     "Description"
#define INF_NAME        "Name"
#define INF_STRUCTURE           "Structure"
#define INF_STRUCTUREGROUP      "StructureGroup"
#define INF_COMMENT             "Comment"
#define INF_SIDE                "Side"
#define INF_RESULTTYPE      "ResultType"
#define INF_IMPORTANCE          "Importance"

#endif // __mlMETK_H




