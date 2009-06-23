# Python module import
from mevis import *
from string import *



# !!!Das ImageValue scheint nur mit der Queue zu funktionieren?

updateStep = -1

def init(againstTError = 0):
   #ctx.field("IdleTrigger.on").setBoolValue(False)
   ctx.field("creatingSurface").setBoolValue(False)
   ctx.field("ready").setBoolValue(False)
   
   global updateStep
   updateStep = -1
   
   return


def addFile(againstTError = 0):
   if len(ctx.field("filename").value) > 1:
      if ctx.field("queueing").value == False:
         ctx.field("ImageFile.path").value = ctx.field("filename").value
         ctx.field("update").touch() # irgendwie muss man ihn manchmal daran erinnern
         if ctx.field("ImageExists").value == True:
            ctx.field("Image.filename").value = ctx.field("filename").value
         else:
            ctx.log("Image file not found - "+ctx.field("ImageFile.path").value)
      else:         
         ctx.field("Queue.input").value = generateQueueObject()
         ctx.field("Queue.push").touch()
      init()
   return

def startByField(args):
   start(args,True)
   return

def start(args,FirstTry):
   if (args!=0):      
      ctx.field("objTotal").value = ctx.field("Queue.numEntries").value
   
   if (FirstTry):
      ctx.field("objCurrent").value = ctx.field("objTotal").value - ctx.field("Queue.numEntries").value

   if (FirstTry):
      #if ctx.field("BoundingBox.output0").sizeZ() >= 3:         
      ctx.field("BoolInt.boolValue").value = ctx.field("smoothing").value
      
      #Dateinamen einlesen
      if ctx.field("queueing").value == False:
         switchImageValueAuto(0)
         ctx.field("ImageFile.path").value = ctx.field("filename").value      
      else:
         ctx.field("Queue.pop").touch()      
         ctx.field("ImageFile.path").value = splitQueueObject(ctx.field("Queue.output").value,"FileIn")
         ctx.log(splitQueueObject(ctx.field("Queue.output").value,"FileIn"))
         # wenn fileOut leer ist, wird die Standard-Bau-Anleitung genommen, wie sie im Netz im Modul InventorFile definiert ist (Name minus.dcm + .iv)      
         if (splitQueueObject(ctx.field("Queue.output").value,"FileOut")!=""):
            ctx.field("filenameOutput").value = splitQueueObject(ctx.field("Queue.output").value,"FileOut")
            ctx.log("FileOut = " + splitQueueObject(ctx.field("Queue.output").value,"FileOut")    )
    
      
   # testen, ob Surface ueberhaupt erstellt werden muss
   # DCM ueberhaupt vorhanden?
   ctx.field("update").touch()   
   if ctx.field("ImageExists").value == True:
      # Inventor-Datei bereits erstellt und soll nicht ueberschrieben werden?
      if not ctx.field("InventorExists").value or ctx.field("overwrite").value:
         ctx.field("Image.filename").value = ctx.field("ImageFile.path").value
         if ctx.field("queueing").value == True:
            ctx.field("imageValue").setIntValue(splitQueueObject(ctx.field("Queue.output").value,"ImageValue"))            
            ctx.field("imageValueAuto").setBoolValue(splitQueueObject(ctx.field("Queue.output").value,"ImageValueAuto"))
            ctx.log("imageValue = " + str(splitQueueObject(ctx.field("Queue.output").value,"ImageValue")) + "    imageValueAuto = " + str(splitQueueObject(ctx.field("Queue.output").value,"ImageValueAuto")))
            switchImageValueAuto(0)
                                        
         global updateStep
         updateStep = 1
         
         if (ctx.field("BoundingBox.boxVoxels").value>0):
            ctx.field("IsoSurface.createSurface").touch()                          
         else:
            ctx.log("No voxels with objectValue found. No IV-File generated.")
            updateStep=3            
            ctx.field("continueWithNext").touch()
         
      else:
         if (ctx.field("Queue.isEmpty").value == False) and (ctx.field("queueing").value == True):
            start(0,True)
         else:            
            ready()
   else:
      ctx.log("DCM not found - "+ctx.field("Image.path").value) #eigentlich filename, aber filename ist hier leer und in path steht alles drin
   return

        
def surfaceCreated(againstTError = 0):
   global updateStep
   if (updateStep==1):
      ctx.field("OffscreenRender.update").touch()
      #1kb Bug - Nr.3: Durch die Glaettung koennen kleine Gefaeﬂe nicht erstellt werden, deswegen wird smoohting abgestellt und es nochmal versucht, wenn 0 Polygone erzeugt wurden
      #Beispiel: (0/0) polygons created
      tempStr = ctx.field("IsoSurface.info").value
      polygons = tempStr.split("(")[1].split("/")[0]
      ctx.log(polygons+" polygons created")
      if (polygons=="0" and ctx.field("smoothing").value==True):
         ctx.log("Empty object, retry it without smoothing again ...")
         ctx.field("BoolInt.boolValue").value = False
         start(0,False)
      else:      
         MLAB.processInventorQueue()
         updateStep = 2
         ctx.field("SoConvertShapeENT.update").touch()
   else:
      ctx.log("HIER WAER EIN FEHLER GEWESEN")
   return
   
   
