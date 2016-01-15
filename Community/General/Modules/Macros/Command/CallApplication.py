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
#! Macro module CallApplication
#/*!
# \file    CallApplication.py
# \author  Marijn van Stralen
# \date    2009-10-19
#
# Calls an application using a command line call
#*/
#----------------------------------------------------------------------------------

from mevis import *

import os.path
import os
import glob
import string

def processOut(p):
   m = p.readStdOut();
   ctx.field("logWindow").value += m;
   return

def processExit(p):
   MLAB.log("The following process has finished: " + p.userInfo() );
   return;

def clear():
  ctx.field("logWindow").value = "";
  return

def call():
  success = False;
  ctx.field("callSucceeded").value = success;
  prog = ctx.field("executable").value;
  args = str(ctx.field("arguments").value).split(" ");
  workingdir = ctx.field("workingDirectory").value;
  g_Process = MLAB.newProcess();
  #global g_Process;
  MLAB.processEvents(False);
  MLAB.log( prog + " is started with the following options:");
  MLAB.log(args);
  if g_Process:
    g_Process.kill();
  if g_Process:
    MLAB.deleteProcess( g_Process );
  g_Process = MLAB.newProcess();
  g_Process.clearArguments();
  g_Process.addArgument(prog);
  g_Process.addArguments(args);
  g_Process.setWorkDir(workingdir);
  if ctx.field("outputToLogWindow").value:
    g_Process.setStdOutHandler(ctx,"processOut");
    g_Process.setStdErrHandler(ctx,"processOut");
    g_Process.setExitedHandler(ctx,"processExit");
  g_Process.setUserInfo( prog );
  g_Process.run();
  g_Process.waitForLaunch();
  if g_Process.isRunning():
    MLAB.log(prog + " is running");
    success = True;
  else:
    MLAB.log(prog + " failed to start");
    success = False;
  MLAB.processEvents(True);
  if g_Process:
    g_Process.waitForExit();
  if g_Process:
    MLAB.deleteProcess( g_Process );
  ctx.field("callSucceeded").value = success;
  return ;

#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBANEfsmYse2e1dRhkQ9AQbreCq9uxwzWLoGom13MNYmyfwoJqQOEXljLFAgw2eEjaT12G4CdqKWhRxh9ANP6n7GMCARE=:VI/mB8bT4u+mRtf/ru8yUQi8BzpaS3UeL2x62YxsUYnVqCWuLrVNLiukIIjnJMKQXlc8ezmgOIcVAV7pgvgKpQ==
#//# owner: MeVis
#//# date: 2009-06-10T03:33:37
#//# hash: lks83XOD1rwLpts/0uROAT66Ocrko+7LhFbofqmeuxV1ZH30HY6fhGIHF+HZNbHAg04QggrC+0bSlk6r3GDj5Q==
#//# MeVis end

