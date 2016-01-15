/*
// -----------------------------------------------------------------------
// 
// Copyright (c) 2001-2010, MeVis Medical Solutions AG, Bremen, Germany
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of MeVis Medical Solutions AG nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY MEVIS MEDICAL SOLUTIONS AG ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL MEVIS MEDICAL SOLUTIONS AG BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//----------------------------------------------------------------------------------
//! OrthoView2D
/*!
// \file    OrthoView2D.js
// \date    04/2005
*/
//----------------------------------------------------------------------------------

function inventorInputOnChanged(field)
{
  var flag = !field.boolValue();
  ctx.field("inInvPreLUT").setHidden(flag);
  ctx.field("inInvPostLUT").setHidden(flag);
  ctx.field("inInvPostViewer").setHidden(flag);
  ctx.updateLayout();
}

function toggleSynchPosition()
{    
  var synchPosOn = ctx.field("synchronizePosition").value; 
  if (synchPosOn){
    ctx.field("view.worldPosition").connectFrom(ctx.field("pos.worldPosition"));
  } else {
    ctx.field("view.worldPosition").disconnect();
  }  
}
//# MeVis signature v1
//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBANEfsmYse2e1dRhkQ9AQbreCq9uxwzWLoGom13MNYmyfwoJqQOEXljLFAgw2eEjaT12G4CdqKWhRxh9ANP6n7GMCARE=:VI/mB8bT4u+mRtf/ru8yUQi8BzpaS3UeL2x62YxsUYnVqCWuLrVNLiukIIjnJMKQXlc8ezmgOIcVAV7pgvgKpQ==
//# owner: MeVis
//# date: 2010-06-22T21:02:14
//# hash: UuHPkd9Wz80h3TyHswq8XUQA7AVUjgFRVfpFDDK1hecJryxvL8zGvve+d2aaRLZ9qlFdsxbH9BOeHTWRPip82w==
//# MeVis end
