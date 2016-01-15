  
from mevis import MLAB
from mevis import MLABFileManager
import datetime

_processes = {}

#---------------------------------------------------------------------------------------------------
def download(field=None):
  sources = ctx.field("sources").stringValue().split("\n")
  target = ctx.field("target").stringValue()
  
  if not sources or not target:
    return
  
  ctx.field("transferring").value = 1
    
  if not target[-1] == "\\":
     target += "\\"
  target = MLABFileManager.nativePath(target)
     
  for source in sources:
    script  = _getScript()
    script += "get \"" + source + "\" \"" + target + "\"\n"
    script += "close\nexit\n"
    
    _executeScript(script)
  
#---------------------------------------------------------------------------------------------------
def upload(field=None):
  sources = ctx.field("sources").stringValue().split("\n")
  target = ctx.field("target").stringValue()
  
  if not sources or not target:
    return
    
  if not target[-1] == "/":
    target += "/"    
  
  winsources = []
  for source in sources:
    winsources.append(MLABFileManager.nativePath(source))
    
  script  = _getScript()    
  commands = ctx.field("preCommand").stringValue().split(";")
  for command in commands:
    if command:
      script += command + "\n"
  script += "put \"" + "\" \"".join(winsources) + "\" \"" + target + "\"\n"
  commands = ctx.field("postCommand").stringValue().split(";")
  for command in commands:
    if command:
      script += command + "\n"
  script += "close\nexit\n"
    
  _executeScript(script)

#---------------------------------------------------------------------------------------------------
def _getScript():
  hostname = ctx.field("hostname").stringValue()
  hostkey  = ctx.field("hostkey").stringValue()
  username = ctx.field("username").stringValue()
  password = ctx.field("password").stringValue()
  
  script  = "option batch on\noption confirm off\nopen "
  script += username + ":" + password + "@" + hostname
  script += " -hostkey=\"" + hostkey + "\"\n"
  script += "option transfer binary\n"
  return script
    
#---------------------------------------------------------------------------------------------------
def _executeScript(script):
  tmp = MLABFileManager.getTmpDir()
  if not tmp[-1] == "/":
    tmp += "/"
  now = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
  
  file = MLABFileManager.getUniqueFilename(tmp,now,".scp")
  MLABFileManager.writeStringToFile(file,script)
  
  winscp = ctx.expandFilename("$(LOCAL)/winscp416.exe")
  
  cmd = "\"" + winscp + "\" /script=\"" + file + "\""
  
  proc = MLAB.newProcess()
  proc.addArgument(cmd)
  proc.setStdOutHandler(ctx,"_procStdOut")
  proc.setStdErrHandler(ctx,"_procStdErr")
  proc.setExitedHandler(ctx,"_procFinished")
  
  global _processes
  _processes[proc] = file
  
  proc.run()
  
#---------------------------------------------------------------------------------------------------
def _procStdOut(proc):
  MLAB.log(proc.readStdOut())
  
#---------------------------------------------------------------------------------------------------
def _procStdErr(proc):
  MLAB.log(proc.readStdErr())
  
#---------------------------------------------------------------------------------------------------
def _procFinished(proc):
  global _processes  
  
  MLABFileManager.remove(_processes[proc])
  del _processes[proc]
  
  if len(_processes) == 0:
    ctx.field("transferring").value = 0
    ctx.field("finished").touch()
    
#---------------------------------------------------------------------------------------------------
def clean(field=None):
  MLABFileManager.recursiveRemoveDir(ctx.field("target").stringValue())