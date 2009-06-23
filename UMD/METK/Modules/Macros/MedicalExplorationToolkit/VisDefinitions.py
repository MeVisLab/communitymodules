# ObjMgr database information

# Import the ObjMgr symbols
from ObjMgr import *


# fixed object IDs

OBJ_APPLICATION         = "ApplicationObject"
OBJ_PATIENT             = "PatientObject"
#OBJ_LABEL               = "LabelObject"
OBJ_CASE                = "CaseObject"
OBJ_EVENTS              = "Events"
OBJ_COLLECTIONS         = "Collections"
OBJ_CSO                 = "CSO"
OBJ_COMMUNICATION       = "Communication"            # must be nonpersistent!!!


###################################################################################################################

# all layers and infos for fixed objects

# message exchange

INF_MESSAGE             = "Message"                  # Message
INF_DATA                = "Data"                     # String
INF_STATUS              = "Status"                   # Message

LAY_GLOBALEVENTS	= "GlobalEvents"
INF_CASELOADED		= "CaseLoaded"               # Message
INF_GETALLVIEWER	= "GetAllViewer"

LAY_STATUS              = "Status"
INF_STATUSMSG		= "StatusMessage"            # Message
INF_PROGRESS            = "Progress"                 # Double 0..1

# global messages
MSG_CONVERTED           = "converted"
MSG_LOADED		= "Loaded"                   # is sent from METKManager
MSG_TOTALLOADED		= "TotalLoaded"              # is sent from a METKScriptInit after inital animation is ready
MSG_SAVE                = "Save"                     # Send when the temporary data should be saved
MSG_CLEANUP             = "Cleanup"                  # sent for telling all modules to cleanup their stuff
MSG_GET			= "Get"		             # triggers all viewer to write their inital data into the objMgr

LAY_ROIS                = "ROIS"                   #to communicate selected ROIs

# status messages
LAY_STATUSEVENTS	= "StatusEvents"

# these messages are reserved for METKMsgSender and METKMsgReceiver
MSG_INIT                = "initialize"
MSG_PROCESSING          = "processing"
MSG_DONE                = "done"



# layer and info IDs for OBJ_APPLICATION

LAY_APPLICATION         = "Application"
INF_APPLICATIONNAME     = "ApplicationName"
INF_APPLICATIONVERSION  = "ApplicationVersion"
INF_MEVISLAB            = "MeVisLab"


# layer and info IDs for OBJ_PATIENT

LAY_PATIENT             = "Patient"
INF_DOB                 = "DOB"                      # String   Day of Birth YYYYMMDD
INF_NAME                = "Name"                     # String
INF_PID                 = "PID"                      # String
INF_SEX                 = "Sex"                      # String
INF_WEIGHT              = "Weight"                   # String


# layer and info IDs for OBJ_LABEL
#VERALTET!!! FÜR 3DLABELING?
#LAY_LABELSTYLE          = "LabelStyle"
#INF_MULTIREF            = "MultiRef"                 # bool
#INF_MULTIREFFORK        = "MultiRefFork"             # bool
#INF_FLUSHLAYOUT         = "Flush"                    # bool
#INF_ORTHOGONALLINES     = "OrthogonalLines"          # bool
#INF_LINEINSTEADARROW    = "LineInsteadArrow"         # bool
#INF_HALOCOLORINVERT     = "HaloColorInvert"          # bool
#INF_LINEPATTERN         = "LinePattern"              # Int32
#INF_LINEPATTERN_IO      = "LinePattern_io"           # Int32


