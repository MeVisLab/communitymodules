# Python module import
from ObjMgr import *
from VisDefinitions import *


# Global class instances
_cls_info = None


def init():
   global _cls_info
   ctx.field("messageOnError").value = False
   ctx.field("MLLogFile.clear").touch()
   
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CASE, LAY_CASE)
   pass

def wakeUp():
   ctx.field("Console.clear").touch()   
   ctx.field("logLocation").touch()
   ctx.field("loggingMode").touch()
   pass


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   return

def handleObjectRemovedEvent():
   return

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return

def handleAttributeRemovedEvent():
   return

def handleAttributeModifiedEvent():
   if _cls_info.activeInfo() == INF_CASEDIR and ctx.field("logLocation").value == "CaseDir":
      ctx.field("logLocation").touch()
      ctx.field("loggingMode").touch()
   return
  
def handleLoadedEvent(field = 0):
   pass

def handleSaveEvent():
   pass
   
def handleCleanupEvent():
   ctx.field("messageOnError").value = False
   pass




def loggingMode(field = 0):
   if   field.value == "Demo":
      ctx.field("MLLogFile.enableLog").value = False
      ctx.field("CoreControl.traceOnFatals").value = False
      ctx.field("messageOnError").value = False
      
      ctx.field("Console.logWarnings").value = False
      ctx.field("Console.logErrors").value = False
      ctx.field("Console.logFatals").value = False
      ctx.field("Console.logDebugs").value = False
      ctx.field("Console.logCouts").value = False
      ctx.field("Console.logCerrs").value = False
      ctx.field("Console.logOthers").value = False
      
   elif field.value == "Normal":
      ctx.field("MLLogFile.enableLog").value = True
      ctx.field("CoreControl.traceOnFatals").value = True
      ctx.field("messageOnError").value = False
      
      ctx.field("Console.logWarnings").value = True
      ctx.field("Console.logErrors").value = True
      ctx.field("Console.logFatals").value = True
      ctx.field("Console.logDebugs").value = True
      ctx.field("Console.logCouts").value = True
      ctx.field("Console.logCerrs").value = True
      ctx.field("Console.logOthers").value = True
      
   elif field.value == "User":
      ctx.field("MLLogFile.enableLog").value = True
      ctx.field("CoreControl.traceOnFatals").value = False
      ctx.field("messageOnError").value = True
      
      ctx.field("Console.logWarnings").value = False
      
      ctx.field("Console.logErrors").value = True
      ctx.field("Console.logFatals").value = True
      
      ctx.field("Console.logDebugs").value = False
      ctx.field("Console.logCouts").value = False
      ctx.field("Console.logCerrs").value = False
      ctx.field("Console.logOthers").value = False
      
   elif field.value == "Test":
      ctx.field("MLLogFile.enableLog").value = True
      ctx.field("CoreControl.traceOnFatals").value = True
      ctx.field("messageOnError").value = False
      
      ctx.field("Console.logWarnings").value = True
      ctx.field("Console.logErrors").value = True
      ctx.field("Console.logFatals").value = True
      ctx.field("Console.logDebugs").value = True
      ctx.field("Console.logCouts").value = True
      ctx.field("Console.logCerrs").value = True
      ctx.field("Console.logOthers").value = True
      
   pass


def logLocation(field = 0):
   path = ""
   if ctx.field("logLocation").value == "UserHome":
      path = MLABFileManager.getHomeDir() + "/"
   elif ctx.field("logLocation").value == "CaseDir":
      path = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   elif ctx.field("logLocation").value == "UserDefined":
      path = ctx.field("userDefPath").value + "/"

   if path == "":
      ctx.field("MLLogFile.logFile").value = ""
   else:
      ctx.field("MLLogFile.logFile").value = path + ctx.field("userDefFileName").value
      
   ctx.field("trueFileName").value = path + ctx.field("userDefFileName").value
      
   if field.value == "CaseDir":
      ctx.field("MLLogFile.clear").touch()
   pass


def showWarning(field = 0):
   if field.value != "" and ctx.field("messageOnError").value == True:
      MLAB.showInformation (field.value, "Error")
      ctx.callLater(0, "clear")
   pass


def clear():
   ctx.field("Console.clear").touch()