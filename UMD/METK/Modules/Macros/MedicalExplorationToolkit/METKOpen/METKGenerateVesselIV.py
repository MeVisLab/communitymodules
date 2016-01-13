# Python module import
from ObjMgr import *
from VisDefinitions import *


# Global class instances
_cls_info = None
_waitforExtractFaces = False
_waitForShapeParition = False
_waitForSaving = False
#_traceColors = ["#ffffff","#dc002c","#ff767d","#00d129","#8fff6f","#404040","#2f3fed","#7d8aff","#fff95e","#da9932","#000000","#80fffa","#00aec1","#e461ff","#a100ad"]
#_traceColorsFloat = ["1.0 1.0 1.0","0.8627 0.0 0.17255","1.0 0.46275 0.4902","0.0 0.81961 0.16078","0.56078 1.0 0.43529","0.25098 0.25098 0.25098","0.18431 0.24706 0.92941","0.4902 0.5412 1.0","1.0 0.9765 0.3686","0.8549 0.6 0.19608","0.0 0.0 0.0","0.50196 1.0 0.9804","0.0 0.6824 0.7569","0.89411 0.3804 1.0","0.6314 0.0 0.6784"]
_traceColorsFloat = ["","","","","","","","","","","","","","","","","","",]
_traceColorsFloat[0] = "1 1 1"
_traceColorsFloat[2] = "0.862745 0 0.172549"
_traceColorsFloat[3] = "1 0.462745 0.490196"
_traceColorsFloat[4] = "0 0.819608 0.160784"
_traceColorsFloat[5] = "0.560784 1 0.435294"
_traceColorsFloat[1] = "0.501961 0.501961 0.501961"
_traceColorsFloat[6] = "0.184314 0.239216 0.929412"
_traceColorsFloat[7] = "0.490196 0.541176 1"
_traceColorsFloat[13] = "1 0.976471 0.368627"
_traceColorsFloat[12] = "0.615686 0.6 0.196078"
_traceColorsFloat[15] = "0 0 0"
_traceColorsFloat[11] = "0.501961 1 0.980392"
_traceColorsFloat[10] = "0 0.682353 0.756863"
_traceColorsFloat[9] = "0.894118 0.380392 1"
_traceColorsFloat[8] = "0.631373 0 0.678431"


#AUS mlGraphLabelToColor.cpp
#Doppelte Abbildung: ImageValue->TraceColorCode->index für _traceColorsFloat

#    // Selle LUT
#    _selleLUTLabelToRGBA.push_back( vec4(1, 1, 1, 1) );                         // White  0
#    _selleLUTLabelToRGBA.push_back( vec4(0.862745, 0, 0.172549, 1) );           // Red    1
#    _selleLUTLabelToRGBA.push_back( vec4(1, 0.462745, 0.490196, 1) );           // lRed   2
#    _selleLUTLabelToRGBA.push_back( vec4(0, 0.819608, 0.160784, 1 ) );          // Green  3
#    _selleLUTLabelToRGBA.push_back( vec4(0.560784, 1, 0.435294, 1) );           // lGreen 4
#
#    _selleLUTLabelToRGBA.push_back( vec4(0.501961, 0.501961, 0.501961, 1) );    // Gray   5
#
#    _selleLUTLabelToRGBA.push_back( vec4(0.184314, 0.239216, 0.929412, 1) );    // Blue   6
#    _selleLUTLabelToRGBA.push_back( vec4(0.490196, 0.541176, 1, 1) );           // lBlue  7
#    _selleLUTLabelToRGBA.push_back( vec4(1, 0.976471, 0.368627, 1) );           // Yellow 8
#    _selleLUTLabelToRGBA.push_back( vec4(0.615686, 0.6, 0.196078, 1) );         // dYellow 9
#
#    _selleLUTLabelToRGBA.push_back( vec4(0, 0, 0, 0) );                         // Transparent 10
#
#    _selleLUTLabelToRGBA.push_back( vec4(0.501961, 1, 0.980392, 1) );           // Cyan  11
#    _selleLUTLabelToRGBA.push_back( vec4(0, 0.682353, 0.756863, 1) );           // dCyan 12
#    _selleLUTLabelToRGBA.push_back( vec4(0.894118, 0.380392, 1, 1) );           // Magenta   13
#    _selleLUTLabelToRGBA.push_back( vec4(0.631373, 0, 0.678431, 1) );           // dMagenta  14
#
#    // selle -> RL
#    _selleLUTLabelToValue.push_back( 12025 ); // 0->0 white
#    _selleLUTLabelToValue.push_back( 12125 ); // 1->2 red
#    _selleLUTLabelToValue.push_back( 12175 ); // 2->3 lred
#    _selleLUTLabelToValue.push_back( 12225 ); // 3->4 green
#    _selleLUTLabelToValue.push_back( 12275 ); // 4->5 lgreen
#
#    _selleLUTLabelToValue.push_back( 12075 ); // 5->1 gray
#
#    _selleLUTLabelToValue.push_back( 12325 ); // 6->6  blue
#    _selleLUTLabelToValue.push_back( 12375);  // 7->7  lblue
#    _selleLUTLabelToValue.push_back( 12675 ); // 8->13 yellow
#    _selleLUTLabelToValue.push_back( 12625 ); // 9->12 dyellow
#
#    _selleLUTLabelToValue.push_back( 12775); // 10->15 transparent
#
#    _selleLUTLabelToValue.push_back( 12575); // 11->11 cyan
#    _selleLUTLabelToValue.push_back( 12525); // 12->10 dcyan
#    _selleLUTLabelToValue.push_back( 12475); // 13->9  magenta
#    _selleLUTLabelToValue.push_back( 12425); // 14->8  dtransparent


