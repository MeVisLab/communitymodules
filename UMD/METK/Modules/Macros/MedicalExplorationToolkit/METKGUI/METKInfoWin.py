# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info= None
_win = None


def init():
   global _cls_info      
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))      
   return


def initWindow(fieldP=0):
   #global _win
   #ctx.log("initShowWin")
   #_win = ctx.window("METKInfoWin")   
   return
   

def showWin(fieldP=0):
   global _win
   if (ctx.field("showWin").value==1):
      ctx.log("show win")      
      
      _win = ctx.showWindow("METKInfoWin")
      _win.center()

      ctx.field("infoHeader").value = "<html><font size='+2'><b>"+_cls_info.get(OBJ_GUI,LAY_INFOWIN,INF_INFOHEADER)+"</b></font></html>"
      ctx.field("infoText").value = _cls_info.get(OBJ_GUI,LAY_INFOWIN,INF_INFOTEXT)      
   else:      
      if _win:
         ctx.log("hide win")
         _win.hide()
   
   return



def checkVisibility(args):
   if (ctx.field("showWin").value==1):
      visible = _cls_info.get(OBJ_GUI, LAY_INFOWIN, INF_ISON)
      if (visible == "FALSE"):
         ctx.log("infoWin closed by timer")
         ctx.field("showWin").value=0
   return
   
   
   
   
   
#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():   
   handleAttributeModifiedEvent()
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

   #ctx.log("o:"+object+" "+layer+" "+info+" "+value+"#")
   
   if (object == OBJ_GUI):
      if (layer == LAY_INFOWIN):
         if (info == INF_ISON):
            if (value == "TRUE"):
               ctx.field("showWin").value = 1
            else:
               ctx.field("showWin").value = 0
         elif (info == INF_INFOHEADER):            
            ctx.field("infoHeader").value = "<html><font size='+2'><b>"+value+"</b></font></html>"            
         elif (info == INF_INFOTEXT):            
            ctx.field("infoText").value = value
   return