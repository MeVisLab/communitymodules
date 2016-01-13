# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
from string import *


# called from METKImageDisplay.py
def closeImage(image = ""):
   ctx.field("ImgLoad.close").touch()
   ctx.field("ready").value = False
   pass


# called from METKImageDisplay.py
def getImage(parent):
   objValue = 1
   firstImg = True
   somethingToSave = False
   ctx.field("ready").value = False
   
   save = ctx.module("ImgSave")
   ctx.field("ImgSave.postfix").value = ".bitImage." + parent
   # in den "sourceName" eintragen, damit in "filename" der Suffix enthalten ist
   save.field("sourceName").value = _caseDir + MLABFileManager.splitPath(_cls_info.getImageInfo(parent, INF_FILENAME))["name"]

   fileManager = MLABFileManager
   bitImage = fileManager.splitPath(fileManager.normalizePath(save.field("filename").value))["name"]
   
   if fileManager.exists(fileManager.normalizePath(save.field("filename").value)) and not ctx.field("overwriteFile").value:
      # Jump directly to: BitImage in ImgLoad laden
      somethingToSave = True
   else:
      # Das bisher geoeffnete Bild schliessen
      ctx.field("ImgLoad.close").touch()
      # allen Modulen einen Namen zuweisen, damit sie hinterher leichter wieder gelöscht werden können
      modulCounter = 0

      childIDs = _cls_info.get(parent, LAY_GLOBAL, INF_CHILDS)
      
      while len(childIDs) > 1:
         nextSep = childIDs.find(";")
         if nextSep != -1:
            child = childIDs[:nextSep]
            nextSep += 1
            childIDs = childIDs[nextSep:]
         else:
            child = childIDs
            childIDs = ""
      
         # Netzwerk aufbauen für BitImage. Erstes Image braucht Sonderbehandlung
         if firstImg:
            firstImg = False
            aritL = ctx.addModule("BitImage")
            aritL.field("toBePoweredBy2").value = modulCounter
            aritL.field("filename").value = _caseDir + _cls_info.getImageInfo(child, INF_FILENAME)
            ctx.log("Value: " + str(modulCounter) + " = " + child)
            modulCounter += 1

         else:
            aritR = ctx.addModule("BitImage")
            aritR.field("toBePoweredBy2").value = modulCounter
            aritR.field("filename").value = _caseDir + _cls_info.getImageInfo(child, INF_FILENAME)
            ctx.log("Value: " + str(modulCounter) + " = " + child)
            modulCounter += 1
            
            arit2 = ctx.addModule("Arithmetic2")
            arit2.field("input0").connectFrom(aritL.field("output0"))
            arit2.field("input1").connectFrom(aritR.field("output0"))
            
            aritL = arit2
         
      # letzten aritL mit ImgSave verbinden und Datei speichern
      save = ctx.module("ImgSave")
      save.field("input0").connectFrom(aritL.field("output0"))
      save.field("save").touch()
      MLAB.processEvents()
      ctx.field("Switch.currentInput").value = 0
      
      ctx.log("Combined structure masks (max. 32): " + str(modulCounter))
   
   # BitImage in ImgLoad laden
   ctx.field("ImgLoad.filename").value = _caseDir + bitImage
   ctx.field("Switch.currentInput").value = 0
   ctx.field("ready").value = True
   # zu guter Letzt das neue BitImage propagieren
   _cls_info.notify()
      
   return


def setStructures(field = 0):
   getImage(ctx.field("roiSelect").value)
   pass