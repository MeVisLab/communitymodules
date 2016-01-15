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
#! Macro module CompareModules
#!
# \file    CompareModules.script
# \author  Reinhard Hameeteman
# \date    2010-12-06
#
# Compare the field values of two modules
#
#----------------------------------------------------------------------------------

from mevis import *

def UpdateModuleList():
  network = ctx.parent()
  moduleList = list( network.modules() )
  ctx.field('moduleList').value = ''
  listString = ''
  for iModule in moduleList :
    listString += iModule + '@'
  listString += "Defaults"
  ctx.field('moduleList').value = listString
  return

def TestField(field):
  result = True
  if field :
    if field.type == 'Trigger' or field.name == "instanceName" :
      result = False 
  else:
    result = False
  return result


def UpdateSelections():
  UpdateModuleList()
  ctx.control("moduleList0").setCurrentText( ctx.field("module0").value )
  ctx.control("moduleList1").setCurrentText( ctx.field("module1").value )
  Compare()
  return

def Compare():
  selection0 = ctx.field("module0").value
  selection1 = ctx.field("module1").value
  selection0Default = (selection0 == "Defaults" )
  selection1Default = (selection1 == "Defaults" )
  if (selection0Default and selection1Default) : return
  module0 = None
  module1 = None
  if not selection0Default : module0 = ctx.parent().module( selection0 )
  if not selection1Default : module1 = ctx.parent().module( selection1 )
  if selection0Default and module1:
    module1Type = module1.type()
    if module1Type != "CompareModules" :
      module0 = ctx.addModule( module1Type )
  if selection1Default and module0:
    module0Type = module0.type()
    if module0Type != "CompareModules" :
      module1 = ctx.addModule( module0Type )

  parsString0 = "Parameter&Value@"
  parsString1 = "Parameter&Value@"
  if module0 and module1 :
    pars0 = module0.parameters()
    for par in pars0 :
      field0 = module0.field(par)
      field1 = None
      if module1.hasField( par ) :
        field1 = module1.field(par)
      if TestField(field0) and TestField(field1):
        value0 = field0.stringValue()
        value1 = field1.stringValue()
        pre = ""
        post = ""
        if value0 != value1 :
          pre = "<font color=#ff0000><b>"
          post = "</b></font>"
        parsString0 += par + "&" + pre + value0 + post + "@" 
        parsString1 += par + "&" + pre + value1 + post + "@"
  parsString0 = parsString0.strip("@")
  parsString1 = parsString1.strip("@")
  ctx.field("fields0").value = parsString0
  ctx.field("fields1").value = parsString1
  if selection0Default and module0: module0.remove()
  if selection1Default and module1: module1.remove()
  return 

def ResetToDefaults():
  selection0 = ctx.field("module0").value
  selection1 = ctx.field("module1").value
  selection0Default = (selection0 == "Defaults" )
  selection1Default = (selection1 == "Defaults" )
  if ( not( selection0Default ^ selection1Default)) : return
  module = None
  default = None
  if not selection0Default : module = ctx.parent().module( selection0 )
  if not selection1Default : module = ctx.parent().module( selection1 )
  if module :
    default = ctx.addModule( module.type() )
    if default :
      pars = module.parameters()
      for par in pars :
        if TestField( module.field(par) ) :
          module.field(par).setStringValue( default.field(par).stringValue() )
  Compare()
  return

#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBALMoAKeDufSkjPLfaCUd7Ij4IgEsndoDH9mP+jpEXKnAczgkSCgtNyNEMyiLur8xV1zEN7f71aeTOVWVntbzpucCARE=:lD+X/cPXp4xBkg/BH8EhyVWLOXzUCvL/ccrRKrYcyKMt2wR4QiXj1OCsqQukghRS1dwd5fRaB39vHgPZUYpdyA==
#//# owner: EMC
#//# date: 2010-03-02T15:48:44
#//# hash: coVMpF1OpLnh/Aq1AXmPuWl4n/kpEa2aLF1QJq5AOncdkqez6UhJXMrSPyy97eIAOYYnAAagI3NevWdPsmaM6w==
#//# MeVis end
