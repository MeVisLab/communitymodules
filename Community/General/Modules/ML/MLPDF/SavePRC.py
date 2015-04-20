#----------------------------------------------------------------------------------
#! ML module SavePRC scripting
#/*!
# \file    SavePRC.py
# \author  Axel Newe
# \date    2014-08-30
#
# 
# */
#----------------------------------------------------------------------------------

from mevis import *

def InitModule():
  
  ctx.field("status").value = "Idle."
  ctx.field("progress").value = 0

  UpdateTabView()
  UpdateNewSpecification()
  
  return



def UpdateTabView():
  
  if ctx.field("newSpecificationType"). value == "SPECTYPE_POINTCLOUD":
    ctx.field("selectedTab").value = 0
  
  elif ctx.field("newSpecificationType"). value == "SPECTYPE_LINESET":
    ctx.field("selectedTab").value = 1

  elif ctx.field("newSpecificationType"). value == "SPECTYPE_MESH":
    ctx.field("selectedTab").value = 2

  elif ctx.field("newSpecificationType"). value == "SPECTYPE_METADATA":
    ctx.field("selectedTab").value = 3      
  
  return



def AddNewSpecification(callingField = None):

  if ctx.field("newSpecificationType"). value == "SPECTYPE_POINTCLOUD":
    ctx.field("pointCloudSpecification").value += ctx.field("newSpecification").value
  
  elif ctx.field("newSpecificationType"). value == "SPECTYPE_LINESET":
    ctx.field("lineSetSpecification").value += ctx.field("newSpecification").value

  elif ctx.field("newSpecificationType"). value == "SPECTYPE_MESH":
    ctx.field("meshSpecification").value += ctx.field("newSpecification").value

  elif ctx.field("newSpecificationType"). value == "SPECTYPE_METADATA":
    ctx.field("metaDataSpecification").value += ctx.field("newSpecification").value

  return
  
  
def UpdateNewSpecification(callingField = None):
   
  newSpecificationString = ""
  
  if (callingField == ctx.field("newSpecificationType")):
    UpdateTabView()
  
  if ctx.field("newSpecificationType"). value == "SPECTYPE_POINTCLOUD":
    newSpecificationString = "<PointSet>\n"
    newSpecificationString += GetPositionTypes()
    newSpecificationString += GetObjectName()
    newSpecificationString += GetGroupPath()
    newSpecificationString += GetModelVisibility()
    newSpecificationString += "\n"
    ctx.field("newSpecificationOutputValid").value = True 
  
  elif ctx.field("newSpecificationType"). value == "SPECTYPE_LINESET":
    newSpecificationString = "<LineSet>\n"
    newSpecificationString += GetPositionTypes()
    newSpecificationString += GetConnectionTypes()
    newSpecificationString += GetObjectName()
    newSpecificationString += GetGroupPath()
    newSpecificationString += GetColor()
    newSpecificationString += GetModelVisibility()
    newSpecificationString += "\n"
    ctx.field("selectedTab").value = 1
    ctx.field("newSpecificationOutputValid").value = True 

  elif ctx.field("newSpecificationType"). value == "SPECTYPE_MESH":
    newSpecificationString = "<Mesh>\n"
    newSpecificationString += GetWEMLabel()
    newSpecificationString += GetObjectName()
    newSpecificationString += GetGroupPath()
    newSpecificationString += GetColor()
    newSpecificationString += GetSpecularColor()
    newSpecificationString += GetModelVisibility()
    newSpecificationString += "\n"
    ctx.field("selectedTab").value = 2
    ctx.field("newSpecificationOutputValid").value = True 


  elif ctx.field("newSpecificationType"). value == "SPECTYPE_METADATA":
    newSpecificationString = "<MetaData>\n"
    newSpecificationString += GetMetaDataKey()
    newSpecificationString += GetMetaDataValue()
    newSpecificationString += "\n"
    ctx.field("selectedTab").value = 3
    ctx.field("newSpecificationOutputValid").value = True 

  ctx.field("newSpecification").value = newSpecificationString
  
  return


