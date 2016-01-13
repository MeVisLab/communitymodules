#//----------------------------------------------------------------------------------
#//! Macro module CaseScanner
#/*!
#// \file    CaseScanner.py    
#// \author  Maik
#// \date    2007-01-05
#//
#// // 
#*/
#//----------------------------------------------------------------------------------

from ObjMgr import *
from xml.dom.minidom import *
from string import *

dataBaseAccess = None

def init():
    #creates object access to the objManager
    global dataBaseAccess
    dataBaseAccess = ObjInfo(ctx.module("Info"))
    if dataBaseAccess.existObject("Dimensions") == False:
        dataBaseAccess.createObject("Dimensions")
    if dataBaseAccess.existObject("Cases") == False:
        dataBaseAccess.createObject("Cases")
    #for testing
    #dirName = "H:/Studium/Maik/Diplom/Mevis/mamewes/MLabModules/Projects/LiverSurgeryTrainer/"
    #    dirName = ""
    #if dirName == "" :
    #    print "no directory, use browse trigger to start scan"
    #else:
    #    print "dirName: " + dirName + "\nscanning"     
    #    scan ()
    return

def browseForDirectory(field):
    browseForDir()

def browseForDir():
    dirName = MLABFileDialog.getExistingDirectory(ctx.localPath(), "Select a Directory", True)
    if dirName == "" : return
    #print "Directory: " + dirName
    ctx.field("dirName").setStringValue(dirName)
    scan()
    return

def scanTrigger(field):
  scan()
  return 

def scan():
    dirName = ctx.field("dirName").stringValue()
    #print "Now scanning directory for cases...:" + dirName
    stringlist = MLABFileManager.recursiveContentOfDirectory(dirName, "casedata.xml")
    length = str(len(stringlist))
    #print "Found " + length + " files named casedata.xml"
    if len == 0:
        return
    #print "Iterating list..."
    for xmlFile in stringlist :
        #print dirName + "/" + xmlFile
        domDoc = parse(dirName + "/" + xmlFile)
        nodeList = domDoc.documentElement.getElementsByTagName("*")
        #print "searching for parameters in document..."
        activeCaseID = ""        
        #first we need the case ID
        for element in nodeList :
            if element.getAttribute("id") == "CaseParameter":
                parameters = element.getElementsByTagName("*")
                for layer in parameters:
                    if layer.getAttribute("id") == "Parameter":
                        values = layer.getElementsByTagName("*")
                        for val in values:
                            name = val.getAttribute("id")
                            if name == "CaseID":
                              activeCaseID = val.getAttribute("value")                               
        for element in nodeList :     
            if element.getAttribute("id") == "CaseParameter" or element.getAttribute("id") == "PatientData":
                #print "found Parameters"
                parameters = element.getElementsByTagName("*")
                for layer in parameters :
                    if layer.getAttribute("id") == "Parameter":
                        values = layer.getElementsByTagName("*")
                        for val in values :
                            name = val.getAttribute("id")
                            if activeCaseID != "" :
                                dataBaseAccess.activateObject("Cases")
                                dataBaseAccess.activateLayer(activeCaseID)
                                dataBaseAccess.activateInfo(name)
                                if name == "Difficulty":
                                  diffvalue = val.getAttribute("value")
                                  if diffvalue == "low":
                                    dataBaseAccess.setString("1 (" + diffvalue + ")")
                                  if diffvalue == "medium":
                                    dataBaseAccess.setString("2 (" + diffvalue + ")")
                                  if diffvalue == "high":
                                    dataBaseAccess.setString("3 (" + diffvalue + ")")
                                else:
                                  dataBaseAccess.setString(val.getAttribute("value"))
                            dataBaseAccess.activateObject("Dimensions")
                            dataBaseAccess.activateLayer(name)
                            difficulty = False 
                            if name == "Difficulty":
                              difficulty = True 
                            name = val.getAttribute("type")
                            #the type: string, bool, integer number or floating point number
                            name = retrieveType(name)
                            dataBaseAccess.activateInfo("type")
                            dataBaseAccess.setString(name)                            
                            v = val.getAttribute("value")
                            if difficulty:
                              if v == "low":
                                v = "1 (" + v + ")"
                              if v == "medium":
                                v = "2 (" + diffvalue + ")"
                              if v == "high":
                                v = "3 (" + diffvalue + ")"
                            #ctx.log("activating min")
                            dataBaseAccess.activateInfo("min")
                            #string -> split values list and add to list if it is not in
                            if name == "string" :
                                setString(v)
                                #bool
                            elif name == "bool" :
                               dataBaseAccess.setBool(False)
                                #number types
                            else:
                                setNumberMin(v)
                            #ctx.log("activating max")
                            dataBaseAccess.activateInfo("max")
                            if name == "string" :
                                setString(v)
                            #bool
                            elif name == "bool" :
                                dataBaseAccess.setBool(True)
                            #number types
                            else:
                                setNumberMax(v)                         
                            #ctx.log("activating step")
                            dataBaseAccess.activateInfo("step")
                            if name == "string" :
                                dataBaseAccess.setString("-1")
                            #bool
                            elif name == "bool" :
                                dataBaseAccess.setString("-1")                       
                            #number types
                            else:
                                setNumberStep(name)                                                         
                #ctx.log("CaseScanner:  found Objects --> notifying")
                dataBaseAccess.notify();                                        
        domDoc.unlink()
    return

def retrieveType(name):
    name.lower()
    if str(name).find("string") != -1:
        name = "string"
    elif str(name).find("int") != -1:
        name = "int"
    elif str(name).find("double") != -1:
        name = "double"
    elif str(name).find("bool") != -1:
        name = "bool"
    return name

def setString(v):
    theInfoString = dataBaseAccess.getString()
    availableValues = ""
    if theInfoString == None : 
        theInfoString = ""
    availableValues = split(theInfoString, "|")
    found = False
    for availableValue in availableValues:
        if availableValue == v:
            found = True
    if found == False:
        dataBaseAccess.setString(join(availableValues + [v], "|"))

def setNumberMin(value):
   theInfoValue = dataBaseAccess.getString()
   if theInfoValue == "":
       dataBaseAccess.setString(value)
       return
   if atof(value) < atof(theInfoValue):
       dataBaseAccess.setString(value)

def setNumberMax(value):
   theInfoValue = dataBaseAccess.getString()
   if theInfoValue == "":
       dataBaseAccess.setString(value)
       return
   if atof(value) > atof(theInfoValue):
       dataBaseAccess.setString(value)

def setNumberStep(type):
    dataBaseAccess.activateInfo("min")
    min = dataBaseAccess.getString()
    dataBaseAccess.activateInfo("max")
    max = dataBaseAccess.getString()
    #todo determinStepWidth
    dataBaseAccess.activateInfo("step")
    dataBaseAccess.setString("1")     
        
#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAKyWeOZdMOT9Sj40ZS32A8i8GAjHkcHh3QT05EJBoHkqE3S0/FOcZM6SFkMLVcjiDmzBdoc9REZUIl5061137BkCARE=:mbUARszcrJxTM2e3H2YskVHojl15mYg4OBjlYsSeJfjTYEVwkgvJJ0eFJB4r7/u64QtiMUxLsor5/MgAGO4iCA==
#//# owner: MeVis Internal Developer
#//# date: 2006-06-14T10:26:53
#//# hash: GlkdWCfY0toZv5evUL9ociXACj18IR1NloBzc4Dof+Z5wo6iQmamtuD7PM/7pEgdva2MlExiI1IFpc1aIokrhg==
#//# MeVis end
