# Python module import
from ObjMgr import *
from VisDefinitions import *
from os import *


# Global class instances
_cls_info= None

_caseDir = ""

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))   
   #_cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO + "," + OBJ_COMMUNICATION, LAY_SELECTEDOBJ+","+LAY_MOUSEOVEROBJ)
   
   return


def importImg(fieldP=0):
   print "Start Import"
   global _caseDir
   _caseDir = _cls_info.get(OBJ_CASE,LAY_CASE,INF_CASEDIR)
   if (_caseDir==""):
      print "No CaseDir available. Please create a new case first using the METKManager."
      return
   
   
   imageFile = MLABFileDialog.getOpenFileName(_caseDir,"Image *.*","Select an image")
   imageFile = os.path.split(imageFile)[1]
   imageFilePure = os.path.splitext(imageFile)[0]
   print imageFile
   
   if (imageFile==""):
      print "No image selected."
      return
   
   ctx.field("ImgLoad.filename").value = _caseDir+"/"+imageFile
   ctx.field("METKAddImage.objectID").value = "Image_"+imageFilePure
   ctx.field("METKAddImage.create").touch()
   
   ctx.field("validImage").setValue(True)
   
   
   ctx.field("METKAddROI.objectID").value = "ROI_"+imageFilePure   
   ctx.field("METKAddROI.parentID").value = ctx.field("METKAddImage.objectID").value
   ctx.field("METKAddROI.create").touch()
   
   
   #Import Structures
   global _strucFiles
   _strucFiles=""
   for fileinfolder in listdir(_caseDir):
      if (os.path.splitext(fileinfolder)[1]==".dcm"):
         if (fileinfolder!=imageFile and fileinfolder!=os.path.split(ctx.field("METKAddROI.filename").value)[1]):
            print fileinfolder
            _strucFiles = _strucFiles+fileinfolder+","

   createNextStruc()
   
   return
   
   
   
def createNextStruc(fieldP=0):
   global _strucFiles
   if (_strucFiles==""):
      print "Ready"
      return
            
   if (ctx.field("METKAddStructure.ready").value==True):   
      singleStruc = _strucFiles.split(',')[0]
      if (singleStruc!=""):         
         ctx.field("segMask.filename").value=_caseDir+"/"+singleStruc
         ctx.field("METKAddStructure.name").value=os.path.splitext(singleStruc)[0]
         ctx.field("METKAddStructure.parentID").value=ctx.field("METKAddROI.objectID").value
         
         #
         #ctx.field("METKAddStructure.createIVFile").value=False
         #
         
         ctx.field("infoText").value = "Please fill out some information about the structure:<br><b>" + singleStruc+"</b>"
         if (ctx.showModalDialog("StructureInfo", "StructureInfo for "+ctx.field("METKAddStructure.name").value)==1):
                  
            structureList = ctx.field("structuresEnum").value
            if (structureList.find(ctx.field("METKAddStructure.structure").value+",")==-1):
               ctx.field("structuresEnum").value += ctx.field("METKAddStructure.structure").value+","
            
            structuregroupList = ctx.field("structuregroupEnum").value
            if (structuregroupList.find(ctx.field("METKAddStructure.strGroup").value+",")==-1):
               ctx.field("structuregroupEnum").value += ctx.field("METKAddStructure.strGroup").value+","
               
                        
            ctx.field("METKAddStructure.create").touch()
         else:
            ctx.callLater(0,"createNextStruc")

         _strucFiles = _strucFiles.replace(singleStruc+",","")
         print singleStruc            
   return
   

def closeStructInfo(fieldP=0):
   ctx.field("METKAddStructure.structure").value = ctx.control("comboStructure").currentText()
   ctx.field("METKAddStructure.strGroup").value = ctx.control("comboStructureGroup").currentText()
   ctx.window().accept()
   return
   
def closeStructInfoSkip(fieldP=0):
   ctx.window().reject()
   return
  

#--------------------------------------------
# Event handling
#--------------------------------------------

def handleLoadedEvent():
   pass

def handleSaveEvent():
   pass

def handleCleanupEvent():
   pass
   
def handleObjectCreatedEvent():
   return

def handleObjectRemovedEvent():
   pass

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return

def handleAttributeRemovedEvent():
   pass

def handleAttributeModifiedEvent():
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
   
   
   #if object == OBJ_CSO and layer == LAY_SELECTEDOBJ and (info == INF_SELECTION or info == INF_OBJID):      
   #   if _cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID) != "":

   return