#LAY_LABELPARAMETER      = "Parameter"
#INF_ORBIT               = "Orbit"                    # Int32
#INF_BUFFERSKIP          = "BufferSkip"               # Int32
#INF_CLUSTERDISTANCE     = "ClusterDistance"          # Int32
#INF_CLUSTERTOLERANCE    = "ClusterTolerance"         # Int32
#INF_CLUSTERSPLITMEDIAL  = "ClusterSplitMedial"       # bool
#INF_CLUSTERSPLITOBJ     = "ClusterSplitObj"          # bool
#INF_CLUSTERYDISTANCE    = "ClusterYDistance"         # bool
#INF_CLUSTERCOHERENCE    = "ClusterCoherence"         # bool
#INF_CLUSTERELEMENTS     = "ClusterElements"          # Int32
#INF_WEIGHTANGLE         = "Weight_Angle"             # Double
#INF_WEIGHTDELTA         = "Weight_Delta"             # Double
#INF_WEIGHTLENGTH        = "Weight_Length"            # Double
#INF_WEIGHTNEIGHBOR      = "Weight_Neighbor"          # Double
#INF_WEIGHTINTERSECTION  = "Weight_Intersection"      # Double
#weights for invisible objects
#INF_WEIGHTANGLE_IO      = "Weight_Angle_io"          # Double
#INF_WEIGHTDELTA_IO      = "Weight_Delta_io"          # Double
#INF_WEIGHTLENGTH_IO     = "Weight_Length_io"         # Double
#INF_WEIGHTNEIGHBOR_IO   = "Weight_Neighbor_io"       # Double
#INF_WEIGHTINTERSEC_IO   = "Weight_Intersec_io"       # Double
#weights for n-1 Labeling (n objects -> 1 Labe)
#INF_WEIGHTANGLE_N1      = "Weight_Angle_n1"          # Int32
#INF_WEIGHTDELTA_N1      = "Weight_Delta_n1"          # Int32
#INF_WEIGHTLENGTH_N1     = "Weight_Length_n1"         # Int32
#INF_WEIGHTNEIGHBOR_N1   = "Weight_Neighbor_n1"       # Int32

# layer and info IDs for OBJ_CASE

LAY_CASE                = "Case"
INF_COMMENT             = "Comment"                  # String
INF_FINDING             = "Finding"                  # String
INF_ORDER               = "Order"                    # String
INF_CASEDIR             = "Directory"                # String  must be nonpersistent!!!
INF_XMLFILE             = "XMLFile"                  # String  must be nonpersistent!!!


OBJ_ANIMATION		= "Animation"
LAY_ANIMATION_GLOBAL	= "Global"
INF_ANIMATION_RESET	= "Reset"
LAY_ANIMATION_SCRIPT	= "Script"
INF_SCRIPT_FULLSCRIPT	= "FullScript"
INF_SCRIPT_STATUS	= "Status"
INF_SCRIPT_SCRIPTFILE	= "ScriptFile"
INF_SCRIPT_OBJNAMESPATH	= "ObjNamesPath"


OBJ_BOOKMARKS		= "Bookmarks"
INF_COUNT3D		= "Count3D"
INF_CAMPOSITION		= "CamPosition"
INF_CAMORIENTATION	= "CamOrientation"
INF_CAMHEIGHT		= "CamHeight"
INF_CAMFOCALDISTANCE	= "CamFocalDistance"



OBJ_CLIPPING		= "Clipping"
INF_CLIPPING_SCALE	= "Scale"
LAY_CLIPPING_LEFT	= "PlaneLeft"
LAY_CLIPPING_RIGHT	= "PlaneRight"
LAY_CLIPPING_TOP	= "PlaneTop"
LAY_CLIPPING_BOTTOM	= "PlaneBottom"
LAY_CLIPPING_REAR	= "PlaneRear"
LAY_CLIPPING_FRONT	= "PlaneFront"
INF_CLIPPING_TRANSLATION = "Translation"
INF_CLIPPING_ROTATION	= "Rotation"
INF_CLIPPING_ON		= "ClippingOn"


OBJ_CODEDSEGMENTATION   = "CodedSegmentation"
INF_VERSION		= "Version"


# everything for OBJ_COLLECTIONS
# layer IDs are dynamic
# collection filename as in LAY_IMAGE
# comment as in LAY_CASE
INF_THUMBNAME           = "ThumbFilename"
INF_SCREENSHOT          = "Screenshot"
INF_TITLE               = "Title"


