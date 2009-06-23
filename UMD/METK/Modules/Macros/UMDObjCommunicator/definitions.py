# ObjMgr database information

# Import the ObjMgr symbols
from ObjMgr import *


OBJ_GENERAL             = "General"
INF_MESSAGE             = "Message"
INF_PATH                = "Path"


LAY_DATA                = "Data"
LAY_DESCRIPTION         = "Description"
LAY_APPEARANCE          = "Appearance"
LAY_MEASUREMENTS        = "Measurements"
INF_STRUCTURE           = "Structure"
INF_FILENAME            = "Filename"
INF_INFO                = "Info"
INF_SIDE                = "Side"
INF_LEVEL               = "Level"

INF_COLOR               = "Color"
INF_TRANSPARENCY        = "Transparency"
INF_VISIBLE             = "Visible"
INF_BB		        = "BoundingBoxOn"


# Add here all messages for Image data

OBJ_IMAGE               = "Image"
OBJ_ROI                 = "ROI[0-99]+"
LAY_RESULT              = "Result[0-99]+"
LAY_MODALITY            = "Modality"
LAY_ROI                 = "ROI"
INF_LUTCENTER           = "LutCenter"
INF_LUTWIDTH            = "LutWidth"
INF_IMAGEDATE           = "Date"
INF_EQUIPMENT           = "Equipment"
INF_INSTITUTION         = "Institition"
INF_ROIX                = "ROI[0-99]+"

# ObjMgr symbols and messages for tumor segmentation results
OBJ_TUMORSEG            = "TumorSegmentation"
LAY_TUMORPREFIX         = "Tumor_"
INF_BARYCENTER          = "BaryCenter"
INF_MAX_DIAMETER        = "maxDiameter"
INF_MIDPOINT            = "MidPoint"
INF_PATH                = "Path"
INF_ROI_CENTER          = "RoiCenter"
INF_ROI_SIZE            = "RoiSize"
INF_VOLUME              = "Volume"
INF_X_AXIS              = "xAxis"
INF_X_DIAMETER          = "xDiameter"
INF_Y_AXIS              = "yAxis"
INF_Y_DIAMETER          = "yDiameter"
INF_Z_AXIS              = "zAxis"
INF_Z_DIAMETER          = "zDiameter"

OBJ_PLANCONFIG          = "PlanConfig"
OBJ_MONITORCONFIG       = "MonitorConfig"
OBJ_ASSESSCONFIG        = "AssessConfig"

LAY_CASE                = "Case"
INF_CASEDIR             = "Directory"

OBJ_THIS_IS_A_DUMMY     = ""
LAY_THIS_IS_A_DUMMY     = ""
INF_THIS_IS_A_DUMMY     = ""


# Add here all global messages (LAY_GLOBAL + INF_MESSAGE)

LAY_GLOBAL              = "Global"

MSG_LOAD                = "load"
MSG_CLEANUP             = "cleanup"

# Add here all messages for the Viewer module (LAY_VIEWER + INF_MESSAGE)

LAY_VIEW                = "View"






# VIEWER XYZ layers

INF_GLOBAL_REFRESH			= "Refresh"
LAY_VIEWER_CAMERA			= "Camera"
LAY_VIEWER_PROPERTIES			= "Properties"
INF_VIEWER_CAMERA_POSITION		= "Position"
INF_VIEWER_CAMERA_ORIENTATION		= "Orientation"
INF_VIEWER_CAMERA_HEIGHT		= "Height"
INF_VIEWER_CAMERA_NEWDATA		= "NewData"
INF_VIEWER_CAMERA_FOCALDISTANCE		= "FocalDistance"
INF_VIEWER_PROPERTIES_FRAMERATE		= "Framerate"
INF_VIEWER_PROPERTIES_CAPTUREFRAME	= "CaptureFrame"
INF_VIEWER_PROPERTIES_CREATEAVI		= "CreateAVI"
INF_VIEWER_PROPERTIES_CANCELRECORDING 	= "CancelRecording"
INF_VIEWER_PROPERTIES_UPDATEMLOUTPUT 	= "UpdateMLOutput"
INF_VIEWER_PROPERTIES_VIEWERSTATUS	= "ViewerStatus"
INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS = "ProtocolViewerStatus"
INF_VIEWER_PROPERTIES_BGCOLOR		= "BGColor"
INF_VIEWER_PROPERTIES_BGGREYCENTER	= "BGGreyCenter"
INF_VIEWER_PROPERTIES_BGGREYWIDTH	= "BGGreyWidth"


















class ObjInfoCom(ObjInfo):
   """Extends ObjInfo class with communication functions"""
   
   def sendMessage(self, layer, msg, arg= None):
     """Send 'message' and 'argument' to module specified by 'layer'
        (e.g. sendMessage(LAY_GLOBAL,MSG_LOAD) or sendMessage(LAY_VIEWER,MSG_VIEWMODE,""))"""

     self.typedSet(OBJ_COMMUNICATION,layer,INF_MESSAGE, msg, INFOTYPE_MESSAGE)
     if arg: self.set(INF_DATA, arg)
     self.notify()
     return
