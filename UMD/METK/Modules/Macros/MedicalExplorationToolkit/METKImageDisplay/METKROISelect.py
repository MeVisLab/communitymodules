# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *


# called from METKImageDisplay.py
def getImage(image):
   global _caseDir
   ###Nur weil er aus rigendwelchen Gründen das CaseDir hier nicht hatte obwohl er es in der imageDisplay überall mehrfahc richtig gesetzt hatte
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   ###
   
   switchOutput = 0
   imageName = _cls_info.getImageInfo(image, INF_FILENAME)
   if imageName != None:
      ctx.field("ImgLoad.filename").value = _caseDir + _cls_info.getImageInfo(image, INF_FILENAME)
      switchOutput = 1
      
      # look for defined sub image
      start = _cls_info.getImageInfo(image, INF_STARTVOXEL, INFOTYPE_STRING)
      end = _cls_info.getImageInfo(image, INF_ENDVOXEL, INFOTYPE_STRING)
      if start != None and end != None:
         switchOutput = 2
         ctx.field("startVector.v").value = start
         ctx.field("endVector.v").value = end
         
   ctx.field("Switch.currentInput").value = switchOutput
   return


# called from METKImageDisplay.py
def closeImage(image = ""):
   ctx.log("METKROISelect closeImage("+image+")")
   ctx.field("ImgLoad.close").touch()
   ctx.field("Switch.currentInput").value = 0
   pass