# everything for OBJ_CSO
LAY_SELECTEDOBJ         = "SelectedObject"
LAY_MOUSEOVEROBJ        = "MouseOverObject"
INF_OBJID               = "ObjectID"
INF_MULTIOBJID          = "MuliObjectID"
INF_SELECTION           = "Selection"                # Vec3f   Selection point in world coordinates
INF_IGNOREPICK          = "IgnorePickable"           # Bool    stored in LAY_GLOBAL. Set to ignore LAY_APPEARANCE / INF_PICKSTYLE

######################################################################################################################

# overall layer and info IDs for dynamic objects


LAY_DESCRIPTION         = "Description"
LAY_MEASURES            = "Measures"
LAY_APPEARANCE          = "Appearance"
LAY_IMAGE               = "Image"
LAY_FILES               = "Files"

#everything for LAY_LABEL
LAY_LABEL               = "Labels"
INF_DRAWLABEL           = "LabelOn"                 #bool
INF_OBJECTISBACKGROUND  = "ObjectIsBackground"      #bool
INF_LABELPRIORITY       = "LabelPriority"           #int32
INF_LABELTEXT           = "LabelText"               #string
INF_LABELBG             = "LabelBG"                 #vec3f
INF_FONTSIZE            = "FontSize"                #int32
INF_LABELIFINVIS        = "LabelIfInvis"            #bool
INF_LABELIFINTERACTION  = "LabelIfInteraction"      #bool
INF_LABELTEXTCOLOR      = "LabelTextColor"          #vec3f
INF_REFLINECOLOR        = "RefLineColor"            #vec3f
INF_ARROWCOLOR          = "ArrowColor"              #vec3f
INF_ARROWSIZE           = "ArrowSize"               #int32


# everything for LAY_GLOBAL
INF_OBJTYPE             = "ObjectType"
INF_CHILDS              = "ChildIds"                 # String  Name der untergeordneten Objekte
INF_PARENT              = "ParentId"                 # String  Name des übergeordneten Objekts
INF_VALID               = "Valid"                    # Bool

# everything for LAY_DESCRIPTION
INF_NAME                = "Name"                     # String
INF_STRUCTURE           = "Structure"                # String
INF_STRUCTUREGROUP      = "StructureGroup"           # String
INF_COMMENT             = "Comment"                  # String
INF_SIDE                = "Side"                     # String  Left, Right
INF_RESULTTYPE		= "ResultType"               # String
INF_IMPORTANCE          = "Importance"               # Double  0..1

# everything for LAY_MEASURES
INF_BARYCENTER          = "BaryCenter"               # Vec3f
INF_MAX_DIAMETER        = "maxDiameter"              # Double
INF_EXACT               = "ExactMeasures"            # Bool
INF_MIDPOINT            = "MidPoint"                 # Vec3f
INF_X_AXIS              = "xAxis"                    # Vec3f
INF_X_DIAMETER          = "xDiameter"                # Double
INF_Y_AXIS              = "yAxis"                    # Vec3f
INF_Y_DIAMETER          = "yDiameter"                # Double
INF_Z_AXIS              = "zAxis"                    # Vec3f
INF_Z_DIAMETER          = "zDiameter"                # Double
INF_VOLUME              = "Volume"                   # Double
INF_BOUNDVOL            = "BoundaryVoxelVolume"      # Double
INF_VOLUMEDIGITS        = "VolumeDigits"             # Int32
INF_VOLUMEUNIT          = "VolumeUnit"               # String (ml)
INF_BBMAX               = "BoundingBix_max"          # Vec3f
INF_BBMIN               = "BoundingBix_min"          # Vec3f
INF_OBJCENTER           = "objectCenter"             # Vec3f
INF_BBCENTER            = "BoxCenter"                # Vec3f
INF_WORLDBB             = "MidPoint"                 # String (2x Vec3: min, max)


