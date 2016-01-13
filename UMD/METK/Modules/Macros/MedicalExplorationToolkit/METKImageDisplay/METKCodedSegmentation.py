# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

_modulCounter = 0

_version = "2.0"

# called from METKImageDisplay.py
def closeImage(image = ""):
   ctx.field("ready").value = False
   ctx.field("ImgLoad.close").touch()
   pass


# called from METKImageDisplay.py
def getImage(parent,force=False):
   global _caseDir
   #It was to buggy to think, the _caseDir will be set by someone else
   #so the _caseDir will be get from the ObjMgr in every case here
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   objValue = 1   
   somethingToSave = False
   ctx.field("ready").value = False

   if (not ctx.field("roiSelect").value):
      MLAB.logWarning("No ROI selected for generation of CodedSegmentation")
      return
      
   fileManager = MLABFileManager
   
   # be sure that you cut off relative paths
   codSegImage = fileManager.splitPath(fileManager.normalizePath(_cls_info.getImageInfo(parent, INF_FILENAME)))["base"] + ".codseg.dcm"
      
   if (not fileManager.exists(_caseDir + codSegImage) or not _cls_info.existLayer(OBJ_CODEDSEGMENTATION,ctx.field("roiSelect").value) or force or _cls_info.get(OBJ_CODEDSEGMENTATION,LAY_GLOBAL,INF_VERSION)!=_version):
      if (not fileManager.exists(_caseDir + codSegImage)):
         MLAB.logWarning("CodSeg files not found (dir+file): "+_caseDir + codSegImage)
      if (not _cls_info.existLayer(OBJ_CODEDSEGMENTATION,ctx.field("roiSelect").value)):
         MLAB.logWarning("No CodSeg information in METK-File found.")
      else:
         _cls_info.removeLayer(OBJ_CODEDSEGMENTATION,ctx.field("roiSelect").value)

      if (_cls_info.existObject(OBJ_CODEDSEGMENTATION) and _cls_info.get(OBJ_CODEDSEGMENTATION,LAY_GLOBAL,INF_VERSION)!=_version):
         MLAB.logWarning("Old CodedSegmentation Version. old version="+_cls_info.get(OBJ_CODEDSEGMENTATION,LAY_GLOBAL,INF_VERSION)+"  new version ="+_version)
         # Window is not closeable sometimes. Hopefully all cases were converted in the last few month
         #MLAB.showWarning ("The 2D overlay of the segmentation results may be incorrect or outdated and will be recomputed now. You must save the case after loading has been finished.")
   
            
      _cls_info.setStatusMsg("Generating Coded Segmentation...")
      _cls_info.set(parent, LAY_IMAGE, INF_CODSEGIMAGE, codSegImage)
      
      ctx.field("toAdd.close").touch()
      
      #Write Version
      _cls_info.set(OBJ_CODEDSEGMENTATION,LAY_GLOBAL,INF_VERSION,_version)
            
      MLAB.log("Create new coded segmentation for "+ctx.field("roiSelect").value)
                  
      firstFile = True
      
      childIDs = _cls_info.get(parent, LAY_GLOBAL, INF_CHILDS)
      _cls_info.set(parent, LAY_IMAGE, INF_CODSEGIMAGE, codSegImage)
      
      totalNumObj = len(childIDs)
      
      while len(childIDs) > 1:
         
         #set progressbar
         _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS, (totalNumObj-len(childIDs))/totalNumObj, INFOTYPE_DOUBLE)
         _cls_info.notify()

         nextSep = childIDs.find(";")
         if nextSep != -1:
            somethingToSave = True
            child = childIDs[:nextSep]
            nextSep += 1
            childIDs = childIDs[nextSep:]
         else:
            child = childIDs
            childIDs = ""
                            
         ctx.field("toAdd.filename").value = _caseDir + _cls_info.get(child, LAY_IMAGE, INF_FILENAME)
         objValue = _cls_info.getImageInfo(child, INF_OBJVALUE)
         MLAB.log("child:"+child+"    objValue="+str(objValue))
         if (objValue!=None):
            ctx.field("CalcCodedSegmentation.addAllExceptNull").setBoolValue(False)
            ctx.field("CalcCodedSegmentation.addMinValue").setIntValue(objValue)
            ctx.field("CalcCodedSegmentation.addMaxValue").setIntValue(objValue)
            #ctx.field("Bypass.input0").connectFrom(ctx.field("IntervalThresh.output0"))
            #ctx.field("IntervalThresh.threshCenter").setIntValue(int(objValue))
         else:
            ctx.field("CalcCodedSegmentation.addAllExceptNull").setBoolValue(True)
            #ctx.field("Bypass.input0").connectFrom(ctx.field("OrthoReformat36.output2"))
            
         #Das RESET des CodSeg Moduls wird erst hier ausgeführt, weil am CodedSeg Modul zwei valide Eingänge hängen müssen um ein Reset fehlerfrei durchführen zu können
         if (firstFile):
            ctx.field("CalcCodedSegmentation.Reset").touch()
            ctx.log("CalcCodedSegmentation.Reset")
            firstFile=False;
        
         ctx.field("CalcCodedSegmentation.NameToAdd").value = child
         ctx.field("CalcCodedSegmentation.Add").touch()
         somethingToSave = True
      
      if (somethingToSave):
         ctx.field("CalcCodedSegmentation.Purge").touch()  
            
         #Alle INF_CODSEGVALUE eintragen
         singleObjects = ctx.field("objValues").value.split(";")
         for obj in singleObjects:
            objectParts=obj.split(":")
            if (len(objectParts)==2):
               _cls_info.set(objectParts[0], LAY_IMAGE, INF_CODSEGVALUE, objectParts[1])            
         
         singleImgValues = ctx.field("imgValues").value.split(";")
         for imgVal in singleImgValues:
            imgValParts = imgVal.split(":")
            if (len(imgValParts)==2):            
               _cls_info.set(OBJ_CODEDSEGMENTATION, ctx.field("roiSelect").value, imgValParts[0].zfill(6), imgValParts[1])
                                   
         save = ctx.module("ImgSave")
         save.field("sourceName").value = _caseDir + codSegImage         
         save.field("save").touch()
         
         MLAB.log("CodedSegmentation finished for "+ctx.field("roiSelect").value)                                  
   
   # Coded segmentation in ImgLoad laden
   if (fileManager.exists(_caseDir + codSegImage)):
      MLAB.log("Load CodedSegmentation for "+ctx.field("roiSelect").value)
      ctx.field("ImgLoad.filename").value = _caseDir + codSegImage
      ctx.field("ready").value = True
   else:
      MLAB.logWarning("CodedSegmentation not found and not created for "+ctx.field("roiSelect").value)   
   
   # zu guter Letzt die neue CodedSegmentation propagieren
   _cls_info.notify()      
      
   return
   
   
def forceGeneration(fieldP=0):
   getImage(ctx.field("roiSelect").value,True)
   return
   
def closeImg(foledP=0):
   #close opened CodSeg to write a new one
   ctx.field("ready").value = False
   ctx.field("ImgLoad.close").touch()   
   return