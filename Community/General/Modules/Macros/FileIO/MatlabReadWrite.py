#----------------------------------------------------------------------------------
#! Macro module MatlabReadWrite
#/*!
# \file    MatlabReadWrite.py
# \author  Reinhard Hameeteman
# \date    2011-01-24
#
# conversion between a MeVisLab image and Matlab mat files
#*/
#----------------------------------------------------------------------------------

from mevis import *

types = {'Image':'0',
         'XMarkerList':'XMarkerList',
         'WEM':'WEM',
         'CurveList':'CurveList'}
script = "\
vars=whos(\'-file\',fileName);\n\
exists=false;\n\
foundNames = [];\n\
for i=1:size(vars,1)\n\
  if strcmp(vars(i).name, varName)\n\
    exists= true;\n\
  end\n\
  foundNames=[foundNames vars(i).name ','  ];\n\
end\n\
if exists \n\
  load( fileName );\n\
  <Output>= <Input>;\n\
else\n\
  result=[\'Variable not found; only found: \' foundNames];\n\
end\n\
"

def Save():
  global types,script
  type = types[ctx.field("type").value]
  ctx.field("MatlabScriptWrapper.matlabScript").value = "save(fileName,\'Input"+type+"\')"
  ctx.field("MatlabScriptWrapper.update").touch()
  return

def Load():
  global types, script
  ctx.field("MatlabScriptWrapper.scalar0").value = 1
  type = types[ctx.field("type").value]
  ctx.field("MatlabScriptWrapper.string1").value = type
  varName = ctx.field("variableName")
  if varName.value == "" :
    varName.value = "Input"+type
  ctx.field("MatlabScriptWrapper.string2").value = varName.value
  loadScript = script.replace('<Output>','Output'+type)
  loadScript = loadScript.replace('<Input>',varName.value)
  ctx.field("MatlabScriptWrapper.matlabScript").value = loadScript
  ctx.field("MatlabScriptWrapper.update").touch()
  result = ctx.field("MatlabScriptWrapper.string3")
  if result.value != "" :
    ctx.field("MatlabScriptWrapper.status").value = result.value.strip(',')
    result.value = ""
  return

  
#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBANEfsmYse2e1dRhkQ9AQbreCq9uxwzWLoGom13MNYmyfwoJqQOEXljLFAgw2eEjaT12G4CdqKWhRxh9ANP6n7GMCARE=:VI/mB8bT4u+mRtf/ru8yUQi8BzpaS3UeL2x62YxsUYnVqCWuLrVNLiukIIjnJMKQXlc8ezmgOIcVAV7pgvgKpQ==
#//# owner: MeVis
#//# date: 2010-06-22T21:02:14
#//# hash: WZ2Zrb7QlWYG9lWMjeYuCHRLvxmYYaZ6MQqO9Kg9XxPCBYK/DJHa2Qh3o2PxB1FLSyHsag8deKQO4CvxQgQMAA==
#//# MeVis end