def surfaceConverted(againstTError = 0):
   global updateStep
   if (updateStep==2):
      MLAB.processInventorQueue()
      updateStep = 3
      ctx.field("SceneWriter.writeScene").touch()
   else:
      ctx.log("HIER WAER EIN FEHLER GEWESEN")
   return
   
   
def surfaceSaved(againstTError = 0):
   global updateStep
   if (updateStep==3):      
      updateStep = -1
      
      ctx.field("saved").setBoolValue(False)
      
      # Neue Datei bearbeiten oder Ready!
      if (ctx.field("Queue.isEmpty").value == True) or (ctx.field("queueing").value == False):
         ready()
      else:
         ctx.field("ready").setBoolValue(False)         
         start(0,True)
   else:
      ctx.log("HIER WAER EIN FEHLER GEWESEN")
   return
   
   

def continueWithNext(args=0):
   global updateStep
   updateStep=3
   surfaceSaved() #evtl. verzettlung wegen nicht moeglichem callLater=true
   return
   
   
def switchImageValueAuto(againstTError = 0):
   #print "switchImageValueAuto"
   if ctx.field("imageValueAuto").boolValue():
      #print "TRUE"

      ctx.field("IntervalThresh.threshMin").value = ctx.field("MinMaxScan.trueMinValue").value
      #Loesung des 2. 1kB Problems: Das IntervalTresh setzt den Max Wert eigenhaendig zurueck, wenn er groeﬂer als der ImageMax wert ist.      
      ctx.field("IntervalThresh.imageMax").setDoubleValue(ctx.field("MinMaxScan.trueMaxValue").doubleValue())
      ctx.field("IntervalThresh.threshMax").setDoubleValue(ctx.field("MinMaxScan.trueMaxValue").doubleValue())
                     
      ctx.field("BoundingBox.objectMin").value = 1
      ctx.field("BoundingBox.objectMax").value = ctx.field("MinMaxScan.trueMaxValue").value
      
      ctx.field("Scale.inputMin").value = 0
      ctx.field("Scale.inputMax").value = ctx.field("MinMaxScan.trueMaxValue").value
      
   else:
      #print "FALSE"
      ctx.field("IntervalThresh.threshMin").value = ctx.field("imageValue").value
      ctx.field("IntervalThresh.imageMax").setDoubleValue(ctx.field("imageValue").doubleValue()) #Erklaerung siehe bei IF-Bedingung oben
      #print "set IntervalThresh.imageMax with imageValue = " + str(ctx.field("imageValue").value) + "***"
      ctx.field("IntervalThresh.threshMax").value = ctx.field("imageValue").value
      #print "set IntervalThresh.threshMax with imageValue = " + str(ctx.field("imageValue").value) + "***"
      
      ctx.field("BoundingBox.objectMin").value = ctx.field("imageValue").value
      ctx.field("BoundingBox.objectMax").value = ctx.field("imageValue").value
      
      ctx.field("Scale.inputMin").value = 0
      ctx.field("Scale.inputMax").value = ctx.field("imageValue").value
   
   # Take care about the image diemensions, because Resample3D can't handle image with just one slice
   if ctx.field("BoundingBox.output0").sizeZ() < 3:
      ctx.field("BoolInt.boolValue").value = False
   ctx.log("Smoothing activated: " + str(ctx.field("BoolInt.boolValue").value))
   return
   
def filenameOutputChanged(againstTError = 0):   
   if ctx.field("filenameOutput").stringValue() == "":
      ctx.field("InventorFile.update").connectFrom(ctx.field("ImageFile.update"))
      ctx.field("InventorFile.dirname").connectFrom(ctx.field("ImageFile.dirname"))
      ctx.field("InventorFile.basename").connectFrom(ctx.field("ImageFile.basename"))
   else:
      ctx.field("InventorFile.update").connectFrom(ctx.field("UserInventorFile.update"))
      ctx.field("InventorFile.dirname").connectFrom(ctx.field("UserInventorFile.dirname"))
      ctx.field("InventorFile.basename").connectFrom(ctx.field("UserInventorFile.basename"))
   return
   
   
   
def generateQueueObject():
   queueStr = ""
   queueStr = ctx.field("filename").stringValue()+";"+ctx.field("filenameOutput").stringValue()+";"+ctx.field("imageValueAuto").stringValue()+";"+ctx.field("imageValue").stringValue()   
   return queueStr
   

def splitQueueObject(value, param):
   result = split(value,";")
   if param == "FileIn":
      return result[0]
   elif param == "FileOut":
      return result[1]
   elif param == "ImageValueAuto":
      return result[2]=="TRUE"
   elif param == "ImageValue":
      return (int)(result[3])
   return ""
   
   
def objTotal(args):
   ctx.log("Objects to create: "+str(ctx.field("objTotal").value))
   return

def objCurrent(args):
   ctx.log(str(ctx.field("objCurrent").value)+" of "+str(ctx.field("objTotal").value) + " objects created")
   return
   
   
def ready():
   ctx.field("objCurrent").value = ctx.field("objTotal").value - ctx.field("Queue.numEntries").value
   ctx.field("Image.filename").value = ""
   ctx.field("ready").setBoolValue(True)
   #ctx.field("IdleTrigger.on").setBoolValue(False)          
   return
   

def ping(args):
   return