# everything for LAY_APPEARANCE
INF_COLOR               = "Color"                    # Vec3f   means diffuse color
INF_USEGROUPCOLOR       = "UseGroupColor"            # Bool
INF_DRAWSTYLE           = "DrawStyle"                # String  FILLED, LINES, POINTS, INVISIBLE
INF_PICKSTYLE           = "PickStyle"                # String  SHAPE, BOUNDING_BOX, UNPICKABLE
INF_TRANSPARENCY        = "Transparency"             # Double  0..1
INF_SILHOUETTE		= "Silhouette"               # Bool
INF_SILHOUETTECOLOR	= "SilhouetteColor"          # Vec3f
INF_SILHOUETTEWIDTH	= "SilhouetteWidth"          # Double
INF_STIPPLING		= "Stippling"                # Bool
INF_STIPPLINGGAMMA	= "StipplingGamma"           # Double  > 0
INF_STIPPLINGCOLOR	= "StipplingColor"           # Vec3f
INF_VISIBLE             = "Visible"                  # Bool
INF_ISOSURFACE          = "IsoSurface"               # Bool, Visibility of IsoSurface, default=true
INF_FLATISO		= "FlatIso"		     # Bool
INF_BB		        = "BoundingBoxOn"
INF_RENDERMODE          = "RenderMode"
INF_CLIPPING		= "Clipping"
INF_SURFACEDIST         = "SurfaceDistance"          # Bool
INF_TEXTURIZE           = "Texturize"                # Bool   Enable Texturing
INF_TEXTURETYPE         = "TextureType"              # String HATCHING_TAM, HATCHING_OPACITY
INF_TEXTURESCALE        = "TextureScale"             # Double
INF_TRACECOLORCODE      = "TraceColorCode"

# everything for LAY_IMAGE
INF_FILENAME            = "Filename"                 # String
INF_IMAGETYPE           = "ImageType"                # String Segmentation (segmented Structure), Original (Original Image, ROI)
INF_OBJVALUE            = "ObjectValue"              # Int32
INF_WMATRIX             = "ToWorldMatrix"            # Mat4
INF_MODALITY            = "Modality"                 # String
INF_PROTOCOL            = "Protocol"                 # String
INF_VOXELSIZEX          = "VoxelSizeX"               # Double
INF_VOXELSIZEY          = "VoxelSizeY"               # Double
INF_VOXELSIZEZ          = "VoxelSizeZ"               # Double
INF_SIZEX               = "SizeX"                    # Int32
INF_SIZEY               = "SizeY"                    # Int32
INF_SIZEZ               = "SizeZ"                    # Int32
INF_STUDYDATE           = "StudyDate"                # String
INF_STUDYTIME           = "StudyTime"                # String
INF_INSTITUTION         = "Institution"              # String
INF_MANUFACTURER        = "Manufacturer"             # String
INF_MMODELNAME          = "ManufacturersModelName"   # String
INF_BPEX                = "BodyPartExamined"
INF_CODSEGIMAGE         = "CodedSegFilename"         # is always a CodedSegmentation
INF_CODSEGVALUE         = "CodedSegObjectValue"      # String e.g. "1,23,45"
INF_STARTVOXEL          = "StartVoxel"               # Vec3f (for ROI Objects)
INF_ENDVOXEL            = "EndVoxel"                 # Vec3f (for ROI Objects)
INF_IMAGEINFOVERSION    = "ImageInfoVersion"         # Int32     Version number of the info type
INF_WINDOWCENTER        = "WindowCenter"             # Int32
INF_WINDOWWIDTH         = "WindowWidth"              # Int32

