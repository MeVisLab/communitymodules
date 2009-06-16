# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

# Global class instances
_cls_input= None
_cls_output= None


def init():
   global _cls_output
   _cls_output = ObjInfo(ctx.module("output"))
   return
   
def convertFile(againstTypeError = 0):
   ctx.field("NeckVisionXMLConverter.convert").touch()
   ctx.field("StandardXMLConverter.convert").touch()
   ctx.field("HepaXMLConverter.convert").touch()
   
   if ctx.field("NeckVisionXMLConverter.readableFormat").value == True:
      ctx.field("detectedFormat").value = "NeckVision 1.0"

   elif ctx.field("StandardXMLConverter.readableFormat").value == True:
      ctx.field("detectedFormat").value = "MedicalExplorationToolkit 1.0"
      
   elif ctx.field("HepaXMLConverter.readableFormat").value == True:
      ctx.field("detectedFormat").value = "MeVisLiver"
   
   else:
      ctx.field("detectedFormat").value = "could not convert"
      ctx.field("successful").value = False
   return


def setSuccessful(field):
   if field.value == True:
      _cls_output.typedSet(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED, MSG_CONVERTED, INFOTYPE_MESSAGE)
      _cls_output.notify()
      ctx.field("successful").value = True
   return