def init():
   global _cls_info

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   
   global _waitforExtractFaces
   _waitforExtractFaces = False
   global _waitForShapeParition
   _waitForShapeParition = False
   global _waitForSaving
   _waitForSaving = False
   pass


def requestReceived(field):
   global _objectID
   global _cls_info
   _objectID = ctx.field("METKMsgReceiver.data").value
   
   if _objectID != "":      
      caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
      vesselFile = _cls_info.get(_objectID, LAY_FILES, INF_TREEDATAFILE)
      vesselFile = vesselFile.replace(".xml",".txt")
      ctx.log("ObjectID= "+_objectID+"   Vesselfile="+vesselFile)
      
      ctx.field("LoadTreeData.filename").value = caseDir + "/" + vesselFile
      ctx.field("LoadTreeData.load").touch()
      
      inventorFile = _cls_info.get(_objectID, LAY_FILES, INF_IVFILE)
      ctx.field("SoSceneWriterMD.fileName").value = caseDir + "/" + inventorFile
      
      
      #color = ctx.field("defaultColor").hexValue()
      color = ctx.field("defaultColor").stringValue()
      #colorCode = int(_cls_info.get(_objectID, LAY_APPEARANCE, INF_TRACECOLORCODE))
      objMgrColor = _cls_info.get(_objectID, LAY_APPEARANCE, INF_COLOR)      
      
      if (objMgrColor!=""):
         color = objMgrColor
         
         if (ctx.field("useTraceColors").value == True):
            colorCode = int(_cls_info.get(_objectID, LAY_APPEARANCE, INF_TRACECOLORCODE))            
            if (colorCode>=0):
               color = _traceColorsFloat[colorCode]
               ctx.log(color)
               
               if (ctx.field("setTraceColorsAsColors").value == True):
                  _cls_info.typedSet(_objectID, LAY_APPEARANCE, INF_COLOR, color , INFOTYPE_VEC3)
                  _cls_info.notify()
            #else:
            #   color = _traceColors[0]
            
         else:
            ctx.log(objMgrColor)
         
         if (color=="0.8 0.8 0.8"): color="1 1 1" #Das ist nötig, weil ide Grundstümpfe eigentlich grau sein sollen nach der Case-XML, aber weiß sind         
         
      global _waitforExtractFaces
      _waitforExtractFaces = True
      #if (ctx.field("useTraceColors").value==True and colorCode>0):
      #   ctx.field("SoExtractColoredFaces.selectedColor").setHexValue(color)
      #else:
      ctx.field("SoExtractColoredFaces.selectedColor").setStringValue(color)
      ctx.field("SoExtractColoredFaces.update").touch()
   pass

def facesExtracted(args=0):
   global _waitforExtractFaces, _waitForShapeParition
   if (_waitforExtractFaces==True and ctx.field("SoExtractColoredFaces.valid").value==True):
      _waitforExtractFaces=False
      _waitForShapeParition=True
      ctx.field("SoShapePartition.update").touch()   
   return
   
def shapeCreated(args):
   global _waitForShapeParition, _waitForSaving
   if (_waitForShapeParition and ctx.field("SoShapePartition.valid").value == True):
      ctx.log("shapeCreated = " + str(ctx.field("SoShapePartition.valid").value))
      _waitForSaving = True
      ctx.field("SoSceneWriterMD.writeScene").touch()
      _waitForShapeParition = False
   return
   
   
def saved(args):
   global _waitForSaving
   if (_waitForSaving):
      _waitForSaving = False
      ctx.field("ready").value = True
      ctx.field("METKMsgReceiver.done").touch()   
   return