# everything for LAY_FILES
INF_IVFILE              = "InventorFile"             # String
INF_IVPOINTER           = "InventorPointer"          # Int32  must be nonpersistent!!!   Pointer to the original inventor file in METKManager
INF_IVFULLPOINTER       = "InventorFullPointer"      # Int32  must be nonpersistent!!!   Output from a single object in METKIsoSurface, ignores visibility
INF_ISOPOINTER          = "IsoSurfacePointer"        # Int32  must be nonpersistent!!!   Output from a single object in METKIsoSurface, including visibility
INF_STIPPLEPOINTER      = "StipplePointer"           # Int32  must be nonpersistent!!!   Output from a single object in METKStippling
INF_SILHOUETTEPOINTER   = "SilhouettePointer"        # Int32  must be nonpersistent!!!   Output from a single object in METKSilhouette
INF_TEXPOINTER          = "TexturePointer"           # Int32  must be nonpersistent!!!   Output from a single object in METKTexturing
INF_TREEDATAFILE        = "TreeDataFile"

# everything for LAY_SEGMENTATION
LAY_SEGMENTATION        = "Segmentation"
INF_CONTOURFILE         = "ContourFile"              # String
INF_MARKERFILE          = "MarkerListFile"           # Sting
INF_INTERVALTHRESHLOW   = "LowerIntervalThreshold"   # Double
INF_INTERVALTHRESHHIGH  = "UpperIntervalThreshold"   # Double
INF_THRESH              = "Threshold"                # Double

# Add here all global messages (LAY_GLOBAL + INF_MESSAGE)

LAY_GLOBAL              = "Global"



# METKInfoWin communication
OBJ_GUI			= "GUI"
LAY_INFOWIN		= "InfoWin"
INF_ISON		= "isOn"
INF_INFOHEADER		= "InfoHeader"
INF_INFOTEXT		= "InfoText"



#Layer and Information for VolViz modules
LAY_PROPERTIES		= "Properties"
LAY_TRANSFERFUNCTION	= "Transferfunction"
INF_COLORPOINTS		= "ColorPoints"
INF_ALPHAPOINTS		= "AlphaPoints"



# VIEWER XYZ layers

INF_GLOBAL_REFRESH			= "Refresh"
LAY_VIEWER_CAMERA			= "Camera"
LAY_VIEWER_PROPERTIES			= "Properties"
INF_VIEWER_CAMERA_POSITION		= "Position"
INF_VIEWER_CAMERA_ORIENTATION		= "Orientation"
INF_VIEWER_CAMERA_HEIGHT		= "Height"
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

LAY_VIEWER_SCREENSHOT                   = "Screenshot"
INF_SCREENSHOTFILE                      = "ScreenshotFilename" # String
INF_SCREENSHOTTHUMB                     = "ScreenshotThumb"    # Bool    generate thumbnail from screenshot
INF_SCREENSHOTFLAG                      = "ScreenshotFlag"     # Bool    True = make screenshot False = Done

# additional tags for Viewer2D
INF_WINDOWCENTER        = "WindowCenter"             # Double  0..1
INF_WINDOWWIDTH         = "WindowWidth"              # Double  0..1
INF_WPOSITION           = "WorldPosition"            # Vec3f   Current world position
INF_LAYOUT              = "LayoutMode"               # String  LAYOUT_AXIAL, LAYOUT_SAGITTAL, LAYOUT_CORONAL, 
                                                     #         LAYOUT_CUBE, LAYOUT_CUBE_EQUAL, LAYOUT_CUBE_CUSTOMIZED, 
                                                     #         LAYOUT_ROW, LAYOUT_ROW_EQUAL, LAYOUT_ROW_AXIALEXTRA, LAYOUT_COLUMN
INF_ANNOTATIONS         = "Annotations"              # Bool



# Measurement Tools
# INF_OBJTYPE value is "Tool"

OBJ_MEASTOOL            = "MeasurementTool"
LAY_DETAILS             = "Details"
INF_MEASTOOL            = "SelectedTool"             # String  Off,MainAxis,MinimalDistance,UserDefDistance,Angle
INF_MEASOBJ1            = "MeasuredObject_1"         # String  Object ID of the selected object 1
INF_MEASOBJ2            = "MeasuredObject_2"         # String  Object ID of the selected object 2
INF_MEASOBJ3            = "MeasuredObject_3"         # String  Object ID of the selected object 3
INF_MEASPOS1            = "MeasuredPosition_1"       # Vec3f   Selected Position 1
INF_MEASPOS2            = "MeasuredPosition_2"       # Vec3f   Selected Position 2
INF_MEASPOS3            = "MeasuredPosition_3"       # Vec3f   Selected Position 3





