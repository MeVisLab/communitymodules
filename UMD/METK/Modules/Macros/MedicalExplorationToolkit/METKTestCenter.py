# Python module import
from ObjMgr import *
from VisDefinitions import *

def init():
   modules = MLAB.modules()
   ignoreList = ["METKVesselExplorer","METKManualResection","METKNPRShading","METKTestCenter","METKLabels","METKLogFile","METKGenerateVesselIV"]
   for module in modules:
      if module[:4] == "METK" and (module not in ignoreList):
         #ctx.field("Console.clear").touch()
         ctxmod = ctx.addModule(module)
         ctxmod.showWindow()
         ctxmod.removeUndoable()
   MLAB.processEvents()
   MLAB.exit(0)
   pass