def GetMetaDataKey():
  
  metaDataKey = ctx.field("newSpecificationMetaDataKey").value
  
  if (metaDataKey == ""):
    metaDataKey = "[No key specified]"
  
  result = "<MetaDataKey>"
  result += metaDataKey
  result += "</MetaDataKey>\n"
  
  return result

def GetMetaDataValue():

  metaDataValue = ctx.field("newSpecificationMetaDataValue").value
  
  if (metaDataValue == ""):
    metaDataValue = "[No value specified]"
  
  result = "<MetaDataValue>"
  result += metaDataValue
  result += "</MetaDataValue>\n"
  
  return result

def GetWEMLabel():
  
  result = ""

  wemLabel = ctx.field("newSpecificationWEMLabel").value

  if (wemLabel != ""):

    result = "<WEMLabel>"
    result += wemLabel
    result += "</WEMLabel>\n"
  
  return result


def GetPositionTypes():
  
  result = ""

  positionTypes = ctx.field("newSpecificationPositionTypes").stringValue()

  if (positionTypes != ""):

    result = "<PositionTypes>"
    result += positionTypes
    result += "</PositionTypes>\n"
  
  return result


def GetConnectionTypes():
  
  result = ""

  connectionTypes = ctx.field("newSpecificationConnectionTypes").stringValue()

  if (connectionTypes != ""):

    result = "<ConnectionTypes>"
    result += connectionTypes
    result += "</ConnectionTypes>\n"
  
  return result


def GetObjectName():
  
  result = ""

  objectName = ctx.field("newSpecificationObjectName").value

  if (objectName != ""):

    result = "<ObjectName>"
    result += objectName
    result += "</ObjectName>\n"
  
  return result


def GetGroupPath():
  
  result = "" 

  groupPath = ctx.field("newSpecificationGroupPath").value

  if (groupPath != ""):

    groupPath = MLABFileManager.normalizePath(groupPath)

    if not groupPath.startswith("/"):
      groupPath = "/" + groupPath

    if not groupPath.endswith("/"):
      groupPath += "/"

    result = "<GroupPath>"
    result += groupPath  
    result += "</GroupPath>\n"
  
  return result


def GetColor():
  
  result = ""

  if not (ctx.field("newSpecificationUseDefaultColor").value):
    r, g, b = ctx.field("newSpecificationColor").value
    t       = ctx.field("newSpecificationColorAlpha").value
    
    result = "<Color>"
    result += "{:.3f} {:.3f} {:.3f} {:.3f}".format(r,g,b,t)
    result += "</Color>\n"
      
  return result


def GetSpecularColor():
  
  result = ""

  if not (ctx.field("newSpecificationUseDefaultSpecularColor").value):
    r, g, b = ctx.field("newSpecificationSpecularColor").value
    
    result = "<SpecularColor>"
    result += "{:.3f} {:.3f} {:.3f}".format(r,g,b)
    result += "</SpecularColor>\n"
  
  return result


def GetModelVisibility():
  
  result = ""

  modelVisibility = ctx.field("newSpecificationModelVisibility").value
  
  if (modelVisibility == "VISIBILITY_NOTVISIBLE"):
      modelVisibility = "0"
  elif (modelVisibility == "SPECTYPE_FRONTVISIBLE"):
      modelVisibility = "1"
  elif (modelVisibility == "SPECTYPE_BACKVISIBLE"):
      modelVisibility = "2"
  elif (modelVisibility == "SPECTYPE_FRONTANDBACKVISIBLE"):
      modelVisibility = "3"  

  if (modelVisibility != ""):

    result = "<ModelVisibility>"
    result += modelVisibility  
    result += "</ModelVisibility>\n"
  
  return result