class ObjInfoExt(ObjInfo):

   def handleEventsOnce(self, *args):
      """Call event handling function handleEventsOnce"""
      self.fld_eventFirst.touch()
      try: self.ctx.call("handleEventsOnce")
      except: raise RuntimeError, "Couldn't call '"+self.fld_eventType.stringValue()+"' handler"
      self.fld_clearQueue.touch()
      return
   
   def firstInfo(self, objId= None):
      """Get the Id of the current layer's 'first' info. If the given layer has no info error() will
         return 'True'."""
      self.fld_infoFirst.touch()
      return self.fld_infoId.value
   
   def nextInfo(self):
      """Get the Id of the current object's 'next' info. If the database contains
         no more infos for the current object error() will return 'True'.
         NOTE: The firstInfo() function must be called before nextInfo()."""
      self.fld_infoNext.touch()
      return self.fld_infoId.value
   
   def adjustObjectID(self, name):
      """Some old cases use german Umlauts or other special characters, which have to be corrected."""
      # Sonderzeichen
      name = name.replace(".", "_").replace("-", "_")
      # Umlaute können leider nicht ersetzt werden. auch "replace" mag kein Ä
      #name = name.replace("ä", "ae").replace("ü", "ue").replace("ö", "oe").replace("ß", "ss")
      return name

      
class METKObjInfo(ObjInfoExt):
   
   # the following method is taken from the MevVis Safir project
   """Extends ObjInfo class with communication functions"""
   def sendMessage(self, layer, msg, arg= None):
     """Send 'message' and 'argument' to module specified by 'layer'
        (e.g. sendMessage(LAY_GLOBAL,MSG_LOAD) or sendMessage(LAY_VIEWER,MSG_VIEWMODE,""))"""

     self.typedSet(OBJ_COMMUNICATION,layer,INF_MESSAGE, msg, INFOTYPE_MESSAGE)
     if arg: self.set(INF_DATA, arg)
     self.notify()
     return

   def getImageInfo(self, objectID, infoID, type = None):
      """To avoid redundant information in the ObjMgr database, informations for the image should be saved only one time.
         For this purpose, the image layer only saves information which differs from the parent entry. 
         This function retrieves the information you need and passes the parent entries if necessary.
         """
      if self.existInfo(objectID, LAY_IMAGE, infoID):
         if type == None:
            type = self.getType(objectID, LAY_IMAGE, infoID)
            
         if type == INFOTYPE_STRING:
            info = self.get(objectID, LAY_IMAGE, infoID)
         elif type == INFOTYPE_BOOL:
            info = self.getBool(objectID, LAY_IMAGE, infoID)
         elif type == INFOTYPE_INT32:
            info = self.getInt(objectID, LAY_IMAGE, infoID)
         elif type == INFOTYPE_DOUBLE:
            info = self.getDouble(objectID, LAY_IMAGE, infoID)
         elif type == INFOTYPE_MAT4:
            info = self.getMat4(objectID, LAY_IMAGE, infoID)
         elif type == INFOTYPE_VEC3:
            info = self.getVec3(objectID, LAY_IMAGE, infoID)
         else:
            MLAB.logError("getImageInfo: Info type " + type + " not supported!")
            info = None
      else:         
         info = None
      if info == None:
         objectIDNew = self.get(objectID, LAY_GLOBAL, INF_PARENT)         
         if objectIDNew != "" and objectID != objectIDNew:
            info = self.getImageInfo(objectIDNew, infoID)
         else:
            info = None
      return info
      
   def setStatusMsg(self, message):
      self.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, message, INFOTYPE_MESSAGE)
      self.notify()
      pass
      
   def setStatusProgress(self, progress):
      self.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS, progress, INFOTYPE_DOUBLE)
      self.notify()
      pass
      
   def setEventReceiveMode(self, mode, objIds= None, layerIds= "*"):
      """Set event receive mode"""
      if mode == ERM_NONE:
         mode = ERM_SELECTED
      self.fld_eventRcvMode.value= mode
      if objIds:
         self.fld_acceptObjs  .value= objIds + "," + OBJ_COMMUNICATION
         self.fld_acceptLayers.value= layerIds + "," + LAY_GLOBALEVENTS
      else:
         self.fld_acceptObjs  .value= OBJ_COMMUNICATION
         self.fld_acceptLayers.value= LAY_GLOBALEVENTS
      return

   def handleEvents(self, *args):
      """Call event handling functions.
         The method is extended in a way that the global messages "Loaded", "Save" and "Cleanup" are send to extra methods."""

      self.fld_eventFirst.touch();
      while self.fld_successful.value:
         if self.activeObject() == OBJ_COMMUNICATION:
            if self.get() == MSG_LOADED:
               try: self.ctx.call("handleLoadedEvent");
               except: raise RuntimeError, "Couldn't call 'handleLoadedEvent' handler"
            elif self.get() == MSG_SAVE:
               try: self.ctx.call("handleSaveEvent");
               except: raise RuntimeError, "Couldn't call 'handleSaveEvent' handler"
            elif self.get() == MSG_CLEANUP:
               try: self.ctx.call("handleCleanupEvent");
               except: raise RuntimeError, "Couldn't call 'handleCleanupEvent' handler"
            elif not self.fld_eventType.stringValue() == "ObjectCreated" and not self.fld_eventType.stringValue() == "ObjectRemoved":
               try: self.ctx.call("handle"+self.fld_eventType.stringValue()+"Event");
               except: raise RuntimeError, "Couldn't call '"+self.fld_eventType.stringValue()+"' handler"
         else:
            try: self.ctx.call("handle"+self.fld_eventType.stringValue()+"Event");
            except: raise RuntimeError, "Couldn't call '"+self.fld_eventType.stringValue()+"' handler"
         self.fld_eventNext.touch();
      self.fld_clearQueue.touch();
      return

















