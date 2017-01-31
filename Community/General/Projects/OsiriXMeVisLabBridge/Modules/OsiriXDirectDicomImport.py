# **InsertLicense** code

from mevis import MLAB
import os.path

def init():
  if ctx.field("OsiriXBridge.alternativeTargetName").value == "":
    ctx.field("OsiriXBridge.alternativeTargetName").value = ctx.field("instanceName").value

def showGuide():
  MLAB.openFile(os.path.join(ctx.package().documentationPath(),'Publish','OsiriXMeVisLabBridge.pdf'))
