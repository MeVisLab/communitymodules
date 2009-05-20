

def selectOutputTab(field=None):
  if not ctx:
    return
    
  if ctx.field("outputTabSelected").value:
    ctx.field("selectedTab").value = 0
  updateTabs()

def updateTabs(field=None):
  if not ctx:
    return
    
  if ctx.hasField("selectedTab"):    
    selTab = ctx.field("selectedTab").value

    if ctx.hasField("copyInputWEMs") and ctx.hasField("useInputWEMToCreateOutputWEM"):
      if ctx.field("copyInputWEMs").value == 0:
        if not ctx.field("useInputWEMToCreateOutputWEM").value == 0:
          ctx.field("selectedTab").value = 0
          selTab = 0
          
    if ctx.hasField("outputTabSelected"):
      if selTab == 0:
        ctx.field("outputTabSelected").value = 1
      else:
        ctx.field("outputTabSelected").value = 0
  else:
    selTab = 0
    
  ctx.control("parametersTab").selectTabAtIndex(selTab)
