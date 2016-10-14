#/----------------------------------------------------------------------------------
#/! Macro module CommandBox
#*!
#/ \file    CommandBox.py
#/ \author  Alexander Broersen
#/ \date    2016-10-14
#/
#/
#*/
#/----------------------------------------------------------------------------------

from mevis import MLAB

def processOnce():
   commandList = ctx.field("commandList").value;
   isVerbose = ctx.field("isVerbose").value;
   isProcessEvents = ctx.field("isProcessEvents").value;
   isProcessInventorQueue = ctx.field("isProcessInventorQueue").value;
   sleep = ctx.field("sleep").value;
   commands = commandList.split('\n')
   for i in range(0, len(commands)):
     command = commands[i]
     if ( (command != '') and (command[0] != '#') ):
       if (isVerbose):
         MLAB.log("CommandBox ... " + command);
       ctx.parent().field(command).touch();
       if (isProcessEvents):
         MLAB.processEvents();
       if (isProcessInventorQueue):
         MLAB.processInventorQueue();
       MLAB.msleep(sleep);
     
   


def apply(field):
   if ctx.field("isLoop").value:
     start = ctx.field("startLoop").value;
     stop = ctx.field("stopLoop").value;
     for i in range(start, stop+1):
       ctx.field("val").setValue(i);
       processOnce();
   else:
     processOnce();
   