_debug_Output = True

def setDebugOutput(value):
   global _debug_Output
   _debug_Output = value
   return
   
   
def Debug(_ctx,debugStr):
   global _debug_Output
   if (_debug_Output):
      _ctx.log("Debug:"+debugStr)
   return
   
   
def statusMsg(_cls_info,value):
   _cls_info.set(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, value)
   _cls_info.notify()
   return
   
   
   
def cleanPathSlashes(path):
   path = path.replace("\\","/")
   return path
   

# print relpath("D:/jhfsd", "D:/a")
# '../a'
# Funktioniert nur mir Forward Slashes!
# Am Ende des Paths darf kein Slash sein!

#
# helper functions for relative paths
#
import os

def pathsplit(p, rest=[]):
    (h,t) = os.path.split(p)
    if len(h) < 1: return [t]+rest
    if len(t) < 1: return [h]+rest
    return pathsplit(h,[t]+rest)

def commonpath(l1, l2, common=[]):
    if len(l1) < 1: return (common, l1, l2)
    if len(l2) < 1: return (common, l1, l2)
    if l1[0] != l2[0]: return (common, l1, l2)
    return commonpath(l1[1:], l2[1:], common+[l1[0]])

def relpath(p1, p2):
    p1 = MLABFileManager.cleanupPath(p1)
    p2 = MLABFileManager.cleanupPath(p2)
    if p1 != p2:
       (common,l1,l2) = commonpath(pathsplit(p1), pathsplit(p2))
       p = []
       if len(l1) > 0:
           p = [ '../' * len(l1) ]
       p = p + l2
       return os.path.join( *p )
    else:
       return ""
       
       
       
#math helper functions
def crossProduct(a,b):
   result = (a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0])
   return result
   
def dotProduct(a,b):
   result = a[0]*b[0]+a[1]*b[1]+a[2]*b[2]
   return result


def vecLength(a):
   result = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])
   return result