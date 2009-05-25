
from mevis import *
from time import *
from thread import *
from threading import *

_modList = ()
_listenerList = ()
_notificationDict = {}
_outListCtr = None
_inListCtr = None
_currentSequenceID = 0
_updateViewTimer = None
_lastFieldListenerTime = clock()
_lastFieldListenerName = ""
_ownOutFieldName = ""

def setControlPtr():
  global _outListCtr  
  _outListCtr = ctx.control("outputlist")
  #_inListCtr = ctx.control("inFieldlist")
  #_inListCtr.setRootIsDecorated(True)
  
def resetVariables():
  global _modList, _currentSequenceID
  global _updateViewTimer
  global _lastFieldListenerTime, _lastFieldListenerName
  _lastFieldListenerTime = clock()
  _currentSequenceID = 0
  _modList = () 
  clearFieldListeners()
  _notificationDict.clear()
  _updateViewTimer = Timer(2.0,updateListview)
  _lastFieldListenerTime = clock()
  _lastFieldListenerName = ""

def fieldNotified(arg=None):
  global _lastFieldListenerTime, _lastFieldListenerName  
  global _currentSequenceID  
  global _updateViewTimer
  global _ownOutFieldName
  
  notificationDict = _notificationDict
  
  ownerMod = arg.owner()
  completeFieldName = arg.getName()
  _currentSequenceID += 1
  while ownerMod.parent() != ctx.parent():    
    completeFieldName = ownerMod.name + "." + completeFieldName
    ownerMod = ownerMod.parent()
  completeFieldName = ownerMod.name +"."+ completeFieldName
  
  if completeFieldName in notificationDict:    
    notificationDict[completeFieldName][0] = notificationDict[completeFieldName][0] + 1
  else:    
    idStr = "%(currID)06d" % {"currID":_currentSequenceID}
    notificationDict[completeFieldName] = [1,idStr,0.0]
  if ctx.field("logToConsole").value == True:
    ctx.log(completeFieldName)
   
   
  # time measurements
  currentTime = clock()
  deltaTime = currentTime - _lastFieldListenerTime
  #ctx.log(_lastFieldListenerName + "|" + completeFieldName + "|" + str(currentTime) + "|" + str(deltaTime))
  _lastFieldListenerTime = currentTime   
  if _lastFieldListenerName != "" and _lastFieldListenerName != _ownOutFieldName :
    if completeFieldName in notificationDict:
      notificationDict[completeFieldName][2] = notificationDict[completeFieldName][2]+deltaTime
    else:
      ctx.log("Error: completeFieldName not in _notificationDict")    
  elif _lastFieldListenerName == "":
    _ownOutFieldName = completeFieldName
      
  #_updateViewTimer.cancel()
  #_updateViewTimer = Timer(2.0,updateListview)
  #_updateViewTimer.start()*/
  if ctx.field("autoUpdateListview").value == True:
    updateListview()
  _lastFieldListenerName = completeFieldName
  
  return
  
def clearFieldListeners():
  global _listenerList
  for i in _listenerList:    
    ctx.removeFieldListener(i)
  _listenerList = ()  
  return  

def addFieldListenerForModuleFields(parentMod,modName,fieldType):
  global _listenerList
  if fieldType == "output":
    fields = parentMod.module(modName).outputs()
  elif fieldType == "parameter":
    fields = parentMod.module(modName).parameters()
  for name in fields:
    fieldName = modName+"."+name       
    _listenerList += tuple([ctx.addFieldListener(parentMod.field(fieldName),"fieldNotified",False)])
  return

def startAnalysis():
  global _modList
  resetVariables()
  parentMod = ctx.parent()
  collectModules("",parentMod)
  #ctx.log(_modList)
  
  for i in _modList:
    addFieldListenerForModuleFields(parentMod,i,"output")
    if ctx.field("analyzeParameterFieldsAlso").value == True:
      addFieldListenerForModuleFields(parentMod,i,"parameter")
  updateListview()
  return
  
def collectModules(namePrefix,mod):
  global _modList
  modList = mod.modules()
  recursiveAnalysis = ctx.field("analyseMacrosRecursively").value
  for i in modList: 
    _modList += tuple([namePrefix+i])        
    nextMod = mod.module(i)
    if recursiveAnalysis == True and nextMod != None and nextMod.modules() != ():
      collectModules(namePrefix+i+".",nextMod)  
  return  





def updateListenedFields():
  global _modList
  resetVariables()
  parentMod = ctx.parent()
  collectModules("",parentMod)
  #ctx.log(_modList)
  
  #for i in _modList:  
  #
  return  
  
# --------------- Output ---------------------

def updateListview(): 
  global _outListCtr
  
  outListCtr_insertItem = _outListCtr.insertItem
  
  notificationDict = _notificationDict
  
  _outListCtr.clearItems()
  for i in notificationDict:
    numNotifies = notificationDict[i][0]
    numNotifiesString = "%(numNotifies)06d" % {"numNotifies":numNotifies}
    sequenceNumber = notificationDict[i][1]
    deltaT = notificationDict[i][2] 
    deltaTString = "%(deltaT)08.4f" % {"deltaT":deltaT}
    averageT = deltaT/numNotifies
    averageTString = "%(averageT)08.4f" % {"averageT":averageT}
    outListCtr_insertItem((i,numNotifiesString,sequenceNumber,deltaTString,averageTString))
  return
  
