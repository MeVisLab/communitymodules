#----------------------------------------------------------------------------------
# 
# Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
# Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of BIGR nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------------
#! Macro module PythonExec
#/*!
# \file    PythonExec.py
# \author  Reinhard Hameeteman
# \date    2008-08-01
#
# Run a python exec command
#*/
#----------------------------------------------------------------------------------

from mevis import *
from math import log
width = 800
height = 200

def SaveWindowSize(arg=None):
  global width,height
  width = ctx.control("WindowCtrl").width()
  height= ctx.control("WindowCtrl").height()
  return

def SetWindowSize():
  global width,height
  #ctx.callLater(3, 'SetSize' )
  return

def SetSize():
  ctx.control("WindowCtrl").setWidth( width )
  ctx.control("WindowCtrl").setHeight( height )
  ctx.control("WindowCtrl").updateFrame()
  return

def Run( field = None):
  if ( field != ctx.field("run") and not ctx.field('onArgChange').boolValue() ) : return
  command = ctx.field("command").value
  arg1 = ctx.field("arg1").stringValue()
  arg2 = ctx.field("arg2").stringValue()
  arg3 = ctx.field("arg3").stringValue()
  command = command.replace("$1$", "\'" + arg1 + "\'")
  command = command.replace("$2$", "\'" + arg2 + "\'")
  command = command.replace("$3$", "\'" + arg3 + "\'")
  result1 = None 
  result2 = None 
  command = command.replace("%1%", "result1")
  command = command.replace("%2%", "result2")
  command = command.strip(' ')
  ctx.field("comString").value = command
  command += '\n'
  exec( command )

  if result1 != None : 
    ctx.field("result1").value = str(result1)
  else :
    ctx.field("result1").value = ""

  if result2 != None :
    ctx.field("result2").value = str(result2)
  else :
    ctx.field("result2").value = ""
  return

def FieldDroped( object ) :
  command  = ctx.field("command").value
  lastLine = command.split('\n')[-1]
  spaces = len(lastLine)-len(lastLine.lstrip())
  if ( ( object.owner().fullName().find( 'FieldIterator' ) != -1 or
         object.owner().fullName().find( 'FileIterator' ) != -1 ) and
       object.name == 'newValue' ) :
    moduleName = object.owner().name
    command += "\n"+spaces*" "+"for i"+moduleName+" in range( nw.field('" + moduleName+".numValues').value  ) :"
    command += "\n"+spaces*" "+"  if MLAB.shouldStop() : break"    
    command += "\n"+spaces*" "+"  nw.field('"+moduleName+".curIndex').value = i"+moduleName
    command += "\n"+spaces*" "+"  print nw.field('"+moduleName+".curValue').stringValue()"
  else: 
    if object.getType() == 'Trigger' :
      command +=  "\n"+spaces*" "+"nw.field('" + object.fullName() + "').touch()"
    else :
      command +=  "\n"+spaces*" "+"nw.field('" + object.fullName() + "').value"
  ctx.field("command").value = command
  return
