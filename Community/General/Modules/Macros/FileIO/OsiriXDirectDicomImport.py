# **InsertLicense** code

def init():
  if ctx.field("OsiriXBridge.alternativeTargetName").value == "":
    ctx.field("OsiriXBridge.alternativeTargetName").value = ctx.field("instanceName").value
