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
   
   structures = ctx.field("structures").value
   # Kommas durch Punkte ersetzen und Leerzeichen löschen
   structures.replace(',', '.')
   structures.replace(' ', '')
   
   save = ctx.module("ImgSave")
   ctx.field("ImgSave.postfix").value = ".euclid." + parent + "." + structures
   # in den "sourceName" eintragen, damit in "filename" der Suffix enthalten ist
   save.field("sourceName").value = _caseDir + MLABFileManager.splitPath(_cls_info.getImageInfo(parent, INF_FILENAME))["name"]

   fileManager = MLABFileManager
   distanceImage = fileManager.splitPath(fileManager.normalizePath(save.field("filename").value))["name"]
   
   if fileManager.exists(fileManager.normalizePath(save.field("filename").value)) and not ctx.field("overwriteFile").value:
      # Jump directly to: Distance Transform in ImgLoad laden
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
         
         if structures.find(_cls_info.get(child, LAY_DESCRIPTION, INF_STRUCTURE)) != -1:
            somethingToSave = True
         
            # Netzwerk aufbauen für Distance Transform. Erstes Image braucht Sonderbehandlung
            if firstImg:
               firstImg = False
               img = ctx.addModule("ImgLoad")
               img.field("instanceName").setStringValue("modul"+str(modulCounter))
               modulCounter += 1
               img.field("filename").value = _caseDir + _cls_info.getImageInfo(child, INF_FILENAME)
               
               aritL = ctx.addModule("Arithmetic1")
               aritL.field("instanceName").setStringValue("modul"+str(modulCounter))
               modulCounter += 1
               aritL.field("function").value = "Divide"
               aritL.field("constant").value = _cls_info.getImageInfo(child, INF_OBJVALUE, INFOTYPE_INT32) * 10
               
               aritL.field("input0").connectFrom(img.field("output0"))
            else:
               img = ctx.addModule("ImgLoad")
               img.field("instanceName").setStringValue("modul"+str(modulCounter))
               modulCounter += 1
               img.field("filename").value = _caseDir + _cls_info.getImageInfo(child, INF_FILENAME)
               
               aritR = ctx.addModule("Arithmetic1")
               aritR.field("instanceName").setStringValue("modul"+str(modulCounter))
               modulCounter += 1
               aritR.field("function").value = "Divide"
               aritR.field("constant").value = _cls_info.getImageInfo(child, INF_OBJVALUE, INFOTYPE_INT32) * 10
               
               aritR.field("input0").connectFrom(img.field("output0"))
               
               arit2 = ctx.addModule("Arithmetic2")
               arit2.field("instanceName").setStringValue("modul"+str(modulCounter))
               modulCounter += 1
               arit2.field("input0").connectFrom(aritL.field("output0"))
               arit2.field("input1").connectFrom(aritR.field("output0"))
               
               aritL = arit2
            
      if somethingToSave:
         # letzten aritL mit ImgSave verbinden und Datei speichern
         save = ctx.module("ImgSave")
         euclid = ctx.module("EuclideanDTF")
         euclid.field("input0").connectFrom(aritL.field("output0"))
         save.field("save").touch()
         MLAB.processEvents()
         ctx.field("Switch.currentInput").value = 0
         
         # Netzwerk wieder aufräumen, damit der unglaubliche Speicherbedarf wieder rückgängig gemacht wird
         delModule = 0
         while delModule < modulCounter:
            ctx.module("modul"+str(delModule)).remove()
            delModule += 1
      else:
         constImg = ctx.module("ConstImg")
         constImg.field("sizeX").value = _cls_info.getImageInfo(parent, INF_SIZEX, INFOTYPE_INT32)
         constImg.field("sizeY").value = _cls_info.getImageInfo(parent, INF_SIZEY, INFOTYPE_INT32)
         constImg.field("sizeZ").value = _cls_info.getImageInfo(parent, INF_SIZEZ, INFOTYPE_INT32)
         constImg.field("apply").touch()
         ctx.field("Switch.currentInput").value = 1
         ctx.field("ready").value = True
   
   # Distance Transform in ImgLoad laden
   if somethingToSave:
      ctx.field("ImgLoad.filename").value = _caseDir + distanceImage
      ctx.field("Switch.currentInput").value = 0
      ctx.field("ready").value = True
   # zu guter Letzt die neue Distance Transform propagieren
   _cls_info.notify()
      
   return


def setStructures(field = 0):
   getImage(ctx.field("roiSelect").value)
   pass