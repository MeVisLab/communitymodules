
from mevis import MLAB
from mevis import MLABFileManager
from mevis import MLABFileDialog

def fileDropped(filename):
  ctx.field("filename").setStringValue(ctx.unexpandFilename(filename))
  filenameChanged()
  
#----------------------------------------------------------------------------------------------------------------------------------------------------
def init(field=None):
  filename = ctx.field("filename").stringValue()
  if filename == "":
    ctx.field("filename").setStringValue("")
    ctx.field("AnalyzeHeader.filename").setStringValue("")

  filename = ctx.expandFilename(ctx.field("filename").stringValue())
  parseFile(filename)
    
#----------------------------------------------------------------------------------------------------------------------------------------------------
def filenameChanged(field=None):
  filename = ctx.expandFilename(ctx.field("filename").stringValue())
  if MLABFileManager.exists(filename):
    if MLABFileManager.isDir(filename):
      MLAB.logWarning("LocalAnalyzeLoad.filename - File is a directory (" + filename + ")")
    else:
      ctx.field("AnalyzeHeader.filename").setStringValue(filename)
  else:
    if not filename == "":
      MLAB.logWarning("LocalAnalyzeLoad.filename - File does not exist (" + filename + ")")
    ctx.field("AnalyzeHeader.filename").setStringValue("")
    
  parseFile(filename)

#----------------------------------------------------------------------------------------------------------------------------------------------------
def fileDialog(field=None):
  filename = ctx.expandFilename(ctx.field("filename").stringValue())
  filename = MLABFileDialog.getOpenFileName(filename,"Hdr/Img Files (*.hdr)","Open file")
  if not filename == "":
    ctx.field("filename").setStringValue(ctx.unexpandFilename(filename))
    
  parseFile(filename)
    
#----------------------------------------------------------------------------------------------------------------------------------------------------
def parseFile(filename):
  ctx.field("Bypass.input0").disconnect()
  ctx.field("ImgPropConvert.input0").disconnect()
  
  useScaling = ctx.field("useScaling").value
  constant = 1.0
  funused1 = ctx.field("AnalyzeHeader.funused1").value
  funused2 = ctx.field("AnalyzeHeader.funused2").value
  funused3 = ctx.field("AnalyzeHeader.funused3").value
  
  if not funused1 == 0.0:
    constant = funused1
  elif not funused2 == 0.0:
    constant = funused2
  elif not funused3 == 0.0:
    constant = funused3
    
  if constant > 1:
    constant = 1.0

  ctx.field("Arithmetic1.constant").value = constant    
      
  if constant == 1.0:
    useScaling = 0
    
  ctx.field("scaleFactor").value = constant
    
  if useScaling:
    ctx.field("ImgPropConvert.input0").connectFrom(ctx.field("ImgLoad.output0"))
    ctx.field("Bypass.input0").connectFrom(ctx.field("Arithmetic1.output0"))
  else:
    ctx.field("ImgPropConvert.input0").disconnect()
    ctx.field("Bypass.input0").connectFrom(ctx.field("ImgLoad.output0"))
  
  ctx.field("ImgLoad.filename").setStringValue(ctx.field("AnalyzeHeader.filename").stringValue())