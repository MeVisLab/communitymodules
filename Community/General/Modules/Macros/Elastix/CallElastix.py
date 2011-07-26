#----------------------------------------------------------------------------------
#
# Copyright (c) 2011, Image Sciences Institute, UMC Utrecht.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of ISI nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ISI BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------------
#! Macro module ISICallElastix
#/*!
# \file    ISICallElastix.py
# \author  Marijn van Stralen
# \date    2009-10-15
#
# Calls elastix using command line interface
#*/
#----------------------------------------------------------------------------------

from mevis import *

import os.path
import os
import glob
import string

g_Process = MLAB.newProcess();

def clearLog():
  ctx.field("SubProcessOutput").value = ""
  return

def testButton(*args):
   MLAB.log("testButton function has been triggered");
   return

def makeDir(arg):
   retcode = MLABFileManager.mkdir( arg );
   return retcode

def processOut(p):
   m = p.readStdOut();
   ctx.field("SubProcessOutput").value += m;
   return

def processExit(p):
   MLAB.log("The following process has finished: " + p.userInfo() );
   return;

def killSubProcess():
   global g_Process;
   if g_Process:
     MLAB.log( "The following process is killed: " + g_Process.userInfo() );
     g_Process.kill();
     if g_Process:
       MLAB.deleteProcess( g_Process );
   return

# prog: elastix executable (string)
# fi: fixed image file (string)
# mi: moving image file (string)
# fm: fixed mask file (string)
# mm: moving mask file (string)
# out: output directory (string)
# p1: parameter file 1 (string)
# p2: parameter file 2 (string)
# p3: parameter file 3 (string)
# arg: other program arguments (string)
def callElastix():
  prog = ctx.field("elastixEXE").value;
  prog2 = ctx.field("transformixEXE").value;
  fi = ctx.field("fixedImage").value;
  mi = ctx.field("movingImage").value;
  fm = ctx.field("fixedMask").value;
  mm = ctx.field("movingMask").value;
  checkTemporaryDirectory();
  out = ctx.field("outputDirectory").value;
  p1 = ctx.field("parameterFile1").value;
  p2 = ctx.field("parameterFile2").value;
  p3 = ctx.field("parameterFile3").value;
  threads = ctx.field("numberOfThreads").value;
  arg = ctx.field("otherArguments").value;
  t0 = ctx.field("initialTransformationFile").value;
  
  error = False;
  args = [];
  argstring = "";
  args.append("-f");
  args.append(fi);
  argstring += "-f " + fi;
  args.append("-m");
  args.append(mi);
  argstring += " -m " + mi;
  if len(fm) > 0:
    args.append("-fMask");
    args.append(fm);
    argstring += " -fMask " + fm;
  if len(mm) > 0:
    args.append("-mMask");
    args.append(mm);
    argstring += " -mMask " + mm;
  if len(out) > 0:
    args.append("-out");
    args.append(out);
    argstring += " -out " + out;
  else :
    ctx.field("status").value = "Error: no output directory";
    error = True;
  if (MLABFileManager.exists( p1 )):
    args.append("-p");
    args.append(p1);
    argstring += " -p " + p1;
  else :
    ctx.field("status").value = "Error: no parameter file";
    error = True;
  if (MLABFileManager.exists( p2 )):
    args.append("-p");
    args.append(p2);
    argstring += " -p " + p2;
  if (MLABFileManager.exists( p3 )):
    args.append("-p");
    args.append(p3);
    argstring += " -p " + p3;
  if threads!="Default":
    argstring += " -threads " + threads;
  if len(arg) > 0:
    items = arg.split(" ");
    args += items;
    argstring += " " + arg;
  if (MLABFileManager.exists( t0 )):
    args.append("-t0");
    args.append(t0);
    argstring += " -t0 " + t0;
  
  if not error :
    ctx.field("elastix.arguments").value = argstring;
    ctx.field("elastix.call").touch();
    
    # check success
    if checkElastixSuccess(out):
      print "elastix succeeded";
      getElastixResultingTransformationFile();
      if ctx.field("createDeformationImage").value | ctx.field("createJacobianImage").value:
        callTransformix(ctx.field("createDeformationImage").value, ctx.field("createJacobianImage").value, ctx.field("createJacobianMatrixImage").value);  
    else :
      print "elastix failed";
  else :
    ctx.field("succeeded").value = False;
  
  if not ctx.field("succeeded").value:
    ctx.field("resultingTransformationFile").value = "";    
    
  return

def callTransformix(deformation, jacobian, jacobianMatrix):
  ctx.field("transformix.workingDirectory").value = ctx.field("elastix.workingDirectory").value;
  argstring = "";
  argstring += "-in " + ctx.field("movingImage").value;
  argstring += " -out " + ctx.field("outputDirectory").value;
  if (deformation):
    argstring += " -def all";
  if (jacobian):
    argstring += " -jac all";
  if (jacobianMatrix):
    argstring += " -jacmat all";
  argstring += " -tp " ;
  param = getElastixResultingTransformationFile();
  argstring += param;
  ctx.field("transformParameterFile").value = param.replace("//", "/");
  threads = ctx.field("numberOfThreads").value;
  if threads!="Default":
    argstring += " -threads " + threads;
  
  ctx.field("transformix.arguments").value = argstring;
  ctx.field("transformix.call").touch();
  print "transformix done";
  return;
  
def checkElastixSuccess(tempdir):
  succ = False;  
  if not ctx.field("elastix.callSucceeded").value:
    ctx.field("succeeded").value = False;
    return False;
  with open( (ctx.field("outputDirectory").value + "elastix.log") , 'r') as f:
    ls = f.readlines();
    i = len(ls)-1;
    while(len(ls[i])==0 and i >= 0):
      i -= 1;
    print ls[i];
    if (str(ls[i]).find("Errors occurred!") >= 0):
      succ = False;
    else :
      succ = True;
  ctx.field("succeeded").value = succ;
  return succ;

def getElastixResultingTransformationFile():
  param = ctx.field("outputDirectory").value;
  if len(ctx.field("parameterFile3").value) > 0:
    param += "TransformParameters.2.txt";
  elif len(ctx.field("parameterFile2").value) > 0:
    param += "TransformParameters.1.txt";
  elif len(ctx.field("parameterFile1").value) > 0:
    param += "TransformParameters.0.txt";
  if not MLABFileManager.exists(param):
    param = "";
  ctx.field("resultingTransformationFile").value = param;
  return param;

def checkTemporaryDirectory():
  if str(ctx.field("outputDirectory").value).find(' ') != -1:
    print "Error: path contains spaces!";
  if (len(ctx.field("outputDirectory").value) > 0 and ctx.field("outputDirectory").value[-1] != '/'):
    ctx.field("outputDirectory").value += '/';

#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBANEfsmYse2e1dRhkQ9AQbreCq9uxwzWLoGom13MNYmyfwoJqQOEXljLFAgw2eEjaT12G4CdqKWhRxh9ANP6n7GMCARE=:VI/mB8bT4u+mRtf/ru8yUQi8BzpaS3UeL2x62YxsUYnVqCWuLrVNLiukIIjnJMKQXlc8ezmgOIcVAV7pgvgKpQ==
#//# owner: MeVis
#//# date: 2009-06-10T03:33:37
#//# hash: lks83XOD1rwLpts/0uROAT66Ocrko+7LhFbofqmeuxV1ZH30HY6fhGIHF+HZNbHAg04QggrC+0bSlk6r3GDj5Q==
#//# MeVis end

