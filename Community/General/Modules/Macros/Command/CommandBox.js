//----------------------------------------------------------------------------------
//! Macro module CommandBox
/*!
// \file    CommandBox.js
// \author  Stefan Saur
// \date    2007-11-28
//
// 
*/
//----------------------------------------------------------------------------------

function processOnce()
{
	var commandList = ctx.field("commandList").value;
	var commands = new Array();
	var isVerbose = ctx.field("isVerbose").value;
	var isProcessEvents = ctx.field("isProcessEvents").value;
	var isProcessInventorQueue = ctx.field("isProcessInventorQueue").value;
	var sleep = ctx.field("sleep").value;
	commands = commandList.split('\n');
	for (var i=0; i<commands.length; i++)
	{
		var command = commands[i];
		if ( (command != '') && (command.charAt(0) != '#') ) {
			if (isVerbose) {
				MLAB.log("CommandBox ... " + command);
			}
			ctx.parent().field(command).touch();
			if (isProcessEvents) {
			  MLAB.processEvents();
			}
			if (isProcessInventorQueue) {
			  MLAB.processInventorQueue();
			}
			MLAB.msleep(sleep);
		}
	}
   
}

function apply()
{
   if (ctx.field("isLoop").value) {
     var start = ctx.field("startLoop").value;
     var stop = ctx.field("stopLoop").value;
     for (var i=start; i<=stop;i++) {
       ctx.field("val").setValue(i);
       processOnce();
     }
   }
   else {
     processOnce();
   }
	
}

//# MeVis signature v1
//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAKyWeOZdMOT9Sj40ZS32A8i8GAjHkcHh3QT05EJBoHkqE3S0/FOcZM6SFkMLVcjiDmzBdoc9REZUIl5061137BkCARE=:mbUARszcrJxTM2e3H2YskVHojl15mYg4OBjlYsSeJfjTYEVwkgvJJ0eFJB4r7/u64QtiMUxLsor5/MgAGO4iCA==
//# owner: MeVis Internal Developer
//# date: 2007-07-05T03:39:16
//# hash: Tzy7AGzs3RTVPEX8HYNWb67yw48RjE3KzJBfPjHXAcvDneKgDMcD//dT2Tvl6AZmx/gsbhkHVMhWl3KW+V/IXA==
//# MeVis end

